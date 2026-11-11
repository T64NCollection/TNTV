#include "vita_compat.h"
#include "tnt_game_types.h"

/* Source garbage.c restored for the Vita gameplay core.  Normal solo play
 * deliberately leaves g_garbage_ptr NULL; the small NULL guards preserve the
 * old garbage-disabled behavior until One Player OPPONENT creates a Landfill. */

static void tnt_garbage_hole_init(UnkStruct_110 *state, u32 seed) {
    state->unk0 = 12;
    state->unk2 = 0;
    state->seed = seed;
    state->hole = (u8)(seed % 10U);
}

static void tnt_garbage_hole_next(UnkStruct_110 *state) {
    state->seed = FUN_026900_PRNG_2(state->seed);
    state->hole = (u8)(state->seed % 10U);
}

static void tnt_garbage_insert_row(UnkStruct_110 *state) {
    s32 x, y;

    for (y = 0; y < 19; y++)
        for (x = 0; x < 10; x++)
            BoardPieces_SwapCubes(x, y, x, y + 1);

    for (x = 0; x < 10; x++) {
        if (x != state->hole)
            BoardP_8006de40_nineliner(x, 19,
                (u8)(FUN_026900_PRNG_1() % 7U));
    }

    state->unk2++;
    if (state->unk2 == state->unk0)
        tnt_garbage_hole_next(state);
}

void func_80072CC8(Garbage *garbage, u8 player, u8 enabled) {
    if (garbage == NULL || player > 3)
        return;
    garbage->unk1E[player] = enabled;
}

u8 func_80072D24(Garbage *garbage, u8 player) {
    if (garbage == NULL || player > 3)
        return FALSE;
    return garbage->unk1E[player];
}

void Garbage_Init(Garbage *garbage, u32 *seed) {
    s32 i;
    if (garbage == NULL || seed == NULL)
        return;

    garbage->unk1 = 0;
    garbage->unk0 = FALSE;
    garbage->unk1C = 0;
    garbage->unk8 = 0;
    garbage->unkC = 0;
    garbage->unk10 = 5;
    garbage->unk1D = g_currentplayer;
    garbage->unk3 = FALSE;
    garbage->unk4 = FALSE;
    garbage->unk5 = 0;
    garbage->unk6 = 0;
    garbage->unk2 = 0;

    tnt_garbage_hole_init(&garbage->unk14, *seed);
    *seed = FUN_026900_PRNG_2(*seed);
    for (i = 0; i < 4; i++)
        func_80072CC8(garbage, (u8)i, TRUE);
}

void Garbage_Deinit(Garbage *garbage) {
    if (garbage != NULL)
        garbage->unk1 = 0;
}

void Garbage_80072e2c_set_arg0p_to_arg1(Garbage *garbage, u8 enabled) {
    if (garbage != NULL)
        garbage->unk0 = enabled;
}

void func_80072E34(Garbage *garbage, s32 player) {
    if (garbage != NULL)
        garbage->unk1D = (u8)player;
}

static void tnt_garbage_update_insert(void) {
    Garbage *garbage = g_garbage_ptr;

    if (garbage == NULL)
        return;

    if (garbage->unk3) {
        if (BoardP_8006ddd0_check_for_nonempty_cell_upwards(3)) {
            garbage->unk2 = 2;
            return;
        }
    } else if (BoardP_8006dd64_check_for_nonempty_cell_downwards(0)) {
        garbage->unk4 = TRUE;
        return;
    }

    if (garbage->unk6 == 0) {
        tnt_garbage_insert_row(&garbage->unk14);
        if (garbage->unk1C > 0)
            garbage->unk1C--;
        garbage->unk5++;
        garbage->unk6 = 4;
        if (garbage->unk1C == 0 || garbage->unk5 >= garbage->unk10)
            garbage->unk2 = 2;
    } else {
        garbage->unk6--;
    }
}

u8 func_80072F1C(s32 ticks) {
    Garbage *garbage = g_garbage_ptr;
    (void)ticks;

    /* This is the source's ordinary LANDFILLTYPE_NONE outcome and preserves
     * the approved one-board gameplay path before OPPONENT is enabled. */
    if (garbage == NULL)
        return 1;

    garbage->unk2 = 0;
    if (garbage->unk1 == 0 || !garbage->unk0) {
        garbage->unk2 = 1;
        return garbage->unk2;
    }

    if (garbage->unk1 == 1)
        tnt_garbage_update_insert();
    if (garbage->unk4)
        garbage->unk2 = 3;
    return garbage->unk2;
}

void func_80072FC4(void) {
    Garbage *garbage = g_garbage_ptr;
    if (garbage == NULL)
        return;

    if (!garbage->unk0)
        garbage->unk1C = 0;
    if (garbage->unk1C == 0) {
        garbage->unk1 = 0;
        return;
    }
    garbage->unk1 = 1;
    garbage->unk6 = 0;
    garbage->unk5 = 0;
}

static void tnt_garbage_add(Garbage *garbage, u32 lines) {
    if (garbage == NULL)
        return;
    garbage->unk1C = (u8)(garbage->unk1C + lines);
    garbage->unk8 += (s32)lines;
}

void func_80073030(u32 lines) {
    Garbage *garbage = g_garbage_ptr;
    Garbage *target;

    if (garbage == NULL || !garbage->unk0 ||
        garbage->unk1D == 0xFF || lines == 0 ||
        garbage->unk1D == g_currentplayer)
        return;

    if (garbage->unk1C > lines) {
        garbage->unk1C = (u8)(garbage->unk1C - lines);
        lines = 0;
    } else {
        lines -= garbage->unk1C;
        garbage->unk1C = 0;
    }

    if (lines != 0 && g_landfill_ptr != NULL &&
        garbage->unk1D < g_landfill_ptr->garbage_idx) {
        target = g_landfill_ptr->list + garbage->unk1D;
        tnt_garbage_add(target, lines);
    }
}
