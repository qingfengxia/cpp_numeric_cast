cmake_minimum_required(VERSION 2.8)

# comment this out, if later used as add_subdirectory()
project(CppToInteger)

set(PROJECT_BRIEF "provide std::to_integer(), and std::to_unsigned template functions for C++,  by Qingfeng Xia")
#project version definition

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# std can and should be applied to target only
if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#################################
option(BUILD_TESTING "Build the testing tree." ON)
option(CODE_COVERAGE "Enable coverage reporting" OFF)
###################################
# Code Coverage Configuration

if(NOT WIN32 AND CODE_COVERAGE)
#if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_library(coverage_config INTERFACE)
  # Add required flags (GCC & LLVM/Clang)
  message(STATUS "coverage config has been integrated into this project")
  target_compile_options(coverage_config INTERFACE
    -O0        # no optimization
    -g         # generate debug info
    --coverage # sets all required flags
  )
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(coverage_config INTERFACE --coverage)
  else()
    target_link_libraries(coverage_config INTERFACE --coverage)
  endif()
endif()

########################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
find_package(Boost 1.65)


############################ demo #############

add_subdirectory(examples)

############### unit test ######################GB
# Only build tests if we are the top-level project
# Allows this to be used by super projects with `add_subdirectory`
if (BUILD_TESTING AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
  enable_testing()
  add_subdirectory(tests)
endif()



