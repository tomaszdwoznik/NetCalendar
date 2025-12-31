#ifndef INCLUDE_H_NETWORK_UTILS_H_
#define INCLUDE_H_NETWORK_UTILS_H_

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "common_utils.h"

void setNonBlock(int fd);
int setup_addr(struct sockaddr_in* addr);
int make_listen_socket(struct sockaddr_in* addr);
void init_selector(int sfd, fd_set* rmask, fd_set* wmask, int* fdmax);
int buf_read(int sfd, char* buf, int bufsize);

#endif  // INCLUDE_H_NETWORK_UTILS_H_