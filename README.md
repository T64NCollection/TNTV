# TNTV — The New Tetris on PS Vita

A native, unofficial port of the Nintendo 64 game *The New Tetris* to
PlayStation Vita handheld and PlayStation TV.

TNTV supports single-player stages, original music, WONDERS progress,
and multiplayer with paired controllers on PlayStation TV. Native GPU
rendering, Vita controls, and performance optimizations make it possible
to play on real Vita hardware. The latest real-device-approved source is
Test216D, including a correction to Greek's upper skybox.

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
These tests are technical packaging checks, **not copyright clearance**.
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

## Attribution, source rights and status

The original gameplay-reconstruction C source contains adapted code from
[chris-gilmore/tnt-splat](https://github.com/chris-gilmore/tnt-splat).
The reviewed upstream snapshot does **not** contain a tracked source
redistribution license. The separate custom GPU fragment shader's
high-level original source and redistribution rights are also not
documented. This project therefore **does not claim** a blanket MIT,
GPL, or other project-wide source license. Making a repository public
would not by itself resolve the rights to adapted game code.

Original game rights and trademarks remain with their respective
owners. This project is unofficial and is not affiliated with or
endorsed by those owners. Read [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md),
[UPSTREAM_SOURCES.md](UPSTREAM_SOURCES.md), and
[PROVENANCE_REVIEW.md](PROVENANCE_REVIEW.md) for known component lineage
and unresolved issues. The [art provenance record](ART_ASSET_PROVENANCE.md)
identifies exactly which approved Vita icon and LiveArea files are included.

**This repository worktree is a draft prepared for eventual publication,
not a claim that distribution is rights-cleared or that a release already exists.**
