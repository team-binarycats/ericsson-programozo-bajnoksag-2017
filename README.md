# Ericsson Programozói Bajnokság 2017
## Building
This package uses the CMake build system (check out the autotools branch for autotools), so compilation is very easy:
- mkdir build
- ( cd build && cmake .. )
### Prerequisites
- Standard development environment
	- `sudo apt install build-essential cmake` (Note: As of writing only `g++` and `cmake` is required really)
- Cap'n Proto
	- `sudo apt install capnp`, _or_
	- `git clone https://github.com/capnproto/capnproto`
	- `cd capnproto`
	- `mkdir build`
	- `( cd $_ && cmake .. && make )`
	- `export CapnProto_DIR=$PWD/c++/cmake`
