/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = MigrationEngine.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V03.00;
FILE.MODULE    = CYBORG.MIGRATION.ENGINE;
FILE.PATH      = <PROJECT>/src/MigrationEngine.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

MIGRATION ENGINE FOR THE CYBORG MIGRATION WIZARD.
SCANS DIRECTORIES, COLLECTS DIRECTORY CHAIN, AND FILTERS FILES
BY CONFIGURED FILE TYPES.
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include "include/MigrationEngine.h"
extern WIZARD_STATE g_ws;

/* ---------------------------------------------------------
   INTERNAL DIRECTORY CHAIN ACCUMULATOR
--------------------------------------------------------- */

static DIR_ENTRY *g_dirList  = NULL;
static int        g_dirCount = 0;
static int        g_dirCap   = 0;

/* ---------------------------------------------------------
   APPEND TO MAIN LOG
--------------------------------------------------------- */

static void AppendLog(HWND hLog, const char *text)
{
    if (!hLog || !text)
        return;

    int len = GetWindowTextLengthA(hLog);
    SendMessageA(hLog, EM_SETSEL, len, len);
    SendMessageA(hLog, EM_REPLACESEL, FALSE, (LPARAM)text);
}

/* ---------------------------------------------------------
   ADD DIRECTORY TO GLOBAL CHAIN
--------------------------------------------------------- */

static void AddDirectoryToList(const char *path)
{
    if (!path || !path[0])
        return;

    if (g_dirCount >= g_dirCap)
    {
        int newCap = (g_dirCap == 0) ? 64 : g_dirCap * 2;
        DIR_ENTRY *newList = (DIR_ENTRY *)realloc(g_dirList, newCap * sizeof(DIR_ENTRY));
        if (!newList)
            return; /* out of memory – ignore */

        g_dirList = newList;
        g_dirCap  = newCap;
    }

    lstrcpynA(g_dirList[g_dirCount].path, path, MAX_PATH);
    g_dirCount++;
}

/* ---------------------------------------------------------
   LOWERCASE HELPER
--------------------------------------------------------- */

static void ToLowerInPlace(char *s)
{
    if (!s) return;
    while (*s)
    {
        if (*s >= 'A' && *s <= 'Z')
            *s = (char)(*s - 'A' + 'a');
        s++;
    }
}

/* ---------------------------------------------------------
   CHECK IF FILE TYPE IS ALLOWED
   params->fileTypes like:
   ".c;.h;.txt"  or  "c;h;txt"
--------------------------------------------------------- */

static int FileTypeAllowed(const ADV_SEARCH_PARAMS *params, const char *fileName)
{
    if (!params || params->fileTypes[0] == '\0')
        return 1; /* no filter => allow all */

    const char *dot = strrchr(fileName, '.');
    char extBuf[32];
    char filter[128];

    if (!dot || !dot[1])
        return 0; /* no extension => reject */

    lstrcpynA(extBuf, dot + 1, sizeof(extBuf));
    extBuf[sizeof(extBuf) - 1] = '\0';

    lstrcpynA(filter, params->fileTypes, sizeof(filter));
    filter[sizeof(filter) - 1] = '\0';

    if (!params->caseSensitive)
    {
        ToLowerInPlace(extBuf);
        ToLowerInPlace(filter);
    }

    /* TOKEN LOOP (portable, no strtok_s) */
    char *token = strtok(filter, ";,");
    while (token)
    {
        while (*token == ' ' || *token == '.')
            token++;

        size_t tlen = strlen(token);
        while (tlen > 0 &&
               (token[tlen - 1] == ' ' || token[tlen - 1] == '.'))
            token[--tlen] = '\0';

        if (tlen > 0 && _stricmp(token, extBuf) == 0)
            return 1;

        token = strtok(NULL, ";,");
    }

    return 0;
}

/* ---------------------------------------------------------
   RECURSIVE DIRECTORY SCAN
   - Collects directories into g_dirList
   - Logs and echoes ONLY files of allowed types
--------------------------------------------------------- */

static void ScanDir(const char *root,
                    const char *sub,
                    FILE *out,
                    HWND hLog,
                    const ADV_SEARCH_PARAMS *params,
                    MIGRATION_ECHO_CALLBACK echoFn,
                    void *echoUser)
{
    char path[MAX_PATH];
    char search[MAX_PATH];
    WIN32_FIND_DATAA fd;

    if (sub && sub[0])
        wsprintfA(path, "%s\\%s", root, sub);
    else
        wsprintfA(path, "%s", root);

    wsprintfA(search, "%s\\*.*", path);

    HANDLE hFind = FindFirstFileA(search, &fd);
    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do
    {
        if (lstrcmpA(fd.cFileName, ".") == 0 ||
            lstrcmpA(fd.cFileName, "..") == 0)
            continue;

        char full[MAX_PATH];
        wsprintfA(full, "%s\\%s", path, fd.cFileName);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            /* collect directory in chain */
            AddDirectoryToList(full);

            /* optional: log directory itself */
            {
                char lineDir[MAX_PATH * 2];
                wsprintfA(lineDir, "%s\\\r\n", full);
                AppendLog(hLog, lineDir);
                if (echoFn) echoFn(hLog, lineDir, echoUser);
                if (out) fprintf(out, "%s\\\n", full);
            }

            /* recurse into subdirectory */
            char nextSub[MAX_PATH];
            if (sub && sub[0])
                wsprintfA(nextSub, "%s\\%s", sub, fd.cFileName);
            else
                wsprintfA(nextSub, "%s", fd.cFileName);

            ScanDir(root, nextSub, out, hLog, params, echoFn, echoUser);
        }
        else
        {
            /* file: filter by allowed types */
            if (!FileTypeAllowed(params, fd.cFileName))
                continue;

            char line[MAX_PATH * 2];
            wsprintfA(line, "%s\r\n", full);

            AppendLog(hLog, line);
            if (echoFn)
                echoFn(hLog, line, echoUser);
            if (out)
                fprintf(out, "%s\n", full);
        }

    } while (FindNextFileA(hFind, &fd));

    FindClose(hFind);
}

/* ---------------------------------------------------------
   PUBLIC ENTRY POINT
--------------------------------------------------------- */
void RunMigration(HWND hLog, MIGRATION_ECHO_CALLBACK echoFn, void *echoUser)
{
    const char *root = g_ws.rootPath;
    ADV_SEARCH_PARAMS *params = &g_ws.searchParams;

    if (!root || !root[0])
        return;

    /* reset directory chain */
    if (g_dirList)
    {
        free(g_dirList);
        g_dirList  = NULL;
        g_dirCount = 0;
        g_dirCap   = 0;
    }

    FILE *out = NULL;
    if (g_ws.outputFile[0])
        out = fopen(g_ws.outputFile, "w");

    /* start scan at root, with empty 'sub' */
    ScanDir(root, "", out, hLog, params, echoFn, echoUser);

    if (out)
        fclose(out);

    /* commit directory chain into wizard state */
    g_ws.searchParams.dirList  = g_dirList;
    g_ws.searchParams.dirCount = g_dirCount;
}
