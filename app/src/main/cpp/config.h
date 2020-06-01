#ifndef __CONFIG_H__
#define __CONFIG_H__

#define YOUDAO_HOST "openapi.youdao.com"
#define YOUDAO_APPKEY "1f5687b5a6b94361"
#define YOUDAO_SECRET "2433z6GPFslGhUuQltdWP7CPlbk8NZC0"
#define BAI_API_HOST "api.fanyi.baidu.com"
#define BAI_APP_ID "20190312000276185"
//#define DEBUG_ENABLE

#define STRIP_BODY(x)                \
    if (x == NULL)                   \
    {                                \
        return 1;                    \
    }                                \
    char* y = strstr(x, "\r\n\r\n"); \
    if (y == NULL || strlen(y) <= 4) \
    {                                \
        free(x);                     \
        return 1;                    \
    }                                \
    y = y + 4;                       \
    char* body = strstr(y, "\r\n");  \
    if (body == NULL)                \
    {                                \
        free(x);                     \
        return 1;                    \
    };
#define BUF(x, y) \
    char x[y];   \
    memset(x, 0, y)

#endif