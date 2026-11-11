/* Test200Q ROM-only Russian fallback sky synthesizer.
 * Inputs are the original N64 scene901 textures decoded ONLY from the
 * SHA256-verified user's own USA1.0 ROM. No game artwork or bitmap bytes are
 * embedded or distributable. Output may visually differ from frozen Test199Q.
 */
#ifndef TNTV_SKY_SYNTH_H
#define TNTV_SKY_SYNTH_H
#include <stdint.h>
int tntv_russian_sky_synthesize(
    const uint16_t rgba16[3][1360], const uint16_t valid_texels[3],
    uint32_t output[400*300]);
#endif
