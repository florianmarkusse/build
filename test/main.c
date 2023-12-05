#include "build/build.h"
#include "build/common.h"
#include <sys/mman.h>

#define CAP 1 << 15

pid_t runBuild(flo_Arena scratch, flo_BuildConfig *config) {
    flo_da_charPtr command = {0};

    flo_addConfiguration(config, &command, &scratch);

    *FLO_PUSH(&command, &scratch) = "example/main.c";

    flo_addPersonalStaticLib("util", "Release", &command, &scratch);
    flo_addCommonCFlags(&command, &scratch);
    flo_addCommonCVersion(&command, &scratch);

    return flo_runAsync(&command, scratch);
}

int main([[maybe_unused]] int argc, char **argv) {
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        FLO_INFO((FLO_STRING("-------------------------------------------------"
                             "-------------------------------\n")));
        FLO_INFO((FLO_STRING("Running flo/build\n")));
        FLO_INFO((FLO_STRING("-------------------------------------------------"
                             "-------------------------------\n")));
    }

    char *begin = mmap(NULL, CAP, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (begin == MAP_FAILED) {
        FLO_ERROR((FLO_STRING("Failed to allocate memory!\n")), FLO_FLUSH);
        return -1;
    }

    flo_Arena arena =
        (flo_Arena){.beg = begin, .cap = CAP, .end = begin + (ptrdiff_t)(CAP)};

    void *jmp_buf[5];
    if (__builtin_setjmp(jmp_buf)) {
        if (munmap(arena.beg, arena.cap) == -1) {
            FLO_FLUSH_AFTER(FLO_STDERR) {
                FLO_ERROR((FLO_STRING("Failed to unmap memory from arena!\n"
                                      "Arena Details:\n"
                                      "  beg: ")));
                FLO_ERROR(arena.beg);
                FLO_ERROR((FLO_STRING("\n end: ")));
                FLO_ERROR(arena.end);
                FLO_ERROR((FLO_STRING("\n cap: ")));
                FLO_ERROR(arena.cap);
                FLO_ERROR((FLO_STRING("\nZeroing Arena regardless.\n")));
            }
        }
        arena.beg = NULL;
        arena.end = NULL;
        arena.cap = 0;
        arena.jmp_buf = NULL;
        FLO_ERROR((FLO_STRING("OOM/overflow in arena!\n")), FLO_FLUSH);
        return -1;
    }
    arena.jmp_buf = jmp_buf;

    // Rebuild ???
    if (!flo_rebuild(argv[0], "test/main.c", arena)) {
        return -1;
    }

    flo_BuildConfig buildConfig = (flo_BuildConfig){
        .buildType = FLO_BUILD_DEBUG, .name = "build/example-main"};

    pid_t childProcessID = runBuild(arena, &buildConfig);
    if (childProcessID == -1) {
        return -1;
    }

    bool buildResult = flo_waitPid(childProcessID);

    FLO_FLUSH_AFTER(FLO_STDOUT) {
        FLO_INFO((FLO_STRING("-------------------------------------------------"
                             "-------------------------------\n")));
    }
    if (buildResult) {
        FLO_FLUSH_AFTER(FLO_STDOUT) {
            FLO_INFO((FLO_STRING("Build result: ")));
            flo_appendColor(FLO_COLOR_GREEN, FLO_STDOUT);
            FLO_INFO((FLO_STRING("success\n")));
            flo_appendColorReset(FLO_STDOUT);
        }
    } else {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR((FLO_STRING("Build result: ")));
            flo_appendColor(FLO_COLOR_RED, FLO_STDERR);
            FLO_ERROR((FLO_STRING("failure\n")));
            flo_appendColorReset(FLO_STDERR);
        }
    }
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        FLO_INFO((FLO_STRING("-------------------------------------------------"
                             "-------------------------------\n")));
    }

    return 0;
}
