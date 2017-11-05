# Ericsson Programozói Bajnokság 2017
## Building
This package uses a docker build system
(check out the autotools branch for autotools, cmake for CMake, and make for GNU make based toolchains)
so compilation is very easy:
```
tag=`golang-petname`
docker build . -t "$tag"
docker run -it --rm "$tag"
```

### Prerequisites
- Docker
	- `sudo apt install docker`
	- https://docs.docker.com/engine/installation/
