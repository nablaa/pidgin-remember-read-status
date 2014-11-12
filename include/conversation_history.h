#ifndef CONVERSATION_HISTORY_H
#define CONVERSATION_HISTORY_H

#include <time.h>
#include <stdbool.h>

typedef struct History History;

History *init_history(const char *filename);

void deinit_history(History *history);

bool has_conversation_unseen_messages(const History *history,
                                      const char *conversation_name,
                                      time_t message_time);

void update_conversation_history(History *history,
                                 const char *conversation_name,
                                 time_t message_time);

#endif /* CONVERSATION_HISTORY_H */
