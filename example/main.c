#include "log.h"

// flo_Command flo_addCommands(flo_Command command, flo_String command);

int testVla(int value) {
    int arr[value];
    arr[0] = 4;
    return arr[0];
}

int main() {
    //   flo_Command command;
    //   command = flo_addCommands(command, "cc");
    //   command = flo_addCommands(command, "-o mydick");
    //   command = flo_addCommands(command, "example.c");

    flo_String someString = FLO_STRING("things");

    testVla(3);

    FLO_INFO("nerdddddddddddd\n", FLO_FLUSH);

    return 0;
}
