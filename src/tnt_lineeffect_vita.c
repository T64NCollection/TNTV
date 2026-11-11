#include "vita_compat.h"
#include "tnt_game_types.h"

/*
 * Vita compatibility implementation of the original LineEffect state machine.
 * Cube/FallingCube ownership and timing are restored here.  The original
 * N64 SFX variation calls are restored alongside the existing state machine.
 */

void LineEffect_80069668_fourliner_sets_arg0_struct_v1(
    LineEffect *lineEffect_ptr
) {
    lineEffect_ptr->type = 0;
    lineEffect_ptr->state = 0;
    lineEffect_ptr->unk2 = TRUE;
    lineEffect_ptr->unk3 = TRUE;
}

void LineEffect_Init(LineEffect *lineEffect_ptr, s32 row, u8 type) {
    s32 col;

    lineEffect_ptr->type = type;

    for (col = 0; col < 10; col++) {
        lineEffect_ptr->cells[col] =
            g_boardPieces_ptr->cell_ptr_arr[row * 10 + col];
    }

    switch (type) {
    case 0:
        lineEffect_ptr->state = 0;
        lineEffect_ptr->unk2 = TRUE;
        lineEffect_ptr->unk3 = TRUE;
        break;

    case 1:
        lineEffect_ptr->state = 2;
        lineEffect_ptr->unk2 = FALSE;
        lineEffect_ptr->unk3 = FALSE;
        lineEffect_ptr->col_step = 0;
        break;

    case 4:
        lineEffect_ptr->state = 4;
        lineEffect_ptr->unk2 = FALSE;
        lineEffect_ptr->unk3 = FALSE;
        lineEffect_ptr->col_step = 0;
        lineEffect_ptr->unk4 = 0;
        break;

    case 2:
    case 3:
        lineEffect_ptr->state = 5;
        lineEffect_ptr->unk2 = FALSE;
        lineEffect_ptr->unk3 = FALSE;
        lineEffect_ptr->unk4 = 0;
        break;

    default:
        lineEffect_ptr->state = 0;
        lineEffect_ptr->unk2 = TRUE;
        lineEffect_ptr->unk3 = TRUE;
        break;
    }
}

static void TNT_LineEffect_NormalPair(LineEffect *lineEffect_ptr) {
    if (lineEffect_ptr->col_step < 5) {
        BoardP_8006d674_tenliner_alloc_fallingcubes(
            lineEffect_ptr->cells[4 - lineEffect_ptr->col_step]);
        BoardP_8006d674_tenliner_alloc_fallingcubes(
            lineEffect_ptr->cells[5 + lineEffect_ptr->col_step]);
        Audio2_Play_SFX(
            &D_801235B0,
            &g_gameSfxBank,
            func_800A35EC(SFX_GAME_9, SFX_GAME_12));

        lineEffect_ptr->col_step++;
        lineEffect_ptr->unk3 = TRUE;
        lineEffect_ptr->unk2 = FALSE;
    } else {
        lineEffect_ptr->state = 3;
        lineEffect_ptr->unk3 = TRUE;
        lineEffect_ptr->unk2 = FALSE;
    }
}

static void TNT_LineEffect_NormalCollapse(LineEffect *lineEffect_ptr) {
    s32 col;

    for (col = 0; col < 10; col++) {
        BoardP_8006e288_adds_8_entries_and_swaps_cubes(
            lineEffect_ptr->cells[col]);
    }

    lineEffect_ptr->unk2 = TRUE;
    lineEffect_ptr->state = 0;
    lineEffect_ptr->unk3 = TRUE;
}

static void TNT_LineEffect_TetrisPair(LineEffect *lineEffect_ptr) {
    Cell *left_cell;
    Cell *right_cell;

    if (lineEffect_ptr->unk4 < 3) {
        lineEffect_ptr->unk4++;
        return;
    }

    lineEffect_ptr->unk4 = 0;

    if (lineEffect_ptr->col_step < 5) {
        left_cell = lineEffect_ptr->cells[4 - lineEffect_ptr->col_step];
        right_cell = lineEffect_ptr->cells[5 + lineEffect_ptr->col_step];

        Audio2_Play_SFX(
            &D_801235B0,
            &g_gameSfxBank,
            func_800A35EC(SFX_GAME_9, SFX_GAME_12));
        Audio2_Play_SFX(&D_801235B0, &g_gameSfxBank, SFX_GAME_8);

        BoardP_8006d674_tenliner_alloc_fallingcubes(left_cell);
        BoardP_8006d674_tenliner_alloc_fallingcubes(right_cell);
        BoardP_8006e288_adds_8_entries_and_swaps_cubes(left_cell);
        BoardP_8006e288_adds_8_entries_and_swaps_cubes(right_cell);

        lineEffect_ptr->col_step++;
        lineEffect_ptr->unk3 = TRUE;
        lineEffect_ptr->unk2 = FALSE;
    } else {
        lineEffect_ptr->unk2 = TRUE;
        lineEffect_ptr->state = 0;
        lineEffect_ptr->unk3 = TRUE;
    }
}

static void TNT_LineEffect_MetalBegin(LineEffect *lineEffect_ptr) {
    s32 col;

    lineEffect_ptr->unk4 = 24;

    BoardP_8006d5bc_sixliner_calls_UpdateQueue_AddEntry(
        lineEffect_ptr->cells[0]);

    for (col = 1; col < 10; col++) {
        BoardP_8006d5bc_sixliner_calls_UpdateQueue_AddEntry(
            lineEffect_ptr->cells[col]);
        BoardP_8006d450_fourteenliner_pf_logic_cell_loops(
            lineEffect_ptr->cells[col],
            lineEffect_ptr->cells[0]);
    }

    for (col = 0; col < 10; col++) {
        Cube *cube_ptr = lineEffect_ptr->cells[col]->cubeInfo.cube_ptr;

        if (cube_ptr != NULL) {
            cube_ptr->mino.unk22 =
                (0xFF - cube_ptr->mino.unk20) / 24;
            Minos_80070860_fortyliner(
                &cube_ptr->mino,
                &g_color_metal,
                24
            );
        }
    }

    lineEffect_ptr->state = 6;
    lineEffect_ptr->unk3 = FALSE;
    lineEffect_ptr->unk2 = FALSE;
}

static void TNT_LineEffect_MetalWait(LineEffect *lineEffect_ptr) {
    if (--lineEffect_ptr->unk4 == 0) {
        lineEffect_ptr->state = 7;
        Audio2_Play_SFX(&D_801235B0, &g_gameSfxBank, SFX_GAME_16);
    }

    lineEffect_ptr->unk3 = TRUE;
    lineEffect_ptr->unk2 = FALSE;
}

static void TNT_LineEffect_MetalCollapse(LineEffect *lineEffect_ptr) {
    s32 col;

    for (col = 0; col < 10; col++) {
        BoardP_8006e288_adds_8_entries_and_swaps_cubes(
            lineEffect_ptr->cells[col]);
    }

    lineEffect_ptr->unk2 = TRUE;
    lineEffect_ptr->state = 0;
    lineEffect_ptr->unk3 = TRUE;
}

u8 LineEffect_Update(LineEffect *lineEffect_ptr, u8 arg1) {
    (void)arg1;

    do {
        switch (lineEffect_ptr->state) {
        case 0:
        case 1:
            lineEffect_ptr->state = 0;
            lineEffect_ptr->unk2 = TRUE;
            lineEffect_ptr->unk3 = TRUE;
            break;

        case 2:
            TNT_LineEffect_NormalPair(lineEffect_ptr);
            break;

        case 3:
            TNT_LineEffect_NormalCollapse(lineEffect_ptr);
            break;

        case 4:
            TNT_LineEffect_TetrisPair(lineEffect_ptr);
            break;

        case 5:
            TNT_LineEffect_MetalBegin(lineEffect_ptr);
            break;

        case 6:
            TNT_LineEffect_MetalWait(lineEffect_ptr);
            break;

        case 7:
            TNT_LineEffect_MetalCollapse(lineEffect_ptr);
            break;

        default:
            lineEffect_ptr->state = 0;
            lineEffect_ptr->unk2 = TRUE;
            lineEffect_ptr->unk3 = TRUE;
            break;
        }
    } while (lineEffect_ptr->unk3 == FALSE);

    return lineEffect_ptr->unk2;
}

/*
 * Kept as a compatibility hook for the flat Vita renderer.  With restored
 * FallingCubes ownership the affected board Cells are now actually empty, so
 * this is normally redundant, but it remains harmless during transition.
 */
u8 TNT_Vita_LineEffectCellHidden(LineScan *lineScan_ptr, Cell *cell_ptr) {
    s32 row;

    for (row = 0; row < 20; row++) {
        LineEffect *effect = &lineScan_ptr->unk2C[row];
        s32 hidden_pairs = 0;
        s32 pair;

        if (effect->type == 1 &&
            (effect->state == 2 || effect->state == 3)) {
            hidden_pairs = effect->state == 3 ? 5 : effect->col_step;
        } else if (effect->type == 4 && effect->state == 4) {
            hidden_pairs = effect->col_step;
        } else {
            continue;
        }

        for (pair = 0; pair < hidden_pairs; pair++) {
            if (cell_ptr == effect->cells[4 - pair] ||
                cell_ptr == effect->cells[5 + pair]) {
                return TRUE;
            }
        }
    }

    return FALSE;
}
