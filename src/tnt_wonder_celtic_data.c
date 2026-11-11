#include "tnt_wonder_milestone_vita.h"

/* Canonical Celtic Wonder construction data from USA 1.0 FX:
 * scene 1244, one-shot stage animations 1245..1251, common camera
 * scene 1277/animation 1278, and Wonders stage tables 21..27. */
f32 g_tnt_celtic_scene1244_nodes[9][9]; /* Original scene1244 Celtic Wonder source verified ROM. */
f32 g_tnt_celtic_scene1244_stage_nodes[7][9][9]; /* Polynomial animation sampled exclusively from user ROM. */
TNTWonderMilestoneTriangle g_tnt_celtic_scene1244_triangles[530]; /* Original FX scene1244 source ROM RSP triangles. */
u16 g_tnt_celtic_milestone_textures[4][1360]; /* Original scene1244 Celtic Wonder source verified ROM. */
u8 g_tnt_celtic_milestone_texture_wrap[4]; /* Reconstructed from user original NTSC USA N64 ROM. */
f32 g_tnt_celtic_milestone_camera[997][6]; /* Polynomial animation sampled exclusively from user ROM. */
u8 g_tnt_celtic_milestone_block_count[7]; /* Reconstructed from user original NTSC USA N64 ROM. */
TNTWonderMilestoneBlock g_tnt_celtic_milestone_blocks[7][TNT_WONDER_MILESTONE_MAX_BLOCKS]; /* Reconstructed from user original NTSC USA N64 ROM. */
