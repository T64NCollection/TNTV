#ifndef TNTV_ROM_LOCATOR_H
#define TNTV_ROM_LOCATOR_H
#include <stddef.h>

/* Look only within the private TNTV data directory. Never alter or copy a
 * ROM. Prefer established names; otherwise find a file with an N64 ROM
 * extension. A hit is a candidate, NOT validation: the reader subsequently
 * checks the exact USA 1.0 content and byte order. Returns 1 on a hit.
 */
int tntv_rom_find_in_directory(const char *directory, char *out, size_t capacity);
#endif
