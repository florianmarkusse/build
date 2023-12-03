#ifndef BUILD_COMMON_H
#define BUILD_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "definitions.h"

void flo_addCommonCFlags(flo_da_charPtr *command, flo_Arena *perm);
void flo_addCommonCVersion(flo_da_charPtr *command, flo_Arena *perm);
void flo_addPersonalStaticLib(char *lib, char *buildType,
                              flo_da_charPtr *command, flo_Arena *perm);
void flo_setBuildDebug(flo_da_charPtr *command, flo_Arena *perm);
void flo_setBuildProfiling(flo_da_charPtr *command, flo_Arena *perm);
void flo_setBuildRelease(flo_da_charPtr *command, flo_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
