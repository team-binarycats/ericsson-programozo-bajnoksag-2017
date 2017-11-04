FROM lezsakdomi/capnproto-cplusplus
COPY . .
RUN make || make
