# Using Ubuntu image as base
FROM ubuntu:20.04

# Zaktualizuj system i zainstaluj podstawowe narzędzia, bez interaktywnych pytań
ENV DEBIAN_FRONTEND=noninteractive


# Update the system and install basic tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    wget \
    git \ 
    libsfml-dev \
    libcurl4-openssl-dev \
    libudev-dev \ 
    && rm -rf /var/lib/apt/lists/*


# Download TensorFlow C API
WORKDIR /tensorflow 
RUN wget https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-linux-x86_64-2.6.0.tar.gz
RUN tar -C /usr/local -xzf libtensorflow-cpu-linux-x86_64-2.6.0.tar.gz
# Update dynamic linking
RUN ldconfig 

# Working directory
WORKDIR /app

# Copy project files
COPY . .

# Create build folder and build a project
RUN mkdir build && cd build && cmake .. && cmake --build .

# Uruchomienie aplikacji po starcie kontenera
CMD ["./build/MicroSociety"]