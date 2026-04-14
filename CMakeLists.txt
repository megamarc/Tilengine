cmake_minimum_required(VERSION 4.1.2)

project(Tilengine)


file(GLOB SOURCES
     "src/*.c"
)


# main library
list(REMOVE_ITEM SOURCES "src/Test.c")
add_library(${PROJECT_NAME} ${SOURCES})

if (UNIX AND NOT APPLE)
	set(CMAKE_C_FLAGS "-m64 -msse2")
elseif (UNIX AND APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "framework")
endif()

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

find_package(PNG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE PNG::PNG)

target_include_directories(${PROJECT_NAME} PRIVATE include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O2 -fpic -DLIB_EXPORTS")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-result -Wno-format-truncation")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/Tilengine.h
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# test executable
add_executable(Test src/Test.c)
target_include_directories(Test PRIVATE include)
target_link_libraries(Test PRIVATE "Tilengine")
target_link_libraries(Test PRIVATE "m")

# samples
add_subdirectory(samples)