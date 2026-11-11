# TNTV — The New Tetris for PS Vita & PlayStation TV

Take *The New Tetris* on the go—or bring four controllers to the couch.
TNTV is a native, unofficial port of the Nintendo 64 game for **PS Vita**
and **PlayStation TV (PSTV / Vita TV)**.

**Up to four players on real PSTV hardware is a major focus of this port.**
Two-, three-, and four-player gameplay has been tested on Vita TV, with
all four controllers connected using a mix of DualShock 3 and DualShock 4
controllers. Testing covered garbage attacks, target switching, Hot Potato,
pause/resume, and post-match results and line rewards.

TNTV's particular focus is the complete handheld-and-couch experience:
multiplayer on real hardware, native Vita rendering and performance work,
and straightforward setup from your own ROM directly on the device.
Other Vita ports deserve credit for their work too; TNTV is another approach
to bringing this game to Vita, with its own implementation and priorities.

- **Single-player and local multiplayer:** original game modes, stages and
  progression, with up to four players on PSTV / Vita TV.
- **Music and WONDERS:** original soundtrack reconstructed from your ROM,
  WONDERS Hall, construction sequences and saved progress.
- **Native Vita presentation:** GXM rendering, stage-specific visual fixes,
  performance tuning and an FPS display for real-hardware play.
- **On-device ROM setup:** support for .z64, .v64 and .n64 byte orders,
  visible loading progress, and reusable local asset caches.

[Download v1.0-rc1](https://github.com/T64NCollection/TNTV/releases/tag/v1.0-rc1).
The release contains the hardware-tested Test216D build.

**A supported original N64 game ROM supplied by the player is required.**
Neither the application VPK nor the source package includes the original
game ROM, original soundtrack, copied artwork pack, save data, or generated
game-resource cache.

## What you need

- A PlayStation Vita or PlayStation TV that can install homebrew VPKs using VitaShell.
- TNTV's VPK, built with VitaSDK as explained below.
- Your own supported original USA 1.0 N64 ROM of *The New Tetris*,
  exactly **12,582,912 bytes (12 MiB)**.
- Storage for the ROM and local scene, music, and graphics caches.

## Installation

Install the VPK using VitaShell. Place a copy of your original supported
ROM under `ux0:/data/TNTV/` with one of the following filenames:
`baserom.us.z64`, `baserom.z64`, `baserom.v64`, or `baserom.n64`.

The app looks for the first existing candidate in that order. Keep
the ROM in place even after the first successful launch. Asset extraction
takes longer on initial setup; later runs reuse verified private caches.

Read [INSTALLATION.md](INSTALLATION.md) for detailed steps and controls,
and [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for ROM and cache errors.
**Never delete `ux0:/data/TNTV/tntv_sram.bin` to troubleshoot asset
loading; it contains profiles, progress and high scores.**

## Compiling TNTV

You need VitaSDK, CMake 3.16 or newer, a CMake-supported build tool
(such as Make), and Python 3.9 or newer for the offline checks, with the
`VITASDK` environment variable set to
the installed SDK prefix. The original game ROM is *not required* to
compile the VPK; it is required only to run the program on Vita.

```sh
cmake -S . -B ../tntv-build
cmake --build ../tntv-build -j2
```

The resulting VPK is `../tntv-build/new_tetris_vita.vpk`. Keep build
output outside the source folder so the source hygiene audit can check
exactly what will be distributed. With VitaSDK installed,
offline project checks can also be run:

```sh
bash tools/rebuild_texedge_shader_object.sh
python3 -B tools/check_repository_completeness.py
python3 -B tools/check_runtime_docs.py
python3 -B tools/audit_release_artifacts.py --vpk ../tntv-build/new_tetris_vita.vpk
```

The shader command recreates an ELF object from the bundled compiled GXM
program; it does not regenerate the original high-level shader source.
These checks verify source completeness, installation documentation and packaging.
A clean separate source build has passed on the development toolchain,
but independent second-machine reproducibility is not established yet.

## Directory guide

- `src/` and `include/` — reconstructed game logic and native Vita support
- `romloader/` — user-ROM validation and on-device asset construction
- `third_party/` — decoder and shader dependencies
- `sce_sys/` — Vita LiveArea and icon resources
- `tools/` — offline validation and shader-object reconstruction

## Known limitations

Full-length WONDERS cinematic sequences aren't guaranteed to run at
60 FPS; some game scenes intentionally run at 30 FPS. Faster first-boot
resource preparation is possible future work. The original ROM must
remain present for each launch.

See [RELEASE_NOTES.md](RELEASE_NOTES.md) for the first-release summary.

## Credits

TNTV adapts the game reconstruction from
[chris-gilmore/tnt-splat](https://github.com/chris-gilmore/tnt-splat).
Porting and optimization involved substantial AI assistance and extensive
hands-on testing on PS Vita and PSTV / Vita TV.

Thanks to the VitaSDK, homebrew and decompilation communities, and the
original game's creators. This is an unofficial fan project, unaffiliated
with the original game's owners. Original copyrights and trademarks remain
with their respective owners.

See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) and
[UPSTREAM_SOURCES.md](UPSTREAM_SOURCES.md) for component credits,
[PROVENANCE_REVIEW.md](PROVENANCE_REVIEW.md) for the source and asset overview,
and [ART_ASSET_PROVENANCE.md](ART_ASSET_PROVENANCE.md) for the packaged
LiveArea artwork record. Component license texts are preserved alongside
their respective dependencies.
