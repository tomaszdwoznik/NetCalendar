#ifndef CALENDAR_H
#define CALENDAR_H

#include <openssl/ssl.h>
#include "cJSON.h"

cJSON* load_database();
void save_database(cJSON *db);
void add_event_to_db(const char *date, const char *time, const char *title);
cJSON* get_events_by_date(const char *target_date);

#endif