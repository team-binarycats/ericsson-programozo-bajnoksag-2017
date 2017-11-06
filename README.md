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
- Cap'n Proto and its libraries
	- `sudo apt install capnproto libcapnp-dev`
