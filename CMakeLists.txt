cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)

# add options for testing
option(ENABLE_COVERAGE "Enable code coverage testing." OFF)
option(ENABLE_MEMCHECK "Enable testing for memory leaks." OFF)
option(ENABLE_PROFILING "Enable valgrind profiling." OFF)

# define project name
project (scrawble VERSION 0.1.0)

# set path to custom modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_program(HEADERDOC headerdoc2html)
find_program(GATHERDOC gatherheaderdoc)
find_program(DOXYGEN doxygen)

include(FindCurses)

# add target for code coverage
if(ENABLE_COVERAGE)
	include(CodeCoverage)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_COVERAGE}")
	setup_target_for_coverage(TARGET ${PROJECT_NAME}-coverage OUTPUT ${PROJECT_BINARY_DIR}/gen/coverage)
endif()

set(TEST_PROJECT_NAME "${PROJECT_NAME}_test")

set(VENDOR_PATH ${CMAKE_CURRENT_LIST_DIR}/vendor)

include_directories(src)

# add directories
add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(tests)
