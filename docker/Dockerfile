FROM ubuntu:22.04 AS lib-builder

ARG thread=8
COPY ./build-env.sh /build-env.sh
ENV PARALLEL_NUM=$thread
RUN bash /build-env.sh

FROM ubuntu:22.04 AS final

RUN apt update && apt install -y mingw-w64 mingw-w64-tools autoconf automake make nasm libtool
COPY --from=lib-builder /usr/x86_64-w64-mingw32 /usr/x86_64-w64-mingw32
COPY --from=lib-builder /usr/i686-w64-mingw32 /usr/i686-w64-mingw32
