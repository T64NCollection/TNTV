#ifndef TNT_WONDER_COMPLETE_GREEK_VITA_H
#define TNT_WONDER_COMPLETE_GREEK_VITA_H

#include "tnt_wonder_complete_vita.h"

#define TNT_GREEK_COMPLETE_NODE_COUNT 46
#define TNT_GREEK_COMPLETE_MESH_COUNT 36
#define TNT_GREEK_COMPLETE_TEXTURE_COUNT 76
#define TNT_GREEK_COMPLETE_TRIANGLE_COUNT 3646
#define TNT_GREEK_COMPLETE_TEXEL_COUNT 75520

extern f32 g_tnt_greek_complete_nodes[TNT_GREEK_COMPLETE_NODE_COUNT][12];
extern u8 g_tnt_greek_complete_node_type[TNT_GREEK_COMPLETE_NODE_COUNT];
extern s8 g_tnt_greek_complete_parents[TNT_GREEK_COMPLETE_NODE_COUNT];
extern u8 g_tnt_greek_complete_node_mesh[TNT_GREEK_COMPLETE_NODE_COUNT];
extern f32 g_tnt_greek_complete_mesh_scale[TNT_GREEK_COMPLETE_MESH_COUNT];
extern TNTWonderCompleteTriangle g_tnt_greek_complete_triangles[TNT_GREEK_COMPLETE_TRIANGLE_COUNT];
extern TNTWonderCompleteTexture g_tnt_greek_complete_textures[TNT_GREEK_COMPLETE_TEXTURE_COUNT];
extern u16 g_tnt_greek_complete_texels[TNT_GREEK_COMPLETE_TEXEL_COUNT];
extern u8 g_tnt_greek_complete_anim_315[8300];
extern u8 g_tnt_greek_complete_anim_316[5996];
extern u8 g_tnt_greek_complete_anim_317[3980];

void TNT_Vita_GreekWonderCompleteBuildPose(u8 mode,u32 ticks,f32 out_nodes[TNT_GREEK_COMPLETE_NODE_COUNT][12]);
void TNT_Vita_GreekWonderCompleteCamera(u8 mode,u32 ticks,f32 *fovy,f32 *near_plane,f32 *far_plane,f32 *aspect);

#endif
