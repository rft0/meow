#ifndef __MWERROR_H
#define __MWERROR_H

typedef enum {
    ERROR_LEXER,
    ERROR_PARSER,
    ERROR_ANALYSE,
    ERROR_BCGEN,
} MwErrorType;

typedef struct {
    MwErrorType type;
    int line;
    int column;
    char* message;
} MwError;

typedef enum {
    RT_ERROR_ARITHMETIC,
} MwRuntimeErrorType;

typedef struct {
    MwRuntimeErrorType type;
    int line;
    int column;
    char* message;

} MwRuntimeError;

void mwerror_new(MwErrorType type, int line, int column, const char* fmt, ...);
void mwerror_free(MwError* error);

void init_mwerrors();
int size_mwerrors();
void dump_mwerrors();
void free_mwerrors();

#endif