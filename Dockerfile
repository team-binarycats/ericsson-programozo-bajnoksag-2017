FROM lezsakdomi/capnproto-cplusplus

COPY . .

ARG MAKEFLAGS=""
ARG TARGET=""
RUN make $MAKEFLAGS $TARGET
