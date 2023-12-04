#include "build/build.h"
#include "build/common.h"
#include "file/path.h"
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

bool flo_waitPid(pid_t pid) {
    while (true) {
        int wstatus = 0;
        if (waitpid(pid, &wstatus, 0) < 0) {
            FLO_FLUSH_AFTER(FLO_STDERR) {
                FLO_ERROR((FLO_STRING("Could not wait on pid: ")));
                FLO_ERROR(pid, FLO_NEWLINE);
                FLO_ERROR((FLO_STRING("Error code: ")));
                FLO_ERROR(strerror(errno), FLO_NEWLINE);
            }

            return false;
        }

        if (WIFEXITED(wstatus)) {
            int exit_status = WEXITSTATUS(wstatus);
            if (exit_status != 0) {
                FLO_FLUSH_AFTER(FLO_STDERR) {
                    FLO_ERROR((FLO_STRING("Command exited with exit code: ")));
                    FLO_ERROR(exit_status, FLO_NEWLINE);
                }

                return false;
            }

            return true;
        }

        if (WIFSIGNALED(wstatus)) {
            FLO_FLUSH_AFTER(FLO_STDERR) {
                FLO_ERROR((FLO_STRING("Command process was terminated by: ")));
                FLO_ERROR(strsignal(WTERMSIG(wstatus)), FLO_NEWLINE);
            }

            return false;
        }
    }
}

pid_t flo_runAsync(flo_da_charPtr *command, flo_Arena scratch) {
    pid_t childProcess = fork();

    if (childProcess < 0) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR((FLO_STRING("Child process not created!\n")));
            FLO_ERROR(strerror(errno), FLO_NEWLINE);
        }
        return -1;
    }

    if (childProcess == 0) {
        FLO_FLUSH_AFTER(FLO_STDOUT) {
            FLO_INFO((FLO_STRING("Attempting to run: ")));
            FLO_APPEND_COMMAND(command, FLO_STDOUT);
            FLO_INFO((FLO_STRING("\n")));
        }

        *FLO_PUSH(command, &scratch) = NULL;

        if (execvp(command->buf[0], command->buf) < 0) {
            FLO_FLUSH_AFTER(FLO_STDERR) {
                FLO_ERROR((FLO_STRING("Child process with command: ")));

                FLO_APPEND_COMMAND(command, FLO_STDERR);

                flo_appendColor(FLO_COLOR_RED, FLO_STDERR);
                FLO_ERROR((FLO_STRING("failed with error: ")));
                flo_appendColorReset(FLO_STDERR);
                FLO_ERROR(strerror(errno), FLO_NEWLINE);
            }

            return -1;
        }
    }

    return childProcess;
}

pid_t flo_runSync(flo_da_charPtr *command, flo_Arena scratch) {
    pid_t childPid = flo_runAsync(command, scratch);
    if (childPid == -1) {
        return childPid;
    }
    if (!flo_waitPid(childPid)) {
        return -1;
    }
    return childPid;
}

time_t getLastChange(char *path) {
    struct stat file_stat;
    if (stat(path, &file_stat) == 0) {
        return file_stat.st_mtime;
    } else {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR((FLO_STRING("Could not retrieve time changed of ")));
            FLO_ERROR(path);
            FLO_ERROR((FLO_STRING(" : ")));
            FLO_ERROR(strerror(errno), FLO_NEWLINE);
        }
        return -1;
    }
}

bool renameFile(char *oldFile, char *newFile) {
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        FLO_INFO((FLO_STRING("Renaming ")));
        FLO_INFO(oldFile);
        FLO_INFO((FLO_STRING(" to ")));
        FLO_INFO(newFile, FLO_NEWLINE);
    }
    if (rename(oldFile, newFile) != 0) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR((FLO_STRING("Could not rename ")));
            FLO_ERROR(oldFile);
            FLO_ERROR((FLO_STRING(" to ")));
            FLO_ERROR(newFile);
            FLO_ERROR((FLO_STRING(" : ")));
            FLO_ERROR(strerror(errno), FLO_NEWLINE);
        }
        return false;
    }
    return true;
}

bool runRebuild(flo_Arena scratch, char *binaryName, char *buildCodeFile) {
    flo_BuildConfig config = {.name = binaryName, .buildType = FLO_BUILD_DEBUG};
    flo_da_charPtr command = {0};

    *FLO_PUSH(&command, &scratch) = "bear";
    *FLO_PUSH(&command, &scratch) = "--output";
    *FLO_PUSH(&command, &scratch) = "build/compile_commands.json";
    *FLO_PUSH(&command, &scratch) = "--";
    flo_addConfiguration(&config, &command, &scratch);

    // TODO remove this need once we are actually building a static build lib.
    *FLO_PUSH(&command, &scratch) = "src/build.c";
    *FLO_PUSH(&command, &scratch) = "src/common.c";
    *FLO_PUSH(&command, &scratch) = "src/definitions.c";
    *FLO_PUSH(&command, &scratch) = buildCodeFile;
    *FLO_PUSH(&command, &scratch) = "-Iinclude";

    flo_addPersonalStaticLib("util", "Release", &command, &scratch);
    flo_addCommonCFlags(&command, &scratch);
    flo_addCommonCVersion(&command, &scratch);

    return flo_runSync(&command, scratch) != -1;
}

bool flo_rebuild(char *executableName, char *buildCodeFile, flo_Arena scratch) {
    flo_createPath(FLO_STRING("build/"), scratch);

    // TODO add build type check into this one!!!
    time_t buildTime = getLastChange(executableName);
    time_t sourceTime = getLastChange(buildCodeFile);
    if (buildTime < 0 || sourceTime < 0) {
        return false;
    }

    // TODO remove true
    if (true || sourceTime > buildTime) {
        FLO_INFO((FLO_STRING("Going to rebuild from source\n")), FLO_FLUSH);

        ptrdiff_t oldBinaryLen =
            strlen(executableName) + 4 + 1; // executableName + .old + '\0'
        char *oldBinary = FLO_NEW(&scratch, char, oldBinaryLen);
        snprintf(oldBinary, oldBinaryLen, "%s%s", executableName, ".old");

        if (!renameFile(executableName, oldBinary)) {
            return false;
        }

        if (!runRebuild(scratch, executableName, buildCodeFile)) {
            FLO_ERROR(
                (FLO_STRING("Rebuilding failed, trying to roll back rename\n")),
                FLO_FLUSH);
            renameFile(oldBinary, executableName);
            return false;
        }

        FLO_INFO((FLO_STRING("Removing old binary\n")), FLO_FLUSH);
        if (remove(oldBinary) != 0) {
            FLO_FLUSH_AFTER(FLO_STDERR) {
                FLO_ERROR((FLO_STRING("Could not remove ")));
                FLO_ERROR(oldBinary);
                FLO_ERROR((FLO_STRING(" : ")));
                FLO_ERROR(strerror(errno), FLO_NEWLINE);
            }
        }
    } else {
        FLO_INFO((FLO_STRING("No need to rebuild\n")), FLO_FLUSH);
    }

    FLO_FLUSH_AFTER(FLO_STDOUT) {
        FLO_INFO((FLO_STRING("Completed rebuilding flo/build\n")));
        FLO_INFO((FLO_STRING("-------------------------------------------------"
                             "-------------------------------\n")));
    }

    return true;
}
