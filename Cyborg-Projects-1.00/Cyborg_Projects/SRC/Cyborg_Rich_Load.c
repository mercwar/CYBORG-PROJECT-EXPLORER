/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = Cyborg_Rich_Load.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.RICHEDIT.WRAPPER;
FILE.PATH      = <PROJECT>/src/Cyborg_Rich_Load.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

LOAD RICHEDIT WRAPPER FOR MAIN.C COMPATIBILITY.
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "include/RichEdit_Dynamic.h"

void LoadRichEdit(void)
{
    LoadRichEditLibrary();
}
