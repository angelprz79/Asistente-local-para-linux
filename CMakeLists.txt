cmake_minimum_required(VERSION 3.20)
project(asistente VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(CURL REQUIRED)

include_directories(include)
include_directories(src)

add_executable(asistente
    src/main.cpp
    src/core/core.cpp
    src/ia/ia.cpp
    src/herramientas/sistema.cpp
    src/herramientas/archivos.cpp
)

target_link_libraries(asistente CURL::libcurl)

target_compile_options(asistente PRIVATE -Wall -Wextra -O2)
