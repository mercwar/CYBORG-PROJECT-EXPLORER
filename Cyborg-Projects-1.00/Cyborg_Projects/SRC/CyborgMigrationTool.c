/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = CyborgMigrationTool.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.MIGRATION.TOOL;
FILE.PATH      = <PROJECT>/src/CyborgMigrationTool.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

MAIN CYBORG MIGRATION TOOL ENTRY AND LOGIC.
AVIS DO NOT REMOVE */

#include "include/shared_inc.h"          /* pulls in defs.h, main.h, Cyborg_Rich, etc. */


#include "include/AdvancedSearchDialog.h"
#include "include/FileCopySelector.h"
#include "include/DirSelector.h"
#include "include/FileTypeSelector.h"
#include "include/Debug_Dialog.h"


/* ---------------------------------------------------------
   Append text to log edit (terminal-style)
--------------------------------------------------------- */
static void AppendLog(HWND hLog, const char *text)
{
    int len = GetWindowTextLengthA(hLog);
    SendMessageA(hLog, EM_SETSEL, (WPARAM)len, (LPARAM)len);
    SendMessageA(hLog, EM_REPLACESEL, FALSE, (LPARAM)text);
}

/* ---------------------------------------------------------
   Recursive directory scan
   (Basic implementation ignores most advanced fields for now;
    you can extend it to filter by params->fileTypes, etc.)
--------------------------------------------------------- */
static void ScanDir(const char *root, const char *sub, FILE *out, HWND hLog,
                    const ADV_SEARCH_PARAMS *params)
{
    char search[MAX_PATH];
    WIN32_FIND_DATAA fd;

    const char *filter = (params && params->searchFilter[0]) ? params->searchFilter : "*.*";

    if (sub && sub[0])
        wsprintfA(search, "%s\\%s\\%s", root, sub, filter);
    else
        wsprintfA(search, "%s\\%s", root, filter);

    HANDLE h = FindFirstFileA(search, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return;

    do {
        if (!lstrcmpA(fd.cFileName, ".") || !lstrcmpA(fd.cFileName, ".."))
            continue;

        char rel[MAX_PATH];
        if (sub && sub[0])
            wsprintfA(rel, "%s\\%s", sub, fd.cFileName);
        else
            wsprintfA(rel, "%s", fd.cFileName);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            ScanDir(root, rel, out, hLog, params);
        }
        else
        {
            fprintf(out, "%s\\%s\n", root, rel);
        }

    } while (FindNextFileA(h, &fd));

    FindClose(h);
}

/* ---------------------------------------------------------
   Public migration entry — called from main.c
--------------------------------------------------------- */


