#include "../h/common_utils.h"

struct user_state states[MAX_FD];

// czyszczenie tablicy stanow na inicjalizacji serwera
void init_all_states() { memset(states, 0, sizeof(states)); }