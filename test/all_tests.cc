/**
 * Pidgin remember read status plugin
 * Copyright (C) 2014 Miika-Petteri Matikainen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
*/
#include "CppUTest/CommandLineTestRunner.h"

IMPORT_TEST_GROUP(ConversationHistoryInitTests);
IMPORT_TEST_GROUP(ConversationHistoryTests);

int main(int argc, char **argv) {
	return RUN_ALL_TESTS(argc, argv);
}
