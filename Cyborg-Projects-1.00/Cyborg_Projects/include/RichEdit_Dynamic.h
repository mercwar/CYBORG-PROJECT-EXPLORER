/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = RichEdit_Dynamic.h;
FILE.TYPE      = HEADER;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.RICHEDIT.DYNAMIC;
FILE.PATH      = <PROJECT>/include/RichEdit_Dynamic.h;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

DYNAMIC LOADER FOR RICHEDIT50W WITHOUT LINKING.
AVIS DO NOT REMOVE */

#ifndef RICHEDIT_DYNAMIC_H
#define RICHEDIT_DYNAMIC_H

#include <windows.h>   // REQUIRED for HMODULE

#ifdef __cplusplus
extern "C" {
#endif

extern HMODULE g_hMsftEdit;
void LoadRichEditLibrary(void);

#define RICHEDIT_CLASS_50A  "RICHEDIT50A"
#define RICHEDIT_CLASS_50W L"RICHEDIT50W"

#ifdef __cplusplus
}
#endif

#endif /* RICHEDIT_DYNAMIC_H */
