#include "conversation_history.h"

#include <glib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>

struct History {
	FILE *file;
};

History *init_history(const char *filename) {
	return NULL;
}

bool has_conversation_unseen_messages(const History *history,
		                      const char *conversation_name,
				      time_t message_time) {
	return false;
}
