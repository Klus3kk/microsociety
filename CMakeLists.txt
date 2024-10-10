cmake_minimum_required(VERSION 3.14)
project(MicroSociety)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Automatically fetch SFML and nlohmann_json libraries
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

# Source files for your project
file(GLOB SOURCES "src/*.cpp")

# Create the executable
add_executable(MicroSociety ${SOURCES})

# Link libraries
target_link_libraries(MicroSociety
    sfml-graphics
    sfml-audio
    sfml-system
    sfml-window
    nlohmann_json::nlohmann_json
)
