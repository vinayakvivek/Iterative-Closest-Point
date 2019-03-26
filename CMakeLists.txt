cmake_minimum_required(VERSION 2.8.9)
project(icp)

add_compile_options(-std=c++11)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

set(target icp)
add_executable(${target} ${SOURCES})