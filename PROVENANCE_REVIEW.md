# TNTV source and asset overview

## Source

The game reconstruction adapts [chris-gilmore/tnt-splat](https://github.com/chris-gilmore/tnt-splat).
Native Vita input, audio, rendering, loading and asset preparation are
implemented in the port. Component credits and upstream notices are
listed in [UPSTREAM_SOURCES.md](UPSTREAM_SOURCES.md) and
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

## Runtime game assets

The player supplies the supported original USA 1.0 ROM. TNTV validates
it and reconstructs game graphics, animation scenes, music, text and
other game data locally on the device. Generated caches speed up later
launches. The ROM remains required.

The distributed VPK and source archive do not bundle an original ROM,
extracted soundtrack, game-art archive, generated cache or user save.

## Build resources

Vita shader objects and LiveArea resources are included as build inputs.
The custom texture-edge shader's ELF wrapper can be reproduced from the
bundled compiled GXM program; this is not a high-level shader compilation.
The packaged icon and LiveArea files are identified by hash in
[ART_ASSET_PROVENANCE.md](ART_ASSET_PROVENANCE.md).
