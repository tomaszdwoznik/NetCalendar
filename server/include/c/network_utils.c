#include "../h/network_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

// ustawienie gniazda w tryb nieblokujacy 
void setNonBlock(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// przygotowanie struktury adresowej
int setup_addr(struct sockaddr_in* addr) {
  memset(addr, 0, sizeof(*addr));
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = INADDR_ANY;
  addr->sin_port = htons(8888);
  return 0;
}

// tworzenie i konfiguracja gniazda sieciowego
int make_listen_socket(struct sockaddr_in* addr) {
  int on = 1;
  int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sfd < 0) {
    perror("Błąd socket");
    return -1;
  }
  setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  setNonBlock(sfd);

  if (bind(sfd, (struct sockaddr*)addr, sizeof(*addr)) < 0) {
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

// inicjalizacja masek dla funkcji select()
void init_selector(int sfd, fd_set* rmask, fd_set* wmask, int* fdmax) {
  FD_ZERO(rmask);
  FD_ZERO(wmask);
  FD_SET(sfd, rmask);
  *fdmax = sfd;
}

// dokladne i bezstratne czytanie danych 
int buf_read(int sfd, char* buf, int bufsize) {
  int rc = 0;
  do {
    int i = read(sfd, buf, bufsize);
    if (i < 0) return i;
    bufsize -= i;
    buf += i;
    rc += i;
  } while (bufsize > 0);
  return rc;
}