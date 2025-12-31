#include "../h/common_utils.h"

struct user_state states[MAX_FD];

void init_all_states()
{
    memset(states, 0, sizeof(states));
}

void broadcast_json(int sender_fd, const char *message, int fdmax)
{
    int len = strlen(message);
    for (int i = 0; i <= fdmax; i++)
    {
        if (states[i].ssl != NULL && i != sender_fd)
        {
            SSL_write(states[i].ssl, message, len);
        }
    }
}