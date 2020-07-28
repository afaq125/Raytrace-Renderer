
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_CONFIGURATION_TYPES Release Debug)
set(CMAKE_BUILD_TYPE Release ... FORCE)

project(RaytracerRenderer CXX)
cmake_minimum_required(VERSION 3.16)

find_path(
    FREEIMAGE_INCLUDE_DIR
    NAMES "FreeImage.h"
    HINTS ${FreeImage_ROOT})

find_library(
    FREEIMAGE_LIBRARY 
    NAMES "FreeImage" 
    HINTS ${FreeImage_ROOT})

include_directories(${FREEIMAGE_INCLUDE_DIR})

add_executable(
    RaytraceRenderer
    Include/Constants.h
    Include/Matrix.h
    Include/RayTracer.h
    Include/Renderer.h
    Include/Singleton.h
    Include/ThreadPool.h
    Include/Vector.h
    Source/Matrix.cpp
    Source/Renderer.cpp
    Source/Singleton.cpp
    Source/Vector.cpp
    Source/Main.cpp)

target_include_directories(
    RaytraceRenderer
    PUBLIC
    Include)

target_link_libraries(
    RaytraceRenderer 
    PUBLIC 
    ${FREEIMAGE_LIBRARY})

target_precompile_headers(
    RaytraceRenderer
    PUBLIC
    Include/Renderer.h)

