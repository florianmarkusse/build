cc -o first example/main.c -Ilibs/util/include -Llibs/util/build -lutil-Release
bear -- cc -o first example/main.c -Ilibs/util/include -Llibs/util/build -lutil-Release


bear -- cc -std=gnu2x -o builder test/main.c src/build.c -Ilibs/util/include -Llibs/util/build -lutil-Release -Iinclude -O0 -g3
