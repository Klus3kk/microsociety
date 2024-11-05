cmake_minimum_required(VERSION 3.14)
project(MicroSociety LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

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
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(SFML nlohmann_json googletest)

enable_testing()
include(GoogleTest)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Add main source files
file(GLOB SOURCES "src/*.cpp")

add_executable(MicroSociety ${SOURCES})

if(WIN32)
    target_link_libraries(MicroSociety PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json)
    add_custom_command(
        TARGET MicroSociety
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:MicroSociety>
        VERBATIM)
elseif(UNIX)
    target_link_libraries(MicroSociety PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json pthread)
endif()

# Unit test executables - add only files that exist
add_executable(TestTerrain tests/test_terrain.cpp src/Game.cpp) # Adjusted sources for existing files
target_link_libraries(TestTerrain PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json gtest_main)
gtest_discover_tests(TestTerrain)

add_executable(TestEntity tests/test_entity.cpp)  # Add sources here as needed
target_link_libraries(TestEntity PRIVATE sfml-graphics sfml-audio sfml-system sfml-window gtest_main)
gtest_discover_tests(TestEntity)

# Collision tests executable
add_executable(TestCollision tests/test_collision.cpp src/Game.cpp) # Adjust source files to match project
target_link_libraries(TestCollision PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json gtest_main)
gtest_discover_tests(TestCollision)
