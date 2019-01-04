#ifndef UISCREENSELECTFILE_H
#define UISCREENSELECTFILE_H

#include "uiscreen.h"
// TODO is this necessary?
#include "Widgets/uiwidget-text.h"
#include "Widgets/uiwidget-input.h"
#include "Widgets/uiwidget-list.h"

class UIScreenSelectFileDialog : public UIScreen
{
public:
	UIScreenSelectFileDialog();
	virtual ~UIScreenSelectFileDialog();

	DEFINE_UISCREEN_FACTORY( SelectFileDialog );

	void SetParameters(
		bool				PauseGame,
		const SimpleString& TitleString,
		const SimpleString&	TitleDynamicString,
		const SimpleString& Path,
		const SimpleString&	Pattern,
		const HashedString&	OkEvent,
		const SimpleString&	OkCommand,
		const SUICallback&	OkCallback);

	void SetPath( const SimpleString& Path );
	void SetFile( const SimpleString& File );

	SimpleString m_Filename;
	SimpleString m_Pattern;

private:
	// The OK callback is called with a string instead of a widget
	SUICallback m_OkCallback;
	UIWidgetText* m_Path;
	UIWidgetInput* m_File;
	UIWidgetList* m_Selector;
	void CallOkCallback();

	static void SelectFileDialogOkCallback(void* pWidget, void* pData);
	static void SelectFileDialogSetPathCallback(void* pWidget, void* pData);
};

#endif // UISCREENSELECTFILE_H
