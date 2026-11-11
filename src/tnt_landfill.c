#include "vita_compat.h"
#include "tnt_game_types.h"
#include <stdlib.h>
#include <string.h>

void func_80072300(u8 player) {
    if (g_landfill_ptr != NULL && g_landfill_ptr->unk30.unk0 == player)
        g_landfill_ptr->unk30.unk1 = TRUE;
}

u32 func_8007232C(void) {
    u32 count = 0;
    s32 i;
    if (g_landfill_ptr == NULL) return 0;
    for (i = 0; i < g_playercount; i++)
        if (g_landfill_ptr->list[i].unk0) count++;
    /* Original decomp is missing this return; preserve intended behavior. */
    return count;
}

void Landfill_800723a0_calls_garbage_fun(u8 player, u8 target, u8 enabled) {
    if (g_landfill_ptr == NULL || player >= g_landfill_ptr->garbage_idx) return;
    func_80072CC8(g_landfill_ptr->list + player, target, enabled);
    func_80073690(&g_landfill_ptr->unk8, player, target, enabled);
}

void Landfill_8007240c_fiveliner(u8 player, u8 target) {
    Garbage *garbage;
    if (g_landfill_ptr == NULL || player >= g_landfill_ptr->garbage_idx) return;
    garbage = g_landfill_ptr->list + player;
    if (garbage->unk0) {
        func_80072E34(garbage, target);
        if (g_landfill_ptr->unk8.list != NULL && player < g_landfill_ptr->unk8.numIndicators)
            g_landfill_ptr->unk8.list[player].unkA = target;
    }
}

static u8 landfill_target(u8 player) {
    return (g_landfill_ptr != NULL && player < g_landfill_ptr->garbage_idx)
        ? g_landfill_ptr->list[player].unk1D : 0xFF;
}
static u8 landfill_target_enabled(u8 player, u8 target) {
    return (g_landfill_ptr != NULL && player < g_landfill_ptr->garbage_idx)
        ? func_80072D24(g_landfill_ptr->list + player, target) : FALSE;
}

void func_80072508(u8 player) {
    s32 current = landfill_target(player), target;
    if (current <= 0) return;
    target = current;
    do { target--; } while (target >= 0 && !landfill_target_enabled(player,(u8)target));
    if (target >= 0) Landfill_8007240c_fiveliner(player,(u8)target);
}

void func_8007257C(u8 player) {
    s32 max, current, target;
    if (g_landfill_ptr == NULL) return;
    max = g_landfill_ptr->garbage_idx - 1;
    current = landfill_target(player);
    if (current >= max) return;
    target = current;
    do { target++; } while (target <= max && !landfill_target_enabled(player,(u8)target));
    if (target <= max) Landfill_8007240c_fiveliner(player,(u8)target);
}

static void landfill_cycle_target(u8 player) {
    s32 max, current, target;
    if (g_landfill_ptr == NULL) return;
    max = g_landfill_ptr->garbage_idx - 1;
    current = landfill_target(player);
    target = current;
    do {
        target++;
        if (target > max) target = 0;
    } while (!landfill_target_enabled(player,(u8)target) && target != current);
    if (target != current) Landfill_8007240c_fiveliner(player,(u8)target);
}

void func_800726A4(Landfill *landfill, u8 player) {
    s32 i;
    if (landfill == NULL || player >= landfill->garbage_idx) return;
    Garbage_80072e2c_set_arg0p_to_arg1(landfill->list + player, FALSE);
    for (i=0;i<landfill->garbage_idx;i++) func_80072CC8(landfill->list+i,player,FALSE);
    func_800736E0(&landfill->unk8,player,FALSE);
    if (landfill->type == LANDFILLTYPE_DIRECTED) {
        for (i=0;i<landfill->garbage_idx;i++)
            if (landfill->list[i].unk1D == player) landfill_cycle_target((u8)i);
    } else if (landfill->type == LANDFILLTYPE_HOTPOTATO && landfill->unk30.unk0 == player) {
        landfill->unk30.unk1 = TRUE;
    }
}

Garbage *Landfill_Garbage(void) {
    Garbage *garbage;
    if (g_landfill_ptr == NULL || g_landfill_ptr->list == NULL ||
        g_landfill_ptr->garbage_idx >= g_playercount) return NULL;
    garbage = &g_landfill_ptr->list[g_landfill_ptr->garbage_idx++];
    return garbage;
}


void HotPotato_Init(UnkStruct_29 *state) {
    if (state == NULL || g_playercount == 0) return;
    state->unk0 = (u8)(FUN_026900_PRNG_1() % g_playercount);
    state->unk1 = FALSE;
    state->unk2 = 0xFF; state->unk4 = 0; state->unk6 = 0; state->unk8 = 0xFF;
    state->unkA = 0xFF; state->unkC = 0xFF; state->unkE = 0xFF; state->unk10 = 0x20;
    state->unk12 = 0;
}

void HotPotato_Update(UnkStruct_29 *state) {
    u8 old_holder;
    u32 active;
    s32 i;
    if (state == NULL || !state->unk1 || g_landfill_ptr == NULL) return;
    active = func_8007232C();
    if (active <= 1) return;
    old_holder = state->unk0;
    if (active == 2) {
        do {
            state->unk0 = (u8)((state->unk0 + 1U) % g_playercount);
        } while (state->unk0 != old_holder &&
                 !g_landfill_ptr->list[state->unk0].unk0);
    } else {
        u32 steps = (FUN_026900_PRNG_1() % (active - 1U)) + 1U;
        while (steps--) {
            do {
                state->unk0 = (u8)((state->unk0 + 1U) % g_playercount);
            } while (!g_landfill_ptr->list[state->unk0].unk0);
        }
    }
    for (i = 0; i < g_playercount; i++)
        Landfill_8007240c_fiveliner((u8)i, state->unk0);
    state->unk1 = FALSE;
}

void HotPotato_Deinit(UnkStruct_29 *state) { (void)state; }

void Landfill_Init(Landfill *landfill) {
    if (landfill == NULL) return;
    g_landfill_ptr = landfill;
    landfill->garbage_idx = 0;
    landfill->list = calloc(g_playercount ? g_playercount : 1, sizeof(Garbage));
    func_80073940(&landfill->unk8);
    switch (landfill->type) {
    case LANDFILLTYPE_NONE:
        func_80073784(&landfill->unk8, FALSE);
        break;
    case LANDFILLTYPE_HOTPOTATO:
        HotPotato_Init(&landfill->unk30);
        break;
    case LANDFILLTYPE_DIRECTED:
    default:
        break;
    }
}

void Landfill_Deinit(Landfill *landfill) {
    if (landfill == NULL) return;
    func_80073AC4(&landfill->unk8);
    if (landfill->type == LANDFILLTYPE_HOTPOTATO)
        HotPotato_Deinit(&landfill->unk30);
    free(landfill->list);
    landfill->list = NULL;
    landfill->garbage_idx = 0;
    landfill->type = LANDFILLTYPE_NONE;
    if (g_landfill_ptr == landfill) g_landfill_ptr = NULL;
}
