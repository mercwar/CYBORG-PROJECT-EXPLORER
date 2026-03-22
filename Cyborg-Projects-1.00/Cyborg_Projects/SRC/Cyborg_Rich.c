/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = Cyborg_Rich.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.RICHEDIT.WRAPPER;
FILE.PATH      = <PROJECT>/src/Cyborg_Rich.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

NOTEPAD-STYLE EDITOR WINDOW IMPLEMENTATION.
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN

#include "include/Cyborg_Rich.h"

HWND CyborgRich_Create(HWND hParent, int x, int y, int w, int h)
{
    LoadRichEditLibrary();

    return CreateWindowExA(
        WS_EX_CLIENTEDGE,
        RICHEDIT_CLASS_50A,
        "",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE |
        ES_AUTOVSCROLL | ES_AUTOHSCROLL |
        WS_VSCROLL | WS_HSCROLL,
        x, y, w, h,
        hParent,
        NULL,
        (HINSTANCE)GetModuleHandle(NULL),
        NULL
    );
}

void CyborgRich_SetText(HWND hEdit, const char *text)
{
    SendMessageA(hEdit, WM_SETTEXT, 0, (LPARAM)text);
}

void CyborgRich_Append(HWND hEdit, const char *text)
{
    int len = GetWindowTextLengthA(hEdit);
    SendMessageA(hEdit, EM_SETSEL, len, len);
    SendMessageA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)text);
}

void CyborgRich_Clear(HWND hEdit)
{
    SendMessageA(hEdit, WM_SETTEXT, 0, (LPARAM)"");
}
