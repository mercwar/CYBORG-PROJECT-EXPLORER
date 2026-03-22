/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = PopupEditor.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V06.00;
FILE.MODULE    = CYBORG.POPUP.EDITOR;
FILE.PATH      = <PROJECT>/src/PopupEditor.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

FULL-FEATURED POPUP EDITOR FOR MIGRATION RESULTS.
RICHEDIT50W. 10MB LIMIT. ALWAYS LOADS ECHO BUFFER.
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include "include/PopupEditor.h"
#include "include/FileCopySelector.h"
#include "include/Registry.h"
#include "include/Cyborg_Rich.h"
#include "include/DirSelector.h"
#include "include/FileTypeSelector.h"

extern WIZARD_STATE g_ws;
static HWND g_hEdit = NULL;

/* ---------------------------------------------------------
   DESTINATION FOLDER SELECTOR (GLOBAL, REGISTRY-BACKED)
--------------------------------------------------------- */
static void DestinationFolder_Select(HWND hwndOwner)
{
    BROWSEINFOA bi = {0};
    bi.hwndOwner = hwndOwner;
    bi.lpszTitle = "Select Destination Folder for Copy";
    bi.ulFlags   = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
    if (!pidl)
        return;

    char path[MAX_PATH] = {0};
    if (SHGetPathFromIDListA(pidl, path))
    {
        if (g_ws.destinationFolder)
            free(g_ws.destinationFolder);

        g_ws.destinationFolder = strdup(path);

        RegSaveDestinationFolder(g_ws.destinationFolder);

        SendMessageA(g_hEdit, EM_REPLACESEL, FALSE,
            (LPARAM)"[Destination Folder Set]\r\n");
        SendMessageA(g_hEdit, EM_REPLACESEL, FALSE, (LPARAM)path);
        SendMessageA(g_hEdit, EM_REPLACESEL, FALSE, (LPARAM)"\r\n\r\n");
    }

    LPMALLOC pMalloc = NULL;
    if (SUCCEEDED(SHGetMalloc(&pMalloc)))
    {
        pMalloc->lpVtbl->Free(pMalloc, pidl);
        pMalloc->lpVtbl->Release(pMalloc);
    }
}

/* ---------------------------------------------------------
   POPUP EDITOR WINDOW PROC
--------------------------------------------------------- */
static LRESULT CALLBACK PopupEditorProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        LoadRichEditLibrary();



        g_hEdit = CreateWindowExW(
            WS_EX_CLIENTEDGE,
            RICHEDIT_CLASS_50W,
            L"",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE |
            ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,
            0, 35, 0, 0,
            hwnd,
            NULL,
            ((LPCREATESTRUCT)lParam)->hInstance,
            NULL
        );

        SendMessageW(g_hEdit, EM_SETLIMITTEXT, 10 * 1024 * 1024, 0);

        HFONT hFont = CreateFontA(
            16, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            FIXED_PITCH | FF_MODERN,
            "Consolas"
        );
        SendMessageA(g_hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

        HMENU hMenu  = CreateMenu();
        HMENU hFile  = CreatePopupMenu();
        HMENU hTools = CreatePopupMenu();
        HMENU hView  = CreatePopupMenu();

        AppendMenuA(hFile, MF_STRING, 10001, "Close");

        AppendMenuA(hTools, MF_STRING, 10011, "Working Directory Selector...");
        AppendMenuA(hTools, MF_STRING, 10005, "File Type Selector...");
        AppendMenuA(hTools, MF_STRING, 10010, "File List Selector...");
        AppendMenuA(hTools, MF_SEPARATOR, 0, NULL);
        AppendMenuA(hTools, MF_STRING, 10020, "Select Destination Folder for Copy...");

        AppendMenuA(hView, MF_STRING, 10003, "Refresh Results");

        AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hFile,  "File");
        AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hTools, "Tools");
        AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hView,  "View");

        SetMenu(hwnd, hMenu);

        return 0;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);

        if (id == 10001)
        {
            DestroyWindow(hwnd);
            return 0;
        }

        if (id == 10011)
        {
            DirSelector_Open(hwnd, g_hEdit, &g_ws);
            return 0;
        }

        if (id == 10005)
        {
            FileTypeSelector_Open(hwnd, g_hEdit, &g_ws);
            return 0;
        }


if (id == 10010)
{
    FileProjectSelector_Main(hwnd, g_hEdit, &g_ws);

    return 0;
}



        if (id == 10020)
        {
            DestinationFolder_Select(hwnd);
            return 0;
        }

        if (id == 10003)
        {
            int len = GetWindowTextLengthA(g_hEdit);
            if (len > 0)
            {
                char *buf = malloc(len + 1);
                GetWindowTextA(g_hEdit, buf, len + 1);
                SetWindowTextA(g_hEdit, buf);
                free(buf);
            }
            return 0;
        }

        return 0;
    }

    case WM_SIZE:
        MoveWindow(g_hEdit, 0, 35, LOWORD(lParam), HIWORD(lParam) - 35, TRUE);
        return 0;

    case WM_DESTROY:
        g_hEdit = NULL;
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

/* ---------------------------------------------------------
   PUBLIC ENTRY POINT — ALWAYS LOADS ECHO BUFFER
   CURSOR STAYS AT TOP (OPTION A)
--------------------------------------------------------- */
HWND PopupEditor_Show(HWND hParent, const char *title, const char *content)
{
    const char *CLASS = "CYBORG_POPUP_EDITOR";

    WNDCLASSA wc = {0};
    wc.lpfnWndProc   = PopupEditorProc;
    wc.hInstance     = GetModuleHandleA(NULL);
    wc.lpszClassName = CLASS;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA(
        CLASS,
        title ? title : "Popup Editor",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        700, 500,
        hParent,
        NULL,
        wc.hInstance,
        NULL
    );

    if (g_hEdit)
    {
        const char *text = content ? content : "";
        SetWindowTextA(g_hEdit, text);

        /* Cursor stays at TOP (Option A) */
        SendMessageA(g_hEdit, EM_SETSEL, 0, 0);
        SendMessageA(g_hEdit, EM_SCROLLCARET, 0, 0);
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return hwnd;
}
