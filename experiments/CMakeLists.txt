

cmake_minimum_required(VERSION 3.15)

#
# Project details
#

project(
  ${CMAKE_PROJECT_NAME}_Experiments
  LANGUAGES CXX
)

message("Adding tests under ${CMAKE_PROJECT_NAME}_Experiments...")

foreach(file ${experiments_sources})
  string(REGEX REPLACE "(.*/)([a-zA-Z0-9_ ]+)(\.cpp)" "\\2" experiment_name ${file})
  add_executable(${experiment_name} ${file})


 #
 # Set the compiler standard
 #

 target_compile_features(${experiment_name} PUBLIC cxx_std_20)

 #
 # Link the executeable to the used library
 #
 target_link_libraries(${experiment_name} PRIVATE ${CMAKE_PROJECT_NAME})
  
endforeach()

message("Finished adding experiments for ${CMAKE_PROJECT_NAME}.")
