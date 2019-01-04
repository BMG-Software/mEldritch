#include "core.h"
#include "Widgets/uiwidget-frame.h"
#include "uiwidget-input.h"
#include "uimanager.h"
#include "uifactory.h"
#include "font.h"
#include "mesh.h"
#include "configmanager.h"
#include "irenderer.h"
#include "mathcore.h"
#include "fontmanager.h"
#include "keyboard.h"
#include "iuiinputmap.h"

#include "mini_utf8.h"

UIWidgetInput::UIWidgetInput()
:	m_Font( NULL )
,	m_ClampToPixelGrid( false )
,	m_Mesh( NULL )
,	m_FontPrintFlags( FONT_PRINT_LTHENR | FONT_SINGLE_LINE )
,	m_Width( 0.0f )
,	m_Height( 0.0f )
,	m_HasDropShadow( false )
,	m_DropShadowOffset()
,	m_DropShadowColor( ARGB_TO_COLOR( 255, 0, 0, 0 ) )
,	m_DropShadowMesh( NULL )
,	m_ResultString( "" )
,	m_InputBuffer()
,	m_InputFrame( NULL )
{
}

UIWidgetInput::UIWidgetInput( const SimpleString& DefinitionName )
:	m_Font( NULL )
,	m_ClampToPixelGrid( false )
,	m_Mesh( NULL )
,	m_FontPrintFlags( FONT_PRINT_LTHENR | FONT_SINGLE_LINE )
,	m_Width( 0.0f )
,	m_Height( 0.0f )
,	m_HasDropShadow( false )
,	m_DropShadowOffset()
,	m_DropShadowColor( ARGB_TO_COLOR( 255, 0, 0, 0 ) )
,	m_DropShadowMesh( NULL )
,	m_ResultString( "" )
,	m_InputBuffer()
,	m_InputFrame( NULL )
{
	InitializeFromDefinition( DefinitionName );
}

UIWidgetInput::~UIWidgetInput()
{
	SafeDelete( m_Mesh );
	SafeDelete( m_DropShadowMesh );
	SafeDelete( m_InputFrame );
}

void UIWidgetInput::UpdateRender()
{
	// This will fail if UpdateRender is called between submitting UI meshes to the renderer and actually
	// rendering the scene. It may actually be safe if this widget was not submitted to the renderer.
	DEBUGASSERT( m_UIManager->DEBUGIsSafeToUpdateRender( m_LastRenderedTime ) );

	SafeDelete( m_Mesh );
	SafeDelete( m_DropShadowMesh );

	IRenderer* pRenderer = m_UIManager->GetRenderer();

	m_Mesh = m_Font->Print( m_ResultString.CStr(), SRect( 0.0f, 0.0f, m_Width, GetHeight() ), m_FontPrintFlags );
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

	m_InputFrame->UpdateRender();
}

void UIWidgetInput::Render( bool HasFocus )
{
	XTRACE_FUNCTION;

	m_InputFrame->Render( HasFocus );

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

	// If we got the focus upon opening the screen, we still need to put
	// the keyboard in WantsTextInput mode so that the input widget will
	// behave as expected.
	if ( HasFocus )
	{
		Keyboard *pKeyboard = m_UIManager->GetKeyboard();
		if ( !pKeyboard->m_WantsTextInput )
		{
			GetFocus();
		}
	}
}

void UIWidgetInput::InitializeFromDefinition( const SimpleString& DefinitionName )
{
	SafeDelete( m_InputFrame );

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

	float Height = m_Font->GetHeight();

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

	AdjustDimensionsToParent( X, Y, m_Width, Height, ParentX, ParentY, ParentWidth, ParentHeight );
	GetPositionFromOrigin( X, Y, m_Width, Height );

	STATICHASH( ClampToPixelGrid );
	if( ConfigManager::GetArchetypeBool( sClampToPixelGrid, sm_Archetype, true, sDefinitionName ) )
	{
		m_TopLeft.x = Round( m_TopLeft.x );
		m_TopLeft.y = Round( m_TopLeft.y );
	}

	STATICHASH( InputFrame );
	m_InputFrame = static_cast<UIWidgetFrame*>(	UIFactory::CreateWidget( ConfigManager::GetArchetypeString( sInputFrame, sm_Archetype, "", sDefinitionName ), GetOwnerScreen() ) );

	ASSERT(m_InputFrame);

	UpdateRender();
}

void UIWidgetInput::GetBounds( SRect& OutBounds )
{
	SRect InputFrameBounds;
	m_InputFrame->GetBounds( InputFrameBounds );
	OutBounds.m_Left	= Min(InputFrameBounds.m_Left, m_TopLeft.x);
	OutBounds.m_Top		= Min(InputFrameBounds.m_Top, m_TopLeft.y);
	OutBounds.m_Right	= Max(InputFrameBounds.m_Right, m_TopLeft.x + m_Width);
	OutBounds.m_Bottom	= Max(InputFrameBounds.m_Bottom, m_TopLeft.y + m_Font->GetHeight() );
}

void UIWidgetInput::Refresh()
{
	UIWidget::Refresh();

	const SimpleString	InputString		= m_InputBuffer;
	const bool			StringChanged	= ( InputString != m_ResultString );

	if( StringChanged )
	{
		m_ResultString = InputString;
		UpdateRender();
	}
}

/*virtual*/ void UIWidgetInput::Tick( const float DeltaTime )
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

/* virtual */ void UIWidgetInput::OnTrigger()
{
	// ignore as this is called by the screen on mouse click and we don't
	// want our actions to be triggered on a click.
}

void UIWidgetInput::Backspace()
{
	const char *pStart = m_InputBuffer.GetData();
	const char *pChar = pStart + m_InputBuffer.Size();
	const char *pPChar = pChar;
	if ( pStart && ( mini_utf8_prevchar(&pPChar, pStart) > 0 ) )
	{
		int Diff = pChar - pPChar;
		for ( int i = 0; i < Diff; ++i )
		{
			m_InputBuffer.PopBack();
		}
	}
}

bool UIWidgetInput::HandleInput()
{
	Keyboard *pKeyboard = m_UIManager->GetKeyboard();
	SimpleString InputText = "";

	if ( pKeyboard->GetText( InputText ) )
	{
		for ( uint i = 0; i < InputText.Length(); ++i )
		{
			char c = InputText.GetChar( i );
			if ( c >= 0x20 || c < 0 )
			{
				m_InputBuffer.PushBack( InputText.GetChar( i ) );
			}
			else if ( c == '\n' || c == '\t' )
			{
				OnTrigger();
			}
		}
		return true;
	}
	// Backspace/Ctrl+Backspace
	else if( pKeyboard->OnRise( Keyboard::EB_Backspace ) )
	{
		if( pKeyboard->IsHigh( Keyboard::EB_LeftControl ) )
		{
			ClearBuffer();
		}
		else
		{
			Backspace();
		}
		return true;
	}
	/* TODO later, needs cursor
	// Left arrow
	if( pKeyboard->OnRise( Keyboard::EB_Left ) )
	{
		// Move Cursor Left
	}
	// Right arrow
	if( pKeyboard->OnRise( Keyboard::EB_Right ) )
	{
		// Move Cursor Right
	}
	*/
	// Enter
	else if( pKeyboard->OnRise( Keyboard::EB_Enter ) )
	{
		// use the parent classes Ontrigger() method to do the work
		UIWidget::OnTrigger();
		return true;
	}
	return false;
}

void UIWidgetInput::GetFocus()
{
	Keyboard *pKeyboard = m_UIManager->GetKeyboard();
	pKeyboard->WantTextInput( true );
}

void UIWidgetInput::LoseFocus()
{
	Keyboard *pKeyboard = m_UIManager->GetKeyboard();
	pKeyboard->WantTextInput( false );
}

void UIWidgetInput::ClearBuffer()
{
	while( m_InputBuffer.Size() )
	{
		m_InputBuffer.PopBack();
	}
}

const SimpleString& UIWidgetInput::GetString()
{
	return m_ResultString;
}

void UIWidgetInput::SetString( const SimpleString& String )
{
	String.FillArray(m_InputBuffer);
	UpdateRender();
}

void UIWidgetInput::GetFontPrintFlags( const SimpleString& Alignment )
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
	else if( Alignment == "Input" )
	{
		m_FontPrintFlags = FONT_PRINT_LTHENR;
	}
#if BUILD_DEV
	else
	{
		ASSERT( ( m_FontPrintFlags & FONT_PRINT_MASK ) == FONT_PRINT_LTHENR );
	}
#endif
	m_FontPrintFlags |= FONT_SINGLE_LINE;
}
