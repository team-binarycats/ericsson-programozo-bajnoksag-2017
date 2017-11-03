cmake_minimum_required(VERSION 3.1)

find_package(CapnProto REQUIRED)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(protokoll)
#TODO move out
foreach(hdr ${CAPNP_HDRS})
	get_filename_component(dir "${hdr}" DIRECTORY)
	include_directories("${dir}")
endforeach(hdr)

add_subdirectory(src)
