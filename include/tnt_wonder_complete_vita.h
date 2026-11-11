#ifndef TNT_WONDER_COMPLETE_VITA_H
#define TNT_WONDER_COMPLETE_VITA_H

#include "tnt_wonder_vita.h"

typedef struct { u32 offset; u16 width; u16 height; u8 wrap; u8 pad[3]; } TNTWonderCompleteTexture;
typedef struct { u8 texture; u8 node; u8 pad[2]; TNTWonderVertex v[3]; } TNTWonderCompleteTriangle;

extern f32 g_tnt_wonder_complete_nodes[99][12];
extern u8 g_tnt_wonder_complete_node_type[99];
extern s8 g_tnt_wonder_complete_parents[99];
extern u8 g_tnt_wonder_complete_node_mesh[99];
extern f32 g_tnt_wonder_complete_mesh_scale[75];
extern TNTWonderCompleteTriangle g_tnt_wonder_complete_triangles[3320];
extern TNTWonderCompleteTexture g_tnt_wonder_complete_textures[147];
extern u16 g_tnt_wonder_complete_texels[130320];
extern u8 g_tnt_wonder_complete_anim_187[18312];
extern u8 g_tnt_wonder_complete_anim_188[36176];
extern u8 g_tnt_wonder_complete_anim_189[13472];
extern u8 g_tnt_wonder_complete_anim_1396[13384];

enum {
    TNT_WONDER_COMPLETE_ANIM_PRESENTATION = 0,
    TNT_WONDER_COMPLETE_ANIM_FINAL = 1,
    TNT_WONDER_COMPLETE_ANIM_HALL = 2,
    TNT_WONDER_COMPLETE_ANIM_FINALE = 3
};

void TNT_Vita_WonderCompleteBuildPose(
    u8 mode, u32 ticks, f32 out_nodes[99][12]);
void TNT_Vita_MayanWonderCompleteCamera(
    u8 mode, u32 ticks, f32 *fovy, f32 *near_plane, f32 *far_plane, f32 *aspect);

#endif
