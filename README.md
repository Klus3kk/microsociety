# microsociety

## Description

An object-oriented project, that represents the simulator of the society, with dynamically changing events and AI implemented methods for more lively environments.

## Setup

1. You need to build Docker image to run the simulator:

```bash
docker build -t micro-society .
```

2. After building run the container:

```bash
docker run -it micro-society
```

Or you can build it by using CMake:

```bash
mkdir build
```

```bash
cd build
```

```bash
cmake ..
```

```bash
make
```

After that, you can run it just by writing:

```bash
./MicroSociety
```
