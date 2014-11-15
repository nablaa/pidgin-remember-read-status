/**
 * Pidgin remember read status plugin
 * Copyright (C) 2014 Miika-Petteri Matikainen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
*/
#ifndef CONVERSATION_HISTORY_H
#define CONVERSATION_HISTORY_H

#include <time.h>
#include <stdbool.h>

typedef struct History History;

History *init_history(const char *filename);

void deinit_history(History *history);

bool is_conversation_in_history(const History *history,
                                const char *conversation_name);

bool has_conversation_history_unseen_messages(const History *history,
                                              const char *conversation_name,
                                              time_t message_time);

void update_conversation_history(History *history,
                                 const char *conversation_name,
                                 time_t message_time);

#endif /* CONVERSATION_HISTORY_H */
