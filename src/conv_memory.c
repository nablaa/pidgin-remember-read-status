#define PURPLE_PLUGINS

#include <glib.h>

#include "notify.h"
#include "plugin.h"
#include "version.h"
#include "gtkplugin.h"
#include "gtkconv.h"

#define PLUGIN_VERSION "0.1-dev"


static gboolean
plugin_load(PurplePlugin *plugin) {
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

	"gtk-nablaa-conversation-last-said-memory",
	"Conversion last said memory",
	PLUGIN_VERSION,

	"TODO",
	"TODO",
	"Miika-Petteri Matikainen <miikapetteri@gmail.com>",
	"TODO",

	plugin_load,
	NULL,
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

static void
init_plugin(PurplePlugin *plugin) {
}

PURPLE_INIT_PLUGIN(conv_last_said_memory, init_plugin, info)
