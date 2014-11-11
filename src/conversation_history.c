#include "conversation_history.h"

#include <glib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

struct History {
	FILE *file;
	GHashTable *table;
};

static void init_hash_table(History *history) {
	assert(history != NULL);
	history->table = g_hash_table_new(g_str_hash, g_str_equal);
}

static void destroy_hash_table(History *history) {
	g_hash_table_destroy(history->table);
	history->table = NULL;
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
