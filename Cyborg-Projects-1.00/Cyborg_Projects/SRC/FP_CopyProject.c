/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = FP_CopyProject.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.PROJECT.COPY;
FILE.PATH      = <PROJECT>/src/FP_CopyProject.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <stdio.h>

#include "include/shared_inc.h"
#include "include/FP_CopyProject.h"
#include "include/Debug_Dialog.h"

/* ---------------------------------------------------------
   INTERNAL LOGGING
--------------------------------------------------------- */
static void FP_Log(FP_STATE *st, const char *fmt, ...)
{
    char buf[1024];
    va_list ap;

    va_start(ap, fmt);
    _vsnprintf(buf, sizeof(buf)-1, fmt, ap);
    buf[sizeof(buf)-1] = 0;
    va_end(ap);

    DebugPrint("CopyProject: %s\n", buf);
}

/* ---------------------------------------------------------
   COPY ALL FILES IN A DIRECTORY
--------------------------------------------------------- */
static void FP_CopyDirectory(FP_STATE *st, const char *srcDir, const char *destDir)
{
    WIN32_FIND_DATAA fd;
    char pattern[MAX_PATH];

    wsprintfA(pattern, "%s\\*.*", srcDir);

    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return;

    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        char src[MAX_PATH];
        char dst[MAX_PATH];

        wsprintfA(src, "%s\\%s", srcDir, fd.cFileName);
        wsprintfA(dst, "%s\\%s", destDir, fd.cFileName);

        if (CopyFileA(src, dst, FALSE))
            FP_Log(st, "Copied: %s", fd.cFileName);
        else
            FP_Log(st, "FAILED: %s", fd.cFileName);

    } while (FindNextFileA(h, &fd));

    FindClose(h);
}

/* ---------------------------------------------------------
   MAIN COPY OPERATION
--------------------------------------------------------- */
void FP_CopyProject(FP_STATE *st)
{
    if (!st->manifestPath[0]) {
        FP_Log(st, "No project loaded. Cannot copy.");
        return;
    }

    /* Ask user for destination folder */
    BROWSEINFOA bi = {0};
    bi.hwndOwner = st->hDlg;
    bi.lpszTitle = "Select destination folder for project copy";

    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
    if (!pidl)
        return;

    char dest[MAX_PATH];
    SHGetPathFromIDListA(pidl, dest);
    CoTaskMemFree(pidl);

    FP_Log(st, "Copying project to: %s", dest);

    /* Open manifest */
    FILE *f = fopen(st->manifestPath, "r");
    if (!f) {
        FP_Log(st, "Failed to open manifest.");
        return;
    }

    char dir[MAX_PATH];

    while (fgets(dir, sizeof(dir), f)) {

        /* Strip CR/LF */
        dir[strcspn(dir, "\r\n")] = 0;

        if (!dir[0])
            continue;

        /* Normalize path */
        PathRemoveBackslashA(dir);

        FP_Log(st, "Scanning: %s", dir);

        /* Copy files from this directory */
        FP_CopyDirectory(st, dir, dest);
    }

    fclose(f);

    FP_Log(st, "Project copy complete.");
}
