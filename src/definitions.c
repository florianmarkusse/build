#include "build/definitions.h"

static char *buildTypeToString[FLO_BUILD_NUM] = {"Debug", "Profiling",
                                                 "Release"};

char *flo_buildTypeToString(flo_BuildType status) {
    if (status >= 0 && status < FLO_BUILD_NUM) {
        return buildTypeToString[status];
    }
    return "ERROR";
}
