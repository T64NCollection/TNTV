#ifndef TNT_GAMEFINISH_VITA_H
#define TNT_GAMEFINISH_VITA_H

#include "vita_compat.h"

struct UnkStruct_20;

extern u16 g_tnt_finish_win_rgba16[56 * 20];
extern u16 g_tnt_finish_lose_rgba16[58 * 18];
extern u16 g_tnt_finish_game_over_rgba16[70 * 40];

void TNT_Vita_GameFinish_Render(
    void *buffer,
    int display_width,
    int display_height,
    int display_stride,
    int board_x,
    int board_y,
    int cell,
    const struct UnkStruct_20 *game_finish);

#endif
