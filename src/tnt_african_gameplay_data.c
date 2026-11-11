#include "tnt_african_gameplay_vita.h"

/* Canonical African one-player gameplay assets: sky scene 567/anim 568,
 * background IMG 19/20, and volcano smoke D_800D2C38. */
f32 g_tnt_african_sky_nodes[TNT_AFRICAN_SKY_NODE_COUNT][9]; /* Reconstructed from player ROM scene 567. */
u8 g_tnt_african_sky_node_type[TNT_AFRICAN_SKY_NODE_COUNT]; /* Reconstructed from player ROM scene 567. */
s8 g_tnt_african_sky_parents[TNT_AFRICAN_SKY_NODE_COUNT]; /* Reconstructed from player ROM scene 567. */
u8 g_tnt_african_sky_node_mesh[TNT_AFRICAN_SKY_NODE_COUNT]; /* Reconstructed from player ROM scene 567. */
f32 g_tnt_african_sky_mesh_scale[TNT_AFRICAN_SKY_MESH_COUNT]; /* Reconstructed from player ROM scene 567. */
TNTWonderTriangle g_tnt_african_sky_triangles[TNT_AFRICAN_SKY_TRIANGLE_COUNT]; /* ROM-reconstructed scene 567. */
u8 g_tnt_african_sky_triangle_alpha[TNT_AFRICAN_SKY_TRIANGLE_COUNT]; /* ROM-reconstructed scene 567. */
u16 g_tnt_african_sky_textures[TNT_AFRICAN_SKY_TEXTURE_COUNT][1360]; /* Reconstructed from player ROM scene 567. */
u8 g_tnt_african_background_packbits[87473]; /* Original art loaded from user ROM V6 cache on startup. */
const u32 g_tnt_african_background_packbits_size = 87473U;
u16 g_tnt_african_background_rgba16[256]; /* Original art loaded from user ROM V6 cache on startup. */
u8 g_tnt_african_smoke_ci8[TNT_AFRICAN_SMOKE_FRAME_COUNT][131*106]; /* Original art loaded from user ROM V6 cache on startup. */
u16 g_tnt_african_smoke_rgba16[TNT_AFRICAN_SMOKE_FRAME_COUNT][256]; /* Original art loaded from user ROM V6 cache on startup. */
