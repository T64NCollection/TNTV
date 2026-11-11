#include "tnt_multiplayer_4p_data.h"

/* EXACT untouched native USA/NTSC N64 400x300 CI8 originals from verified
 * 12MiB ROM. Indexed packbits, lossless; original RGBA5551 palettes intact.
 * Mayan remains in existing tnt_multiplayer_4p_data.c byte-for-byte.
 * Images 52,54,56,58,60,62; selected once per match, NEVER per frame.
 * For source screen 7, retail original maps to the Russian fallback. */

/* IMG_EGYPTIAN_4P_BACKGROUND (52) and palette (53). */
u8 g_tnt_egyptian_4p_packbits[112143]; /* From verified player-ROM IMG; never embedded. */
u16 g_tnt_egyptian_4p_rgba16[256]; /* Player-ROM palette. */

/* IMG_JAPANESE_4P_BACKGROUND (54) and palette (55). */
u8 g_tnt_japanese_4p_packbits[104483]; /* From verified player-ROM IMG; never embedded. */
u16 g_tnt_japanese_4p_rgba16[256]; /* Player-ROM palette. */

/* IMG_CELTIC_4P_BACKGROUND (56) and palette (57). */
u8 g_tnt_celtic_4p_packbits[116288]; /* From verified player-ROM IMG; never embedded. */
u16 g_tnt_celtic_4p_rgba16[256]; /* Player-ROM palette. */

/* IMG_AFRICAN_4P_BACKGROUND (58) and palette (59). */
u8 g_tnt_african_4p_packbits[117458]; /* From verified player-ROM IMG; never embedded. */
u16 g_tnt_african_4p_rgba16[256]; /* Player-ROM palette. */

/* IMG_GREEK_4P_BACKGROUND (60) and palette (61). */
u8 g_tnt_greek_4p_packbits[109609]; /* From verified player-ROM IMG; never embedded. */
u16 g_tnt_greek_4p_rgba16[256]; /* Player-ROM palette. */

/* IMG_RUSSIAN_4P_BACKGROUND (62) and palette (63). */
u8 g_tnt_russian_4p_packbits[110636]; /* From verified player-ROM IMG; never embedded. */
u16 g_tnt_russian_4p_rgba16[256]; /* Player-ROM palette. */

const TNT4PCultureBackground g_tnt_4p_culture_backgrounds[TNT_4P_CULTURE_COUNT] = {
    {g_tnt_mayan_4p_background_packbits, 113458U, g_tnt_mayan_4p_background_rgba16},
    {g_tnt_egyptian_4p_packbits, sizeof(g_tnt_egyptian_4p_packbits), g_tnt_egyptian_4p_rgba16},
    {g_tnt_japanese_4p_packbits, sizeof(g_tnt_japanese_4p_packbits), g_tnt_japanese_4p_rgba16},
    {g_tnt_celtic_4p_packbits, sizeof(g_tnt_celtic_4p_packbits), g_tnt_celtic_4p_rgba16},
    {g_tnt_african_4p_packbits, sizeof(g_tnt_african_4p_packbits), g_tnt_african_4p_rgba16},
    {g_tnt_greek_4p_packbits, sizeof(g_tnt_greek_4p_packbits), g_tnt_greek_4p_rgba16},
    {g_tnt_russian_4p_packbits, sizeof(g_tnt_russian_4p_packbits), g_tnt_russian_4p_rgba16},
};
