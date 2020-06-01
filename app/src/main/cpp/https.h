#ifndef __HTTPS_H__
#define __HTTPS_H__

#include <mbedtls/net.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>

//#define  HTTP_DEBUG

typedef struct {
    mbedtls_net_context fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt crt;
} https;


int ssl_certificates(https *h);

int ssl_close(https *h);

int ssl_connect(https *h, const char *host, const char *port);

int ssl_handshake(https *h);

int ssl_init(https *h);

char *ssl_read_fully(https *h);

int ssl_setup(https *h, const char *host);

int ssl_write(https *h, const unsigned char *buf, size_t buf_len);

int ssl_write_file(https *h, const char *file_path, size_t buf_size);

char *fetch(
        const char *host,
        const char *port,
        const char *path,
        const char *headers,
        const char *content_type,
        const char *user_agent,
        const char *body);

#endif