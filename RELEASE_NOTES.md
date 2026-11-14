# TNTV v1.01 — Single Player Opponent 60 FPS update

## What's new in v1.01

- **All seven Single Player OPPONENT (vs. CPU) cultural backdrops** now render at approximately **60 FPS, up from 30 FPS in TNTV v1.0**. All seven were validated on physical PlayStation TV hardware; several were spot-checked on handheld Vita.
- Corrected precise playfield-to-backdrop alignment so locked blocks meet the artwork borders, while preserving metallic blocks, rotations, line clears, holds, garbage attacks, pause/Progress, and winner/loser results.
- Optimized individual locked-block texture rendering and framebuffer restoration without changing CPU difficulty or game speed.
- The optional **L + R + Select FPS counter** remains; internal development timing diagnostics have been removed from the public build.

## Updating from v1.0

Install `TNTV_v1.01.vpk` over the existing TNTV installation using VitaShell. The app retains Title ID `NTET00001`; **do not delete `ux0:/data/TNTV/`**, which contains your user-supplied ROM, generated assets, and saved profiles/Wonders progress. No ROM rename or cache reset is required.

The new 60 FPS claim applies to **Single Player Opponent**, not local 2–4 player multiplayer or cinematic sequences. This update does not change the other game modes.

---

# TNTV v1.0 — PS Vita & PlayStation TV

## Highlights

- **Eight optimized solo stages:** The seven original cultural environments plus Industrial/Finale, tuned for **approximately 60 FPS** in normal single-player gameplay.
- **Hardware-tested 2–4 player PSTV multiplayer:** DualShock 3 and DualShock 4 controllers, garbage, targeting, Hot Potato and match results.
- **Restored Wonders:** Construction milestones, cinematic sequences, WONDERS Hall and finale, with saved progression.
- **Original game experience:** Music reconstructed from your ROM, sound effects, native Vita rendering, original name-entry cheats and an FPS toggle.
- **Simple ROM setup:** Accepts compatible `.z64`, `.v64` and `.n64` filenames as-is.

## Getting started

Install `TNTV_v1.0.vpk` through VitaShell and place your own original
*The New Tetris* **USA 1.0 N64 ROM (12,582,912 bytes)** in
`ux0:/data/TNTV/`. No renaming is needed. The first launch creates local
game-resource caches; preserve `tntv_sram.bin` to keep saved progress.

Controls are listed in [README.md](README.md). For a complete setup guide,
see [INSTALLATION.md](INSTALLATION.md).

**Note:** Solo stage gameplay is optimized for around 60 FPS; performance
varies by mode and scene.

## Credits

Based on the game reconstruction by chris-gilmore/tnt-splat, with
substantial AI assistance in porting and optimization and hands-on PS Vita
and PSTV hardware testing. This is an unofficial fan project. Component
notices and acknowledgments are in THIRD_PARTY_NOTICES.md and
UPSTREAM_SOURCES.md.
