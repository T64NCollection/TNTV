#include "tnt_wonder_complete_african_vita.h"

/* Canonical African completed-Wonder scene 557 from USA 1.0 FX. */
f32 g_tnt_african_complete_nodes[TNT_AFRICAN_COMPLETE_NODE_COUNT][12]; /* Initialized from verified player ROM. */
u8 g_tnt_african_complete_node_type[TNT_AFRICAN_COMPLETE_NODE_COUNT]; /* Initialized from verified player ROM. */
s8 g_tnt_african_complete_parents[TNT_AFRICAN_COMPLETE_NODE_COUNT]; /* Initialized from verified player ROM. */
u8 g_tnt_african_complete_node_mesh[TNT_AFRICAN_COMPLETE_NODE_COUNT]; /* Initialized from verified player ROM. */
f32 g_tnt_african_complete_mesh_scale[TNT_AFRICAN_COMPLETE_MESH_COUNT]; /* Initialized from verified player ROM. */
TNTWonderCompleteTexture g_tnt_african_complete_textures[TNT_AFRICAN_COMPLETE_TEXTURE_COUNT]; /* Initialized from verified player ROM. */
u16 g_tnt_african_complete_texels[TNT_AFRICAN_COMPLETE_TEXEL_COUNT]; /* original Wonder texels from user ROM */
TNTWonderCompleteTriangle g_tnt_african_complete_triangles[TNT_AFRICAN_COMPLETE_TRIANGLE_COUNT]; /* Reconstructed from player ROM. */
u32 g_tnt_african_complete_untextured_colors[TNT_AFRICAN_COMPLETE_UNTEXTURED_TRIANGLE_COUNT][3]; /* Reconstructed from player ROM. */
