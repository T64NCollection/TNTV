#ifndef TNT_WONDER_HALL_VITA_H
#define TNT_WONDER_HALL_VITA_H
#include "tnt_wonder_vita.h"
extern f32 g_tnt_wonder_locked_nodes[2][9];
extern TNTWonderTriangle g_tnt_wonder_locked_triangles[236];
extern u16 g_tnt_wonder_locked_texture[1024];
extern f32 g_tnt_wonder_finale_hall_nodes[4][9];
extern s8 g_tnt_wonder_finale_hall_parents[4];
extern f32 g_tnt_wonder_finale_hall_mesh_scale[3];
extern TNTWonderTriangle g_tnt_wonder_finale_hall_triangles[340];
extern u16 g_tnt_wonder_finale_hall_texture[1024];
extern u8 g_tnt_wonder_arrow_left_ci8[50 * 34];
extern u16 g_tnt_wonder_arrow_left_rgba16[256];
extern u8 g_tnt_wonder_arrow_right_ci8[49 * 33];
extern u16 g_tnt_wonder_arrow_right_rgba16[256];
void TNT_Vita_WonderRenderLocked(void *buffer, s32 stride, s32 screen_width, s32 screen_height, s32 board_x, s32 board_y, s32 cell, u16 anim_tick);
void TNT_Vita_WonderRenderFinaleHall(void *buffer, s32 stride, s32 screen_width, s32 screen_height, s32 board_x, s32 board_y, s32 cell, u16 anim_tick);
#endif
