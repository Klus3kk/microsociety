cmake_minimum_required(VERSION 3.10)
project(MicroSociety)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SFML 2.5 COMPONENTS graphics audio system window REQUIRED)
find_package(TensorFlowCC REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

include_directories(include)


file(GLOB SOURCES "src/*.cpp")

add_executable(MicroSociety ${SOURCES})

target_link_libraries(SocietySimulator 
    sfml-graphics 
    sfml-audio 
    sfml-system 
    sfml-window 
    TensorFlow::TensorFlow 
    nlohmann_json::nlohmann_json
)