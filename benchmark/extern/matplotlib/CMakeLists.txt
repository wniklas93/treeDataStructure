cmake_minimum_required(VERSION 3.15)

#
# Project details
#

project(
  Matplotlib
  DESCRIPTION "Matplotlib Library"
  LANGUAGES CXX
)

add_library(${PROJECT_NAME} INTERFACE)
find_package(PythonLibs REQUIRED)

target_include_directories(${PROJECT_NAME}
                           INTERFACE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           ${PYTHON_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} INTERFACE ${PYTHON_LIBRARIES})

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

message(STATUS "Target `${PROJECT_NAME}` successfully built.")