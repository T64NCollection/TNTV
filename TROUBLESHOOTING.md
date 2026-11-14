# TNTV — first launch, ROM validation and cache troubleshooting

TNTV requires a compatible original ROM supplied by the user; no game ROM or generated cache packs are bundled.

## The ROM setup screen appears

The setup screen uses the same cybergrid style as loading and displays the problem, plus **X to retry** and **O to exit**. The application does not automatically source a ROM from the network.

| Screen message | Likely cause | What to check |
|---|---|---|
| `ROM FILE MISSING` | No ROM file with a supported extension was found in the expected folder | Verify `ux0:/data/TNTV/` and the extension (`.z64`, `.v64`, or `.n64`); the file can keep its original name |
| `ROM VALIDATION FAILED` | A candidate file was found but was the wrong size, byte format or original game revision | Confirm your own USA 1.0 original ROM is exactly 12,582,912 bytes; supported formats are z64, v64 and n64 |
| `ROM DATA SETUP ERROR` | The ROM was accepted, but scene, UI, art or music preparation failed | Check free space, card accessibility and the loader status/timing logs; leave the existing save untouched |

**File precedence matters when multiple ROMs are present:** `baserom.us.z64`, then `baserom.z64`, then `baserom.v64`, then `baserom.n64`, then `New Tetris, The (USA).z64`, then `The New Tetris (USA).z64`. If none of these exists, TNTV scans for other `.z64`, `.v64` and `.n64` filenames in lexical order. The first file that opens is validated; an invalid one will not be silently skipped. It does not rename files.

## It looks like loading is taking a long time

First launch has to verify the ROM and reconstruct original gameplay assets, animations and music. The cybergrid loading screen shows progress such as scene/art/music preparation. Later launches normally reuse cached data and are faster. A cache integrity problem can trigger a longer repair/reconstruction.

Avoid closing TNTV or interrupting storage access during initial asset generation. No specific seconds-to-load guarantee is made, because storage, ROM order, and cache conditions vary.

## The ROM is already in place, but retry doesn't work

Use VitaShell to check the directory and filename extension. For example, if both `baserom.us.z64` and `New Tetris, The (USA).z64` are present, the first name wins even if it is the wrong revision. Keep only your compatible ROM in the folder and preserve its existing filename. Don't change saves or overwrite any other files.

After correcting the file, return to TNTV and press **X** to retry, or exit and relaunch.

## Music, art or scenes appear missing

Three user-ROM-derived cache files are generated automatically under `ux0:/data/TNTV/`:

- `tntv_rom_bridge_v6.pak` — local game art / bound data cache
- `tntv_scene_cache_v1.bin` — local scene tables and animation resources
- `tntv_music_from_rom_v1.bin` — music reconstructed from your own ROM

These are **not** public distribution assets. They are distinct from the save file, and TNTV has logic to verify/recreate them when needed. **Do not delete caches as a first troubleshooting step** and never delete `tntv_sram.bin` as part of cache repair. Backup user data first if any manual file work becomes necessary.

## Diagnostic logs (private)

TNTV may write these text files in `ux0:/data/TNTV/`:

- `tntv_rom_bridge_status.txt` — the most recent bridge status or error
- `tntv_boot_timings.txt` — timing breakdown for completed ROM setup phases
- `tntv_boot_trace.txt` — application startup trace
- `tntv_hall212_timing.txt` — retained WONDERS Hall performance diagnostics in the tested build

An internal diagnostic line may still include a historical test identifier. **The user-visible loading title is simply “TNTV LOADING”** in TNTV v1.01.

These logs can help local debugging. Before sharing a diagnostic excerpt anywhere, inspect it for personal paths or details. **Do not share the ROM, extracted caches, player saves, music files or hidden cheat values.**

## Unofficial port and licensing

TNTV is not affiliated with the creators or publishers of the original game. Requiring a user-owned ROM does not by itself resolve all rights involving original code or game-derived source. No project-wide redistribution license has been established for adapted game code. See [PROVENANCE_REVIEW.md](PROVENANCE_REVIEW.md).
