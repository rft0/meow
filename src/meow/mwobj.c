#include "mwobj.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

MwObject* mwobj_new(MwObjectType type, MwObjectValue value, u8 size) {
    MwObject* obj = (MwObject*)malloc(sizeof(MwObject));
    obj->refcnt = 1;
    obj->type = type;
    obj->size = size;
    obj->value = value;

    if (type == OBJ_STR) {
        obj->value.str.data = (const char*)malloc(value.str.len);
        memcpy((void*)obj->value.str.data, value.str.data, value.str.len);
    }

    return obj;
}

MwObject* mwobj_add(MwObject* a, MwObject* b) {
    if (a->type == OBJ_INT && b->type == OBJ_INT)
        return mwobj_new(OBJ_INT, (MwObjectValue){.i = a->value.i + b->value.i}, 4);
    if (a->type == OBJ_FLOAT && b->type == OBJ_FLOAT)
        return mwobj_new(OBJ_FLOAT, (MwObjectValue){.f = a->value.f + b->value.f}, 4);

    if (a->type == OBJ_INT && b->type == OBJ_FLOAT)
        return mwobj_new(OBJ_FLOAT, (MwObjectValue){.f = a->value.i + b->value.f}, 4);
    if (a->type == OBJ_FLOAT && b->type == OBJ_INT)
        return mwobj_new(OBJ_FLOAT, (MwObjectValue){.f = a->value.f + b->value.i}, 4);

    if (a->type == OBJ_STR || b->type == OBJ_STR) {
        char buf_a[64] = {0};
        char buf_b[64] = {0};
        u64 len_a = 0, len_b = 0;
        const char *str_a, *str_b;
        
        // Get string representation of a
        if (a->type == OBJ_STR) {
            str_a = a->value.str.data;
            len_a = a->value.str.len;
        } else if (a->type == OBJ_INT) {
            len_a = snprintf(buf_a, sizeof(buf_a), "%d", a->value.i);
            str_a = buf_a;
        } else if (a->type == OBJ_FLOAT) {
            len_a = snprintf(buf_a, sizeof(buf_a), "%g", a->value.f);
            str_a = buf_a;
        }
        
        // Get string representation of b
        if (b->type == OBJ_STR) {
            str_b = b->value.str.data;
            len_b = b->value.str.len;
        } else if (b->type == OBJ_INT) {
            len_b = snprintf(buf_b, sizeof(buf_b), "%d", b->value.i);
            str_b = buf_b;
        } else if (b->type == OBJ_FLOAT) {
            len_b = snprintf(buf_b, sizeof(buf_b), "%g", b->value.f);
            str_b = buf_b;
        }
        
        u64 total_len = len_a + len_b;
        char* combined = (char*)malloc(total_len);
        memcpy(combined, str_a, len_a);
        memcpy(combined + len_a, str_b, len_b);
        
        return mwobj_new(OBJ_STR, (MwObjectValue){.str = {combined, total_len}}, 4);
    }

    return NULL;
}

MwObject* mwobj_sub(MwObject* a, MwObject* b) {
    if (a->type == OBJ_INT && b->type == OBJ_INT)
        return mwobj_new(OBJ_INT, (MwObjectValue){.i = a->value.i - b->value.i}, 4);
    if (a->type == OBJ_FLOAT && b->type == OBJ_FLOAT)
        return mwobj_new(OBJ_FLOAT, (MwObjectValue){.f = a->value.f - b->value.f}, 4);

    if (a->type == OBJ_INT && b->type == OBJ_FLOAT)
        return mwobj_new(OBJ_FLOAT, (MwObjectValue){.f = a->value.i - b->value.f}, 4);
    if (a->type == OBJ_FLOAT && b->type == OBJ_INT)
        return mwobj_new(OBJ_FLOAT, (MwObjectValue){.f = a->value.f - b->value.i}, 4);

    return NULL;
}

MwObject* mwobj_mul(MwObject* a, MwObject* b) {
    if (a->type == OBJ_INT && b->type == OBJ_INT)
        return mwobj_new(OBJ_INT, (MwObjectValue){.i = a->value.i * b->value.i}, 4);
    if (a->type == OBJ_FLOAT && b->type == OBJ_FLOAT)
        return mwobj_new(OBJ_FLOAT, (MwObjectValue){.f = a->value.f * b->value.f}, 4);

    if (a->type == OBJ_INT && b->type == OBJ_FLOAT)
        return mwobj_new(OBJ_FLOAT, (MwObjectValue){.f = a->value.i * b->value.f}, 4);
    if (a->type == OBJ_FLOAT && b->type == OBJ_INT)
        return mwobj_new(OBJ_FLOAT, (MwObjectValue){.f = a->value.f * b->value.i}, 4);

    if (a->type == OBJ_STR && b->type == OBJ_INT) {
        u64 len = a->value.str.len * b->value.i;
        char* data = (char*)malloc(len);
        for (u64 i = 0; i < b->value.i; i++)
            memcpy(data + i * a->value.str.len, a->value.str.data, a->value.str.len);
        return mwobj_new(OBJ_STR, (MwObjectValue){.str = {data, len}}, 4);
    }
    if (a->type == OBJ_INT && b->type == OBJ_STR) {
        u64 len = a->value.i * b->value.str.len;
        char* data = (char*)malloc(len);
        for (u64 i = 0; i < a->value.i; i++)
            memcpy(data + i * b->value.str.len, b->value.str.data, b->value.str.len);
        return mwobj_new(OBJ_STR, (MwObjectValue){.str = {data, len}}, 4);
    }

    return NULL;
}

MwObject* mwobj_div(MwObject* a, MwObject* b) {
    if (a->type == OBJ_INT && b->type == OBJ_INT)
        return mwobj_new(OBJ_INT, (MwObjectValue){.i = a->value.i / b->value.i}, 4);
    if (a->type == OBJ_FLOAT && b->type == OBJ_FLOAT)
        return mwobj_new(OBJ_FLOAT, (MwObjectValue){.f = a->value.f / b->value.f}, 4);

    if (a->type == OBJ_INT && b->type == OBJ_FLOAT)
        return mwobj_new(OBJ_FLOAT, (MwObjectValue){.f = a->value.i / b->value.f}, 4);
    if (a->type == OBJ_FLOAT && b->type == OBJ_INT)
        return mwobj_new(OBJ_FLOAT, (MwObjectValue){.f = a->value.f / b->value.i}, 4);


    return NULL;
}

MeoWMStatusFlags mwobj_cmp(MwObject* a, MwObject* b) {
    MeoWMStatusFlags flags = {0};
    if (a == b) {
        flags.zf = 1;
        return flags;
    }

    if (a->type == OBJ_INT && b->type == OBJ_INT) {
        // Sucks right? I know.
        if (a->value.i == b->value.i)
            flags.zf = 1;
        if (a->value.i < b->value.i)
            flags.sf = 1;
    }

    if (a->type == OBJ_FLOAT && b->type == OBJ_FLOAT) {
        if (a->value.f == b->value.f)
            flags.zf = 1;
        if (a->value.f < b->value.f)
            flags.sf = 1;
    }

    if (a->type == OBJ_STR && b->type == OBJ_STR) {
        int mlen = (a->value.str.len < b->value.str.len) ? a->value.str.len : b->value.str.len;
        int res = strncmp(a->value.str.data, b->value.str.data, mlen);
        if (res == 0) {
            if (a->value.str.len == b->value.str.len)
                flags.zf = 1;
            else if (a->value.str.len < b->value.str.len)
                flags.sf = 1;
        } else if (res < 0) {
            flags.sf = 1;
        }
    }

    return flags;
}

void mwobj_out(MwObject* obj) {
    if (!obj || obj->type == OBJ_NONE)
        printf("NONE\n");
    else if (obj->type == OBJ_INT)
        printf("%d\n", obj->value.i);
    else if (obj->type == OBJ_FLOAT)
        printf("%f\n", obj->value.f);
    else if (obj->type == OBJ_STR)
        printf("%.*s\n", (int)obj->value.str.len, obj->value.str.data);
}

void mwobj_free(MwObject* obj) {
    if (obj->type == OBJ_STR)
        free((void*)obj->value.str.data);
    free(obj);
}