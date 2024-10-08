# Use an official Ubuntu base image
FROM ubuntu:20.04

# Update system and install necessary libraries and tools
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    wget \
    git \
    libsfml-dev \
    libcurl4-openssl-dev \
    libudev-dev \
    libopenal-dev \
    libvorbis-dev \
    libogg-dev \
    libflac-dev \
    libmpg123-dev \
    libxrandr-dev \
    libx11-dev \
    libxext-dev \
    libxcursor-dev \
    libxi-dev \
    libxinerama-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libfreetype6-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    && rm -rf /var/lib/apt/lists/*

# Download TensorFlow C++ library
WORKDIR /tensorflow
RUN wget -q --no-check-certificate https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-linux-x86_64-2.16.0.tar.gz
RUN tar -C /usr/local -xzf libtensorflow-cpu-linux-x86_64-2.16.0.tar.gz

# Set environment variables for TensorFlow
ENV TENSORFLOW_INCLUDE_DIR=/usr/local/include
ENV TENSORFLOW_LIB_DIR=/usr/local/lib
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
ENV CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/include

# Set working directory for the project
WORKDIR /app

# Copy project files into the container
COPY . .

# Create a build directory and build the project using CMake
RUN mkdir build && cd build && cmake .. && cmake --build .

# Run the application
CMD ["./build/MicroSociety"]
