#ifndef BUILD_BUILD_H
#define BUILD_BUILD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "definitions.h"
#include "log.h"
#include "memory/arena.h"
#include <stdbool.h>
#include <sys/wait.h>

#define FLO_APPEND_COMMAND(flo_da_charPtr, bufferType)                         \
    flo_appendColor(FLO_COLOR_MAGENTA, bufferType);                            \
    for (ptrdiff_t i = 0; i < (flo_da_charPtr)->len; i++) {                    \
        FLO_LOG((flo_da_charPtr)->buf[i], bufferType);                         \
        FLO_LOG((FLO_STRING(" ")), bufferType);                                \
    }                                                                          \
    flo_appendColorReset(bufferType);

bool flo_waitPid(pid_t pid);
pid_t flo_runAsync(flo_da_charPtr *command, flo_Arena scratch);
pid_t flo_runSync(flo_da_charPtr *command, flo_Arena scratch);

bool flo_rebuild(char *executableName, char *buildCodeFile, flo_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
