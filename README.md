# Ericsson Programozói Bajnokság 2017
## Building
This package uses no build system
(check out the autotools branch for autotools, cmake for CMake, and make for GNU make based toolchains)

### Prerequisites
- Standard development environment
	- `sudo apt install build-essential` (Note: As of writing only `g++` is required really)
- Cap'n Proto
	- `sudo apt install capnproto`, _or_
	- `git clone https://github.com/capnproto/capnproto`
	- `cd capnproto`
	- `mkdir build`
	- `( cd $_ && cmake .. && make )`
