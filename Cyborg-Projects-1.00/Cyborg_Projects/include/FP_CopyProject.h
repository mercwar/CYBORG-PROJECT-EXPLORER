//avis FP_CopyProject.h
/* AVIS DO NOT REMOVE
::AVIS FILEINFO V01.00::
FILE.NAME      = FP_CopyProject.h;
FILE.TYPE      = HEADER;
FILE.VERSION   = V01.00;
FILE.MODULE    = CYBORG.PROJECT.COPY;
FILE.PATH      = <PROJECT>/include/FP_CopyProject.h;
FILE.OWNER     = AI.FVS.AVIS.SYSTEM;
AVIS DO NOT REMOVE */

#ifndef FP_COPYPROJECT_H
#define FP_COPYPROJECT_H

#include <windows.h>
#include "include/shared_inc.h"

/* Forward declaration of FP_STATE from FileProjectSelector_Main.c */
typedef struct _FP_STATE FP_STATE;

/* Public entry for project copy subsystem */
void FP_CopyProject(FP_STATE *st);

#endif /* FP_COPYPROJECT_H */
