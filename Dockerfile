FROM tensorflow/tensorflow:2.11.0  

# Install additional dependencies needed for your project
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
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

# Set working directory for the project
WORKDIR /app

# Copy project files into the container
COPY . .

# Create a build directory and build the project using the TensorFlow CMake file
RUN mkdir build && cd build && cmake -C ../CMakeLists_with_tf.txt .. && cmake --build .

# Run the application
CMD ["./build/MicroSociety"]
