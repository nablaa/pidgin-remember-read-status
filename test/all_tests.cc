#include "CppUTest/CommandLineTestRunner.h"

IMPORT_TEST_GROUP(ConversationHistoryInitTests);
IMPORT_TEST_GROUP(ConversationHistoryTests);

int main(int argc, char **argv) {
	return RUN_ALL_TESTS(argc, argv);
}
