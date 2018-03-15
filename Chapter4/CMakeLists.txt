cmake_minimum_required(VERSION 3.1)
project(chapter_four)
set(CMAKE_BUILD_TYPE Release)

MESSAGE(${PROJECT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "-Wall -std=c++0x -O0 -g3")

# Project Headers
include_directories(include)
include_directories(/System/Library/Frameworks)

# Project Sources
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(chapter_four STATIC ${SOURCES})

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
    chapter_four
)

link_directories(build)

# tools
add_executable(simple tools/simple/main.cpp)
target_link_libraries(simple ${LIBS} )

add_executable(texture_mapping tools/texture_mapping/main.cpp)
target_link_libraries(texture_mapping ${LIBS} )

add_executable(video_processing tools/video_processing/main.cpp)
target_link_libraries(video_processing ${LIBS} )
