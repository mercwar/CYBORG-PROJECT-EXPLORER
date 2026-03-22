/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = AdvancedSearchDialog.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V02.00;
FILE.MODULE    = CYBORG.MIGRATION.ADVANCED.SEARCH;
FILE.PATH      = <PROJECT>/src/AdvancedSearchDialog.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

ADVANCED SEARCH POPUP (STUB).
AVIS DO NOT REMOVE */
#define WIN32_LEAN_AND_MEAN
#include "include/AdvancedSearchDialog.h"



BOOL AdvancedSearch_Show(HWND hParent, ADV_SEARCH_PARAMS *params)
{
    if (!params)
        return FALSE;

    /* Fill defaults */
    if (!params->searchFilter[0]) lstrcpyA(params->searchFilter, "*.*");
    if (!params->fileTypes[0])    lstrcpyA(params->fileTypes,    "*.*");
    if (!params->maxDepth[0])     lstrcpyA(params->maxDepth,     "all");

    params->includeHidden  = 0;
    params->includeSystem  = 0;
    params->caseSensitive  = 0;

    MessageBoxA(
        hParent,
        "Advanced search options applied:\n"
        " - Filter: *.*\n"
        " - Depth: all\n"
        "This is a stub dialog. Replace later with full UI.",
        "Advanced Search",
        MB_OK | MB_ICONINFORMATION
    );

    return TRUE;
}
