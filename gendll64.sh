#!/bin/bash

if [ "$1" == "--clean" ]; then
    rm -rf build
    git stash --all

    if [ -f Makefile ]; then
        make distclean
    fi

    export CFLAGS="-fPIC -static -static-libgcc -static-libstdc++"

    if [ ! -f configure ]; then
        ./autogen.sh
    fi

    ./configure --prefix=/usr/i686-w64-mingw32/ --build=x86_64-linux-gnu --host=x86_64-w64-mingw32 --enable-static=yes --enable-shared=no
fi

make -j8
x86_64-w64-mingw32-g++ -shared -o accupos64.dll accupos/*.o libass/*.o libass/x86/*.o -Wl,--output-def=accupos64.def -Wl,-static -static -static-libgcc -static-libstdc++ -Wl,--version-script=version-script.txt -Wl,--retain-symbols-file=retain-symbols-file.txt -lfontconfig -lexpat -lfreetype -lharfbuzz -lfribidi -lunibreak -liconv -lpng -lz -lgdi32
x86_64-w64-mingw32-strip -s accupos64.dll

x86_64-w64-mingw32-gcc -std=gnu99 -Wall -O2 -fPIC -static -static-libgcc -static-libstdc++ -c -o test/test.o test/test.c
x86_64-w64-mingw32-gcc -static -static-libgcc -static-libstdc++ -o accupos64.exe test/test.o accupos64.dll
x86_64-w64-mingw32-strip -s accupos64.exe
