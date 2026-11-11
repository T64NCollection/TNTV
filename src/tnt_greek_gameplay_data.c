#include "tnt_greek_gameplay_vita.h"

/* Canonical Greek one-player screen assets extracted from the USA 1.0 ROM:
 * Borg skybox 1399, rain scene/animation 1318/1319, IMG_GREEK_BACKGROUND,
 * and D_800D2890 fire-pot frames.
 *
 * Skybox S/T below stores effective RSP coordinates in TNTV's /64 units.
 * Material 0x54 selects gSPTexture scale 0x8000 (1/2) for G_VTX-loaded S/T,
 * while G_MODIFYVTX(G_MWO_POINT_ST) explicitly bypasses that scale.  Modified
 * S/T therefore has to be doubled here relative to the raw display-list word.
 * Losing that provenance creates the large rectangular cloud-panel seams. */
f32 g_tnt_greek_skybox_nodes[2][9]; /* User original ROM sky FX scene1399. */

TNTWonderTriangle g_tnt_greek_skybox_triangles[64]; /* Native Greek FX skybox RSP triangles from user ROM. */

u16 g_tnt_greek_skybox_textures[8][1360]; /* User original ROM sky FX scene1399. */

f32 g_tnt_greek_rain_nodes[8][9]; /* Reconstructed from player ROM FX scene1318. */

TNTWonderTriangle g_tnt_greek_rain_triangles[23]; /* ROM FX scene1318 original RSP triangle source. */

u16 g_tnt_greek_rain_texture[1360]; /* Reconstructed from player ROM FX scene1318. */

u8 g_tnt_greek_rain_anim_1319[3024]; /* original FX from user ROM */

u8 g_tnt_greek_background_ci8[400 * 300]; /* Filled from verified local user-ROM cache before gameplay. */

u16 g_tnt_greek_background_rgba16[256]; /* Filled from verified local user-ROM cache before gameplay. */

u8 g_tnt_greek_fire_ci8[14][103 * 71]; /* User original ROM animation frames, not embedded. */

u16 g_tnt_greek_fire_rgba16[14][256]; /* User original ROM animation frames, not embedded. */
