#include "tnt_greek_gameplay_vita.h"

#include <string.h>

static u16 tnt_be16(const u8 *p) {
    return (u16)(((u16)p[0] << 8) | p[1]);
}

static u32 tnt_be32(const u8 *p) {
    return ((u32)p[0] << 24) | ((u32)p[1] << 16) |
           ((u32)p[2] << 8) | (u32)p[3];
}

static f32 tnt_bef32(const u8 *p) {
    union { u32 u; f32 f; } v;
    v.u = tnt_be32(p);
    return v.f;
}

static void load_first_key(const u8 *data, u32 key_off, f32 p[4]) {
    u16 mask = tnt_be16(data + key_off + 2);
    u32 coeff_off = tnt_be32(data + key_off + 4);
    int i;
    memset(p, 0, sizeof(f32) * 4);
    for (i = 0; i < 4 && mask != 0; i++, mask >>= 1) {
        if (mask & 1U) {
            p[i] = tnt_bef32(data + coeff_off);
            coeff_off += 4;
        }
    }
}

static void load_next_key(
    const u8 *data, u32 key_off, f32 p[4], u16 *mask_out
) {
    u16 mask = tnt_be16(data + key_off + 2);
    u16 scan = mask;
    u32 coeff_off = tnt_be32(data + key_off + 4);
    int i = 0;

    while (i < 4 && !(scan & 1U)) {
        i++;
        scan >>= 1;
    }
    for (; i < 4; i++, scan >>= 1) {
        if (scan & 1U) {
            p[i] = tnt_bef32(data + coeff_off);
            coeff_off += 4;
        } else {
            p[i] = 0.0f;
        }
    }
    *mask_out = mask;
}

static void advance_difference(f32 p[4], u16 mask, u32 count) {
    f32 n, c2, c3;
    if (count == 0 || !(mask & 2U))
        return;
    n = (f32)count;
    c2 = n * (n - 1.0f) * 0.5f;
    c3 = n * (n - 1.0f) * (n - 2.0f) / 6.0f;
    p[0] += n * p[1];
    if (mask & 4U) p[0] += c2 * p[2];
    if (mask & 8U) p[0] += c3 * p[3];
    if (mask & 4U) {
        p[1] += n * p[2];
        if (mask & 8U) p[1] += c2 * p[3];
    }
    if (mask & 8U) p[2] += n * p[3];
}

static f32 track_value(const u8 *data, u32 track_off, u32 ticks) {
    u16 terminal = tnt_be16(data + track_off + 4);
    u32 key_off = tnt_be32(data + track_off + 12);
    u32 current_tick = 1;
    u32 current_key = key_off;
    u16 current_mask = tnt_be16(data + current_key + 2);
    f32 p[4];

    load_first_key(data, current_key, p);
    if (terminal == 0)
        return p[0];
    ticks %= terminal;
    if (ticks == 0)
        return p[0];

    while (current_tick <= ticks) {
        u32 next_key = current_key + 8U;
        u16 next_tick = tnt_be16(data + next_key);
        u32 stop_tick = ticks + 1U;
        if (next_tick == current_tick) {
            current_key = next_key;
            load_next_key(data, current_key, p, &current_mask);
            advance_difference(p, current_mask, 1U);
            current_tick++;
            continue;
        }
        if (next_tick > current_tick && next_tick < stop_tick)
            stop_tick = next_tick;
        if (stop_tick > current_tick) {
            u32 count = stop_tick - current_tick;
            advance_difference(p, current_mask, count);
            current_tick += count;
        }
    }
    return p[0];
}

void TNT_Vita_GreekRainBuildPose(
    u32 ticks, f32 out_nodes[8][9], u8 *material_alpha
) {
    const u8 *data = g_tnt_greek_rain_anim_1319;
    u32 count = tnt_be32(data + 4);
    u32 list_off = tnt_be32(data + 8);
    u32 i;
    f32 material_transparency = 0.5f;

    memcpy(out_nodes, g_tnt_greek_rain_nodes, sizeof(f32) * 8U * 9U);

    for (i = 0; i < count; i++) {
        const u8 *entry = data + list_off + i * 0x24U;
        u32 type = tnt_be32(entry + 0x0CU);
        u32 target = tnt_be32(entry + 0x10U);
        u32 bits = tnt_be32(entry + 0x14U);
        u32 track_count = tnt_be32(entry + 0x18U);
        u32 tracks_off = tnt_be32(entry + 0x1CU);
        u32 bit, track = 0;

        if (type == 1U && target < 8U) {
            for (bit = 0; bit < 9U && track < track_count; bit++) {
                if (bits & (1U << bit)) {
                    out_nodes[target][bit] = track_value(
                        data, tracks_off + track * 0x14U, ticks);
                    track++;
                }
            }
        } else if (type == 2U && target == 0U && (bits & 0x8U) && track_count) {
            /* Entry 6 targets material 0 of mesh 1315 and animates its fourth
             * material float (unk10). Borg converts that to alpha as
             * (1 - unk10) * 255. */
            material_transparency = track_value(data, tracks_off, ticks);
        }
    }

    if (material_transparency < 0.0f) material_transparency = 0.0f;
    if (material_transparency > 1.0f) material_transparency = 1.0f;
    if (material_alpha) {
        *material_alpha = (u8)((1.0f - material_transparency) * 255.0f + 0.5f);
    }
}
