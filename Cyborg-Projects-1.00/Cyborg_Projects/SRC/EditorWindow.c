/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = EditorWindow.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V02.00;
FILE.MODULE    = CYBORG.EDITOR.WINDOW;
FILE.PATH      = <PROJECT>/src/EditorWindow.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

NOTEPAD-STYLE EDITOR WINDOW IMPLEMENTATION.
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include "include/EditorWindow.h"
static HWND g_hEditor = NULL;
static HFONT g_hFont = NULL;

/* ---------------------------------------------------------
   MENU COMMANDS
--------------------------------------------------------- */
#define ID_FILE_NEW        50001
#define ID_FILE_OPEN       50002
#define ID_FILE_SAVE       50003
#define ID_FILE_SAVEAS     50004
#define ID_FILE_EXIT       50005

#define ID_EDIT_COPY       51001
#define ID_EDIT_PASTE      51002
#define ID_EDIT_CUT        51003
#define ID_EDIT_SELECTALL  51004

#define ID_HELP_ABOUT      52001

/* ---------------------------------------------------------
   SAVE FILE HELPER
--------------------------------------------------------- */
static BOOL SaveFile(HWND hwnd, const char *path)
{
    int len = GetWindowTextLengthA(g_hEditor);
    char *buf = (char*)HeapAlloc(GetProcessHeap(), 0, len + 1);
    if (!buf) return FALSE;

    GetWindowTextA(g_hEditor, buf, len + 1);

    HANDLE hFile = CreateFileA(
        path, GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
    );

    if (hFile == INVALID_HANDLE_VALUE)
    {
        HeapFree(GetProcessHeap(), 0, buf);
        return FALSE;
    }

    DWORD written = 0;
    WriteFile(hFile, buf, len, &written, NULL);
    CloseHandle(hFile);

    HeapFree(GetProcessHeap(), 0, buf);
    return TRUE;
}

/* ---------------------------------------------------------
   FILE → SAVE AS
--------------------------------------------------------- */
static BOOL SaveAsDialog(HWND hwnd)
{
    char path[MAX_PATH] = {0};

    OPENFILENAMEA ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = hwnd;
    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
    ofn.lpstrFile   = path;
    ofn.nMaxFile    = MAX_PATH;
    ofn.Flags       = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameA(&ofn))
        return SaveFile(hwnd, path);

    return FALSE;
}

/* ---------------------------------------------------------
   MENU CREATION
--------------------------------------------------------- */
static HMENU CreateEditorMenu(void)
{
    HMENU hMenu = CreateMenu();
    HMENU hFile = CreatePopupMenu();
    HMENU hEdit = CreatePopupMenu();
    HMENU hHelp = CreatePopupMenu();

    AppendMenuA(hFile, MF_STRING, ID_FILE_NEW,    "New");
    AppendMenuA(hFile, MF_STRING, ID_FILE_OPEN,   "Open...");
    AppendMenuA(hFile, MF_STRING, ID_FILE_SAVE,   "Save");
    AppendMenuA(hFile, MF_STRING, ID_FILE_SAVEAS, "Save As...");
    AppendMenuA(hFile, MF_SEPARATOR, 0, NULL);
    AppendMenuA(hFile, MF_STRING, ID_FILE_EXIT,   "Exit");

    AppendMenuA(hEdit, MF_STRING, ID_EDIT_COPY,      "Copy");
    AppendMenuA(hEdit, MF_STRING, ID_EDIT_PASTE,     "Paste");
    AppendMenuA(hEdit, MF_STRING, ID_EDIT_CUT,       "Cut");
    AppendMenuA(hEdit, MF_STRING, ID_EDIT_SELECTALL, "Select All");

    AppendMenuA(hHelp, MF_STRING, ID_HELP_ABOUT, "About");

    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hFile, "File");
    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hEdit, "Edit");
    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hHelp, "Help");

    return hMenu;
}

/* ---------------------------------------------------------
   WINDOW PROC
--------------------------------------------------------- */
static LRESULT CALLBACK EditorProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        g_hEditor = CreateWindowExA(
            WS_EX_CLIENTEDGE,
            "EDIT",
            "",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE |
            ES_AUTOVSCROLL | ES_AUTOHSCROLL |
            WS_VSCROLL | WS_HSCROLL,
            0, 0, 0, 0,
            hwnd,
            NULL,
            ((LPCREATESTRUCTA)lParam)->hInstance,
            NULL
        );

        g_hFont = CreateFontA(
            16, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            FIXED_PITCH | FF_MODERN,
            "Consolas"
        );

        SendMessageA(g_hEditor, WM_SETFONT, (WPARAM)g_hFont, TRUE);

        SetMenu(hwnd, CreateEditorMenu());
        return 0;
    }

    case WM_SIZE:
        MoveWindow(g_hEditor, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_FILE_EXIT:
            DestroyWindow(hwnd);
            return 0;

        case ID_FILE_SAVEAS:
            SaveAsDialog(hwnd);
            return 0;

        case ID_EDIT_COPY:
            SendMessageA(g_hEditor, WM_COPY, 0, 0);
            return 0;

        case ID_EDIT_PASTE:
            SendMessageA(g_hEditor, WM_PASTE, 0, 0);
            return 0;

        case ID_EDIT_CUT:
            SendMessageA(g_hEditor, WM_CUT, 0, 0);
            return 0;

        case ID_EDIT_SELECTALL:
            SendMessageA(g_hEditor, EM_SETSEL, 0, -1);
            return 0;

        case ID_HELP_ABOUT:
            MessageBoxA(hwnd, "Cyborg Editor Window\nAVIS Notepad Classic", "About", MB_OK);
            return 0;
        }
        break;
    }

    case WM_DESTROY:
        if (g_hFont)
            DeleteObject(g_hFont);
        g_hFont = NULL;
        g_hEditor = NULL;
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

/* ---------------------------------------------------------
   PUBLIC ENTRY POINT
--------------------------------------------------------- */
void EditorWindow_Open(void)
{
    const char *CLASS = "CYBORG_EDITOR_WINDOW";

    WNDCLASSA wc = {0};
    wc.lpfnWndProc   = EditorProc;
    wc.hInstance     = GetModuleHandleA(NULL);
    wc.lpszClassName = CLASS;
    wc.hCursor       = LoadCursor(NULL, IDC_IBEAM);

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA(
        CLASS,
        "Cyborg Editor Window",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, NULL, wc.hInstance, NULL
    );

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
}
