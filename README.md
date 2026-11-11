# The New Tetris for PS Vita & PlayStation TV (TNTV)

[![Platform: PS Vita / PSTV](https://img.shields.io/badge/Platform-PS%20Vita%20%7C%20PSTV-1863AE)](https://github.com/T64NCollection/TNTV)
[![Players: 1–4](https://img.shields.io/badge/Players-1%E2%80%934-blueviolet)](https://github.com/T64NCollection/TNTV)
[![Release: v1.0-rc2](https://img.shields.io/badge/Release-v1.0--rc2-brightgreen)](https://github.com/T64NCollection/TNTV/releases/tag/v1.0-rc2)

**TNTV** is an unofficial native port of the Nintendo 64 classic *The New Tetris* for **PS Vita** and **PlayStation TV (PSTV / Vita TV)**, with an emphasis on the complete original experience—including its Wonders and four-player gameplay.

**[Download TNTV v1.0-rc2](https://github.com/T64NCollection/TNTV/releases/tag/v1.0-rc2)**

---

## ✨ Features

- **🎮 All eight solo stages, optimized:** The seven cultural stages and Industrial/Finale gameplay have been tuned for **approximately 60 FPS** on tested Vita hardware, with extensive work on stage scenery, skies, effects and playfield rendering.
- **👥 Genuine four-player PSTV support:** Local **2-, 3- and 4-player** games tested with four physical **DualShock 3 / DualShock 4** controllers. Includes garbage attacks, targeting, Hot Potato and full match results.
- **🏛️ The complete Wonders experience:** Seven cultures, construction milestones, animations, WONDERS Hall displays and the finale—with saved progression and unlocks.
- **🎵 Original sound and presentation:** Soundtrack reconstructed locally from your ROM, original sound effects, native Vita rendering and custom cybergrid pillarboxes.
- **🧩 Original gameplay and extras:** Original modes, rotation rules, hold mechanics, name-entry cheats and perhaps a surprise or two. Includes an optional FPS display.

TNTV is a separate approach to bringing this game to Vita, with particular attention to **visual fidelity, Wonders progression and multiplayer on actual PSTV hardware**.

## 🎮 Controls

The controls below apply to the PS Vita, or to a paired DualShock 3/4 on PSTV (where **L/R** correspond to **L1/R1**).

| Action | PS Vita / PSTV controls |
|---|---|
| Move piece | D-pad **Left / Right** |
| Soft drop | D-pad **Down** |
| Hard drop | D-pad **Up** |
| Rotate clockwise | **Cross (×)** |
| Rotate counterclockwise | **Circle (○)** |
| Hold / swap piece | **L / L1** |
| Pause / Start | **Start** |
| Menu navigation | **D-pad** (left stick where supported) |
| Confirm / Back | **Cross (×) / Circle (○)** |
| Target left / right | **Square / Triangle** (3–4 player **Directed** games) |
| Original N64 R input | **R / R1** (context-dependent) |
| Toggle FPS display | **L + R + Select** |

The same controller mappings apply to each player on PSTV. Multiplayer requires enough paired controllers.

## 📦 Installation

1. Download [the latest TNTV player package](https://github.com/T64NCollection/TNTV/releases/tag/v1.0-rc2), extract it and install the **VPK** using **VitaShell**.
2. Put **your own compatible USA 1.0 Nintendo 64 ROM** of *The New Tetris* (**12,582,912 bytes / 12 MiB**) in `ux0:/data/TNTV/`.
3. Leave the ROM's filename alone. TNTV accepts `.z64`, `.v64` and `.n64` files, including their common original names. For best results, keep just one ROM in the folder.
4. Launch TNTV. The first run prepares graphics, scenes and music from your ROM; later launches reuse local caches.

**No ROM or original game media is included.** Keep your ROM on the device after setup. To preserve names, scores and Wonder progress, **do not delete** `ux0:/data/TNTV/tntv_sram.bin`.

See [INSTALLATION.md](INSTALLATION.md) for a detailed walkthrough or [TROUBLESHOOTING.md](TROUBLESHOOTING.md) if a ROM fails validation.

## 🛠️ Building from source

Install [VitaSDK](https://vitasdk.org/), CMake 3.16+ and a CMake-supported build tool, then run:

```sh
cmake -S . -B ../tntv-build
cmake --build ../tntv-build -j2
```

The output is `../tntv-build/new_tetris_vita.vpk`. **A ROM is not needed to compile TNTV.**

<details>
<summary>Source checks and repository layout</summary>

Offline validation tools (Python 3.9+; the ROM filename test also needs a host C compiler):

```sh
bash tools/rebuild_texedge_shader_object.sh
python3 -B tools/check_repository_completeness.py
python3 -B tools/check_runtime_docs.py
python3 -B tools/test_rom_locator.py
python3 -B tools/audit_release_artifacts.py --vpk ../tntv-build/new_tetris_vita.vpk
```

- `src/`, `include/` — game logic and native Vita code
- `romloader/` — ROM verification and local asset generation
- `sce_sys/` — LiveArea and bubble artwork
- `third_party/`, `tools/` — dependencies, shaders and validation tools

The shader helper reconstructs an ELF object from the bundled compiled program; it does not recreate the original high-level source.

</details>

## 📝 Notes & credits

**Performance varies by scene and player count**; the ~60 FPS figure describes normal *solo stage gameplay*, not every game mode or cinematic. Some visuals, notably the Russian sky, are approximations.

TNTV adapts the reconstruction by [chris-gilmore/tnt-splat](https://github.com/chris-gilmore/tnt-splat). Porting and optimization involved **substantial AI assistance** and extensive testing on PS Vita and PSTV. Thanks to the original developers and the VitaSDK and homebrew communities.

This is an **unofficial fan project** unaffiliated with the game's owners. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md), [UPSTREAM_SOURCES.md](UPSTREAM_SOURCES.md), [PROVENANCE_REVIEW.md](PROVENANCE_REVIEW.md) and [ART_ASSET_PROVENANCE.md](ART_ASSET_PROVENANCE.md) for component notices and credits.
