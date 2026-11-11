#include "tnt_wonder_complete_vita.h"

/* Canonical Mayan completed-Wonder scene 186, extracted from the USA 1.0 FX pack. */

f32 g_tnt_wonder_complete_nodes[99][12]; /* Initialized from verified player ROM. */
u8 g_tnt_wonder_complete_node_type[99]; /* Initialized from verified player ROM. */

s8 g_tnt_wonder_complete_parents[99]; /* Initialized from verified player ROM. */

u8 g_tnt_wonder_complete_node_mesh[99]; /* Initialized from verified player ROM. */

f32 g_tnt_wonder_complete_mesh_scale[75]; /* Initialized from verified player ROM. */

TNTWonderCompleteTexture g_tnt_wonder_complete_textures[147]; /* Initialized from verified player ROM. */
u16 g_tnt_wonder_complete_texels[130320]; /* original Wonder texels from user ROM */
TNTWonderCompleteTriangle g_tnt_wonder_complete_triangles[3320]; /* Reconstructed from verified player ROM. */
