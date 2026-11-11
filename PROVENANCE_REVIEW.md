# Source provenance and rights review

This document describes the source lineage of TNTV and the limitations of
our technical audits. It is **not a license**, permission from upstream
copyright holders, or a legal conclusion.

## Original-game reconstruction

TNTV's core game-logic reconstruction is adapted from the public
[chris-gilmore/tnt-splat](https://github.com/chris-gilmore/tnt-splat)
Nintendo 64 decompilation source. A local source-to-source comparison found
direct lexical overlap in many gameplay C modules and shared game structures.
The upstream snapshot examined for TNTV had no tracked top-level LICENSE,
COPYING or NOTICE. Publication of upstream files does not automatically
authorize others to redistribute or relicense them. The original game's
rights are distinct from the source-reconstruction contributors' rights.

Vita input, audio, GXM rendering, ROM-decoding and asset preparation contain
substantial additional port-specific development. That alone does not
establish that every adaptation of the original game can be redistributed.

## Original ROM assets and executable audit

The VPK contains no separate original ROM, original music archive, or
extracted game-art/scene cache. On the player's Vita, game assets are
reconstructed from their own verified original USA 1.0 ROM.

An earlier sparse-sampling scan of the compiled executable incorrectly
reported no original-ROM byte overlap. A later full-byte-offset comparison
corrected that result and found **14 contiguous matches of 32 bytes or
longer** in Test214D. Original source-ROM-derived high-score defaults,
piece/color data, playfield layout tables and user-interface strings were
then moved into user-ROM-time loading in Test215D.

The improved full-offset scanner found **zero direct ROM/ELF matches of
32 bytes or longer** in Test215D and Test216D, using a 24-byte seed
containing at least seven distinct bytes. This is a **limited exact-match
test**, not proof that short, low-entropy, encoded, transformed, or
original-game-derived code/data are absent. It does not determine rights
in the reconstructed game engine.

## External source and precompiled shaders

- The bundled lzokay source preserves its original MIT license text.
- The libvita2d shader objects carry an MIT notice and upstream author
  attribution; exact versions and binary-to-source correspondence have not
  been independently established.
- Brad Conte's SHA-256 source includes the upstream README describing its
  public-domain designation and requested acknowledgment.
- One custom texture-edge fragment shader is available as precompiled GXM
  bytecode. Its ELF wrapper can be reproduced byte-for-byte from the
  included GXM file. The original high-level shader source and rights
  have not been established.

Read THIRD_PARTY_NOTICES.md and UPSTREAM_SOURCES.md for component details.

## Vita artwork

The packaged icon, LiveArea background and startup graphic are fixed,
hash-identified assets. Their approved appearance is documented in
ART_ASSET_PROVENANCE.md. Exact authorship/licensing of every visual
component or font has not been independently established.

## Licensing and public distribution

No project-wide open-source license is asserted for game-derived code, and
the technical audit results do not grant such rights. A user-owned original
ROM is required for playing TNTV, but that requirement does not itself
settle all distribution rights in adapted game code. These unresolved
questions should not be represented as having been cleared merely because
the VPK is original-asset-free.
