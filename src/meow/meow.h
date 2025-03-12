#ifndef __MEOW_H
#define __MEOW_H

#include "co.h"

typedef struct {
    u32 ip;
    u32 sp;
    MeoWMCodeObject* code;
    MwObject** stack;
    MeoWMStatusFlags flags;
} MeoWM;

MeoWM* meowm_new(MeoWMCodeObject* code);
int meowm_exec(MeoWM* MeoWM);
void meowm_free(MeoWM* MeoWM);

#endif