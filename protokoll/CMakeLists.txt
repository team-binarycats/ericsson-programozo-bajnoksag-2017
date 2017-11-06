set(PROTOKOLL_FILES Bugfix.capnp Request.capnp Response.capnp)

capnp_generate_cpp(CAPNP_SRCS CAPNP_HDRS ${PROTOKOLL_FILES})
set(CAPNP_SRCS "${CAPNP_SRCS}" PARENT_SCOPE)
set(CAPNP_HDRS "${CAPNP_HDRS}" PARENT_SCOPE)

foreach(hdr ${CAPNP_HDRS})
	get_filename_component(dir "${hdr}" DIRECTORY)
	include_directories("${dir}")
endforeach(hdr)

add_custom_target(capnp_cpp ALL
                  DEPENDS ${CAPNP_SRCS} ${CAPNP_HDRS})
