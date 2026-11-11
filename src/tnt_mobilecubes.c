#include "vita_compat.h"
#include "tnt_game_types.h"
#include <stdlib.h>
#include <string.h>

MobileCubes *g_mobileCubes_ptr;

static void MobileCubes_Move(Cube *cube_ptr) {
    s16 delta;

    cube_ptr->unk26 = FALSE;

#define MOVE_COMPONENT(current, target) \
    do { \
        delta = (current) - (target); \
        if ((delta >= -0x40) && (delta < 0x41)) { \
            (current) = (target); \
        } else { \
            cube_ptr->unk26 = TRUE; \
            if (delta >= 0x41) (current) -= 0x40; \
            if (delta < -0x40) (current) += 0x40; \
        } \
    } while (0)

    MOVE_COMPONENT(cube_ptr->unk2.unk0.p.x, cube_ptr->unk14.unk0.p.x);
    MOVE_COMPONENT(cube_ptr->unk2.unk6.p.x, cube_ptr->unk14.unk6.p.x);
    MOVE_COMPONENT(cube_ptr->unk2.unk0.p.y, cube_ptr->unk14.unk0.p.y);
    MOVE_COMPONENT(cube_ptr->unk2.unk6.p.y, cube_ptr->unk14.unk6.p.y);

#undef MOVE_COMPONENT
}

void MobileCubes_Update(void) {
    MobileCubes *mobileCubes_ptr = g_mobileCubes_ptr;
    s32 i;

    if (mobileCubes_ptr == NULL || mobileCubes_ptr->cube_list == NULL) {
        return;
    }

    for (i = 0; i < mobileCubes_ptr->max_allowed; i++) {
        Cube *cube_ptr = &mobileCubes_ptr->cube_list[i];

        if (cube_ptr->is_active) {
            MobileCubes_Move(cube_ptr);
            /* Original MobileCubes_Render advances Mino morph state once
             * per displayed frame. Vita advances one gameplay tick per frame,
             * so keep that same cadence in the update-side compatibility path.
             */
            Minos_80070528_Morph(&cube_ptr->mino);
        }
    }
}

Cube *MobileCubes_Alloc(void) {
    MobileCubes *mobileCubes_ptr = g_mobileCubes_ptr;
    s32 i;

    if (mobileCubes_ptr == NULL || mobileCubes_ptr->cube_list == NULL) {
        return NULL;
    }

    for (i = 0; i < mobileCubes_ptr->max_allowed; i++) {
        Cube *cube_ptr = &mobileCubes_ptr->cube_list[i];

        if (!cube_ptr->is_active) {
            memset(cube_ptr, 0, sizeof(*cube_ptr));
            cube_ptr->is_active = TRUE;
            mobileCubes_ptr->num_active++;
            return cube_ptr;
        }
    }

    return NULL;
}

void MobileCubes_Unalloc(Cube *cube_ptr) {
    MobileCubes *mobileCubes_ptr = g_mobileCubes_ptr;

    if (mobileCubes_ptr == NULL || cube_ptr == NULL || !cube_ptr->is_active) {
        return;
    }

    cube_ptr->is_active = FALSE;
    if (mobileCubes_ptr->num_active > 0) {
        mobileCubes_ptr->num_active--;
    }
}

void MobileCubes_Init(s32 num_cubes, Minos *minos_ptr) {
    MobileCubes *mobileCubes_ptr = g_mobileCubes_ptr;

    if (mobileCubes_ptr == NULL) {
        return;
    }

    memset(mobileCubes_ptr, 0, sizeof(*mobileCubes_ptr));
    mobileCubes_ptr->minos_ptr = minos_ptr;
    mobileCubes_ptr->alpha = 0xFF;
    mobileCubes_ptr->max_allowed = num_cubes;
    mobileCubes_ptr->cube_list = calloc((size_t)num_cubes, sizeof(Cube));
}

void MobileCubes_Deinit(void) {
    MobileCubes *mobileCubes_ptr = g_mobileCubes_ptr;

    if (mobileCubes_ptr == NULL) {
        return;
    }

    free(mobileCubes_ptr->cube_list);
    mobileCubes_ptr->cube_list = NULL;
    mobileCubes_ptr->max_allowed = 0;
    mobileCubes_ptr->num_active = 0;
}
