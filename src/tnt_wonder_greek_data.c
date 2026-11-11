#include "tnt_wonder_milestone_vita.h"

/* Canonical Greek Wonder construction data extracted from USA 1.0 FX
 * scene 1125, camera animation 1137, and Wonders stage tables 7..13. */
f32 g_tnt_greek_scene1125_nodes[8][9]; /* Scene1125 original node poses from verified player ROM. */

TNTWonderMilestoneTriangle g_tnt_greek_scene1125_triangles[432]; /* Original milestone mesh scene1125 reconstructed from user ROM. */

f32 g_tnt_greek_camera_xz[509][2]; /* Original FX scene1136 animation1137 X/Z sampled from user ROM. */

u8 g_tnt_greek_milestone_block_count[7]; /* Original verified USA 1.0 ROM source Greek Wonder block pointers. */

TNTWonderMilestoneBlock g_tnt_greek_milestone_blocks[7][19]; /* Original verified USA 1.0 ROM source Greek Wonder block pointers. */
