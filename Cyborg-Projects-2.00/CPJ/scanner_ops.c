/* AVIS-ARTIFACT: SCANNER OPS */
/* FILENAME: scanner_ops.c */

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "scanner_ops.h"
#include "root.h"
#include "../BOX/include/BOX_DEBUG.h"

/* IDs mirrored from scanner.c */
#define LB_DIRS   1002
#define LB_FILES  1003
#define LB_SOURCE 1004

char CURRENT_PATH[MAX_PATH];

static void LogAnsiAsWide(const char *s)
{
    wchar_t wbuf[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, s, -1, wbuf, MAX_PATH);
    BOX_Debug_Log(wbuf);
}

void InitCurrentPath(const char *root)
{
    strcpy(CURRENT_PATH, root);
    BOX_Debug_Log(L"[SCANNER_OPS] CURRENT_PATH initialized");
    LogAnsiAsWide(CURRENT_PATH);
}

static void GetProjectFile(char *out)
{
    /* Single canonical buffer file */
    sprintf(out, "%sproject_source.txt", CPJ_ROOT);
    LogAnsiAsWide(out);
}

/* ---------------------- DRIVES ---------------------- */

void FillDrives(HWND hList)
{
    BOX_Debug_Log(L"[SCANNER_OPS] FillDrives()");
    SendMessageA(hList, LB_RESETCONTENT, 0, 0);

    DWORD mask = GetLogicalDrives();
    for (int i = 0; i < 26; i++) {
        if (mask & (1 << i)) {
            char drive[4];
            sprintf(drive, "%c:\\", 'A' + i);
            SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)drive);
        }
    }
}

/* ---------------------- DIRS ---------------------- */

void FillDirs(HWND hList, const char *root)
{
    BOX_Debug_Log(L"[SCANNER_OPS] FillDirs()");
    LogAnsiAsWide(root);

    SendMessageA(hList, LB_RESETCONTENT, 0, 0);

    /* Add ".." if not drive root */
    char drive[4];
    strncpy(drive, root, 3);
    drive[3] = '\0';

    if (_stricmp(root, drive) != 0)
        SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)"..");

    char path[MAX_PATH];
    WIN32_FIND_DATAA fd;

    sprintf(path, "%s*", root);

    HANDLE h = FindFirstFileA(path, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return;

    do {
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            strcmp(fd.cFileName, ".") &&
            strcmp(fd.cFileName, ".."))
        {
            SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)fd.cFileName);
        }
    } while (FindNextFileA(h, &fd));

    FindClose(h);
}

/* ---------------------- FILES ---------------------- */

void FillFiles(HWND hList, const char *root)
{
    BOX_Debug_Log(L"[SCANNER_OPS] FillFiles()");
    LogAnsiAsWide(root);

    SendMessageA(hList, LB_RESETCONTENT, 0, 0);

    char path[MAX_PATH];
    WIN32_FIND_DATAA fd;

    sprintf(path, "%s*", root);

    HANDLE h = FindFirstFileA(path, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return;

    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)fd.cFileName);
        }
    } while (FindNextFileA(h, &fd));

    FindClose(h);
}

/* ---------------------- LOAD FILE ---------------------- */

void AddSelectedFile(HWND hWnd)
{
    BOX_Debug_Log(L"[SCANNER_OPS] AddSelectedFile()");

    char file[256], full[MAX_PATH];

    int fi = SendDlgItemMessageA(hWnd, LB_FILES, LB_GETCURSEL, 0, 0);
    if (fi == LB_ERR) return;

    SendDlgItemMessageA(hWnd, LB_FILES, LB_GETTEXT, fi, (LPARAM)file);

    sprintf(full, "%s%s", CURRENT_PATH, file);

    LogAnsiAsWide(full);
    SendDlgItemMessageA(hWnd, LB_SOURCE, LB_ADDSTRING, 0, (LPARAM)full);
}

/* ---------------------- COMPILE ---------------------- */

void CompileSources(HWND hWnd)
{
    BOX_Debug_Log(L"[SCANNER_OPS] CompileSources()");

    char outpath[MAX_PATH];
    GetProjectFile(outpath);

    FILE *out = fopen(outpath, "ab");
    if (!out) {
        BOX_Debug_Log(L"[SCANNER_OPS] FAILED to open output file");
        return;
    }

    int count = SendDlgItemMessageA(hWnd, LB_SOURCE, LB_GETCOUNT, 0, 0);

    for (int i = 0; i < count; i++) {
        char path[512];
        SendDlgItemMessageA(hWnd, LB_SOURCE, LB_GETTEXT, i, (LPARAM)path);

        DWORD attr = GetFileAttributesA(path);
        if (attr == INVALID_FILE_ATTRIBUTES ||
            (attr & FILE_ATTRIBUTE_DIRECTORY))
            continue;

        BOX_Debug_Log(L"[SCANNER_OPS] Compiling file...");
        LogAnsiAsWide(path);

        FILE *in = fopen(path, "rb");
        if (!in) {
            BOX_Debug_Log(L"[SCANNER_OPS] FAILED to open file during compile");
            continue;
        }

        fprintf(out, "\n===== FILE: %s =====\n", path);

        char buf[4096];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), in)) > 0)
            fwrite(buf, 1, n, out);

        fclose(in);
    }

    fclose(out);

    BOX_Debug_Log(L"[SCANNER_OPS] Compile complete");
}

/* ---------------------- CLIPBOARD ---------------------- */

void CopyToClipboard(HWND hWnd)
{
    BOX_Debug_Log(L"[SCANNER_OPS] CopyToClipboard()");

    char outpath[MAX_PATH];
    GetProjectFile(outpath);

    FILE *f = fopen(outpath, "rb");
    if (!f) {
        BOX_Debug_Log(L"[SCANNER_OPS] FAILED to open compiled file for clipboard");
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size + 1);
    if (!hMem) {
        fclose(f);
        return;
    }

    char *buf = GlobalLock(hMem);
    if (!buf) {
        GlobalFree(hMem);
        fclose(f);
        return;
    }

    fread(buf, 1, size, f);
    buf[size] = 0;

    GlobalUnlock(hMem);
    fclose(f);

    if (!OpenClipboard(hWnd)) {
        GlobalFree(hMem);
        return;
    }

    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();

    BOX_Debug_Log(L"[SCANNER_OPS] Clipboard updated");
}

/* ---------------------- SNAP DIR ---------------------- */

void SnapshotDirectoryTree(HWND hWnd)
{
    BOX_Debug_Log(L"[SCANNER_OPS] SnapshotDirectoryTree()");
    LogAnsiAsWide(CURRENT_PATH);

    char outpath[MAX_PATH];
    GetProjectFile(outpath);

    FILE *out = fopen(outpath, "ab");
    if (!out) {
        BOX_Debug_Log(L"[SCANNER_OPS] FAILED to open snapshot file");
        return;
    }

    fprintf(out, "\n===== DIRECTORY SNAPSHOT: %s =====\n", CURRENT_PATH);

    char path[MAX_PATH];
    WIN32_FIND_DATAA fd;

    sprintf(path, "%s*", CURRENT_PATH);

    HANDLE h = FindFirstFileA(path, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        fclose(out);
        return;
    }

    do {
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            strcmp(fd.cFileName, ".") &&
            strcmp(fd.cFileName, ".."))
        {
            char entry[MAX_PATH];
            sprintf(entry, "%s%s\\", CURRENT_PATH, fd.cFileName);

            SendDlgItemMessageA(hWnd, LB_SOURCE, LB_ADDSTRING, 0, (LPARAM)entry);
            fprintf(out, "%s\n", entry);
        }
    } while (FindNextFileA(h, &fd));

    fclose(out);
    FindClose(h);

    BOX_Debug_Log(L"[SCANNER_OPS] SnapshotDirectoryTree complete");
}

/* ---------------------- SNAP FILES ---------------------- */

void SnapshotFilesList(HWND hWnd)
{
    BOX_Debug_Log(L"[SCANNER_OPS] SnapshotFilesList()");
    LogAnsiAsWide(CURRENT_PATH);

    char outpath[MAX_PATH];
    GetProjectFile(outpath);

    FILE *out = fopen(outpath, "ab");
    if (!out) {
        BOX_Debug_Log(L"[SCANNER_OPS] FAILED to open snapshot file");
        return;
    }

    fprintf(out, "\n===== FILE SNAPSHOT: %s =====\n", CURRENT_PATH);

    char path[MAX_PATH];
    WIN32_FIND_DATAA fd;

    sprintf(path, "%s*", CURRENT_PATH);

    HANDLE h = FindFirstFileA(path, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        fclose(out);
        return;
    }

    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            char entry[MAX_PATH];
            sprintf(entry, "%s%s", CURRENT_PATH, fd.cFileName);

            SendDlgItemMessageA(hWnd, LB_SOURCE, LB_ADDSTRING, 0, (LPARAM)entry);
            fprintf(out, "%s\n", entry);
        }
    } while (FindNextFileA(h, &fd));

    fclose(out);
    FindClose(h);

    BOX_Debug_Log(L"[SCANNER_OPS] SnapshotFilesList complete");
}

/* ---------------------- CLEAR ---------------------- */

void ClearSourceList(HWND hWnd)
{
    BOX_Debug_Log(L"[SCANNER_OPS] ClearSourceList()");

    SendDlgItemMessageA(hWnd, LB_SOURCE, LB_RESETCONTENT, 0, 0);

    char outpath[MAX_PATH];
    GetProjectFile(outpath);

    FILE *out = fopen(outpath, "wb");
    if (out) {
        fclose(out);
        BOX_Debug_Log(L"[SCANNER_OPS] project_source.txt truncated");
    } else {
        BOX_Debug_Log(L"[SCANNER_OPS] FAILED to truncate project_source.txt");
    }
}
