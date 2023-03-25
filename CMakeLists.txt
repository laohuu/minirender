cmake_minimum_required(VERSION 3.24)
project(raytracing)

set(CMAKE_CXX_STANDARD 17)


include_directories(${CMAKE_SOURCE_DIR}/includes)

add_executable(raytracing main.cpp)

find_package(assimp REQUIRED)
target_link_libraries(raytracing assimp::assimp)