#include "../h/ssl_methods.h"

#include <openssl/err.h>
#include <sys/select.h>
#include <unistd.h>

SSL_CTX* init_ssl_context(const char* cert_file, const char* key_file) {
  SSL_library_init();
  SSL_load_error_strings();
  OpenSSL_add_all_algorithms();

  const SSL_METHOD* method = TLS_server_method();
  SSL_CTX* ctx = SSL_CTX_new(method);

  if (!ctx) {
    perror("Błąd: Nie można stworzyć kontekstu SSL");
    return NULL;
  }

  if (SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    return NULL;
  }

  if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    return NULL;
  }

  return ctx;
}

void close_ssl_connection(int fd, fd_set* main_rmask) {
  if (states[fd].ssl != NULL) {
    SSL_shutdown(states[fd].ssl);
    SSL_free(states[fd].ssl);
    states[fd].ssl = NULL;
  }

  FD_CLR(fd, main_rmask);
  close(fd);
  states[fd].state = 0;
  memset(states[fd].response_buf, 0, sizeof(states[fd].response_buf));
  printf("Połączenie %d zamknięte.\n", fd);
}

int ssl_buf_read(SSL* ssl, char* buf, int bufsize) {
  if (ssl == NULL) return -1;

  int n = SSL_read(ssl, buf, bufsize);
  if (n <= 0) {
    int err = SSL_get_error(ssl, n);
    if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
      return -2;
    }
    return n;
  }
  return n;
}