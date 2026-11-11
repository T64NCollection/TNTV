#include "tnt_celtic_gameplay_vita.h"

/* Canonical Celtic one-player gameplay assets: sky scene 487/anim 488,
 * leaves scene 1335/anim 1336, background IMG 17/18, and lamp D_800D2008. */
f32 g_tnt_celtic_sky_nodes[TNT_CELTIC_SKY_NODE_COUNT][9]; /* Reconstructed from verified player ROM scene 487. */
u8 g_tnt_celtic_sky_node_type[TNT_CELTIC_SKY_NODE_COUNT]; /* Reconstructed from verified player ROM scene 487. */
s8 g_tnt_celtic_sky_parents[TNT_CELTIC_SKY_NODE_COUNT]; /* Reconstructed from verified player ROM scene 487. */
u8 g_tnt_celtic_sky_node_mesh[TNT_CELTIC_SKY_NODE_COUNT]; /* Reconstructed from verified player ROM scene 487. */
f32 g_tnt_celtic_sky_mesh_scale[TNT_CELTIC_SKY_MESH_COUNT]; /* Reconstructed from verified player ROM scene 487. */
f32 g_tnt_celtic_leaf_nodes[TNT_CELTIC_LEAF_NODE_COUNT][9]; /* Reconstructed from player ROM FX scene 1335. */
u8 g_tnt_celtic_leaf_node_type[TNT_CELTIC_LEAF_NODE_COUNT]; /* Reconstructed from player ROM FX scene 1335. */
s8 g_tnt_celtic_leaf_parents[TNT_CELTIC_LEAF_NODE_COUNT]; /* Reconstructed from player ROM FX scene 1335. */
u8 g_tnt_celtic_leaf_node_mesh[TNT_CELTIC_LEAF_NODE_COUNT]; /* Reconstructed from player ROM FX scene 1335. */
f32 g_tnt_celtic_leaf_mesh_scale[TNT_CELTIC_LEAF_MESH_COUNT]; /* Reconstructed from player ROM FX scene 1335. */
TNTWonderTriangle g_tnt_celtic_sky_triangles[TNT_CELTIC_SKY_TRIANGLE_COUNT]; /* Original scene 487 restored locally from player ROM. */
u8 g_tnt_celtic_sky_alpha_class[TNT_CELTIC_SKY_TRIANGLE_COUNT]; /* Original scene 487 restored locally from player ROM. */
TNTWonderTriangle g_tnt_celtic_sky_untextured[TNT_CELTIC_SKY_UNTEXTURED_TRIANGLE_COUNT]; /* Original scene 487 restored locally from player ROM. */
u16 g_tnt_celtic_sky_textures[TNT_CELTIC_SKY_TEXTURE_COUNT][1360]; /* Reconstructed from verified player ROM scene 487. */
TNTWonderTriangle g_tnt_celtic_leaf_triangles[TNT_CELTIC_LEAF_TRIANGLE_COUNT]; /* Reconstructed from player ROM FX scene 1335. */
u8 g_tnt_celtic_background_packbits[91883]; /* Original art loaded from user ROM V6 cache on startup. */
const u32 g_tnt_celtic_background_packbits_size = 91883U;
u16 g_tnt_celtic_background_rgba16[256]; /* Original art loaded from user ROM V6 cache on startup. */
u8 g_tnt_celtic_lamp_ci8[4][61*60]; /* Original art loaded from user ROM V6 cache on startup. */
u16 g_tnt_celtic_lamp_rgba16[4][256]; /* Original art loaded from user ROM V6 cache on startup. */
