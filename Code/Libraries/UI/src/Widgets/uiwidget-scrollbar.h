#ifndef UIWIDGETSCROLLBAR_H
#define UIWIDGETSCROLLBAR_H

#include "uiwidget.h"
#include "vector2.h"
#include "simplestring.h"

class Font;
class Mesh;
class UIWidgetText;
class UIWidgetImage;

class UIWidgetScrollbar : public UIWidget
{
public:
	UIWidgetScrollbar();
	UIWidgetScrollbar( const SimpleString& DefinitionName );
	virtual ~UIWidgetScrollbar();

	DEFINE_UIWIDGET_FACTORY( Scrollbar );

	virtual void	Render( bool HasFocus );
	virtual void	UpdateRender();
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );
	virtual void	GetBounds( SRect& OutBounds );
	virtual void	Refresh();
	virtual void	Released();
	virtual void	Drag( float X, float Y );

	virtual void	OnTrigger();

	void			SendScrollbarEvent() const;

	void			SetScrollbarValue( const float T );
	float			GetScrollbarValue() const;
	void			SetScrollbarPosition( const float T );
	float			GetScrollbarPosition() const;
	void			SetScrollbarTotal( const float T );
	float			GetScrollbarTotal() const;
	void			SetScrollbarVisible( const float T );
	float			GetScrollbarVisible() const;

	UIWidgetImage*	m_ScrollbarBack;
	UIWidgetImage*	m_Scrollbar;

	float			m_Total, m_Visible;
	bool			m_Vertical;
};

#endif // UIWIDGETSCROLLBAR_H
