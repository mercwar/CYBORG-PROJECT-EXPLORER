/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = FolderPicker.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.FOLDER.PICKER;
FILE.PATH      = <PROJECT>/src/FolderPicker.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

SIMPLE SHELL FOLDER PICKER IMPLEMENTATION.
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <string.h>

#include "include/FolderPicker.h"

BOOL FolderPicker_PickFolder(HWND hParent, char *outPath, int outPathSize)
{
    BROWSEINFOA bi;
    LPITEMIDLIST pidl;
    BOOL result = FALSE;

    ZeroMemory(&bi, sizeof(bi));
    bi.hwndOwner      = hParent;
    bi.lpszTitle      = "Select a folder";
    bi.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    pidl = SHBrowseForFolderA(&bi);
    if (pidl)
    {
        char path[MAX_PATH];

        if (SHGetPathFromIDListA(pidl, path))
        {
            lstrcpynA(outPath, path, outPathSize);
            result = TRUE;
        }

        CoTaskMemFree(pidl);
    }

    return result;
}
