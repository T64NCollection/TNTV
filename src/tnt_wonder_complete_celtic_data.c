#include "tnt_wonder_complete_celtic_vita.h"

/* Canonical Celtic completed-Wonder scene 470 from USA 1.0 FX. */
f32 g_tnt_celtic_complete_nodes[TNT_CELTIC_COMPLETE_NODE_COUNT][12]; /* Initialized from verified player ROM. */
u8 g_tnt_celtic_complete_node_type[TNT_CELTIC_COMPLETE_NODE_COUNT]; /* Initialized from verified player ROM. */
s8 g_tnt_celtic_complete_parents[TNT_CELTIC_COMPLETE_NODE_COUNT]; /* Initialized from verified player ROM. */
u8 g_tnt_celtic_complete_node_mesh[TNT_CELTIC_COMPLETE_NODE_COUNT]; /* Initialized from verified player ROM. */
f32 g_tnt_celtic_complete_mesh_scale[TNT_CELTIC_COMPLETE_MESH_COUNT]; /* Initialized from verified player ROM. */
TNTWonderCompleteTexture g_tnt_celtic_complete_textures[TNT_CELTIC_COMPLETE_TEXTURE_COUNT]; /* Initialized from verified player ROM. */
u16 g_tnt_celtic_complete_texels[TNT_CELTIC_COMPLETE_TEXEL_COUNT]; /* original Wonder texels from user ROM */
TNTWonderCompleteTriangle g_tnt_celtic_complete_triangles[TNT_CELTIC_COMPLETE_TRIANGLE_COUNT]; /* Reconstructed from player ROM. */
u32 g_tnt_celtic_complete_untextured_colors[TNT_CELTIC_COMPLETE_UNTEXTURED_TRIANGLE_COUNT][3]; /* Reconstructed from player ROM. */
