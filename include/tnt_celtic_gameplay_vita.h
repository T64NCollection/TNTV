#ifndef TNT_CELTIC_GAMEPLAY_VITA_H
#define TNT_CELTIC_GAMEPLAY_VITA_H
#include "tnt_wonder_vita.h"
#define TNT_CELTIC_SKY_NODE_COUNT 12
#define TNT_CELTIC_SKY_MESH_COUNT 7
#define TNT_CELTIC_SKY_TEXTURE_COUNT 3
#define TNT_CELTIC_SKY_TRIANGLE_COUNT 160
#define TNT_CELTIC_SKY_UNTEXTURED_TRIANGLE_COUNT 112
#define TNT_CELTIC_LEAF_NODE_COUNT 15
#define TNT_CELTIC_LEAF_MESH_COUNT 13
#define TNT_CELTIC_LEAF_TRIANGLE_COUNT 26
extern f32 g_tnt_celtic_sky_nodes[TNT_CELTIC_SKY_NODE_COUNT][9];
extern u8 g_tnt_celtic_sky_node_type[TNT_CELTIC_SKY_NODE_COUNT];
extern s8 g_tnt_celtic_sky_parents[TNT_CELTIC_SKY_NODE_COUNT];
extern u8 g_tnt_celtic_sky_node_mesh[TNT_CELTIC_SKY_NODE_COUNT];
extern f32 g_tnt_celtic_sky_mesh_scale[TNT_CELTIC_SKY_MESH_COUNT];
extern TNTWonderTriangle g_tnt_celtic_sky_triangles[TNT_CELTIC_SKY_TRIANGLE_COUNT];
extern u8 g_tnt_celtic_sky_alpha_class[TNT_CELTIC_SKY_TRIANGLE_COUNT];
extern TNTWonderTriangle g_tnt_celtic_sky_untextured[TNT_CELTIC_SKY_UNTEXTURED_TRIANGLE_COUNT];
extern u16 g_tnt_celtic_sky_textures[TNT_CELTIC_SKY_TEXTURE_COUNT][1360];
extern f32 g_tnt_celtic_leaf_nodes[TNT_CELTIC_LEAF_NODE_COUNT][9];
extern u8 g_tnt_celtic_leaf_node_type[TNT_CELTIC_LEAF_NODE_COUNT];
extern s8 g_tnt_celtic_leaf_parents[TNT_CELTIC_LEAF_NODE_COUNT];
extern u8 g_tnt_celtic_leaf_node_mesh[TNT_CELTIC_LEAF_NODE_COUNT];
extern f32 g_tnt_celtic_leaf_mesh_scale[TNT_CELTIC_LEAF_MESH_COUNT];
extern TNTWonderTriangle g_tnt_celtic_leaf_triangles[TNT_CELTIC_LEAF_TRIANGLE_COUNT];
extern u8 g_tnt_celtic_background_packbits[];
extern const u32 g_tnt_celtic_background_packbits_size;
extern u16 g_tnt_celtic_background_rgba16[256];
extern u8 g_tnt_celtic_lamp_ci8[4][61*60];
extern u16 g_tnt_celtic_lamp_rgba16[4][256];
extern u8 g_tnt_celtic_sky_anim_488[3532];
extern u8 g_tnt_celtic_leaf_anim_1336[36160];
void TNT_Vita_CelticSkyBuildPose(u32 ticks,f32 out_nodes[TNT_CELTIC_SKY_NODE_COUNT][9],u8 *animated_alpha);
void TNT_Vita_CelticLeavesBuildPose(u32 ticks,f32 out_nodes[TNT_CELTIC_LEAF_NODE_COUNT][9]);
#endif
