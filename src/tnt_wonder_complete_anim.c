#include "tnt_wonder_complete_vita.h"

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

static void tnt_anim_load_first_key(const u8 *data, u32 key_off, f32 p[4]) {
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

static void tnt_anim_load_next_key(
    const u8 *data, u32 key_off, f32 p[4], u16 *mask_out
) {
    u16 mask = tnt_be16(data + key_off + 2);
    u16 scan = mask;
    u32 coeff_off = tnt_be32(data + key_off + 4);
    int i = 0;

    /* func_800A6E48 preserves leading finite-difference slots before the
     * first bit set in a new key, then replaces/zeros the remaining slots. */
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

static void tnt_anim_advance_difference(f32 p[4], u16 mask, u32 count) {
    f32 n, c2, c3;
    if (count == 0 || !(mask & 2U))
        return;
    n = (f32)count;
    c2 = (count >= 2U) ? n * (n - 1.0f) * 0.5f : 0.0f;
    c3 = (count >= 3U)
        ? n * (n - 1.0f) * (n - 2.0f) / 6.0f : 0.0f;

    /* Same forward-difference update order as func_800A6E48:
     * p0 += p1; p1 += p2; p2 += p3. */
    p[0] += n * p[1];
    if (mask & 4U)
        p[0] += c2 * p[2];
    if (mask & 8U)
        p[0] += c3 * p[3];
    if (mask & 4U) {
        p[1] += n * p[2];
        if (mask & 8U)
            p[1] += c2 * p[3];
    }
    if (mask & 8U)
        p[2] += n * p[3];
}

static f32 tnt_anim_track_value(const u8 *data, u32 track_off, u32 ticks) {
    u16 terminal = tnt_be16(data + track_off + 4);
    u32 key_off = tnt_be32(data + track_off + 12);
    u32 current_tick = 1;
    u32 current_key = key_off;
    u16 current_mask = tnt_be16(data + current_key + 2);
    f32 p[4];

    tnt_anim_load_first_key(data, current_key, p);
    if (terminal == 0)
        return p[0];

    /* A running Borg animation resets on the terminal update rather than
     * integrating it, so its visible period is exactly terminal ticks. */
    ticks %= terminal;
    if (ticks == 0)
        return p[0];

    while (current_tick <= ticks) {
        u32 next_key = current_key + 8U;
        u16 next_tick = tnt_be16(data + next_key);
        u32 stop_tick = ticks + 1U;

        if (next_tick == current_tick) {
            current_key = next_key;
            tnt_anim_load_next_key(data, current_key, p, &current_mask);
            tnt_anim_advance_difference(p, current_mask, 1U);
            current_tick++;
            continue;
        }
        if (next_tick > current_tick && next_tick < stop_tick)
            stop_tick = next_tick;
        if (stop_tick > current_tick) {
            u32 count = stop_tick - current_tick;
            tnt_anim_advance_difference(p, current_mask, count);
            current_tick += count;
        }
    }
    return p[0];
}

static void tnt_anim_apply_type1(
    const u8 *data, u32 ticks, f32 out_nodes[99][12]
) {
    u32 count = tnt_be32(data + 4);
    u32 list_off = tnt_be32(data + 8);
    u32 i;

    for (i = 0; i < count; i++) {
        const u8 *entry = data + list_off + i * 0x24U;
        u32 type = tnt_be32(entry + 0x0CU);
        u32 target = tnt_be32(entry + 0x10U);
        u32 bits = tnt_be32(entry + 0x14U);
        u32 track_count = tnt_be32(entry + 0x18U);
        u32 tracks_off = tnt_be32(entry + 0x1CU);
        u32 bit;
        u32 track = 0;

        if (type != 1U || target >= 99U)
            continue;
        for (bit = 0; bit < 12U && track < track_count; bit++) {
            if (bits & (1U << bit)) {
                out_nodes[target][bit] = tnt_anim_track_value(
                    data, tracks_off + track * 0x14U, ticks);
                track++;
            }
        }
    }
}

void TNT_Vita_MayanWonderCompleteCamera(
    u8 mode, u32 ticks, f32 *fovy, f32 *near_plane, f32 *far_plane, f32 *aspect
) {
    const u8 *data;
    u32 count, list_off, i;
    f32 fv = 66.6679992676f;
    f32 np = 0.0500000007f;
    f32 fp = 2000.0f;
    f32 ar = 1.3333333731f;

    if (mode != TNT_WONDER_COMPLETE_ANIM_FINALE) {
        if (fovy) *fovy = fv;
        if (near_plane) *near_plane = np;
        if (far_plane) *far_plane = fp;
        if (aspect) *aspect = ar;
        return;
    }
    if (ticks >= 1999U) ticks = 1998U;
    data = g_tnt_wonder_complete_anim_188;
    count = tnt_be32(data + 4);
    list_off = tnt_be32(data + 8);
    for (i = 0; i < count; i++) {
        const u8 *entry = data + list_off + i * 0x24U;
        u32 asset = tnt_be32(entry);
        u32 type = tnt_be32(entry + 0x0CU);
        u32 target = tnt_be32(entry + 0x10U);
        u32 bits = tnt_be32(entry + 0x14U);
        u32 track_count = tnt_be32(entry + 0x18U);
        u32 tracks_off = tnt_be32(entry + 0x1CU);
        u32 bit, track = 0;
        if (asset != 182U || type != 0U || target != 4U) continue;
        for (bit = 0; bit < 4U && track < track_count; bit++) {
            if (bits & (1U << bit)) {
                f32 v = tnt_anim_track_value(data, tracks_off + track * 0x14U, ticks);
                track++;
                if (bit == 0U) fv = v;
                else if (bit == 1U) np = v;
                else if (bit == 2U) fp = v;
            }
        }
        break;
    }
    if (fv < 1.0f) fv = 1.0f;
    if (fv > 170.0f) fv = 170.0f;
    if (np <= 0.0f) np = 0.0500000007f;
    if (fp <= np) fp = 2000.0f;
    if (fovy) *fovy = fv;
    if (near_plane) *near_plane = np;
    if (far_plane) *far_plane = fp;
    if (aspect) *aspect = ar;
}

void TNT_Vita_WonderCompleteBuildPose(
    u8 mode, u32 ticks, f32 out_nodes[99][12]
) {
    memcpy(out_nodes, g_tnt_wonder_complete_nodes,
           sizeof(f32) * 99U * 12U);

    switch (mode) {
    case TNT_WONDER_COMPLETE_ANIM_PRESENTATION:
        /* wonders1_80043c7c marks the completed-model wrapper for a forced
         * two-tick update and then stops it. */
        tnt_anim_apply_type1(g_tnt_wonder_complete_anim_189, 2U, out_nodes);
        break;
    case TNT_WONDER_COMPLETE_ANIM_HALL:
        tnt_anim_apply_type1(g_tnt_wonder_complete_anim_189, ticks, out_nodes);
        break;
    case TNT_WONDER_COMPLETE_ANIM_FINALE:
        /* Retail all-Wonders FINALE uses resource 188 on completed scene 186. */
        if (ticks >= 1999U) ticks = 1998U;
        tnt_anim_apply_type1(g_tnt_wonder_complete_anim_188, ticks, out_nodes);
        break;
    case TNT_WONDER_COMPLETE_ANIM_FINAL:
    default:
        /* Mayan's finished viewer runs 187 for 2749 ticks while 1396 advances
         * silently at weight zero. At the endpoint their weights swap, so the
         * already-running 1396 track becomes visible at its current phase. */
        if (ticks < 2749U)
            tnt_anim_apply_type1(g_tnt_wonder_complete_anim_187, ticks, out_nodes);
        else
            tnt_anim_apply_type1(g_tnt_wonder_complete_anim_1396, ticks, out_nodes);
        break;
    }
}
