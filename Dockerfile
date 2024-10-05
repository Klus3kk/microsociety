cmake_minimum_required(VERSION 3.14)
project(MicroSociety)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Znalezienie TensorFlow C API
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

# Automatyczne pobieranie SFML i nlohmann_json przez FetchContent
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

# Pliki źródłowe projektu
file(GLOB SOURCES "src/*.cpp")

# Tworzenie pliku wykonywalnego
add_executable(MicroSociety ${SOURCES})

# Linkowanie bibliotek TensorFlow, SFML i JSON
target_link_libraries(MicroSociety
    sfml-graphics
    sfml-audio
    sfml-system
    sfml-window
    nlohmann_json::nlohmann_json
    tensorflow
)
