#include "conversation_history.h"

#include <glib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

struct History {
	FILE *file;
	GHashTable *table;
};

static void init_hash_table(History *history) {
	assert(history != NULL);
	history->table = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);
}

static void destroy_hash_table(History *history) {
	g_hash_table_destroy(history->table);
	history->table = NULL;
}

static bool read_history_from_file(const char *filename, History *history) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		return false;
	}

	size_t max_line_len = 1024;
	char buffer[max_line_len];
	while (fgets(buffer, max_line_len, file)) {
		char *name = (char *)malloc(max_line_len);
		time_t time;
		int args = sscanf(buffer, "%s\t%lu", name, &time);
		if (args != 2) {
			return false;
		}
		update_conversation_history(history, name, time);
	}

	fclose(file);
	return true;
}

History *init_history(const char *filename) {
	return NULL;
}

bool has_conversation_unseen_messages(const History *history,
		                      const char *conversation_name,
				      time_t message_time) {
	if (!history || !conversation_name) {
		return false;
	}

	GHashTable *table = history->table;
	gpointer value = g_hash_table_lookup(table, conversation_name);
	if (!value) {
		return false;
	}

	return message_time != (time_t)value;
}

void update_conversation_history(History *history,
		                 const char *conversation_name,
				 time_t message_time) {
	assert(history);
	GHashTable *table = history->table;
	g_hash_table_insert(table, (gpointer)conversation_name, GINT_TO_POINTER(message_time));
}
