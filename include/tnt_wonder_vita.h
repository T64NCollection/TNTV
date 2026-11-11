#ifndef TNT_WONDER_VITA_H
#define TNT_WONDER_VITA_H

#include "vita_compat.h"

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 s;
    s16 t;
    s8 nx;
    s8 ny;
    s8 nz;
    s8 pad;
} TNTWonderVertex;

typedef struct {
    u8 texture;
    u8 node;
    TNTWonderVertex v[3];
} TNTWonderTriangle;

extern f32 g_tnt_wonder_mayan_nodes[18][9];
extern u16 g_tnt_wonder_mayan_textures[20][80];
extern TNTWonderTriangle g_tnt_wonder_mayan_triangles[568];
extern u8 g_tnt_wonder_overlay_ci8[397 * 254];
extern u16 g_tnt_wonder_overlay_rgba16[256];
extern u8 g_tnt_wonder_pie_a_ci8[70 * 109];
extern u16 g_tnt_wonder_pie_a_rgba16[256];
extern u8 g_tnt_wonder_pie_b_ci8[70 * 109];
extern u16 g_tnt_wonder_pie_b_rgba16[256];
extern u8 g_tnt_wonder_pie_c_ci8[70 * 109];
extern u16 g_tnt_wonder_pie_c_rgba16[256];
extern u8 g_tnt_wonder_pie_d_ci8[70 * 109];
extern u16 g_tnt_wonder_pie_d_rgba16[256];
extern u8 g_tnt_wonder_pie_e_ci8[70 * 109];
extern u16 g_tnt_wonder_pie_e_rgba16[256];
extern u8 g_tnt_wonder_pie_f_ci8[70 * 109];
extern u16 g_tnt_wonder_pie_f_rgba16[256];
extern u8 g_tnt_wonder_pie_g_ci8[70 * 109];
extern u16 g_tnt_wonder_pie_g_rgba16[256];
extern u8 g_tnt_wonder_pie_h_ci8[70 * 109];
extern u16 g_tnt_wonder_pie_h_rgba16[256];

void TNT_Vita_WonderRenderMayan(
    void *buffer,
    s32 stride,
    s32 screen_width,
    s32 screen_height,
    s32 board_x,
    s32 board_y,
    s32 cell,
    u16 anim_tick
);

#endif
