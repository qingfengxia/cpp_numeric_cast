cmake_minimum_required(VERSION 2.8)

# comment this out, if later used as add_subdirectory()
project(CppToSigned)

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

#############################################################

add_executable(MyTest "tests/test_to_integer.cpp")

##############################################################
set(MyDemo "demo")
add_executable(${MyDemo}
    demo.cpp
)

add_executable(demo_mixed_sign
    tests/demo_mixed_sign.cpp
)

##set_target_properties(${MyDemo} PROPERTIES OUTPUT_NAME "demo")


##########################################################
# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
# See https://cmake.org/cmake/help/v3.4/variable/CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS.html
# See https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/

if(WIN32)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Let's set compiler-specific flags
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    # G++
    target_compile_options(${MyDemo} PRIVATE -Wall -Wextra -Wno-unused)
    target_compile_options(demo_mixed_sign PRIVATE -Wall -Wextra -Wno-unused)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    # clang++
    target_compile_options(${MyDemo} PRIVATE -Wall -Wextra -Wno-unused)
    target_compile_options(demo_mixed_sign PRIVATE -Wall -Wextra -Wno-unused)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    # MSVC
    target_compile_options(${MyDemo} PRIVATE /EHsc /MTd /W2 /c)
    # Set the DLLEXPORT variable to export symbols
    target_compile_options(demo_mixed_sign PRIVATE /EHsc /MTd /W2 /c)
    target_compile_definitions(${MyDemo} PRIVATE WIN_EXPORT)
endif()

