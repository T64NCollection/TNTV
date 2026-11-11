/* TNTV ROM-derived music cache generator — no game music data included.
 *
 * The output contains original commercial-game audio and is PRIVATE to the
 * device whose user supplies the ROM. Do not distribute generated archives.
 */
#ifndef TNTV_MUSIC_BUILDER_H
#define TNTV_MUSIC_BUILDER_H
/* Return 0 on complete byte-verified build; negative on failures.
 * Existing destination is never overwritten. Creates output.tmp and renames
 * only after its SHA256 equals the known supported revision's music cache.
 */
int tntv_music_build_from_rom(const char *rom_path,const char *destination);
#include "tntv_rom_reader.h"
/* Caller already passed tntv_rom_open SHA-256; do not rehash the same ROM. */
int tntv_music_build_from_verified_rom(TNTVRom *verified_rom,const char *destination);
/* Validate a locally existing music archive without loading 10.6MiB at once.
 * Return 0 only for a byte-perfect expected file SHA256.
 */
int tntv_music_validate(const char *path);

#include "tntv_rom_reader.h"
int tntv_sfx_metadata(TNTVRom *rom,const uint16_t *dcm_ids,uint32_t dcm_count,
                     const uint16_t *sample_ids,uint32_t sample_count,
                     uint32_t slots,uint32_t *stream_lengths,uint32_t *lengths,
                     uint32_t *begins,uint32_t *ends,uint8_t *flags,int8_t *maps);
#endif
