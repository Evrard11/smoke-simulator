cmake_minimum_required(VERSION 3.10)
project(smoke-simulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_subdirectory(2D)
add_subdirectory(3D)