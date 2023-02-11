#!/bin/bash

function clear_srcs {
    cd /root

    rm -rf /root/expat-2.5.0
    rm -rf /root/libunibreak-5.1
    rm -rf /root/zlib-1.2.13
    rm -rf /root/libpng-1.6.39
    rm -rf /root/freetype-2.13.0
    rm -rf /root/libiconv-1.17
    rm -rf /root/harfbuzz-6.0.0
    rm -rf /root/fribidi-1.0.12
    rm -rf /root/fontconfig-2.14.2
    
    find -name '*.tar.*' -exec tar xvf {} \;
}

function prepare_package_and_srcs {
    dpkg --add-architecture i386
    apt update
    apt install -y mingw-w64 mingw-w64-tools pkg-config autoconf automake make gperf aria2 build-essential python3

    cd /root
    echo "
https://github.com/libexpat/libexpat/releases/download/R_2_5_0/expat-2.5.0.tar.xz
https://www.zlib.net/zlib-1.2.13.tar.xz
https://github.com/adah1972/libunibreak/releases/download/libunibreak_5_1/libunibreak-5.1.tar.gz
https://nchc.dl.sourceforge.net/project/libpng/libpng16/1.6.39/libpng-1.6.39.tar.xz
https://mirrors.sarata.com/non-gnu/freetype/freetype-2.13.0.tar.gz
https://github.com/harfbuzz/harfbuzz/releases/download/6.0.0/harfbuzz-6.0.0.tar.xz
https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.17.tar.gz
https://github.com/fribidi/fribidi/releases/download/v1.0.12/fribidi-1.0.12.tar.xz
https://www.freedesktop.org/software/fontconfig/release/fontconfig-2.14.2.tar.xz
" > download-list.txt

    aria2c --input-file=download-list.txt --max-concurrent-downloads=8 --max-connection-per-server=8
    rm download-list.txt
}

function set_environment {
    export CC=/usr/bin/$PREFIX-gcc-posix
    export CPP=/usr/bin/$PREFIX-cpp-posix
    export CXX=/usr/bin/$PREFIX-g++-posix
    export LD=/usr/bin/$PREFIX-ld
    export RANLIB=/usr/bin/$PREFIX-gcc-ranlib-posix
    export STRIP=/usr/bin/$PREFIX-strip
    export AR=/usr/bin/$PREFIX-ar
    export HOST=$PREFIX
    export PKG_CONFIG=/usr/bin/$PREFIX-pkg-config
    export PKG_CONFIG_LIBDIR=/usr/$PREFIX/lib/pkgconfig
    export PKG_CONFIG_PATH=/usr/$PREFIX/lib/pkgconfig
    export CFLAGS='-fPIC -g -static -static-libgcc -static-libstdc++ -Wl,-static'
    export CPPFLAGS='-fPIC -g -static -static-libgcc -static-libstdc++ -Wl,-static'
    export LDFLAGS='-fPIC -g -static -static-libgcc -static-libstdc++ -Wl,-static'
}

function set_simple_environment {
    unset CC
    unset CPP
    unset CXX
    unset LD
    unset RANLIB
    unset STRIP
    unset AR
    export HOST=$PREFIX
    unset PKG_CONFIG
    export PKG_CONFIG_LIBDIR=/usr/$PREFIX/lib/pkgconfig
    export PKG_CONFIG_PATH=/usr/$PREFIX/lib/pkgconfig
    export CFLAGS='-fPIC -g -static -static-libgcc -static-libstdc++ -Wl,-static'
    export CPPFLAGS='-fPIC -g -static -static-libgcc -static-libstdc++ -Wl,-static'
    export LDFLAGS='-fPIC -g -static -static-libgcc -static-libstdc++ -Wl,-static'
}

function configure_make_make_install {
    ./configure --prefix=/usr/$PREFIX/ --build=x86_64-linux-gnu --host=$PREFIX --enable-static=yes --enable-shared=no && \
    make -j$PARALLEL_NUM && make install
}

function make_expat {
    printf "\n\n[[ Making expat for $PREFIX...... ]]\n\n"
    cd /root/expat-2.5.0
    configure_make_make_install
}

function make_libunibreak {
    printf "\n\n[[ Making libunibreak for $PREFIX... ]]\n\n"
    cd /root/libunibreak-5.1
    configure_make_make_install
}

function make_zlib {
    printf "\n\n[[ Making zlib for $PREFIX... ]]\n\n"
    cd /root/zlib-1.2.13
    ./configure --prefix=/usr/$PREFIX --static --libdir=/usr/$PREFIX/lib/ --includedir=/usr/$PREFIX/include/ && make -j$PARALLEL_NUM && make install
}

function make_libpng {
    printf "\n\n[[ Making libpng for $PREFIX... ]]\n\n"
    cd /root/libpng-1.6.39
    configure_make_make_install
}

function make_freetype {
    printf "\n\n[[ Making freetype for $PREFIX... ]]\n\n"
    cd /root/freetype-2.13.0
    configure_make_make_install
}

function make_libiconv {
    printf "\n\n[[ Making libiconv for $PREFIX... ]]\n\n"
    cd /root/libiconv-1.17
    configure_make_make_install
}

function make_harfbuzz {
    printf "\n\n[[ Making harfbuzz for $PREFIX... ]]\n\n"
    cd /root/harfbuzz-6.0.0
    configure_make_make_install
}

function make_fribidi {
    printf "\n\n[[ Making fribidi for $PREFIX... ]]\n\n"
    cd /root/fribidi-1.0.12
    configure_make_make_install
}

function make_fontconfig {
    printf "\n\n[[ Making fontconfig for $PREFIX... ]]\n\n"
    cd /root/fontconfig-2.14.2
    configure_make_make_install
}

function make_all {    
    clear_srcs && \
    set_environment && \
    make_zlib && \
    set_simple_environment && \
    make_expat && \
    make_libunibreak && \
    make_libpng && \
    make_freetype && \
    make_libiconv && \
    make_harfbuzz && \
    make_fribidi && \
    make_fontconfig
}

function check_env {
    # If PARALLEL_NUM is not set, exit
    if [ -z "$PARALLEL_NUM" ]; then
        echo "PARALLEL_NUM is not set. Please set it to the number of parallel jobs you want to run."
        exit 1
    fi
}

check_env
prepare_package_and_srcs

PREFIX=i686-w64-mingw32 && \
make_all && \
PREFIX=x86_64-w64-mingw32 && \
make_all
