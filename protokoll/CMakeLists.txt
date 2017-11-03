set(PROTOKOLL_FILES Bugfix.capnp Request.capnp Response.capnp)

capnp_generate_cpp(CAPNP_SRCS CAPNP_HDRS ${PROTOKOLL_FILES})
add_custom_target(capnp_cpp ALL
	DEPENDS ${CAPNP_SRCS} ${CAPNP_HDRS})
