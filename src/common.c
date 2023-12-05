#include "build/common.h"
#include "log.h"
#include <stdio.h>

void flo_addConfiguration(flo_BuildConfig *config, flo_da_charPtr *command,
                          flo_Arena *perm) {
    *FLO_PUSH(command, perm) = "gcc";
    *FLO_PUSH(command, perm) = "-o";
    *FLO_PUSH(command, perm) = config->name;

    switch (config->buildType) {
    case FLO_BUILD_DEBUG: {
        *FLO_PUSH(command, perm) = "-O0";
        *FLO_PUSH(command, perm) = "-g3";
        *FLO_PUSH(command, perm) = "-DDEBUG";
        break;
    }
    case FLO_BUILD_PROFILING: {
        *FLO_PUSH(command, perm) = "-pg";
        *FLO_PUSH(command, perm) = "-O2";
        break;
    }
    case FLO_BUILD_RELEASE: {
        *FLO_PUSH(command, perm) = "-O3";
        break;
    }
    default: {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR((FLO_STRING("build type was set incorrectly: ")));
            FLO_ERROR(flo_buildTypeToString(config->buildType));
            FLO_ERROR((FLO_STRING(" ignoring...\n")));
        }
    }
    }
}

void flo_addCommonCFlags(flo_da_charPtr *command, flo_Arena *perm) {
    *FLO_PUSH(command, perm) = "-Wall";
    *FLO_PUSH(command, perm) = "-Wextra";
    *FLO_PUSH(command, perm) = "-Wconversion";
    *FLO_PUSH(command, perm) = "-Wno-sign-conversion";
    *FLO_PUSH(command, perm) = "-Wno-missing-field-initializers";
    *FLO_PUSH(command, perm) = "-Wdouble-promotion";
    *FLO_PUSH(command, perm) = "-Wvla";
    *FLO_PUSH(command, perm) = "-W";
}

void flo_addCommonCVersion(flo_da_charPtr *command, flo_Arena *perm) {
    *FLO_PUSH(command, perm) = "-std=gnu2x";
}

void flo_addPersonalStaticLib(char *lib, char *buildType,
                              flo_da_charPtr *command, flo_Arena *perm) {
    char *libPath = "libs/";
    ptrdiff_t libPathLen = strlen(libPath);

    ptrdiff_t libLen = strlen(libPath);
    ptrdiff_t buildTypeLen = strlen(buildType);

    ptrdiff_t includeLen =
        2 + libPathLen + libLen + 1 + 7 + 1; // -I libs/ util / include '\0'
    char *includePath = FLO_NEW(perm, char, includeLen);
    snprintf(includePath, includeLen, "-I%s%s/include", libPath, lib);
    *FLO_PUSH(command, perm) = includePath;

    ptrdiff_t libraryLen =
        2 + libPathLen + libLen + 1 + 5 + 1; // -L libs/ util / build '\0'
    char *libraryPath = FLO_NEW(perm, char, libraryLen);
    snprintf(libraryPath, libraryLen, "-L%s%s/build", libPath, lib);
    *FLO_PUSH(command, perm) = libraryPath;

    ptrdiff_t linkLen =
        2 + libLen + 1 + buildTypeLen + 1; // -l util - Release '\0'
    char *linkPath = FLO_NEW(perm, char, linkLen);
    snprintf(linkPath, linkLen, "-l%s-%s", lib, buildType);
    *FLO_PUSH(command, perm) = linkPath;
}
