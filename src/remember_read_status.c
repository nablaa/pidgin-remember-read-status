/**
 * Pidgin remember read status plugin
 * Copyright (C) 2014 Miika-Petteri Matikainen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
*/
#define PURPLE_PLUGINS

#include "conversation_history.h"

#include <glib.h>

#include "notify.h"
#include "plugin.h"
#include "version.h"
#include "gtkplugin.h"
#include "gtkconv.h"
#include "debug.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define PLUGIN_ID "gtk-nablaa-remember-read-status"
#define PLUGIN_VERSION "0.1.0-dev"
#define HISTORY_FILENAME ".pidgin-remember-read-status-history"

static History *history = NULL;

static bool is_disconnect_message(const char *msg) {
	return strstr(msg, "The account has disconnected and you are no longer in this chat.") != NULL;
}

static time_t get_latest_conversation_message_timestamp(PurpleConversation *conv) {
	time_t latest = 0;
	GList *history_list = purple_conversation_get_message_history(conv);
	for (GList *l = history_list; l != NULL; l = l->next) {
		PurpleConvMessage *message = (PurpleConvMessage *)l->data;
		const char *msg = purple_conversation_message_get_message(message);
		time_t timestamp = purple_conversation_message_get_timestamp(message);

		// Skip disconnect messages
		if (is_disconnect_message(msg)) {
			continue;
		}

		if (latest == 0 || timestamp > latest) {
			latest = timestamp;
		}
	}
	return latest;
}

static bool are_unseen_messages_in_conversation(PurpleConversation *conv) {
	gpointer count_data = purple_conversation_get_data(conv, "unseen-count");
	gpointer state_data = purple_conversation_get_data(conv, "unseen-state");
	int unseen_count = 0;
	PidginUnseenState unseen_state = PIDGIN_UNSEEN_NONE;

	if (count_data) {
		unseen_count = GPOINTER_TO_INT(count_data);
	}
	if (state_data) {
		unseen_state = GPOINTER_TO_INT(state_data);
	}

	return unseen_count != 0 ||
		(unseen_state != PIDGIN_UNSEEN_NONE && unseen_state != PIDGIN_UNSEEN_EVENT);
}

static void chat_left_cb(PurpleConversation *conv, const char *conv_name) {
	if (history == NULL) {
		purple_debug_misc(PLUGIN_ID, "%s: Cannot update conversation history: no history file\n", conv_name);
		return;
	}

	if (are_unseen_messages_in_conversation(conv)) {
		purple_debug_misc(PLUGIN_ID, "%s: Not updating conversation history: unseen messages in conversation\n", conv_name);
		return;
	}


	time_t message_timestamp = get_latest_conversation_message_timestamp(conv);
	if (message_timestamp == 0) {
		purple_debug_misc(PLUGIN_ID, "%s: Not updating conversation history: no messages in chat\n", conv_name);
		return;
	}

	size_t size = strlen(conv_name) + 1;
	char *name = (char *)malloc(size);
	strncpy(name, conv_name, size);
	update_conversation_history(history, name, message_timestamp);
	purple_debug_misc(PLUGIN_ID, "%s: Updated with timestamp: %lu\n", conv_name, message_timestamp);
}

static void mark_conversation_none_unseen(PurpleConversation *conv) {
	purple_conversation_set_data(conv, "unseen-count", GINT_TO_POINTER(0));
	purple_conversation_set_data(conv, "unseen-state", GINT_TO_POINTER(PIDGIN_UNSEEN_NONE));

	PidginConversation *gtkconv = PIDGIN_CONVERSATION(conv);
	if (!gtkconv) {
		return;
	}

	gtkconv->unseen_count = 0;
	gtkconv->unseen_state = PIDGIN_UNSEEN_NONE;
	purple_conversation_update(conv, PURPLE_CONV_UPDATE_UNSEEN);
}

static void reset_conversation_unseen_status(PurpleConversation *conv, const char *conv_name) {
	PidginUnseenState old_state = PIDGIN_UNSEEN_NONE;
	gpointer data = purple_conversation_get_data(conv, "unseen-state");
	if (data) {
		old_state = GPOINTER_TO_INT(data);
	}

	if (old_state != PIDGIN_UNSEEN_NONE) {
		mark_conversation_none_unseen(conv);
	}
}

static void conversation_updated_cb(PurpleConversation *conv, PurpleConvUpdateType type) {
	const char *conv_name = purple_conversation_get_name(conv);
	if (type == PURPLE_CONV_UPDATE_CHATLEFT) {
		chat_left_cb(conv, conv_name);
	}
}

static void wrote_chat_msg_cb(PurpleAccount *account, const char *who,
                              char *message, PurpleConversation *conv,
                              PurpleMessageFlags flags) {
	const char *conv_name = purple_conversation_get_name (conv);
	if (history == NULL) {
		purple_debug_misc(PLUGIN_ID, "%s: Cannot check unseen status: no history file\n", conv_name);
		return;
	}

	time_t message_timestamp = get_latest_conversation_message_timestamp(conv);
	if (message_timestamp == 0) {
		purple_debug_misc(PLUGIN_ID, "%s: No messages in chat, marking chat read\n", conv_name);
		reset_conversation_unseen_status(conv, conv_name);
		return;
	}

	if (!is_conversation_in_history(history, conv_name)) {
		purple_debug_misc(PLUGIN_ID, "%s: No history for conversation, skipping unseen status fixing\n", conv_name);
		return;
	}

	bool has_unseen = has_conversation_history_unseen_messages(history, conv_name, message_timestamp);
	if (!has_unseen) {
		purple_debug_misc(PLUGIN_ID, "%s: No unseen messages in conversation, marking as read\n", conv_name);
		reset_conversation_unseen_status(conv, conv_name);
	} else {
		purple_debug_misc(PLUGIN_ID, "%s: Unseen messages in conversation, not modifying unseen status\n", conv_name);
	}
}

static char *get_history_filepath(void) {
	char *home_directory = getenv("HOME");
	if (home_directory == NULL) {
		return NULL;
	}

	size_t directory_length = strlen(home_directory);
	size_t filename_length = strlen(HISTORY_FILENAME);
	size_t size = directory_length + 1 + filename_length + 1;
	char *filepath = malloc(size);
	strncpy(filepath, home_directory, directory_length);
	filepath[directory_length] = '/';
	strncpy(filepath + directory_length + 1, HISTORY_FILENAME, filename_length);
	filepath[size - 1] = '\0';
	return filepath;
}

static gboolean plugin_load(PurplePlugin *plugin) {
	char *filepath = get_history_filepath();
	if (filepath == NULL) {
		purple_debug_error(PLUGIN_ID, "Could not find HOME directory, cannot load plugin\n");
		return FALSE;
	}

	purple_debug_misc(PLUGIN_ID, "Saving history to file %s\n", filepath);
	history = init_history(filepath);
	free(filepath);

	void *convs_handle;
	convs_handle = purple_conversations_get_handle();

	purple_signal_connect(convs_handle, "conversation-updated", plugin,
	                      PURPLE_CALLBACK(conversation_updated_cb), NULL);

	purple_signal_connect_priority(convs_handle, "wrote-chat-msg", plugin,
	                               PURPLE_CALLBACK(wrote_chat_msg_cb), NULL,
	                               PURPLE_SIGNAL_PRIORITY_LOWEST);

	return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin) {
	purple_signals_disconnect_by_handle(plugin);
	deinit_history(history);
	return TRUE;
}

static PurplePluginInfo info = {
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,
	PIDGIN_PLUGIN_TYPE,
	0,
	NULL,
	PURPLE_PRIORITY_DEFAULT,

	PLUGIN_ID,
	"Remember read status",
	PLUGIN_VERSION,

	"Remembers read status of chat conversations",

	"This plugin will remember the read status of chat conversations "
	"when disconnecting from a chat. Without this plugin, when connecting "
	"to a chat room, the chat room will be shown as if there are unseen "
	"messages even if you had previously seen all messages. This plugin "
	"fixes that by remembering the last messages that have arrived to a "
	"certain chatroom. If the user has seen previously the latest "
	"message, the chat will be be marked as unseen.",

	"Miika-Petteri Matikainen <miikapetteri@gmail.com>",
	"https://github.com/nablaa/pidgin-remember-read-status",

	plugin_load,
	plugin_unload,
	NULL,

	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static void init_plugin(PurplePlugin *plugin) {
}

PURPLE_INIT_PLUGIN(conv_last_said_memory, init_plugin, info)
