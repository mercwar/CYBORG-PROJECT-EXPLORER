/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = DebugDialog.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.DEBUG.DIALOG;
FILE.PATH      = <PROJECT>/src/DebugDialog.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

INTERNAL DEBUG WINDOW FOR CYBORG PROJECTS.
NO EXTERNAL TOOLS REQUIRED.
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include "include/Debug_Dialog.h"

static HWND g_hDebugWnd  = NULL;
static HWND g_hDebugEdit = NULL;

static LRESULT CALLBACK DebugWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        g_hDebugEdit = CreateWindowExA(
            WS_EX_CLIENTEDGE,
            "EDIT",
            "",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL |
            ES_READONLY | WS_VSCROLL,
            0, 0, 0, 0,
            hwnd,
            (HMENU)1,
            ((LPCREATESTRUCT)lParam)->hInstance,
            NULL
        );
        return 0;
    }

    case WM_SIZE:
        MoveWindow(g_hDebugEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;

    case WM_CLOSE:
        ShowWindow(hwnd, SW_HIDE);
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

void DebugDialog_Open(void)
{
    if (g_hDebugWnd)
    {
        ShowWindow(g_hDebugWnd, SW_SHOW);
        SetForegroundWindow(g_hDebugWnd);
        return;
    }

    const char *CLASS = "CYBORG_DEBUG_WINDOW";

    WNDCLASSA wc = {0};
    wc.lpfnWndProc   = DebugWndProc;
    wc.hInstance     = GetModuleHandleA(NULL);
    wc.lpszClassName = CLASS;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);

    RegisterClassA(&wc);

    g_hDebugWnd = CreateWindowA(
        CLASS,
        "Cyborg Debug Output",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        600, 400,
        NULL,
        NULL,
        wc.hInstance,
        NULL
    );
}

void DebugPrint(const char *fmt, ...)
{
    if (!g_hDebugWnd)
        DebugDialog_Open();

    char buffer[2048];

    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);

    int len = GetWindowTextLengthA(g_hDebugEdit);
    SendMessageA(g_hDebugEdit, EM_SETSEL, len, len);
    SendMessageA(g_hDebugEdit, EM_REPLACESEL, FALSE, (LPARAM)buffer);
}
