cmake_minimum_required(VERSION 3.14)
project(MicroSociety LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
    # Use MultiThreadedDLL for both Debug and Release for consistency.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

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
set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT" FORCE)
FetchContent_MakeAvailable(SFML nlohmann_json googletest)

enable_testing()
include(GoogleTest)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Add main source files, excluding main.cpp for tests
file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

# Add assets files
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Add main executable
add_executable(MicroSociety src/main.cpp ${SOURCES})

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

# Separate unit test files to avoid duplicate main definitions
add_executable(TestTerrain tests/test_terrain.cpp ${SOURCES})
target_link_libraries(TestTerrain PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json gtest_main)
gtest_discover_tests(TestTerrain)

add_executable(TestEntity tests/test_entity.cpp ${SOURCES})
target_link_libraries(TestEntity PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json gtest_main)
gtest_discover_tests(TestEntity)

add_executable(TestCollision tests/test_collision.cpp ${SOURCES})
target_link_libraries(TestCollision PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json gtest_main)
gtest_discover_tests(TestCollision)

add_executable(TestActions tests/test_actions.cpp ${SOURCES})
target_link_libraries(TestActions PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json gtest_main)
gtest_discover_tests(TestActions)

add_executable(TestInventory tests/test_inventory.cpp ${SOURCES})
target_link_libraries(TestInventory PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json gtest_main)
gtest_discover_tests(TestInventory)

add_executable(TestResourceCollection tests/test_resource_collection.cpp ${SOURCES})
target_link_libraries(TestResourceCollection PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json gtest_main)
gtest_discover_tests(TestResourceCollection)

add_executable(TestMapGeneration tests/test_map_generation.cpp ${SOURCES})
target_link_libraries(TestMapGeneration PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json gtest_main)
gtest_discover_tests(TestMapGeneration)

add_executable(TestNPCBehavior tests/test_npc_behavior.cpp ${SOURCES})
target_link_libraries(TestNPCBehavior PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json gtest_main)
gtest_discover_tests(TestNPCBehavior)

add_executable(TestTrading tests/test_trading.cpp ${SOURCES})
target_link_libraries(TestTrading PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json gtest_main)
gtest_discover_tests(TestTrading)

add_executable(TestMarket tests/test_market.cpp ${SOURCES})
target_link_libraries(TestMarket PRIVATE sfml-graphics sfml-audio sfml-system sfml-window nlohmann_json::nlohmann_json gtest_main)
gtest_discover_tests(TestMarket)