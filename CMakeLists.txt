cmake_minimum_required(VERSION 3.1)

set(PROTO_DIR protokoll)

find_package(CapnProto REQUIRED)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory("${PROTO_DIR}")
include_directories("${PROTO_DIR}")

set(CLIENT_SRCS src/client.cpp ${CAPNP_SRCS})

#TODO move out
set_source_files_properties(${CAPNP_SRCS} ${CAPNP_HDRS}
                            PROPERTIES GENERATED TRUE)

add_executable(client ${CLIENT_SRCS})
