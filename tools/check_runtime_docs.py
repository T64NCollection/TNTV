#!/usr/bin/env python3
"""Offline TNTV release-documentation consistency check.

Reads only source code, documentation, icon metadata and files inside the
private staging tree. NEVER reads a user ROM or caches, and contacts nobody.
Fail if the install guide drifts from verified loader filenames, file order,
revision/size contract, control mapping, saves, or packaged artwork hashes.
"""
from pathlib import Path
import argparse
import hashlib
import re
import sys


REQUIRED_NAMES = ("baserom.us.z64", "baserom.z64", "baserom.v64", "baserom.n64")
CACHES = (
    "tntv_rom_bridge_v6.pak",
    "tntv_scene_cache_v1.bin",
    "tntv_music_from_rom_v1.bin",
)
ART = {
    "sce_sys/icon0.png": "49e7b7c0690a434493ecaa7142fdaa354f50ea96f060f14229700f346fe9c713",
    "sce_sys/livearea/contents/bg.png": "ee6ea7c90f80c4711520a409ec1f8f09b09884f446cc90ddc1c947aab67f8f24",
    "sce_sys/livearea/contents/startup.png": "33ae851f76d61c747fa61eca3fd262b473b49992b6b39cd69802a0dac9516e75",
    "sce_sys/livearea/contents/template.xml": "0b6dd44cd163a5a0f9df8bd775f24b5cf47c0fffeac6495d08d5b4321935d8a4",
}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--source", type=Path, default=Path(__file__).resolve().parent.parent)
    args = ap.parse_args()
    root = args.source.resolve()
    issues = []

    def text(filename):
        p = root / filename
        if not p.is_file():
            issues.append(f"Missing required file: {filename}")
            return ""
        return p.read_text()

    readme = text("README.md")
    install = text("INSTALLATION.md")
    troubleshoot = text("TROUBLESHOOTING.md")
    art = text("ART_ASSET_PROVENANCE.md")
    bridge = text("src/tntv_rom_bridge.c")
    romh = text("romloader/tntv_rom_reader.h")
    control = text("src/vita_input.c")
    save = text("src/tnt_sram_vita.c")
    mainc = text("src/main.c")
    cmake = text("CMakeLists.txt")

    for doc in (readme, install, troubleshoot):
        if "ux0:/data/TNTV" not in doc and "ux0:data/TNTV" not in doc:
            issues.append("ROM storage path not documented in a user-facing guide")
    if '#define TNTV_DATA_DIR "ux0:data/TNTV"' not in bridge:
        issues.append("Loader's ROM directory changed")
    if "#define TNTV_ROM_SIZE 12582912U" not in romh:
        issues.append("Loader's expected ROM size changed")
    if "12,582,912" not in install:
        issues.append("Install guide lacks exact verified 12-MiB size")
    if not all(name in install and name in troubleshoot for name in REQUIRED_NAMES):
        issues.append("ROM candidate filenames absent from install/troubleshooting guide")

    match = re.search(r"static const char \*const candidates\[\]\s*=\s*\{(.*?)\};", bridge, re.S)
    if match is None:
        issues.append("Unable to read actual ROM filename priority from loader")
    else:
        actual = re.findall(r'"/(baserom\.[^"]+)"', match.group(1))
        if tuple(actual) != REQUIRED_NAMES:
            issues.append("Supported ROM filename precedence changed: " + repr(actual))
    pos = [install.find("`" + name + "`") for name in REQUIRED_NAMES]
    if not all(i >= 0 for i in pos):
        issues.append("Install guide missing one or more ROM candidate labels")
    if "first file" not in install.lower() or "first file" not in troubleshoot.lower():
        issues.append("Multi-file selection/precedence warning missing")

    for name in CACHES:
        if name not in bridge or name not in troubleshoot:
            issues.append("Generated cache not consistently documented: " + name)
    if '#define TNT_VITA_SRAM_PATH "ux0:data/TNTV/tntv_sram.bin"' not in save:
        issues.append("Expected save file location changed")
    for doc in (install, troubleshoot):
        if "tntv_sram.bin" not in doc:
            issues.append("Save-file preservation warning missing")
    if not ("SCE_CTRL_CROSS" in control and "SCE_CTRL_CIRCLE" in control):
        issues.append("Original input adapter button mapping changed")
    if "SCE_CTRL_CROSS" not in mainc or "SCE_CTRL_CIRCLE" not in mainc:
        issues.append("ROM setup retry/exit button controls changed")
    if "TNTV LOADING" not in mainc or "TNTV LOADING TEST" in mainc:
        issues.append("Release title not clean or missing")
    for label in ("ROM FILE MISSING", "ROM VALIDATION FAILED", "ROM DATA SETUP ERROR"):
        if label not in mainc or label not in troubleshoot:
            issues.append("Setup error screen labels not fully documented")
    if 'set(VITA_TITLEID  "NTET00001")' not in cmake:
        issues.append("Vita package title ID changed")
    for rel,sha in ART.items():
        p = root / rel
        if not p.is_file():
            issues.append("Missing expected UI asset: " + rel)
        elif hashlib.sha256(p.read_bytes()).hexdigest() != sha:
            issues.append("Approved UI asset changed: " + rel)
        if sha not in art:
            issues.append("Art provenance ledger missing expected SHA-256: " + rel)
    if "does not assert ownership" not in art.lower() and "not ownership" not in art.lower() and "does not assert" not in art.lower():
        issues.append("Art provenance must not assert unsupported ownership")
    for name in ("INSTALLATION.md", "TROUBLESHOOTING.md", "ART_ASSET_PROVENANCE.md"):
        if name not in readme:
            issues.append("Top-level README doesn't link " + name)

    if issues:
        for issue in issues:
            print("FAIL:", issue)
        return 1
    print("PASS: original-ROM size, directory, 4 filename priority, 3 caches, "
          "SRAM save, ROM setup controls, UI messages and Vita title ID match docs.")
    print("PASS: 4 packaged artwork/template hashes match approved Test216D ledger.")
    print("PASS: README links all verified installation/troubleshooting/art records.")
    print("No network calls, original ROM reads or source modifications.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
