# MicroSociety

![CI](https://github.com/Klus3kk/microsociety/actions/workflows/ci.yml/badge.svg)
![License](https://img.shields.io/badge/License-MIT-blue)
![Language](https://img.shields.io/badge/C++-blue)

## Description

MicroSociety is an object-oriented simulation project that models a dynamic society with AI-driven behaviors, events, and interactions. The project uses AI to create a lively, ever-changing environment.

## Setup Options

MicroSociety offers two setup options:

- **With TensorFlow (Docker-based)**: Enables advanced AI capabilities using TensorFlow.
- **Without TensorFlow (Local build)**: Provides a simpler setup without TensorFlow dependencies.

### Docker Setup with TensorFlow

1. Build the Docker image (TensorFlow-enabled):

   ```bash
   docker build -t micro-society .
   ```

2. Run the Docker container:

   ```bash
   docker run -it micro-society
   ```

### Local Setup without TensorFlow (Linux)

1. Rename `CMakeLists_without_tf.txt` to `CMakeLists.txt` in your local folder.

2. Create a build directory and compile:

   ```bash
   mkdir build && cd build
   cmake ..
   make clean && make -j$(nproc)
   ```

3. Run the executable:

   ```bash
   ./MicroSociety
   ```

### Local Setup without TensorFlow (Windows)

1. Rename `CMakeLists_without_tf.txt` to `CMakeLists.txt` in your local folder.

2. Open the **Developer Command Prompt** for your compiler (e.g., MSVC).

3. Create a build directory:

   ```bash
   mkdir build && cd build
   ```

4. Generate the project:

   ```bash
   cmake -G "NMake Makefiles" ..
   ```

5. Build the project:

   ```bash
   nmake
   ```

6. Run the executable:

   ```bash
   MicroSociety.exe
   ```

### Memory Leak/Performance Profiling

If you want to check memory leaks or performance profiling, use Valgrind:

```bash
valgrind --leak-check=full --track-origins=yes ./MicroSociety
```

```bash
valgrind --tool=callgrind ./MicroSociety
kcachegrind callgrind.out.<pid>
```

Or gdb:

```bash
gdb ./MicroSociety
run
```

If the simulation crashes, write:

```bash
bt full 
```


