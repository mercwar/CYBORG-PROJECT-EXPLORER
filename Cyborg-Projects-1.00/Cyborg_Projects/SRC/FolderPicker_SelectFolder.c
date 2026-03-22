/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = FolderPicker_SelectFolder.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.FOLDER.PICKER;
FILE.PATH      = <PROJECT>/src/FolderPicker_SelectFolder.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

COMPATIBILITY WRAPPER FOR LEGACY CALLS.
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "include/FolderPicker.h"

BOOL FolderPicker_SelectFolder(HWND hParent, char *outPath, int outSize)
{
    return FolderPicker_PickFolder(hParent, outPath, outSize);
}
