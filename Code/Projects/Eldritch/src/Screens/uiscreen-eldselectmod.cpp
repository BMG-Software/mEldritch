#include "core.h"
#include "Widgets/uiwidget-text.h"
#include "Widgets/uiwidget-frame.h"
#include "Widgets/uiwidget-image.h"
#include "Widgets/uiwidget-list.h"
#include "uiscreen-eldselectmod.h"
#include "configmanager.h"
#include "packstream.h"
#include "modmanager.h"
#include "wbeventmanager.h"
#include "wbworld.h"
#include "eldritchgame.h"
#include "eldritchframework.h"

UIScreenEldSelectMod::UIScreenEldSelectMod()
:	m_Selector( NULL )
,	m_Image( NULL )
,	m_Name( NULL )
,	m_Version( NULL )
,	m_Author( NULL )
,	m_Description( NULL )
{
}

UIScreenEldSelectMod::~UIScreenEldSelectMod()
{
}

void UIScreenEldSelectMod::InitializeModlist()
{
	const Array< SModDescription >& FullList = ModManager::GetInstance()->GetMods();

	for ( uint ModIter = 0; ModIter < FullList.Size(); ++ModIter )
	{
		if ( FullList[ ModIter ].m_InitialLevel != "" )
		{
			m_Selector->AddEntry( FullList[ ModIter ].m_Name );
		}
	}
}

void UIScreenEldSelectMod::ShowMod( const SimpleString& TheMod )
{
	if ( TheMod != "" )
	{
		const SModDescription *pModDescription = ModManager::GetInstance()->FindMod( TheMod );
		if ( pModDescription )
		{
			if ( pModDescription->m_Image != "" )
			{
				m_Image->SetTexture( pModDescription->m_Image.CStr() );
			}
			else
			{
				m_Image->SetTexture( "Textures/UI/mod-image-default_NODXT.tga" );
			}
			m_Image->UpdateRender();

			SimpleString Buffer;

			m_Name->SetString( pModDescription->m_Name );

			const char* Version = ConfigManager::GetLocalizedString( "ModVersion", "" );
			Buffer = SimpleString::PrintF( "%s: %s", Version, pModDescription->m_Version.CStr() );
			m_Version->SetString( Buffer.CStr() );
			const char* Author = ConfigManager::GetLocalizedString( "ModAuthor", "" );
			Buffer = SimpleString::PrintF( "%s: %s", Author, pModDescription->m_Author.CStr() );
			m_Author->SetString( Buffer.CStr() );
			const char* Description = ConfigManager::GetLocalizedString( pModDescription->m_Description.CStr(), pModDescription->m_Description.CStr() );
			m_Description->SetString( Description );

			return;
		}
	}
	m_Image->SetTexture( "Textures/UI/mod-image-default_NODXT.tga" );
	m_Image->UpdateRender();
	m_Name->SetString( "" );
	m_Version->SetString( "" );
	m_Author->SetString( "" );
	m_Description->SetString( "" );
}

void UIScreenEldSelectMod::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	UIScreen::InitializeFromDefinition( DefinitionName );

	m_Selector = static_cast< UIWidgetList* >( m_RenderWidgets[2] );
	m_Image = static_cast< UIWidgetImage* >( m_RenderWidgets[3] );
	m_Name = static_cast< UIWidgetText* >( m_RenderWidgets[4] );
	m_Version = static_cast< UIWidgetText* >( m_RenderWidgets[5] );
	m_Author = static_cast< UIWidgetText* >( m_RenderWidgets[6] );
	m_Description = static_cast< UIWidgetText* >( m_RenderWidgets[7] );
	UIWidgetText *pOk = static_cast< UIWidgetText* >( m_RenderWidgets[8] );
	UIWidgetText *pCancel = static_cast< UIWidgetText* >( m_RenderWidgets[9] );

	STATIC_HASHED_STRING( Pop );
	const SimpleString	UnusedCommand;
	const SUICallback UnusedCallback;

	m_Selector->m_Callback = SUICallback( ShowModCallback, this );

	pOk->m_EventName	= sPop;
	pOk->m_Command		= UnusedCommand;
	pOk->m_Callback		= SUICallback( SelectModCallback, this );

	pCancel->m_EventName= sPop;
	pCancel->m_Command	= UnusedCommand;
	pCancel->m_Callback = UnusedCallback;

	InitializeModlist();

	ShowMod();

	UpdateRender();
}

void UIScreenEldSelectMod::RequestGoToLevel( const SimpleString& LevelName )
{
	MAKEHASH( LevelName );
	STATICHASH( Name );
	STATICHASH( WorldDef );

	const char* WorldDef = ConfigManager::GetString( sWorldDef, NULL, sLevelName );

	if ( WorldDef )
	{
		EldritchGame* const	pGame			= EldritchFramework::GetInstance()->GetGame();
		WBEventManager* const pEventManager	= WBWorld::GetInstance()->GetEventManager();

		WB_MAKE_EVENT( GoToLevel, NULL );
		WB_LOG_EVENT( GoToLevel );
		WB_SET_AUTO( GoToLevel, Hash, Level, sLevelName );
		WB_SET_AUTO( GoToLevel, Hash, WorldDef, HashedString( WorldDef ) );
		WB_DISPATCH_EVENT( pEventManager, GoToLevel, pGame );
	}
	else
	{
		PRINTF( "Failed to start level %s\n", LevelName.CStr() );
	}
}

void UIScreenEldSelectMod::SelectModCallback(void* pWidget, void* pData)
{
	UIScreenEldSelectMod* pUIScreenEldSelectMod = static_cast< UIScreenEldSelectMod* >( pData );

	SimpleString ModName = pUIScreenEldSelectMod->m_Name->GetString();
	if ( ModName != "" )
	{
		const SModDescription *pModDescription = ModManager::GetInstance()->FindMod( ModName );
		if ( ( ModName != "" ) && ( pModDescription->m_InitialLevel != "" ) )
		{
			pUIScreenEldSelectMod->RequestGoToLevel( pModDescription->m_InitialLevel );
		}
	}
}

void UIScreenEldSelectMod::ShowModCallback(void* pWidget, void* pData)
{
	UIWidgetList *pUIWidgetList = static_cast< UIWidgetList* >( pWidget );
	UIScreenEldSelectMod* pUIScreenEldSelectMod = static_cast< UIScreenEldSelectMod* >( pData );

	SimpleString ModName = pUIWidgetList->GetString();
	if ( ModName != "" )
	{
		pUIScreenEldSelectMod->ShowMod( ModName );
	}
}
