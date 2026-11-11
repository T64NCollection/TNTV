#include "vita_compat.h"
#include "tnt_game_types.h"
#include <stdlib.h>

FallingCubes *g_fallingCubes_ptr;

static void FallingCubes_Drop(Cube **cube_ptr_ptr) {
    Cube *cube_ptr = *cube_ptr_ptr;

    cube_ptr->unk2.unk6.p.y += 0x80;
    cube_ptr->unk14.unk6 = cube_ptr->unk2.unk6;

    if (cube_ptr->unk2.unk6.p.y > 0x1600) {
        MobileCubes_Unalloc(cube_ptr);
        *cube_ptr_ptr = NULL;
        g_fallingCubes_ptr->num_active--;
    }
}

void FallingCubes_Update(void) {
    FallingCubes *fallingCubes_ptr = g_fallingCubes_ptr;
    s32 i;

    if (fallingCubes_ptr == NULL ||
        fallingCubes_ptr->cube_ptr_list == NULL) {
        return;
    }

    for (i = 0; i < fallingCubes_ptr->max_allowed; i++) {
        Cube **cube_ptr_ptr = &fallingCubes_ptr->cube_ptr_list[i];

        if (*cube_ptr_ptr != NULL) {
            Minos_800709d8_threeliner_sets_arg0_1c_1e_1d(
                &(*cube_ptr_ptr)->mino,
                fallingCubes_ptr->alpha
            );
            FallingCubes_Drop(cube_ptr_ptr);
        }
    }
}

void FallingCubes_SetAlpha(FallingCubes *fallingCubes_ptr, u8 alpha) {
    s32 i;

    if (fallingCubes_ptr == NULL) {
        return;
    }

    fallingCubes_ptr->alpha = alpha;

    if (fallingCubes_ptr->cube_ptr_list == NULL) {
        return;
    }

    for (i = 0; i < fallingCubes_ptr->max_allowed; i++) {
        Cube *cube_ptr = fallingCubes_ptr->cube_ptr_list[i];

        if (cube_ptr != NULL) {
            Minos_800709d8_threeliner_sets_arg0_1c_1e_1d(
                &cube_ptr->mino,
                alpha
            );
        }
    }
}

Cube **FallingCubes_Alloc(void) {
    FallingCubes *fallingCubes_ptr = g_fallingCubes_ptr;
    s32 i;

    if (fallingCubes_ptr == NULL ||
        fallingCubes_ptr->cube_ptr_list == NULL) {
        return NULL;
    }

    for (i = 0; i < fallingCubes_ptr->max_allowed; i++) {
        if (fallingCubes_ptr->cube_ptr_list[i] == NULL) {
            fallingCubes_ptr->num_active++;
            return &fallingCubes_ptr->cube_ptr_list[i];
        }
    }

    debug_print_reason_routine("FallingCubes_Alloc()", "Failed!");
    return NULL;
}

void FallingCubes_Init(s32 num_cubes) {
    FallingCubes *fallingCubes_ptr = g_fallingCubes_ptr;

    if (fallingCubes_ptr == NULL) {
        return;
    }

    fallingCubes_ptr->alpha = 0xFF;
    fallingCubes_ptr->max_allowed = num_cubes;
    fallingCubes_ptr->num_active = 0;
    fallingCubes_ptr->cube_ptr_list =
        calloc((size_t)num_cubes, sizeof(Cube *));
}

void FallingCubes_Deinit(void) {
    FallingCubes *fallingCubes_ptr = g_fallingCubes_ptr;

    if (fallingCubes_ptr == NULL) {
        return;
    }

    free(fallingCubes_ptr->cube_ptr_list);
    fallingCubes_ptr->cube_ptr_list = NULL;
    fallingCubes_ptr->max_allowed = 0;
    fallingCubes_ptr->num_active = 0;
}
