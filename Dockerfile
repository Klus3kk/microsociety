FROM ubuntu:22.04

# system dependencies
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

# working directory
WORKDIR /app

# clone/build TensorFlow Lite 
RUN wget -q https://github.com/tensorflow/tensorflow/archive/refs/tags/v2.11.0.zip && \
    unzip v2.11.0.zip && \
    mv tensorflow-2.11.0 tensorflow-lite && \
    rm v2.11.0.zip && \
    cd tensorflow-lite/tensorflow/lite && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/tflite && \
    cmake --build . -j$(nproc) && \
    cmake --install . && \
    mkdir -p /usr/local/tflite/lib && \
    cp /usr/local/tensorflow-lite/tensorflow/lite/build/libtensorflow-lite.a /usr/local/tflite/lib/ && \
    ls -lh /usr/local/tflite/lib  

# copy project files
COPY . .

# set TensorFlow Lite environment variable
ENV TFLITE_DIR="/usr/local/tflite"

# build microsociety
RUN rm -rf build && mkdir build && cd build && \
    cmake .. -DTFLITE_DIR=${TFLITE_DIR} && \
    cmake --build .

# run the application
CMD ["./build/MicroSociety"]
