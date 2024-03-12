cmake_minimum_required (VERSION 3.16)

project("HyperPong")
set(CMAKE_CXX_STANDARD 20)

add_subdirectory("src")

add_subdirectory("dependencies/Glad")
add_subdirectory("dependencies/GLFW")