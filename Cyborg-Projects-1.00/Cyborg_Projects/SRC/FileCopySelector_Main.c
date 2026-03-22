/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = FileCopySelector_Main.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V02.00;
FILE.MODULE    = CYBORG.FILECOPY.SELECTOR;
FILE.PATH      = <PROJECT>/src/FileCopySelector_Main.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

FILE COPY SELECTOR ENGINE AND DIALOG.
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>

#include "include/shared_inc.h"
#include "include/FileCopySelector.h"
#include "include/Debug_Dialog.h"

/* ---------------------------------------------------------
   INTERNAL STATE
   --------------------------------------------------------- */
typedef struct _FILECOPY_STATE
{
    WIZARD_STATE *ws;
    HWND hOwner;
    HWND hEdit;

    HWND hDlg;
    HWND hSrcList;
    HWND hDstList;
    HWND hDestEdit;

    char destFolder[MAX_PATH];

} FILECOPY_STATE;

/* ---------------------------------------------------------
   LOGGING
   --------------------------------------------------------- */
static void FC_Log(FILECOPY_STATE *st, const char *fmt, ...)
{
    char buf[1024];
    va_list ap;

    va_start(ap, fmt);
    _vsnprintf(buf, sizeof(buf)-1, fmt, ap);
    buf[sizeof(buf)-1] = '\0';
    va_end(ap);

    DebugPrint("FileCopySelector: %s\n", buf);

    if (st->hEdit)
    {
        int len = GetWindowTextLengthA(st->hEdit);
        SendMessageA(st->hEdit, EM_SETSEL, len, len);
        SendMessageA(st->hEdit, EM_REPLACESEL, FALSE, (LPARAM)buf);
        SendMessageA(st->hEdit, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
    }
}

/* ---------------------------------------------------------
   POPULATE SOURCE LIST
   --------------------------------------------------------- */
static void FC_PopulateSourceList(FILECOPY_STATE *st)
{
    WIN32_FIND_DATAA ffd;
    char pattern[MAX_PATH];
    HANDLE hFind;

    SendMessageA(st->hSrcList, LB_RESETCONTENT, 0, 0);

    lstrcpynA(pattern, st->ws->workingDir, sizeof(pattern));
    PathAppendA(pattern, "*.*");

    FC_Log(st, "Scanning directory: %s", st->ws->workingDir);

    hFind = FindFirstFileA(pattern, &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        FC_Log(st, "Directory empty or inaccessible.");
        return;
    }

    do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        const char *dot = strrchr(ffd.cFileName, '.');
        if (!dot)
            continue;

        char ext[16];
        lstrcpynA(ext, dot, sizeof(ext));
        CharLowerA(ext);

        if (st->ws->selectedFileType[0])
        {
            char want[16];
            lstrcpynA(want, st->ws->selectedFileType, sizeof(want));
            CharLowerA(want);

            if (lstrcmpA(ext, want) != 0)
                continue;
        }

        SendMessageA(st->hSrcList, LB_ADDSTRING, 0, (LPARAM)ffd.cFileName);

    } while (FindNextFileA(hFind, &ffd));

    FindClose(hFind);
}

/* ---------------------------------------------------------
   MOVE ITEMS
   --------------------------------------------------------- */
static void FC_MoveSelected(HWND hSrc, HWND hDst)
{
    int count = SendMessageA(hSrc, LB_GETSELCOUNT, 0, 0);
    if (count <= 0)
        return;

    int *idx = malloc(sizeof(int) * count);
    SendMessageA(hSrc, LB_GETSELITEMS, count, (LPARAM)idx);

    for (int i = count - 1; i >= 0; --i)
    {
        char buf[MAX_PATH];
        SendMessageA(hSrc, LB_GETTEXT, idx[i], (LPARAM)buf);
        SendMessageA(hDst, LB_ADDSTRING, 0, (LPARAM)buf);
        SendMessageA(hSrc, LB_DELETESTRING, idx[i], 0);
    }

    free(idx);
}

/* ---------------------------------------------------------
   COPY FILES
   --------------------------------------------------------- */
static void FC_DoCopy(FILECOPY_STATE *st)
{
    int count = SendMessageA(st->hDstList, LB_GETCOUNT, 0, 0);
    if (count <= 0)
    {
        FC_Log(st, "No files selected.");
        return;
    }

    GetWindowTextA(st->hDestEdit, st->destFolder, sizeof(st->destFolder));
    if (!st->destFolder[0])
    {
        FC_Log(st, "Destination folder empty.");
        return;
    }

    FC_Log(st, "Copying %d file(s) to %s", count, st->destFolder);

    for (int i = 0; i < count; ++i)
    {
        char file[MAX_PATH];
        char src[MAX_PATH];
        char dst[MAX_PATH];

        SendMessageA(st->hDstList, LB_GETTEXT, i, (LPARAM)file);

        PathCombineA(src, st->ws->workingDir, file);
        PathCombineA(dst, st->destFolder, file);

        if (!CopyFileA(src, dst, FALSE))
            FC_Log(st, "FAILED: %s -> %s", src, dst);
        else
            FC_Log(st, "Copied: %s -> %s", src, dst);
    }
}

/* ---------------------------------------------------------
   RESET
   --------------------------------------------------------- */
static void FC_Reset(FILECOPY_STATE *st)
{
    SendMessageA(st->hSrcList, LB_RESETCONTENT, 0, 0);
    SendMessageA(st->hDstList, LB_RESETCONTENT, 0, 0);
    SetWindowTextA(st->hDestEdit, "");
    FC_PopulateSourceList(st);
}

/* ---------------------------------------------------------
   DIALOG PROC
   --------------------------------------------------------- */
static INT_PTR CALLBACK FileCopySelector_DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    FILECOPY_STATE *st = (FILECOPY_STATE*)GetWindowLongPtrA(hDlg, GWLP_USERDATA);

    switch (msg)
    {
    case WM_INITDIALOG:
        st = (FILECOPY_STATE*)lParam;
        st->hDlg      = hDlg;
        st->hSrcList  = GetDlgItem(hDlg, IDC_FC_SRC_LIST);
        st->hDstList  = GetDlgItem(hDlg, IDC_FC_DST_LIST);
        st->hDestEdit = GetDlgItem(hDlg, IDC_FC_DEST_EDIT);

        SetWindowLongPtrA(hDlg, GWLP_USERDATA, (LONG_PTR)st);

        FC_Log(st, "Init: workingDir=%s ext=%s",
               st->ws->workingDir,
               st->ws->selectedFileType[0] ? st->ws->selectedFileType : "<none>");

        FC_PopulateSourceList(st);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_FC_ADD:    FC_MoveSelected(st->hSrcList, st->hDstList); return TRUE;
        case IDC_FC_REMOVE: FC_MoveSelected(st->hDstList, st->hSrcList); return TRUE;
        case IDC_FC_COPY:   FC_DoCopy(st); return TRUE;
        case IDC_FC_RESET:  FC_Reset(st); return TRUE;
        case IDCANCEL:      EndDialog(hDlg, 0); return TRUE;
        }
        break;
    }

    return FALSE;
}

/* ---------------------------------------------------------
   PUBLIC ENTRY
   --------------------------------------------------------- */
void FileCopySelector_Main(HWND hOwner, HWND hEdit, WIZARD_STATE *ws)
{
    FILECOPY_STATE st;
    ZeroMemory(&st, sizeof(st));

    st.ws     = ws;
    st.hOwner = hOwner;
    st.hEdit  = hEdit;

    DialogBoxParamA(
        GetModuleHandleA(NULL),
        MAKEINTRESOURCEA(IDD_FILEPROJECTSELECTOR),
        hOwner,
        FileCopySelector_DialogProc,
        (LPARAM)&st
    );
}
