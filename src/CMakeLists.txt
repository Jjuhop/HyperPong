# CMakeList.txt : CMake project for src

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "main.cpp" "game.cpp" "game.h"
	"Utils/logger.cpp" "Utils/logger.h" "Utils/matrix.cpp" "Utils/matrix.h"
	"OpenGL/renderer.cpp" "OpenGL/renderer.h"
 "OpenGL/shader.cpp" "OpenGL/shader.h" "projectile.cpp" "projectile.h" "playerBar.cpp" "playerBar.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(${PROJECT_NAME}
	PUBLIC glad
	PUBLIC "${CMAKE_SOURCE_DIR}/dependencies/GLFW/include"
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC glad
	PUBLIC glfw
)

target_precompile_headers(${PROJECT_NAME}
  PRIVATE
	<array>
    <chrono>
	<fstream>
	<iostream>
	<random>
	<string>
	<unordered_map>
	<vector>
)

# TODO: Add tests and install targets if needed.
