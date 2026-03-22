/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = Registry.c;
FILE.TYPE      = SOURCE;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.REGISTRY;
FILE.PATH      = <PROJECT>/src/Registry.c;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

REGISTRY HELPERS FOR CYBORG_PROJECTS.
AVIS DO NOT REMOVE */

#define WIN32_LEAN_AND_MEAN
#include "include/Registry.h"
static BOOL OpenOrCreateCyborgKey(HKEY *phKey)
{
    HKEY hKey = NULL;
    LONG r = RegCreateKeyExA(
        HKEY_CURRENT_USER,
        CYBORG_REG_MIGRATION,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ | KEY_WRITE,
        NULL,
        &hKey,
        NULL
    );
    if (r != ERROR_SUCCESS)
        return FALSE;

    *phKey = hKey;
    return TRUE;
}

BOOL RegLoadDestinationFolder(char **ppszDest)
{
    if (!ppszDest) return FALSE;
    *ppszDest = NULL;

    HKEY hKey = NULL;
    LONG r = RegOpenKeyExA(
        HKEY_CURRENT_USER,
        CYBORG_REG_MIGRATION,
        0,
        KEY_READ,
        &hKey
    );
    if (r != ERROR_SUCCESS)
        return FALSE;

    char  buffer[MAX_PATH * 4];
    DWORD type = 0;
    DWORD size = sizeof(buffer);

    r = RegQueryValueExA(
        hKey,
        CYBORG_REG_VALUE_DEST,
        NULL,
        &type,
        (LPBYTE)buffer,
        &size
    );
    RegCloseKey(hKey);

    if (r != ERROR_SUCCESS || type != REG_SZ || size == 0)
        return FALSE;

    char *dst = (char*)malloc(size);
    if (!dst) return FALSE;

    memcpy(dst, buffer, size);
    *ppszDest = dst;
    return TRUE;
}

BOOL RegSaveDestinationFolder(const char *pszDest)
{
    if (!pszDest || !*pszDest) return FALSE;

    HKEY hKey = NULL;
    if (!OpenOrCreateCyborgKey(&hKey))
        return FALSE;

    DWORD len = (DWORD)(strlen(pszDest) + 1);

    LONG r = RegSetValueExA(
        hKey,
        CYBORG_REG_VALUE_DEST,
        0,
        REG_SZ,
        (const BYTE*)pszDest,
        len
    );

    RegCloseKey(hKey);
    return (r == ERROR_SUCCESS);
}
