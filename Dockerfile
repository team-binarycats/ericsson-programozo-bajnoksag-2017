FROM lezsakdomi/capnproto-cplusplus

ARG MAKEFLAGS=""
ARG TARGET=""

COPY . .

RUN make $MAKEFLAGS $TARGET
