#!/bin/bash

if [ "$1" == "--clean" ]; then
    rm -rf build
    git clean -xdf
    git reset --hard HEAD

    if [ -f Makefile ]; then
        make distclean
    fi

    export CFLAGS="-O2 -fPIC -static-libgcc -static-libstdc++"

    if [ ! -f configure ]; then
        ./autogen.sh
    fi

    ./configure --enable-static=no --enable-shared=yes
fi

make -j8
g++ -shared -o accupos64.dll libass/.libs/*.o libass/x86/.libs/*.o accupos/.libs/*.o -Wl,--output-def=accupos64.def  -Wl,-static -static-libgcc -static-libstdc++ -Wl,--version-script=version-script.txt -Wl,--retain-symbols-file=retain-symbols-file.txt -lm -liconv -lfreetype -lbrotlidec -lharfbuzz -lfribidi -lfontconfig -lexpat -lfreetype -lpng -lbz2 -lz -lgdi32
strip -s accupos64.dll

mkdir -p test/.libs
gcc -std=gnu99 -Wall -O2 -fPIC -static-libgcc -static-libstdc++ -c -o test/.libs/test.o test/test.c
gcc -static -static-libgcc -static-libstdc++ -o accupos64.exe test/.libs/test.o accupos64.dll
strip -s accupos64.exe
