#include "tnt_wonder_milestone_vita.h"

/* Canonical African Wonder construction data from USA 1.0 FX:
 * scene 1292, one-shot stage animations 1293..1299, common camera
 * scene 1303/animation 1304, and Wonders stage tables 28..34. */
f32 g_tnt_african_scene1292_nodes[8][9]; /* User-verified ROM FX African Wonder scene1292. */
f32 g_tnt_african_scene1292_stage_nodes[7][8][9]; /* FX scene and one-shot animation sampled from player ROM. */
TNTWonderMilestoneTriangle g_tnt_african_scene1292_triangles[1758]; /* User original ROM FX African construction RSP. */
u16 g_tnt_african_milestone_textures[6][1360]; /* User-verified ROM FX African Wonder scene1292. */
u8 g_tnt_african_milestone_texture_wrap[6]; /* User original ROM FX texture wrap flags. */
f32 g_tnt_african_milestone_camera[997][6]; /* Original scene1303 FX anim1304 user ROM 997 ticks. */
u8 g_tnt_african_milestone_block_count[7]; /* Original player ROM 7-stage block locations/ticks. */
TNTWonderMilestoneBlock g_tnt_african_milestone_blocks[7][TNT_WONDER_MILESTONE_MAX_BLOCKS]; /* Original player ROM 7-stage block locations/ticks. */
