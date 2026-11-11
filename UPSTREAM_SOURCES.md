# TNTV upstream code attribution

## Game reconstruction

TNTV adapts C reconstruction code from
[chris-gilmore/tnt-splat](https://github.com/chris-gilmore/tnt-splat).
The local development reference was commit `bbbded9`.

This includes original-game input processing, piece behavior, rules,
scoring, AI, state-machine routines and game structures.

## Vita implementation

The port adds native Vita controller mapping, audio, GXM rendering,
ROM validation and local asset reconstruction, a loading interface,
and stage-specific performance and visual fixes. Development involved
substantial AI assistance and testing on PS Vita and PSTV / Vita TV.

## Dependencies

lzokay and libvita2d shader notices are preserved with their bundled
components. Brad Conte's SHA-256 implementation includes its upstream
README. The TNTV texture-edge shader is bundled as compiled GXM bytecode
with a script to reproduce its ELF object wrapper.

See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for credits and
component license locations, and [PROVENANCE_REVIEW.md](PROVENANCE_REVIEW.md)
for the source and runtime-asset overview.
