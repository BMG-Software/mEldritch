#include "core.h"
#include "3d.h"
#include "uiwidget-scrollbar.h"
#include "uimanager.h"
#include "mesh.h"
#include "shadermanager.h"
#include "configmanager.h"
#include "stringmanager.h"
#include "Widgets/uiwidget-image.h"
#include "mathcore.h"
#include "uifactory.h"
#include "wbeventmanager.h"
#include "wbworld.h"
#include "iuiinputmap.h"

UIWidgetScrollbar::UIWidgetScrollbar()
:	m_ScrollbarBack( NULL )
,	m_Scrollbar( NULL )
,	m_Total( 0.0f )
,	m_Visible( 0.0f )
,	m_Vertical( true )
{
}

UIWidgetScrollbar::UIWidgetScrollbar( const SimpleString& DefinitionName )
:	m_ScrollbarBack( NULL )
,	m_Scrollbar( NULL )
,	m_Total( 0.0f )
,	m_Visible( 0.0f )
,	m_Vertical( true )
{
	InitializeFromDefinition( DefinitionName );
}

UIWidgetScrollbar::~UIWidgetScrollbar()
{
	SafeDelete( m_ScrollbarBack );
	SafeDelete( m_Scrollbar );
}

void UIWidgetScrollbar::UpdateRender()
{
	if ( m_Total > 0 )
	{
		float Factor = Saturate( m_Visible / m_Total );
		if ( m_Vertical )
		{
			m_Scrollbar->m_Dimensions.y = ( m_ScrollbarBack->m_Dimensions.y * Factor );
			if ( m_Scrollbar->m_Dimensions.y < 4 )
			{
				m_Scrollbar->m_Dimensions.y = 4;
			}
		}
		else
		{
			m_Scrollbar->m_Dimensions.x = ( m_ScrollbarBack->m_Dimensions.x * Factor );
			if ( m_Scrollbar->m_Dimensions.x < 4 )
			{
				m_Scrollbar->m_Dimensions.x = 4;
			}
		}
	}

	m_ScrollbarBack->UpdateRender();
	m_Scrollbar->UpdateRender();
}

void UIWidgetScrollbar::Render( bool HasFocus )
{
	XTRACE_FUNCTION;

	m_ScrollbarBack->Render( HasFocus );
	m_Scrollbar->Render( HasFocus );
}

void UIWidgetScrollbar::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	SafeDelete( m_ScrollbarBack );
	SafeDelete( m_Scrollbar );

	UIWidget::InitializeFromDefinition( DefinitionName );

	STATICHASH( ScrollbarType );
	STATICHASH( ScrollbarBack );
	STATICHASH( Scrollbar );
	STATICHASH( Vertical );
	STATICHASH( Horizontal );
	MAKEHASH( m_Archetype );
	MAKEHASH( DefinitionName );

	STATICHASH( UI );
	STATICHASH( UIScreenVersion );
	const uint UIScreenVersion = ConfigManager::GetInt( sUIScreenVersion, 0, sUI );

	m_Vertical = !( ConfigManager::GetHash(sScrollbarType, sVertical, sDefinitionName ) == sHorizontal );
	if( UIScreenVersion < 2 )
	{
		m_ScrollbarBack = new UIWidgetImage( ConfigManager::GetArchetypeString( sScrollbarBack, sm_Archetype, "", sDefinitionName ) );
		m_Scrollbar = new UIWidgetImage( ConfigManager::GetArchetypeString( sScrollbar, sm_Archetype, "", sDefinitionName ) );
	}
	else
	{
		m_ScrollbarBack	= static_cast<UIWidgetImage*>(	UIFactory::CreateWidget( ConfigManager::GetArchetypeString( sScrollbarBack, sm_Archetype, "", sDefinitionName ), GetOwnerScreen() ) );
		m_Scrollbar		= static_cast<UIWidgetImage*>(	UIFactory::CreateWidget( ConfigManager::GetArchetypeString( sScrollbar, sm_Archetype, "", sDefinitionName ), GetOwnerScreen() ) );

		ASSERT( m_ScrollbarBack );
		ASSERT( m_Scrollbar );
	}
}

void UIWidgetScrollbar::GetBounds( SRect& OutBounds )
{
	SRect ScrollbarBackBounds;
	SRect ScrollbarBounds;

	m_ScrollbarBack->GetBounds( ScrollbarBackBounds );
	m_Scrollbar->GetBounds( ScrollbarBounds );

	OutBounds.m_Left	= Min( ScrollbarBackBounds.m_Left,		ScrollbarBounds.m_Left );
	OutBounds.m_Top		= Min( ScrollbarBackBounds.m_Top,		ScrollbarBounds.m_Top );
	OutBounds.m_Right	= Max( ScrollbarBackBounds.m_Right,		ScrollbarBounds.m_Right );
	OutBounds.m_Bottom	= Max( ScrollbarBackBounds.m_Bottom,	ScrollbarBounds.m_Bottom );
}

void UIWidgetScrollbar::Refresh()
{
	UIWidget::Refresh();

	m_ScrollbarBack->Refresh();
	m_Scrollbar->Refresh();
}

/*virtual*/ void UIWidgetScrollbar::Released()
{
	OnTrigger();
}

/*virtual*/ void UIWidgetScrollbar::OnTrigger()
{
	UIWidget::OnTrigger();

	SendScrollbarEvent();
}

void UIWidgetScrollbar::SendScrollbarEvent() const
{
	WBWorld *World = WBWorld::GetInstance();
	if ( World != NULL )
	{
		WBEventManager* WorldEventManager = World->GetEventManager();
		WB_MAKE_EVENT( OnScrollbarChanged, NULL );
		WB_SET_AUTO( OnScrollbarChanged, Hash, ScrollbarName, m_Name );
		WB_SET_AUTO( OnScrollbarChanged, Float, ScrollbarValue, GetScrollbarValue() );
		WB_SET_AUTO( OnScrollbarChanged, Float, ScrollbarPosition, GetScrollbarPosition() );
		WB_DISPATCH_EVENT( WorldEventManager, OnScrollbarChanged, NULL );
	}
}

/*virtual*/ void UIWidgetScrollbar::Drag( float X, float Y )
{
	if ( m_Vertical )
	{
		m_Scrollbar->m_TopLeft.y = Clamp( Y - ( m_Scrollbar->m_Dimensions.y / 2 ), m_ScrollbarBack->m_TopLeft.y, m_ScrollbarBack->m_TopLeft.y + m_ScrollbarBack->m_Dimensions.y - m_Scrollbar->m_Dimensions.y );
	}
	else
	{
		m_Scrollbar->m_TopLeft.x = Clamp( X - ( m_Scrollbar->m_Dimensions.x / 2 ), m_ScrollbarBack->m_TopLeft.x, m_ScrollbarBack->m_TopLeft.x + m_ScrollbarBack->m_Dimensions.x - m_Scrollbar->m_Dimensions.x );
	}
	UpdateRender();

	OnTrigger();
}

void UIWidgetScrollbar::SetScrollbarValue( const float T )
{
	if ( m_Vertical )
	{
		m_Scrollbar->m_TopLeft.y = Lerp( m_ScrollbarBack->m_TopLeft.y, m_ScrollbarBack->m_TopLeft.y + m_ScrollbarBack->m_Dimensions.y - m_Scrollbar->m_Dimensions.y, Saturate( T ) );
	}
	else
	{
		m_Scrollbar->m_TopLeft.x = Lerp( m_ScrollbarBack->m_TopLeft.x, m_ScrollbarBack->m_TopLeft.x + m_ScrollbarBack->m_Dimensions.x - m_Scrollbar->m_Dimensions.x, Saturate( T ) );
	}
	UpdateRender();

	SendScrollbarEvent();
}

float UIWidgetScrollbar::GetScrollbarValue() const
{
	if ( m_Visible >= m_Total )
	{
		return 0;
	}
	if ( m_Vertical )
	{
		return Saturate( InvLerp( m_Scrollbar->m_TopLeft.y, m_ScrollbarBack->m_TopLeft.y, m_ScrollbarBack->m_TopLeft.y + m_ScrollbarBack->m_Dimensions.y - m_Scrollbar->m_Dimensions.y ) );
	}
	else
	{
		return Saturate( InvLerp( m_Scrollbar->m_TopLeft.x, m_ScrollbarBack->m_TopLeft.x, m_ScrollbarBack->m_TopLeft.x + m_ScrollbarBack->m_Dimensions.x - m_Scrollbar->m_Dimensions.x ) );
	}
}

void UIWidgetScrollbar::SetScrollbarPosition( const float T )
{
	return SetScrollbarValue( T / ( m_Total - m_Visible ) );
}

float UIWidgetScrollbar::GetScrollbarPosition() const
{
	return ( m_Total - m_Visible ) * GetScrollbarValue();
}

void UIWidgetScrollbar::SetScrollbarTotal( const float T )
{
	m_Total = T;
	UpdateRender();
}

float UIWidgetScrollbar::GetScrollbarTotal() const
{
	return m_Total;
}

void UIWidgetScrollbar::SetScrollbarVisible( const float T )
{
	m_Visible = T;
	UpdateRender();
}

float UIWidgetScrollbar::GetScrollbarVisible() const
{
	return m_Visible;
}
