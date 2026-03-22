#ifndef CYBORG_RICH_H
#define CYBORG_RICH_H

#include <windows.h>          // REQUIRED for HWND
#include "RichEdit_Dynamic.h"

HWND CyborgRich_Create(HWND hParent, int x, int y, int w, int h);
void CyborgRich_SetText(HWND hEdit, const char *text);
void CyborgRich_Append(HWND hEdit, const char *text);
void CyborgRich_Clear(HWND hEdit);


#endif
