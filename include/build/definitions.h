#ifndef BUILD_DEFINITIONS_H
#define BUILD_DEFINITIONS_H

#include "array.h"

typedef FLO_DYNAMIC_ARRAY(char *) flo_da_charPtr;

typedef enum {
    FLO_BUILD_DEBUG,
    FLO_BUILD_PROFILING,
    FLO_BUILD_RELEASE,
    FLO_BUILD_NUM
} flo_BuildType;

char *flo_buildTypeToString(flo_BuildType status);

typedef struct {
    flo_da_charPtr commands;
    char *name;
    flo_BuildType buildType;
} flo_BuildCommand;

#endif
