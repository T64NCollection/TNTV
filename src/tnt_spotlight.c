#include "vita_compat.h"
#include "tnt_game_types.h"
#include <string.h>

extern u8 g_tnt_spotlight_128x128[16384];

typedef struct {
    s32 uv[4][24][16];
    u8 counter;
} VitaMetalField;

typedef struct {
    s32 u;
    s32 v;
} VitaUV;

static VitaMetalField g_metal_field;

static s32 TNT_Vita_WrapAdd32(s32 a, s32 b) {
    return (s32)((u32)a + (u32)b);
}

static s32 TNT_Vita_FieldBase(s32 index, u32 bias) {
    s32 numerator = (s32)(((u32)index << 24) + bias);
    return numerator / 24;
}

static void TNT_Vita_MetalField_RecalcDeltas(void) {
    s32 i;
    s32 j;

    for (i = 0; i < 24; i++) {
        for (j = 0; j < 16; j++) {
            s32 tmp;

            tmp = (s32)((FUN_026900_PRNG_1() & 0x1FFFFF) + 0xFFF00000u);
            tmp = TNT_Vita_WrapAdd32(tmp, TNT_Vita_FieldBase(j, 0x07000000u));
            g_metal_field.uv[2][i][j] =
                (s32)((u32)tmp - (u32)g_metal_field.uv[0][i][j]) / 32;

            tmp = (s32)((FUN_026900_PRNG_1() & 0x1FFFFF) + 0xFFF00000u);
            tmp = TNT_Vita_WrapAdd32(tmp, TNT_Vita_FieldBase(i, 0x02000000u));
            g_metal_field.uv[3][i][j] =
                (s32)((u32)tmp - (u32)g_metal_field.uv[1][i][j]) / 32;
        }
    }
}

void TNT_Vita_MetalField_Init(void) {
    s32 i;
    s32 j;

    memset(&g_metal_field, 0, sizeof(g_metal_field));

    for (i = 0; i < 24; i++) {
        for (j = 0; j < 16; j++) {
            g_metal_field.uv[0][i][j] =
                TNT_Vita_FieldBase(j, 0x07000000u);
            g_metal_field.uv[1][i][j] =
                TNT_Vita_FieldBase(i, 0x02000000u);
        }
    }

    TNT_Vita_MetalField_RecalcDeltas();
    g_metal_field.counter = 32;
}

void TNT_Vita_MetalField_Update(u32 ticks) {
    while (ticks-- > 0) {
        s32 i;
        s32 j;

        for (i = 0; i < 24; i++) {
            for (j = 0; j < 16; j++) {
                g_metal_field.uv[0][i][j] = TNT_Vita_WrapAdd32(
                    g_metal_field.uv[0][i][j], g_metal_field.uv[2][i][j]);
                g_metal_field.uv[1][i][j] = TNT_Vita_WrapAdd32(
                    g_metal_field.uv[1][i][j], g_metal_field.uv[3][i][j]);
            }
        }

        if (--g_metal_field.counter == 0) {
            TNT_Vita_MetalField_RecalcDeltas();
            g_metal_field.counter = 32;
        }
    }
}

void TNT_Vita_MetalField_Deinit(void) {
}

static void TNT_Vita_FixedUV(s16 x, s16 y, VitaUV *uv) {
    uv->u = (x * 0xA00) + 0x800000;
    uv->v = (y * 0xA00) + 0x800000;
}

static void TNT_Vita_MetalUV(s16 x, s16 y, VitaUV *uv) {
    s32 j = (x >> 8) + 6;
    s32 i = (y >> 8) + 11;

    if (i < 0 || i >= 24 || j < 0 || j >= 16) {
        uv->u = 0;
        uv->v = 0;
        return;
    }

    uv->u = g_metal_field.uv[0][i][j];
    uv->v = g_metal_field.uv[1][i][j];
}

static void TNT_Vita_SampleSpotlight(
    const CubeTile *gradients,
    const VitaUV uv[4],
    u8 out[64]
) {
    s32 left_u = uv[0].u;
    s32 left_v = uv[0].v;
    s32 right_u = uv[1].u;
    s32 right_v = uv[1].v;
    s32 left_du = (uv[2].u - uv[0].u) >> 3;
    s32 left_dv = (uv[2].v - uv[0].v) >> 3;
    s32 right_du = (uv[3].u - uv[1].u) >> 3;
    s32 right_dv = (uv[3].v - uv[1].v) >> 3;
    s32 row;
    s32 out_idx = 0;

    for (row = 0; row < 8; row++) {
        s32 u = left_u;
        s32 v = left_v;
        s32 du = (right_u - left_u) >> 3;
        s32 dv = (right_v - left_v) >> 3;
        s32 col;

        for (col = 0; col < 8; col++, out_idx++) {
            const Gradient2D *gradient = &gradients->gradients[out_idx];
            s32 sx = (u >> 16) + ((s32)gradient->x << 2);
            s32 sy = (v >> 16) + ((s32)gradient->y << 2);

            if ((sx >> 8) == 0 && (sy >> 8) == 0) {
                sx >>= 1;
                sy >>= 1;
                out[out_idx] = g_tnt_spotlight_128x128[(sx << 7) | sy];
            } else {
                out[out_idx] = 0;
            }

            u += du;
            v += dv;
        }

        left_u += left_du;
        left_v += left_dv;
        right_u += right_du;
        right_v += right_dv;
    }
}

void TNT_Vita_MinoTextures_Update(Mino *mino_ptr, Cube *cube_ptr) {
    VitaUV fixed_uv[4];
    VitaUV metal_uv[4];
    s16 cx;
    s16 cy;
    s16 half;
    s16 px[4];
    s16 py[4];
    s32 i;
    u8 blend;

    if (mino_ptr == NULL || cube_ptr == NULL || g_minos_ptr == NULL) {
        return;
    }

    blend = (mino_ptr->unk20 < 0) ? 0 :
        (mino_ptr->unk20 > 0xFF ? 0xFF : (u8)mino_ptr->unk20);

    if (!(mino_ptr->unk1 & 1) && blend == 0) {
        return;
    }

    cx = cube_ptr->unk2.unk6.p.x + cube_ptr->unk2.unk0.p.x -
        g_minos_ptr->unkE.x;
    cy = cube_ptr->unk2.unk6.p.y + cube_ptr->unk2.unk0.p.y -
        g_minos_ptr->unkE.y;
    half = g_minos_ptr->unk8;

    px[0] = cx - half; py[0] = cy - half;
    px[1] = cx + half; py[1] = cy - half;
    px[2] = cx - half; py[2] = cy + half;
    px[3] = cx + half; py[3] = cy + half;

    for (i = 0; i < 4; i++) {
        TNT_Vita_FixedUV(px[i], py[i], &fixed_uv[i]);
        TNT_Vita_MetalUV(px[i], py[i], &metal_uv[i]);
    }

    /* The fixed spotlight depends only on cube geometry + CubeTile gradients.
     * During a settled metallic morph, blend changes and the animated metal
     * field changes every frame, but the fixed texture does not. Rebuild it
     * only when the source dirty bit says its inputs changed. This is exactly
     * the same fixed texture the old path redundantly regenerated each frame. */
    if (blend < 0xFF && (mino_ptr->unk1 & 1)) {
        TNT_Vita_SampleSpotlight(
            &mino_ptr->unk24, fixed_uv, mino_ptr->unkB0);
    }
    if (blend > 0) {
        TNT_Vita_SampleSpotlight(
            &mino_ptr->unk24, metal_uv, mino_ptr->unkF0);
    }

    mino_ptr->unk1 &= ~1;
}

/*
 * Vita adapter for the original falling MobilePiece Minos_Mino_Render path.
 * MobilePiece_Render decomposes rotation into a coarse quarter-turn used for
 * the mino center plus the low 14-bit residual used to rotate the mino quad.
 * The resulting four world-space corners feed the same fixed/metal spotlight
 * coordinate generators as the N64 renderer.
 */
void TNT_Vita_MinoTextures_UpdatePiece(
    Mino *mino_ptr,
    Piece *piece_ptr,
    u8 mino_idx,
    u16 mino_scale
) {
    VitaUV fixed_uv[4];
    VitaUV metal_uv[4];
    Point center;
    s32 residual;
    s32 sin_val;
    s32 cos_val;
    s32 half;
    s32 offsets[4][2];
    s32 i;
    u8 blend;

    if (mino_ptr == NULL || piece_ptr == NULL || g_minos_ptr == NULL) {
        return;
    }

    TNT_Vita_MobilePieceGetMinoPointScaled(
        piece_ptr, mino_idx, mino_scale, &center);

    center.x -= g_minos_ptr->unkE.x;
    center.y -= g_minos_ptr->unkE.y;

    residual = piece_ptr->physicalCkwRotValue & 0x3FFF;
    sin_val = FUN_026900_800608f4_oneliner_shift6_1((u16)residual);
    cos_val = FUN_026900_80060914_oneliner_shift6_2((u16)residual);
    half = mino_scale >> 1;

    offsets[0][0] = -half; offsets[0][1] = -half;
    offsets[1][0] =  half; offsets[1][1] = -half;
    offsets[2][0] = -half; offsets[2][1] =  half;
    offsets[3][0] =  half; offsets[3][1] =  half;

    for (i = 0; i < 4; i++) {
        s32 ox = offsets[i][0];
        s32 oy = offsets[i][1];
        s16 px = (s16)(center.x +
            (((ox * cos_val) - (oy * sin_val)) >> 14));
        s16 py = (s16)(center.y +
            (((ox * sin_val) + (oy * cos_val)) >> 14));

        TNT_Vita_FixedUV(px, py, &fixed_uv[i]);
        TNT_Vita_MetalUV(px, py, &metal_uv[i]);
    }

    blend = (mino_ptr->unk20 < 0) ? 0 :
        (mino_ptr->unk20 > 0xFF ? 0xFF : (u8)mino_ptr->unk20);

    /* MobilePiece_Render marks every moving Mino dirty every frame. */
    if (blend < 0xFF) {
        TNT_Vita_SampleSpotlight(
            &mino_ptr->unk24, fixed_uv, mino_ptr->unkB0);
    }
    if (blend > 0) {
        TNT_Vita_SampleSpotlight(
            &mino_ptr->unk24, metal_uv, mino_ptr->unkF0);
    }

    mino_ptr->unk1 &= ~1;
}
