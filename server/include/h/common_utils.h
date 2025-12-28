#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>

#define MAX_FD 64

struct user_map {
    int id;
    char name[50];
};

struct user_state {
    int state; 
    char response_buf[256]; 
    SSL *ssl;
};

void init_all_states();

extern struct user_state states[MAX_FD];

#endif