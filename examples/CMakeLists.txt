
add_executable(demo_numeric_cast
    "demo_numeric_cast.cpp"
)
##set_target_properties(${MyDemo} PROPERTIES OUTPUT_NAME "demo")

add_executable(demo_mixed_sign
    "demo_mixed_sign.cpp"
)
add_executable(demo_safe_get
    "demo_safe_get.cpp"
)
add_executable(demo_has_member
    "demo_has_member.cpp"
)

add_executable(demo_explicit
    "demo_explicit.cpp"
)

set(DemoList
    demo_numeric_cast
    demo_mixed_sign
    demo_safe_get
    demo_has_member
    demo_explicit
)

##############################################
# MACOS has some error in boost::safe_numerics
if(Boost_FOUND AND NOT APPLE)
    message(STATUS "boost lib found with version ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
    target_compile_definitions(demo_numeric_cast
        PRIVATE "-DUSE_BOOST_MULTIPRECISION")
    if(EXISTS "${Boost_INCLUDE_DIRS}/boost/safe_numerics")
    #if(Boost_VERSION VERSION_GREATER  1.68)
        add_executable(demo_boost_safe_numerics
            "demo_boost_safe_numerics.cpp"
        )
        list(APPEND DemoList demo_boost_safe_numerics)
        target_compile_definitions(demo_boost_safe_numerics 
            PRIVATE "-DUSE_BOOST_SAFE_NUMERICS")
    endif()
endif()


##########################################################
# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
# See https://cmake.org/cmake/help/v3.4/variable/CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS.html
# See https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/

if(WIN32)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

foreach(MyDemo IN LISTS DemoList)
    # Let's set compiler-specific flags
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        # G++
        if(CODE_COVERAGE)
        target_link_libraries(${MyDemo} PRIVATE coverage_config)
        endif()
        target_compile_options(${MyDemo} PRIVATE -Wall -Wextra -Wno-unused)
    elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        # clang++
        if(CODE_COVERAGE)
        target_link_libraries(${MyDemo} PRIVATE coverage_config)
        endif()
        target_compile_options(${MyDemo} PRIVATE -Wall -Wextra -Wno-unused)
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
        # MSVC
        target_compile_options(${MyDemo} PRIVATE /EHsc /MTd /W2 /c)
        # Set the DLLEXPORT variable to export symbols
        target_compile_definitions(${MyDemo} PRIVATE WIN_EXPORT)
    endif()
endforeach()