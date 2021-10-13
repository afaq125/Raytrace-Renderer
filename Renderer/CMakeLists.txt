
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_CONFIGURATION_TYPES Release Debug)
set(CMAKE_BUILD_TYPE Release ... FORCE)

project(RaytracerRenderer CXX)
cmake_minimum_required(VERSION 3.16)

#
# Paths and variables
#

set(PROJECT_DIR
    ${CMAKE_CURRENT_SOURCE_DIR})

set(INCLUDE_FILES
    ${PROJECT_DIR}/Include)

set(SOURCE_FILES
    ${PROJECT_DIR}/Include/AsyncQueue.h
    ${PROJECT_DIR}/Include/Camera.h
    ${PROJECT_DIR}/Include/Constants.h
    ${PROJECT_DIR}/Include/Error.h
    ${PROJECT_DIR}/Include/Lights.h
    ${PROJECT_DIR}/Include/Logger.h
    ${PROJECT_DIR}/Include/Matrix.h
    ${PROJECT_DIR}/Include/Objects.h
    ${PROJECT_DIR}/Include/RayTracer.h
    ${PROJECT_DIR}/Include/Renderer.h
    ${PROJECT_DIR}/Include/Shader.h
    ${PROJECT_DIR}/Include/Singleton.h
    ${PROJECT_DIR}/Include/ThreadPool.h
    ${PROJECT_DIR}/Include/Utilities.h
    ${PROJECT_DIR}/Include/Vector.h
    ${PROJECT_DIR}/Source/Camera.cpp
    ${PROJECT_DIR}/Source/Lights.cpp
    ${PROJECT_DIR}/Source/Logger.cpp
    ${PROJECT_DIR}/Source/Matrix.cpp
    ${PROJECT_DIR}/Source/Objects.cpp
    ${PROJECT_DIR}/Source/RayTracer.cpp
    ${PROJECT_DIR}/Source/Renderer.cpp
    ${PROJECT_DIR}/Source/Shader.cpp
    ${PROJECT_DIR}/Source/Utilities.cpp
    ${PROJECT_DIR}/Source/Vector.cpp)

set(TEST_INCLUDE_FILES
    ${INCLUDE_FILES}
    ${PROJECT_DIR}/Tests)

set(TEST_SOURCE_FILES
    ${PROJECT_DIR}/Tests/Tests.h
    ${PROJECT_DIR}/Tests/TestUtilities.h
    ${PROJECT_DIR}/Tests/RendererTest.cpp
    ${PROJECT_DIR}/Tests/Tests.cpp
    ${PROJECT_DIR}/Tests/TestUtilities.cpp
    ${PROJECT_DIR}/Tests/VectorTest.cpp)

set(GTEST_DIR 
    ${PROJECT_DIR}/../Dependencies/GoogleTest/googletest)

set(TARGET_NAME ${PROJECT_NAME})

#
# Renderer lib
#

add_library(
    ${TARGET_NAME}
    STATIC
    ${SOURCE_FILES})

target_include_directories(
    ${TARGET_NAME}
    PRIVATE
    ${INCLUDE_FILES})

target_precompile_headers(
    ${TARGET_NAME}
    PUBLIC
    ${INCLUDE_FILES}/Renderer.h)

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
    ${TEST_SOURCE_FILES})

target_include_directories(
    Tests
    PUBLIC
    ${TEST_INCLUDE_FILES})

target_link_libraries(
    Tests
    ${TARGET_NAME}
    ${FREEIMAGE_LIBRARY}
    GoogleTest)

