#ifndef __HASHMAP_H
#define __HASHMAP_H

#include <stdlib.h>

#define hashmap_lit_str(str) (str), (sizeof(str) - 1)

typedef struct _HashMap HashMap;

HashMap* hashmap_new();

int hashmap_get(HashMap* map, const void* key, size_t key_len, uintptr_t* out);
int hashmap_set(HashMap* map, const void* key, size_t key_len, uintptr_t value);

void hashmap_free(HashMap* map);


#endif