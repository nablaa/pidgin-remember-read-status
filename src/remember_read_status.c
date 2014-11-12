#define PURPLE_PLUGINS

#include <glib.h>

#include "notify.h"
#include "plugin.h"
#include "version.h"
#include "gtkplugin.h"
#include "gtkconv.h"
#include "debug.h"

#include <stdbool.h>
#include <string.h>

#define PLUGIN_ID "gtk-nablaa-remember-read-status"
#define PLUGIN_VERSION "0.1-dev"

static bool is_disconnect_message(const char *msg) {
	return strstr(msg, "The account has disconnected and you are no longer in this chat.") != NULL;
}

static time_t get_latest_conversation_message_timestamp(PurpleConversation *conv) {
	time_t time = 0;
	GList *history_list = purple_conversation_get_message_history(conv);
	for (GList *l = history_list; l != NULL; l = l->next) {
		PurpleConvMessage *message = (PurpleConvMessage *)l->data;
		const char *msg = purple_conversation_message_get_message(message);
		time = purple_conversation_message_get_timestamp(message);

		// Skip disconnect messages
		if (is_disconnect_message(msg)) {
			continue;
		}
		break;
	}
	return time;
}

static void chat_joined_cb(PurpleConversation *conv) {
	const char *conv_name = purple_conversation_get_name (conv);
	purple_debug_misc(PLUGIN_ID, "CALLBACK (%s): chat joined\n", conv_name);
}

static void chat_left_cb(PurpleConversation *conv) {
	const char *conv_name = purple_conversation_get_name (conv);
	purple_debug_misc(PLUGIN_ID, "CALLBACK (%s): chat left\n", conv_name);
}

static void conversation_created_cb(PurpleConversation *conv) {
	const char *conv_name = purple_conversation_get_name (conv);
	purple_debug_misc(PLUGIN_ID, "CALLBACK (%s): conversation created\n", conv_name);
}

static void conversation_updated_cb(PurpleConversation *conv, PurpleConvUpdateType type) {
	const char *conv_name = purple_conversation_get_name (conv);
	purple_debug_misc(PLUGIN_ID, "CALLBACK (%s): conversation updated, type: %d\n", conv_name, type);
	switch (type) {
		case PURPLE_CONV_UPDATE_ADD:
			purple_debug_misc(PLUGIN_ID, "UPDATE_ADD\n");
			break;
		case PURPLE_CONV_UPDATE_REMOVE:
			purple_debug_misc(PLUGIN_ID, "UPDATE_REMOVE\n");
			break;
		case PURPLE_CONV_UPDATE_ACCOUNT:
			purple_debug_misc(PLUGIN_ID, "UPDATE_ACCOUNT\n");
			break;
		case PURPLE_CONV_UPDATE_TYPING:
			purple_debug_misc(PLUGIN_ID, "UPDATE_TYPING\n");
			break;
		case PURPLE_CONV_UPDATE_UNSEEN:
			purple_debug_misc(PLUGIN_ID, "UPDATE_UNSEEN\n");
			break;
		case PURPLE_CONV_UPDATE_LOGGING:
			purple_debug_misc(PLUGIN_ID, "UPDATE_LOGGING\n");
			break;
		case PURPLE_CONV_UPDATE_TOPIC:
			purple_debug_misc(PLUGIN_ID, "UPDATE_TOPIC\n");
			break;
		case PURPLE_CONV_ACCOUNT_ONLINE:
			purple_debug_misc(PLUGIN_ID, "ACCOUNT_ONLINE\n");
			break;
		case PURPLE_CONV_ACCOUNT_OFFLINE:
			purple_debug_misc(PLUGIN_ID, "ACCOUNT_OFFLINE\n");
			break;
		case PURPLE_CONV_UPDATE_AWAY:
			purple_debug_misc(PLUGIN_ID, "UPDATE_AWAY\n");
			break;
		case PURPLE_CONV_UPDATE_ICON:
			purple_debug_misc(PLUGIN_ID, "UPDATE_ICON\n");
			break;
		case PURPLE_CONV_UPDATE_TITLE:
			purple_debug_misc(PLUGIN_ID, "UPDATE_TITLE\n");
			break;
		case PURPLE_CONV_UPDATE_CHATLEFT:
			purple_debug_misc(PLUGIN_ID, "UPDATE_CHATLEFT\n");
			break;
		case PURPLE_CONV_UPDATE_FEATURES:
			purple_debug_misc(PLUGIN_ID, "UPDATE_FEATURES\n");
			break;
		default:
			purple_debug_misc(PLUGIN_ID, "UPDATE_UNKOWN\n");
			break;
    }

}

static void received_chat_msg_cb(PurpleAccount *account, char *sender, char *message,
                          PurpleConversation *conv, PurpleMessageFlags flags) {
	const char *conv_name = purple_conversation_get_name (conv);
	purple_debug_misc(PLUGIN_ID, "CALLBACK (%s): received chat msg\n", conv_name);
}


static gboolean plugin_load(PurplePlugin *plugin) {
	void *convs_handle;
	convs_handle = purple_conversations_get_handle();

	purple_signal_connect(convs_handle, "chat-joined", plugin,
	                      PURPLE_CALLBACK(chat_joined_cb), NULL);

	purple_signal_connect(convs_handle, "chat-left", plugin,
	                      PURPLE_CALLBACK(chat_left_cb), NULL);

	purple_signal_connect(convs_handle, "conversation-created", plugin,
	                      PURPLE_CALLBACK(conversation_created_cb), NULL);

	purple_signal_connect(convs_handle, "conversation-updated", plugin,
	                      PURPLE_CALLBACK(conversation_updated_cb), NULL);

	purple_signal_connect(convs_handle, "received-chat-msg", plugin,
	                      PURPLE_CALLBACK(received_chat_msg_cb), NULL);

	return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin) {
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

	"TODO summary",
	"TODO description",
	"Miika-Petteri Matikainen <miikapetteri@gmail.com>",
	"TODO website",

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
