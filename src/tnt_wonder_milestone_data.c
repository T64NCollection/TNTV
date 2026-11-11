#include "tnt_wonder_milestone_vita.h"


/* D_800C60E4..D_800C69D0: Mayan construction milestones 0..6.
 * The first three entries of each canonical object list are the common
 * camera/environment/static-scene objects; these are the remaining falling
 * block wrappers (DCM 1007), preserving their authored XYZ and start tick. */
u8 g_tnt_mayan_milestone_block_count[7]; /* Player original N64 ROM pointer tables, not static art. */

TNTWonderMilestoneBlock g_tnt_mayan_milestone_blocks[7][6]; /* Player original N64 ROM pointer tables, not static art. */

f32 g_tnt_milestone_scene1006_nodes[9][9]; /* Original FX ROM data only; no static original pixels. */

f32 g_tnt_milestone_scene1088_nodes[4][9]; /* Original FX ROM data only; no static original pixels. */

f32 g_tnt_milestone_scene1104_nodes[8][9]; /* Original FX ROM data only; no static original pixels. */

f32 g_tnt_milestone_scene1057_nodes[2][9]; /* Original FX ROM data only; no static original pixels. */

TNTWonderMilestoneTriangle g_tnt_milestone_scene1006_triangles[244]; /* Scene-original N64 RSP reconstructed from verified player ROM. */

TNTWonderMilestoneTriangle g_tnt_milestone_scene1088_triangles[126]; /* Scene-original N64 RSP reconstructed from verified player ROM. */

TNTWonderMilestoneTriangle g_tnt_milestone_scene1104_triangles[212]; /* Scene-original N64 RSP reconstructed from verified player ROM. */

TNTWonderMilestoneTriangle g_tnt_milestone_scene1057_triangles[10]; /* Scene-original N64 RSP reconstructed from verified player ROM. */

u16 g_tnt_milestone_large_textures[15][1360]; /* Original FX ROM data only; no static original pixels. */

u8 g_tnt_milestone_large_texture_wrap[15]; /* Original FX ROM data only; no static original pixels. */

f32 g_tnt_milestone_block_y[191][7]; /* Original scene1006 FX animation1007 sampled from user ROM. */

f32 g_tnt_milestone_camera_xz[510][2]; /* User original FX scene1114 anim1115 510 frames. */
