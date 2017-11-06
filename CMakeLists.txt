cmake_minimum_required(VERSION 3.1)

set(PROTO_DIR protokoll)
set(PROTO_FILES protokoll/Bugfix.capnp protokoll/Request.capnp protokoll/Response.capnp)

find_package(CapnProto REQUIRED)
include_directories(${CAPNP_INCLUDE_DIRS})
add_definitions(${CAPNP_DEFINITIONS})

set(CMAKE_CXX_STANDARD 11)

set(CAPNPC_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
capnp_generate_cpp(CAPNP_SRCS CAPNP_HDRS ${PROTO_FILES})
add_custom_target(capnp_cpp
                  DEPENDS ${CAPNP_SRCS} ${CAPNP_HDRS})
include_directories("${CMAKE_CURRENT_BINARY_DIR}/${PROTO_DIR}")

set(CLIENT_SRCS src/client.cpp ${CAPNP_SRCS} ${CAPNP_HDRS})

add_executable(client ${CLIENT_SRCS})
