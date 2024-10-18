cmake_minimum_required(VERSION 3.14)
project(MicroSociety)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for TensorFlow, Bazel output, and Protobuf
# include_directories("/usr/local/include/tensorflow")
# include_directories("/usr/local/include/tensorflow/bazel-bin/tensorflow/")  # Bazel output directory
# include_directories("/usr/include/google/protobuf")    # Include Protobuf

# Link TensorFlow libraries
link_directories("/usr/local/lib")

# Fetch SFML and nlohmann_json libraries
include(FetchContent)

FetchContent_Declare(
  sfml
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(sfml)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.10.5
)
FetchContent_MakeAvailable(json)

# Add sources
file(GLOB SOURCES "src/*.cpp")

# Add executable
add_executable(MicroSociety ${SOURCES})

# Link TensorFlow, SFML, and other libraries
target_link_libraries(MicroSociety
    sfml-graphics
    sfml-audio
    sfml-system
    sfml-window
    nlohmann_json::nlohmann_json
    # tensorflow_cc   # Link TensorFlow C++ API
    # tensorflow_framework  # TensorFlow framework if needed
)
