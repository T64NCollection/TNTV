#include "vita_compat.h"

/* Source-accurate N64 credits assets/text/tables. */

u8 g_tnt_credits_background_ci8[30000]; /* user ROM V3 loaded */

u8 g_tnt_credits_background_rgb[768]; /* user ROM V3 loaded */

u8 g_tnt_credits_height_i8[30000]; /* user ROM V3 loaded */

u8 g_tnt_credits_spotlight_ci8[16384]; /* user ROM V3 loaded */

u8 g_tnt_credits_spotlight_rgb[768]; /* user ROM V3 loaded */

u8 g_tnt_credits_font_is_b[192]; /* Reconstructed from verified player ROM. */

s16 g_tnt_credits_sintable[1024]; /* Loaded from verified player ROM. */

/* Text and per-line references are loaded from the verified USA 1.0 ROM.
 * No original credits words or typography are embedded in this C file. */
char g_tnt_credits_rom_text[1894];
const char *g_tnt_credits_lines[181];


const u16 g_tnt_credits_line_count = 181;
