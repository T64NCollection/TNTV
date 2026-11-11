#include "vita_compat.h"

/* IMG_SQUARE_GLOW (resource 127), decoded from the user's baserom.
 * Original format: 66x66 IA8. High nibble is intensity, low nibble alpha.
 */
u8 g_tnt_square_glow_ia8[66 * 66]; /* user ROM V3 loaded */
