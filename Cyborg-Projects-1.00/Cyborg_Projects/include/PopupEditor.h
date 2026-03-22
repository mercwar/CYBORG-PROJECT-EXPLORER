/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = PopupEditor.h;
FILE.TYPE      = HEADER;
FILE.VERSION   = V02.00;
FILE.MODULE    = CYBORG.POPUP.EDITOR;
FILE.PATH      = <PROJECT>/include/PopupEditor.h;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

LIGHTWEIGHT POPUP EDITOR FOR MIGRATION RESULTS.
AVIS DO NOT REMOVE */

#ifndef CYBORG_POPUP_EDITOR_H
#define CYBORG_POPUP_EDITOR_H

#define WIN32_LEAN_AND_MEAN
#include "include/shared_inc.h"

HWND PopupEditor_Show(HWND hParent, const char *title, const char *content);

#endif /* CYBORG_POPUP_EDITOR_H */
