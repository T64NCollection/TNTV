#include "tnt_opponent_2p_vita.h"

/* Exact USA 1.0 one-player OPPONENT / two-board presentation assets.
 * Screens 0..6 use culture-specific 2P art. Source screen 7 reuses Russian. */

u8 s_mayan_2p_bg_packbits[114331]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_mayan_2p_bg_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

u8 s_greek_2p_bg_packbits[114336]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_greek_2p_bg_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

u8 s_egyptian_2p_bg_packbits[115869]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_egyptian_2p_bg_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

u8 s_celtic_2p_bg_packbits[113825]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_celtic_2p_bg_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

u8 s_african_2p_bg_packbits[118734]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_african_2p_bg_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

u8 s_japanese_2p_bg_packbits[111196]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_japanese_2p_bg_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

u8 s_russian_2p_bg_packbits[111221]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_russian_2p_bg_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

u8 s_mayan_2p_time[1920]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_mayan_2p_time_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

u8 s_greek_2p_time[1920]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_greek_2p_time_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

u8 s_egyptian_2p_time[2176]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_egyptian_2p_time_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

u8 s_celtic_2p_time[1920]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_celtic_2p_time_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

u8 s_african_2p_time[1920]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_african_2p_time_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

u8 s_japanese_2p_time[1920]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_japanese_2p_time_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

u8 s_russian_2p_time[1920]; /* Loaded from the local user-ROM-derived V6 cache. */
u16 s_russian_2p_time_pal[256]; /* Loaded from the local user-ROM-derived V6 cache. */

const TNTOpponent2PBackground g_tnt_opponent_2p_backgrounds[TNT_OPPONENT_2P_CULTURE_COUNT] = {
    {s_mayan_2p_bg_packbits, sizeof(s_mayan_2p_bg_packbits), s_mayan_2p_bg_pal},
    {s_greek_2p_bg_packbits, sizeof(s_greek_2p_bg_packbits), s_greek_2p_bg_pal},
    {s_egyptian_2p_bg_packbits, sizeof(s_egyptian_2p_bg_packbits), s_egyptian_2p_bg_pal},
    {s_celtic_2p_bg_packbits, sizeof(s_celtic_2p_bg_packbits), s_celtic_2p_bg_pal},
    {s_african_2p_bg_packbits, sizeof(s_african_2p_bg_packbits), s_african_2p_bg_pal},
    {s_japanese_2p_bg_packbits, sizeof(s_japanese_2p_bg_packbits), s_japanese_2p_bg_pal},
    {s_russian_2p_bg_packbits, sizeof(s_russian_2p_bg_packbits), s_russian_2p_bg_pal},
};

const TNTOpponent2PTimePanel g_tnt_opponent_2p_time_panels[TNT_OPPONENT_2P_CULTURE_COUNT] = {
    {64,30,s_mayan_2p_time,s_mayan_2p_time_pal},
    {64,30,s_greek_2p_time,s_greek_2p_time_pal},
    {64,34,s_egyptian_2p_time,s_egyptian_2p_time_pal},
    {64,30,s_celtic_2p_time,s_celtic_2p_time_pal},
    {64,30,s_african_2p_time,s_african_2p_time_pal},
    {64,30,s_japanese_2p_time,s_japanese_2p_time_pal},
    {64,30,s_russian_2p_time,s_russian_2p_time_pal},
};
