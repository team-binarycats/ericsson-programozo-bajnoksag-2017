protokoll_SRC=Bugfix.capnp Request.capnp Response.capnp

all-local: $(protokoll_SRC)
	capnp compile -oc++ $?

clean-local:
	-rm *.capnp.{c++,h}
