cmake_minimum_required(VERSION 2.8)

# comment this out, if later used as add_subdirectory()
project(CppToSigned)

set(PROJECT_BRIEF "provide std::to_integer(), and std::to_unsigned template functions for C++,  by Qingfeng Xia")
#project version definition

set(CMAKE_BUILD_TYPE Debug)


# std can and should be applied to target only
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#############################################################

add_executable(MyTest "tests/test_to_integer.cpp")

##############################################################
set(MyDemo "demo")
add_executable(${MyDemo}
    demo.cpp
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
    target_compile_options(${MyDemo} PRIVATE -Wall -Wextra)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    # clang++
    target_compile_options(${MyDemo} PRIVATE -Wall -Wextra)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    # MSVC
    target_compile_options(${MyDemo} PRIVATE /EHsc /MTd /W2 /c)
    # Set the DLLEXPORT variable to export symbols
    target_compile_definitions(${MyDemo} PRIVATE WIN_EXPORT)
endif()

