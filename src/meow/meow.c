#include "meow.h"

#include <stdlib.h>
#include <stdio.h>

#include "../mwerror.h"

MeoWM* meowm_new(MeoWMCodeObject* code) {
    MeoWM* vm = (MeoWM*)malloc(sizeof(MeoWM));
    vm->ip = 0;
    vm->sp = 0;
    vm->code = code;
    vm->stack = (MwObject**)malloc(sizeof(MwObject*) * code->max_stack_size);
    vm->flags = (MeoWMStatusFlags){0};

    return vm;
}

void meowm_free(MeoWM* vm) {
    free_mwerrors();

    for (int i = 0; i < vm->sp; i++)
        mwobj_decref(vm->stack[i]);
    co_free(vm->code);
    free(vm->stack);
    free(vm);
}

int meowm_exec(MeoWM* vm) {
    while (vm->ip < vm->code->size) {
        u8 op = vm->code->bytecode[vm->ip];
        // printf("op: %d\n", op);
        switch (op) {
        case OP_STORE_VAR:
        {
            if (!vm->sp) {
                printf("Runtime Error: Stack underflow at line %d\n", lnotab_get_line_from_ip(vm->code->lnotab, vm->ip));
                return 1;
            }

            uintptr_t idx = *(u16*)(vm->code->bytecode + vm->ip + 1);
            if (vm->code->vars[idx])
                mwobj_decref(vm->code->vars[idx]);

            vm->code->vars[idx] = vm->stack[--vm->sp];
            mwobj_incref(vm->code->vars[idx]);
            vm->ip += 3;
        } break;
        case OP_LOAD_VAR: {
            uintptr_t idx = *(u16*)(vm->code->bytecode + vm->ip + 1);
            vm->stack[vm->sp++] = vm->code->vars[idx];
            mwobj_incref(vm->code->vars[idx]);
            vm->ip += 3;
        } break;
        case OP_LOAD_CONST:
        {
            uintptr_t idx = *(u16*)(vm->code->bytecode + vm->ip + 1);
            vm->stack[vm->sp++] = vm->code->constants[idx];
            mwobj_incref(vm->code->constants[idx]);
            vm->ip += 3;
        } break;
        case OP_ADD:
        {
            if (vm->sp < 2) {
                printf("Runtime Error: Stack underflow at line %d\n", lnotab_get_line_from_ip(vm->code->lnotab, vm->ip));
                return 1;
            }

            MwObject* a = vm->stack[--vm->sp];
            MwObject* b = vm->stack[--vm->sp];

            MwObject* res = mwobj_add(b, a);
            if (!res) {
                printf("Runtime Error: Type mismatch between operands at line %d\n", lnotab_get_line_from_ip(vm->code->lnotab, vm->ip));
                return 1;
            }

            mwobj_incref(res);
            mwobj_decref(a);
            mwobj_decref(b);
            vm->stack[vm->sp++] = res;
            vm->ip++;
        } break;
        case OP_SUB:
        {
            if (vm->sp < 2) {
                printf("Runtime Error: Stack underflow at line %d\n", lnotab_get_line_from_ip(vm->code->lnotab, vm->ip));
                return 1;
            }

            MwObject* a = vm->stack[--vm->sp];
            MwObject* b = vm->stack[--vm->sp];

            MwObject* res = mwobj_sub(b, a);

            mwobj_incref(res);
            mwobj_decref(a);
            mwobj_decref(b);
            vm->stack[vm->sp++] = res;
            vm->ip++;
        } break;
        case OP_MUL:
        {
            if (vm->sp < 2) {
                printf("Runtime Error: Stack underflow at line %d\n", lnotab_get_line_from_ip(vm->code->lnotab, vm->ip));
                return 1;
            }

            MwObject* a = vm->stack[--vm->sp];
            MwObject* b = vm->stack[--vm->sp];

            MwObject* res = mwobj_mul(b, a);

            mwobj_incref(res);
            mwobj_decref(a);
            mwobj_decref(b);
            vm->stack[vm->sp++] = res;
            vm->ip++;
        } break;
        case OP_DIV:
        {
            if (vm->sp < 2) {
                printf("Runtime Error: Stack underflow at line %d\n", lnotab_get_line_from_ip(vm->code->lnotab, vm->ip));
                return 1;
            }

            MwObject* a = vm->stack[--vm->sp];
            MwObject* b = vm->stack[--vm->sp];

            if (b->type == OBJ_INT && b->value.i == 0 || b->type == OBJ_FLOAT && b->value.f == 0) {
                printf("Runtime error: Division by zero at line %d\n", lnotab_get_line_from_ip(vm->code->lnotab, vm->ip));
                return 1;
            }

            MwObject* res = mwobj_div(b, a);

            mwobj_incref(res);
            mwobj_decref(a);
            mwobj_decref(b);
            vm->stack[vm->sp++] = res;
            vm->ip++;
        } break;
        case OP_CMP: {
            if (vm->sp < 2) {
                printf("Runtime Error: Stack underflow at line %d\n", lnotab_get_line_from_ip(vm->code->lnotab, vm->ip));
                return 1;
            }

            MwObject* a = vm->stack[--vm->sp];
            MwObject* b = vm->stack[--vm->sp];
            vm->flags = mwobj_cmp(b, a);
            
            mwobj_decref(a);
            mwobj_decref(b);
            vm->ip++;
        } break;
        case OP_JMP:
        {
            vm->ip = *(u32*)(vm->code->bytecode + vm->ip + 1);
        } break;
        case OP_JE: {
            if (vm->flags.zf)
                vm->ip = *(u32*)(vm->code->bytecode + vm->ip + 1);
            else
                vm->ip += 5;
        } break;
        case OP_JNE: {
            if (!vm->flags.zf)
                vm->ip = *(u32*)(vm->code->bytecode + vm->ip + 1);
            else
                vm->ip += 5;
        } break;
        case OP_JG: {
            if (!vm->flags.sf)
                vm->ip = *(u32*)(vm->code->bytecode + vm->ip + 1);
            else
                vm->ip += 5;
        } break;
        case OP_JGE: {
            if (!vm->flags.sf || vm->flags.zf)
                vm->ip = *(u32*)(vm->code->bytecode + vm->ip + 1);
            else
                vm->ip += 5;
        } break;
        case OP_JL: {
            if (vm->flags.sf)
                vm->ip = *(u32*)(vm->code->bytecode + vm->ip + 1);
            else {

            printf("jmp offset: %d\n", *(u32*)(vm->code->bytecode + vm->ip + 1));
                vm->ip += 5;
            }
        } break;
        case OP_JLE: {
            if (vm->flags.sf || vm->flags.zf)
                vm->ip = *(u32*)(vm->code->bytecode + vm->ip + 1);
            else
                vm->ip += 5;
        } break;
        case OP_OUT: {
            if (!vm->sp) {
                printf("Runtime Error: Stack underflow at line %d\n", lnotab_get_line_from_ip(vm->code->lnotab, vm->ip));
                return 1;
            }

            MwObject* obj = vm->stack[--vm->sp];
            mwobj_out(obj);

            mwobj_decref(obj);
            vm->ip += 1;
        } break;
        case OP_EXIT: {
            return 0;
        } break;
        }
    }

    return 0;
}