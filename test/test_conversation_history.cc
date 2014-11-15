extern "C" {
#include "conversation_history.h"
#include "conversation_history.c"
}

#include <CppUTest/TestHarness.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

TEST_GROUP(ConversationHistoryInitTests)
{
};

TEST(ConversationHistoryInitTests, init_with_null_filename_should_return_null)
{
	History *history = init_history(NULL);
	History *expected = NULL;
	CHECK_EQUAL(expected, history);
}

TEST(ConversationHistoryInitTests, deinit_after_init_should_succeed)
{
	History *history = init_history("history_data");
	CHECK(history != NULL);
	deinit_history(history);
}

History *create_mock_history() {
	History *history = (History *)malloc(sizeof(History));
	init_hash_table(history);
	return history;
}

void destroy_mock_history(History *history) {
	destroy_hash_table(history);
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

	void write_test_file(const char *filename) {
		FILE *file = fopen(filename, "w");
		CHECK(file != NULL);
		fprintf(file, "%s\t%d\n", "foo", 1234);
		fprintf(file, "%s\t%d\n", "bar", 42);
		fprintf(file, "%s\t%d\n", "baz", 999);
		fclose(file);
	}

	void update_with_malloc_string(History *h, const char *name, time_t time) {
		size_t size = strlen(name) + 1;
		char *temp = (char *)malloc(size);
		strncpy(temp, name, size);
		update_conversation_history(h, temp, time);
		// variable "temp" will be removed by hash table inside history
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

TEST(ConversationHistoryTests, message_with_same_timestamp_should_mean_no_unseen_messages)
{
	const char *name = "foo";
	time_t time = 1234;
	update_with_malloc_string(history, name, time);
	CHECK_EQUAL(false, has_conversation_unseen_messages(history, name, time));
}

TEST(ConversationHistoryTests, message_to_different_conversation_should_mean_no_unseen_messages)
{
	const char *name = "foo";
	time_t time = 1234;
	update_with_malloc_string(history, "bar", 4433);
	CHECK_EQUAL(false, has_conversation_unseen_messages(history, name, time));
}

TEST(ConversationHistoryTests, message_to_same_conversation_should_indicate_unseen_messages)
{
	const char *name = "foo";
	time_t time = 1234;
	update_with_malloc_string(history, name, 4433);
	CHECK_EQUAL(true, has_conversation_unseen_messages(history, name, time));
}

TEST(ConversationHistoryTests, updating_conversation_after_unseen_messages_should_give_no_unseen_messages)
{
	const char *name = "foo";
	time_t time = 1234;
	time_t new_time = 4433;
	update_with_malloc_string(history, name, new_time);
	CHECK_EQUAL(false, has_conversation_unseen_messages(history, name, new_time));
	CHECK_EQUAL(true, has_conversation_unseen_messages(history, name, time));
	update_with_malloc_string(history, name, time);
	CHECK_EQUAL(true, has_conversation_unseen_messages(history, name, new_time));
	CHECK_EQUAL(false, has_conversation_unseen_messages(history, name, time));
}

TEST(ConversationHistoryTests, reading_history_from_file_should_return_proper_unseen_status)
{
	const char *filename = "history_data";
	write_test_file(filename);
	CHECK_EQUAL(true, read_history_from_file(filename, history));
	CHECK_EQUAL(false, has_conversation_unseen_messages(history, "foo", 1234));
	CHECK_EQUAL(false, has_conversation_unseen_messages(history, "bar", 42));
	CHECK_EQUAL(false, has_conversation_unseen_messages(history, "baz", 999));
	CHECK_EQUAL(true, has_conversation_unseen_messages(history, "foo", 42));
	CHECK_EQUAL(true, has_conversation_unseen_messages(history, "bar", 1234));
	CHECK_EQUAL(true, has_conversation_unseen_messages(history, "baz", 342));
	CHECK_EQUAL(false, has_conversation_unseen_messages(history, "none", 1234));
}

TEST(ConversationHistoryTests, writing_history_to_file_should_be_readable)
{
	const char *filename = "history_data";
	update_with_malloc_string(history, "foo", 1234);
	update_with_malloc_string(history, "bar", 42);
	update_with_malloc_string(history, "baz", 4433);
	update_with_malloc_string(history, "abc", 1234);

	CHECK_EQUAL(true, write_history_to_file(filename, history));
	History *new_history = create_mock_history();
	CHECK_EQUAL(true, read_history_from_file(filename, new_history));

	CHECK_EQUAL(false, has_conversation_unseen_messages(new_history, "foo", 1234));
	CHECK_EQUAL(false, has_conversation_unseen_messages(new_history, "bar", 42));
	CHECK_EQUAL(false, has_conversation_unseen_messages(new_history, "baz", 4433));
	CHECK_EQUAL(false, has_conversation_unseen_messages(new_history, "abc", 1234));

	CHECK_EQUAL(true, has_conversation_unseen_messages(new_history, "foo", 4433));
	CHECK_EQUAL(true, has_conversation_unseen_messages(new_history, "bar", 4433));
	CHECK_EQUAL(true, has_conversation_unseen_messages(new_history, "baz", 1234));
	CHECK_EQUAL(true, has_conversation_unseen_messages(new_history, "abc", 42));
	destroy_mock_history(new_history);
}

TEST(ConversationHistoryTests, non_existing_conversation_should_not_be_in_history)
{
	CHECK_EQUAL(false, is_conversation_in_history(history, "foo"));
	CHECK_EQUAL(false, is_conversation_in_history(history, "bar"));
}

TEST(ConversationHistoryTests, conversation_should_be_in_history_after_update)
{
	CHECK_EQUAL(false, is_conversation_in_history(history, "foo"));
	CHECK_EQUAL(false, is_conversation_in_history(history, "bar"));
	update_with_malloc_string(history, "foo", 1);
	CHECK_EQUAL(true, is_conversation_in_history(history, "foo"));
	CHECK_EQUAL(false, is_conversation_in_history(history, "bar"));
	update_with_malloc_string(history, "bar", 1);
	CHECK_EQUAL(true, is_conversation_in_history(history, "foo"));
	CHECK_EQUAL(true, is_conversation_in_history(history, "bar"));
}
