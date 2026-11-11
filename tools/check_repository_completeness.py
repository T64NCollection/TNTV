#!/usr/bin/env python3
"""Offline TNTV CMake/include/repository completeness verification.

A previous broad gitignore rule matched a required source .inc file.
This scanner walks the CMake sources and transitively resolves quoted
local includes. In a Git checkout, all required files MUST be Git-indexed.
In an exported ZIP, it verifies the actual required files exist.

No internet, ROM access, developer system paths, or file modification.
"""
from pathlib import Path
import argparse
import re
import subprocess
import sys

EXTS = {".c", ".cpp", ".h", ".hpp", ".inc"}
SOURCE_RE = re.compile(r"(?m)^\s*((?:src|romloader|third_party)/[-_a-zA-Z0-9./]+\.(?:c|cpp|o))\s*$")
INCLUDE_RE = re.compile(r'(?m)^\s*#\s*include\s+"([^"]+)"')
REQUIRED_METADATA = (
    "CMakeLists.txt", ".gitignore", "README.md", "INSTALLATION.md",
    "TROUBLESHOOTING.md", "RELEASE_NOTES.md",
    "THIRD_PARTY_NOTICES.md", "PROVENANCE_REVIEW.md",
    "UPSTREAM_SOURCES.md", "ART_ASSET_PROVENANCE.md",
)
REQUIRED_ART = (
    "sce_sys/icon0.png", "sce_sys/livearea/contents/bg.png",
    "sce_sys/livearea/contents/startup.png",
    "sce_sys/livearea/contents/template.xml",
)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=Path(__file__).resolve().parent.parent)
    args = parser.parse_args()
    root = args.source.resolve()
    cmake = root / "CMakeLists.txt"
    if not cmake.is_file():
        print("FAIL: no CMakeLists.txt")
        return 2
    content = cmake.read_text()
    match = re.search(r"add_executable\(\$\{PROJECT_NAME\}(.*?)\n\)", content, re.S)
    if not match:
        print("FAIL: cannot find project executable source block")
        return 2
    compiled = set(SOURCE_RE.findall(match.group(1)))
    if len(compiled) < 145:
        print("FAIL: only", len(compiled), "CMake executable inputs detected")
        return 1
    issues = []
    required = set(compiled) | set(REQUIRED_ART) | set(REQUIRED_METADATA)
    required |= {
        "third_party/tntv_shaders/texedge_f.gxp",
        "third_party/libvita2d_shaders/LICENSE",
        "romloader/third_party/lzokay/LICENSE",
        "romloader/third_party/crypto-algorithms/README.md",
    }
    todo = [name for name in compiled if Path(name).suffix in EXTS]
    visited = set()
    for name in list(required):
        if not (root / name).is_file():
            issues.append("Required compiled/source/metadata input missing: " + name)
    while todo:
        name = todo.pop()
        if name in visited:
            continue
        visited.add(name)
        f = root / name
        if not f.is_file():
            continue
        for header in INCLUDE_RE.findall(f.read_text(errors="replace")):
            paths = [
                f.parent / header, root / "include" / header,
                root / "romloader" / header, root / "src" / header,
                root / header,
            ]
            resolved = next((p for p in paths if p.is_file() and p.resolve().is_relative_to(root)), None)
            if resolved is None:
                issues.append(f"Unresolved local quoted include: {name} -> {header}")
                continue
            relative = resolved.resolve().relative_to(root).as_posix()
            if relative not in required:
                required.add(relative)
                if resolved.suffix in EXTS:
                    todo.append(relative)
    # No file obtained by scanning contains the original game ROM itself.
    if (root / ".git").exists():
        r = subprocess.run(["git", "ls-files", "--cached", "--full-name", "-z"],
                           cwd=root, capture_output=True)
        if r.returncode:
            issues.append("Could not inspect current Git index")
        else:
            tracked = {s.decode() for s in r.stdout.split(b"\0") if s}
            for name in required:
                if name not in tracked:
                    issues.append("Required source exists but IS NOT GIT-TRACKED: " + name)
            bad = [name for name in tracked if name.endswith((".z64",".v64",".n64",
                       ".vpk",".pak",".sav",".elf",".self"))]
            if bad:
                issues.append("Forbidden game/binary artifacts Git-tracked: "+repr(sorted(bad)))
            print("Git index:",len(tracked),"files; source/header dependencies tracked:",len(required))
    print("CMake inputs:", len(compiled), "resolved local source/include and metadata paths:", len(required))
    print("Important ROM-scene header tracked:", (root/"romloader/tntv_scene_cache.inc").is_file())
    if issues:
        for issue in sorted(set(issues))[:60]:
            print("FAIL:", issue)
        print("TOTAL ISSUES",len(issues))
        return 1
    print("PASS: every CMake input and transitively required local header is present")
    print("PASS: required files in Git index if checked from a local Git worktree")
    return 0


if __name__ == "__main__":
    sys.exit(main())
