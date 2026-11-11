#ifndef TNT_MULTIPLAYER_4P_DATA_H
#define TNT_MULTIPLAYER_4P_DATA_H
#include "vita_compat.h"
extern u8 g_tnt_mayan_4p_background_packbits[113458];
extern const u32 g_tnt_mayan_4p_background_packbits_size;
extern u16 g_tnt_mayan_4p_background_rgba16[256];
/* Native 3P/4P use the exact same seven culturally themed 4P
 * backgrounds. Original screen ID 7 repeats Russian, not Finale. */
#define TNT_4P_CULTURE_COUNT 7
typedef struct {
    const u8 *packbits;
    u32 packbits_size;
    const u16 *palette;
} TNT4PCultureBackground;
extern u8 g_tnt_egyptian_4p_packbits[112143];
extern u16 g_tnt_egyptian_4p_rgba16[256];
extern u8 g_tnt_japanese_4p_packbits[104483];
extern u16 g_tnt_japanese_4p_rgba16[256];
extern u8 g_tnt_celtic_4p_packbits[116288];
extern u16 g_tnt_celtic_4p_rgba16[256];
extern u8 g_tnt_african_4p_packbits[117458];
extern u16 g_tnt_african_4p_rgba16[256];
extern u8 g_tnt_greek_4p_packbits[109609];
extern u16 g_tnt_greek_4p_rgba16[256];
extern u8 g_tnt_russian_4p_packbits[110636];
extern u16 g_tnt_russian_4p_rgba16[256];
extern const TNT4PCultureBackground g_tnt_4p_culture_backgrounds[TNT_4P_CULTURE_COUNT];
#endif
