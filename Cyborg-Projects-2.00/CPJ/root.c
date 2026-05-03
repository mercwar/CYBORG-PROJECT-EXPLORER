/* AVIS-ARTIFACT: CPJ ROOT */
/* FILENAME: root.c */

#include <windows.h>
#include <shlwapi.h>
#include <string.h>

#include "root.h"
#include "../BOX/include/BOX_DEBUG.h"

char CPJ_ROOT[MAX_PATH];

static void LogAnsiAsWide(const char *s)
{
    wchar_t wbuf[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, s, -1, wbuf, MAX_PATH);
    BOX_Debug_Log(wbuf);
}

void GetExeRoot(void)
{
    BOX_Debug_Log(L"[ROOT] GetExeRoot() starting");

    if (GetModuleFileNameA(NULL, CPJ_ROOT, MAX_PATH) == 0) {
        BOX_Debug_Log(L"[ROOT] GetModuleFileNameA FAILED");
        return;
    }

    BOX_Debug_Log(L"[ROOT] Raw EXE path retrieved");
    LogAnsiAsWide(CPJ_ROOT);

    /* remove scanner.exe */
    PathRemoveFileSpecA(CPJ_ROOT);
    /* remove \bin */
    PathRemoveFileSpecA(CPJ_ROOT);

    size_t len = strlen(CPJ_ROOT);
    if (len > 0 && CPJ_ROOT[len - 1] != '\\')
        strcat(CPJ_ROOT, "\\");

    BOX_Debug_Log(L"[ROOT] Final CPJ_ROOT:");
    LogAnsiAsWide(CPJ_ROOT);

    BOX_Debug_Log(L"[ROOT] GetExeRoot() complete");
}
