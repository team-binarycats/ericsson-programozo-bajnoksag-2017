# Ericsson Programozói Bajnokság 2017
## Building
This package uses the autotools build system (check out the cmake branch for CMake), so compilation is very easy:

- `./configure`
- `make`

### Prerequisites
- Standard development environment
	- `sudo apt install build-essential` (Note: As of writing only `g++` is required really)
- Cap'n Proto
	- `sudo apt install capnp`, _or_
	- `git clone https://github.com/capnproto/capnproto`
	- `cd capnproto`
	- `mkdir build`
	- `( cd $_ && cmake .. && make )`
	- `export PATH=$PWD/c++/src/capnp:$PATH`
