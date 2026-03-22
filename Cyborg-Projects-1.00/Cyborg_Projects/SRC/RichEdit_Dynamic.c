/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = RichEdit_Dynamic.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.RICHEDIT.DYNAMIC;
FILE.PATH      = <PROJECT>/src/RichEdit_Dynamic.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

DYNAMIC LOADER FOR RICHEDIT50W WITHOUT LINKING.
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN


#include "include/RichEdit_Dynamic.h"

HMODULE g_hMsftEdit = NULL;

void LoadRichEditLibrary(void)
{
    if (g_hMsftEdit)
        return;

    g_hMsftEdit = LoadLibraryA("msftedit.dll");
}
