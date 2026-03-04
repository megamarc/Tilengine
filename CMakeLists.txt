cmake_minimum_required(VERSION 3.16)
project(Tilengine VERSION 2.16.0 LANGUAGES C)

file(GLOB TILENGINE_SOURCES "src/*.c")
list(FILTER TILENGINE_SOURCES EXCLUDE REGEX "(Test|Window)\\.c$")

add_library(Tilengine STATIC ${TILENGINE_SOURCES})
add_library(Tilengine::Tilengine ALIAS Tilengine)

target_include_directories(Tilengine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_definitions(Tilengine PRIVATE LIB_EXPORTS TLN_EXCLUDE_WINDOW)
target_compile_options(Tilengine PRIVATE -std=c99 -O2 -Wno-unused-result)
