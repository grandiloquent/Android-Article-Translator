#include "utils_string.h"
#include "log.h"
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

int indexof(const char *s, const char *find) {
    char c, sc;
    size_t len;
    int count = 0;

    if ((c = *find++) != 0) {
        len = strlen(find);
        do {
            do {
                if ((sc = *s++) == 0)
                    return -1;
                count++;
            } while (sc != c);
        } while (strncmp(s, find, len) != 0);
        count--;
    }
    return count;
}

bool iswhitespace(const char *s) {
    if (!s)
        return true;
    while (*s)
        if (!isspace(*s++))
            return false;

    return true;
}

char *remove_redundancy(const char *s) {
    size_t len = strlen(s);
    char *r = malloc(len + 1);
    memset(r, 0, len + 1);
    char *t = r;

    while (*s) {
        if (*s == '\n') {
            *t++ = '\n';
            char ch = 0;
            while (*++s && isspace(*s)) {

                if (ch == 0 && *s == '\n') {

                    ch = '\n';
                }
            }

            if (ch != 0)
                *t++ = '\n';
            continue;
        }

        *t++ = *s++;
    }
    return r;
}