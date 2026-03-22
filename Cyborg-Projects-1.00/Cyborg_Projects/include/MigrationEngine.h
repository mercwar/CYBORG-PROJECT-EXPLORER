/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = MigrationEngine.h;
FILE.TYPE      = HEADER;
FILE.VERSION   = V03.00;
FILE.MODULE    = CYBORG.MIGRATION.ENGINE;
FILE.PATH      = <PROJECT>/include/MigrationEngine.h;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

HEADER FOR THE CYBORG MIGRATION ENGINE.
AVIS DO NOT REMOVE */

#ifndef CYBORG_MIGRATION_ENGINE_H
#define CYBORG_MIGRATION_ENGINE_H
#include "include/shared_inc.h"


/* echo callback used by migration engine to feed popup editor / log */
typedef void (*MIGRATION_ECHO_CALLBACK)(HWND hLog, const char *text, void *userData);

/* main entry point for running the migration */
void RunMigration(HWND hLog, MIGRATION_ECHO_CALLBACK echoFn, void *echoUser);

#endif /* CYBORG_MIGRATION_ENGINE_H */
