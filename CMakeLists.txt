cmake_minimum_required(VERSION 2.8.9)
project(icp)

add_compile_options(-std=c++11)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

set(target icp)
add_executable(${target} ${SOURCES})

# set_target_properties(${target} PROPERTIES COMPILE_FLAGS "-fopenmp")

include(FindOpenMP)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")