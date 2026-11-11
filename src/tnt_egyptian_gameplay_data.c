#include "tnt_egyptian_gameplay_vita.h"

/* Canonical Egyptian 1P gameplay assets extracted from the USA 1.0 ROM.
 * IMG_EGYPTIAN_BACKGROUND is authored as 400x301; the N64 400x300 render
 * viewport clips the final row, so TNTV stores the visible 400x300 region. */
u8 g_tnt_egyptian_background_packbits[105326]; /* Filled from verified local user-ROM cache before gameplay. */
const u32 g_tnt_egyptian_background_packbits_size = 105326U;

u16 g_tnt_egyptian_background_rgba16[256]; /* Filled from verified local user-ROM cache before gameplay. */

u8 g_tnt_egyptian_coal_ci8[12][50 * 26]; /* Player original ANIM sprite or palette at startup. */

u16 g_tnt_egyptian_coal_rgba16[12][256]; /* Player original ANIM sprite or palette at startup. */

u8 g_tnt_egyptian_candle_a_ci8[4][22 * 19]; /* Player original ANIM sprite or palette at startup. */

u16 g_tnt_egyptian_candle_a_rgba16[4][256]; /* Player original ANIM sprite or palette at startup. */

u8 g_tnt_egyptian_candle_b_ci8[4][22 * 19]; /* Player original ANIM sprite or palette at startup. */

u16 g_tnt_egyptian_candle_b_rgba16[4][256]; /* Player original ANIM sprite or palette at startup. */

