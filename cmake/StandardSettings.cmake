

#
# Compiler options
#

option(${PROJECT_NAME}_WARNINGS_AS_ERRORS "Treat compiler warnings as errors." ON)

#
# Experiments
#
option(${PROJECT_NAME}_ENABLE_EXPERIMENTS "Enable experiments for the project (from the `experiments` subfolder)." ON)

#
# Unit testing
#

option(${PROJECT_NAME}_ENABLE_UNIT_TESTING "Enable unit tests for the projects (from the `test` subfolder)." ON)

#
# Benchmarking
#

option(${PROJECT_NAME}_ENABLE_BENCHMARKING "Enable unit tests for the projects (from the `test` subfolder)." ON)

#
# Miscellanious options
#

option(${PROJECT_NAME}_ENABLE_CCACHE "Enable the usage of Ccache, in order to speed up rebuild times." ON)
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()