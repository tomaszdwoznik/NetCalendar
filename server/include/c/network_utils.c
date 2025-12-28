#include "../h/network_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

void setNonBlock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int setup_addr(struct sockaddr_in *addr, const char *port_str) {    
    if (atoi(port_str) <= 1024 || atoi(port_str) > 65535) {
        printf("Podano błędny port - zakres [1024; 65535]\n");
        return -1; 
    }

    int port = atoi(port_str);
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(port);
    return 0;
}

int make_listen_socket(struct sockaddr_in *addr) {
    int on = 1;
    int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (sfd < 0) {
        perror("Błąd socket");
        return -1;
    }
    setNonBlock(sfd);

    if (bind(sfd, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
        perror("Błąd bind");
        close(sfd);
        return -1;
    }
    
    if (listen(sfd, 10) < 0) {
        perror("Błąd listen");
        close(sfd);
        return -1;
    }
    return sfd;
}

void init_selector(int sfd, fd_set *rmask, fd_set *wmask, int *fdmax) {
    FD_ZERO(rmask);
    FD_ZERO(wmask);
    FD_SET(sfd, rmask);
    *fdmax = sfd;
}

int buf_read(int sfd, char *buf, int bufsize){
	int i, rc = 0;

	do {
		i = read(sfd, buf, bufsize);
		if (i < 0){
			return i;
		}
		bufsize -= i;
		buf += i;
		rc += i;
	}
	while (bufsize > 0);

	return rc;
}