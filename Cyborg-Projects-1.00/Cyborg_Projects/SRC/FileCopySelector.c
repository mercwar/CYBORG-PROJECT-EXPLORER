/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = FileCopySelector.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V02.00;
FILE.MODULE    = CYBORG.FILECOPY.SELECTOR;
FILE.PATH      = <PROJECT>/src/FileCopySelector.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

FILE COPY SELECTOR LOGIC (LEGACY WRAPPER).
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "include/FileCopySelector.h"
#include "include/Debug_Dialog.h"

void FileCopySelector_Open(HWND hOwner, HWND hSourceEdit)
{
    DebugDialog_Open();
    DebugPrint(
        "FileCopySelector_Open was called without a workspace.\r\n"
        "Use FileCopySelector_Main(hOwner, hSourceEdit, &g_ws)."
    );
}
