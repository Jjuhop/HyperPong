# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.16)

project("GravityPong")
set(CMAKE_CONFIGURATION_TYPES "Debug;RelWithDebInfo" CACHE STRING "My multi config types" FORCE)
set(CMAKE_CXX_STANDARD 20)

# Include sub-projects.
add_subdirectory("src")

add_subdirectory("dependencies/Glad")
add_subdirectory("dependencies/GLFW")