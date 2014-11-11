extern "C" {
#include "conversation_history.h"
}

#include <CppUTest/TestHarness.h>

TEST_GROUP(ConversationHistoryTests)
{
};

TEST(ConversationHistoryTests, init_with_null_filename_should_return_null)
{
	History *history = init_history(NULL);
	History *expected = NULL;
	CHECK_EQUAL(expected, history);
}
