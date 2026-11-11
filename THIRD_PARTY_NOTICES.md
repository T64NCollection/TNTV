# Third-party notices

Original upstream license texts are included verbatim beside their components. These notices are not a license to distribute reconstructed N64 game logic.

**lzokay**, Jack Andersen — MIT license. Decoder source under `romloader/third_party/lzokay/`; full text at `romloader/third_party/lzokay/LICENSE`.

**libvita2d shader objects**, Sergi Granell (xerpi) — MIT license per the shipped notice in `third_party/libvita2d_shaders/LICENSE`. Exact binary revisions and object provenance should be independently verified.

**crypto-algorithms SHA-256**, Brad Conte — upstream README declares public domain and requests acknowledgment. See `romloader/third_party/crypto-algorithms/README.md`. Exact imported source revision should be documented.

**TNTV texedge fragment shader object** — `third_party/tntv_shaders/texedge_f_gxp.o`. Its object wrapper is exactly reproducible from the bundled 356-byte `texedge_f.gxp` using `tools/rebuild_texedge_shader_object.sh` (SHA-256 of object `5b5cd354e30b73d49caa09c4aa7b34515dbbe207c4195c5dcbab4ddd14a26208`). This only recreates the ELF wrapper around compiled GXM bytecode, **not** the source shader; original authorship, high-level shader source, and redistribution rights remain unresolved.

**Game-logic reconstruction and brand artwork** — original copyright and provenance are separate from the above dependencies. No permission or project-wide license is implied by this notice.

## Adapted game-logic reconstruction — source provenance, rights pending

TNTV incorporates modified N64 C decompilation source from the public chris-gilmore/tnt-splat project (local checked revision bbbded9). The source snapshot contains no tracked license or permission statement. See UPSTREAM_SOURCES.md. This attribution is not proof of permission to redistribute the source or original game code.
