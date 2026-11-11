/* Player-ROM original IMG50 PackBits reconstitution; no original art data. */
#ifndef TNTV_IMG_READER_H
#define TNTV_IMG_READER_H
#include <stdint.h>
#include "tntv_rom_reader.h"
int tntv_img_restore_4p_packbits(TNTVRom *rom,uint8_t *destination,uint32_t outcap);
/* Source IDs specify the ORIGINAL N64 ANIM frame/palette lookup order.
 * Destination pixel frames and 256-color RGBA16 palettes are native-endian.
 */
int tntv_img_restore_anim_ci8(TNTVRom *rom,const uint16_t *frame_ids,
                              const uint16_t *palette_ids,uint32_t frames,
                              uint32_t width,uint32_t height,
                              uint8_t *ci8,uint16_t *rgba16);
int tntv_img_font_widths(TNTVRom *rom,uint32_t image_id,uint8_t widths[46]);
int tntv_img_restore_4p_culture(TNTVRom *rom, uint32_t image_id,
                                uint32_t palette_id, uint8_t *destination,
                                uint32_t capacity, uint16_t palette[256]);
int tntv_img_restore_credits_text(TNTVRom *rom, char *destination,
                                  uint32_t capacity, const char **lines,
                                  uint32_t line_count);
int tntv_img_restore_ui_strings(TNTVRom *rom,
        char story_storage[7][18][32], const char *stories[7][18],
        char guest_storage[8][9][16], const char *guests[8][9]);
#endif
