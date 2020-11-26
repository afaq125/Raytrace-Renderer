
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_CONFIGURATION_TYPES Release Debug)
set(CMAKE_BUILD_TYPE Release ... FORCE)

project(RaytracerRenderer CXX)
cmake_minimum_required(VERSION 3.16)

#
# Paths and variables
#

SET(SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR})

set(TEST_SRC_DIR 
    ${SOURCE_DIR}/Tests) 

set(GTEST_DIR 
    ${SOURCE_DIR}/../Dependencies/GoogleTest/googletest)

SET(SOURCE_FILES
    ${SOURCE_DIR}/Include/Camera.h
    ${SOURCE_DIR}/Include/Constants.h
    ${SOURCE_DIR}/Include/Lights.h
    ${SOURCE_DIR}/Include/Matrix.h
    ${SOURCE_DIR}/Include/Objects.h
    ${SOURCE_DIR}/Include/RayTracer.h
    ${SOURCE_DIR}/Include/Renderer.h
    ${SOURCE_DIR}/Include/Shader.h
    ${SOURCE_DIR}/Include/Singleton.h
    ${SOURCE_DIR}/Include/ThreadPool.h
    ${SOURCE_DIR}/Include/Utilities.h
    ${SOURCE_DIR}/Include/Vector.h
    ${SOURCE_DIR}/Source/Camera.cpp
    ${SOURCE_DIR}/Source/Lights.cpp
    ${SOURCE_DIR}/Source/Matrix.cpp
    ${SOURCE_DIR}/Source/Objects.cpp
    ${SOURCE_DIR}/Source/RayTracer.cpp
    ${SOURCE_DIR}/Source/Renderer.cpp
    ${SOURCE_DIR}/Source/Shader.cpp
    ${SOURCE_DIR}/Source/Singleton.cpp
    ${SOURCE_DIR}/Source/Utilities.cpp
    ${SOURCE_DIR}/Source/Vector.cpp
    ${SOURCE_DIR}/Source/Main.cpp)

SET(TEST_FILES
    ${SOURCE_DIR}/Tests/TestVector.h
    ${SOURCE_DIR}/Tests/TestVector.cpp)

#
# FreeImage
#

find_path(FREEIMAGE_INCLUDE_DIR
    NAMES "FreeImage.h"
    HINTS ${FreeImage_ROOT})

find_library(FREEIMAGE_LIBRARY 
    NAMES "FreeImage" 
    HINTS ${FreeImage_ROOT})

include_directories(${FREEIMAGE_INCLUDE_DIR})

#
# Renderer executable
#

add_executable(RaytraceRenderer 
    ${SOURCE_FILES})

target_include_directories(RaytraceRenderer
    PUBLIC
    ${SOURCE_DIR}/Include)

target_link_libraries(RaytraceRenderer 
    PUBLIC 
    ${FREEIMAGE_LIBRARY})

target_precompile_headers(RaytraceRenderer
    PUBLIC
    ${SOURCE_DIR}/Include/Renderer.h)

#
# Renderer lib
#

add_library(RaytraceRenderer-Lib
    STATIC
    ${SOURCE_FILES})

target_include_directories(RaytraceRenderer-Lib
    PUBLIC
    ${SOURCE_DIR}/Include)

target_link_libraries(RaytraceRenderer-Lib 
    PUBLIC 
    ${FREEIMAGE_LIBRARY})

target_precompile_headers(RaytraceRenderer-Lib
    PUBLIC
    ${SOURCE_DIR}/Include/Renderer.h)

#
# GoogleTest lib
#

add_library(GoogleTest
    STATIC 
    ${GTEST_DIR}/src/gtest_main.cc ${GTEST_DIR}/src/gtest-all.cc)

target_include_directories(GoogleTest 
    PRIVATE 
    ${GTEST_DIR})

target_include_directories(GoogleTest 
    PUBLIC 
    ${GTEST_DIR}/include)

#
# Test executable
#

add_executable(RaytraceRenderer-Test
        ${TEST_FILES})

target_include_directories(RaytraceRenderer-Test
    PUBLIC
    ${SOURCE_DIR}/Include)

target_link_libraries(RaytraceRenderer-Test
    RaytraceRenderer-Lib 
    GoogleTest)

