FROM lezsakdomi/capnproto-cplusplus:buildpack

ARG MAKEFLAGS=""
ARG TARGET=""
ARG WORKDIR="/src"

COPY . $WORKDIR
WORKDIR $WORKDIR

RUN make $MAKEFLAGS $TARGET
