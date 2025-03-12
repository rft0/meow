#ifndef __STR_H
#define __STR_H

#define EMPTY_STRV (StringView){NULL, 0}

typedef struct {
    const char* data;
    unsigned long long len;
} StringView;

#endif