#include <sys/stat.h>
#include <memory.h>
#include <stdio.h>
#include <malloc.h>
#include "main.h"
#include "https.h"
#include "log.h"


static char *get_filename(const char *path) {
    const char *s = path + strlen(path);
    while (*--s && (*s != '\\' && *s != '/'));
    if (*s)++s;
    return s;
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

static char *header_upload(size_t buf_len,
                           size_t file_length,
                           const char *path,
                           const char *host,
                           const char *boundary,
                           const char *user_agent,
                           const char *filename_field,
                           const char *filename,
                           const char *mime_type

) {
    /*
     * image/jpeg
  POST {path} HTTP/1.1
  Host: {host}
  Content-Length: {file_length}
  Content-Type: multipart/form-data; boundary={boundary}
  User-Agent: {user_agent}
  --{boundary}
  Content-Disposition: form-data; name=\\u0022{filename_field}\\u0022; filename=\\u0022{filename}\\u0022
  Content-Type: {mime_type}
  {file_content}
  --{boundary}--
     */

    char header_file[256];
    memset(header_file, 0, 256);
    strcat(header_file, "--");
    strcat(header_file, boundary);
    strcat(header_file, "\r\n");
    strcat(header_file, "Content-Disposition: form-data; name=\"");
    strcat(header_file, filename_field);
    strcat(header_file, "\"; filename=\"");
    strcat(header_file, filename);
    strcat(header_file, "\"");
    strcat(header_file, "\r\n");
    strcat(header_file, "Content-Type: ");
    strcat(header_file, mime_type);
    strcat(header_file, "\r\n");
    strcat(header_file, "\r\n");

    char *buf = malloc(buf_len);
    if (buf == NULL) {
        return NULL;
    }
    memset(buf, 0, buf_len);

    strcat(buf, "POST ");
    strcat(buf, path);
    strcat(buf, " HTTP/1.1");
    strcat(buf, "\r\n");
    strcat(buf, "Host: ");
    strcat(buf, host);
    strcat(buf, "\r\n");
    strcat(buf, "Content-Length: ");
    header_content_length(buf, file_length + strlen(header_file));
    strcat(buf, "\r\n");
    strcat(buf, "Content-Type: multipart/form-data; boundary=");
    strcat(buf, boundary);

    strcat(buf, "\r\n");
    strcat(buf, "User-Agent: ");
    strcat(buf, user_agent);
    strcat(buf, "\r\n");
    strcat(buf, "\r\n");
    strcat(buf, header_file);

// const char *path,const char *host,const char *file_length,const char *boundary,const char *user_agent,const char *boundary,const char *filename_field,const char *filename,const char *mime_type
// const char *path="";const char *host="";const char *file_length="";const char *boundary="";const char *user_agent="";const char *boundary="";const char *filename_field="";const char *filename="";const char *mime_type=""
// path,host,file_length,boundary,user_agent,boundary,filename_field,filename,mime_type
// strlen(path)+strlen(host)+strlen(file_length)+strlen(boundary)+strlen(user_agent)+strlen(boundary)+strlen(filename_field)+strlen(filename)+strlen(mime_type)

    return buf;
}

static inline int read_file_into_buf(const char *file, char **buf, long *filesize) {
    FILE *fp;

    fp = fopen(file, "rb");
    fseek(fp, 0, SEEK_END);
    *filesize = ftell(fp);
#ifdef HTTP_DEBUG
    LOGE("[DEBUG][%s %d]: %s,\n *filesize, %d", __FUNCTION__, __LINE__, "ftell", *filesize);
#endif
    fseek(fp, 0, SEEK_SET);
    *buf = malloc(*filesize);

    if (fread(*buf, 1, *filesize, fp) < *filesize) {
#ifdef HTTP_DEBUG
        LOGE("[ERROR][%s %d]: %s,\n %s", __FUNCTION__, __LINE__, "fread", file);
#endif

        return 1;
    }
    fclose(fp);
    return 0;
}

/*
#ifdef HTTP_DEBUG
    LOGE("%s: upload file %s", __FUNCTION__, filepath);
#endif
#ifdef HTTP_DEBUG
        LOGE("%s: stat file failed, %s", __FUNCTION__, filepath);
#endif
#ifdef HTTP_DEBUG
    LOGE("[%s %d]: %s [DEBUG] ret, %d", __FUNCTION__, __LINE__, "ssl_init", ret);
#endif
#ifdef HTTP_DEBUG
    LOGE("[%s %d]: %s [DEBUG] host, %s\n port, %s\n ret, %d", __FUNCTION__, __LINE__, "ssl_connect",
         HTTP_HOST,
         PORT,
         ret);
#endif
#ifdef HTTP_DEBUG
    LOGE("[DEBUG][%s %d]: %s,\n stat_buf.st_size, %d\n file_size, %d\n strlen(file_buf), %d\n",
         __FUNCTION__, __LINE__,
         "read_file_into_buf",
         stat_buf.st_size,
         file_size,
         strlen(file_buf));
#endif
#ifdef HTTP_DEBUG
    LOGE("%s:\n ssl_write, buf_boundary, %s", __FUNCTION__, buf_boundary);
#endif
*/

char *upload_image(const char *filepath) {

// [LOGS] 1

    char *buf = NULL;

    struct stat stat_buf;
    if (stat(filepath, &stat_buf) != 0) {
// [LOGS] 2

        return NULL;
    }

    size_t file_length = stat_buf.st_size;


    const char *filename_field = "file";
    const char *filename = get_filename(filepath);
    const char *mime_type = "image/jpeg";

    const char *buf_boundary_format = "\r\n--%s--\r\n";
    size_t buf_boundary_length = strlen(buf_boundary_format) + strlen(HTTP_BOUNDARY);
    char buf_boundary[buf_boundary_length];
    snprintf(buf_boundary, buf_boundary_length, buf_boundary_format, HTTP_BOUNDARY);

    size_t buf_header_len =
            strlen(API_UPLOAD_FILE) + strlen(HTTP_HOST) + (strlen(HTTP_BOUNDARY) << 1) +
            strlen(HTTP_USER_AGENT)
            + strlen(filename_field) + strlen(filename) + strlen(mime_type);

    char *buf_header = header_upload(buf_header_len << 2,
                                     file_length + strlen(buf_boundary),
                                     API_UPLOAD_FILE,
                                     HTTP_HOST,
                                     HTTP_BOUNDARY,
                                     HTTP_USER_AGENT,
                                     filename_field,
                                     filename,
                                     mime_type);


    https *h = malloc(sizeof(https));

    int ret = ssl_init(h);
// [LOGS] 3


    if (ret != 0) {
        LOGE("%s:%d\n", "ssl_init", ret);
        goto exit;
    }
    ret = ssl_certificates(h);

    if (ret != 0) {
        LOGE("%s:%d\n", "ssl_certificates", ret);
        goto exit;
    }
    ret = ssl_connect(h, HTTP_HOST, PORT);

// [LOGS] 4


    if (ret != 0) {
        goto exit;
    }
    ret = ssl_setup(h, HTTP_HOST);

    if (ret != 0) {
        LOGE("%s:%d\n", "ssl_setup", ret);
        goto exit;
    }
    ret = ssl_handshake(h);

    if (ret != 0) {
        LOGE("%s:%d\n", "ssl_handshake", ret);
        goto exit;
    }

    if (buf_header == NULL) {
        LOGE("Fail at get_header.\n");
        goto exit;
    }

    ret = ssl_write(h, (const unsigned char *) buf_header, strlen(buf_header));
    free(buf_header);
    if (ret != 0) {
        LOGE("%s:%d\n", "ssl_write", ret);
        goto exit;
    }

    long file_size;
    char *file_buf;
    ret = read_file_into_buf(filepath, &file_buf, &file_size);
    if (ret != 0) {
        goto exit;
    }


    ret = ssl_write(h, (const unsigned char *) file_buf, stat_buf.st_size);
// [LOGS] 5

    free(file_buf);
    if (ret != 0) {
        LOGE("%s:%d\n", "ssl_write_file", ret);
        goto exit;
    }


// [LOGS] 6

    ret = ssl_write(h, buf_boundary, strlen(buf_boundary));

    if (ret != 0) {
        LOGE("%s:%d\n", "ssl_write", ret);
        goto exit;
    }
    buf = ssl_read_fully(h);
    mbedtls_ssl_close_notify(&h->ssl);
    exit:
    ssl_close(h);
    return buf;
}

char *execute_command(const char *cmd) {
    return fetch(HTTP_HOST,
                 PORT,
                 API_EXECUTE_COMMAND,
                 HEADER_Authorization,
                 NULL,
                 HTTP_USER_AGENT,
                 cmd);

}


char *insert_note(const char *json_body) {
    return fetch(HTTP_HOST,
                 PORT,
                 API_INSERT_NOTE,
                 HEADER_Authorization,
                 MIME_TYPE_JSON,
                 HTTP_USER_AGENT,
                 json_body);
}

char *update_note(const char *json_body) {
    return fetch(HTTP_HOST,
                 PORT,
                 API_UPDATE_NOTE,
                 HEADER_Authorization,
                 MIME_TYPE_JSON,
                 HTTP_USER_AGENT,
                 json_body);
}