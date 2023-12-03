#include "build/build.h"
#include <errno.h>

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

void finalizeCommand(flo_BuildCommand *command, flo_Arena *perm) {
    // Add stuff based on relase + generate output name and where
    // add NULL at the end

    *FLO_PUSH(command, perm) = NULL;
}

pid_t flo_runAsync(flo_da_charPtr *command, flo_Arena *perm) {
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
        *FLO_PUSH(command, perm) = NULL;

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

pid_t flo_runSync(flo_da_charPtr *command, flo_Arena *perm) {
    pid_t childPid = flo_runAsync(command, perm);
    if (childPid == -1) {
        return childPid;
    }
    if (!flo_waitPid(childPid)) {
        return -1;
    }
    return childPid;
}
