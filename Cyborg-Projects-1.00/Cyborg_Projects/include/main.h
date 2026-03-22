/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = main.h;
FILE.TYPE      = HEADER;
FILE.VERSION   = V03.00;
FILE.MODULE    = CYBORG.MAIN.SYSTEM;
FILE.PATH      = <PROJECT>/include/main.h;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

ROOT HEADER FOR THE CYBORG MIGRATION WIZARD.
AVIS DO NOT REMOVE */

#ifndef CYBORG_MAIN_H
#define CYBORG_MAIN_H
#define WIN32_LEAN_AND_MEAN
#include "include/shared_inc.h"
#include "include/FolderPicker.h"


/* ---------------------------------------------------------
   WIZARD STEPS
--------------------------------------------------------- */
typedef enum _WIZARD_STEP
{
    WIZ_STEP_SELECT_FOLDER = 0,
    WIZ_STEP_CONFIG_SEARCH = 1,
    WIZ_STEP_RUN_MIGRATION = 2,
    WIZ_STEP_VIEW_RESULTS  = 3
} WIZARD_STEP;

/* ---------------------------------------------------------
   ADVANCED SEARCH PARAMETERS
--------------------------------------------------------- */
typedef struct {
    char path[MAX_PATH];
} DIR_ENTRY;


typedef struct _ADV_SEARCH_PARAMS
{
    char searchFilter[128];
    char fileTypes[128];
    char maxDepth[16];

    int includeHidden;
    int includeSystem;
    int caseSensitive;

    /* directory chain from scan engine */
    DIR_ENTRY *dirList;
    int dirCount;

} ADV_SEARCH_PARAMS;

typedef struct {
    DIR_ENTRY *dirs;
    int count;
} DIR_SELECTOR_CONTEXT;


/* ---------------------------------------------------------
   WIZARD STATE (GLOBAL MIGRATION CONTEXT)
--------------------------------------------------------- */
typedef struct _WIZARD_STATE
{
    HWND hLogWindow;

    char *workingDir;

    char selectedFileType[64];

    char *destinationFolder;

    ADV_SEARCH_PARAMS searchParams;

    char rootPath[MAX_PATH];
    char outputFile[MAX_PATH];

    char *echoBuffer;
    int   echoSize;
    int   echoUsed;

} WIZARD_STATE;

/* ---------------------------------------------------------
   SUBSYSTEM HEADERS
--------------------------------------------------------- */
#include "include/FolderPicker.h"
#include "include/AdvancedSearchDialog.h"
#include "include/MigrationEngine.h"
#include "include/PopupEditor.h"
#include "include/EditorWindow.h"

#endif /* CYBORG_MAIN_H */
