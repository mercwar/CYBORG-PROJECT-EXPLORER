/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = defs.h;
FILE.TYPE      = HEADER;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.PROJECTS.DEFS;
FILE.PATH      = <PROJECT>/include/defs.h;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

GLOBAL RESOURCE IDENTIFIER DEFINITIONS.
AVIS DO NOT REMOVE */

#ifndef CYBORG_PROJECTS_DEFS_H
#define CYBORG_PROJECTS_DEFS_H

/* ---------------------------------------------------------
   MAIN WINDOW / APPLICATION COMMANDS
   --------------------------------------------------------- */

/* Main menu commands (used in main.c) */
#define ID_MENU_FILE_EXIT          1000    /* "File -> Exit" */
#define ID_MENU_TOOLS_EDITOR       1001    /* "Tools -> Editor" */

/* Reserved range for future menu commands */
#define ID_MENU_RESERVED_START     1050
#define ID_MENU_RESERVED_END       1099


/* ---------------------------------------------------------
   MAIN WINDOW CONTROLS
   --------------------------------------------------------- */

/* Controls referenced in main.c */
#define ID_EDIT_LOG                1100    /* Main log edit control */
#define ID_BUTTON_NEXT             1101    /* Migration / Next action button */
#define ID_BUTTON_RESET            1102    /* Reset / Clear state button */

/* Reserved range for future main window controls */
#define ID_MAINCTRL_RESERVED_START 1150
#define ID_MAINCTRL_RESERVED_END   1199


/* ---------------------------------------------------------
   FILE TYPE SELECTOR DIALOG
   --------------------------------------------------------- */

/* Dialog resource ID (from cyborg_projects.rc) */
#define IDD_FILETYPE_SELECTOR      3003

/* Controls inside IDD_FILETYPE_SELECTOR */
#define IDC_FILETYPE_LIST          3000
#define IDC_FILETYPE_OK            3001
#define IDC_FILETYPE_CANCEL        3002

/* Reserved range for FileTypeSelector expansion */
#define IDC_FILETYPE_RESERVED_START 3050
#define IDC_FILETYPE_RESERVED_END   3099


/* ---------------------------------------------------------
   DIRECTORY SELECTOR DIALOG
   --------------------------------------------------------- */

/* Dialog resource ID (from cyborg_projects.rc) */
#define IDD_DIR_SELECTOR           3103

/* Controls inside IDD_DIR_SELECTOR */
#define IDC_DIR_LIST               3100
#define IDC_DIR_OK                 3101
#define IDC_DIR_CANCEL             3102

/* Reserved range for DirSelector expansion */
#define IDC_DIR_RESERVED_START     3150
#define IDC_DIR_RESERVED_END       3199


/* ---------------------------------------------------------
   FILE COPY SELECTOR DIALOG
   --------------------------------------------------------- */

/* Dialog resource ID (from cyborg_projects.rc) */
#define IDD_FILECOPYSELECTOR       5000

/* Controls inside IDD_FILECOPYSELECTOR */
#define IDC_FC_SRC_LIST            5001   /* Source file list */
#define IDC_FC_DST_LIST            5002   /* Selected file list */
#define IDC_FC_ADD                 5003   /* Move -> */
#define IDC_FC_REMOVE              5004   /* Move <- */
#define IDC_FC_DEST_EDIT           5005   /* Destination folder edit */
#define IDC_FC_BROWSE              5006   /* "..." folder picker */
#define IDC_FC_COPY                5007   /* Copy action */
#define IDC_FC_RESET               5008   /* Reset selection */

/* Reserved range for FileCopySelector expansion */
#define IDC_FC_RESERVED_START      5050
#define IDC_FC_RESERVED_END        5099


/* ---------------------------------------------------------
   FUTURE: DEBUG DIALOG / ADVANCED SEARCH / EDITOR WINDOW
   --------------------------------------------------------- */
/*
   When you finalize DebugDialog.rc, AdvancedSearchDialog.rc,
   EditorWindow.rc, or PopupEditor.rc, their IDs should be
   added here in their own ceremonial ranges:

   Example layout (NOT active until you command it):

   #define IDD_DEBUGDIALOG         4000
   #define IDC_DEBUG_OUTPUT        4001
   #define IDC_DEBUG_CLEAR         4002
   #define IDC_DEBUG_CLOSE         4003

   #define IDD_ADVSEARCH           4100
   ... etc ...
*/

/* Reserved global ranges for future dialogs */
#define IDD_RESERVED_START         6000
#define IDD_RESERVED_END           6999

#define IDD_FILEPROJECTSELECTOR   5200

#define IDC_FP_SRC_LIST           5201
#define IDC_FP_DST_LIST           5202
#define IDC_FP_ADD                5203
#define IDC_FP_REMOVE             5204
#define IDC_FP_NEW                5205
#define IDC_FP_SELECT             5206
#define IDC_FP_SAVE               5207

#define ID_MENU_TOOLS_SELECTPROJECT   1200
#define ID_MENU_TOOLS_COPYPROJECT     1201

#endif /* CYBORG_PROJECTS_DEFS_H */
