# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    unzip \
    python3 \
    python3-pip \
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

# Set working directory
WORKDIR /app

# Clone and Build TensorFlow Lite
RUN wget -q https://github.com/tensorflow/tensorflow/archive/refs/tags/v2.11.0.zip && \
    unzip v2.11.0.zip && \
    mv tensorflow-2.11.0 tensorflow-lite && \
    rm v2.11.0.zip && \
    cd tensorflow-lite/tensorflow/lite && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/tflite && \
    cmake --build . -j$(nproc) && \
    cmake --install . && \
    ls -lh /usr/local/tensorflow-lite/tensorflow/lite/build # Check install

# Copy project files
COPY . .

# Set TensorFlow Lite Environment Variable
ENV TFLITE_DIR="/usr/local/tensorflow-lite/tensorflow/lite/build"

# Build MicroSociety
RUN rm -rf build && mkdir build && cd build && cmake .. && cmake --build .

# Run the application
CMD ["./build/MicroSociety"]
