#include "vita_compat.h"
#include "tnt_game_types.h"
#include "tnt_pause_vita.h"
#include "tnt_audio_vita.h"
#include <stdint.h>
#include "tntv_rom_bridge.h"

extern u8 g_tnt_font_a_ia8[19456];
extern u8 g_tnt_font_a_char_widths[46];
extern u8 g_tnt_font_b_ia8[10304];
extern u8 g_tnt_font_b_char_widths[46];



static int floor_div_s64(s64 n, s64 d) {
    if (n >= 0)
        return (int)(n / d);
    return -(int)(((-n) + d - 1) / d);
}

static uint32_t blend_rgb(
    uint32_t dst, u8 src_r, u8 src_g, u8 src_b, u8 src_a) {
    u32 inv = 255U - src_a;
    u32 dr = dst & 0xFFU;
    u32 dg = (dst >> 8) & 0xFFU;
    u32 db = (dst >> 16) & 0xFFU;
    u32 r = (src_r * src_a + dr * inv + 127U) / 255U;
    u32 g = (src_g * src_a + dg * inv + 127U) / 255U;
    u32 b = (src_b * src_a + db * inv + 127U) / 255U;
    return (uint32_t)(r | (g << 8) | (b << 16) | 0xFF000000U);
}

void TNT_Vita_SetGameplayAlpha(Tetris *tetris_ptr, u8 alpha) {
    u32 i;

    tetris_ptr->board.currentPiece.alpha = alpha;
    tetris_ptr->board.mobileCubes.alpha = alpha;
    tetris_ptr->board.nextPieces.alpha = alpha;
    FallingCubes_SetAlpha(&tetris_ptr->board.fallingCubes, alpha);
    tetris_ptr->pieceHold.buf[0].piece.alpha = alpha;
    tetris_ptr->pieceHold.buf[0].shadow.alpha = alpha;
    tetris_ptr->pieceHold.buf[1].piece.alpha = alpha;
    tetris_ptr->pieceHold.buf[1].shadow.alpha = alpha;

    for (i = 0; i < 44; i++) {
        Square *square_ptr = &tetris_ptr->squares.arr[i];
        if (square_ptr->type != SQUARETYPE_NONE)
            square_ptr->glow.alpha = alpha;
    }
}

static int font_text_width(const u8 *char_widths, const char *text) {
    int width = 0;
    while (*text != '\0') {
        unsigned c = (unsigned char)*text++;
        if (c >= 32 && c < 96)
            width += char_widths[g_tnt_font_c_map[c - 32]];
    }
    return width;
}

static void font_draw_text(
    void *buffer,
    s32 stride,
    s32 screen_width,
    s32 screen_height,
    s32 board_x,
    s32 board_y,
    s32 cell,
    s32 native_x,
    s32 native_y,
    const char *text,
    u8 red,
    u8 green,
    u8 blue,
    u8 global_alpha,
    const u8 *font_ia8,
    const u8 *char_widths,
    s32 font_width,
    s32 font_height
) {
    uint32_t *dst = (uint32_t *)buffer;
    s32 pen_x = native_x;
    u8 r_lut[16];
    u8 g_lut[16];
    u8 b_lut[16];
    u8 a_lut[16];
    s32 level;

    /* IA8 has only sixteen intensity and sixteen alpha levels.  The old hot
     * path repeated the same color/alpha multiplies and /255 divisions for
     * every source texel.  Precompute the exact integer results once per text
     * draw; this changes no rounding or sampled value. */
    for (level = 0; level < 16; level++) {
        u8 v = (u8)(level * 17U);
        r_lut[level] = (u8)(((u32)red * v + 127U) / 255U);
        g_lut[level] = (u8)(((u32)green * v + 127U) / 255U);
        b_lut[level] = (u8)(((u32)blue * v + 127U) / 255U);
        a_lut[level] = (u8)(((u32)v * global_alpha + 127U) / 255U);
    }

    while (*text != '\0') {
        unsigned c = (unsigned char)*text++;
        u8 glyph;
        s32 glyph_w;
        s32 dx0_map[19];
        s32 dx1_map[19];
        s32 gx;
        s32 gy;

        if (c < 32 || c >= 96)
            continue;
        glyph = g_tnt_font_c_map[c - 32];
        glyph_w = char_widths[glyph];

        /* Destination X bounds depend on glyph column, not glyph row.  The
         * previous loop redid these signed 64-bit floor divisions on every
         * row (14x for Font B, 22x for Font A).  Cache the exact bounds once
         * per glyph column and reuse them for every source row. */
        for (gx = 0; gx < glyph_w; gx++) {
            s32 native_px = pen_x + gx;
            s32 dx0 = board_x +
                floor_div_s64((s64)(native_px - 83) * cell, 11);
            s32 dx1 = board_x +
                floor_div_s64((s64)(native_px + 1 - 83) * cell + 10, 11);
            if (dx0 < 0) dx0 = 0;
            if (dx1 > screen_width) dx1 = screen_width;
            dx0_map[gx] = dx0;
            dx1_map[gx] = dx1;
        }

        for (gy = 0; gy < font_height; gy++) {
            s32 native_py = native_y + gy;
            s32 dy0 = board_y + floor_div_s64((s64)(native_py - 36) * cell, 11);
            s32 dy1 = board_y + floor_div_s64((s64)(native_py + 1 - 36) * cell + 10, 11);

            if (dy1 <= 0 || dy0 >= screen_height)
                continue;
            if (dy0 < 0) dy0 = 0;
            if (dy1 > screen_height) dy1 = screen_height;

            for (gx = 0; gx < glyph_w; gx++) {
                u8 ia = font_ia8[
                    (s32)glyph * (font_width * font_height) + gy * font_width + gx
                ];
                u8 alpha = a_lut[ia & 0x0F];
                s32 dx0 = dx0_map[gx];
                s32 dx1 = dx1_map[gx];
                s32 y;

                if (alpha == 0 || dx1 <= 0 || dx0 >= screen_width || dx0 >= dx1)
                    continue;

                for (y = dy0; y < dy1; y++) {
                    s32 x;
                    for (x = dx0; x < dx1; x++) {
                        uint32_t *pixel = &dst[y * stride + x];
                        *pixel = blend_rgb(
                            *pixel,
                            r_lut[ia >> 4],
                            g_lut[ia >> 4],
                            b_lut[ia >> 4],
                            alpha);
                    }
                }
            }
        }

        pen_x += glyph_w;
    }
}

int TNT_Vita_FontA_TextWidth(const char *text) {
    return font_text_width(g_tnt_font_a_char_widths, text);
}

void TNT_Vita_FontA_DrawText(
    void *buffer, s32 stride, s32 screen_width, s32 screen_height,
    s32 board_x, s32 board_y, s32 cell, s32 native_x, s32 native_y,
    const char *text, u8 red, u8 green, u8 blue, u8 global_alpha
) {
    font_draw_text(
        buffer, stride, screen_width, screen_height,
        board_x, board_y, cell, native_x, native_y,
        text, red, green, blue, global_alpha,
        g_tnt_font_a_ia8, g_tnt_font_a_char_widths, 19, 22);
}

int TNT_Vita_FontB_TextWidth(const char *text) {
    return font_text_width(g_tnt_font_b_char_widths, text);
}

void TNT_Vita_FontB_DrawText(
    void *buffer, s32 stride, s32 screen_width, s32 screen_height,
    s32 board_x, s32 board_y, s32 cell, s32 native_x, s32 native_y,
    const char *text, u8 red, u8 green, u8 blue, u8 global_alpha
) {
    font_draw_text(
        buffer, stride, screen_width, screen_height,
        board_x, board_y, cell, native_x, native_y,
        text, red, green, blue, global_alpha,
        g_tnt_font_b_ia8, g_tnt_font_b_char_widths, 16, 14);
}

void TNT_Vita_PauseInit(TNTVitaPauseMenu *menu) {
    menu->active = FALSE;
    menu->selected = 0;
    menu->just_entered = FALSE;
    menu->fade_active = TRUE;
    menu->pulse = 1.0f;
    menu->pulse_rate = -0.03f;

    /* Original Game_Init starts gameplay objects fully transparent and uses
     * the same shared 16-tick alpha interpolator later reused by pause. */
    menu->fade.alpha = 0.0f;
    menu->fade.unk8 = 255.0f;
    menu->fade.unk4 = (255.0f - menu->fade.alpha) / 16.0f;
}

void TNT_Vita_PauseStart(TNTVitaPauseMenu *menu, Tetris *tetris_ptr) {
    if (menu->active || tetris_ptr->frameAct.unkC)
        return;

    menu->active = TRUE;
    menu->selected = 0;
    menu->just_entered = TRUE;
    menu->pulse = 1.0f;
    menu->pulse_rate = -0.03f;

    Tetris_FUN_80054220_fourliner(tetris_ptr, TRUE);

    menu->fade.unk8 = 0.0f;
    menu->fade.unk4 = (0.0f - menu->fade.alpha) / 16.0f;
    menu->fade_active = TRUE;
}

void TNT_Vita_PauseResumeGame(
    TNTVitaPauseMenu *menu,
    Tetris *tetris_ptr,
    PlayerVars *pv_ptr
) {
    menu->active = FALSE;
    Tetris_FUN_80054220_fourliner(tetris_ptr, FALSE);

    /* Original Game_80051618 clears repeat/new-button/controller-queue state
     * before fading the paused game back to full visibility. */
    FUN_800535B8_fiveliner(pv_ptr);
    pv_ptr->unk20 = 0;

    menu->fade.unk8 = 255.0f;
    menu->fade.unk4 = (255.0f - menu->fade.alpha) / 16.0f;
    menu->fade_active = TRUE;
}

s32 TNT_Vita_PauseUpdate(
    TNTVitaPauseMenu *menu,
    Tetris *tetris_ptr,
    PlayerVars *pv_ptr
) {
    if (menu->fade_active) {
        FUN_026900_80060770_sevenliner(&menu->fade, 1.0f);
        if (menu->fade.alpha < 0.0f) menu->fade.alpha = 0.0f;
        if (menu->fade.alpha > 255.0f) menu->fade.alpha = 255.0f;
        TNT_Vita_SetGameplayAlpha(tetris_ptr, (u8)menu->fade.alpha);
        if (menu->fade.unk4 == 0.0f)
            menu->fade_active = FALSE;
    }

    if (!menu->active)
        return TNT_VITA_PAUSE_ACTION_NONE;

    if (menu->just_entered) {
        menu->just_entered = FALSE;
        return TNT_VITA_PAUSE_ACTION_NONE;
    }

    menu->pulse += menu->pulse_rate;
    if (menu->pulse >= 1.0f) {
        menu->pulse = 1.0f;
        menu->pulse_rate *= -1.0f;
    }
    if (menu->pulse <= 0.5f) {
        menu->pulse = 0.5f;
        menu->pulse_rate *= -1.0f;
    }

    if (pv_ptr->unk24 & CONT_UP) {
        menu->selected = (u8)((menu->selected + 2) % 3);
        menu->pulse = 1.0f;
        menu->pulse_rate = -0.03f;
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
    }
    if (pv_ptr->unk24 & CONT_DOWN) {
        menu->selected = (u8)((menu->selected + 1) % 3);
        menu->pulse = 1.0f;
        menu->pulse_rate = -0.03f;
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
    }

    if (pv_ptr->unk24 & CONT_A) {
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
        if (menu->selected == 0) {
            TNT_Vita_PauseResumeGame(menu, tetris_ptr, pv_ptr);
            return TNT_VITA_PAUSE_ACTION_CONTINUE;
        }

        if (menu->selected == 1) {
            /* Original PROGRESS callback tears down only the popup/input state.
             * Tetris remains paused and faded while the Wonder viewer owns the
             * frontend; Game_80051618 runs only when B returns from the viewer. */
            menu->active = FALSE;
            FUN_800535B8_fiveliner(pv_ptr);
            pv_ptr->unk20 = 0;
            return TNT_VITA_PAUSE_ACTION_PROGRESS;
        }

        if (menu->selected == 2) {
            /* Original EXIT callback destroys the pause popup and clears the
             * player input state. The outer state then routes through tally's
             * pause-exit branch, which immediately returns to ONE PLAYER. */
            menu->active = FALSE;
            FUN_800535B8_fiveliner(pv_ptr);
            pv_ptr->unk20 = 0;
            return TNT_VITA_PAUSE_ACTION_EXIT;
        }
    }

    return TNT_VITA_PAUSE_ACTION_NONE;
}

u8 TNT_Vita_PauseIsActive(const TNTVitaPauseMenu *menu) {
    return menu->active;
}

void TNT_Vita_PauseRender(
    void *buffer,
    s32 stride,
    s32 screen_width,
    s32 screen_height,
    s32 board_x,
    s32 board_y,
    s32 cell,
    const TNTVitaPauseMenu *menu
) {
    static const char *labels[3] = { "CONTINUE", "PROGRESS", "EXIT" };
    static const s32 centers_y[3] = { 130, 150, 170 };
    s32 i;

    if (!menu->active)
        return;

    for (i = 0; i < 3; i++) {
        int width = TNT_Vita_FontA_TextWidth(labels[i]);
        u8 red = 0xFF;
        u8 alpha = 0xFF;

        if (i == menu->selected) {
            red = (u8)(menu->pulse * 255.0f);
            alpha = red;
        }

        TNT_Vita_FontA_DrawText(
            buffer, stride, screen_width, screen_height,
            board_x, board_y, cell,
            200 - (width >> 1), centers_y[i] - 22,
            labels[i], red, 0xFF, 0xFF, alpha
        );
    }
}
