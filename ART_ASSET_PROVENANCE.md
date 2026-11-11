# Vita icon and LiveArea artwork — file provenance

This record identifies the exact icon and LiveArea resources shipped
alongside the TNTV executable.

| Built source asset | Dimensions | SHA-256 |
|---|---:|---|
| `sce_sys/icon0.png` | 128 × 128 | `49e7b7c0690a434493ecaa7142fdaa354f50ea96f060f14229700f346fe9c713` |
| `sce_sys/livearea/contents/bg.png` | 840 × 500 | `ee6ea7c90f80c4711520a409ec1f8f09b09884f446cc90ddc1c947aab67f8f24` |
| `sce_sys/livearea/contents/startup.png` | 280 × 158 | `33ae851f76d61c747fa61eca3fd262b473b49992b6b39cd69802a0dac9516e75` |
| `sce_sys/livearea/contents/template.xml` | XML | `0b6dd44cd163a5a0f9df8bd775f24b5cf47c0fffeac6495d08d5b4321935d8a4` |

The source PNGs and template are byte-for-byte identical to the
hardware-approved Test216D build's corresponding files. The LiveArea
template references the approved background and startup PNGs.
No user-ROM image data is separately packaged as a LiveArea resource.

The visual designs were iterated and approved during TNTV development.

See [PROVENANCE_REVIEW.md](PROVENANCE_REVIEW.md).
