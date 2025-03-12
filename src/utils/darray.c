#include "darray.h"

#include <stdlib.h>
#include <string.h>

typedef struct _DArrayHeader {
    size_t size;
    size_t capacity;
    size_t stride;
} DArrayHeader;

#define DARRAY_MAX_LOAD 0.75f
#define DARRAY_MIN_LOAD 0.25f

#define DARRAY_GROWTH_FACTOR 2
#define DARRAY_SHRINK_FACTOR 0.5f

#define DARRAY_INITIAL_CAPACITY 2

void* __darray_new(size_t stride) {
    DArrayHeader* header = (DArrayHeader*)malloc(sizeof(DArrayHeader) + stride * DARRAY_INITIAL_CAPACITY);
    header->size = 0;
    header->capacity = DARRAY_INITIAL_CAPACITY;
    header->stride = stride;

    return header + 1;
}

void __darray_push(void** da, void* value) {
    DArrayHeader* header = (DArrayHeader*)((char*)*da - sizeof(DArrayHeader));
    if ((float)header->size / header->capacity >= DARRAY_MAX_LOAD) {
        header->capacity *= DARRAY_GROWTH_FACTOR;
        header = (DArrayHeader*)realloc(header, sizeof(DArrayHeader) + header->stride * header->capacity);
        *da = header + 1;
    }

    memcpy((char*)header + sizeof(DArrayHeader) + header->size * header->stride, value, header->stride);
    header->size++;
}

#ifndef DARRAY_NOT_SHRINKABLE
void __darray_pop(void** da) {
    DArrayHeader* header = (DArrayHeader*)((char*)*da - sizeof(DArrayHeader));
    if (header->size == 0)
        return;

    if ((float)header->size / header->capacity <= DARRAY_MIN_LOAD && header->capacity > DARRAY_INITIAL_CAPACITY) {
        header->capacity *= DARRAY_SHRINK_FACTOR;
        header = (DArrayHeader*)realloc(header, sizeof(DArrayHeader) + header->stride * header->capacity);
        *da = header + 1;
    }

    header->size--;
}
#endif

void __darray_iterate(void* da, darray_callback_t callback, void* usr) {
    DArrayHeader* header = (DArrayHeader*)((char*)da - sizeof(DArrayHeader));
    for (size_t i = 0; i < header->size; i++) {
        callback((char*)da + i * header->stride, i, usr);
    }

}

size_t __darray_get_member(void* da, size_t index) {
    return ((size_t*)((char*)da - sizeof(DArrayHeader)))[index];
}

void __darray_set_member(void* da, size_t index, size_t value) {
    ((size_t*)((char*)da - sizeof(DArrayHeader)))[index] = value;
}

void __darray_free(void* da) {
    free((char*)da - sizeof(DArrayHeader));
}