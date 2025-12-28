#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include "common_utils.h"
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

void setNonBlock(int fd);
int setup_addr(struct sockaddr_in *addr, const char *port_str);
int make_listen_socket(struct sockaddr_in *addr);
void init_selector(int sfd, fd_set *rmask, fd_set *wmask, int *fdmax);
int buf_read(int sfd, char *buf, int bufsize);

#endif