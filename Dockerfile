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
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Install Bazel (required for TensorFlow build)
RUN apt-get install -y apt-transport-https curl gnupg \
    && curl https://bazel.build/bazel-release.pub.gpg | apt-key add - \
    && echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list \
    && apt-get update && apt-get install -y bazel

# Clone TensorFlow repository
WORKDIR /tensorflow
RUN git clone https://github.com/tensorflow/tensorflow.git && cd tensorflow && git checkout v2.15.0

# Configure TensorFlow build (automated)
WORKDIR /tensorflow/tensorflow
RUN yes "" | ./configure

# Build TensorFlow with C++ API
RUN bazel build //tensorflow:libtensorflow_cc.so

# Copy TensorFlow headers and libraries to /usr/local
RUN cp -r bazel-bin/tensorflow/include /usr/local/include/tensorflow && cp bazel-bin/tensorflow/libtensorflow_cc.so /usr/local/lib/ && ldconfig

# Set environment variables for TensorFlow
ENV TENSORFLOW_INCLUDE_DIR=/usr/local/include
ENV TENSORFLOW_LIB_DIR=/usr/local/lib

# Ensure TensorFlow is in the library path
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
ENV CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/include

# Set working directory for the project
WORKDIR /app

# Copy project files into the container
COPY . .

# Create a build directory and build the project using CMake
RUN mkdir build && cd build && cmake .. -DTENSORFLOW_INCLUDE_DIR=/usr/local/include/tensorflow -DTENSORFLOW_LIB_DIR=/usr/local/lib && cmake --build .

# Run the application
CMD ["./build/MicroSociety"]
