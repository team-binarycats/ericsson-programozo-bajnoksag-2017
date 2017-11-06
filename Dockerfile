FROM lezsakdomi/capnproto-cplusplus:buildpack

RUN apt-get update \
	&& apt-get install -y --no-install-recommends \
		cmake

ARG CMAKEFLAGS=""
ARG MAKEFLAGS=""
ARG TARGET=""
ARG SRCDIR="/src"
ARG WORKDIR="$SRCDIR/build"
RUN mkdir -p "$WORKDIR"
WORKDIR $WORKDIR

COPY . $SRCDIR

RUN cmake $CMAKEFLAGS "$SRCDIR"
RUN make $MAKEFLAGS $TARGET

ENTRYPOINT make $MAKEFLAGS run
