#ifndef ADDUISCREENFACTORY

#include "Screens/uiscreen-base.h"
#include "Screens/uiscreen-fade.h"
#include "Screens/uiscreen-okdialog.h"
#include "Screens/uiscreen-waitdialog.h"
#include "Screens/uiscreen-yesnodialog.h"
#include "Screens/uiscreen-selectfiledialog.h"

#else

ADDUISCREENFACTORY( Base );
ADDUISCREENFACTORY( Fade );
ADDUISCREENFACTORY( OKDialog );
ADDUISCREENFACTORY( WaitDialog );
ADDUISCREENFACTORY( YesNoDialog );
ADDUISCREENFACTORY( SelectFileDialog );

#endif
