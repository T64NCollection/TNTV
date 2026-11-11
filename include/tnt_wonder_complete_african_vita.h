#ifndef TNT_WONDER_COMPLETE_AFRICAN_VITA_H
#define TNT_WONDER_COMPLETE_AFRICAN_VITA_H

#include "tnt_wonder_complete_vita.h"

#define TNT_AFRICAN_COMPLETE_NODE_COUNT 25
#define TNT_AFRICAN_COMPLETE_MESH_COUNT 16
#define TNT_AFRICAN_COMPLETE_TEXTURE_COUNT 49
#define TNT_AFRICAN_COMPLETE_TRIANGLE_COUNT 3044
#define TNT_AFRICAN_COMPLETE_TEXEL_COUNT 45376
#define TNT_AFRICAN_COMPLETE_UNTEXTURED_TRIANGLE_COUNT 30
#define TNT_AFRICAN_COMPLETE_DYNAMIC_TEXTURE_BASE 252
#define TNT_AFRICAN_COMPLETE_EFFECT_GROUP_MASK 0x07
#define TNT_AFRICAN_COMPLETE_NO_DEPTH 0x08

extern f32 g_tnt_african_complete_nodes[TNT_AFRICAN_COMPLETE_NODE_COUNT][12];
extern u8 g_tnt_african_complete_node_type[TNT_AFRICAN_COMPLETE_NODE_COUNT];
extern s8 g_tnt_african_complete_parents[TNT_AFRICAN_COMPLETE_NODE_COUNT];
extern u8 g_tnt_african_complete_node_mesh[TNT_AFRICAN_COMPLETE_NODE_COUNT];
extern f32 g_tnt_african_complete_mesh_scale[TNT_AFRICAN_COMPLETE_MESH_COUNT];
extern TNTWonderCompleteTriangle g_tnt_african_complete_triangles[TNT_AFRICAN_COMPLETE_TRIANGLE_COUNT];
extern TNTWonderCompleteTexture g_tnt_african_complete_textures[TNT_AFRICAN_COMPLETE_TEXTURE_COUNT];
extern u16 g_tnt_african_complete_texels[TNT_AFRICAN_COMPLETE_TEXEL_COUNT];
extern u32 g_tnt_african_complete_untextured_colors[TNT_AFRICAN_COMPLETE_UNTEXTURED_TRIANGLE_COUNT][3];
extern u8 g_tnt_african_complete_anim_558[6420];
extern u8 g_tnt_african_complete_anim_559[5292];
extern u8 g_tnt_african_complete_anim_560[4140];

void TNT_Vita_AfricanWonderCompleteCamera(u8 mode,u32 ticks,f32 *fovy,f32 *near_plane,f32 *far_plane,f32 *aspect);
void TNT_Vita_AfricanWonderCompleteBuildPose(u8 mode,u32 ticks,f32 out_nodes[TNT_AFRICAN_COMPLETE_NODE_COUNT][12]);
void TNT_Vita_AfricanWonderCompleteEffectState(u8 mode,u32 ticks,f32 out_material[4][4],u8 *out_texture);
void TNT_Vita_AfricanWonderCompleteLightState(u8 mode,u32 ticks,f32 out_light[3][4]);

#endif
