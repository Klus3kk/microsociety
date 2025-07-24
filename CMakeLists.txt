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
option(USE_TENSORFLOW "Use TensorFlow for AI" ON)

include(FetchContent)

# Fetch Dependencies
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
    
# UPDATE: Use newer nlohmann/json version that's compatible with modern CMake
FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL)
    
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL)

set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT" FORCE)
FetchContent_MakeAvailable(SFML nlohmann_json googletest)

enable_testing()
include(GoogleTest)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Smart LLVM Detection and TensorFlow Version Selection
if(USE_TENSORFLOW)
    # Try to find LLVM to detect version
    find_package(LLVM QUIET CONFIG)
    
    if(LLVM_FOUND)
        message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
        
        if(LLVM_PACKAGE_VERSION VERSION_LESS "15.0")
            message(WARNING "Old LLVM ${LLVM_PACKAGE_VERSION} detected. Using TensorFlow 2.8.0 for compatibility.")
            set(TENSORFLOW_VERSION "2.8.0")
            
            # Add flags to work around LLVM CommandLine conflicts in older versions
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING=1")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")
            
        elseif(LLVM_PACKAGE_VERSION VERSION_GREATER_EQUAL "19.0")
            message(STATUS "Modern LLVM ${LLVM_PACKAGE_VERSION} detected (like Arch). Using TensorFlow 2.11.0.")
            set(TENSORFLOW_VERSION "2.11.0")
            
        else()
            message(STATUS "LLVM ${LLVM_PACKAGE_VERSION} detected. Using TensorFlow 2.11.0.")
            set(TENSORFLOW_VERSION "2.11.0")
        endif()
    else()
        message(WARNING "LLVM not found. Defaulting to TensorFlow 2.8.0 for maximum compatibility.")
        set(TENSORFLOW_VERSION "2.8.0")
        
        # Add conservative flags when LLVM version is unknown
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING=1")
    endif()

    message(STATUS "Selected TensorFlow version: ${TENSORFLOW_VERSION}")

    # Platform-specific URL setup
    if(WIN32)
        set(TENSORFLOW_URL "https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-windows-x86_64-${TENSORFLOW_VERSION}.zip")
        set(TENSORFLOW_ARCHIVE "${CMAKE_BINARY_DIR}/libtensorflow.zip")
    elseif(APPLE)
        set(TENSORFLOW_URL "https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-darwin-x86_64-${TENSORFLOW_VERSION}.tar.gz")
        set(TENSORFLOW_ARCHIVE "${CMAKE_BINARY_DIR}/libtensorflow.tar.gz")
    else() # Linux
        set(TENSORFLOW_URL "https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-linux-x86_64-${TENSORFLOW_VERSION}.tar.gz")
        set(TENSORFLOW_ARCHIVE "${CMAKE_BINARY_DIR}/libtensorflow.tar.gz")
    endif()
    
    # Setup directories
    set(TENSORFLOW_EXTRACT_DIR "${CMAKE_BINARY_DIR}/tensorflow")
    set(TENSORFLOW_INCLUDE_DIR "${TENSORFLOW_EXTRACT_DIR}/include")
    set(TENSORFLOW_LIB_DIR "${TENSORFLOW_EXTRACT_DIR}/lib")
    
    # Check if we need to download/extract TensorFlow
    if(NOT EXISTS "${TENSORFLOW_LIB_DIR}")
        message(STATUS "Downloading TensorFlow ${TENSORFLOW_VERSION} from ${TENSORFLOW_URL}")
        file(DOWNLOAD ${TENSORFLOW_URL} ${TENSORFLOW_ARCHIVE} SHOW_PROGRESS STATUS DOWNLOAD_STATUS)
        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        
        if(NOT STATUS_CODE EQUAL 0)
            message(FATAL_ERROR "Failed to download TensorFlow: ${DOWNLOAD_STATUS}")
        endif()
        
        # Extract archive
        message(STATUS "Extracting TensorFlow...")
        file(MAKE_DIRECTORY ${TENSORFLOW_EXTRACT_DIR})
        
        if(WIN32)
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xf ${TENSORFLOW_ARCHIVE}
                WORKING_DIRECTORY ${TENSORFLOW_EXTRACT_DIR}
            )
        else()
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xzf ${TENSORFLOW_ARCHIVE}
                WORKING_DIRECTORY ${TENSORFLOW_EXTRACT_DIR}
            )
        endif()
        
        message(STATUS "TensorFlow extracted to ${TENSORFLOW_EXTRACT_DIR}")
    else()
        message(STATUS "Using existing TensorFlow installation at ${TENSORFLOW_EXTRACT_DIR}")
    endif()

    set(TENSORFLOW_RUNTIME_DIR "${CMAKE_BINARY_DIR}/bin")

    # Copy runtime libraries
    if(WIN32)
        file(GLOB TF_RUNTIME_LIBS "${TENSORFLOW_LIB_DIR}/*.dll")
    else()
        file(GLOB TF_RUNTIME_LIBS 
            "${TENSORFLOW_LIB_DIR}/libtensorflow.so*"
            "${TENSORFLOW_LIB_DIR}/libtensorflow_framework.so*"
            "${TENSORFLOW_LIB_DIR}/libtensorflow.dylib*"
        )
    endif()
    
    foreach(LIB ${TF_RUNTIME_LIBS})
        get_filename_component(LIB_NAME ${LIB} NAME)
        file(COPY ${LIB} DESTINATION ${TENSORFLOW_RUNTIME_DIR})
        message(STATUS "Copied ${LIB_NAME} to runtime directory")
    endforeach()
    
    # Set the library path (platform specific)
    if(WIN32)
        set(TENSORFLOW_LIBS "${TENSORFLOW_LIB_DIR}/tensorflow.lib")
    elseif(APPLE)
        set(TENSORFLOW_LIBS "${TENSORFLOW_LIB_DIR}/libtensorflow.dylib")
    else() # Linux
        set(TENSORFLOW_LIBS "${TENSORFLOW_LIB_DIR}/libtensorflow.so" "${TENSORFLOW_LIB_DIR}/libtensorflow_framework.so")
    endif()
    
    # Set include directory
    include_directories(${TENSORFLOW_INCLUDE_DIR})
    
    # Set RPATH for Linux/macOS
    if(UNIX)
        set(CMAKE_INSTALL_RPATH "${CMAKE_BINARY_DIR}/bin")
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif()
    
    # Add definition for conditional compilation
    add_definitions(-DUSE_TENSORFLOW)
    
    # Additional linking flags for older LLVM compatibility
    if(LLVM_PACKAGE_VERSION VERSION_LESS "15.0")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-multiple-definition")
    endif()
endif()

# Source Files
file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

# Assets
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/models")

# Main Executable
add_executable(MicroSociety src/main.cpp ${SOURCES})

# Link libraries
if(USE_TENSORFLOW)
    message(STATUS "Linking with TensorFlow libraries: ${TENSORFLOW_LIBS}")
    target_link_libraries(MicroSociety PRIVATE 
        sfml-graphics sfml-audio sfml-system sfml-window 
        nlohmann_json::nlohmann_json 
        pthread
        ${TENSORFLOW_LIBS}
    )
else()
    target_link_libraries(MicroSociety PRIVATE 
        sfml-graphics sfml-audio sfml-system sfml-window 
        nlohmann_json::nlohmann_json 
        pthread
    )
endif()

# Unit Tests
file(GLOB TEST_SOURCES "tests/*.cpp")
add_executable(all_tests ${TEST_SOURCES} ${SOURCES})

# Link libraries for tests
if(USE_TENSORFLOW)
    target_link_libraries(all_tests PRIVATE 
        sfml-graphics sfml-audio sfml-system sfml-window 
        nlohmann_json::nlohmann_json 
        gtest_main 
        ${TENSORFLOW_LIBS}
    )
else()
    target_link_libraries(all_tests PRIVATE 
        sfml-graphics sfml-audio sfml-system sfml-window 
        nlohmann_json::nlohmann_json 
        gtest_main
    )
endif()

# Discover all tests
gtest_discover_tests(all_tests)