
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <zconf.h>
#include "https.h"
#include "ca_cert.h"
#include "log.h"
#include "utils_string.h"

#define STR_LEN(X) (X==NULL?0:strlen(X))

static char *header(size_t buf_length,
                    const char *headers,
                    const char *method,
                    const char *path,
                    const char *host,
                    const char *content_type,
                    const char *user_agent,
                    const char *body);

static void header_content_length(char *buf, size_t len);
// ------------------

char *fetch(
        const char *host,
        const char *port,
        const char *path,
        const char *headers,
        const char *content_type,
        const char *user_agent,
        const char *body) {

    char *buf = NULL;
    const char *method = body == NULL ? "GET" : "POST";

    https *h = malloc(sizeof(https));

    int ret = ssl_init(h);

#ifdef HTTP_DEBUG
    LOGE("[DEBUG][%s %d]: %s,\n"
         " ret = %d\n"
         " ",
         __FUNCTION__, __LINE__, "ssl_init", ret);
#endif

    if (ret != 0) {
        goto exit;
    }
    ret = ssl_certificates(h);

    if (ret != 0) {
        LOGE("%s:%d\n", "ssl_certificates", ret);
        goto exit;
    }
    ret = ssl_connect(h, host, port);

#ifdef HTTP_DEBUG
    LOGE("[DEBUG][%s %d]: %s,\n"
         " ret = %d\n"
         " ", __FUNCTION__, __LINE__, "ssl_connect", ret);
#endif

    if (ret != 0) {
        goto exit;
    }
    ret = ssl_setup(h, host);

    if (ret != 0) {
        LOGE("%s:%d\n", "ssl_setup", ret);
        goto exit;
    }
    ret = ssl_handshake(h);

    if (ret != 0) {
        LOGE("%s:%d\n", "ssl_handshake", ret);
        goto exit;
    }

    size_t buf_header_len =
            strlen(method) +
            strlen(path)
            + strlen(host)
            + STR_LEN(headers)
            + STR_LEN(content_type)
            + strlen(user_agent)
            + STR_LEN(body);

    char *buf_header =
            header(buf_header_len << 1, headers, method, path, host, content_type, user_agent,
                   body);

#ifdef HTTP_DEBUG
    LOGE("[DEBUG][%s %d]: %s,\n"
         "  buf_header_len = %d\n"
         "  strlen(buf_header) = %d\n"
         "  buf_header = %s\n"
         " ",
         __FUNCTION__,
         __LINE__,
         "header",
         buf_header_len, strlen(buf_header), buf_header);
#endif

    if (buf_header == NULL) {
        LOGE("Fail at get_header.\n");
        goto exit;
    }
    ret = ssl_write(h, (const unsigned char *) buf_header, strlen(buf_header));
    free(buf_header);

#ifdef HTTP_DEBUG
    LOGE("[DEBUG][%s %d]: %s,\n"
         " ret = %d\n"
         " ", __FUNCTION__, __LINE__, "ssl_write", ret);
#endif

    if (ret != 0) {
        goto exit;
    }

    buf = ssl_read_fully(h);
    mbedtls_ssl_close_notify(&h->ssl);

#ifdef HTTP_DEBUG
    LOGE("[DEBUG][%s %d]: %s,\n"
         " strlen(buf) = %d\n"
         "  buf = %s\n"
         " ", __FUNCTION__, __LINE__, "ssl_read_fully",
         strlen(buf), buf);
#endif

//    if (buf != NULL) {
//        buf = strstr(buf, "\r\n\r\n");
//        if (buf != NULL)buf += 4;
//    }


    // --------------------------
    exit:
    ssl_close(h);
    return buf;
}


int ssl_certificates(https *h) {
    ca_crt_rsa[ca_crt_rsa_size - 1] = 0;

    int ret = mbedtls_x509_crt_parse(&h->crt, (const unsigned char *) ca_crt_rsa,
                                     ca_crt_rsa_size);
    if (ret < 0) {
        LOGE(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        return ret;
    }
    return 0;
}

#define  HTTP_DEBUG

int ssl_close(https *h) {
    mbedtls_net_free(&h->fd);
    mbedtls_x509_crt_free(&h->crt);
    mbedtls_ssl_free(&h->ssl);
    mbedtls_ssl_config_free(&h->conf);
    mbedtls_ctr_drbg_free(&h->ctr_drbg);
    mbedtls_entropy_free(&h->entropy);
    free(h);
}

int ssl_connect(https *h, const char *host, const char *port) {
    int ret;
    if ((ret = mbedtls_net_connect(&h->fd, host,
                                   port, MBEDTLS_NET_PROTO_TCP)) != 0) {
        LOGE(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
        return ret;
    }
    return 0;

}

int ssl_handshake(https *h) {
    int ret;
    while ((ret = mbedtls_ssl_handshake(&h->ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            LOGE(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret);
            return ret;
        }
    }
    return 0;

}

int ssl_init(https *h) {
    int ret;
    mbedtls_net_init(&h->fd);
    mbedtls_ssl_init(&h->ssl);
    mbedtls_ssl_config_init(&h->conf);
    mbedtls_x509_crt_init(&h->crt);
    mbedtls_ctr_drbg_init(&h->ctr_drbg);
    mbedtls_entropy_init(&h->entropy);
    if ((ret = mbedtls_ctr_drbg_seed(&h->ctr_drbg, mbedtls_entropy_func, &h->entropy,
                                     NULL, 0)) != 0) {
        LOGE(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
        return 1;
    }
    return 0;
}

char *ssl_read_fully(https *h) {
    char *buf = malloc(4096);
    memset(buf, 0, 4096);
    if (buf == NULL) {
        return NULL;
    }
    char *tmp = NULL;
    size_t size = 0, capacity = 4096, content_length = 0, chunked = 01;
    ssize_t rret;

    do {
        if (size == capacity) {
            capacity *= 2;
            buf = realloc(buf, capacity);
            if (buf == NULL) {
                return NULL;
            }
        }


        rret = mbedtls_ssl_read(&h->ssl, buf + size, capacity - size);

        // trying parse the body length
        if (tmp == NULL && (tmp = strstr(buf, "Content-Length: ")) != NULL) {
            tmp = tmp + strlen("Content-Length: ");

            for (int i = 0, j = strlen(tmp); i < j; ++i) {
                if (tmp[i] == '\r') {
                    char buf_content[i + 1];
                    tmp = memcpy(buf_content, tmp, i + 1);
                    if (tmp != NULL) {
                        content_length = strtol(tmp, NULL, 10);
                    }
                    break;
                }
            }

        }

        if (!chunked && indexof(buf, "Transfer-Encoding: chunked") != -1) {
            chunked = 1;
        }
        if (chunked && indexof(buf, "0\r\n\r\n") != -1) {
            char *buf_body = strstr(buf, "\r\n\r\n");
            if (buf_body != NULL) {
                buf_body = buf_body + 4;
                buf_body = strstr(buf_body, "\r\n");
            }
            if (buf_body != NULL)buf_body = buf_body + 2;
            return buf_body;
        }
        if (content_length != 0) {
            char *buf_body = strstr(buf, "\r\n\r\n");
            if (buf_body != NULL && strlen(buf_body) + 4 >= content_length) {
                return buf_body + 4;
            }
        }
        if (rret == MBEDTLS_ERR_SSL_WANT_READ || rret == MBEDTLS_ERR_SSL_WANT_WRITE)
            continue;

        if (rret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
            break;

        if (rret < 0) {
            LOGE("failed\n  ! mbedtls_ssl_read returned %d\n\n", rret);
            break;
        }

        if (rret == 0) {
            LOGE("\n\nEOF\n\n");
            break;
        }

        size += rret;
    } while (1);
    return buf;
}

int ssl_setup(https *h, const char *host) {
    int ret;

    if ((ret = mbedtls_ssl_config_defaults(&h->conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        LOGE(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        return ret;
    }

    mbedtls_ssl_conf_authmode(&h->conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&h->conf, &h->crt, NULL);
    mbedtls_ssl_conf_rng(&h->conf, mbedtls_ctr_drbg_random, &h->ctr_drbg);

    if ((ret = mbedtls_ssl_setup(&h->ssl, &h->conf)) != 0) {
        LOGE(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        return ret;
    }

    if ((ret = mbedtls_ssl_set_hostname(&h->ssl, host)) != 0) {
        LOGE(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
        return ret;
    }

    mbedtls_ssl_set_bio(&h->ssl, &h->fd, mbedtls_net_send, mbedtls_net_recv, NULL);
    return 0;
}

int ssl_write(https *h, const unsigned char *buf, size_t buf_len) {
    int ret;
    void *p = buf;

    while (buf_len > 0) {
        ret = mbedtls_ssl_write(&h->ssl, p, buf_len);
        if (ret <= 0 && ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            LOGE(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
            return ret;
        }
        buf_len -= ret;
        p += ret;
    }
    return 0;
//  while ((ret = mbedtls_ssl_write(&h->ssl, p, buf_len)) <= 0) {
//    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
//      LOGE(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
//      return ret;
//    }
//    buf_len -= ret;
//    buf += ret;
//  }
//  return 0;
}


int ssl_write_file(https *h, const char *file_path, size_t buf_size) {

    FILE *in = fopen(file_path, "r");
    if (in == NULL) {
        return 1;
    }
    char buf[buf_size];
    size_t file_read, file_send;
    while (1) {
        file_read = read(in, buf, buf_size);
        if (file_read == 0) {
            close(in);
            return 0;
        }
        while ((file_send = mbedtls_ssl_write(&h->ssl, buf, file_read)) <= 0) {
            if (file_send != MBEDTLS_ERR_SSL_WANT_READ && file_send != MBEDTLS_ERR_SSL_WANT_WRITE) {
                {
                    close(in);
                    return file_send;
                }
            }
        }

    }

}

static void header_content_length(char *buf, size_t len) {
    size_t buf_body_len = 0;
    size_t body_len = len, tmp_len = body_len;
    while ((tmp_len /= 10) > 0) {
        buf_body_len++;
    }
    buf_body_len += 2;
    char buf_content_len[buf_body_len];
    snprintf(buf_content_len, buf_body_len, "%d", body_len);
    strcat(buf, buf_content_len);
}

static char *header(size_t buf_length,
                    const char *headers,
                    const char *method,
                    const char *path,
                    const char *host,
                    const char *content_type,
                    const char *user_agent,
                    const char *body) {
    size_t len = buf_length;
    char *buf = malloc(len);
    if (buf == NULL)
        return NULL;
    memset(buf, 0, len);
/*
 * application/json
{method} {path} HTTP/1.1
Host: {host}
Content-Length: {content_length}
Content-Type: {content_type}
User-Agent: {user_agent}

{body}

  Connection: keep-alive <crlf>
  Accept: text/html,application/xhtml+xml,application/xml;q=0.9... <crlf>
  User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_4)... <crlf>
  Accept-Encoding: gzip, deflate, sdch <crlf>
  Accept-Language: en-US,en;q=0.8 <crlf>
  Cookie: pfy_cbc_lb=p-browse-w; customerZipCode=99912|N; ltc=%20;... <crlf>
  <crlf>

 */
    strcat(buf, method);
    strcat(buf, " ");
    strcat(buf, path);
    strcat(buf, " HTTP/1.1");
    strcat(buf, "\r\n");
    strcat(buf, "Host: ");
    strcat(buf, host);
    strcat(buf, "\r\n");

    if (body != NULL) {
        strcat(buf, "Content-Length: ");
        header_content_length(buf, strlen(body));
        strcat(buf, "\r\n");

    }
    if (content_type != NULL) {
        strcat(buf, "Content-Type: ");
        strcat(buf, content_type);
        strcat(buf, "\r\n");
    }

    strcat(buf, "User-Agent: ");
    strcat(buf, user_agent);
    strcat(buf, "\r\n");

    if (headers != NULL) {
        strcat(buf, headers);
    }
    strcat(buf, "\r\n");

    if (body != NULL)
        strcat(buf, body);

// const char *method,const char *path,const char *host,const char *content_length,const char *content_type,const char *user_agent,const char *body
// const char *method="";const char *path="";const char *host="";const char *content_length="";const char *content_type="";const char *user_agent="";const char *body=""
// method,path,host,content_length,content_type,user_agent,body
// strlen(method)+strlen(path)+strlen(host)+strlen(content_length)+strlen(content_type)+strlen(user_agent)+strlen(body)

    return
            buf;
}