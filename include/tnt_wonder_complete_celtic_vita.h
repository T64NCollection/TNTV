#ifndef TNT_WONDER_COMPLETE_CELTIC_VITA_H
#define TNT_WONDER_COMPLETE_CELTIC_VITA_H

#include "tnt_wonder_complete_vita.h"

#define TNT_CELTIC_COMPLETE_NODE_COUNT 49
#define TNT_CELTIC_COMPLETE_MESH_COUNT 37
#define TNT_CELTIC_COMPLETE_TEXTURE_COUNT 55
#define TNT_CELTIC_COMPLETE_TRIANGLE_COUNT 2200
#define TNT_CELTIC_COMPLETE_TEXEL_COUNT 48640
#define TNT_CELTIC_COMPLETE_UNTEXTURED_TRIANGLE_COUNT 48
#define TNT_CELTIC_COMPLETE_DYNAMIC_TEXTURE_BASE 252
#define TNT_CELTIC_COMPLETE_EFFECT_GROUP_MASK 0x07
#define TNT_CELTIC_COMPLETE_NO_DEPTH 0x08

extern f32 g_tnt_celtic_complete_nodes[TNT_CELTIC_COMPLETE_NODE_COUNT][12];
extern u8 g_tnt_celtic_complete_node_type[TNT_CELTIC_COMPLETE_NODE_COUNT];
extern s8 g_tnt_celtic_complete_parents[TNT_CELTIC_COMPLETE_NODE_COUNT];
extern u8 g_tnt_celtic_complete_node_mesh[TNT_CELTIC_COMPLETE_NODE_COUNT];
extern f32 g_tnt_celtic_complete_mesh_scale[TNT_CELTIC_COMPLETE_MESH_COUNT];
extern TNTWonderCompleteTriangle g_tnt_celtic_complete_triangles[TNT_CELTIC_COMPLETE_TRIANGLE_COUNT];
extern TNTWonderCompleteTexture g_tnt_celtic_complete_textures[TNT_CELTIC_COMPLETE_TEXTURE_COUNT];
extern u16 g_tnt_celtic_complete_texels[TNT_CELTIC_COMPLETE_TEXEL_COUNT];
extern u32 g_tnt_celtic_complete_untextured_colors[TNT_CELTIC_COMPLETE_UNTEXTURED_TRIANGLE_COUNT][3];
extern u8 g_tnt_celtic_complete_anim_471[13796];
extern u8 g_tnt_celtic_complete_anim_472[4588];
extern u8 g_tnt_celtic_complete_anim_473[5584];

void TNT_Vita_CelticWonderCompleteCamera(u8 mode,u32 ticks,f32 *fovy,f32 *near_plane,f32 *far_plane,f32 *aspect);
void TNT_Vita_CelticWonderCompleteBuildPose(u8 mode,u32 ticks,f32 out_nodes[TNT_CELTIC_COMPLETE_NODE_COUNT][12]);
void TNT_Vita_CelticWonderCompleteEffectState(u8 mode,u32 ticks,f32 out_material[6][4],u8 out_texture[3]);

#endif
