/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = FolderPicker.h;
FILE.TYPE      = HEADER;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.FOLDER.PICKER;
FILE.PATH      = <PROJECT>/include/FolderPicker.h;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

SIMPLE SHELL FOLDER PICKER INTERFACE.
AVIS DO NOT REMOVE */

#ifndef CYBORG_FOLDER_PICKER_H
#define CYBORG_FOLDER_PICKER_H

#include <windows.h>
#include <shlobj.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL FolderPicker_PickFolder(HWND hParent, char *outPath, int outPathSize);

#ifdef __cplusplus
}
#endif

#endif /* CYBORG_FOLDER_PICKER_H */
