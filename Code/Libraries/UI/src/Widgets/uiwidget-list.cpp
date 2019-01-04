#include "core.h"
#include "uiwidget-list.h"
#include "uimanager.h"
#include "uifactory.h"
#include "font.h"
#include "mesh.h"
#include "configmanager.h"
#include "irenderer.h"
#include "mathcore.h"
#include "fontmanager.h"
#include "keyboard.h"
#include "mouse.h"
#include "iuiinputmap.h"

void UIWidgetList::ScrollbarScrolled( void *pWidget, void *pData )
{
	UIWidgetScrollbar *pScrollbar = static_cast<UIWidgetScrollbar*>( pWidget );
	UIWidgetList *pList = static_cast<UIWidgetList*>( pData );
	pList->SetFirst( pScrollbar->GetScrollbarPosition() );
}

UIWidgetList::UIWidgetList()
:	m_Font( NULL )
,	m_ClampToPixelGrid( false )
,	m_Mesh( NULL )
,	m_FontPrintFlags( FONT_PRINT_LEFT | FONT_NOWRAP )
,	m_Width( 0.0f )
,	m_Height( 0.0f )
,	m_HasDropShadow( false )
,	m_DropShadowOffset()
,	m_DropShadowColor( ARGB_TO_COLOR( 255, 0, 0, 0 ) )
,	m_DropShadowMesh( NULL )
,	m_First( 0 )
,	m_Selected( -1 )
,	m_Entries()
,	m_Scrollbar( NULL )
,	m_Initializing( true )
,	m_EmptyString( "" )
{
}

UIWidgetList::UIWidgetList( const SimpleString& DefinitionName )
:	m_Font( NULL )
,	m_ClampToPixelGrid( false )
,	m_Mesh( NULL )
,	m_FontPrintFlags( FONT_PRINT_LEFT | FONT_NOWRAP )
,	m_Width( 0.0f )
,	m_Height( 0.0f )
,	m_HasDropShadow( false )
,	m_DropShadowOffset()
,	m_DropShadowColor( ARGB_TO_COLOR( 255, 0, 0, 0 ) )
,	m_DropShadowMesh( NULL )
,	m_First( 0 )
,	m_Selected( 0 )
,	m_Entries()
,	m_Scrollbar( NULL )
,	m_Initializing( true )
{
	InitializeFromDefinition( DefinitionName );
}

UIWidgetList::~UIWidgetList()
{
	SafeDelete( m_Mesh );
	SafeDelete( m_DropShadowMesh );
}

void UIWidgetList::UpdateRender()
{
	// This will fail if UpdateRender is called between submitting UI meshes to the renderer and actually
	// rendering the scene. It may actually be safe if this widget was not submitted to the renderer.
	DEBUGASSERT( m_UIManager->DEBUGIsSafeToUpdateRender( m_LastRenderedTime ) );

	SafeDelete( m_Mesh );
	SafeDelete( m_DropShadowMesh );

	IRenderer* pRenderer = m_UIManager->GetRenderer();

	if ( !m_Initializing )
	{
		// don't do this on screen creation as the world may not yet be
		// available for the scrollbar to send it's update event.
		UpdateScrollbar();
		m_Initializing = false;
	}

	// TODO
	SimpleString RenderString = "";
	uint NumEntries = Min( (uint) m_Visible, m_Entries.Size() );
	for ( uint i = m_First; i < m_First + NumEntries; ++i )
	{
		RenderString += m_Entries[i] + "\n";
	}

	m_Mesh = m_Font->Print( RenderString.CStr(), SRect( 0.0f, 0.0f, m_Width, GetHeight() ), m_FontPrintFlags );
	ASSERT( m_Mesh );

	m_Mesh->m_Location = Vector( m_TopLeft.x, 0.0f, m_TopLeft.y );
	m_Mesh->m_AABB.m_Min.x += m_TopLeft.x;
	m_Mesh->m_AABB.m_Min.z += m_TopLeft.y;
	m_Mesh->m_AABB.m_Max.x += m_TopLeft.x;
	m_Mesh->m_AABB.m_Max.z += m_TopLeft.y;

	m_Mesh->SetMaterialDefinition( "Material_HUD", pRenderer );
	m_Mesh->SetMaterialFlags( m_RenderInWorld ? MAT_INWORLDHUD : MAT_HUD );
	m_Mesh->m_ConstantColor = m_Color;

	if( m_HasDropShadow )
	{
		m_DropShadowMesh = new Mesh;
		m_DropShadowMesh->Initialize( m_Mesh->m_VertexBuffer, m_Mesh->m_VertexDeclaration, m_Mesh->m_IndexBuffer, NULL );
		m_DropShadowMesh->m_Material		= m_Mesh->m_Material;

		m_DropShadowMesh->m_Location		= m_Mesh->m_Location;
		m_DropShadowMesh->m_Location.x		+= m_DropShadowOffset.x;
		m_DropShadowMesh->m_Location.z		+= m_DropShadowOffset.y;

		m_DropShadowMesh->m_AABB			= m_Mesh->m_AABB;
		m_DropShadowMesh->m_AABB.m_Min.x	+= m_DropShadowOffset.x;
		m_DropShadowMesh->m_AABB.m_Min.z	+= m_DropShadowOffset.y;
		m_DropShadowMesh->m_AABB.m_Max.x	+= m_DropShadowOffset.x;
		m_DropShadowMesh->m_AABB.m_Max.z	+= m_DropShadowOffset.y;

		m_DropShadowMesh->m_ConstantColor	= m_DropShadowColor;
	}

	UpdateScrollbar();
}

void UIWidgetList::Render( bool HasFocus )
{
	XTRACE_FUNCTION;

	UIWidget::Render( HasFocus );

	if( IsDisabled() )
	{
		m_Mesh->m_ConstantColor = m_DisabledColor;
	}
	else if( HasFocus )
	{
		m_Mesh->m_ConstantColor = GetHighlightColor();
	}
	else
	{
		m_Mesh->m_ConstantColor = m_Color;
	}

	// Don't draw mesh if it's going to be invisible
	if( m_Mesh->m_ConstantColor.a > 0.0f )
	{
		if( m_HasDropShadow )
		{
			m_UIManager->GetRenderer()->AddMesh( m_DropShadowMesh );
		}

		m_UIManager->GetRenderer()->AddMesh( m_Mesh );
	}
}

void UIWidgetList::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	UIWidget::InitializeFromDefinition( DefinitionName );

	MAKEHASH( m_Archetype );
	MAKEHASH( DefinitionName );

	STATICHASH( Font );
	m_Font = m_UIManager->GetRenderer()->GetFontManager()->GetFont( ConfigManager::GetArchetypeString( sFont, sm_Archetype, DEFAULT_FONT, sDefinitionName ) );

	STATICHASH( Alignment );
	GetFontPrintFlags( ConfigManager::GetArchetypeString( sAlignment, sm_Archetype, "", sDefinitionName ) );

	STATICHASH( HasDropShadow );
	m_HasDropShadow = ConfigManager::GetArchetypeBool( sHasDropShadow, sm_Archetype, false, sDefinitionName );

	STATICHASH( DropShadowColorR );
	m_DropShadowColor.r = ConfigManager::GetArchetypeFloat( sDropShadowColorR, sm_Archetype, 1.0f, sDefinitionName );

	STATICHASH( DropShadowColorG );
	m_DropShadowColor.g = ConfigManager::GetArchetypeFloat( sDropShadowColorG, sm_Archetype, 1.0f, sDefinitionName );

	STATICHASH( DropShadowColorB );
	m_DropShadowColor.b = ConfigManager::GetArchetypeFloat( sDropShadowColorB, sm_Archetype, 1.0f, sDefinitionName );

	STATICHASH( DropShadowColorA );
	m_DropShadowColor.a = ConfigManager::GetArchetypeFloat( sDropShadowColorA, sm_Archetype, 1.0f, sDefinitionName );

	STATICHASH( DisplayWidth );
	const float DisplayWidth	= ConfigManager::GetFloat( sDisplayWidth );
	const float ParentWidth		= m_OriginParent ? m_OriginParent->GetWidth() : DisplayWidth;
	const float ParentX			= m_OriginParent ? Ceiling( m_OriginParent->GetX() ) : 0.0f;

	STATICHASH( DisplayHeight );
	const float DisplayHeight	= ConfigManager::GetFloat( sDisplayHeight );
	const float ParentHeight	= m_OriginParent ? m_OriginParent->GetHeight() : DisplayHeight;
	const float ParentY			= m_OriginParent ? Ceiling( m_OriginParent->GetY() ) : 0.0f;

	STATICHASH( PixelX );
	STATICHASH( ScreenX );
	float X = Pick(
		ConfigManager::GetArchetypeFloat( sPixelX, sm_Archetype, 0.0f, sDefinitionName ),
		ParentWidth * ConfigManager::GetArchetypeFloat( sScreenX, sm_Archetype, 0.0f, sDefinitionName ) );

	STATICHASH( PixelY );
	STATICHASH( ScreenY );
	float Y = Pick(
		ConfigManager::GetArchetypeFloat( sPixelY, sm_Archetype, 0.0f, sDefinitionName ),
		ParentHeight * ConfigManager::GetArchetypeFloat( sScreenY, sm_Archetype, 0.0f, sDefinitionName ) );

	STATICHASH( PixelWidth );
	STATICHASH( ScreenWidth );
	m_Width = Pick(
		ConfigManager::GetArchetypeFloat( sPixelWidth, sm_Archetype, 0.0f, sDefinitionName ),
		ParentWidth * ConfigManager::GetArchetypeFloat( sScreenWidth, sm_Archetype, 0.0f, sDefinitionName ) );

	STATICHASH( PixelHeight );
	STATICHASH( ScreenHeight );
	m_Height = Pick(
		ConfigManager::GetArchetypeFloat( sPixelHeight, sm_Archetype, 0.0f, sDefinitionName ),
		ParentHeight * ConfigManager::GetArchetypeFloat( sScreenHeight, sm_Archetype, 0.0f, sDefinitionName ) );

	STATICHASH( DropShadowOffsetPixelX );
	STATICHASH( DropShadowOffsetScreenX );
	m_DropShadowOffset.x =
		Pick(
			ConfigManager::GetArchetypeFloat( sDropShadowOffsetPixelX, sm_Archetype, 0.0f, sDefinitionName ),
			DisplayWidth * ConfigManager::GetArchetypeFloat( sDropShadowOffsetScreenX, sm_Archetype, 0.0f, sDefinitionName ) );

	STATICHASH( DropShadowOffsetPixelY );
	STATICHASH( DropShadowOffsetScreenY );
	m_DropShadowOffset.y =
		Pick(
			ConfigManager::GetArchetypeFloat( sDropShadowOffsetPixelY, sm_Archetype, 0.0f, sDefinitionName ),
			DisplayHeight * ConfigManager::GetArchetypeFloat( sDropShadowOffsetScreenY, sm_Archetype, 0.0f, sDefinitionName ) );

	AdjustDimensionsToParent( X, Y, m_Width, m_Height, ParentX, ParentY, ParentWidth, ParentHeight );
	GetPositionFromOrigin( X, Y, m_Width, m_Height );

	STATICHASH( ClampToPixelGrid );
	if( ConfigManager::GetArchetypeBool( sClampToPixelGrid, sm_Archetype, true, sDefinitionName ) )
	{
		m_TopLeft.x = Round( m_TopLeft.x );
		m_TopLeft.y = Round( m_TopLeft.y );
	}

	// only call this when constructing the widget, not thereafter.
	STATICHASH( Scrollbar );
	m_Scrollbar = static_cast<UIWidgetScrollbar*>(	UIFactory::CreateWidget( ConfigManager::GetArchetypeString( sScrollbar, sm_Archetype, "", sDefinitionName ), GetOwnerScreen() ) );
	if ( m_Scrollbar )
	{
		GetOwnerScreen()->DeferredAddWidget( m_Scrollbar );
		m_Scrollbar->m_Callback = SUICallback( ScrollbarScrolled, this );
	}

	UpdateRender();
}

void UIWidgetList::GetBounds( SRect& OutBounds )
{
	OutBounds.m_Left	= m_TopLeft.x;
	OutBounds.m_Top		= m_TopLeft.y;
	OutBounds.m_Right	= m_TopLeft.x + m_Width;
	OutBounds.m_Bottom	= m_TopLeft.y + m_Height;
}

/*virtual*/ void UIWidgetList::Tick( const float DeltaTime )
{
	XTRACE_FUNCTION;

	if( !m_Velocity.IsZero() )
	{
		m_TopLeft += m_Velocity * DeltaTime;

		if( m_ClampToPixelGrid )
		{
			// Floor instead of round so we don't mess with the velocity.
			// (Essentially the same as subtracting the half pixel offset
			// before rounding, since it just adds 0.5.)
			const float PixelGridOffset = 0.5f - GetPixelGridOffset();
			m_TopLeft.x = Floor( m_TopLeft.x ) + PixelGridOffset;
			m_TopLeft.y = Floor( m_TopLeft.y ) + PixelGridOffset;
		}
	}
}

/* virtual */ void UIWidgetList::OnTrigger()
{
	Mouse *pMouse = m_UIManager->GetMouse();
	int X, Y;
	pMouse->GetPosition( X, Y, m_UIManager->GetWindow() );
	X -= m_TopLeft.x;
	Y -= m_TopLeft.y;
	m_Selected = m_First + floorf( Y / m_Font->GetHeight() );
	if ( m_Selected >= m_Entries.Size() )
	{
		m_Selected = -1;
		return;
	}
	UIWidget::OnTrigger();
}

bool UIWidgetList::HandleInput()
{
	IUIInputMap* const pInputMap = m_UIManager->GetUIInputMap();
	Mouse *pMouse = m_UIManager->GetMouse();

	// Up arrow
	if( pInputMap->OnUp() || pMouse->OnRise( Mouse::EB_WheelUp ) )
	{
		SetFirst( m_First - 1 );
		return true;
	}
	// Down arrow
	else if( pInputMap->OnDown() || pMouse->OnRise( Mouse::EB_WheelDown ) )
	{
		SetFirst( m_First + 1 );
		return true;
	}
	return false;
}

void UIWidgetList::UpdateScrollbar()
{
	if ( m_Scrollbar )
	{
		m_Visible = floorf( m_Height / m_Font->GetHeight() );
		m_Scrollbar->SetScrollbarVisible( m_Visible );
		m_Scrollbar->SetScrollbarTotal( m_Entries.Size() );
		m_Scrollbar->SetScrollbarPosition( m_First );
	}
}

void UIWidgetList::AddEntry( const SimpleString& Entry )
{
	m_Entries.PushBack( Entry );
	UpdateRender();
}

void UIWidgetList::ClearEntries()
{
	m_Entries.Clear();
	SetFirst( 0 );
}

void UIWidgetList::SetFirst( const float First )
{
	int ProtoFirst = Round( First );
	int MaxFirst = m_Entries.Size();
	if ( MaxFirst > m_Visible )
	{
		MaxFirst -= m_Visible;
	}
	else
	{
		MaxFirst = 0;
	}
	if ( ProtoFirst < 0 )
	{
		ProtoFirst = 0;
	}
	else if ( ProtoFirst > MaxFirst )
	{
		ProtoFirst = MaxFirst;
	}
	m_First = ProtoFirst;
	UpdateRender();
}

const SimpleString& UIWidgetList::GetString()
{
	return m_Selected >= 0 ? m_Entries[ m_Selected ] : m_EmptyString;
}

void UIWidgetList::GetFontPrintFlags( const SimpleString& Alignment )
{
	if( Alignment == "Right" )
	{
		m_FontPrintFlags = FONT_PRINT_RIGHT;
	}
	else if( Alignment == "Center" )
	{
		m_FontPrintFlags = FONT_PRINT_CENTER;
	}
	else if( Alignment == "Left" )
	{
		m_FontPrintFlags = FONT_PRINT_RIGHT;
	}
#if BUILD_DEV
	else
	{
		ASSERT( ( m_FontPrintFlags & FONT_PRINT_MASK ) == FONT_PRINT_LEFT );
	}
#endif
	m_FontPrintFlags |= FONT_NOWRAP;
}
