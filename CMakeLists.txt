cmake_minimum_required(VERSION 3.15)

#
# Project details
#

project(
    Tree_Library
    DESCRIPTION "Library for a varying data type tree data structure for embedded devices"
    VERSION 0.1.0
    LANGUAGES CXX
)

set (CMAKE_CXX_STANDARD 20)

message(STATUS "Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...\n")

#
# Set compiler options and build options
#

include(cmake/StandardSettings.cmake)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_NAME})

message(STATUS "Set compiler options and build options \n")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

message(STATUS "\"CMAKE_BUILD_TYPE\"=${CMAKE_BUILD_TYPE}")

#
# Prevent building in the source directory
#

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.\n")
endif()


#
# Create library, setup header and source files
#

# Find all headers and source files
include(cmake/SourcesAndHeaders.cmake)

message(STATUS "Build library:")
add_library(
    ${PROJECT_NAME}
    STATIC
    ${headers}
    ${sources})

message("\t Found the following sources:")
foreach(source IN LISTS sources)
  message("\t\t * ${source}")
endforeach()
message("\t Found the following headers:")
foreach(header IN LISTS headers)
  message("\t\t * ${header}")
endforeach()

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
)

message(STATUS "Added all header and source files.\n")

#
# Model project dependencies (Libraries)
#

# no external dependencies exist

#
# Add library to project
#

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/sr
    INTERFACE
  )

# message(STATUS "Finished setting up include directories.")
# add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

message(STATUS "Added library ${PROJECT_NAME}")

#
# Unit testing setup
#

if(${PROJECT_NAME}_ENABLE_UNIT_TESTING)
  enable_testing()
  message(STATUS "Build unit tests for the project. Tests should always be found in the `test` folder\n")
  add_subdirectory(test)
endif()


#
# Experiment setup
#

if(${PROJECT_NAME}_ENABLE_EXPERIMENTS)
  message(STATUS "Build experiments for the project. Experiments should always be found in the `experiments` folder\n")
  add_subdirectory(experiments)
endif()