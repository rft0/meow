#include "mwerror.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "utils/darray.h"

MwError** g_mwerrors = NULL;

void mwerror_new(MwErrorType type, int line, int column, const char* fmt, ...) {
    MwError* error = (MwError*)malloc(sizeof(MwError));
    error->type = type;
    error->line = line;
    error->column = column;

    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (size < 0) {
        va_end(args);
        free(error);
        return;
    }

    error->message = (char*)malloc(size + 1);
    vsnprintf(error->message, size + 1, fmt, args);

    va_end(args);

    darray_push(g_mwerrors, error);
}

void mwerror_free(MwError* error) {
    free(error->message);
    free(error);
}

void init_mwerrors() {
    g_mwerrors = darray_new(MwError*);
}

int size_mwerrors() {
    return darray_size(g_mwerrors);
}

void dump_mwerrors() {
    if (g_mwerrors) {
        for (int i = 0; i < darray_size(g_mwerrors); i++) {
            MwError* error = g_mwerrors[i];
            printf("%d:%d: Error: %s\n", error->line, error->column, error->message);
        }
    }
}

void free_mwerrors() {
    if (g_mwerrors) {
        for (int i = 0; i < darray_size(g_mwerrors); i++)
            mwerror_free(g_mwerrors[i]);
        darray_free(g_mwerrors);
    }
}