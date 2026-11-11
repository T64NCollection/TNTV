#include "tnt_wonder_complete_japanese_vita.h"

/* Canonical Japanese completed-Wonder scene 755 from USA 1.0 FX. */
f32 g_tnt_japanese_complete_nodes[TNT_JAPANESE_COMPLETE_NODE_COUNT][12]; /* Initialized from verified player ROM. */
u8 g_tnt_japanese_complete_node_type[TNT_JAPANESE_COMPLETE_NODE_COUNT]; /* Initialized from verified player ROM. */
s8 g_tnt_japanese_complete_parents[TNT_JAPANESE_COMPLETE_NODE_COUNT]; /* Initialized from verified player ROM. */
u8 g_tnt_japanese_complete_node_mesh[TNT_JAPANESE_COMPLETE_NODE_COUNT]; /* Initialized from verified player ROM. */
f32 g_tnt_japanese_complete_mesh_scale[TNT_JAPANESE_COMPLETE_MESH_COUNT]; /* Initialized from verified player ROM. */
TNTWonderCompleteTexture g_tnt_japanese_complete_textures[TNT_JAPANESE_COMPLETE_TEXTURE_COUNT]; /* Initialized from verified player ROM. */
u16 g_tnt_japanese_complete_texels[TNT_JAPANESE_COMPLETE_TEXEL_COUNT]; /* original Wonder texels from user ROM */
TNTWonderCompleteTriangle g_tnt_japanese_complete_triangles[TNT_JAPANESE_COMPLETE_TRIANGLE_COUNT]; /* Reconstructed from player ROM. */
u32 g_tnt_japanese_complete_untextured_colors[TNT_JAPANESE_COMPLETE_UNTEXTURED_TRIANGLE_COUNT][3]; /* Reconstructed from player ROM. */
