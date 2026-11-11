#!/usr/bin/env bash
# Rebuild custom TNTV GXM shader ELF wrapper from the existing .gxp bytecode.
# Does NOT regenerate .gxp from a high-level shader source.
# No ROM, asset cache, or network access.
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
shader_dir="$root/third_party/tntv_shaders"
objcopy="${VITASDK:-/usr/local/vitasdk}/bin/arm-vita-eabi-objcopy"
if [[ ! -x "$objcopy" ]]; then
    objcopy="$(command -v arm-vita-eabi-objcopy || true)"
fi
if [[ -z "$objcopy" || ! -x "$objcopy" ]]; then
    printf 'ERROR: VitaSDK arm-vita-eabi-objcopy is required.\n' >&2
    exit 2
fi
input="$shader_dir/texedge_f.gxp"
output="$shader_dir/texedge_f_gxp.o"
if [[ ! -f "$input" ]]; then
    printf 'ERROR: missing precompiled shader %s\n' "$input" >&2
    exit 3
fi
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
(
    cd "$shader_dir"
    "$objcopy" -I binary -O elf32-littlearm -B arm texedge_f.gxp "$tmp/shader_raw.o"
)
"$objcopy" \
    --redefine-sym _binary_texedge_f_gxp_start=tntv_texedge_f_gxp_start \
    --redefine-sym _binary_texedge_f_gxp_end=tntv_texedge_f_gxp_end \
    --redefine-sym _binary_texedge_f_gxp_size=tntv_texedge_f_gxp_size \
    "$tmp/shader_raw.o" "$tmp/shader_names.o"
"$objcopy" --set-section-alignment .data=4 "$tmp/shader_names.o" "$tmp/shader_final.o"
expected="5b5cd354e30b73d49caa09c4aa7b34515dbbe207c4195c5dcbab4ddd14a26208"
actual="$(sha256sum "$tmp/shader_final.o" | cut -d' ' -f1)"
if [[ "$actual" != "$expected" ]]; then
    printf 'ERROR: rebuilt shader object differs from audited version. Expected: %s Actual: %s\n' "$expected" "$actual" >&2
    exit 4
fi
cp "$tmp/shader_final.o" "$output"
printf 'PASS: exact TNTV shader object reproduced from .gxp (%s)\n' "$actual"
