#include "tnt_wonder_milestone_vita.h"

/* Canonical Egyptian/Sphinx Wonder construction data from USA 1.0 FX:
 * scene 1148, camera animation 1159, and Wonders stage tables 14..20. */
f32 g_tnt_egyptian_scene1148_nodes[8][9]; /* Original scene1148 Egyptian Wonder source N64 ROM. */
TNTWonderMilestoneTriangle g_tnt_egyptian_scene1148_triangles[486]; /* Original Egyptian scene1148 mesh and atlas from user ROM. */
u16 g_tnt_egyptian_milestone_textures[4][1360]; /* Original scene1148 Egyptian Wonder source N64 ROM. */
u8 g_tnt_egyptian_milestone_texture_wrap[4]; /* Player original USA N64 ROM data reconstruction. */
u8 g_tnt_egyptian_milestone_block_count[7]; /* Player original USA N64 ROM data reconstruction. */
TNTWonderMilestoneBlock g_tnt_egyptian_milestone_blocks[7][TNT_WONDER_MILESTONE_MAX_BLOCKS]; /* Player original USA N64 ROM data reconstruction. */
