#ifndef ADDUIWIDGETFACTORY

#include "Widgets/uiwidget-text.h"
#include "Widgets/uiwidget-image.h"
#include "Widgets/uiwidget-slider.h"
#include "Widgets/uiwidget-frame.h"
#include "Widgets/uiwidget-spacer.h"
#include "Widgets/uiwidget-composite.h"
#include "Widgets/uiwidget-input.h"
#include "Widgets/uiwidget-scrollbar.h"
#include "Widgets/uiwidget-list.h"

#else

ADDUIWIDGETFACTORY( Text );
ADDUIWIDGETFACTORY( Input );
ADDUIWIDGETFACTORY( Image );
ADDUIWIDGETFACTORY( Slider );
ADDUIWIDGETFACTORY( Frame );
ADDUIWIDGETFACTORY( Spacer );
ADDUIWIDGETFACTORY( Composite );
ADDUIWIDGETFACTORY( Scrollbar );
ADDUIWIDGETFACTORY( List );

#endif
