#ifndef CALENDAR_H
#define CALENDAR_H

#include <stdbool.h>
#include <openssl/ssl.h>
#include "cJSON.h"

cJSON* load_database();
void save_database(cJSON *db);
void add_event_to_db(const char *date, const char *time, const char *title, cJSON *db);
cJSON* get_events_by_date(const char *target_date, cJSON *db);
int is_event_duplicate(cJSON *db, const char *date, const char *time);
bool is_valid_date(const char *date_str);
bool is_valid_time(const char *time_str);

#endif