/**
 * Pidgin remember read status plugin
 * Copyright (C) 2014 Miika-Petteri Matikainen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
*/
#include "conversation_history.h"

#include <glib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

struct History {
	char *filename;
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

static void init_filename(History *history, const char *filename) {
	size_t size = strlen(filename) + 1;
	history->filename = (char *)malloc(size);
	strncpy(history->filename, filename, size);
}

static void destroy_filename(History *history) {
	free(history->filename);
	history->filename = NULL;
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
		int args = sscanf(buffer, "%512s\t%32lu", name, &time);
		if (args != 2) {
			free(name);
			fclose(file);
			return false;
		}
		update_conversation_history(history, name, time);
	}

	fclose(file);
	return true;
}

static bool write_history_to_file(const char *filename, const History *history) {
	FILE *file = fopen(filename, "w");
	if (!file) {
		return false;
	}

	GHashTableIter iter;
	gpointer key;
	gpointer value;

	g_hash_table_iter_init(&iter, history->table);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		fprintf(file, "%s\t%lu\n", (const char *)key, (time_t)value);
	}

	fclose(file);
	return true;
}

static void print_history(const History *history) {
	GHashTableIter iter;
	gpointer key;
	gpointer value;

	g_hash_table_iter_init(&iter, history->table);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		printf("Name: \"%s\" timestamp: %lu\n", (const char *)key, (time_t)value);
	}
}

static void free_history(History *history) {
	destroy_hash_table(history);
	destroy_filename(history);
	free(history);
}

static void create_file_if_it_doesnt_exist(const char *filename) {
	FILE *file = fopen(filename, "ab+");
	if (file) {
		fclose(file);
	}
}

History *init_history(const char *filename) {
	if (!filename) {
		return NULL;
	}
	History *history = (History *)malloc(sizeof(History));
	init_hash_table(history);
	init_filename(history, filename);

	create_file_if_it_doesnt_exist(history->filename);
	if (!read_history_from_file(history->filename, history)) {
		free_history(history);
		return NULL;
	}
	return history;
}

void deinit_history(History *history) {
	if (!history) {
		return;
	}
	write_history_to_file(history->filename, history);
	free_history(history);
}

bool is_conversation_in_history(const History *history,
                                const char *conversation_name) {
	if (!history || !conversation_name) {
		return false;
	}

	GHashTable *table = history->table;
	gpointer value = g_hash_table_lookup(table, conversation_name);
	return value != NULL;
}

bool has_conversation_history_unseen_messages(const History *history,
                                              const char *conversation_name,
                                              time_t message_time) {
	if (!is_conversation_in_history(history, conversation_name)) {
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
