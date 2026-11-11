#include "vita_compat.h"
#include "tnt_game_types.h"
#include <psp2/kernel/processmgr.h>

/* Original frontend/AI LCG.  Seed lazily from the Vita process clock, matching
 * the original intent of seeding this stream from osGetTime(). */
static u32 g_tnt_rand_seed;
static u8 g_tnt_rand_seeded;

static u32 tnt_rand_next(void) {
    if (!g_tnt_rand_seeded) {
        g_tnt_rand_seed = (u32)sceKernelGetProcessTimeWide();
        g_tnt_rand_seeded = TRUE;
    }
    g_tnt_rand_seed = g_tnt_rand_seed * 10001U + 100001U;
    return g_tnt_rand_seed;
}

s32 func_800A35EC(s32 arg0, s32 arg1) {
    s32 tmp;
    if (arg0 == arg1) return arg0;
    if (arg1 < arg0) {
        tmp = arg0;
        arg0 = arg1;
        arg1 = tmp;
    }
    return (s32)(tnt_rand_next() % (u32)((arg1 - arg0) + 1)) + arg0;
}
