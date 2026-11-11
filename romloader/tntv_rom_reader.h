/* TNTV public-tooling prototype: user-supplied original ROM I/O.
 * Does not contain any ROM data. Uses third_party/crypto-algorithms/sha256
 * (Brad Conte, public domain) and third_party/lzokay (Jack Andersen, MIT).
 * Designed for VitaSDK C callers but also testable on Linux.
 */
#ifndef TNTV_ROM_READER_H
#define TNTV_ROM_READER_H
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define TNTV_ROM_SIZE 12582912U
#define TNTV_ROM_MAX_RESOURCE 8388608U
#define TNTV_ROM_IMG_DIR 0x273A00U
#define TNTV_ROM_ANIM_DIR 0x5206B0U

typedef struct {
    FILE *file;
    uint32_t length;
    uint8_t swap_mask; /* canonical z64=0, swapped v64=1, little-endian n64=3 */
    uint8_t fully_verified; /* 1 only after complete canonical-ROM SHA256 */
} TNTVRom;

typedef enum {
    TNTV_ROM_OK=0,
    TNTV_ROM_ERR_PARAM=-1,
    TNTV_ROM_ERR_OPEN=-2,
    TNTV_ROM_ERR_SIZE=-3,
    TNTV_ROM_ERR_FORMAT=-4,
    TNTV_ROM_ERR_REVISION=-5,
    TNTV_ROM_ERR_IO=-6,
    TNTV_ROM_ERR_BOUND=-7,
    TNTV_ROM_ERR_DECODE=-8,
    TNTV_ROM_ERR_MEMORY=-9
} TNTVRomResult;

/* Validate entire ROM with exact SHA256 after normalization, without writing a
 * normalized ROM to disk. Does not retain 12 MiB in memory.
 */
int tntv_rom_open(TNTVRom *rom, const char *rom_path);
/* Warm-only sampled revision check, NOT equivalent to a full checksum.
 * Never use for building/rebuilding derived assets without upgrading to
 * tntv_rom_verify_full first. It requires the actual ROM file to exist. */
int tntv_rom_open_sampled(TNTVRom *rom, const char *rom_path);
int tntv_rom_verify_full(TNTVRom *rom);

void tntv_rom_close(TNTVRom *rom);

/* Read a normalized big-endian byte range. File byte orders are transparent. */
int tntv_rom_read(TNTVRom *rom, uint32_t offset, void *out, size_t len);

/* Allocate and decode one H2ON/H2OS entry from the original ROM.
 * Caller owns *out on success and must free it. Works for IMG, ANIM, and any
 * other valid resource directory base in the verified retail ROM.
 */
int tntv_rom_extract(TNTVRom *rom, uint32_t directory, uint32_t resource_id,
                     uint8_t **out, uint32_t *outlen);

/* Original USA 1.0 hash IDs for archive version / first-run caching. */
const uint8_t *tntv_rom_expected_sha256(void); /* 32 bytes */
const uint8_t *tntv_rom_expected_md5(void);    /* 16 bytes */

#endif
