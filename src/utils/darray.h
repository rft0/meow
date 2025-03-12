#ifndef __DARRAY_H
#define __DARRAY_H

#include <stddef.h>

typedef void (*darray_callback_t)(void* value, size_t index, void* usr);

#define darray_new(type) __darray_new(sizeof(type))
#define darray_free(da) __darray_free((void*)(da))
#define darray_iterate(da, callback, usr) __darray_iterate((void*)(da), callback, (usr))
#define darray_push(da, value) __darray_push((void**)&(da), &(value))
#define darray_pushd(da, value) \
    do { \
        __typeof__(value) t = (value); \
        __darray_push((void**)&(da), &t); \
    } while (0)

#define darray_size(da) __darray_get_member(da, 0)
#define darray_capacity(da) __darray_get_member(da, 1)
#define darray_stride(da) __darray_get_member(da, 2)

#define darray_set_size(da, value) __darray_set_member(da, 0, value)
#define darray_set_capacity(da, value) __darray_set_member(da, 1, value)
#define darray_set_stride(da, value) __darray_set_member(da, 2, value)

void* __darray_new(size_t stride);
void __darray_push(void** da, void* value);
void __darray_iterate(void* da, darray_callback_t callback, void* usr);
void __darray_free(void* da);

#ifndef DARRAY_NOT_SHRINKABLE
#define darray_pop(da) __darray_pop((void**)&(da))
void __darray_pop(void** da);
#endif

size_t __darray_get_member(void* da, size_t index);
void __darray_set_member(void* da, size_t index, size_t value);

#endif