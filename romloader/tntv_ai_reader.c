#include "tntv_ai_reader.h"
#include <string.h>
/* USA1.0 exact original .data segment, N64 005580 AI shape search:
 * bytes are extracted from a player's verified ROM, never compiled in.
 * Contiguous ROM range includes only six alignment bytes between tables.
 */
int tntv_ai_restore_piece_patterns(
    TNTVRom *rom,
    uint8_t mask[5], uint8_t directions[19],
    uint8_t search_ranges[38], int8_t base_shapes[80],
    int8_t rotated_shapes[1696]) {
    uint8_t stage[0x734];
    if(!rom || !mask || !directions || !search_ranges ||
       !base_shapes || !rotated_shapes)return -1;
    if(tntv_rom_read(rom,0x8B830U,stage,sizeof(stage))!=TNTV_ROM_OK)
        return -2;
    /* All output buffers commit only after entire source ROM read succeeds. */
    memcpy(mask,stage+0x00U,5U);
    memcpy(directions,stage+0x08U,19U);
    memcpy(search_ranges,stage+0x1CU,38U);
    memcpy(base_shapes,stage+0x44U,80U);
    memcpy(rotated_shapes,stage+0x94U,1696U);
    return 0;
}
