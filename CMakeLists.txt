cmake_minimum_required(VERSION 3.14)
project(MicroSociety LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Option to build shared or static libraries
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Automatically fetch SFML and nlohmann_json libraries
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.10.5
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(SFML nlohmann_json)

# Add include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add TensorFlow directories
include_directories(${TENSORFLOW_INCLUDE_DIR})
link_directories(${TENSORFLOW_LIB_DIR})

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Add assets files
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Add executable for the main project
add_executable(MicroSociety ${SOURCES})

# Link TensorFlow libraries to the main executable
target_link_libraries(MicroSociety PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json tensorflow_cc tensorflow_framework)

# Platform-specific settings
if(WIN32)
    # Windows-specific linking
    add_custom_command(
        TARGET MicroSociety
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:MicroSociety>
        VERBATIM)
elseif(UNIX)
    # Link additional pthread for Unix
    target_link_libraries(MicroSociety PRIVATE pthread)
endif()

# Add the test executable for unit tests
add_executable(TestTerrain tests/test_terrain.cpp src/Game.cpp include/Tile.hpp include/Object.hpp)
target_link_libraries(TestTerrain PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json tensorflow_cc tensorflow_framework)

add_executable(TestEntity tests/test_entity.cpp)
target_link_libraries(TestEntity PRIVATE sfml-graphics sfml-audio sfml-system sfml-window)
