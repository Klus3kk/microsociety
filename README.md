# MicroSociety

![CI](https://github.com/Klus3kk/microsociety/actions/workflows/ci.yml/badge.svg)
![License](https://img.shields.io/badge/License-MIT-blue)
![Language](https://img.shields.io/badge/C++-blue)

## Description

MicroSociety is an object-oriented simulation project that models a dynamic society with AI-driven behaviors, events, and interactions. The project uses AI to create a lively, ever-changing environment with support for both traditional Q-learning and advanced TensorFlow-based deep learning.

## Setup Options

MicroSociety offers flexible setup options depending on your platform and AI requirements:

- **With TensorFlow (Linux/macOS/Docker)**: Full AI capabilities including deep Q-learning with TensorFlow
- **Without TensorFlow (All platforms)**: Q-learning based AI without TensorFlow dependencies
- **Windows**: Automatic fallback to Q-learning (TensorFlow C API has incomplete headers on Windows)

## Platform-Specific Setup

### Linux/macOS (Full TensorFlow Support)

**Prerequisites:**

- CMake 3.14+
- C++17 compatible compiler
- Git

**Build with TensorFlow:**

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
./bin/MicroSociety
```

**Build without TensorFlow:**

```bash
mkdir build && cd build
cmake .. -DUSE_TENSORFLOW=OFF
make -j$(nproc)
./bin/MicroSociety
```

### Windows (Q-Learning Only)

**Prerequisites:**

- Visual Studio 2019/2022 Build Tools or Visual Studio Community
- CMake 3.14+
- Git

**Note:** Windows automatically disables TensorFlow due to incomplete C API headers. The simulation uses Q-learning instead.

1. Open **Developer Command Prompt** for Visual Studio
2. Build the project:

   ```cmd
   mkdir build && cd build
   cmake -G "NMake Makefiles" ..
   nmake
   bin\MicroSociety.exe
   ```

### Docker Setup (Full TensorFlow Support on Any Platform)

**For Windows users who need TensorFlow support:**

1. Install Docker Desktop
2. Build and run:

   ```bash
   docker build -t microsociety .
   docker run -it microsociety
   ```

**With X11 forwarding (Linux/macOS):**

```bash
# Allow X11 connections
xhost +local:docker

# Run with GUI support
docker run -it --rm \
  -e DISPLAY=$DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  microsociety
```

### WSL2 Setup (Windows Alternative)

For Windows users who prefer a native Linux environment:

1. Install WSL2 with Ubuntu:

   ```cmd
   wsl --install -d Ubuntu-22.04
   ```

2. Enter WSL2 and build:

   ```bash
   wsl
   cd /mnt/c/path/to/microsociety
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ./bin/MicroSociety
   ```

## AI Modes

The simulation offers three AI modes selectable at startup:

1. **Single Player Mode**: Play as a character alongside AI NPCs
2. **Reinforcement Learning C++**: NPCs use Q-learning algorithms
3. **Deep Q-Learning TensorFlow**: NPCs use neural networks (Linux/macOS/Docker only)

## Build Options

| Platform | TensorFlow | Q-Learning | Recommended Setup |
|----------|------------|------------|-------------------|
| Linux    | ✅ Full Support | ✅ | Native build |
| macOS    | ✅ Full Support | ✅ | Native build |
| Windows  | ❌ Auto-disabled | ✅ | Native build or Docker |

## Troubleshooting

### TensorFlow Issues on Windows

If you see TensorFlow-related errors on Windows, the build system automatically falls back to Q-learning. For full TensorFlow support on Windows, use Docker or WSL2.

### Memory Leak/Performance Profiling

**Linux/macOS with Valgrind:**

```bash
valgrind --leak-check=full --track-origins=yes ./MicroSociety
```

**Performance profiling:**

```bash
valgrind --tool=callgrind ./MicroSociety
kcachegrind callgrind.out.<pid>
```

**Debugging with GDB:**

```bash
gdb ./MicroSociety
run
# If crash occurs:
bt full
```

**Windows with Visual Studio:**

```cmd
# Debug build
cmake -G "NMake Makefiles" .. -DCMAKE_BUILD_TYPE=Debug
nmake
# Run with debugger attached in Visual Studio
```
