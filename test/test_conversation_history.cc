extern "C" {
#include "conversation_history.h"
#include "conversation_history.c"
}

#include <CppUTest/TestHarness.h>

TEST_GROUP(ConversationHistoryInitTests)
{
};

TEST(ConversationHistoryInitTests, init_with_null_filename_should_return_null)
{
	History *history = init_history(NULL);
	History *expected = NULL;
	CHECK_EQUAL(expected, history);
}

History *create_mock_history() {
	History *history = (History *)malloc(sizeof(History));
	return history;
}

void destroy_mock_history(History *history) {
	free(history);
}

TEST_GROUP(ConversationHistoryTests)
{
	History *history;

	void setup() {
		history = create_mock_history();
	}

	void teardown() {
		destroy_mock_history(history);
	}
};

TEST(ConversationHistoryTests, has_conversation_unseen_messages_with_null_should_return_false)
{
	CHECK_EQUAL(false, has_conversation_unseen_messages(NULL, "foo", 1234));
	CHECK_EQUAL(false, has_conversation_unseen_messages(history, NULL, 1234));
}

TEST(ConversationHistoryTests, empty_history_should_not_have_unseen_messages)
{
	CHECK_EQUAL(false, has_conversation_unseen_messages(history, "foo", 1234));
	CHECK_EQUAL(false, has_conversation_unseen_messages(history, "bar", 1234));
}
