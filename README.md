# Ericsson Programozói Bajnokság 2017
## Building
This package uses the CMake build system
(check out the autotools branch for autotools, cmake for CMake, and make for GNU make based toolchains)
so compilation is very easy:

- mkdir build
- ( cd build && cmake .. )

Ok, in real life it is a little more complicated. I presonally use the followin to test:
```
(cd build; (cmake -DCapnProto_DIR=$HOME/Downloads/capnproto/build/c++/cmake .. && make) && printf "\033[32m=\%.0s\033[0m" {1..80} || echo -e "\033[31m============================= Exited with status \033[0;1m$?\033[0;31m =============================\033[0m")
```

Please note that it mostly still doesn't work.

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
