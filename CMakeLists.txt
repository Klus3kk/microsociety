cmake_minimum_required(VERSION 3.14)
project(MicroSociety)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${TENSORFLOW_INCLUDE_DIR})
link_directories(${TENSORFLOW_LIB_DIR})

# Fetch SFML and nlohmann_json
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

# Link libraries
target_link_libraries(MicroSociety
    sfml-graphics
    sfml-audio
    sfml-system
    sfml-window
    nlohmann_json::nlohmann_json
    tensorflow_cc
)
