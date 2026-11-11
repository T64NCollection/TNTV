/* TNTV local asset-cache reader: original game content NOT included.
 *
 * Reads the private ROM-derived TNTVPAK1 cache generated on the device or
 * from the user's own original USA1.0 ROM. A pack is not redistributable
 * merely because this parser is open.
 */
#ifndef TNTV_CACHE_READER_H
#define TNTV_CACHE_READER_H
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#define TNTV_CACHE_MAX_ENTRIES 512
typedef struct {
    char name[64];
    uint32_t offset;
    uint32_t length;
    uint32_t crc32;
} TNTVCacheEntry;
typedef struct {
    FILE *file;
    uint32_t file_size;
    uint32_t count;
    TNTVCacheEntry entries[TNTV_CACHE_MAX_ENTRIES];
} TNTVCache;

/* Returns 0 on success; all failures leave the cache safe to close. */
int tntv_cache_open(TNTVCache *c, const char *path, const uint8_t expected_rom_md5[16]);
void tntv_cache_close(TNTVCache *c);
const TNTVCacheEntry *tntv_cache_find(const TNTVCache *c, const char *name);

/* Destination must be at least entry->length. Validates CRC32 on load.
 * Returns 0 on success, negative on I/O, bounds, size, or CRC mismatch. */
int tntv_cache_read(const TNTVCache *c, const char *name,
                    void *dst, size_t dst_capacity);
#endif
