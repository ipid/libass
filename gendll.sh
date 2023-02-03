#!/bin/bash

rm -rf build
make distclean

export CFLAGS="-fPIC -static-libgcc -static-libstdc++"

./configure --enable-static=no --enable-shared=yes

make -C libass libass.la

g++ -shared -o libass-9.dll libass/.libs/*.o libass/x86/.libs/*.o -Wl,-static -static-libgcc -static-libstdc++ -Wl,--output-def,libass-9.def -Wl,--out-implib=libass.dll.a -Wl,--version-script=version-script.txt -Wl,--retain-symbols-file=retain-symbols-file.txt -lm -liconv -lfreetype -lbrotlidec -lharfbuzz -lfribidi -lfontconfig -lexpat -lfreetype -lpng -lbz2 -lz -lgdi32

'/c/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.31.31103/bin/HostX64/x64/lib' '/def:libass-9.def' '/machine:x64' '/out:libass-9.lib'
