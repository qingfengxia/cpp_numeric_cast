

add_executable(MyTest "test_to_integer.cpp")

target_link_libraries(MyTest PRIVATE coverage_config)

add_test(NAME example_test COMMAND MyTest)