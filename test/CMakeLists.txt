add_executable(UnitTests all_tests.cc)
target_link_libraries(UnitTests CppUTest)

add_test(UnitTests UnitTests)
