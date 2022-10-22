cmake_minimum_required(VERSION 3.16)
# C++ standard 20 required
# set(CMAKE_CXX_STANDARD 20)

# Add external sources
# SET(EXT_SOURCES ${CMAKE_CURRENT_LIST_DIR}/er-coap-13/er-coap-13.c)

#set(HEADER_LIST "${ModernCMakeExample_SOURCE_DIR}/include/coap.hpp")

# Add sources in this folder
set(NODES_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/nodes.cpp
)

add_library(libNodesCpp ${NODES_SOURCES})

target_include_directories(libNodesCpp PUBLIC ../include)

target_compile_features(libNodesCpp PUBLIC cxx_std_20)