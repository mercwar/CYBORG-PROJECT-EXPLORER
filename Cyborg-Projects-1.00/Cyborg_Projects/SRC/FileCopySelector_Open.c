/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = FileCopySelector_Open.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.FILECOPY.SELECTOR;
FILE.PATH      = <PROJECT>/src/FileCopySelector_Open.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

FILE COPY SELECTOR LEGACY ENTRY POINT.
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "include/FileCopySelector.h"
#include "include/Debug_Dialog.h"

/*
    Legacy/simple entry point.
    This version CANNOT run without a workspace.
    Callers MUST use FileCopySelector_Main() with a valid CYBORG_WORKSPACE*.
*/

