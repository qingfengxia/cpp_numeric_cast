

add_executable(MyTest "test_numeric_cast.cpp")

if(NOT WIN32 AND CODE_COVERAGE)
    target_link_libraries(MyTest PRIVATE coverage_config)
endif()

add_test(NAME example_test COMMAND MyTest)