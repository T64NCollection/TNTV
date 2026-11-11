#include "tnt_intro_vita.h"

#include <string.h>

typedef struct {
    float state[4];
    uint32_t key;
    uint16_t tick;
} TNTIntroTrackRuntime;

static TNTIntroPose s_pose;
static TNTIntroTrackRuntime s_track[TNT_INTRO_TRACK_COUNT];

static void intro_track_load_first(uint32_t i) {
    const TNTIntroTrackDef *td = &g_tnt_intro_tracks[i];
    const TNTIntroKey *key = &g_tnt_intro_keys[td->first_key];
    TNTIntroTrackRuntime *rt = &s_track[i];
    uint32_t c;

    rt->key = 0;
    rt->tick = 1;
    for (c = 0; c < 4; c++)
        rt->state[c] = (key->mask & (1U << c)) ? key->v[c] : 0.0f;
}

static void intro_track_advance(uint32_t i) {
    const TNTIntroTrackDef *td = &g_tnt_intro_tracks[i];
    TNTIntroTrackRuntime *rt = &s_track[i];
    const TNTIntroKey *key;
    uint16_t mask;
    uint32_t c;

    if (rt->tick == td->end_tick) {
        intro_track_load_first(i);
        return;
    }

    if (rt->key + 1U < td->key_count) {
        const TNTIntroKey *next =
            &g_tnt_intro_keys[td->first_key + rt->key + 1U];
        if ((uint16_t)next->tick == rt->tick) {
            uint32_t first = 4;
            rt->key++;
            for (c = 0; c < 4; c++) {
                if (next->mask & (1U << c)) {
                    first = c;
                    break;
                }
            }
            for (c = first; c < 4; c++)
                rt->state[c] = (next->mask & (1U << c)) ? next->v[c] : 0.0f;
        }
    }

    key = &g_tnt_intro_keys[td->first_key + rt->key];
    mask = key->mask;
    if (mask & 0x2U) rt->state[0] += rt->state[1];
    if (mask & 0x4U) rt->state[1] += rt->state[2];
    if (mask & 0x8U) rt->state[2] += rt->state[3];
    rt->tick++;
}

static void intro_apply_targets(void) {
    uint32_t i;
    for (i = 0; i < TNT_INTRO_TARGET_COUNT; i++) {
        const TNTIntroTargetDef *tg = &g_tnt_intro_targets[i];
        uint32_t track = tg->first_track;
        uint32_t bit;
        for (bit = 0; bit < 16; bit++) {
            if (tg->bits & (1U << bit)) {
                float value = s_track[track++].state[0];
                switch (tg->kind) {
                    case 0: /* scene node */
                        if (tg->index < TNT_INTRO_NODE_COUNT && bit < 9)
                            s_pose.node[tg->index][bit] = value;
                        break;
                    case 1: /* fog RGB */
                        if (bit < 3) s_pose.fog[2 + bit] = value;
                        break;
                    case 2: /* camera fovy */
                        if (bit == 0) s_pose.camera[0] = value;
                        break;
                    case 3: /* light RGB */
                        if (tg->index < 3 && bit < 3)
                            s_pose.light[tg->index][1 + bit] = value;
                        break;
                    case 4: /* material RGB base + bit 3 transparency */
                        if (tg->index < TNT_INTRO_DRAW_COUNT && bit == 3)
                            s_pose.draw_transparency[tg->index] = value;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void TNT_Vita_IntroReset(void) {
    uint32_t i,j;
    memset(&s_pose,0,sizeof(s_pose));
    for (i = 0; i < TNT_INTRO_NODE_COUNT; i++)
        for (j = 0; j < 9; j++)
            s_pose.node[i][j] = g_tnt_intro_nodes[i].v[j];
    memcpy(s_pose.fog,g_tnt_intro_fog_initial,sizeof(s_pose.fog));
    memcpy(s_pose.camera,g_tnt_intro_camera_initial,sizeof(s_pose.camera));
    memcpy(s_pose.light,g_tnt_intro_lights_initial,sizeof(s_pose.light));
    for (i = 0; i < TNT_INTRO_DRAW_COUNT; i++)
        s_pose.draw_transparency[i] = g_tnt_intro_draws[i].transparency;
    for (i = 0; i < TNT_INTRO_TRACK_COUNT; i++)
        intro_track_load_first(i);
    s_pose.tick = 1;
}

void TNT_Vita_IntroAdvance(uint32_t ticks) {
    uint32_t step,i;
    for (step = 0; step < ticks; step++) {
        for (i = 0; i < TNT_INTRO_TRACK_COUNT; i++)
            intro_track_advance(i);
        s_pose.tick++;
    }
    intro_apply_targets();
}

uint32_t TNT_Vita_IntroTick(void) {
    return s_pose.tick;
}

const TNTIntroPose *TNT_Vita_IntroPose(void) {
    return &s_pose;
}
