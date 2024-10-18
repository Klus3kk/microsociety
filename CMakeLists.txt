cmake_minimum_required(VERSION 3.14)
project(MicroSociety)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Automatically fetch Raylib and nlohmann_json libraries
include(FetchContent)

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 4.5.0
)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.10.5
)
FetchContent_MakeAvailable(json)

# Source files for your project
file(GLOB SOURCES "src/*.cpp")

# Create the executable
add_executable(MicroSociety ${SOURCES})

# Link libraries
target_link_libraries(MicroSociety
    raylib
    nlohmann_json::nlohmann_json
)
