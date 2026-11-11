#include "tnt_japanese_gameplay_vita.h"

/* Canonical Japanese one-player gameplay assets: sky scene 770,
 * background IMG 13/14, water/sparkle AnimParams art, and butterfly scene 1360. */
f32 g_tnt_japanese_sky_nodes[TNT_JAPANESE_SKY_NODE_COUNT][9]; /* Reconstructed from verified player ROM scene 770. */
u8 g_tnt_japanese_sky_node_type[TNT_JAPANESE_SKY_NODE_COUNT]; /* Reconstructed from verified player ROM scene 770. */
s8 g_tnt_japanese_sky_parents[TNT_JAPANESE_SKY_NODE_COUNT]; /* Reconstructed from verified player ROM scene 770. */
u8 g_tnt_japanese_sky_node_mesh[TNT_JAPANESE_SKY_NODE_COUNT]; /* Reconstructed from verified player ROM scene 770. */
f32 g_tnt_japanese_sky_mesh_scale[TNT_JAPANESE_SKY_MESH_COUNT]; /* Reconstructed from verified player ROM scene 770. */
f32 g_tnt_japanese_butterfly_nodes[TNT_JAPANESE_BUTTERFLY_NODE_COUNT][9]; /* Reconstructed from verified ROM FX scene1360. */
u8 g_tnt_japanese_butterfly_node_type[TNT_JAPANESE_BUTTERFLY_NODE_COUNT]; /* Reconstructed from verified ROM FX scene1360. */
s8 g_tnt_japanese_butterfly_parents[TNT_JAPANESE_BUTTERFLY_NODE_COUNT]; /* Reconstructed from verified ROM FX scene1360. */
u8 g_tnt_japanese_butterfly_node_mesh[TNT_JAPANESE_BUTTERFLY_NODE_COUNT]; /* Reconstructed from verified ROM FX scene1360. */
f32 g_tnt_japanese_butterfly_mesh_scale[TNT_JAPANESE_BUTTERFLY_MESH_COUNT]; /* Reconstructed from verified ROM FX scene1360. */
TNTWonderTriangle g_tnt_japanese_sky_triangles[TNT_JAPANESE_SKY_TRIANGLE_COUNT]; /* Original FX scene770 restored from verified player ROM. */
u16 g_tnt_japanese_sky_textures[TNT_JAPANESE_SKY_TEXTURE_COUNT][1360]; /* Reconstructed from verified player ROM scene 770. */
TNTWonderTriangle g_tnt_japanese_butterfly_triangles[TNT_JAPANESE_BUTTERFLY_TRIANGLE_COUNT]; /* Reconstructed from verified ROM FX scene1360. */
u8 g_tnt_japanese_background_packbits[67534]; /* Original art loaded from user ROM V6 cache on startup. */
const u32 g_tnt_japanese_background_packbits_size = 67534U;
u16 g_tnt_japanese_background_rgba16[256]; /* Original art loaded from user ROM V6 cache on startup. */
u8 g_tnt_japanese_water_ci8[TNT_JAPANESE_WATER_FRAME_COUNT][254*32]; /* Original art loaded from user ROM V6 cache on startup. */
u16 g_tnt_japanese_water_rgba16[TNT_JAPANESE_WATER_FRAME_COUNT][256]; /* Original art loaded from user ROM V6 cache on startup. */
u8 g_tnt_japanese_sparkle_ci8[TNT_JAPANESE_SPARKLE_FRAME_COUNT][30*29]; /* Original art loaded from user ROM V6 cache on startup. */
u16 g_tnt_japanese_sparkle_rgba16[TNT_JAPANESE_SPARKLE_FRAME_COUNT][256]; /* Original art loaded from user ROM V6 cache on startup. */
