#ifndef TNT_GREEK_GAMEPLAY_VITA_H
#define TNT_GREEK_GAMEPLAY_VITA_H

#include "tnt_wonder_vita.h"

extern f32 g_tnt_greek_skybox_nodes[2][9];
extern TNTWonderTriangle g_tnt_greek_skybox_triangles[64];
extern u16 g_tnt_greek_skybox_textures[8][1360];
extern f32 g_tnt_greek_rain_nodes[8][9];
extern TNTWonderTriangle g_tnt_greek_rain_triangles[23];
extern u16 g_tnt_greek_rain_texture[1360];
extern u8 g_tnt_greek_rain_anim_1319[3024];
extern u8 g_tnt_greek_background_ci8[400 * 300];
extern u16 g_tnt_greek_background_rgba16[256];
extern u8 g_tnt_greek_fire_ci8[14][103 * 71];
extern u16 g_tnt_greek_fire_rgba16[14][256];

void TNT_Vita_GreekRainBuildPose(u32 ticks, f32 out_nodes[8][9], u8 *material_alpha);

#endif
