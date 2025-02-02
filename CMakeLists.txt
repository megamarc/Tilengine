cmake_minimum_required(VERSION 3.6)

project(Tilengine)

# set an option to use libpng from the system, otherwise use bundled stb_image
option(TLN_OPTION_LIBPNG "Use the system libpng" ON)
option(TLN_OPTION_BUILD_LIBPNG "Build libpng from source" OFF)

# set an option to make the examples
option(TLN_OPTION_SAMPLES "Build the examples" OFF)

file(GLOB SOURCES
     "src/*.c"
)

# main library
list(REMOVE_ITEM SOURCES "src/Test.c")
add_library(${PROJECT_NAME} ${SOURCES})

if (UNIX AND NOT APPLE)
	set(CMAKE_C_FLAGS "-m64 -msse2")
endif()

if (TLN_OPTION_LIBPNG)
    # this project needs png, if png is not found, let's use fetch content
    find_package(PNG QUIET)
    if (NOT PNG_FOUND AND TLN_OPTION_BUILD_LIBPNG)
        include(FetchContent)
        FetchContent_Declare(
            libpng
            GIT_REPOSITORY https://github.com/pnggroup/libpng.git
            GIT_TAG v1.6.44
        )
        FetchContent_MakeAvailable(libpng)
        set(PNG_LIBRARIES png_static)
        add_definitions(-DTLN_HAVE_PNG)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${PNG_LIBRARIES})
    endif()
    if (NOT PNG_FOUND AND NOT TLN_OPTION_BUILD_LIBPNG)
        # clear the PNG_LIBRARY variable
        unset(PNG_LIBRARY CACHE)
    endif()
    if (PNG_FOUND)
        add_definitions(-DTLN_HAVE_PNG)
        target_include_directories(${PROJECT_NAME} PRIVATE ${PNG_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${PNG_LIBRARIES})
    endif()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE "c")
target_link_libraries(${PROJECT_NAME} PRIVATE "z")

if (TILENGINE_SAMPLES)
    find_package(SDL2 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
    add_definitions(-DTLN_HAVE_SDL2)
else()
    add_definitions(-DTLN_EXCLUDE_WINDOW)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O2 -fpic -DLIB_EXPORTS")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-result")

# gcc specific
if (UNIX AND NOT APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format-truncation")
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/Tilengine.h
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (TILENGINE_SAMPLES)
    # test executable
    add_executable(Test src/Test.c)
    target_include_directories(Test PRIVATE include)
    target_link_libraries(Test PRIVATE "Tilengine")
    target_link_libraries(Test PRIVATE "c")
    target_link_libraries(Test PRIVATE "z")
    if (TLN_OPTION_LIBPNG)
        target_include_directories(Test PRIVATE ${PNG_INCLUDE_DIRS})
        target_link_libraries(Test PRIVATE ${PNG_LIBRARIES})
    endif()
    target_link_libraries(Test PRIVATE ${PNG_LIBRARIES})
    target_link_libraries(Test PRIVATE "m")
endif()

if (TLN_OPTION_SAMPLES)
    # samples
    add_subdirectory(samples)
endif()
