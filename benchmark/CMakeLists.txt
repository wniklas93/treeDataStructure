cmake_minimum_required(VERSION 3.15)

#
# Project details
#

project(
  ${CMAKE_PROJECT_NAME}Benchmarks
  LANGUAGES CXX
)


message(STATUS "Adding benchmarks under ${CMAKE_PROJECT_NAME}Benchmark...")
message("Added Benchmarks:")
foreach(file ${benchmark_sources})

  string(REGEX REPLACE "(.*/)([a-zA-Z0-9_ ]+)(\.cpp)" "\\2" benchmark_name ${file})

  add_definitions(-DWITHOUT_NUMPY) # Define macro to earse numpy dependency (Source: https://github.com/lava/matplotlib-cpp) 

  add_executable(${benchmark_name}_Benchmark ${file})
  message("\t -${benchmark_name}")

  #
  # Set the compiler standard
  #

  target_compile_features(${benchmark_name}_Benchmark PUBLIC cxx_std_20)

  #
  # Add external libraries (Model project dependencies)
  #
  add_subdirectory(extern)

  target_link_libraries(
    ${benchmark_name}_Benchmark
    PUBLIC
    Matplotlib
  )
  
  #
  # Link the executeable to the used library
  #
  
  target_link_libraries(${benchmark_name}_Benchmark PRIVATE ${CMAKE_PROJECT_NAME})

  #
  # Add the benchmarks
  #

  add_test(
    NAME
      ${benchmark_name}
    COMMAND
      ${benchmark_name}_Benchmark
  )

endforeach()

message(STATUS "Finished adding benchmarks for ${CMAKE_PROJECT_NAME}.")