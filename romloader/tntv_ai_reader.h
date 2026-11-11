/* Test199Q: source-EXACT N64 retail AI piece pattern descriptors.
 * Original USA 1.0 player ROM is already SHA256-validated upstream.
 * No piece-pattern arrays are embedded or reproduced in public source.
 */
#ifndef TNTV_AI_READER_H
#define TNTV_AI_READER_H
#include "tntv_rom_reader.h"
#include <stdint.h>
int tntv_ai_restore_piece_patterns(
    TNTVRom *rom,
    uint8_t mask[5], uint8_t directions[19],
    uint8_t search_ranges[38], int8_t base_shapes[80],
    int8_t rotated_shapes[1696]);
#endif
