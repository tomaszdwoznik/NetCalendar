#include "../h/cJSON.h"
#include "../h/common_utils.h"

cJSON* load_database() {
    FILE *f = fopen("../resources/calendar.json", "rb");
    if (!f) {
		return cJSON_CreateArray(); 
	}
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *data = malloc(len + 1);
    fread(data, 1, len, f);
    fclose(f);

    cJSON *json = cJSON_Parse(data);
    free(data);
    return json;
}

void save_database(cJSON *db) {
    char *string = cJSON_Print(db);
    FILE *f = fopen("../resources/calendar.json", "w");
    if (f) {
        fprintf(f, "%s", string);
        fclose(f);
    }
    free(string);
}

void add_event_to_db(const char *date, const char *time, const char *title) {
    cJSON *db = load_database();
    cJSON *new_event = cJSON_CreateObject();
    
    cJSON_AddStringToObject(new_event, "date", date);
    cJSON_AddStringToObject(new_event, "time", time);
    cJSON_AddStringToObject(new_event, "title", title);
    
    cJSON_AddItemToArray(db, new_event);
    save_database(db);
    cJSON_Delete(db); 
}

cJSON* get_events_by_date(const char *target_date) {
    cJSON *db = load_database();
    cJSON *filtered_array = cJSON_CreateArray();
    cJSON *event = NULL;

    cJSON_ArrayForEach(event, db) {
        cJSON *date_item = cJSON_GetObjectItemCaseSensitive(event, "date");
		if (cJSON_IsString(date_item) && (date_item->valuestring != NULL)) {
            if (strcmp(date_item->valuestring, target_date) == 0) {
                cJSON_AddItemToArray(filtered_array, cJSON_Duplicate(event, 1));
            }
        }
    }

    cJSON_Delete(db); 
    return filtered_array; 
}