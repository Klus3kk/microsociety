cmake_minimum_required(VERSION 3.14)
project(MicroSociety LANGUAGES CXX)

# Set compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)

# **🔹 Fetch Dependencies**
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

set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT" FORCE)
FetchContent_MakeAvailable(SFML nlohmann_json googletest)

enable_testing()
include(GoogleTest)

include_directories(${CMAKE_SOURCE_DIR}/include)

# **🔹 TensorFlow Lite Setup (FINAL FIX)**
set(TFLITE_DIR "/usr/local/tflite")
set(TFLITE_LIB "${TFLITE_DIR}/lib/libtensorflow-lite.a")

# **Ensure TensorFlow Lite is installed**
if(NOT EXISTS ${TFLITE_LIB})
    message(FATAL_ERROR "TensorFlow Lite library not found at ${TFLITE_LIB}. Did you install it correctly?")
endif()

include_directories(${TFLITE_DIR}/include)

# **🔹 Source Files**
file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

# **🔹 Assets**
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# **🔹 Main Executable**
add_executable(MicroSociety src/main.cpp ${SOURCES})

target_link_libraries(MicroSociety PRIVATE 
    sfml-graphics sfml-audio sfml-system sfml-window 
    nlohmann_json::nlohmann_json 
    pthread
    ${TFLITE_LIB}
)

# **🔹 Unit Tests**
file(GLOB TEST_SOURCES "tests/*.cpp")
add_executable(all_tests ${TEST_SOURCES} ${SOURCES})
target_link_libraries(all_tests PRIVATE 
    sfml-graphics sfml-audio sfml-system sfml-window 
    nlohmann_json::nlohmann_json 
    gtest_main 
    ${TFLITE_LIB}
)

# **🔹 Discover all tests**
gtest_discover_tests(all_tests)
