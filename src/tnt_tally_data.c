#include "vita_compat.h"

/* Exact ROM-derived tally artwork. Kept mutable so Vita links it into the RW data segment. */

u8 g_tnt_tally_panel_ci8[99898]; /* user ROM V3 loaded */
u16 g_tnt_tally_panel_rgba16[256]; /* user ROM V3 loaded */
u8 g_tnt_tally_shadow_ia8[101400]; /* user ROM V3 loaded */
