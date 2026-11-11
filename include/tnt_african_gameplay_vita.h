#ifndef TNT_AFRICAN_GAMEPLAY_VITA_H
#define TNT_AFRICAN_GAMEPLAY_VITA_H
#include "tnt_wonder_vita.h"
#define TNT_AFRICAN_SKY_NODE_COUNT 3
#define TNT_AFRICAN_SKY_MESH_COUNT 2
#define TNT_AFRICAN_SKY_TEXTURE_COUNT 2
#define TNT_AFRICAN_SKY_TRIANGLE_COUNT 26
#define TNT_AFRICAN_SMOKE_FRAME_COUNT 10
extern f32 g_tnt_african_sky_nodes[TNT_AFRICAN_SKY_NODE_COUNT][9];
extern u8 g_tnt_african_sky_node_type[TNT_AFRICAN_SKY_NODE_COUNT];
extern s8 g_tnt_african_sky_parents[TNT_AFRICAN_SKY_NODE_COUNT];
extern u8 g_tnt_african_sky_node_mesh[TNT_AFRICAN_SKY_NODE_COUNT];
extern f32 g_tnt_african_sky_mesh_scale[TNT_AFRICAN_SKY_MESH_COUNT];
extern TNTWonderTriangle g_tnt_african_sky_triangles[TNT_AFRICAN_SKY_TRIANGLE_COUNT];
extern u8 g_tnt_african_sky_triangle_alpha[TNT_AFRICAN_SKY_TRIANGLE_COUNT];
extern u16 g_tnt_african_sky_textures[TNT_AFRICAN_SKY_TEXTURE_COUNT][1360];
extern u8 g_tnt_african_background_packbits[];
extern const u32 g_tnt_african_background_packbits_size;
extern u16 g_tnt_african_background_rgba16[256];
extern u8 g_tnt_african_smoke_ci8[TNT_AFRICAN_SMOKE_FRAME_COUNT][131*106];
extern u16 g_tnt_african_smoke_rgba16[TNT_AFRICAN_SMOKE_FRAME_COUNT][256];
extern u8 g_tnt_african_sky_anim_568[100];
void TNT_Vita_AfricanSkyBuildPose(u32 ticks,f32 out_nodes[TNT_AFRICAN_SKY_NODE_COUNT][9]);
#endif
