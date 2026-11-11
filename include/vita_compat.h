#ifndef VITA_COMPAT_H
#define VITA_COMPAT_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

typedef int8_t   s8;
typedef uint8_t  u8;
typedef int16_t  s16;
typedef uint16_t u16;
typedef int32_t  s32;
typedef uint32_t u32;
typedef int64_t  s64;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

#define CONT_A      0x8000
#define CONT_B      0x4000
#define CONT_G      0x2000
#define CONT_START  0x1000
#define CONT_UP     0x0800
#define CONT_DOWN   0x0400
#define CONT_LEFT   0x0200
#define CONT_RIGHT  0x0100
#define CONT_L      0x0020
#define CONT_R      0x0010
#define CONT_E      0x0008
#define CONT_D      0x0004
#define CONT_C      0x0002
#define CONT_F      0x0001

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef struct {
    u16 button;
    s8 stick_x;
    s8 stick_y;
    u8 errno;
} OSContPad;

typedef struct {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    s16 unk48;
    s16 unk4A;
    s16 unk4C;
    s16 unk4E;
    s16 unk50;
    s16 unk52;
} Controller;

typedef struct {
    u32 button;
    s16 stick_x;
    s16 stick_y;
} ControllerInfo;

typedef struct {
    s16 tail;
    s16 head;
    s16 InfoSize;
    s32 enqueue_cnt;
    s32 dequeue_cnt;
    ControllerInfo *_ControllerQueue;
    Controller *unk14;
} ControllerQueue;

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
} UnkStruct_25;

typedef struct {
    UnkStruct_25 unk0[32];
    u32 unk80;
    u32 unk84;
    u32 unk88;
    u32 unk8C;
} UnkStruct_1;

typedef struct {
    s16 cont_no;
    ControllerQueue contQ;
    Controller *unk1C;
    u32 unk20;
    u32 unk24;
    UnkStruct_1 unk28;
} PlayerVars;

void func_800B1770(Controller *controller, OSContPad *pad);
void func_800B1A7C(Controller *controller);

void FUN_069580_800A3300_nineliner_mod300(ControllerQueue *queue, OSContPad *pad);
void func_800A33E4(ControllerQueue *queue);
void func_800A3460(ControllerQueue *queue);
void FUN_069580_800A34A8_tenliner_allocs_heap(ControllerQueue *queue);
Controller *func_800A352C(ControllerQueue *queue);
s16 func_800A3534(ControllerQueue *queue);


extern s32 D_800CFED4;

void FUN_026900_8006093c_rounds_floors_loops_1024t(void);
void func_80075DF4(void);
void initPieceColorsScale16(void);

void FUN_026C80_nineliner_loops_32t(UnkStruct_1 *arg0);

void debug_print_reason_routine(const char *reason, const char *routine);


void FUN_026C80_80060ad4_oneliner_calls_fun(UnkStruct_1 *arg0);
void FUN_026C80_80060b04_twelveliner_loops_32t(UnkStruct_1 *arg0, u32 arg1, u8 arg2, u8 arg3);
void FUN_026C80_GU_or_ControllerRepeat_Update(UnkStruct_1 *arg0, u32 arg1, u32 arg2);
void FUN_026C80_nineliner_loops_32t(UnkStruct_1 *arg0);

#define n64HeapAlloc(size) malloc(size)

static inline void n64HeapUnalloc(void *ptr) {
    free(ptr);
}

#endif
