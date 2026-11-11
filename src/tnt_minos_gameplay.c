#include "vita_compat.h"
#include "tnt_game_types.h"
#include <string.h>
#include <arm_neon.h>

/*
 * Gameplay-side Minos routines extracted from the original code.
 * N64 texture/display-list operations will be restored through the Vita
 * renderer later.
 */

static void TNT_Vita_CubeTileCopy(CubeTile *dst, const CubeTile *src) {
    memcpy(dst, src, sizeof(*dst));
}

static s32 TNT_Vita_CubeTileMorphToward(
    const CubeTile *target,
    CubeTile *current
) {
    const s8 *target_ptr = (const s8 *)target;
    s8 *current_ptr = (s8 *)current;
    uint8x16_t any_changed = vdupq_n_u8(0);
    const uint8x16_t one = vdupq_n_u8(1);
    s32 i;

    /* CubeTile is exactly 128 signed gradient bytes. The original loop moves
     * every byte one signed step toward its target per displayed frame. Do the
     * same 16 bytes at a time on Vita's Cortex-A9 NEON unit. There is no
     * saturation/rounding change: a lane can only increment when target is
     * greater (so +1 cannot overflow 127), and only decrement when target is
     * smaller (so -1 cannot underflow -128). */
    _Static_assert(sizeof(CubeTile) == 128, "CubeTile NEON morph size");
    for (i = 0; i < (s32)sizeof(CubeTile); i += 16) {
        int8x16_t cur = vld1q_s8(current_ptr + i);
        int8x16_t dst = vld1q_s8(target_ptr + i);
        uint8x16_t less = vcltq_s8(cur, dst);
        uint8x16_t greater = vcgtq_s8(cur, dst);
        uint8x16_t delta_up = vandq_u8(less, one);
        uint8x16_t delta_down = vandq_u8(greater, one);

        cur = vaddq_s8(cur, vreinterpretq_s8_u8(delta_up));
        cur = vsubq_s8(cur, vreinterpretq_s8_u8(delta_down));
        vst1q_s8(current_ptr + i, cur);
        any_changed = vorrq_u8(any_changed, vorrq_u8(less, greater));
    }

    return (vgetq_lane_u64(vreinterpretq_u64_u8(any_changed), 0) |
            vgetq_lane_u64(vreinterpretq_u64_u8(any_changed), 1)) != 0;
}

void Minos_800702e4_nineliner(Mino *arg0) {
    Minos_80070818_oneliner_sets_arg0_0x130_to_arg1(
        arg0, g_cubeTiles_ptr->unkDC00[0]);
    TNT_Vita_CubeTileCopy(&arg0->unk24, arg0->unk130);
    arg0->unk1C = 0xFF;
    arg0->unk1E = arg0->unk1C;
    arg0->unk1D = 0;
    arg0->unk20 = 0;
    arg0->unk22 = 0;
    arg0->unk0 = 1;
    arg0->unk1 |= 1;
}

void Minos_80070398_fourteenliner(Mino *arg0, Mino *arg1) {
    arg0->unk0 = arg1->unk0;
    arg0->unk1 = arg1->unk1;
    arg0->unk20 = arg1->unk20;
    arg0->unk22 = arg1->unk22;
    arg0->unk130 = arg1->unk130;
    arg0->unk1C = arg1->unk1C;
    arg0->unk2 = arg1->unk2;
    arg0->unk4 = arg1->unk4;
    arg0->unk10 = arg1->unk10;
    memcpy(&arg0->unk24, &arg1->unk24, sizeof(arg0->unk24));
}

void Minos_80070528_Morph(Mino *arg0) {
    if (arg0->unk130 != NULL) {
        if (TNT_Vita_CubeTileMorphToward(arg0->unk130, &arg0->unk24) == 0) {
            arg0->unk130 = NULL;
        }
        arg0->unk1 |= 1;
    }

    if (arg0->unk2 != 0) {
        arg0->unk4.diffuse_red    += arg0->unk10.diffuse_red;
        arg0->unk4.diffuse_green  += arg0->unk10.diffuse_green;
        arg0->unk4.diffuse_blue   += arg0->unk10.diffuse_blue;
        arg0->unk4.specular_red   += arg0->unk10.specular_red;
        arg0->unk4.specular_green += arg0->unk10.specular_green;
        arg0->unk4.specular_blue  += arg0->unk10.specular_blue;
        arg0->unk2 -= 1;
    }

    if (arg0->unk1C != arg0->unk1E) {
        u8 var_s0 = arg0->unk1C;
        u8 var_s1 = arg0->unk1E;

        if (arg0->unk1D > 0) {
            if ((arg0->unk1D + var_s0) >= var_s1) {
                arg0->unk1C = arg0->unk1E;
                arg0->unk1D = 0;
            } else {
                arg0->unk1C += arg0->unk1D;
            }
        }

        if (arg0->unk1D < 0) {
            if ((arg0->unk1D + var_s0) <= var_s1) {
                arg0->unk1C = arg0->unk1E;
                arg0->unk1D = 0;
            } else {
                arg0->unk1C += arg0->unk1D;
            }
        }
    }

    if (arg0->unk22 != 0) {
        arg0->unk20 += arg0->unk22;

        if (arg0->unk20 < 0) {
            arg0->unk20 = 0;
            arg0->unk22 = 0;
        }

        if (arg0->unk20 > 0xFF) {
            arg0->unk20 = 0xFF;
            arg0->unk22 = 0;
        }
    }
}

void Minos_80070818_oneliner_sets_arg0_0x130_to_arg1(
    Mino *arg0, CubeTile *arg1) {
    arg0->unk130 = arg1;
}

void Minos_80070820_fiveliner_sets_arg0_2_4_8_12_to_arg1(
    Mino *arg0, Color *arg1) {
    arg0->unk4 = *arg1;
    arg0->unk2 = 0;
}

static inline s16 TNT_Vita_Div16TruncS16Delta(s32 delta) {
    /* C signed division truncates toward zero. An arithmetic >>4 would round
     * negative values toward -infinity, so mirror the positive magnitude to
     * preserve the original /16 result exactly. Color deltas fit safely in s32. */
    return (s16)(delta >= 0 ? (delta >> 4) : -((-delta) >> 4));
}

void Minos_80070860_fortyliner(Mino *arg0, Color *arg1, s32 arg2) {
    if (arg2 == 16) {
        /* Multisquare creation morphs all 16 cubes over exactly 16 frames.
         * Avoid 6 software __aeabi_idiv calls per cube on the formation frame. */
        arg0->unk10.diffuse_red = TNT_Vita_Div16TruncS16Delta(
            (s32)arg1->diffuse_red - arg0->unk4.diffuse_red);
        arg0->unk10.diffuse_green = TNT_Vita_Div16TruncS16Delta(
            (s32)arg1->diffuse_green - arg0->unk4.diffuse_green);
        arg0->unk10.diffuse_blue = TNT_Vita_Div16TruncS16Delta(
            (s32)arg1->diffuse_blue - arg0->unk4.diffuse_blue);
        arg0->unk10.specular_red = TNT_Vita_Div16TruncS16Delta(
            (s32)arg1->specular_red - arg0->unk4.specular_red);
        arg0->unk10.specular_green = TNT_Vita_Div16TruncS16Delta(
            (s32)arg1->specular_green - arg0->unk4.specular_green);
        arg0->unk10.specular_blue = TNT_Vita_Div16TruncS16Delta(
            (s32)arg1->specular_blue - arg0->unk4.specular_blue);
    } else {
        arg0->unk10.diffuse_red =
            (arg1->diffuse_red - arg0->unk4.diffuse_red) / arg2;
        arg0->unk10.diffuse_green =
            (arg1->diffuse_green - arg0->unk4.diffuse_green) / arg2;
        arg0->unk10.diffuse_blue =
            (arg1->diffuse_blue - arg0->unk4.diffuse_blue) / arg2;
        arg0->unk10.specular_red =
            (arg1->specular_red - arg0->unk4.specular_red) / arg2;
        arg0->unk10.specular_green =
            (arg1->specular_green - arg0->unk4.specular_green) / arg2;
        arg0->unk10.specular_blue =
            (arg1->specular_blue - arg0->unk4.specular_blue) / arg2;
    }

    arg0->unk2 = arg2;
}

void Minos_800709d8_threeliner_sets_arg0_1c_1e_1d(
    Mino *arg0, u8 arg1) {
    arg0->unk1C = arg1;
    arg0->unk1E = arg1;
    arg0->unk1D = 0;
}

static Minos g_minos_storage = {
    .scale = 0x100
};

Minos *g_minos_ptr = &g_minos_storage;

/*
 * Original Minos coordinate-transform setup used by PFGFX before gameplay
 * objects such as PieceHold are initialized.  The N64 display-list side is
 * deferred, but these fields are gameplay-visible fixed-point state.
 */
void Minos_80070a34_twentyliner(void) {
    Minos *minos_ptr = g_minos_ptr;
    u32 var_a1;

    if (minos_ptr->pattern & 0x8) {
        minos_ptr->unkE = minos_ptr->unk16;
    }

    if (minos_ptr->pattern & 0xD) {
        minos_ptr->unk12.x = minos_ptr->unk1A.x +
            ((minos_ptr->unkE.x * minos_ptr->unk2) >> 14) +
            (minos_ptr->unk2 >> 7);
        minos_ptr->unk12.y = minos_ptr->unk1A.y +
            ((minos_ptr->unkE.y * minos_ptr->unk2) >> 14) +
            (minos_ptr->unk2 >> 7);
    }

    if (minos_ptr->pattern & 0x1) {
        minos_ptr->unk4 = (float)0x1000000 / minos_ptr->unk2 + 0.5f;
    }

    if (minos_ptr->pattern & 0x3) {
        var_a1 = minos_ptr->unk2 * minos_ptr->scale;
        var_a1 >>= 8;
        minos_ptr->unk8 = minos_ptr->scale >> 1;
        minos_ptr->dsdx = 0x200000 / var_a1;
        minos_ptr->dtdy = 0x200000 / var_a1;
    }

    minos_ptr->pattern = 0;
}

void Minos_80070c40_twoliner_set_OR_1(u16 arg0) {
    g_minos_ptr->unk2 = arg0;
    g_minos_ptr->pattern |= 0x1;
}

void Minos_80070c70_threeliner_set_OR_8(s16 x, s16 y) {
    g_minos_ptr->unk16.x = x;
    g_minos_ptr->unk16.y = y;
    g_minos_ptr->pattern |= 0x8;
}

void Minos_80070cb8_threeliner_set_OR_4(s16 x, s16 y) {
    g_minos_ptr->unk1A.x = x;
    g_minos_ptr->unk1A.y = y;
    g_minos_ptr->pattern |= 0x4;
}

void Minos_8007104c_fiveliner_nuts(Point *arg0, s16 x, s16 y) {
    Minos *minos_ptr = g_minos_ptr;

    x -= minos_ptr->unk12.x;
    y -= minos_ptr->unk12.y;

    x = (x * minos_ptr->unk4) >> 10;
    y = (y * minos_ptr->unk4) >> 10;

    arg0->x = x + minos_ptr->unkE.x;
    arg0->y = y + minos_ptr->unkE.y;
}

void Minos_80071100_fiveliner_nuts2(Point *arg0, PointAngle *arg1) {
    Minos *minos_ptr = g_minos_ptr;
    s32 tmp_x = arg1->p.x - minos_ptr->unkE.x;
    s32 tmp_y = arg1->p.y - minos_ptr->unkE.y;

    tmp_x = (tmp_x * minos_ptr->unk2) >> 14;
    tmp_y = (tmp_y * minos_ptr->unk2) >> 14;

    arg0->x = tmp_x + minos_ptr->unk12.x;
    arg0->y = tmp_y + minos_ptr->unk12.y;
}

void Minos_SetScale(u16 scale) {
    g_minos_ptr->scale = scale;
    g_minos_ptr->pattern |= 0x2;
}

/* Original Mino color-alpha transition setter used by GameFinish row sweep. */
void Minos_800709EC(Mino *arg0, u8 arg1, s32 arg2) {
    arg0->unk1E = arg1;
    arg0->unk1D = (arg1 - arg0->unk1C) / arg2;
}
