/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = DirSelector.h;
FILE.TYPE      = HEADER;
FILE.VERSION   = V03.00;
FILE.MODULE    = CYBORG.DIR.SELECTOR;
FILE.PATH      = <PROJECT>/include/DirSelector.h;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

HEADER FOR DIRECTORY SELECTOR DIALOG.
AVIS DO NOT REMOVE */

#ifndef CYBORG_DIR_SELECTOR_H
#define CYBORG_DIR_SELECTOR_H
#include "include/shared_inc.h"

/* open the directory selector using the directory chain in ws->searchParams */
void DirSelector_Open(HWND hwndOwner, HWND hEcho, WIZARD_STATE *ws);

#endif /* CYBORG_DIR_SELECTOR_H */
