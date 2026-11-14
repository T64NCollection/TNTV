# TNTV installation and first launch

This guide describes the ROM loader shipped with TNTV v1.01 (also compatible with the original v1.0 setup). TNTV is an unofficial homebrew port for PlayStation Vita and PlayStation TV (PSTV / Vita TV).

## Requirements

- A PS Vita or PlayStation TV capable of running Vita homebrew through VitaShell.
- A TNTV VPK built from the project source.
- **Your own supported original USA 1.0 ROM of *The New Tetris* for Nintendo 64**. The exact image must be **12,582,912 bytes (12 MiB)**. It is checked against the supported original ROM revision; a filename alone does not make an incompatible image work.
- Enough free storage on `ux0:` for the ROM and locally generated game caches. The original ROM is approximately 12 MiB; the art, scene and music caches together require approximately another 20 MiB, plus working space during generation and save data.

TNTV distributes neither an original ROM nor an extracted music/art archive. Do not request, upload, or share ROM images.

## Install

1. Transfer the TNTV VPK to a convenient local location such as `ux0:/download/`.
2. Open VitaShell and install the VPK.
3. In VitaShell, create the directory `ux0:/data/TNTV/` if it does not exist. The initial loader also attempts to create it.
4. Transfer **your own original compatible ROM** into that directory. **Keep its existing filename.** TNTV recognizes common names like `New Tetris, The (USA).z64` and `The New Tetris (USA).z64`, the older `baserom.*` names, and other ROM files ending in `.z64`, `.v64`, or `.n64`.

   All three byte orders are supported directly, regardless of filename. No renaming, PC-side conversion, or patching is required. An unsupported ROM version will still fail content validation.

5. Launch **TNTV** from LiveArea. The first launch checks the supported ROM and prepares local game graphics, animation scenes and soundtrack caches. This takes longer than ordinary launches.
6. When loading completes, play normally. **Leave the original ROM in place**; even a warm launch checks for it and validates it.

## If multiple candidates exist

The loader prefers the **first existing file** in the following order:

1. `baserom.us.z64`
2. `baserom.z64`
3. `baserom.v64`
4. `baserom.n64`
5. `New Tetris, The (USA).z64`
6. `The New Tetris (USA).z64`

If none of those names exists, TNTV scans the same directory for any
`.z64`, `.v64` or `.n64` file (extension case-insensitive), selecting
the first filename in lexical order. It does not search subdirectories.

**For best results, keep one ROM in this directory.** If the first candidate
exists but is an unsupported revision, TNTV will reject it rather than
silently fall through to another ROM. It never renames, moves or copies
the ROM file.

## First launch and subsequent launches

- **First launch / missing caches:** Full ROM SHA-256 validation, local reconstruction of original assets, scene-cache preparation, music preparation. Loading progress appears on TNTV's cybergrid screen.
- **Later launches with intact caches:** Faster ROM fingerprinting and cache integrity checks, reloading local generated assets. If a cache needs regeneration, TNTV upgrades to full-ROM verification first.
- **Unsupported or missing ROM:** The ROM setup screen explains the problem; **X** retries after the file has been corrected, **O** exits.

TNTV's music, artwork and game data are produced **locally from your own ROM**; they are not included in the application package.

## PlayStation TV (PSTV / Vita TV) and controls

The Vita control adapter maps **Cross to original N64 A**, **Circle to N64 B**, **Start to original Start**, the D-pad to N64 directions, L/R shoulder buttons to N64 L/R, and **Triangle/Square** to the port's N64 additional-button mappings. Analog left-stick movement is adapted to N64 stick input. In multiplayer, PlayStation TV detects paired DS3/DS4 controllers by external controller ports.

In-game actions depend on the original game's selected menu and control state. This is a mapping reference, not a separate guarantee about each action's default behavior.

## Saved data — preserve it

TNTV saves player names, options, scores and unlock progress in:

`ux0:/data/TNTV/tntv_sram.bin`

**Never remove or overwrite this file to troubleshoot ROM loading.** Keep a separate VitaShell backup before any device reinstallation or deliberate save migration. The game-generated art, scene and music caches are separate files and should normally be left alone too.

See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for error and recovery guidance.
