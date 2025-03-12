#ifndef __MWOBJ_H
#define __MWOBJ_H

#include <stddef.h>

#define mwobj_decref(obj) if (--(obj)->refcnt == 0) mwobj_free(obj)
#define mwobj_incref(obj) ((obj)->refcnt++);

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef size_t  u64;

typedef struct {
    u8 zf: 1;
    u8 sf: 1;
    u8 reserved: 6;
} MeoWMStatusFlags;

typedef struct {
    const char* data;
    u64 len;
} MWString;

typedef enum {
    OBJ_NONE,
    OBJ_INT,
    OBJ_FLOAT,
    OBJ_STR,
} MwObjectType;

typedef union {
    int i;
    float f;
    MWString str;
} MwObjectValue;

typedef struct {
    u32 refcnt;
    MwObjectType type;
    u8 size;
    MwObjectValue value;
} MwObject;

MwObject* mwobj_new(MwObjectType type, MwObjectValue value, u8 size);
MwObject* mwobj_add(MwObject* a, MwObject* b);
MwObject* mwobj_sub(MwObject* a, MwObject* b);
MwObject* mwobj_mul(MwObject* a, MwObject* b);
MwObject* mwobj_div(MwObject* a, MwObject* b);
MeoWMStatusFlags mwobj_cmp(MwObject* a, MwObject* b);
void mwobj_out(MwObject* obj);
void mwobj_free(MwObject* obj);

#endif