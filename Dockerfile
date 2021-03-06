FROM lezsakdomi/capnproto-cplusplus:buildpack

ARG MAKEFLAGS=""
ENV MAKEFLAGS=$MAKEFLAGS
ARG BUILD_TARGET=""
ARG WORKDIR="/src"

COPY . $WORKDIR
WORKDIR $WORKDIR

ARG BUILD_ARGS=""
RUN make $BUILD_TARGET $BUILD_ARGS -$MAKEFLAGS

ARG RUN_ARGS="RUN_REDIRECTS=2>/dev/null"
ENV RUN_ARGS=$RUN_ARGS
ENTRYPOINT make run $RUN_ARGS -$MAKEFLAGS
