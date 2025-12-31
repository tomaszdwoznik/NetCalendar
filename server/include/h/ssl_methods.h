#ifndef SSL_METHODS_H
#define SSL_METHODS_H

#include "common_utils.h"
#include <openssl/err.h>
#include <sys/select.h>

SSL_CTX *init_ssl_context(const char *cert_file, const char *key_file);
void close_ssl_connection(int fd, fd_set *main_rmask);
int ssl_buf_read(SSL *ssl, char *buf, int bufsize);

#endif