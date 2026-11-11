#include "vita_compat.h"
#include "tnt_game_types.h"
#include <stdlib.h>
#include <string.h>

static void aimed_indicator_init(UnkStruct_111 *indicator, u8 player) {
    s32 i;
    memset(indicator, 0, sizeof(*indicator));
    indicator->unk0 = TRUE;
    for (i = 0; i < 4; i++) indicator->unk1[i] = TRUE;
    indicator->unkA = player;
}

static void aimed_indicator_all(UnkStruct_111 *indicator, u8 enabled) {
    s32 i;
    for (i = 0; i < 4; i++) indicator->unk1[i] = enabled;
    indicator->unk0 = enabled;
}

void func_80073690(UnkStruct_112 *aimed, u8 player, u8 target, u8 enabled) {
    if (aimed == NULL || aimed->list == NULL ||
        player >= aimed->numIndicators || target >= 4) return;
    aimed->list[player].unk1[target] = enabled;
}

void func_800736E0(UnkStruct_112 *aimed, u8 player, u8 enabled) {
    s32 i;
    if (aimed == NULL || aimed->list == NULL || player >= aimed->numIndicators) return;
    for (i = 0; i < aimed->numIndicators; i++)
        aimed->list[i].unk1[player] = enabled;
    aimed_indicator_all(&aimed->list[player], enabled);
}

void func_80073784(UnkStruct_112 *aimed, u8 enabled) {
    s32 i;
    if (aimed == NULL || aimed->list == NULL) return;
    for (i = 0; i < aimed->numIndicators; i++)
        aimed_indicator_all(&aimed->list[i], enabled);
}

void func_80073940(UnkStruct_112 *aimed) {
    static const s16 rgb[4][3] = {
        {0xFF,0,0}, {0,0xFF,0}, {0,0,0xFF}, {0xFF,0xFF,0}
    };
    s32 i;
    if (aimed == NULL) return;
    memset(aimed, 0, sizeof(*aimed));
    aimed->numIndicators = g_playercount < 3 ? g_playercount : 4;
    if (aimed->numIndicators == 0) return;
    aimed->list = calloc(aimed->numIndicators, sizeof(UnkStruct_111));
    if (aimed->list == NULL) { aimed->numIndicators = 0; return; }
    for (i = 0; i < aimed->numIndicators; i++)
        aimed_indicator_init(&aimed->list[i], (u8)i);
    for (i = 0; i < 4; i++) {
        aimed->color[i].red = rgb[i][0];
        aimed->color[i].green = rgb[i][1];
        aimed->color[i].blue = rgb[i][2];
        aimed->color[i].alpha = 0x80;
    }
    /* The source only assigns indicator screen positions for the 4-slot
     * 3/4-player UI. Two-player OPPONENT never renders IMG_INDICATOR. */
    if (aimed->numIndicators == 4) {
        static const s16 pos[4][2]={{61,267},{145,267},{230,267},{314,267}};
        for (i=0;i<4;i++) { aimed->list[i].unk6.x=pos[i][0]; aimed->list[i].unk6.y=pos[i][1]; }
    }
    if (g_playercount == 3)
        func_800736E0(aimed, 3, FALSE);
}

void func_80073AC4(UnkStruct_112 *aimed) {
    if (aimed == NULL) return;
    free(aimed->list);
    aimed->list = NULL;
    aimed->numIndicators = 0;
}
