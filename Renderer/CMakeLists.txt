
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
    ${SOURCE_DIR}/Source/Vector.cpp)

SET(TEST_FILES
    ${SOURCE_DIR}/Tests/Tests.h
    ${SOURCE_DIR}/Tests/Tests.cpp
    ${SOURCE_DIR}/Tests/TestUtilities.h
    ${SOURCE_DIR}/Tests/TestUtilities.cpp
    ${SOURCE_DIR}/Tests/VectorTest.h
    ${SOURCE_DIR}/Tests/VectorTest.cpp
    ${SOURCE_DIR}/Tests/RendererTest.h
    ${SOURCE_DIR}/Tests/RendererTest.cpp)

SET(TARGET_NAME ${PROJECT_NAME})

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
# Renderer lib
#

add_library(
    ${TARGET_NAME}
    STATIC
    ${SOURCE_FILES})

target_include_directories(
    ${TARGET_NAME}
    PUBLIC
    ${SOURCE_DIR}/Include)

target_precompile_headers(
    ${TARGET_NAME}
    PUBLIC
    ${SOURCE_DIR}/Include/Renderer.h)

#
# GoogleTest lib
#

add_library(
    GoogleTest
    STATIC 
    ${GTEST_DIR}/src/gtest_main.cc ${GTEST_DIR}/src/gtest-all.cc)

target_include_directories(
    GoogleTest 
    PRIVATE 
    ${GTEST_DIR})

target_include_directories(
    GoogleTest 
    PUBLIC 
    ${GTEST_DIR}/include)

#
# Test executable
#

add_executable(
    Tests
    ${TEST_FILES})

target_include_directories(
    Tests
    PUBLIC
    ${SOURCE_DIR}/Include)

target_link_libraries(
    Tests
    ${TARGET_NAME}
    ${FREEIMAGE_LIBRARY}
    GoogleTest)

