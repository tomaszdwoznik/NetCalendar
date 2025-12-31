#include "../h/calendar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

// wczytanie pliku calendar.json przy uzyciu struktury cJSON
cJSON* load_database() {
  FILE* f = fopen("server/resources/calendar.json", "rb");
  if (!f) {
    return cJSON_CreateArray();
  }
  // pobieranie dlugosci pliku
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  fseek(f, 0, SEEK_SET);
  char* data = malloc(len + 1);
  if (data) {
    fread(data, 1, len, f);
    data[len] = '\0';
  }
  fclose(f);

  cJSON* json = cJSON_Parse(data);
  free(data);
  return json;
}

// zapis danych do pliku calendar.json
void save_database(cJSON* db) {
  char* string = cJSON_Print(db);
  FILE* f = fopen("../resources/calendar.json", "w");
  if (f) {
    fprintf(f, "%s", string);
    fclose(f);
  }
  free(string);
}

// tworzenie nowego obiektu wydarzenia na podstawie inputu od klienta
void add_event_to_db(const char* date, const char* time, const char* title,
                     cJSON* db) {
  cJSON* new_event = cJSON_CreateObject();
  cJSON_AddStringToObject(new_event, "date", date);
  cJSON_AddStringToObject(new_event, "time", time);
  cJSON_AddStringToObject(new_event, "title", title);

  cJSON_AddItemToArray(db, new_event);
  save_database(db);
}

// zwracanie wydarzen na podstawie inputu od klienta
cJSON* get_events_by_date(const char* target_date, cJSON* db) {
  cJSON* filtered_array = cJSON_CreateArray();
  cJSON* event = NULL;
  size_t target_len = strlen(target_date);

  cJSON_ArrayForEach(event, db) {
    cJSON* date_item = cJSON_GetObjectItemCaseSensitive(event, "date");
    if (cJSON_IsString(date_item) && (date_item->valuestring != NULL)) {
	// sprawdzanie czy data pasuje do zapytania
      if (strncmp(date_item->valuestring, target_date, target_len) == 0) {
        cJSON_AddItemToArray(filtered_array, cJSON_Duplicate(event, 1));
      }
    }
  }
  return filtered_array;
}

// sprawdzanie czy jakies inne wydarzenie o tej samej dacie i godzinie juz wystepuje w calendar.json
int is_event_duplicate(cJSON* db, const char* date, const char* time) {
  cJSON* event = NULL;
  cJSON_ArrayForEach(event, db) {
    cJSON* existing_date = cJSON_GetObjectItemCaseSensitive(event, "date");
    cJSON* existing_time = cJSON_GetObjectItemCaseSensitive(event, "time");

    if (cJSON_IsString(existing_date) && cJSON_IsString(existing_time)) {
      if (strcmp(existing_date->valuestring, date) == 0 &&
          strcmp(existing_time->valuestring, time) == 0) {
        return 1;
      }
    }
  }
  return 0;
}

// walidacja daty poprzez sprawdzanie zakresow oraz ilosci dni w miesiacu
bool is_valid_date(const char* date_str) {
  int year, month, day;
  int scanned = sscanf(date_str, "%d-%d-%d", &year, &month, &day);

  if (year < 2016 || year > 2116) return false;
  if (month < 1 || month > 12) return false;
  if (scanned == 2) return true;
  if (scanned == 3) {
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
      days_in_month[1] = 29;
    }
    if (day < 1 || day > days_in_month[month - 1]) return false;
    return true;
  }
  return false;
}

// walidacja godziny
bool is_valid_time(const char* time_str) {
  int hours, minutes;
  if (sscanf(time_str, "%d:%d", &hours, &minutes) != 2) return false;
  if (hours < 0 || hours > 23) return false;
  if (minutes < 0 || minutes > 59) return false;
  if (strlen(time_str) != 5) return false;
  return true;
}