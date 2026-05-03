/* ---------------------------------------------------------------------------
   AIFVS-ARTIFACT — FILE: BOX_DEBUG.c
   ROLE: Sovereign Debug Window + Log Engine
   LAW:  AVIS-2026 — BOX Debug Layer
   --------------------------------------------------------------------------- */

#include <windows.h>
#include <wchar.h>
#include <stdio.h>


#include "../include/BOX_DEBUG.h"

/* ---------------------------------------------------------------------------
   FORWARD DECLARATIONS
   --------------------------------------------------------------------------- */

static void BOX_ShowRootPath(void);

/* ---------------------------------------------------------------------------
   INTERNAL STATE
   --------------------------------------------------------------------------- */

static HWND   g_boxWnd      = NULL;
static HWND   g_boxEdit     = NULL;
static int    g_initialized = 0;
static wchar_t g_rootPath[MAX_PATH] = {0};

#define ID_BOX_EDIT   2001
#define ID_BOX_COPY   2002
#define ID_BOX_CLEAR  2003
#define ID_BOX_ONTOP  2004

static int    g_boxAlwaysOnTop = 0;
static HBITMAP g_borderBmp     = NULL;
static HBRUSH g_btnBrush = NULL;

/* ---------------------------------------------------------------------------
   LOAD BACKGROUND IMAGE
   --------------------------------------------------------------------------- */

void BOX_SetRoot(void)
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);

    wchar_t* p = wcsrchr(exePath, L'\\');
    if (p) *p = L'\0';
    p = wcsrchr(exePath, L'\\');
    if (p) *p = L'\0';

    wcscpy(g_rootPath, exePath);
}

void BOX_LoadBorderImage(void)
{
    BOX_SetRoot();

    if (g_borderBmp)
        return;
if (!g_btnBrush)
    g_btnBrush = CreateSolidBrush(RGB(0, 0, 0));   // ← change color here

    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);

    wchar_t* p = wcsrchr(exePath, L'\\');
    if (p) *p = L'\0';

    wchar_t imgPath[MAX_PATH];
    swprintf(imgPath, MAX_PATH, L"%ls\\bg-rku.bmp", exePath);

    g_borderBmp = (HBITMAP)LoadImageW(
        NULL,
        imgPath,
        IMAGE_BITMAP,
        0, 0,
        LR_LOADFROMFILE
    );
}

/* ---------------------------------------------------------------------------
   WINDOW PROCEDURE
   --------------------------------------------------------------------------- */

static LRESULT CALLBACK BOX_DebugProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {


        /* CLIENT BACKGROUND ERASE: DO NOTHING (WE PAINT IN WM_PAINT) */
        case WM_ERASEBKGND:
            return 1;

        /* PAINT FULL BACKGROUND BMP INTO CLIENT */
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RECT rc;
            GetClientRect(hWnd, &rc);

            if (g_borderBmp)
            {
                HDC memDC = CreateCompatibleDC(hdc);
                HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, g_borderBmp);

                BITMAP bm;
                GetObject(g_borderBmp, sizeof(bm), &bm);

                StretchBlt(
                    hdc,
                    0, 0,
                    rc.right - rc.left,
                    rc.bottom - rc.top,
                    memDC,
                    0, 0,
                    bm.bmWidth,
                    bm.bmHeight,
                    SRCCOPY
                );

                SelectObject(memDC, oldBmp);
                DeleteDC(memDC);
            }
            else
            {
                FillRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }

            EndPaint(hWnd, &ps);
            return 0;
        }

        /* COMMAND HANDLING */
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case ID_BOX_COPY:
                    if (g_boxEdit)
                    {
                        SendMessageW(g_boxEdit, EM_SETSEL, 0, -1);
                        SendMessageW(g_boxEdit, WM_COPY, 0, 0);
						BOX_Debug_COPY(L"[CVBGOD] COPY");
                    }
                    break;

                case ID_BOX_CLEAR:
                    if (g_boxEdit)
                        SetWindowTextW(g_boxEdit, L"");
					BOX_Debug_COPY(L"[CVBGOD] CLEAR");
                    break;

                case ID_BOX_ONTOP:
                    g_boxAlwaysOnTop = !g_boxAlwaysOnTop;
                    SetWindowPos(
                        g_boxWnd,
                        g_boxAlwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
                        0, 0, 0, 0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
                    );
                    SendMessageW(
                        GetDlgItem(g_boxWnd, ID_BOX_ONTOP),
                        BM_SETCHECK,
                        g_boxAlwaysOnTop ? BST_CHECKED : BST_UNCHECKED,
                        0
                    );
					BOX_Debug_COPY(L"[CVBGOD] ONTOP TOGGLE");
                    break;
            }
            break;
case WM_CTLCOLORBTN:
{
    HDC hdc = (HDC)wParam;

    SetBkMode(hdc, OPAQUE);
    SetBkColor(hdc, RGB(0,0,0));          // background color
    SetTextColor(hdc, RGB(255,255,255));  // text color

    return (LRESULT)g_btnBrush;           // ← THIS IS THE BUTTON FACE
}



case WM_CTLCOLORSTATIC:
{
    HDC hdc = (HDC)wParam;
    SetBkMode(hdc, OPAQUE);
    SetBkColor(hdc, RGB(0,0,0));
    SetTextColor(hdc, RGB(255,255,255));
    return (LRESULT)GetStockObject(BLACK_BRUSH);
}
case WM_DRAWITEM:
{
    LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;

    // Background color
    HBRUSH brush = CreateSolidBrush(RGB(0,0,0));  // ← your button color
    FillRect(dis->hDC, &dis->rcItem, brush);
    DeleteObject(brush);

    // Text
    SetBkMode(dis->hDC, TRANSPARENT);
    SetTextColor(dis->hDC, RGB(255,255,255));

    wchar_t text[128];
    GetWindowTextW(dis->hwndItem, text, 128);

    DrawTextW(
        dis->hDC,
        text,
        -1,
        (RECT*)&dis->rcItem,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );

    return TRUE;
}

case WM_CTLCOLOREDIT:
{
    HDC hdc = (HDC)wParam;
    SetBkMode(hdc, OPAQUE);
    SetBkColor(hdc, RGB(0,0,0));
    SetTextColor(hdc, RGB(255,255,255));
    return (LRESULT)GetStockObject(BLACK_BRUSH);
}


        case WM_DESTROY:
            g_boxWnd  = NULL;
            g_boxEdit = NULL;
            g_boxAlwaysOnTop = 0;
            break;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

/* ---------------------------------------------------------------------------
   OPEN DEBUG WINDOW
   --------------------------------------------------------------------------- */

void BOX_Debug_Open(void)
{
    if (g_boxWnd)
        return;

    WNDCLASSW wc = {0};
    wc.lpfnWndProc   = BOX_DebugProc;
    wc.hInstance     = GetModuleHandleW(NULL);
    wc.lpszClassName = L"BOX_DEBUG_WINDOW";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);

    RegisterClassW(&wc);

    g_boxWnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        L"CVBGODLY BOX",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        150, 150, 900, 700,
        NULL, NULL, wc.hInstance, NULL
    );

    if (!g_boxWnd)
        return;

    BOX_LoadBorderImage();

    /* If BMP loaded, size window to it */
    if (g_borderBmp)
    {
        BITMAP bm;
        GetObject(g_borderBmp, sizeof(bm), &bm);
        SetWindowPos(g_boxWnd, NULL, 0, 0, bm.bmWidth, bm.bmHeight, SWP_NOMOVE | SWP_NOZORDER);
    }

    /* BLACK AREA (COORDS FROM YOUR DESIGN) */
const int BLACK_LEFT   = 80;
const int BLACK_TOP    = 250;
const int BLACK_RIGHT  = 925;
const int BLACK_BOTTOM = 765;



    const int BLACK_WIDTH  = BLACK_RIGHT  - BLACK_LEFT;
    const int BLACK_HEIGHT = BLACK_BOTTOM - BLACK_TOP;

    const int BTN_ROW_H    = 75;
    const int EDIT_HEIGHT  = BLACK_HEIGHT - BTN_ROW_H;

    /* EDIT CONTROL */
    g_boxEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL |
        ES_READONLY | WS_VSCROLL | WS_HSCROLL,
        BLACK_LEFT,
        BLACK_TOP,
        BLACK_WIDTH,
        EDIT_HEIGHT,
        g_boxWnd,
        (HMENU)ID_BOX_EDIT,
        wc.hInstance,
        NULL
    );

    /* BUTTONS */
    int btnY = BLACK_BOTTOM - BTN_ROW_H + 25;
    int btnH = 28;
    int btnW = 90;
    int spacing = 14;

 HWND TWIN=NULL;
TWIN = CreateWindowW(
    L"BUTTON", L"Copy",
    WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
    BLACK_LEFT,
    btnY,
    btnW, btnH,
    g_boxWnd,
    (HMENU)ID_BOX_COPY,
    wc.hInstance,
    NULL
);

TWIN = CreateWindowW(
    L"BUTTON", L"Clear",
    WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
    BLACK_LEFT + btnW + spacing,
    btnY,
    btnW, btnH,
    g_boxWnd,
    (HMENU)ID_BOX_CLEAR,
    wc.hInstance,
    NULL
);


     TWIN=CreateWindowW(
        L"BUTTON", L"Always On Top",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        BLACK_LEFT + (btnW + spacing) * 2,
        btnY,
        btnW + 40, btnH,
        g_boxWnd,
        (HMENU)ID_BOX_ONTOP,
        wc.hInstance,
        NULL
    );

    SendMessageW(
        GetDlgItem(g_boxWnd, ID_BOX_ONTOP),
        BM_SETCHECK,
        g_boxAlwaysOnTop ? BST_CHECKED : BST_UNCHECKED,
        0
    );

    BOX_ShowRootPath();
}

/* ---------------------------------------------------------------------------
   LOGGING
   --------------------------------------------------------------------------- */

void BOX_Debug_COPY(const wchar_t* msg)
{
    if (!g_boxWnd)
        BOX_Debug_Open();

    if (!g_boxEdit)
        return;

    int len = GetWindowTextLengthW(g_boxEdit);

    SendMessageW(g_boxEdit, EM_SETSEL, len, len);
    SendMessageW(g_boxEdit, EM_REPLACESEL, FALSE, (LPARAM)msg);
    SendMessageW(g_boxEdit, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
}

void BOX_Debug_Log(const wchar_t* msg)
{
    BOX_SetRoot();
    BOX_Debug_COPY(msg);

    wchar_t finalPath[MAX_PATH];
    swprintf(finalPath, MAX_PATH, L"%lsfiregem_debug.log", g_rootPath);

    DWORD disp = g_initialized ? OPEN_ALWAYS : CREATE_ALWAYS;

    HANDLE hFile = CreateFileW(
        finalPath,
        FILE_APPEND_DATA,
        FILE_SHARE_READ,
        NULL,
        disp,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD written = 0;

        if (!g_initialized)
        {
            unsigned short bom = 0xFEFF;
            WriteFile(hFile, &bom, 2, &written, NULL);
        }

        WriteFile(hFile, msg, (DWORD)(wcslen(msg) * sizeof(wchar_t)), &written, NULL);

        const wchar_t* newline = L"\r\n";
        WriteFile(hFile, newline, 2 * sizeof(wchar_t), &written, NULL);

        CloseHandle(hFile);
        g_initialized = 1;
    }
}

/* ---------------------------------------------------------------------------
   SHOW ROOT PATH
   --------------------------------------------------------------------------- */

static void BOX_ShowRootPath(void)
{
    if (!g_boxEdit)
        return;

    wchar_t msg[MAX_PATH + 64];
    swprintf(msg, MAX_PATH + 64, L"[ROOT] %ls\r\n", g_rootPath);

    int len = GetWindowTextLengthW(g_boxEdit);

    SendMessageW(g_boxEdit, EM_SETSEL, len, len);
    SendMessageW(g_boxEdit, EM_REPLACESEL, FALSE, (LPARAM)msg);
}
