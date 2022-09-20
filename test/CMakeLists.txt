cmake_minimum_required(VERSION 3.15)

#
# Project details
#

project(
  ${CMAKE_PROJECT_NAME}Tests
  LANGUAGES CXX
)

enable_testing()

message(STATUS "Adding tests under ${CMAKE_PROJECT_NAME}Tests...")
message("Added Tests:")
foreach(file ${test_sources})

  string(REGEX REPLACE "(.*/)([a-zA-Z0-9_ ]+)(\.cpp)" "\\2" test_name ${file}) 
  add_executable(${test_name}_Tests ${file})
  message("\t -${test_name}")

  #
  # Set the compiler standard
  #

  target_compile_features(${test_name}_Tests PUBLIC cxx_std_20)

  #
  # Load the desired unit testing framework
  #

  find_package(ut REQUIRED)

  target_link_libraries(
    ${test_name}_Tests
    PRIVATE
    Boost::ut
  )

  #
  # Add the unit tests
  #

  add_test(
    NAME
      ${test_name}
    COMMAND
      ${test_name}_Tests
  )

endforeach()

message(STATUS "Finished adding unit tests for ${CMAKE_PROJECT_NAME}.")