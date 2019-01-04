#include "core.h"
#include "Widgets/uiwidget-text.h"
#include "Widgets/uiwidget-frame.h"
#include "Widgets/uiwidget-input.h"
#include "Widgets/uiwidget-list.h"
#include "uiscreen-selectfiledialog.h"
#include "configmanager.h"
#include "hashedstring.h" // DEBUG
#include "file.h"

UIScreenSelectFileDialog::UIScreenSelectFileDialog()
:	m_Filename()
,	m_OkCallback()
,	m_Path()
,	m_File()
{
	InitializeFromDefinition( "SelectFileDialog" );
}

UIScreenSelectFileDialog::~UIScreenSelectFileDialog()
{
}

void UIScreenSelectFileDialog::SetParameters(
	bool				PauseGame,
	const SimpleString& TitleString,
	const SimpleString&	TitleDynamicString,
	const SimpleString& Path,
	const SimpleString&	Pattern,
	const HashedString&	OkEvent,
	const SimpleString&	OkCommand,
	const SUICallback&	OkCallback
	)
{
	m_PausesGame = PauseGame;
	m_SeizesMouse = PauseGame;

	// HACKY: Referencing widgets directly by index
	UIWidgetText* pTitle = static_cast< UIWidgetText* >( m_RenderWidgets[1] );
	m_Path = static_cast< UIWidgetText* >( m_RenderWidgets[2] );
	m_File = static_cast< UIWidgetInput* >( m_RenderWidgets[3] );
	UIWidgetText* pOk = static_cast< UIWidgetText* >( m_RenderWidgets[4] );
	UIWidgetText* pCancel = static_cast< UIWidgetText* >( m_RenderWidgets[5] );
	m_Selector = static_cast< UIWidgetList* >( m_RenderWidgets[6] );
	UIWidgetText* pPathLabel = static_cast< UIWidgetText* >( m_RenderWidgets[7] );
	UIWidgetText* pFileLabel = static_cast< UIWidgetText* >( m_RenderWidgets[8] );

	pTitle->m_String = ConfigManager::GetLocalizedString( TitleString, TitleDynamicString.CStr() );
	pTitle->InitializeFromDefinition( pTitle->m_Name );

	m_File->SetString(""); // TODO temp
	m_File->InitializeFromDefinition( m_File->m_Name );
	m_File->m_EventName	= OkEvent;
	m_File->m_Command	= OkCommand;
	m_File->m_Callback	= SUICallback(SelectFileDialogOkCallback, this);

	pOk->m_EventName	= OkEvent;
	pOk->m_Command		= OkCommand;
	pOk->m_Callback		= SUICallback(SelectFileDialogOkCallback, this);

	STATIC_HASHED_STRING( Pop );
	const SimpleString	UnusedCommand;
	const SUICallback UnusedCallback;

	pCancel->m_EventName= sPop;
	pCancel->m_Command	= UnusedCommand;
	pCancel->m_Callback = UnusedCallback;

	//pPathLabel->InitializeFromDefinition( pPathLabel->m_Name );
	//pFileLabel->InitializeFromDefinition( pFileLabel->m_Name );

	m_Selector->m_Callback = SUICallback( SelectFileDialogSetPathCallback, this );

	m_OkCallback = OkCallback;

	m_Pattern = Pattern;
	SetPath( Path );

}

// slightly modified Array.InsertionSort
static void SortEntries( Array< SimpleString >& Entries )
{
	for( uint Index = 0; Index < Entries.Size(); ++Index )
	{
		const SimpleString IterValue = Entries[ Index ];

		uint Hole = Index;
		while( Hole > 0 && IterValue.StrILt( Entries[ Hole - 1 ] ) )
		{
			Entries[ Hole ] = Entries[ Hole - 1 ];
			Hole = Hole - 1;
		}

		Entries[ Hole ] = IterValue;
	}
}

void UIScreenSelectFileDialog::SetPath( const SimpleString& Path )
{
	SimpleString ThePath = FileUtil::Normalize( Path.CStr() );

	// TODO remove trailing slashes
	Array< SimpleString > Folders;
	Array< SimpleString > Files;

	FileUtil::GetAllFolders( ThePath.CStr(), Folders, false );
	SortEntries( Folders );
	FileUtil::GetAllFiles( ThePath.CStr(), m_Pattern.CStr(), Files );
	SortEntries( Files );

	m_Selector->ClearEntries();

	if ( ThePath != "/" )
	{
		m_Selector->AddEntry( "../" );
	}
	FOR_EACH_ARRAY( Folder, Folders, SimpleString )
	{
		m_Selector->AddEntry( ( (*Folder) + "/" ).CStr() );
	}

	FOR_EACH_ARRAY( File, Files, SimpleString )
	{
		m_Selector->AddEntry( (*File).CStr() );
	}

	m_Path->m_String = ThePath;
	m_Path->UpdateRender();

	m_File->SetString( "" );
	m_File->UpdateRender();

	m_Selector->UpdateRender();
}

void UIScreenSelectFileDialog::SetFile( const SimpleString& File )
{
	SimpleString TheFile = FileUtil::Normalize( File.CStr() );;
	if ( FileUtil::IsAbsolutePath( TheFile.CStr() ) )
	{
		SetPath( FileUtil::StripFilename( TheFile.CStr() ) );
		TheFile = SimpleString( FileUtil::StripLeadingFolders( TheFile.CStr() ) );
	}
	m_File->SetString( TheFile );
	m_File->UpdateRender();
}

void UIScreenSelectFileDialog::SelectFileDialogOkCallback(void* pWidget, void* pData)
{
	UIScreenSelectFileDialog* pUIScreenSelectFileDialog = static_cast< UIScreenSelectFileDialog* >( pData );
	pUIScreenSelectFileDialog->CallOkCallback();
}

// The OK callback is called with a string instead of a widget
void UIScreenSelectFileDialog::CallOkCallback()
{
	if ( m_OkCallback.m_Callback && m_File->GetString() != "" )
	{
		m_Filename = m_Path->m_String + "/" + m_File->GetString();
		m_OkCallback.m_Callback( (void*)( m_Filename.CStr() ), m_OkCallback.m_Void );
	}
}

void UIScreenSelectFileDialog::SelectFileDialogSetPathCallback(void* pWidget, void* pData)
{
	UIWidgetList *pUIWidgetList = static_cast< UIWidgetList* >( pWidget );
	UIScreenSelectFileDialog* pUIScreenSelectFileDialog = static_cast< UIScreenSelectFileDialog* >( pData );
#if BUILD_WINDOWS
	SimpleString NewPath( "" );
	if ( pUIScreenSelectFileDialog->m_Path->GetString() != "/" )
	{
		NewPath = pUIScreenSelectFileDialog->m_Path->GetString() + "/";
	}
	NewPath += pUIWidgetList->GetString();
#else
	SimpleString NewPath = pUIScreenSelectFileDialog->m_Path->GetString() + "/" + pUIWidgetList->GetString();
#endif


	char LastChar = NewPath.GetChar( NewPath.Length() - 1 );
	if ( LastChar == '/' || LastChar == '\\' )
	{
		pUIScreenSelectFileDialog->SetPath( NewPath );
	}
	else
	{
		pUIScreenSelectFileDialog->SetFile( NewPath );
	}
}
