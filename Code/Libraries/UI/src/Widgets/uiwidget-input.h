#ifndef UIWidgetInput_H
#define UIWidgetInput_H

#include "uiwidget.h"
#include "vector2.h"
#include "array.h"
#include "simplestring.h"
#include "Widgets/uiwidget-frame.h"

class Font;
class Mesh;

class UIWidgetInput : public UIWidget
{
public:
	UIWidgetInput();
	UIWidgetInput( const SimpleString& DefinitionName );
	virtual ~UIWidgetInput();

	DEFINE_UIWIDGET_FACTORY( Input );

	virtual void	Render( bool HasFocus );
	virtual void	UpdateRender();
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );
	virtual void	GetBounds( SRect& OutBounds );
	virtual void	Refresh();
	virtual void	Tick( const float DeltaTime );
	virtual void	OnTrigger();
	virtual bool	HandleInput();
	virtual void	GetFocus();
	virtual void	LoseFocus();
	
	void			Backspace();
	void			ClearBuffer();
	void			GetFontPrintFlags( const SimpleString& Alignment );

	const SimpleString&	GetString();		// Returns the string that was input
	void			SetString( const SimpleString& String );

	Font*			m_Font;
	bool			m_ClampToPixelGrid;
	Mesh*			m_Mesh;
	uint			m_FontPrintFlags;
	float			m_Width;
	float			m_Height;

	bool			m_HasDropShadow;
	Vector2			m_DropShadowOffset;
	Vector4			m_DropShadowColor;
	Mesh*			m_DropShadowMesh;

	SimpleString	m_ResultString;
	Array< char >	m_InputBuffer;
	UIWidgetFrame*	m_InputFrame;
};

#endif // UIWidgetInput_H
