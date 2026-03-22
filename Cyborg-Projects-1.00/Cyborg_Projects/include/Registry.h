/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = Registry.h;
FILE.TYPE      = HEADER;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.REGISTRY;
FILE.PATH      = <PROJECT>/include/Registry.h;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;

REGISTRY HELPERS FOR CYBORG_PROJECTS.
AVIS DO NOT REMOVE */

#ifndef CYBORG_REGISTRY_H
#define CYBORG_REGISTRY_H
#include "include/shared_inc.h"

/* Root:
   HKCU\Software\Darkstar\Cyborg_Projects\Migration\Wizard
*/
#define CYBORG_REG_ROOT       "Software\\Darkstar\\Cyborg_Projects"
#define CYBORG_REG_MIGRATION  CYBORG_REG_ROOT "\\Migration\\Wizard"
#define CYBORG_REG_VALUE_DEST "DestinationFolder"

/* Destination folder persistence */
BOOL RegLoadDestinationFolder(char **ppszDest);
BOOL RegSaveDestinationFolder(const char *pszDest);

#endif /* CYBORG_REGISTRY_H */
