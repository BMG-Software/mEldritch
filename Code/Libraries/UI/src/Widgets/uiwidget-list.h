#ifndef UIWIDGETLIST_H
#define UIWIDGETLIST_H

#include "uiwidget.h"
#include "vector2.h"
#include "array.h"
#include "simplestring.h"
#include "Widgets/uiwidget-scrollbar.h"

// beware: call InitializeFromDefinition only once! Also, this creates an
// additional widget in the containing screen before itself.

class Font;
class Mesh;

class UIWidgetList : public UIWidget
{
public:
	UIWidgetList();
	UIWidgetList( const SimpleString& DefinitionName );
	virtual ~UIWidgetList();

	DEFINE_UIWIDGET_FACTORY( List );

	static void		ScrollbarScrolled( void *pWidget, void *pData );

	virtual void	Render( bool HasFocus );
	virtual void	UpdateRender();
	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );
	virtual void	GetBounds( SRect& OutBounds );
	virtual void	Tick( const float DeltaTime );
	virtual void	OnTrigger();
	virtual bool	HandleInput();

	void			UpdateScrollbar();
	void			GetFontPrintFlags( const SimpleString& Alignment );
	const SimpleString&	GetString();		// Returns the selected
	const int		GetSelected() { return m_Selected; }
	void			AddEntry( const SimpleString& Entry );
	void			ClearEntries();
	void			SetFirst( const float First );

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

	int				m_First, m_Selected, m_Visible;
	Array< SimpleString >	m_Entries;

	UIWidgetScrollbar	*m_Scrollbar;

private:
	bool			m_Initializing;
	SimpleString	m_EmptyString;
};

#endif // UIWIDGETLIST_H
