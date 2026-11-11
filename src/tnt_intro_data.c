#include "tnt_intro_vita.h"

TNTIntroNodeDef g_tnt_intro_nodes[TNT_INTRO_NODE_COUNT]; /* Original intro FX985 reconstructed from user-ROM. */

float g_tnt_intro_fog_initial[6]; /* Original intro FX985 reconstructed from user-ROM. */
float g_tnt_intro_camera_initial[4]; /* Original intro FX985 reconstructed from user-ROM. */
float g_tnt_intro_lights_initial[3][4]; /* Original intro FX985 reconstructed from user-ROM. */

TNTIntroTextureDef g_tnt_intro_textures[TNT_INTRO_TEXTURE_COUNT]; /* Verified original ROM FX985 original texture pixels/mips. */
uint16_t g_tnt_intro_texture_rgba16[39920]; /* Verified original ROM FX985 original texture pixels/mips. */

TNTIntroVertex g_tnt_intro_vertices[TNT_INTRO_VERTEX_COUNT]; /* Exact original verified ROM FX985 RSP replay. */

TNTIntroDrawDef g_tnt_intro_draws[TNT_INTRO_DRAW_COUNT]; /* Exact original verified ROM FX985 RSP replay. */

TNTIntroKey g_tnt_intro_keys[6722]; /* N64 original animation986 keyframes from player ROM. */

TNTIntroTrackDef g_tnt_intro_tracks[TNT_INTRO_TRACK_COUNT]; /* N64 original animation986 keyframes from player ROM. */

TNTIntroTargetDef g_tnt_intro_targets[TNT_INTRO_TARGET_COUNT]; /* N64 original animation986 keyframes from player ROM. */
