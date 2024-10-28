# MicroSociety

## Description

MicroSociety is an object-oriented simulation project that models a dynamic society with AI-driven behaviors, events, and interactions. The project uses AI to create a lively, ever-changing environment.

## Setup

1. Build the Docker image:

   ```bash
   docker build -t micro-society .
   ```

2. Run the Docker container:

   ```bash
   docker run -it micro-society
   ```

3. Run the executable:

   ```bash
   ./MicroSociety
   ```

### Using CMake (Windows)

1. Open the **Developer Command Prompt** for your compiler (e.g., MSVC).

2. Create a build directory:

   ```bash
   mkdir build && cd build
   ```

3. Generate the project:

   ```bash
   cmake -G "NMake Makefiles" ..
   ```

4. Build the project:

   ```bash
   nmake
   ```

5. Run the executable:

   ```bash
   MicroSociety.exe
   ```

---

This version separates the CMake setup instructions for Linux and Windows.
