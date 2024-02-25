# CMakeList.txt : CMake project for src

# Add source to this project's executable.
add_library(glad "src/glad.c")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET glad PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(glad
  PUBLIC "include/"
)

# TODO: Add tests and install targets if needed.
