# TNTV upstream code attribution

TNTV is a Vita port containing a mixture of newly implemented native
systems and adapted N64 game reconstruction code. This is a source-credit
record, **not a blanket license**.

## Game reconstruction source

Project: [chris-gilmore/tnt-splat](https://github.com/chris-gilmore/tnt-splat)

Local development reference: commit `bbbded9` (October 3, 2026).

TNTV adapts C code representing original-game input processing,
piece behavior, rules, scoring, AI, state-machine routines and memory
structures. In an internal source comparison, 33 of 133 compiled
game/port C modules had at least 50% direct token overlap with individual
upstream C files, including 28 with at least 90%. Another important
source-derived component is the reconstructed game-types header. These
are technical similarity measures, not percentages of authorship,
ownership or legal permission.

The upstream snapshot had no tracked source license. Attribution does not
authorize relicensing its contributors' work or underlying original-game
code. Rights to publish adapted original N64 game code remain unresolved.

## Vita-specific code

The project additionally implements native Vita ROM checks, local
asset reconstruction, native audio, GXM rendering, controller mapping,
startup and loading UI, and substantial frame-rate and visual fixes.
The absence of exact matches against the compared upstream C sources
does not establish independent authorship of every individual routine.

## Dependencies and other decompilations

lzokay and libvita2d shader notices are preserved under their respective
bundled third-party directories. The SHA-256 dependency includes the
original upstream README. The custom TNTV shader's high-level source
and licensing remain undocumented.

Other public recompilation and decompilation projects have their own
independent provenance and licenses; their licensing terms are **not**
automatically applicable to the `chris-gilmore/tnt-splat` source used here.

See [PROVENANCE_REVIEW.md](PROVENANCE_REVIEW.md) and
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
