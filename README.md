# MicroSociety


![CI](https://github.com/Klus3kk/microsociety/actions/workflows/ci.yml/badge.svg)
![License](https://img.shields.io/badge/License-MIT-blue)
![Language](https://img.shields.io/badge/C++-blue)

## Description

MicroSociety is an object-oriented simulation project that models a dynamic society with AI-driven behaviors, events, and interactions.

The project uses AI to create a lively environment with support for both traditional Q-learning and TensorFlow-based deep learning.

## Platform-Specific Setup

### Linux/macOS (Full TensorFlow Support)

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

**Note:** Windows automatically disables TensorFlow due to incomplete C API headers. The simulation can use Q-learning instead.

1. Open **Developer Command Prompt** for Visual Studio
2. Build the project:

   ```cmd
   mkdir build && cd build
   cmake -G "NMake Makefiles" ..
   nmake
   bin\MicroSociety.exe
   ```

### Docker Setup 

**For Windows users**

```bash
docker build -t microsociety .
docker run -it microsociety
```

**With X11 forwarding (Linux/macOS):**

```bash
xhost +local:docker

docker run -it --rm \
  -e DISPLAY=$DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  microsociety
```

## AI Modes

The simulation offers three AI modes selectable at startup:

1. **Single player**: Play as a character alongside AI NPCs
2. **Reinforcement learning C++**: NPCs use Q-learning algorithms
3. **Deep Q-learning TensorFlow**: NPCs use neural networks 


