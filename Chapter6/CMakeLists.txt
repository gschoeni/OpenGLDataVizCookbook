cmake_minimum_required(VERSION 3.1)
project(chapter_six)
set(CMAKE_BUILD_TYPE Release)

MESSAGE(${PROJECT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "-Wall -std=c++0x -O0 -g3")

# Project Headers
include_directories(include)
include_directories(/System/Library/Frameworks)

# Project Sources
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(chapter_six STATIC ${SOURCES})

# Third Party
find_package(OpenCV REQUIRED)
find_library(OpenGL_LIBRARY OpenGL)

include_directories(/usr/local/include/)

set(LIBS
    ${LIBS}
    ${OpenCV_LIBS}
    ${OpenGL_LIBRARY}
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
    glfw3
    SOIL
    GLEW
    assimp
    chapter_six
)

link_directories(build)

# tools
add_executable(render_model tools/render_model/main.cpp)
target_link_libraries(render_model ${LIBS} )
