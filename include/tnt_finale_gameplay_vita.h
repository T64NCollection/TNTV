#ifndef TNT_FINALE_GAMEPLAY_VITA_H
#define TNT_FINALE_GAMEPLAY_VITA_H
#include "vita_compat.h"
#define TNT_FINALE_GAUGE_FRAME_COUNT 7
#define TNT_FINALE_BOILER_INSTANCE_COUNT 3
#define TNT_FINALE_BULB_INSTANCE_COUNT 8
extern u8 g_tnt_finale_background_packbits[];
extern const u32 g_tnt_finale_background_packbits_size;
extern u16 g_tnt_finale_background_rgba16[256];
extern u8 g_tnt_finale_boiler_ci8[121*119];
extern u16 g_tnt_finale_boiler_rgba16[256];
extern u8 g_tnt_finale_bulb_ci8[21*21];
extern u16 g_tnt_finale_bulb_rgba16[256];
extern u8 g_tnt_finale_gauge_ci8[TNT_FINALE_GAUGE_FRAME_COUNT][10*9];
extern u16 g_tnt_finale_gauge_rgba16[TNT_FINALE_GAUGE_FRAME_COUNT][256];
extern const u8 g_tnt_finale_boiler_intra[TNT_FINALE_BOILER_INSTANCE_COUNT];
extern const u8 g_tnt_finale_boiler_inter[TNT_FINALE_BOILER_INSTANCE_COUNT];
extern const s16 g_tnt_finale_bulb_y[TNT_FINALE_BULB_INSTANCE_COUNT];
extern const u8 g_tnt_finale_bulb_intra[TNT_FINALE_BULB_INSTANCE_COUNT];
extern const u8 g_tnt_finale_bulb_inter[TNT_FINALE_BULB_INSTANCE_COUNT];
#endif
