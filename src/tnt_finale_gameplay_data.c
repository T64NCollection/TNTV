#include "tnt_finale_gameplay_vita.h"

/* Canonical screen-7 FINALE / bonus Industrial gameplay art from USA 1.0:
 * IMG 125/126 plus Anim archive 17..25 and palettes 273..281. */
u8 g_tnt_finale_background_packbits[101702]; /* Filled from verified local user-ROM cache before gameplay. */
const u32 g_tnt_finale_background_packbits_size = 101702U;
u16 g_tnt_finale_background_rgba16[256]; /* Filled from verified local user-ROM cache before gameplay. */
u8 g_tnt_finale_boiler_ci8[121*119]; /* Filled from verified local user-ROM cache before gameplay. */
u16 g_tnt_finale_boiler_rgba16[256]; /* Filled from verified local user-ROM cache before gameplay. */
u8 g_tnt_finale_bulb_ci8[21*21]; /* Filled from verified local user-ROM cache before gameplay. */
u16 g_tnt_finale_bulb_rgba16[256]; /* Filled from verified local user-ROM cache before gameplay. */
u8 g_tnt_finale_gauge_ci8[TNT_FINALE_GAUGE_FRAME_COUNT][10*9]; /* Filled from verified local user-ROM cache before gameplay. */
u16 g_tnt_finale_gauge_rgba16[TNT_FINALE_GAUGE_FRAME_COUNT][256]; /* Filled from verified local user-ROM cache before gameplay. */
const u8 g_tnt_finale_boiler_intra[TNT_FINALE_BOILER_INSTANCE_COUNT] = {11,2,1};
const u8 g_tnt_finale_boiler_inter[TNT_FINALE_BOILER_INSTANCE_COUNT] = {30,53,42};
const s16 g_tnt_finale_bulb_y[TNT_FINALE_BULB_INSTANCE_COUNT] = {31,51,91,111,150,170,210,230};
const u8 g_tnt_finale_bulb_intra[TNT_FINALE_BULB_INSTANCE_COUNT] = {20,15,15,10,10,15,15,10};
const u8 g_tnt_finale_bulb_inter[TNT_FINALE_BULB_INSTANCE_COUNT] = {20,12,15,23,17,13,27,16};
