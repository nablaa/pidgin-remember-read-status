include_directories(
    ../include
    )

add_library(conversation_history_test test_conversation_history.cc)

add_executable(UnitTests all_tests.cc)
target_link_libraries(UnitTests conversation_history_test CppUTest)

add_test(UnitTests UnitTests)
