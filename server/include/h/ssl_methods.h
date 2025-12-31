#ifndef INCLUDE_H_SSL_METHODS_H_
#define INCLUDE_H_SSL_METHODS_H_

#include <openssl/err.h>
#include <sys/select.h>

#include "common_utils.h"

SSL_CTX* init_ssl_context(const char* cert_file, const char* key_file);
void close_ssl_connection(int fd, fd_set* main_rmask);
int ssl_buf_read(SSL* ssl, char* buf, int bufsize);

#endif  // INCLUDE_H_SSL_METHODS_H_