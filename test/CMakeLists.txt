include_directories(
    ../include
    ../src
    ${GLIB_INCLUDE_DIRS}
    )

add_library(conversation_history_test test_conversation_history.cc)

add_executable(UnitTests all_tests.cc)
target_link_libraries(UnitTests
    conversation_history_test
    conversation_history
    CppUTest
    ${GLIB_LIBRARIES}
    )

add_test(UnitTests UnitTests)
