/* Author-produced conversion instructions, no original game data included. */
#ifndef TNTV_CACHE_BUILDER_H
#define TNTV_CACHE_BUILDER_H
/* Converts the user's verified ROM into a private local native-format pack.
 * NEVER DISTRIBUTE the resulting pack: it contains original game data.
 * Writes a .tmp and renames on success. Existing output is not overwritten.
 * Return 0 on success, negative on failure.
 */
int tntv_cache_build_from_rom(const char *rom_path, const char *cache_path);
/* For a caller that already validated the entire ROM with tntv_rom_open().
 * The caller owns the open stream and must keep it alive until return. */
#include "tntv_rom_reader.h"
int tntv_cache_build_from_verified_rom(TNTVRom *verified_rom, const char *cache_path);
#endif
