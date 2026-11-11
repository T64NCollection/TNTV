#!/usr/bin/env python3
"""Offline, stdlib-only release hygiene gate for TNTV prepublication staging.

Checks an allowlisted source tree and optional Vita VPK for accidentally
included ROMs, music/scene caches, saves, build outputs and private paths.
It does NOT check copyright permission, adapted source-code rights, or whether
a compiled eboot includes transformed copyrighted material.
No network access, no ROM reading, no files modified.
"""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path
import sys
import zipfile

ROM_AND_CACHE_SUFFIXES = {
    ".z64", ".v64", ".n64", ".rom", ".pak", ".sav", ".srm",
    ".rgba", ".raw", ".wav", ".mp3", ".ogg", ".flac", ".mid",
    ".midi", ".m4a", ".spc", ".aiff", ".zip", ".vpk", ".self", ".velf", ".elf",
}
REQUIRED_DOCUMENTS = (
    "README.md", "THIRD_PARTY_NOTICES.md", "PROVENANCE_REVIEW.md",
    "UPSTREAM_SOURCES.md", ".gitignore", "CMakeLists.txt",
)
REQUIRED_UPSTREAM_NOTICES = (
    "romloader/third_party/lzokay/LICENSE",
    "romloader/third_party/crypto-algorithms/README.md",
    "third_party/libvita2d_shaders/LICENSE",
)
EXPECTED_OBJECTS = {
    "third_party/libvita2d_shaders/color_f_gxp.o",
    "third_party/libvita2d_shaders/color_v_gxp.o",
    "third_party/libvita2d_shaders/texture_f_gxp.o",
    "third_party/libvita2d_shaders/texture_v_gxp.o",
    "third_party/tntv_shaders/texedge_f_gxp.o",
}
EXPECTED_UI_FILES = {
    "sce_sys/icon0.png",
    "sce_sys/livearea/contents/bg.png",
    "sce_sys/livearea/contents/startup.png",
    "sce_sys/livearea/contents/template.xml",
}
EXPECTED_VPK_MEMBERS = {
    "sce_sys/param.sfo", "eboot.bin", *EXPECTED_UI_FILES,
}
OTHER_ALLOWED_SUFFIXES = {".c", ".cpp", ".h", ".hpp", ".inc", ".md", ".sh", ".py"}
BANNED_PARTS = {
    ".git", "checkpoints", "output", "saves", "cache", "__pycache__",
    ".vscode", ".idea", "CMakeFiles",
}
PRIVATE_BYTES = (
    b"/home/" + b"aryzona/", b"/mnt/c/" + b"Users/aryzo/", b"C:" + b"\\Users\\aryzo",
)


def scan_source(root: Path) -> tuple[list[str], int, int]:
    faults = []
    for rel in (*REQUIRED_DOCUMENTS, *REQUIRED_UPSTREAM_NOTICES):
        if not (root / rel).is_file():
            faults.append(f"Required file missing: {rel}")
    total_size = 0
    files = 0
    for p in root.rglob("*"):
        relative = p.relative_to(root)
        # In an ordinary cloned Git checkout .git is bookkeeping, not source.
        # The separate Git-index/archive audit enforces no .git in delivery.
        if relative.parts and relative.parts[0] == ".git":
            continue
        rp = relative.as_posix()
        if p.is_symlink():
            faults.append(f"Symlink not permitted: {rp}")
            continue
        if not p.is_file():
            continue
        files += 1
        total_size += p.stat().st_size
        if any(part in BANNED_PARTS or part.lower().startswith("build") for part in relative.parts):
            faults.append(f"Developer/build directory included: {rp}")
        if p.suffix.lower() in ROM_AND_CACHE_SUFFIXES:
            faults.append(f"ROM/cache/binary archive type included: {rp}")
        if p.suffix.lower() == ".o" and rp not in EXPECTED_OBJECTS:
            faults.append(f"Unexpected compiled object: {rp}")
        if p.suffix.lower() in {".png", ".xml"} and rp not in EXPECTED_UI_FILES:
            faults.append(f"Unreviewed UI asset: {rp}")
        if p.suffix.lower() == ".gxp" and rp != "third_party/tntv_shaders/texedge_f.gxp":
            faults.append(f"Unexpected shader bytecode: {rp}")
        if (
            rp not in EXPECTED_OBJECTS and rp not in EXPECTED_UI_FILES
            and p.name not in {"CMakeLists.txt", ".gitignore", "LICENSE"}
            and p.suffix.lower() not in OTHER_ALLOWED_SUFFIXES
            and rp != "third_party/tntv_shaders/texedge_f.gxp"
        ):
            faults.append(f"Unreviewed source file type: {rp}")
        if p.stat().st_size > 3_000_000:
            faults.append(f"Unexpectedly large source file: {rp}")
        if p.suffix.lower() in OTHER_ALLOWED_SUFFIXES or p.name in {"CMakeLists.txt", ".gitignore"}:
            content = p.read_bytes()
            for marker in PRIVATE_BYTES:
                if marker in content:
                    faults.append(f"Private developer path found: {rp}")
                    break
    return faults, files, total_size


def scan_vpk(vpk: Path) -> list[str]:
    faults = []
    with zipfile.ZipFile(vpk) as zf:
        if zf.testzip() is not None:
            faults.append("VPK ZIP CRC failure")
        entries = zf.namelist()
        names = set(entries)
        if len(entries) != len(names):
            faults.append("Duplicate VPK member names are not permitted")
        if names != EXPECTED_VPK_MEMBERS:
            faults.append(
                f"Unexpected VPK member set: missing={sorted(EXPECTED_VPK_MEMBERS - names)} "
                f"extra={sorted(names - EXPECTED_VPK_MEMBERS)}"
            )
        for name in names:
            if name.startswith("/") or ".." in Path(name).parts or "\\" in name:
                faults.append(f"Unsafe ZIP member path: {name}")
        print("VPK entries:", len(names), "bytes:", vpk.stat().st_size)
    return faults


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--source", type=Path, default=Path(__file__).resolve().parent.parent)
    ap.add_argument("--vpk", type=Path, default=None)
    args = ap.parse_args()
    root = args.source.resolve()
    if not root.is_dir():
        print("ERROR: source path not found", file=sys.stderr)
        return 2
    faults, n, size = scan_source(root)
    print("Source files:", n, "bytes:", size)
    if args.vpk:
        if not args.vpk.is_file():
            faults.append("VPK path not found")
        else:
            try:
                faults.extend(scan_vpk(args.vpk))
            except (OSError, zipfile.BadZipFile, RuntimeError, NotImplementedError) as exc:
                faults.append(f"Unreadable or invalid VPK archive: {exc}")
    if faults:
        print("RELEASE HYGIENE CHECK FAILED", len(faults), "problems")
        for fault in faults:
            print("  -", fault)
        return 1
    print("PASS: source and VPK hygiene allowlist verified.")
    print("NOT a copyright-permission or independent-code provenance clearance.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
