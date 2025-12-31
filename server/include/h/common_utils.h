#ifndef INCLUDE_H_COMMON_UTILS_H_
#define INCLUDE_H_COMMON_UTILS_H_

#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_FD 64

struct user_map {
  int id;
  char name[50];
};

struct user_state {
  int state;
  char response_buf[256];
  SSL* ssl;
};

extern struct user_state states[MAX_FD];

void init_all_states();

#endif  // INCLUDE_H_COMMON_UTILS_H_  