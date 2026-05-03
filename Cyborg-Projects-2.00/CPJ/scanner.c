/* AVIS-ARTIFACT: DARK-COM FILE SCANNER */
/* FILENAME: scanner.c */

#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>

#include "root.h"
#include "scanner_ops.h"
#include "../BOX/include/BOX_DEBUG.h"

#define ST_ROOTPATH   5000
#define LB_DRIVES     1001
#define LB_DIRS       1002
#define LB_FILES      1003
#define LB_SOURCE     1004

#define BTN_LOAD      2001
#define BTN_COMPILE   2002
#define BTN_CLIP      2003
#define BTN_CLOSE     2004

#define BTN_SNAP_DIR   3001
#define BTN_SNAP_FILES 3002
#define BTN_CLEAR      3003


/* ---------------------------------------------------------------------------
   FORCE INITIAL SELECTIONS SO LISTBOXES REFRESH
   BUT WITHOUT CHANGING CURRENT_PATH OR JUMPING TO C:\
   --------------------------------------------------------------------------- */
void ForceInitialSelections(HWND hWnd)
{
    HWND hDrives = GetDlgItem(hWnd, LB_DRIVES);
    HWND hDirs   = GetDlgItem(hWnd, LB_DIRS);
    HWND hFiles  = GetDlgItem(hWnd, LB_FILES);

    BOX_Debug_Log(L"[SCANNER] ForceInitialSelections()");

    int driveCount = (int)SendMessageA(hDrives, LB_GETCOUNT, 0, 0);
    if (driveCount > 0) {
        SendMessageA(hDrives, LB_SETCURSEL, 0, 0);
        PostMessageA(hWnd, WM_COMMAND,
            MAKELONG(LB_DRIVES, LBN_DBLCLK),
            (LPARAM)hDrives);
    }

    int dirCount = (int)SendMessageA(hDirs, LB_GETCOUNT, 0, 0);
    if (dirCount > 0) {
        SendMessageA(hDirs, LB_SETCURSEL, 0, 0);
        PostMessageA(hWnd, WM_COMMAND,
            MAKELONG(LB_DIRS, LBN_DBLCLK),
            (LPARAM)hDirs);
    }

    int fileCount = (int)SendMessageA(hFiles, LB_GETCOUNT, 0, 0);
    if (fileCount > 0) {
        SendMessageA(hFiles, LB_SETCURSEL, 0, 0);
        PostMessageA(hWnd, WM_COMMAND,
            MAKELONG(LB_FILES, LBN_DBLCLK),
            (LPARAM)hFiles);
    }

    SetWindowTextA(GetDlgItem(hWnd, ST_ROOTPATH), CURRENT_PATH);
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {

    case WM_CREATE:

        InitCurrentPath(CPJ_ROOT);

        CreateWindowA("STATIC", CURRENT_PATH,
            WS_VISIBLE | WS_CHILD,
            10, 10, 800, 20,
            hWnd, (HMENU)ST_ROOTPATH, NULL, NULL);

        /* Drives */
        CreateWindowA("LISTBOX", NULL,
            WS_VISIBLE|WS_CHILD|LBS_NOTIFY|WS_VSCROLL|WS_BORDER,
            10, 40, 200, 250,
            hWnd, (HMENU)LB_DRIVES, NULL, NULL);

        /* Directories */
        CreateWindowA("LISTBOX", NULL,
            WS_VISIBLE|WS_CHILD|LBS_NOTIFY|WS_VSCROLL|WS_BORDER,
            220, 40, 300, 250,
            hWnd, (HMENU)LB_DIRS, NULL, NULL);

        /* Files */
        CreateWindowA("LISTBOX", NULL,
            WS_VISIBLE|WS_CHILD|LBS_NOTIFY|WS_VSCROLL|WS_BORDER,
            530, 40, 300, 250,
            hWnd, (HMENU)LB_FILES, NULL, NULL);

        /* Source list */
        CreateWindowA("LISTBOX", NULL,
            WS_VISIBLE|WS_CHILD|WS_VSCROLL|WS_HSCROLL|WS_BORDER,
            10, 300, 820, 400,
            hWnd, (HMENU)LB_SOURCE, NULL, NULL);

        /* Buttons */
        CreateWindowA("BUTTON", "Load File", WS_VISIBLE|WS_CHILD,
            850, 40, 250, 40, hWnd, (HMENU)BTN_LOAD, NULL, NULL);

        CreateWindowA("BUTTON", "Compile SOURCE", WS_VISIBLE|WS_CHILD,
            850, 90, 250, 40, hWnd, (HMENU)BTN_COMPILE, NULL, NULL);

        CreateWindowA("BUTTON", "TO Clipboard", WS_VISIBLE|WS_CHILD,
            850, 140, 250, 40, hWnd, (HMENU)BTN_CLIP, NULL, NULL);

        CreateWindowA("BUTTON", "View IN BOX", WS_VISIBLE|WS_CHILD,
            850, 190, 250, 40, hWnd, (HMENU)4001, NULL, NULL);

        CreateWindowA("BUTTON", "Snap SHOT Dir", WS_VISIBLE|WS_CHILD,
            850, 240, 250, 40, hWnd, (HMENU)BTN_SNAP_DIR, NULL, NULL);

        CreateWindowA("BUTTON", "Snap SHOT Files", WS_VISIBLE|WS_CHILD,
            850, 290, 250, 40, hWnd, (HMENU)BTN_SNAP_FILES, NULL, NULL);

        CreateWindowA("BUTTON", "Clear LIST", WS_VISIBLE|WS_CHILD,
            850, 340, 250, 40, hWnd, (HMENU)BTN_CLEAR, NULL, NULL);

        CreateWindowA("BUTTON", "Close SCANNER", WS_VISIBLE|WS_CHILD,
            850, 390, 250, 40, hWnd, (HMENU)BTN_CLOSE, NULL, NULL);

        /* INITIAL POPULATION — ROOT SAFE */
        FillDrives(GetDlgItem(hWnd, LB_DRIVES));
        FillDirs(GetDlgItem(hWnd, LB_DIRS), CURRENT_PATH);
        FillFiles(GetDlgItem(hWnd, LB_FILES), CURRENT_PATH);
ForceInitialSelections(hWnd);


         //   SetWindowTextA(GetDlgItem(hWnd, ST_ROOTPATH), CURRENT_PATH);
        

        break;


    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        /* VIEW BUTTON */
        case 4001:
        {
            BOX_Debug_Log(L"[SCANNER] VIEW pressed");

            if (!OpenClipboard(hWnd)) break;

            HANDLE hData = GetClipboardData(CF_TEXT);
            if (!hData) { CloseClipboard(); break; }

            char *clip = (char*)GlobalLock(hData);
            if (!clip) { CloseClipboard(); break; }

            /* normalize CRLF */
            int rawLen = (int)strlen(clip);
            char *norm = (char*)malloc(rawLen * 2 + 2);
            int p = 0;

            for (int i = 0; i < rawLen; i++) {
                if (clip[i] == '\n') {
                    norm[p++] = '\r';
                    norm[p++] = '\n';
                } else {
                    norm[p++] = clip[i];
                }
            }
            norm[p] = 0;

            /* convert to wide */
            int wlen = MultiByteToWideChar(CP_ACP, 0, norm, -1, NULL, 0);
            wchar_t *wmsg = (wchar_t*)malloc(wlen * sizeof(wchar_t));
            MultiByteToWideChar(CP_ACP, 0, norm, -1, wmsg, wlen);

            BOX_Debug_COPY(wmsg);

            free(wmsg);
            free(norm);
            GlobalUnlock(hData);
            CloseClipboard();

            BOX_Debug_Log(L"[SCANNER] VIEW sent to BOX_Debug_COPY");
        }
        break;

        /* DRIVE CHANGE */
        case LB_DRIVES:
            if (HIWORD(wParam) == LBN_SELCHANGE) {
                char drive[4];
                int i = SendDlgItemMessageA(hWnd, LB_DRIVES, LB_GETCURSEL, 0, 0);
                if (i == LB_ERR) break;

                SendDlgItemMessageA(hWnd, LB_DRIVES, LB_GETTEXT, i, (LPARAM)drive);

                strcpy(CURRENT_PATH, drive);
                FillDirs(GetDlgItem(hWnd, LB_DIRS), CURRENT_PATH);
                FillFiles(GetDlgItem(hWnd, LB_FILES), CURRENT_PATH);
                SetWindowTextA(GetDlgItem(hWnd, ST_ROOTPATH), CURRENT_PATH);
            }
            break;

        /* DIR CHANGE + DOUBLE CLICK */
        case LB_DIRS:
            if (HIWORD(wParam) == LBN_SELCHANGE) {
                char dir[256], temp[MAX_PATH];

                int di = SendDlgItemMessageA(hWnd, LB_DIRS, LB_GETCURSEL, 0, 0);
                if (di == LB_ERR) break;

                SendDlgItemMessageA(hWnd, LB_DIRS, LB_GETTEXT, di, (LPARAM)dir);

                if (strcmp(dir, "..") == 0) {
                    char parent[MAX_PATH];
                    strcpy(parent, CURRENT_PATH);
                    PathRemoveBackslashA(parent);
                    PathRemoveFileSpecA(parent);
                    strcat(parent, "\\");
                    FillFiles(GetDlgItem(hWnd, LB_FILES), parent);
                } else {
                    sprintf(temp, "%s%s\\", CURRENT_PATH, dir);
                    FillFiles(GetDlgItem(hWnd, LB_FILES), temp);
                }
            }

            if (HIWORD(wParam) == LBN_DBLCLK) {
                char dir[256];
                int di = SendDlgItemMessageA(hWnd, LB_DIRS, LB_GETCURSEL, 0, 0);
                if (di == LB_ERR) break;

                SendDlgItemMessageA(hWnd, LB_DIRS, LB_GETTEXT, di, (LPARAM)dir);

                if (strcmp(dir, "..") == 0) {
                    PathRemoveBackslashA(CURRENT_PATH);
                    PathRemoveFileSpecA(CURRENT_PATH);
                    strcat(CURRENT_PATH, "\\");
                } else {
                    strcat(CURRENT_PATH, dir);
                    strcat(CURRENT_PATH, "\\");
                }

                SetWindowTextA(GetDlgItem(hWnd, ST_ROOTPATH), CURRENT_PATH);

                FillDirs(GetDlgItem(hWnd, LB_DIRS), CURRENT_PATH);
                FillFiles(GetDlgItem(hWnd, LB_FILES), CURRENT_PATH);
            }
            break;

        case BTN_LOAD:     AddSelectedFile(hWnd); break;
        case BTN_SNAP_DIR: SnapshotDirectoryTree(hWnd); break;
        case BTN_SNAP_FILES: SnapshotFilesList(hWnd); break;
        case BTN_CLEAR:    ClearSourceList(hWnd); break;
        case BTN_COMPILE:  CompileSources(hWnd); break;
        case BTN_CLIP:     CopyToClipboard(hWnd); break;
        case BTN_CLOSE:    PostQuitMessage(0); break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProcA(hWnd, msg, wParam, lParam);
}


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
    GetExeRoot();

    WNDCLASSA wc = {0};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = "DC_SCANNER";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassA(&wc)) {
        MessageBoxA(NULL, "RegisterClassA failed", "Error", MB_ICONERROR);
        return 0;
    }

HWND hWnd = CreateWindowA(
    "DC_SCANNER",
    "CVBGOD'S Cyborg-Projects AI.AVIS-FVS",
    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
    CW_USEDEFAULT, CW_USEDEFAULT,
    1150, 750,   // BIGGER WINDOW
    NULL, NULL, hInst, NULL
);

    if (!hWnd) {
        MessageBoxA(NULL, "CreateWindowA failed", "Error", MB_ICONERROR);
        return 0;
    }

    ShowWindow(hWnd, nShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return (int)msg.wParam;
}
