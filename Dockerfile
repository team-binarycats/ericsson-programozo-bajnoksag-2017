FROM lezsakdomi/capnproto-cplusplus:buster-buildpack

ARG MAKEFLAGS=""
ARG TARGET=""
ARG WORKDIR="/src"

COPY . $WORKDIR
WORKDIR $WORKDIR

RUN make $MAKEFLAGS $TARGET

ENTRYPOINT make $MAKEFLAGS run
