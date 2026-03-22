
/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = DebugDialog.h;
FILE.TYPE      = HEADER;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.DEBUG.DIALOG;
FILE.PATH      = <PROJECT>/include/DebugDialog.h;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

INTERNAL DEBUG WINDOW FOR CYBORG PROJECTS.
AVIS DO NOT REMOVE */

#ifndef CYBORG_DEBUG_DIALOG_H
#define CYBORG_DEBUG_DIALOG_H
#include "include/shared_inc.h"


void DebugDialog_Open(void);
void DebugPrint(const char *fmt, ...);

#endif /* CYBORG_DEBUG_DIALOG_H */
