

cmake_minimum_required(VERSION 3.21)

project(Multithread C CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) ## on g++ this ensures: -std=c++11 and not -std=gnu++11
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

add_subdirectory(include)
add_subdirectory(src)
