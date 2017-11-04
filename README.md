# Ericsson Programozói Bajnokság 2017
## Building
This package uses the _GNU Make_ build system
(check out the autotools branch for autotools, cmake for CMake, and make for GNU make based toolchains)
so compilation is relatively easy:

Basically:
```
make
```

Technically:
```
export capnp_repo=`mktemp -d` #DON'T follow this line as-is
git clone https://github.com/capnproto/capnproto $capnp_repo
( cd $capnp_repo && mkdir build && cd $_ && cmake .. && make )
make CXXFLAGS="-I$capnp_repo/c++/src" LINK_FLAGS="$(echo $capnp_repo/c++/src/{capnp/{c++.capnp.c++,blob.c++,arena.c++,layout.c++,list.c++,any.c++,message.c++,schema.capnp.c++,serialize.c++,serialize-packed.c++},kj/{array.c++,common.c++,debug.c++,exception.c++,io.c++,memory.c++,mutex.c++,string.c++,thread.c++,main.c++,arena.c++,test-helpers.c++}})" CAPNP=$capnp_repo/build/c++/src/capnp/capnp CAPNPC=$capnp_repo/build/c++/src/capnp/capnpc-c++
```

### Prerequisites
- Standard development environment
	- `sudo apt install build-essential` (Note: As of writing only `g++` and `make` is required really)
- Cap'n Proto
	- `sudo apt install capnp`, _or_
	- `git clone https://github.com/capnproto/capnproto`
	- `cd capnproto`
	- `mkdir build`
	- `( cd $_ && cmake .. && make )`
