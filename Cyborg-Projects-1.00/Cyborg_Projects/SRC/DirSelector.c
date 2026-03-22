/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = DirSelector.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V03.00;
FILE.MODULE    = CYBORG.DIR.SELECTOR;
FILE.PATH      = <PROJECT>/src/DirSelector.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

DIRECTORY SELECTOR FOR CYBORG MIGRATION WIZARD.
DISPLAYS PRE-SCANNED DIRECTORY CHAIN (DIR_ENTRY[]).
NO FILESYSTEM ENUMERATION.
AVIS DO NOT REMOVE */

#include "include/DirSelector.h"
extern WIZARD_STATE g_ws;
#include "include/Debug_Dialog.h"

/* ---------------------------------------------------------
   INTERNAL IDS (MUST MATCH defs.h / .rc)
--------------------------------------------------------- */
//#define IDC_DIR_LIST     8001
//#define IDC_DIR_OK       8002
//#define IDC_DIR_CANCEL   8003

/* ---------------------------------------------------------
   POPULATE DIRECTORY LIST FROM PRE-SCANNED CHAIN
--------------------------------------------------------- */

static void PopulateDirectoryList(HWND hList, const DIR_ENTRY *dirs, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (dirs[i].path[0] != '\0')
            SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)dirs[i].path);
    }
}

/* ---------------------------------------------------------
   SET WORKING DIRECTORY (heap-allocated)
--------------------------------------------------------- */

static void SetWorkingDirectory(WIZARD_STATE *ws, const char *path)
{
    if (!ws || !path)
        return;

    if (ws->workingDir)
        free(ws->workingDir);

    ws->workingDir = _strdup(path);
}

/* ---------------------------------------------------------
   DIALOG PROC
--------------------------------------------------------- */

static INT_PTR CALLBACK DirSelectorDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND       hList   = NULL;
    static DIR_ENTRY *dirList = NULL;
    static int        dirCount = 0;

    switch (msg)
    {
    case WM_INITDIALOG:
    {
        DIR_SELECTOR_CONTEXT *ctx = (DIR_SELECTOR_CONTEXT *)lParam;
        dirList  = ctx->dirs;
        dirCount = ctx->count;

        hList = GetDlgItem(hDlg, IDC_DIR_LIST);

        PopulateDirectoryList(hList, dirList, dirCount);

        /* Pre-select current workingDir if present */
        if (g_ws.workingDir)
        {
            int count = (int)SendMessageA(hList, LB_GETCOUNT, 0, 0);
            for (int i = 0; i < count; ++i)
            {
                char buf[MAX_PATH];
                SendMessageA(hList, LB_GETTEXT, i, (LPARAM)buf);
                if (lstrcmpiA(buf, g_ws.workingDir) == 0)
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

        if (id == IDC_DIR_OK && code == BN_CLICKED)
        {
            int sel = (int)SendMessageA(hList, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR)
            {
                char buf[MAX_PATH] = {0};
                SendMessageA(hList, LB_GETTEXT, sel, (LPARAM)buf);

                SetWorkingDirectory(&g_ws, buf);

                if (g_ws.hLogWindow)
                {
                    SendMessageA(g_ws.hLogWindow, EM_REPLACESEL, FALSE,
                                 (LPARAM)"[Working Directory Selected]\r\n");
                    SendMessageA(g_ws.hLogWindow, EM_REPLACESEL, FALSE, (LPARAM)buf);
                    SendMessageA(g_ws.hLogWindow, EM_REPLACESEL, FALSE, (LPARAM)"\r\n\r\n");
                }
            }

            EndDialog(hDlg, IDOK);
            return TRUE;
        }

        if (id == IDC_DIR_CANCEL && code == BN_CLICKED)
        {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }

        if (id == IDC_DIR_LIST && code == LBN_DBLCLK)
        {
            int sel = (int)SendMessageA(hList, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR)
            {
                char buf[MAX_PATH] = {0};
                SendMessageA(hList, LB_GETTEXT, sel, (LPARAM)buf);

                SetWorkingDirectory(&g_ws, buf);

                if (g_ws.hLogWindow)
                {
                    SendMessageA(g_ws.hLogWindow, EM_REPLACESEL, FALSE,
                                 (LPARAM)"[Working Directory Selected]\r\n");
                    SendMessageA(g_ws.hLogWindow, EM_REPLACESEL, FALSE, (LPARAM)buf);
                    SendMessageA(g_ws.hLogWindow, EM_REPLACESEL, FALSE, (LPARAM)"\r\n\r\n");
                }
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

void DirSelector_Open(HWND hwndOwner, HWND hEcho, WIZARD_STATE *ws)
{
    (void)hEcho;

    DebugDialog_Open();
    DebugPrint(
        "DirSelector_Open:\r\n"
        "  ws->searchParams.dirList  = %p\r\n"
        "  ws->searchParams.dirCount = %d\r\n\r\n",
        ws->searchParams.dirList,
        ws->searchParams.dirCount
    );

    HINSTANCE hInst = (HINSTANCE)GetModuleHandleA(NULL);

    DIR_SELECTOR_CONTEXT ctx;
    ctx.dirs  = ws->searchParams.dirList;
    ctx.count = ws->searchParams.dirCount;

    DialogBoxParamA(
        hInst,
        MAKEINTRESOURCEA(IDD_DIR_SELECTOR),
        hwndOwner,
        DirSelectorDlgProc,
        (LPARAM)&ctx
    );
}
