cmake_minimum_required(VERSION 3.15)
project(threadPool)

file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)
add_executable(app ${SRC})
