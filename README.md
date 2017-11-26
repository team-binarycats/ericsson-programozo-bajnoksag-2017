# Ericsson Programozói Bajnokság 2017
## Building
This package uses a docker build system on top of GNU Make, so compilation and execution is very easy:
```
tag=`golang-petname`
docker build . -t "$tag"
docker run -it --rm "$tag"
```
_or_
```
make
make run
```

### Prerequisites
- Docker
	- `sudo apt install docker`
	- https://docs.docker.com/engine/installation/
_or_
- Make
	- `sudo apt install make`
- GNU C++ compiler
	- `sudo apt install g++`
- Cap'n Proto and its libraries
	- `sudo apt install capnproto libcapnp-dev`
- ncurses libraries with wide character support
	- `sudo apt install libncursesw5-dev`

# Files
- `Makefile`, `Dockerfile`: Files for the build system...
- `src`: Standard source files
	- `*.cpp` without a corresponding `*.h`: Components. You include their headers in your program
		- To specify costum component, override the `COM` make variable: `make COM=interface client`
		- Directory of components: `COMDIR` make variable
	- `*.cpp` (withoud a header file): Programs, are compiled separated
		- Override `PROGRAM` for make to specify your target: `make PROGRAM="client" run`
		- Specify a program for make as target to compile that: `make client`
- `obj`: Compiled sources (_objects_)
	- `*.o`: ...
		- To specify object file location, override the `OBJDIR` make variable: `make OBJDIR=/tmp/`
		- To add customize objects to be linked, override `OBJ`: `make OBJ="main.o my-lib.o" PROGRAM_NAME="my-application"`
- `protokoll`: Cap'n Proto specifications
	- `*.capnp`: ...
		- To manage capnp specs, see the `proto` target and the `PROTO` (and/or `PROTO_*`) make variables.
	- `*.capnp.{c++,h}`: Compiled specifications
- `bin`: Compiled runnable binaries
	- `*`: ...
		- To override the directory, see the `BINDIR` make variable

### Useful commands
To decode (and print) messages on the fly:
```
make run RUN_SOCKET_REDIRECTS= RUN_REDIRECTS='3< <(tee >(capnp decode protokoll/Response.capnp Response >&2) <&$$socket) 4> >(tee >(capnp decode protokoll/Command.capnp Command >&2) >&$$socket)'
```

To test the program, I recommend to issue the followings in two different terminals:
```
make run RUN_SOCKET_REDIRECTS= RUN_PREPEND='echo -ne "\033[s\033[85H\033[1J\033[u"; time' RUN_REDIRECTS='3< <(tee ds <&$$socket) 4> >(tee us >&$$socket) 2>log'
tail -f log | grep \*
```
