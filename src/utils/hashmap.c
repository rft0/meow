#include "hashmap.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define HASHMAP_LOAD_FACTOR 0.75
#define HASHMAP_INITIAL_CAPACITY 128

typedef struct {
    uint32_t hash;
    void* key;
    size_t key_len;
    uintptr_t value;
} HashEntry;

typedef struct _HashMap {
    size_t capacity;
    size_t count;
    HashEntry* entries;
} HashMap;

static uint32_t murmurhash3(const void* key, size_t len) {
    uint32_t h = 0xc6a4a793;
    uint32_t k;
    const uint8_t* data = (const uint8_t*)key;
    while (len >= 4) {
        memcpy(&k, data, 4);
        k *= h; k ^= k >> 16; k *= h;
        h ^= k; h *= h;
        data += 4; len -= 4;
    }

    while (len--) {
        h ^= (uint32_t)(*data++);
        h *= 0x5bd1e995;
    }

    return h;
}

static void hashmap_resize(HashMap* map) {
    size_t new_capacity = map->capacity * 2;
    HashEntry* new_entries = calloc(new_capacity, sizeof(HashEntry));
    if (!new_entries)
        return;

    for (size_t i = 0; i < map->capacity; i++) {
        if (map->entries[i].key) {
            size_t index = map->entries[i].hash % new_capacity;
            for (size_t j = 0; j < new_capacity; j++) {
                HashEntry* entry = &new_entries[(index + j) % new_capacity];
                if (!entry->key) {
                    *entry = map->entries[i];
                    break;
                }
            }
        }
    }

    map->entries = new_entries;
    map->capacity = new_capacity;
}

HashMap* hashmap_new() {
    HashMap* map = malloc(sizeof(HashMap) + HASHMAP_INITIAL_CAPACITY * sizeof(HashEntry));
    if (!map)
        return NULL;

    map->capacity = HASHMAP_INITIAL_CAPACITY;
    map->count = 0;

    map->entries = calloc(map->capacity, sizeof(HashEntry));
    if (!map->entries) {
        free(map);
        return NULL;
    }

    return map;
}

int hashmap_get(HashMap* map, const void* key, size_t key_len, uintptr_t* out) {
    uint32_t hash = murmurhash3(key, key_len);
    size_t index = hash % map->capacity;
    for (size_t i = 0; i < map->capacity; i++) {
        HashEntry* entry = &map->entries[(index + i) % map->capacity];
        if (!entry->key) {
            return 0;
        }
        if (entry->hash == hash && entry->key_len == key_len && memcmp(entry->key, key, key_len) == 0) {
            if (out)
                *out = entry->value;
            return 1;
        }
    }

    return 0;
}

int hashmap_set(HashMap* map, const void* key, size_t key_len, uintptr_t value) {
    if (map->count >= map->capacity * HASHMAP_LOAD_FACTOR)
        hashmap_resize(map);

    uint32_t hash = murmurhash3(key, key_len);
    size_t index = hash % map->capacity;
    for (size_t i = 0; i < map->capacity; i++) {
        HashEntry* entry = &map->entries[(index + i) % map->capacity];
        if (!entry->key || (entry->hash == hash && entry->key_len == key_len && memcmp(entry->key, key, key_len) == 0)) {
            entry->hash = hash;
            entry->key = (void*)key;
            entry->key_len = key_len;
            entry->value = value;
            map->count++;
            return 1;
        }
    }

    return 0;
}

void hashmap_free(HashMap* map) {
    free(map->entries);
    free(map);
}