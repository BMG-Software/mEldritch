#ifndef UISCREENELDSELECTMOD_H
#define UISCREENELDSELECTMOD_H

#include "uiscreen.h"
#include "Widgets/uiwidget-text.h"
#include "Widgets/uiwidget-image.h"
#include "Widgets/uiwidget-list.h"

class UIScreenEldSelectMod : public UIScreen
{
public:
	UIScreenEldSelectMod();
	virtual ~UIScreenEldSelectMod();

	DEFINE_UISCREEN_FACTORY( EldSelectMod );

	virtual void	InitializeFromDefinition( const SimpleString& DefinitionName );

private:
	void			InitializeModlist();
	void			ShowMod( const SimpleString& TheMod = "" );
	void			RequestGoToLevel( const SimpleString& LevelName );

	static void		SelectModCallback(void* pWidget, void* pData);
	static void		ShowModCallback(void* pWidget, void* pData);

	UIWidgetList*	m_Selector;
	UIWidgetImage*	m_Image;
	UIWidgetText*	m_Name;
	UIWidgetText*	m_Version;
	UIWidgetText*	m_Author;
	UIWidgetText*	m_Description;

};

#endif // UISCREENELDSELECTMOD_H
