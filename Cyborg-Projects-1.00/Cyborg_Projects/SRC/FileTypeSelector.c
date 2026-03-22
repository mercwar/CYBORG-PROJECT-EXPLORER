/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = FileTypeSelector.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V03.00;
FILE.MODULE    = CYBORG.FILETYPE.SELECTOR;
FILE.PATH      = <PROJECT>/src/FileTypeSelector.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

FILE TYPE SELECTOR FOR CYBORG MIGRATION WIZARD.
DYNAMICALLY ENUMERATES FILE TYPES FROM THE SELECTED WORKING DIRECTORY.
USES FIXED BUFFER selectedFileType[64] IN WIZARD_STATE.
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include "include/FileTypeSelector.h"

extern WIZARD_STATE g_ws;

/* ---------------------------------------------------------
   FIXED BUFFER ASSIGNMENT
--------------------------------------------------------- */
static void SetSelectedFileType(WIZARD_STATE *ws, const char *type)
{
    if (!ws || !type)
        return;

    lstrcpynA(ws->selectedFileType, type, sizeof(ws->selectedFileType));
}

/* ---------------------------------------------------------
   ENUMERATE UNIQUE FILE EXTENSIONS IN SELECTED DIRECTORY
--------------------------------------------------------- */
static void PopulateFileTypeList(HWND hList)
{
    DebugPrint("PopulateFileTypeList: workingDir=%s\r\n",
               g_ws.workingDir ? g_ws.workingDir : "(null)");

    if (!g_ws.workingDir || !g_ws.workingDir[0])
        return;

    WIN32_FIND_DATAA fd;
    char search[MAX_PATH];
    wsprintfA(search, "%s\\*.*", g_ws.workingDir);

    HANDLE hFind = FindFirstFileA(search, &fd);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        DebugPrint("  FindFirstFileA failed.\r\n");
        return;
    }

    char types[256][32];
    int typeCount = 0;

    do
    {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        const char *dot = strrchr(fd.cFileName, '.');
        if (!dot || !dot[1])
            continue;

        char ext[32];
        lstrcpynA(ext, dot, sizeof(ext));  /* include the dot */

        DebugPrint("  Found file: %s (ext=%s)\r\n", fd.cFileName, ext);

        /* check for duplicates */
        int exists = 0;
        for (int i = 0; i < typeCount; i++)
        {
            if (_stricmp(types[i], ext) == 0)
            {
                exists = 1;
                break;
            }
        }

        if (!exists && typeCount < 256)
        {
            lstrcpynA(types[typeCount], ext, sizeof(types[typeCount]));
            typeCount++;
        }

    } while (FindNextFileA(hFind, &fd));

    FindClose(hFind);

    DebugPrint("  Unique file types found: %d\r\n", typeCount);

    /* Populate listbox */
    for (int i = 0; i < typeCount; i++)
        SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)types[i]);

    /* Always add *.* at top */
    SendMessageA(hList, LB_INSERTSTRING, 0, (LPARAM)"*.*");
}

/* ---------------------------------------------------------
   DIALOG PROC
--------------------------------------------------------- */
static INT_PTR CALLBACK FileTypeSelectorDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND hList = NULL;

    switch (msg)
    {
    case WM_INITDIALOG:
    {
        hList = GetDlgItem(hDlg, IDC_FILETYPE_LIST);

        PopulateFileTypeList(hList);

        /* Pre-select current type, if any */
        if (g_ws.selectedFileType[0] != '\0')
        {
            int count = (int)SendMessageA(hList, LB_GETCOUNT, 0, 0);
            for (int i = 0; i < count; ++i)
            {
                char buf[64];
                SendMessageA(hList, LB_GETTEXT, i, (LPARAM)buf);
                if (lstrcmpiA(buf, g_ws.selectedFileType) == 0)
                {
                    SendMessageA(hList, LB_SETCURSEL, i, 0);
                    break;
                }
            }
        }

        return TRUE;
    }

    case WM_COMMAND:
    {
        int id   = LOWORD(wParam);
        int code = HIWORD(wParam);

        if (id == IDC_FILETYPE_OK && code == BN_CLICKED)
        {
            int sel = (int)SendMessageA(hList, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR)
            {
                char buf[64] = {0};
                SendMessageA(hList, LB_GETTEXT, sel, (LPARAM)buf);
                SetSelectedFileType(&g_ws, buf);
            }
            EndDialog(hDlg, IDOK);
            return TRUE;
        }

        if (id == IDC_FILETYPE_CANCEL && code == BN_CLICKED)
        {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }

        if (id == IDC_FILETYPE_LIST && code == LBN_DBLCLK)
        {
            int sel = (int)SendMessageA(hList, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR)
            {
                char buf[64] = {0};
                SendMessageA(hList, LB_GETTEXT, sel, (LPARAM)buf);
                SetSelectedFileType(&g_ws, buf);
            }
            EndDialog(hDlg, IDOK);
            return TRUE;
        }

        break;
    }

    case WM_CLOSE:
        EndDialog(hDlg, IDCANCEL);
        return TRUE;
    }

    return FALSE;
}

/* ---------------------------------------------------------
   PUBLIC ENTRY POINT
--------------------------------------------------------- */
void FileTypeSelector_Open(HWND hwndOwner, HWND hEcho, WIZARD_STATE *ws)
{
    (void)hEcho;
    (void)ws;

    DebugDialog_Open();

    DebugPrint(
        "FileTypeSelector_Open:\r\n"
        "  ws=%p\r\n"
        "  ws->workingDir=%s\r\n"
        "  ws->searchParams.dirList=%p\r\n"
        "  ws->searchParams.dirCount=%d\r\n\r\n",
        &g_ws,
        g_ws.workingDir ? g_ws.workingDir : "(null)",
        g_ws.searchParams.dirList,
        g_ws.searchParams.dirCount
    );

    HINSTANCE hInst = (HINSTANCE)GetModuleHandleA(NULL);

    DialogBoxParamA(
        hInst,
        MAKEINTRESOURCEA(IDD_FILETYPE_SELECTOR),
        hwndOwner,
        FileTypeSelectorDlgProc,
        0
    );
}
