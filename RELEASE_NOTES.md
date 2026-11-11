# TNTV v1.0-rc2 — PS Vita & PlayStation TV

## Changes since v1.0-rc1

- **No more manual ROM renaming.** Drop your compatible original USA 1.0
  ROM into `ux0:/data/TNTV/` under its existing filename, including the
  common `New Tetris, The (USA).z64` or `The New Tetris (USA).z64`.
  Other .z64, .v64 and .n64 names are recognized in that folder too.
  The existing `baserom.*` aliases still work.
- File discovery never renames, copies, patches or modifies your ROM.
  Original ROM size/revision verification, cache generation and saved
  profile/progress paths are unchanged.
- Owner-tested on physical Vita hardware with the naturally named ROM:
  startup, asset loading and gameplay passed. Prior hardware testing of
  Wonders and up-to-four-player PSTV modes is retained; the RC2 change
  is limited to initial ROM filename discovery and updated documentation.
- Release descriptions more prominently recognize all seven culture
  stages' Wonders progression, construction sequences, WONDERS Hall,
  finale and secret/cheat support.

## Highlights

- Native *The New Tetris* gameplay on PS Vita and PlayStation TV (PSTV / Vita TV).
- **Hardware-tested two-, three-, and four-player local multiplayer on PSTV**, using DualShock 3 and DualShock 4 controllers, including garbage attacks, targeting, Hot Potato and match results.
- **Extensively restored Wonders:** progression through all seven culture stages, construction milestones and animations, WONDERS Hall (completed, in-progress and locked presentations), the finale, and saved unlock progress. These sequences received extensive visual and performance testing on hardware.
- Original soundtrack reconstructed locally from the user's ROM, supported and tested original name-entry cheats, and perhaps an undisclosed surprise or two.
- Native Vita rendering and performance improvements, FPS display, and the hardware-tested Greek skybox correction.
- On-device ROM setup with loading progress, support for .z64, .v64 and .n64 byte orders, and recognition of common original ROM filenames without renaming.

## Setup

Supply your own supported USA 1.0 N64 ROM of *The New Tetris*, exactly
12,582,912 bytes (12 MiB). No ROM, extracted soundtrack, game-art archive,
generated cache or save data is bundled. See INSTALLATION.md.

## Performance

Frame rate varies by scene; some full WONDERS cinematic sequences run at
30 FPS. First launch creates local caches and takes longer than later
launches. Further startup and cinematic optimization is planned for future work.

## Credits

Game reconstruction adapted from chris-gilmore/tnt-splat. Porting and
optimization involved substantial AI assistance and real-device testing.
See THIRD_PARTY_NOTICES.md and UPSTREAM_SOURCES.md for component credits.
This is an unofficial fan project; original copyrights and trademarks
remain with their respective owners.
