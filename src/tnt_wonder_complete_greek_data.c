#include "tnt_wonder_complete_greek_vita.h"

/* Canonical Greek completed-Wonder scene 314 from USA 1.0 FX. */
f32 g_tnt_greek_complete_nodes[TNT_GREEK_COMPLETE_NODE_COUNT][12]; /* Initialized from verified player ROM. */
u8 g_tnt_greek_complete_node_type[TNT_GREEK_COMPLETE_NODE_COUNT]; /* Initialized from verified player ROM. */
s8 g_tnt_greek_complete_parents[TNT_GREEK_COMPLETE_NODE_COUNT]; /* Initialized from verified player ROM. */
u8 g_tnt_greek_complete_node_mesh[TNT_GREEK_COMPLETE_NODE_COUNT]; /* Initialized from verified player ROM. */
f32 g_tnt_greek_complete_mesh_scale[TNT_GREEK_COMPLETE_MESH_COUNT]; /* Initialized from verified player ROM. */
TNTWonderCompleteTexture g_tnt_greek_complete_textures[TNT_GREEK_COMPLETE_TEXTURE_COUNT]; /* Initialized from verified player ROM. */
u16 g_tnt_greek_complete_texels[TNT_GREEK_COMPLETE_TEXEL_COUNT]; /* original Wonder texels from user ROM */
TNTWonderCompleteTriangle g_tnt_greek_complete_triangles[TNT_GREEK_COMPLETE_TRIANGLE_COUNT]; /* Reconstructed from player ROM. */
