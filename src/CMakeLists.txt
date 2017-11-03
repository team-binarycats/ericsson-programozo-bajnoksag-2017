set(CLIENT_SRCS main.cpp ${CAPNP_SRCS})

#TODO move out
set_source_files_properties(${CAPNP_SRCS} ${CAPNP_HDRS}
                            PROPERTIES GENERATED TRUE)

list(GET CAPNP_SRCS 2 le)
get_source_file_property(PROP "${le}" GENERATED)
message("Property status: ${PROP}")

list(GET CAPNP_SRCS 1 le)
get_source_file_property(PROP "${le}" GENERATED)
message("AFTER Property status: ${PROP}")

add_executable(client ${CLIENT_SRCS})
