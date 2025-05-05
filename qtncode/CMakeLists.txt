cmake_minimum_required(VERSION 3.10)
project(polygonExtrusion)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
 
 
# Enable Qt support
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)    
 
 
include_directories(${INCLUDE_DIRECTORIES})
# Include directories
set(INCLUDE_DIRECTORIES
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_SOURCE_DIR}/headers
)
 
include_directories(${INCLUDE_DIRECTORIES})
 
# Source files
file(GLOB APPLICATION_SRC
    ${CMAKE_SOURCE_DIR}/src/mainwindow.cpp
    ${CMAKE_SOURCE_DIR}/headers/mainwindow.h
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)
 
file(GLOB OPENGL_SRC
    ${CMAKE_SOURCE_DIR}/src/openglwidget.cpp
    ${CMAKE_SOURCE_DIR}/headers/openglwidget.h
)
list(APPEND APPLICATION_SRC ${OPENGL_SRC})
 

# Find Qt
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(OpenGL REQUIRED)
 
add_library(geometry SHARED ${GEOMETRY_SRC})
target_link_libraries(geometry Qt6::Widgets)
 
add_executable(main ${APPLICATION_SRC} "src/mainwindow.cpp")
target_link_libraries(main geometry Qt6::Widgets Qt6::OpenGLWidgets OpenGL::GL)