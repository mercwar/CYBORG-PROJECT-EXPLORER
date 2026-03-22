/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = FileProjectSelector_Main.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V01.01;
FILE.MODULE    = CYBORG.PROJECT.SELECTOR;
FILE.PATH      = <PROJECT>/src/FileProjectSelector_Main.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <shlwapi.h>

#include "include/shared_inc.h"
#include "include/FileProjectSelector.h"
#include "include/FP_CopyProject.h"
#include "include/Debug_Dialog.h"

typedef struct _FP_STATE {
    WIZARD_STATE *ws;
    HWND hOwner;
    HWND hEdit;

    HWND hDlg;
    HWND hSrc;
    HWND hDst;

    char manifestPath[MAX_PATH];

} FP_STATE;

/* ---------------------------------------------------------
   LOGGING
--------------------------------------------------------- */
static void FP_Log(FP_STATE *st, const char *fmt, ...)
{
    char buf[1024];
    va_list ap;

    va_start(ap, fmt);
    _vsnprintf(buf, sizeof(buf)-1, fmt, ap);
    buf[sizeof(buf)-1] = 0;
    va_end(ap);

    DebugPrint("ProjectSelector: %s\n", buf);
}

/* ---------------------------------------------------------
   POPULATE SOURCE LIST
--------------------------------------------------------- */
static void FP_Populate(FP_STATE *st)
{
    WIN32_FIND_DATAA ffd;
    char pattern[MAX_PATH];

    SendMessageA(st->hSrc, LB_RESETCONTENT, 0, 0);

    lstrcpynA(pattern, st->ws->workingDir, sizeof(pattern));
    PathAppendA(pattern, "*.*");

    HANDLE h = FindFirstFileA(pattern, &ffd);
    if (h == INVALID_HANDLE_VALUE)
        return;

    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        const char *dot = strrchr(ffd.cFileName, '.');
        if (!dot)
            continue;

        char ext[16];
        lstrcpynA(ext, dot, sizeof(ext));
        CharLowerA(ext);

        if (st->ws->selectedFileType[0]) {
            char want[16];
            lstrcpynA(want, st->ws->selectedFileType, sizeof(want));
            CharLowerA(want);

            if (lstrcmpA(ext, want) != 0)
                continue;
        }

        SendMessageA(st->hSrc, LB_ADDSTRING, 0, (LPARAM)ffd.cFileName);

    } while (FindNextFileA(h, &ffd));

    FindClose(h);
}

/* ---------------------------------------------------------
   MOVE ITEMS
--------------------------------------------------------- */
static void FP_Move(HWND hSrc, HWND hDst)
{
    int count = SendMessageA(hSrc, LB_GETSELCOUNT, 0, 0);
    if (count <= 0)
        return;

    int *idx = malloc(sizeof(int) * count);
    SendMessageA(hSrc, LB_GETSELITEMS, count, (LPARAM)idx);

    for (int i = count - 1; i >= 0; --i) {
        char buf[MAX_PATH];
        SendMessageA(hSrc, LB_GETTEXT, idx[i], (LPARAM)buf);
        SendMessageA(hDst, LB_ADDSTRING, 0, (LPARAM)buf);
        SendMessageA(hSrc, LB_DELETESTRING, idx[i], 0);
    }

    free(idx);
}

/* ---------------------------------------------------------
   NEW PROJECT
--------------------------------------------------------- */
static void FP_New(FP_STATE *st)
{
    st->manifestPath[0] = 0;
    SendMessageA(st->hDst, LB_RESETCONTENT, 0, 0);
    FP_Log(st, "New project created.");
}

/* ---------------------------------------------------------
   SELECT PROJECT (LOAD)
--------------------------------------------------------- */
static void FP_Select(FP_STATE *st)
{
    OPENFILENAMEA ofn = {0};
    char path[MAX_PATH] = {0};

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = st->hDlg;
    ofn.lpstrFilter = "Manifest (*.txt)\0*.txt\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;

    if (!GetOpenFileNameA(&ofn))
        return;

    lstrcpyA(st->manifestPath, path);

    FILE *f = fopen(path, "r");
    if (!f)
        return;

    SendMessageA(st->hDst, LB_RESETCONTENT, 0, 0);

    char line[MAX_PATH];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (line[0])
            SendMessageA(st->hDst, LB_ADDSTRING, 0, (LPARAM)line);
    }

    fclose(f);

    FP_Log(st, "Loaded project: %s", path);
}

/* ---------------------------------------------------------
   SAVE PROJECT (DIRECTORIES ONLY)
--------------------------------------------------------- */
static void FP_Save(FP_STATE *st)
{
    if (!st->manifestPath[0]) {
        OPENFILENAMEA ofn = {0};
        char path[MAX_PATH] = {0};

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = st->hDlg;
        ofn.lpstrFilter = "Manifest (*.txt)\0*.txt\0";
        ofn.lpstrFile = path;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_OVERWRITEPROMPT;

        if (!GetSaveFileNameA(&ofn))
            return;

        lstrcpyA(st->manifestPath, path);
    }

    FILE *f = fopen(st->manifestPath, "w");
    if (!f)
        return;

    int count = SendMessageA(st->hDst, LB_GETCOUNT, 0, 0);

    char dirs[512][MAX_PATH];
    int dirCount = 0;

    for (int i = 0; i < count; ++i) {
        char fileName[MAX_PATH];
        SendMessageA(st->hDst, LB_GETTEXT, i, (LPARAM)fileName);

        char full[MAX_PATH];
        lstrcpynA(full, st->ws->workingDir, sizeof(full));
        PathAppendA(full, fileName);

        char dirOnly[MAX_PATH];
        lstrcpynA(dirOnly, full, sizeof(dirOnly));
        PathRemoveFileSpecA(dirOnly);

        BOOL exists = FALSE;
        for (int d = 0; d < dirCount; ++d) {
            if (lstrcmpiA(dirs[d], dirOnly) == 0) {
                exists = TRUE;
                break;
            }
        }

        if (!exists && dirCount < 512) {
            lstrcpyA(dirs[dirCount++], dirOnly);
        }
    }

    for (int d = 0; d < dirCount; ++d) {
        fprintf(f, "%s\n", dirs[d]);
    }

    fclose(f);

    FP_Log(st, "Saved project directories: %s", st->manifestPath);
}

/* ---------------------------------------------------------
   DIALOG PROC
--------------------------------------------------------- */
static INT_PTR CALLBACK FP_DlgProc(HWND hDlg, UINT msg, WPARAM w, LPARAM l)
{
    FP_STATE *st = (FP_STATE*)GetWindowLongPtrA(hDlg, GWLP_USERDATA);

    switch (msg)
    {

    case WM_INITDIALOG:
        st = (FP_STATE*)l;
        st->hDlg = hDlg;
        st->hSrc = GetDlgItem(hDlg, IDC_FP_SRC_LIST);
        st->hDst = GetDlgItem(hDlg, IDC_FP_DST_LIST);

        SetWindowLongPtrA(hDlg, GWLP_USERDATA, (LONG_PTR)st);

        FP_Populate(st);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(w))
        {
        case IDC_FP_ADD:    FP_Move(st->hSrc, st->hDst); return TRUE;
        case IDC_FP_REMOVE: FP_Move(st->hDst, st->hSrc); return TRUE;
        case IDC_FP_NEW:    FP_New(st); return TRUE;
        case IDC_FP_SELECT: FP_Select(st); return TRUE;
        case IDC_FP_SAVE:   FP_Save(st); return TRUE;
        case IDC_FP_COPY:   FP_CopyProject(st); return TRUE;
        case IDCANCEL:      EndDialog(hDlg, 0); return TRUE;
        }
        break;
    }

    return FALSE;
}

/* ---------------------------------------------------------
   PUBLIC ENTRY
--------------------------------------------------------- */
void FileProjectSelector_Main(HWND hOwner, HWND hEdit, WIZARD_STATE *ws)
{
    FP_STATE st = {0};
    st.ws = ws;
    st.hOwner = hOwner;
    st.hEdit = hEdit;

    DialogBoxParamA(
        GetModuleHandleA(NULL),
        MAKEINTRESOURCEA(IDD_FILEPROJECTSELECTOR),
        hOwner,
        FP_DlgProc,
        (LPARAM)&st
    );
}
