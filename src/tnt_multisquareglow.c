#include "vita_compat.h"
#include "tnt_game_types.h"
#include <stdint.h>

extern u8 g_tnt_square_glow_ia8[66 * 66];

static s32 TNT_Vita_GlowUnitSize(const SquareGlow *glow_ptr) {
    switch (glow_ptr->size) {
    case 0: return 11;
    case 1: return 10;
    case 2: return 8;
    default: return 11;
    }
}

void MultisquareGlow_Init(SquareGlow *glow_ptr, Point location, u8 unitSize) {
    const s32 width = 66;
    s32 square_width;

    glow_ptr->is_active = TRUE;
    glow_ptr->num_steps = 0x40;
    glow_ptr->val = 0;
    glow_ptr->rate = 0x8000 / glow_ptr->num_steps;
    glow_ptr->alpha = 0xFF;

    switch (unitSize) {
    case 11:
        glow_ptr->size = 0;
        square_width = 44;
        break;
    case 10:
        glow_ptr->size = 1;
        square_width = 40;
        break;
    case 8:
        glow_ptr->size = 2;
        square_width = 32;
        break;
    default:
        glow_ptr->size = 0;
        square_width = 44;
        break;
    }

    glow_ptr->location.x = (location.x >> 2) - ((width - square_width) >> 1);
    glow_ptr->location.y = (location.y >> 2) - ((width - square_width) >> 1);
}

void MultisquareGlow_8006af00_eightliner(SquareGlow *glow_ptr, s32 ticks) {
    if (!glow_ptr->is_active) {
        return;
    }

    if (glow_ptr->num_steps < ticks) {
        ticks = glow_ptr->num_steps;
    }

    glow_ptr->num_steps -= ticks;
    if (glow_ptr->num_steps == 0) {
        glow_ptr->is_active = FALSE;
    }

    glow_ptr->val += glow_ptr->rate * ticks;
}

void Multisquares_UpdateGlows(Squares *squares_ptr, u32 ticks) {
    Square *square_ptr = squares_ptr->arr;
    u32 i;

    for (i = 0; i < 44; i++, square_ptr++) {
        if (square_ptr->type != SQUARETYPE_NONE) {
            MultisquareGlow_8006af00_eightliner(&square_ptr->glow, ticks);
        }
    }
}

/* Exact floor(x / 255) for the 0..65535 range used by 8-bit blends.
 * Replaces a costly integer divide with shifts/adds. */
static inline u32 TNT_Vita_Div255Floor(u32 x) {
    return (x + 1U + (x >> 8)) >> 8;
}

/* Exact round(x / 255) for the weighted 8-bit blend numerators here. */
static inline u32 TNT_Vita_Div255Round(u32 x) {
    return TNT_Vita_Div255Floor(x + 127U);
}

static void TNT_Vita_BlendWhitePixel(uint32_t *dst, u8 alpha) {
    u32 p;
    u32 r;
    u32 g;
    u32 b;
    if (alpha == 0) {
        return;
    }
    if (alpha == 0xFF) {
        *dst = 0xFFFFFFFFu;
        return;
    }

    p = *dst;
    r = p & 0xFF;
    g = (p >> 8) & 0xFF;
    b = (p >> 16) & 0xFF;

    /* White-over-destination is c + round((255-c)*a/255). This is
     * pixel-identical to the old weighted expression but avoids three divides. */
    r += TNT_Vita_Div255Round((0xFFU - r) * alpha);
    g += TNT_Vita_Div255Round((0xFFU - g) * alpha);
    b += TNT_Vita_Div255Round((0xFFU - b) * alpha);

    *dst = 0xFF000000u | (b << 16) | (g << 8) | r;
}

void TNT_Vita_RenderMultisquareGlows(
    void *buffer,
    s32 stride,
    s32 screen_width,
    s32 screen_height,
    s32 board_x,
    s32 board_y,
    s32 cell,
    Squares *squares_ptr
) {
    PointAngle board_origin_pa;
    Point board_origin;
    Square *square_ptr;
    u32 i;

    /* Same source point used by multisquare creation at board (0,0). */
    setPointAngle(board_origin_pa, -0x80, -0x80, 0);
    Minos_80071100_fiveliner_nuts2(&board_origin, &board_origin_pa);
    board_origin.x >>= 2;
    board_origin.y >>= 2;

    square_ptr = squares_ptr->arr;
    for (i = 0; i < 44; i++, square_ptr++) {
        SquareGlow *glow_ptr;
        s32 unit;
        s32 pulse_alpha;
        s32 dst_x;
        s32 dst_y;
        s32 dst_w;
        s32 dst_h;
        s32 x_src[128];
        u8 alpha_lut[16];
        s32 dy;

        if (square_ptr->type == SQUARETYPE_NONE) {
            continue;
        }

        glow_ptr = &square_ptr->glow;
        if (!glow_ptr->is_active) {
            continue;
        }

        /* Original: (sins(val) * 0xC4) >> 15. Our sine LUT is Q14. */
        pulse_alpha =
            (FUN_026900_800608f4_oneliner_shift6_1(glow_ptr->val) * 0xC4) >> 14;
        if (glow_ptr->alpha != 0xFF) {
            pulse_alpha = (glow_ptr->alpha * pulse_alpha) >> 8;
        }
        if (pulse_alpha <= 0) {
            continue;
        }
        if (pulse_alpha > 0xFF) {
            pulse_alpha = 0xFF;
        }

        unit = TNT_Vita_GlowUnitSize(glow_ptr);
        dst_x = board_x +
            ((glow_ptr->location.x - board_origin.x) * cell) / unit;
        dst_y = board_y +
            ((glow_ptr->location.y - board_origin.y) * cell) / unit;
        dst_w = (66 * cell) / unit;
        dst_h = (66 * cell) / unit;
        if (dst_w <= 0 || dst_h <= 0 || dst_w > 128)
            continue;

        /* Geometry is fixed for a glow during this draw. Precompute the
         * nearest-source x coordinate once instead of dividing per pixel.
         * IA8 alpha has only 16 possible values, so scale those 16 once too. */
        for (s32 mx = 0; mx < dst_w; mx++) {
            s32 sx = (mx * 66) / dst_w;
            x_src[mx] = sx > 65 ? 65 : sx;
        }
        for (s32 a4 = 0; a4 < 16; a4++)
            alpha_lut[a4] = (u8)TNT_Vita_Div255Floor(
                (u32)(a4 * 0x11) * (u32)pulse_alpha);

        for (dy = 0; dy < dst_h; dy++) {
            s32 py = dst_y + dy;
            s32 sy;
            s32 dx;

            if (py < 0 || py >= screen_height) {
                continue;
            }

            sy = (dy * 66) / dst_h;
            if (sy > 65) sy = 65;

            for (dx = 0; dx < dst_w; dx++) {
                s32 px = dst_x + dx;
                s32 sx;
                u8 ia;
                u8 alpha;

                if (px < 0 || px >= screen_width) {
                    continue;
                }

                sx = x_src[dx];
                ia = g_tnt_square_glow_ia8[sy * 66 + sx];
                alpha = alpha_lut[ia & 0x0F];
                if (alpha == 0)
                    continue;

                TNT_Vita_BlendWhitePixel(
                    &((uint32_t *)buffer)[py * stride + px], alpha);
            }
        }
    }
}
