set(CLIENT_SRCS client.cpp ${CAPNP_SRCS})

#TODO move out
set_source_files_properties(${CAPNP_SRCS} ${CAPNP_HDRS}
                            PROPERTIES GENERATED TRUE)

add_executable(client ${CLIENT_SRCS})
