#include "tnt_game_types.h"
#include "tnt_gamefinish_vita.h"

#include <stdint.h>

static Color g_tnt_gamefinish_gray = {
    0x800, 0x800, 0x800,
    0xFF0, 0xFF0, 0xFF0
};

typedef struct {
    u16 width;
    u16 height;
    const u16 *pixels;
} TNTFinishImage;

static const TNTFinishImage g_finish_images[3] = {
    { 56, 20, g_tnt_finish_win_rgba16 },
    { 58, 18, g_tnt_finish_lose_rgba16 },
    { 70, 40, g_tnt_finish_game_over_rgba16 },
};

static const TNTFinishImage *tnt_finish_image_for_type(s32 gf_type) {
    if (gf_type < GAMEFINISHTYPE_WIN || gf_type > GAMEFINISHTYPE_GAMEOVER)
        return NULL;
    return &g_finish_images[gf_type];
}

static void gamefinish_update_image_motion(UnkStruct_7 *arg0, u32 ticks) {
    arg0->unk10 += arg0->unk12 * ticks;
    if (arg0->unk10 < 0x500) {
        arg0->unk10 = 0x500;
        arg0->unk12 = 0;
    }

    arg0->unkC += arg0->unkE * ticks;
    arg0->unk14 += arg0->unk16 * ticks;
    if (arg0->unk14 >= 0xFF0) {
        arg0->unk14 = 0xFF0;
        arg0->unk16 = 0;
    }
}

static void gamefinish_set_image(UnkStruct_7 *arg0, s32 gf_type) {
    const TNTFinishImage *image = tnt_finish_image_for_type(gf_type);

    if (image == NULL) {
        debug_print_reason_routine_linenum(
            "invalid type", "gamefinish.c", 127);
        return;
    }

    /* Keep the original void* image field meaningful for state inspection,
     * while using host-endian Vita metadata instead of an N64-endian header.
     */
    arg0->image = (void *)image;
    arg0->image_loc.x =
        arg0->center_of_board.x - (image->width >> 1);
    arg0->image_loc.y =
        arg0->center_of_board.y - (image->height >> 1);

    arg0->unk10 = 0x3C00;
    arg0->unk12 = -arg0->unk10 / 150;
    arg0->unkC = 0;
    arg0->unkE = 0x333;
    arg0->unk14 = 0;
    arg0->unk16 = 0xCC;
}

void gamefinish_80052D48_twoliner(UnkStruct_20 *arg0) {
    arg0->state = 4;
    arg0->unk1 = 1;
}

static void gamefinish_begin_final_hold(UnkStruct_20 *arg0) {
    arg0->state = 3;
    arg0->unk8 = 180;
}

static void gamefinish_begin_row_sweep(UnkStruct_20 *arg0) {
    arg0->state = 2;
    arg0->unk4 = 19;
    arg0->unk8 = 3;
}

static void gamefinish_sweep_rows(UnkStruct_20 *arg0) {
    s32 row = arg0->unk4;
    s32 i;

    if (row >= 0) {
        Cell **cells = &g_boardPieces_ptr->cell_ptr_arr[row * 10];
        for (i = 0; i < 10; i++, cells++) {
            if ((*cells)->piece_type != EMPTY_CELL &&
                (*cells)->cubeInfo.cube_ptr != NULL) {
                Minos_80070860_fortyliner(
                    &(*cells)->cubeInfo.cube_ptr->mino,
                    &g_tnt_gamefinish_gray,
                    16);
            }
        }
    }

    row = arg0->unk4 + 5;
    if (row >= 0 && row < 20) {
        Cell **cells = &g_boardPieces_ptr->cell_ptr_arr[row * 10];
        for (i = 0; i < 10; i++, cells++) {
            if ((*cells)->piece_type != EMPTY_CELL &&
                (*cells)->cubeInfo.cube_ptr != NULL) {
                Minos_800709EC(
                    &(*cells)->cubeInfo.cube_ptr->mino,
                    0,
                    16);
                BoardP_8006d5bc_sixliner_calls_UpdateQueue_AddEntry(*cells);
            }
        }
    }

    if (arg0->unk4 < -4) {
        gamefinish_begin_final_hold(arg0);
    } else {
        arg0->unk4--;
    }
}

static void gamefinish_update_row_sweep(UnkStruct_20 *arg0, u32 ticks) {
    arg0->unk8 -= (s32)ticks;
    while (arg0->unk8 <= 0) {
        arg0->unk8 += 3;
        gamefinish_sweep_rows(arg0);
    }
}

static void gamefinish_update_final_hold(UnkStruct_20 *arg0, u32 ticks) {
    /* Original D_800CFEE8 != 12 path deliberately keeps the finish image up
     * until the outer Game state changes screens.  Our direct gameplay harness
     * has no later screen yet, so preserve that hold behavior.
     */
    if (arg0->unk38)
        return;

    if ((u32)arg0->unk8 < ticks) {
        gamefinish_80052D48_twoliner(arg0);
    } else {
        arg0->unk8 -= (s32)ticks;
    }
}

u8 gamefinish_c_line_383(UnkStruct_20 *arg0, u32 ticks) {
    u8 alpha;

    if (!arg0->unk18)
        return 2;

    arg0->unk1 = 0;
    FUN_026900_80060770_sevenliner(&arg0->unkC, (f32)ticks);
    alpha = (u8)arg0->unkC.alpha;

    g_currentPiece_ptr->alpha = alpha;
    g_nextPieces_ptr->alpha = alpha;
    g_tetris_ptr->pieceHold.buf[0].piece.alpha = alpha;
    g_tetris_ptr->pieceHold.buf[0].shadow.alpha = alpha;
    g_tetris_ptr->pieceHold.buf[1].piece.alpha = alpha;
    g_tetris_ptr->pieceHold.buf[1].shadow.alpha = alpha;
    FallingCubes_SetAlpha(g_fallingCubes_ptr, alpha);

    switch (arg0->state) {
    case 1:
        arg0->unk1 = 0;
        break;
    case 2:
        gamefinish_update_row_sweep(arg0, ticks);
        break;
    case 3:
        gamefinish_update_final_hold(arg0, ticks);
        break;
    case 4:
        arg0->unk1 = 1;
        break;
    default:
        debug_print_reason_routine_linenum(
            "invalid state", "gamefinish.c", 383);
        break;
    }

    gamefinish_update_image_motion(&arg0->unk20, ticks);
    return arg0->unk1;
}

/* N64 FrameAct_Render routes through a Gfx display list.  Vita draws the same
 * state from TNT_Vita_GameFinish_Render after the other gameplay layers.
 */
void gamefinish_80053280_twoliner(UnkStruct_20 *arg0) {
    (void)arg0;
}

void gamefinish_800532DC_nevercalled(UnkStruct_20 *arg0) {
    arg0->unk38 = FALSE;
}

void gamefinish_something2(UnkStruct_20 *arg0, s32 gf_type, u8 hold) {
    gamefinish_set_image(&arg0->unk20, gf_type);
    arg0->unkC.alpha = 255.0f;
    arg0->unkC.unk8 = 0.0f;
    arg0->unkC.unk4 = (0.0f - arg0->unkC.alpha) / 32.0f;
    arg0->gf_type = gf_type;
    arg0->unk18 = TRUE;
    arg0->unk38 = hold;
    gamefinish_begin_row_sweep(arg0);
}

void gamefinish_80053390_fiveliner(
    UnkStruct_20 *arg0, BoardInfo *boardInfo_ptr) {
    Point center;

    center.x = boardInfo_ptr->location.x + boardInfo_ptr->unitSize * 5;
    center.y = boardInfo_ptr->location.y + boardInfo_ptr->unitSize * 10;
    arg0->unk20.center_of_board = center;
    arg0->unk18 = FALSE;
    arg0->state = 5;
    arg0->gf_type = GAMEFINISHTYPE_NULL;
}

void gamefinish_80053400_fiveliner(UnkStruct_20 *arg0) {
    arg0->unk18 = FALSE;
    arg0->state = 5;
    arg0->gf_type = GAMEFINISHTYPE_NULL;
}

static int tnt_floor_div_s64_local(s64 n, s64 d) {
    if (n >= 0)
        return (int)(n / d);
    return -(int)(((-n) + d - 1) / d);
}

static uint32_t tnt_finish_blend(
    uint32_t dst, u8 r, u8 g, u8 b, u8 alpha) {
    uint32_t inv = 255U - alpha;
    uint32_t dr = dst & 0xFFU;
    uint32_t dg = (dst >> 8) & 0xFFU;
    uint32_t db = (dst >> 16) & 0xFFU;
    uint32_t rr = (r * alpha + dr * inv + 127U) / 255U;
    uint32_t gg = (g * alpha + dg * inv + 127U) / 255U;
    uint32_t bb = (b * alpha + db * inv + 127U) / 255U;
    return 0xFF000000U | (bb << 16) | (gg << 8) | rr;
}

void TNT_Vita_GameFinish_Render(
    void *buffer,
    int display_width,
    int display_height,
    int display_stride,
    int board_x,
    int board_y,
    int cell,
    const UnkStruct_20 *game_finish) {
    const UnkStruct_7 *state;
    const TNTFinishImage *image;
    uint32_t *dst;
    s32 cos_val;
    int native_x;
    int native_y;
    int global_alpha;
    int px0_cache[70];
    int px1_cache[70];
    int sx;
    int sy;

    if (game_finish == NULL || !game_finish->unk18 || game_finish->state == 4)
        return;

    image = tnt_finish_image_for_type(game_finish->gf_type);
    if (image == NULL)
        return;

    state = &game_finish->unk20;
    cos_val = FUN_026900_80060914_oneliner_shift6_2(state->unkC);
    native_x = state->image_loc.x;
    native_y = state->image_loc.y -
        ((cos_val * state->unk10) >> 22);
    global_alpha = state->unk14 >> 4;
    if (global_alpha < 0) global_alpha = 0;
    if (global_alpha > 255) global_alpha = 255;
    if (global_alpha == 0)
        return;

    dst = (uint32_t *)buffer;

    /* native_x is fixed for a given finish image. Compute its scaled column
     * spans once per frame instead of repeating the same 64-bit divisions for
     * every source row. This is pixel-identical to the original Vita mapping. */
    for (sx = 0; sx < image->width; sx++) {
        int px0 = board_x + tnt_floor_div_s64_local(
            (s64)(native_x + sx - 83) * cell, 11);
        int px1 = board_x + tnt_floor_div_s64_local(
            (s64)(native_x + sx + 1 - 83) * cell + 10, 11);
        if (px0 < 0) px0 = 0;
        if (px1 > display_width) px1 = display_width;
        px0_cache[sx] = px0;
        px1_cache[sx] = px1;
    }

    for (sy = 0; sy < image->height; sy++) {
        int py0 = board_y + tnt_floor_div_s64_local(
            (s64)(native_y + sy - 36) * cell, 11);
        int py1 = board_y + tnt_floor_div_s64_local(
            (s64)(native_y + sy + 1 - 36) * cell + 10, 11);

        if (py1 <= 0 || py0 >= display_height)
            continue;
        if (py0 < 0) py0 = 0;
        if (py1 > display_height) py1 = display_height;

        for (sx = 0; sx < image->width; sx++) {
            u16 rgba = image->pixels[sy * image->width + sx];
            int px0;
            int px1;
            u8 r5, g5, b5;
            u8 r, g, b;
            int py;

            if ((rgba & 1U) == 0)
                continue;

            px0 = px0_cache[sx];
            px1 = px1_cache[sx];
            if (px1 <= 0 || px0 >= display_width)
                continue;

            r5 = (u8)((rgba >> 11) & 0x1F);
            g5 = (u8)((rgba >> 6) & 0x1F);
            b5 = (u8)((rgba >> 1) & 0x1F);
            r = (u8)((r5 << 3) | (r5 >> 2));
            g = (u8)((g5 << 3) | (g5 >> 2));
            b = (u8)((b5 << 3) | (b5 >> 2));

            for (py = py0; py < py1; py++) {
                int px;
                for (px = px0; px < px1; px++) {
                    uint32_t *pixel = &dst[py * display_stride + px];
                    if (global_alpha == 0xFF) {
                        *pixel = 0xFF000000U |
                            ((uint32_t)b << 16) |
                            ((uint32_t)g << 8) | r;
                    } else {
                        *pixel = tnt_finish_blend(
                            *pixel, r, g, b, (u8)global_alpha);
                    }
                }
            }
        }
    }
}
