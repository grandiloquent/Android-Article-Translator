#ifndef __MACROS_H__
#define __MACROS_H__


#define  BAIDU_SECRET "sdK6QhtFE64Qm0ID_SjG"

#define SOCKET_INIT(X, Y) int ret, fd;    \
    {    \
        struct addrinfo hints, *cur;    \
        memset(&hints, 0x00, sizeof(hints));    \
        hints.ai_family = AF_UNSPEC;    \
        hints.ai_socktype = SOCK_STREAM;    \
        hints.ai_protocol = IPPROTO_TCP;    \
        ret = getaddrinfo(X, Y, &hints, &cur);    \
        if (ret) {    \
            freeaddrinfo(cur);    \
            (*env)->ReleaseStringUTFChars(env, word_, word);    \
            return NULL;    \
        }    \
        fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);    \
        if (fd < 0) {    \
            freeaddrinfo(cur);    \
            (*env)->ReleaseStringUTFChars(env, word_, word);    \
            return NULL;    \
        }    \
        if (connect(fd, cur->ai_addr, cur->ai_addrlen) != 0) {    \
            freeaddrinfo(cur);    \
            (*env)->ReleaseStringUTFChars(env, word_, word);    \
            return NULL;    \
        }    \
        freeaddrinfo(cur);    \
    }

#define URL_ENCODE(X) char buf_encode[strlen(X) * 3 + 1];    \
    const char *path_str = X;    \
    size_t buf_encode_index = 0;    \
    while (*path_str) {    \
        if (isalnum(*path_str) || *path_str == '-' || *path_str == '_' || *path_str == '.' ||    \
            *path_str == '~') {    \
            buf_encode[buf_encode_index] = *path_str;    \
            buf_encode_index = buf_encode_index + 1;    \
        } else if (*path_str == ' ') {    \
            buf_encode[buf_encode_index] = '+';    \
            buf_encode_index = buf_encode_index + 1;    \
        } else {    \
            buf_encode[buf_encode_index] = '%';    \
            buf_encode_index = buf_encode_index + 1;    \
            buf_encode[buf_encode_index] = HEX_ARRAY[*path_str >> 4 & 15];    \
            buf_encode_index = buf_encode_index + 1;    \
            buf_encode[buf_encode_index] = HEX_ARRAY[*path_str & 15 & 15];    \
            buf_encode_index = buf_encode_index + 1;    \
        }    \
        path_str++;    \
    }    \
    buf_encode[buf_encode_index] = 0

#define  SEND_HEADER() ret = send(fd, buf_header, strlen(buf_header), 0);    \
    if (ret <= 0) {    \
        close(fd);    \
        (*env)->ReleaseStringUTFChars(env, word_, word);    \
        return NULL;    \
    }
#define  MAKE_SALT() int salt = time(NULL);    \
    char salt_buf[11];    \
    snprintf(salt_buf, 11, "%d", salt)

#define ENSURE_NOT_BIG()  if (buf_body_read_len >= buf_body_len) {    \
            close(fd);    \
            (*env)->ReleaseStringUTFChars(env, word_, word);    \
            return (*env)->NewStringUTF(env, "Too big.");;    \
        }

#define  MAKE_BOUNDARY() const char *buf_boundary_format = "\r\n--%s--\r\n";    \
  size_t buf_boundary_length = strlen(buf_boundary_format) + strlen(boundary);    \
  char buf_boundary[buf_boundary_length];    \
  snprintf(buf_boundary, buf_boundary_length, buf_boundary_format, boundary)
#endif