/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = FileCopySelector.h;
FILE.TYPE      = HEADER;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.FILECOPY.SELECTOR;
FILE.PATH      = <PROJECT>/include/FileCopySelector.h;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

FILE COPY SELECTOR INTERFACE.
AVIS DO NOT REMOVE */

#ifndef CYBORG_FILECOPY_SELECTOR_H
#define CYBORG_FILECOPY_SELECTOR_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "shared_inc.h"
#include "include/Debug_Dialog.h"
#include "include/main.h"   // make sure this is correct

typedef WIZARD_STATE CYBORG_WORKSPACE;

/* 
   Workspace type used by DirSelector and FileTypeSelector.
   Adjust the name/fields if your actual type differs.
*/
//typedef struct _CYBORG_WORKSPACE
//{
    //char   workingDir[MAX_PATH];    /* e.g. H:\PROGRAMS\darkstar\Cyborg BrowserX\include */
    //char   selectedExt[16];         /* e.g. ".h" or ".txt" */
    //struct {
        //char  **dirList;
        //int     dirCount;
    //} searchParams;

    ///* You likely have more fields; keep them as‑is. */
//} CYBORG_WORKSPACE;

/* 
   Public entry point used by PopupEditor / main UI.
   hOwner      – owner window
   hSourceEdit – optional HWND to log/capture selection text
   pWs         – workspace (must not be NULL)
*/
//void FileCopySelector_Main(HWND hOwner, HWND hSourceEdit, CYBORG_WORKSPACE *pWs);
void FileCopySelector_Main(HWND hOwner, HWND hEdit, WIZARD_STATE *ws);

/* Legacy/simple wrapper used by older callers (no workspace). */
void FileCopySelector_Open(HWND hOwner, HWND hSourceEdit);

#endif /* CYBORG_FILECOPY_SELECTOR_H */
