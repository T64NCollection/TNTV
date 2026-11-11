#include "tnt_russian_gameplay_vita.h"

/* Canonical Russian one-player gameplay assets: sky scene 901/anim 902,
 * background IMG 23/24, thirteen candle flame instances, and snow 1370/1371. */
f32 g_tnt_russian_sky_nodes[TNT_RUSSIAN_SKY_NODE_COUNT][9]; /* Player ROM FX scene901 reconstruction. */
u8 g_tnt_russian_sky_node_type[TNT_RUSSIAN_SKY_NODE_COUNT]; /* Player ROM FX scene901 reconstruction. */
s8 g_tnt_russian_sky_parents[TNT_RUSSIAN_SKY_NODE_COUNT]; /* Player ROM FX scene901 reconstruction. */
u8 g_tnt_russian_sky_node_mesh[TNT_RUSSIAN_SKY_NODE_COUNT]; /* Player ROM FX scene901 reconstruction. */
f32 g_tnt_russian_sky_mesh_scale[TNT_RUSSIAN_SKY_MESH_COUNT]; /* Player ROM FX scene901 reconstruction. */
f32 g_tnt_russian_snow_nodes[TNT_RUSSIAN_SNOW_NODE_COUNT][9]; /* Reconstructed from verified player ROM FX scene1370. */
u8 g_tnt_russian_snow_node_type[TNT_RUSSIAN_SNOW_NODE_COUNT]; /* Reconstructed from verified player ROM FX scene1370. */
s8 g_tnt_russian_snow_parents[TNT_RUSSIAN_SNOW_NODE_COUNT]; /* Reconstructed from verified player ROM FX scene1370. */
u8 g_tnt_russian_snow_node_mesh[TNT_RUSSIAN_SNOW_NODE_COUNT]; /* Reconstructed from verified player ROM FX scene1370. */
f32 g_tnt_russian_snow_mesh_scale[TNT_RUSSIAN_SNOW_MESH_COUNT]; /* Reconstructed from verified player ROM FX scene1370. */
TNTWonderTriangle g_tnt_russian_sky_triangles[TNT_RUSSIAN_SKY_TRIANGLE_COUNT]; /* Player ROM FX scene901 original source reconstruction. */
u8 g_tnt_russian_sky_triangle_alpha[TNT_RUSSIAN_SKY_TRIANGLE_COUNT]; /* Player ROM FX scene901 original source reconstruction. */
u16 g_tnt_russian_sky_textures[TNT_RUSSIAN_SKY_TEXTURE_COUNT][1360]; /* Player ROM FX scene901 reconstruction. */
u16 g_tnt_russian_sky_texture_texels[TNT_RUSSIAN_SKY_TEXTURE_COUNT]; /* Player ROM FX scene901 reconstruction. */
TNTWonderTriangle g_tnt_russian_snow_triangles[TNT_RUSSIAN_SNOW_TRIANGLE_COUNT]; /* Reconstructed from verified player ROM FX scene1370. */
u8 g_tnt_russian_snow_lit[TNT_RUSSIAN_SNOW_TRIANGLE_COUNT]; /* Reconstructed from verified player ROM FX scene1370. */
u8 g_tnt_russian_background_packbits[86608]; /* Original art loaded from user ROM V6 cache on startup. */
const u32 g_tnt_russian_background_packbits_size = 86608U;
u16 g_tnt_russian_background_rgba16[256]; /* Original art loaded from user ROM V6 cache on startup. */
u8 g_tnt_russian_candle_ci8[TNT_RUSSIAN_CANDLE_FRAME_COUNT][2*6]; /* Original art loaded from user ROM V6 cache on startup. */
u16 g_tnt_russian_candle_rgba16[TNT_RUSSIAN_CANDLE_FRAME_COUNT][256]; /* Original art loaded from user ROM V6 cache on startup. */
const s16 g_tnt_russian_candle_x[TNT_RUSSIAN_CANDLE_INSTANCE_COUNT] = {23,53,40,75,196,203,231,250,271,332,337,65,211};
const s16 g_tnt_russian_candle_y[TNT_RUSSIAN_CANDLE_INSTANCE_COUNT] = {259,258,266,257,266,258,267,259,265,259,264,228,229};
const u8 g_tnt_russian_candle_start[TNT_RUSSIAN_CANDLE_INSTANCE_COUNT] = {0,2,6,1,3,5,2,0,7,3,4,3,4};
const u8 g_tnt_russian_candle_intra[TNT_RUSSIAN_CANDLE_INSTANCE_COUNT] = {2,1,3,2,1,2,3,1,2,3,1,3,1};
