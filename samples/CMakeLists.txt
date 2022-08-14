cmake_minimum_required(VERSION 3.6)

project(samples)


if (UNIX AND NOT APPLE)
	set(CMAKE_C_FLAGS "-m64 -msse2")
elseif (UNIX AND APPLE)
    link_libraries("framework")
endif()

include_directories(../include)
link_libraries("Tilengine")
link_libraries("c")
link_libraries("z")
link_libraries("png")
link_libraries("m")
find_package(SDL2 REQUIRED)
link_libraries(${SDL2_LIBRARIES})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O2 -fpic -DLIB_EXPORTS")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-result -Wno-format-truncation")

file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(barrel 
     Barrel.c
     Simon.c)

add_executable(mode7
     Mode7.c
     Sin.c)

add_executable(platformer
     Platformer.c)

add_executable(racer
     Racer.c
     Tree.c
     Actor.c)

add_executable(scaling
     Scaling.c)

add_executable(shadow
     Shadow.c)

add_executable(shooter
     Shooter.c
     Actor.c
     Boss.c
     Enemy.c
     Explosion.c
     Ship.c
     Sin.c)

add_executable(tutorial
     Tutorial.c)

add_executable(wobble
     Wobble.c
     Sin.c)

add_executable(colorcycle
     ColorCycle.c)

add_executable(benchmark
     Benchmark.c)

add_executable(supermarioclone
     SuperMarioClone.c)

add_executable(test_mouse
     TestMouse.c)

add_executable(forest
     Forest.c)

add_executable(querylayer
     QueryLayer.c)



