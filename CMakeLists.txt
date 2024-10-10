cmake_minimum_required(VERSION 3.14)
project(MicroSociety)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for TensorFlow and Protobuf
include_directories("/usr/local/include")
include_directories("/usr/include/google/protobuf")
link_directories("/usr/local/lib")

# Find required packages
find_package(Protobuf REQUIRED)
find_package(Abseil REQUIRED)

# Include directories for Protobuf and Abseil
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${Abseil_INCLUDE_DIRS})

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

# Link TensorFlow, SFML, Protobuf, and Abseil
target_link_libraries(MicroSociety
    sfml-graphics
    sfml-audio
    sfml-system
    sfml-window
    nlohmann_json::nlohmann_json
    tensorflow_cc  # Link TensorFlow C++ API
    ${Protobuf_LIBRARIES}
    ${Abseil_LIBRARIES}
)
