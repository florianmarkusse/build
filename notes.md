gcc -o first example/main.c -Ilibs/util/include -Llibs/util/build -lutil-Release
bear -- gcc -o first example/main.c -Ilibs/util/include -Llibs/util/build -lutil-Release


bear -- gcc -std=gnu2x -o builder test/main.c src/build.c -Ilibs/util/include -Llibs/util/build -lutil-Release -Iinclude -O0 -g3
bear --output build/compile_commands.json -- gcc -std=gnu2x -o build/builder test/main.c src/*.c -Ilibs/util/include -Llibs/util/build -lutil-Release -Iinclude -O0 -g3 -DDEBUG
