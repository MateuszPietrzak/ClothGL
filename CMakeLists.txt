cmake_minimum_required(VERSION 3.13)
project(ClothGL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH /usr/local/lib/cmake)
set(CMAKE_PREFIX_PATH /usr/local/lib/cmake/glfw3)

find_package (PkgConfig REQUIRED)
find_package (OpenGL REQUIRED)
find_package (glfw3 REQUIRED)
find_package (glm REQUIRED)

include_directories(/usr/local/include glad KHR)
include_directories(${GLFW_INCLUDE_DIRS})


set(SOURCE_FILES src/cpp/main.cpp)
add_executable(ClothGL src/cpp/main.cpp src/cpp/glad.c src/cpp/Shader.cpp include/Shader.h src/cpp/Ball.cpp include/Ball.h src/cpp/Constraint.cpp include/Constraint.h src/cpp/Force.cpp include/Force.h src/cpp/Texture.cpp include/Texture.h include/stb_image.h)
target_link_libraries (ClothGL
        ${OPENGL_LIBRARIES}
        ${GLFW3_LIBRARY}
        glfw)