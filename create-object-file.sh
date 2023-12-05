#!/bin/bash

mkdir -p build # create build directory if it doesn't exist
for file in src/*.c; do
	gcc -c "$file" -o "build/$(basename "$file" .c).o" -Iinclude -Ilibs/util/include -Llibs/util/build -lutil-Release
done

ar rcs build/libbuild-Debug.a build/*.o

rm build/*.o
