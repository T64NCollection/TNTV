#include "vita_compat.h"
#include "tnt_game_types.h"
#include "tnt_audio_vita.h"
#include "tnt_sram_vita.h"

#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr/thread.h>
#include <psp2/kernel/threadmgr/mutex.h>
#include <psp2/io/fcntl.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define TNT_AUDIO_RATE          48000
#define TNT_AUDIO_BLOCK         512
#define TNT_DCM_OUTPUT_RATE     36007
#define TNT_DCM_TICK_FRAMES     (TNT_AUDIO_RATE / 50)
#define TNT_AUDIO_SLOT_COUNT    8
#define TNT_MENU_SFX_COUNT      6
#define TNT_MENU_DCM_COUNT      6
#define TNT_MENU_SAMPLE_COUNT   4
#define TNT_GAME_SFX_COUNT      22
#define TNT_GAME_DCM_COUNT      31
#define TNT_GAME_SAMPLE_COUNT   18
#define TNT_INTRO_SFX_COUNT     12
#define TNT_INTRO_DCM_COUNT     28
#define TNT_INTRO_SAMPLE_COUNT  15
#define TNT_GAME_MAX_DCM_SAMPLES 4
#define TNT_INTRO_MAX_DCM_SAMPLES 8
#define TNT_MUSIC_CHANNEL_COUNT  16
#define TNT_TITLE_SAMPLE_COUNT   21
#define TNT_MUSIC_MAX_SAMPLES     64
#define TNT_MUSIC_PACK_SONGS      16

extern u8 g_tnt_menu_sfx_stream_0[18];
extern u8 g_tnt_menu_sfx_stream_1[28];
extern u8 g_tnt_menu_sfx_stream_2[23];
extern u8 g_tnt_menu_sfx_stream_3[22];
extern u8 g_tnt_menu_sfx_stream_4[21];
extern u8 g_tnt_menu_sfx_stream_5[38];
extern u32 g_tnt_menu_sfx_stream_lengths[6];
extern u8 g_tnt_menu_sample_138[15094];
extern u8 g_tnt_menu_sample_139[2514];
extern u8 g_tnt_menu_sample_13a[5642];
extern u8 g_tnt_menu_sample_13b[2394];
extern u32 g_tnt_menu_sample_lengths[4];

extern const u8 *const g_tnt_game_sfx_streams[TNT_GAME_DCM_COUNT];
extern u32 g_tnt_game_sfx_stream_lengths[TNT_GAME_DCM_COUNT];
extern const u8 *const g_tnt_game_samples[TNT_GAME_SAMPLE_COUNT];
extern u32 g_tnt_game_sample_lengths[TNT_GAME_SAMPLE_COUNT];
extern u32 g_tnt_game_sample_loop_begin[TNT_GAME_SAMPLE_COUNT];
extern u32 g_tnt_game_sample_loop_end[TNT_GAME_SAMPLE_COUNT];
extern u8 g_tnt_game_sample_flags[TNT_GAME_SAMPLE_COUNT];
extern s8 g_tnt_game_dcm_sample_map[TNT_GAME_DCM_COUNT][TNT_GAME_MAX_DCM_SAMPLES];

extern const u8 *const g_tnt_intro_sfx_streams[TNT_INTRO_DCM_COUNT];
extern u32 g_tnt_intro_sfx_stream_lengths[TNT_INTRO_DCM_COUNT];
extern const u8 *const g_tnt_intro_samples[TNT_INTRO_SAMPLE_COUNT];
extern u32 g_tnt_intro_sample_lengths[TNT_INTRO_SAMPLE_COUNT];
extern u32 g_tnt_intro_sample_loop_begin[TNT_INTRO_SAMPLE_COUNT];
extern u32 g_tnt_intro_sample_loop_end[TNT_INTRO_SAMPLE_COUNT];
extern u8 g_tnt_intro_sample_flags[TNT_INTRO_SAMPLE_COUNT];
extern s8 g_tnt_intro_dcm_sample_map[TNT_INTRO_DCM_COUNT][TNT_INTRO_MAX_DCM_SAMPLES];

/* Public audio objects retained from the original interface. */
SfxPlayer D_801235B0;
SfxBank g_menuSfxBank;
SfxBank g_gameSfxBank;
SfxBank g_introSfxBank;

static s16 s_eqpower[128] = {
    0x7fff,0x7ffc,0x7ff5,0x7fe8,0x7fd7,0x7fc0,0x7fa5,0x7f84,
    0x7f5f,0x7f34,0x7f05,0x7ed0,0x7e97,0x7e58,0x7e15,0x7dcd,
    0x7d7f,0x7d2d,0x7cd6,0x7c7a,0x7c1a,0x7bb4,0x7b49,0x7ada,
    0x7a66,0x79ed,0x796f,0x78ed,0x7866,0x77da,0x7749,0x76b4,
    0x761a,0x757b,0x74d8,0x7430,0x7384,0x72d3,0x721e,0x7164,
    0x70a6,0x6fe3,0x6f1c,0x6e51,0x6d81,0x6cad,0x6bd5,0x6af9,
    0x6a18,0x6933,0x684a,0x675d,0x666c,0x6577,0x647e,0x6381,
    0x6280,0x617c,0x6073,0x5f67,0x5e57,0x5d43,0x5c2c,0x5b11,
    0x59f2,0x58d0,0x57aa,0x5681,0x5555,0x5425,0x52f2,0x51bc,
    0x5082,0x4f46,0x4e06,0x4cc3,0x4b7d,0x4a35,0x48e9,0x479b,
    0x4649,0x44f5,0x439e,0x4245,0x40e9,0x3f8a,0x3e29,0x3cc6,
    0x3b60,0x39f8,0x388d,0x3721,0x35b2,0x3441,0x32ce,0x3159,
    0x2fe2,0x2e69,0x2cef,0x2b72,0x29f4,0x2875,0x26f3,0x2570,
    0x23ec,0x2266,0x20df,0x1f57,0x1dce,0x1c43,0x1ab7,0x192a,
    0x179c,0x160e,0x147e,0x12ed,0x115c,0x0fca,0x0e38,0x0ca5,
    0x0b11,0x097d,0x07e9,0x0654,0x04c0,0x032a,0x0195,0x0000
};

static const u8 *const s_menu_streams[TNT_MENU_DCM_COUNT] = {
    g_tnt_menu_sfx_stream_0, g_tnt_menu_sfx_stream_1,
    g_tnt_menu_sfx_stream_2, g_tnt_menu_sfx_stream_3,
    g_tnt_menu_sfx_stream_4, g_tnt_menu_sfx_stream_5
};

static const s8 *const s_menu_samples[TNT_MENU_SAMPLE_COUNT] = {
    (const s8 *)g_tnt_menu_sample_138,
    (const s8 *)g_tnt_menu_sample_139,
    (const s8 *)g_tnt_menu_sample_13a,
    (const s8 *)g_tnt_menu_sample_13b
};

static const s8 s_menu_dcm_sample_map[TNT_MENU_DCM_COUNT][TNT_GAME_MAX_DCM_SAMPLES] = {
    {0, -1, -1, -1},
    {1, -1, -1, -1},
    {2, -1, -1, -1},
    {2, -1, -1, -1},
    {2, -1, -1, -1},
    {2,  3, -1, -1}
};

static const u32 s_freq_base[4] = { 0, 0x4000, 0xC000, 0x1C000 };

typedef enum {
    TNT_SFX_BANK_NONE = 0,
    TNT_SFX_BANK_MENU = 1,
    TNT_SFX_BANK_GAME = 2,
    TNT_SFX_BANK_INTRO = 3
} TNTSfxBankKind;

typedef struct {
    u8 layer_count;
    u8 priority;
    u8 dcm_index[4];
} TNTSfxDefinition;

static const TNTSfxDefinition s_menu_sfx_defs[TNT_MENU_SFX_COUNT] = {
    {1, 12, {0, 0, 0, 0}},
    {1, 13, {1, 0, 0, 0}},
    {1, 14, {2, 0, 0, 0}},
    {1, 15, {3, 0, 0, 0}},
    {1, 16, {4, 0, 0, 0}},
    {1, 17, {5, 0, 0, 0}}
};

/* Exact D_800D399C layer count / priority / DCM-index table. */
static const TNTSfxDefinition s_game_sfx_defs[TNT_GAME_SFX_COUNT] = {
    {1,  2, { 0,  0,  0,  0}},
    {1,  4, { 1,  0,  0,  0}},
    {2, 30, { 3,  2,  0,  0}},
    {3, 20, { 4,  5,  6,  0}},
    {1, 20, { 7,  0,  0,  0}},
    {1,  9, { 8,  0,  0,  0}},
    {1,  8, { 9,  0,  0,  0}},
    {1,  7, {10,  0,  0,  0}},
    {1, 25, {11,  0,  0,  0}},
    {1, 12, {12,  0,  0,  0}},
    {1, 12, {13,  0,  0,  0}},
    {1, 12, {14,  0,  0,  0}},
    {1, 12, {15,  0,  0,  0}},
    {1,  4, {16,  0,  0,  0}},
    {1,  7, {17,  0,  0,  0}},
    {1, 25, {18,  0,  0,  0}},
    {1, 25, {19,  0,  0,  0}},
    {1, 25, {20,  0,  0,  0}},
    {1, 25, {21,  0,  0,  0}},
    {1, 25, {22,  0,  0,  0}},
    {4, 25, {23, 24, 25, 26}},
    {4, 25, {27, 28, 29, 30}}
};

/* Exact D_800D3AA4 layer count / priority / DCM-index table. */
static const TNTSfxDefinition s_intro_sfx_defs[TNT_INTRO_SFX_COUNT] = {
    {3, 30, { 0,  1,  2,  0}},
    {2, 29, { 3,  4,  0,  0}},
    {2, 28, { 5,  6,  0,  0}},
    {2, 27, { 7,  8,  0,  0}},
    {2, 26, { 9, 10,  0,  0}},
    {4, 25, {11, 12, 13, 14}},
    {3, 24, {15, 16, 17,  0}},
    {2, 23, {18, 19,  0,  0}},
    {1, 22, {20,  0,  0,  0}},
    {3, 21, {21, 22, 23,  0}},
    {2, 20, {24, 25,  0,  0}},
    {2, 19, {26, 27,  0,  0}}
};

typedef struct {
    f32 pitch;
    u8 volume_index;
    u8 pan;
    s8 sample_index;
    u32 sample_offset;
    u32 phase_q16;
    u32 step_q16;
    s32 gain_l;
    s32 gain_r;
    u8 active;
} TNTDcmVoice;

typedef struct {
    const u8 *stream;
    u32 stream_len;
    u32 stream_pos;
    u32 age;
    u8 delay;
    u8 active;
    u8 restart;
    u8 bank_kind;
    u8 sfx_id;
    u8 dcm_index;
    u8 priority;
    TNTDcmVoice voice[2];
} TNTDcmSfxSlot;

static TNTDcmSfxSlot s_slots[TNT_AUDIO_SLOT_COUNT];
static SceUID s_audio_mutex = -1;
static SceUID s_audio_thread = -1;
static int s_audio_port = -1;
static volatile u8 s_audio_running;
static u32 s_frames_to_tick;
static u32 s_sfx_tick_period_frames = TNT_DCM_TICK_FRAMES;
static u32 s_slot_age = 1;
static u8 s_current_bank = TNT_SFX_BANK_NONE;

typedef struct {
    f32 pitch;
    u8 volume_index;
    u8 pan;
    u8 selected_sample;
    u8 playing_sample;
    u32 sample_offset;
    u32 phase_q16;
    u32 step_q16;
    s32 gain_l;
    s32 gain_r;
    u8 active;
} TNTDcmMusicVoice;

typedef struct {
    const u8 *stream;
    u32 stream_len;
    const u8 *sample[TNT_MUSIC_MAX_SAMPLES];
    u32 sample_len[TNT_MUSIC_MAX_SAMPLES];
    u32 loop_begin[TNT_MUSIC_MAX_SAMPLES];
    u32 loop_end[TNT_MUSIC_MAX_SAMPLES];
    u8 flags[TNT_MUSIC_MAX_SAMPLES];
    u8 channels;
    u8 sample_count;
} TNTMusicTrack;

static TNTDcmMusicVoice s_music_voice[TNT_MUSIC_CHANNEL_COUNT];
static TNTMusicTrack s_music_track;
static u8 *s_music_pack;
static u32 s_music_pack_size;
static u32 s_music_stream_pos;
static u8 s_music_delay;
static u8 s_music_active;
static u8 s_current_song;
static s64 s_music_frames_to_tick_q16;
static u32 s_music_tick_period_q16 = (TNT_DCM_TICK_FRAMES << 16);
static u32 s_current_sfx_level;
static u32 s_current_music_level;

static const TNTSfxDefinition *tnt_effect_def(u8 bank_kind, u8 sfx_id) {
    if (bank_kind == TNT_SFX_BANK_MENU && sfx_id < TNT_MENU_SFX_COUNT)
        return &s_menu_sfx_defs[sfx_id];
    if (bank_kind == TNT_SFX_BANK_GAME && sfx_id < TNT_GAME_SFX_COUNT)
        return &s_game_sfx_defs[sfx_id];
    if (bank_kind == TNT_SFX_BANK_INTRO && sfx_id < TNT_INTRO_SFX_COUNT)
        return &s_intro_sfx_defs[sfx_id];
    return NULL;
}

static const u8 *tnt_bank_stream(u8 bank_kind, u8 dcm_index) {
    if (bank_kind == TNT_SFX_BANK_MENU && dcm_index < TNT_MENU_DCM_COUNT)
        return s_menu_streams[dcm_index];
    if (bank_kind == TNT_SFX_BANK_GAME && dcm_index < TNT_GAME_DCM_COUNT)
        return g_tnt_game_sfx_streams[dcm_index];
    if (bank_kind == TNT_SFX_BANK_INTRO && dcm_index < TNT_INTRO_DCM_COUNT)
        return g_tnt_intro_sfx_streams[dcm_index];
    return NULL;
}

static u32 tnt_bank_stream_len(u8 bank_kind, u8 dcm_index) {
    if (bank_kind == TNT_SFX_BANK_MENU && dcm_index < TNT_MENU_DCM_COUNT)
        return g_tnt_menu_sfx_stream_lengths[dcm_index];
    if (bank_kind == TNT_SFX_BANK_GAME && dcm_index < TNT_GAME_DCM_COUNT)
        return g_tnt_game_sfx_stream_lengths[dcm_index];
    if (bank_kind == TNT_SFX_BANK_INTRO && dcm_index < TNT_INTRO_DCM_COUNT)
        return g_tnt_intro_sfx_stream_lengths[dcm_index];
    return 0;
}

static s8 tnt_bank_sample_map(u8 bank_kind, u8 dcm_index, u8 local_index) {
    if (bank_kind == TNT_SFX_BANK_MENU && dcm_index < TNT_MENU_DCM_COUNT &&
        local_index < TNT_GAME_MAX_DCM_SAMPLES)
        return s_menu_dcm_sample_map[dcm_index][local_index];
    if (bank_kind == TNT_SFX_BANK_GAME && dcm_index < TNT_GAME_DCM_COUNT &&
        local_index < TNT_GAME_MAX_DCM_SAMPLES)
        return g_tnt_game_dcm_sample_map[dcm_index][local_index];
    if (bank_kind == TNT_SFX_BANK_INTRO && dcm_index < TNT_INTRO_DCM_COUNT &&
        local_index < TNT_INTRO_MAX_DCM_SAMPLES)
        return g_tnt_intro_dcm_sample_map[dcm_index][local_index];
    return -1;
}

static const s8 *tnt_bank_sample(u8 bank_kind, s8 sample_index) {
    if (bank_kind == TNT_SFX_BANK_MENU && sample_index >= 0 && sample_index < TNT_MENU_SAMPLE_COUNT)
        return s_menu_samples[(u8)sample_index];
    if (bank_kind == TNT_SFX_BANK_GAME && sample_index >= 0 && sample_index < TNT_GAME_SAMPLE_COUNT)
        return (const s8 *)g_tnt_game_samples[(u8)sample_index];
    if (bank_kind == TNT_SFX_BANK_INTRO && sample_index >= 0 && sample_index < TNT_INTRO_SAMPLE_COUNT)
        return (const s8 *)g_tnt_intro_samples[(u8)sample_index];
    return NULL;
}

static u32 tnt_bank_sample_len(u8 bank_kind, s8 sample_index) {
    if (bank_kind == TNT_SFX_BANK_MENU && sample_index >= 0 && sample_index < TNT_MENU_SAMPLE_COUNT)
        return g_tnt_menu_sample_lengths[(u8)sample_index];
    if (bank_kind == TNT_SFX_BANK_GAME && sample_index >= 0 && sample_index < TNT_GAME_SAMPLE_COUNT)
        return g_tnt_game_sample_lengths[(u8)sample_index];
    if (bank_kind == TNT_SFX_BANK_INTRO && sample_index >= 0 && sample_index < TNT_INTRO_SAMPLE_COUNT)
        return g_tnt_intro_sample_lengths[(u8)sample_index];
    return 0;
}

static u8 tnt_bank_sample_flags(u8 bank_kind, s8 sample_index) {
    if (bank_kind == TNT_SFX_BANK_GAME && sample_index >= 0 && sample_index < TNT_GAME_SAMPLE_COUNT)
        return g_tnt_game_sample_flags[(u8)sample_index];
    if (bank_kind == TNT_SFX_BANK_INTRO && sample_index >= 0 && sample_index < TNT_INTRO_SAMPLE_COUNT)
        return g_tnt_intro_sample_flags[(u8)sample_index];
    return 0x02;
}

static u32 tnt_bank_loop_begin(u8 bank_kind, s8 sample_index) {
    if (bank_kind == TNT_SFX_BANK_GAME && sample_index >= 0 && sample_index < TNT_GAME_SAMPLE_COUNT)
        return g_tnt_game_sample_loop_begin[(u8)sample_index];
    if (bank_kind == TNT_SFX_BANK_INTRO && sample_index >= 0 && sample_index < TNT_INTRO_SAMPLE_COUNT)
        return g_tnt_intro_sample_loop_begin[(u8)sample_index];
    return 0;
}

static u32 tnt_bank_loop_end(u8 bank_kind, s8 sample_index) {
    if (bank_kind == TNT_SFX_BANK_GAME && sample_index >= 0 && sample_index < TNT_GAME_SAMPLE_COUNT)
        return g_tnt_game_sample_loop_end[(u8)sample_index];
    if (bank_kind == TNT_SFX_BANK_INTRO && sample_index >= 0 && sample_index < TNT_INTRO_SAMPLE_COUNT)
        return g_tnt_intro_sample_loop_end[(u8)sample_index];
    return 0;
}

static f32 tnt_dcm_pitch_from_word(u16 raw) {
    u32 band;
    u32 freq;
    f32 pitch;

    band = raw >> 14;
    freq = s_freq_base[band] + ((u32)(raw & 0x3FFFU) << band);
    if ((s16)freq <= 0)
        return 0.0f;

    pitch = 8363.0f * (1712.0f / (f32)(14318184U / freq)) / 36000.0f;
    if (pitch >= 2.0f)
        pitch = 1.0f;
    if (pitch < 0.0f)
        pitch = -pitch;
    return pitch;
}

static void tnt_calculate_gain(u8 volume_index, u8 pan, u32 level, s32 *gain_l, s32 *gain_r) {
    s32 requested_vol;
    s32 env_vol;

    requested_vol = (s32)((sqrtf((f32)volume_index) * (f32)level) / 16.0f);
    if (requested_vol > 32767)
        requested_vol = 32767;
    env_vol = (requested_vol * requested_vol) >> 15;
    *gain_l = (env_vol * s_eqpower[pan]) >> 15;
    *gain_r = (env_vol * s_eqpower[127 - pan]) >> 15;
}

static void tnt_update_sfx_voice_gain(TNTDcmVoice *voice) {
    tnt_calculate_gain(voice->volume_index, voice->pan, s_current_sfx_level,
                       &voice->gain_l, &voice->gain_r);
}

static void tnt_update_music_voice_gain(TNTDcmMusicVoice *voice) {
    tnt_calculate_gain(voice->volume_index, voice->pan, s_current_music_level,
                       &voice->gain_l, &voice->gain_r);
}

static void tnt_voice_set_pitch(TNTDcmVoice *voice, u16 raw) {
    voice->pitch = tnt_dcm_pitch_from_word(raw);
    voice->step_q16 = (u32)(voice->pitch *
        ((f32)TNT_DCM_OUTPUT_RATE / (f32)TNT_AUDIO_RATE) * 65536.0f + 0.5f);
}

static void tnt_voice_reset(TNTDcmVoice *voice, s8 sample_index) {
    memset(voice, 0, sizeof(*voice));
    voice->pitch = 1.0f;
    voice->step_q16 = (u32)(((u64)TNT_DCM_OUTPUT_RATE << 16) / TNT_AUDIO_RATE);
    voice->pan = 64;
    voice->sample_index = sample_index;
}

static void tnt_slot_stop(TNTDcmSfxSlot *slot) {
    slot->active = FALSE;
    slot->restart = FALSE;
    slot->voice[0].active = FALSE;
    slot->voice[1].active = FALSE;
}

static void tnt_stop_all_slots(void) {
    s32 i;
    for (i = 0; i < TNT_AUDIO_SLOT_COUNT; i++)
        tnt_slot_stop(&s_slots[i]);
}

static void tnt_slot_reset_stream(TNTDcmSfxSlot *slot) {
    s8 default_sample = tnt_bank_sample_map(slot->bank_kind, slot->dcm_index, 0);
    slot->stream_pos = 0;
    slot->delay = 0;
    slot->restart = FALSE;
    tnt_voice_reset(&slot->voice[0], default_sample);
    tnt_voice_reset(&slot->voice[1], default_sample);
}

static void tnt_slot_begin(
    TNTDcmSfxSlot *slot,
    u8 bank_kind,
    u8 sfx_id,
    u8 dcm_index,
    u8 priority
) {
    memset(slot, 0, sizeof(*slot));
    slot->bank_kind = bank_kind;
    slot->sfx_id = sfx_id;
    slot->dcm_index = dcm_index;
    slot->priority = priority;
    slot->stream = tnt_bank_stream(bank_kind, dcm_index);
    slot->stream_len = tnt_bank_stream_len(bank_kind, dcm_index);
    slot->age = s_slot_age++;
    if (s_slot_age == 0)
        s_slot_age = 1;
    slot->active = (slot->stream != NULL && slot->stream_len != 0);
    tnt_slot_reset_stream(slot);
}

static s32 tnt_find_slot_for_priority(u8 priority) {
    s32 i;
    s32 best = -1;
    u32 oldest = 0xFFFFFFFFU;

    for (i = 0; i < TNT_AUDIO_SLOT_COUNT; i++) {
        if (!s_slots[i].active)
            return i;
    }

    for (i = 0; i < TNT_AUDIO_SLOT_COUNT; i++) {
        if (s_slots[i].priority <= priority && s_slots[i].age < oldest) {
            best = i;
            oldest = s_slots[i].age;
        }
    }
    return best;
}

static void tnt_play_effect(u8 bank_kind, u8 sfx_id) {
    const TNTSfxDefinition *def;
    s32 i;
    u8 found = FALSE;

    def = tnt_effect_def(bank_kind, sfx_id);
    if (def == NULL)
        return;

    /* Original Audio2_Play_SFX_Bank_Channel does not allocate a duplicate of
     * an effect already in the eight logical slots. It marks every layer of
     * that effect to restart on the next 50 Hz handler tick. */
    for (i = 0; i < TNT_AUDIO_SLOT_COUNT; i++) {
        if (s_slots[i].active &&
            s_slots[i].bank_kind == bank_kind &&
            s_slots[i].sfx_id == sfx_id) {
            s_slots[i].restart = TRUE;
            found = TRUE;
        }
    }
    if (found)
        return;

    for (i = 0; i < def->layer_count; i++) {
        s32 slot_index;
        u8 layer_priority = (u8)(def->priority - i);

        slot_index = tnt_find_slot_for_priority(layer_priority);
        if (slot_index < 0)
            return;

        tnt_slot_begin(
            &s_slots[slot_index],
            bank_kind,
            sfx_id,
            def->dcm_index[i],
            layer_priority);
    }
}

static u16 tnt_read_le16(const u8 *p) {
    return (u16)p[0] | ((u16)p[1] << 8);
}

static u32 tnt_read_le32(const u8 *p) {
    return (u32)p[0] | ((u32)p[1] << 8) |
           ((u32)p[2] << 16) | ((u32)p[3] << 24);
}

static int tnt_music_pack_load(void) {
    SceUID fd;
    SceOff size;
    u8 *data;
    s32 total = 0;

    fd = sceIoOpen("ux0:data/TNTV/tntv_music_from_rom_v1.bin", SCE_O_RDONLY, 0);
    if (fd < 0)
        return -1;
    size = sceIoLseek(fd, 0, SCE_SEEK_END);
    if (size <= 0 || size > 32 * 1024 * 1024) {
        sceIoClose(fd);
        return -1;
    }
    if (sceIoLseek(fd, 0, SCE_SEEK_SET) < 0) {
        sceIoClose(fd);
        return -1;
    }
    data = (u8 *)malloc((size_t)size);
    if (data == NULL) {
        sceIoClose(fd);
        return -1;
    }
    while (total < (s32)size) {
        s32 got = sceIoRead(fd, data + total, (SceSize)((s32)size - total));
        if (got <= 0) {
            free(data);
            sceIoClose(fd);
            return -1;
        }
        total += got;
    }
    sceIoClose(fd);
    if (size < 144 || memcmp(data, "TNMP", 4) != 0 ||
        tnt_read_le32(data + 4) != 1 ||
        tnt_read_le32(data + 8) != TNT_MUSIC_PACK_SONGS ||
        tnt_read_le32(data + 12) != 16) {
        free(data);
        return -1;
    }
    s_music_pack = data;
    s_music_pack_size = (u32)size;
    return 0;
}

static int tnt_music_select_pack_song(u8 song) {
    const u8 *entry;
    const u8 *blob;
    u32 blob_off;
    u32 blob_size;
    u32 stream_len;
    u32 stream_off;
    u32 desc_off;
    u8 channels;
    u8 sample_count;
    s32 i;

    if (s_music_pack == NULL || song >= TNT_MUSIC_PACK_SONGS)
        return -1;
    entry = s_music_pack + 16 + (song * 8);
    blob_off = tnt_read_le32(entry);
    blob_size = tnt_read_le32(entry + 4);
    if (blob_off > s_music_pack_size || blob_size > s_music_pack_size - blob_off || blob_size < 24)
        return -1;
    blob = s_music_pack + blob_off;
    if (memcmp(blob, "TSNG", 4) != 0 || blob[4] != song)
        return -1;
    channels = blob[5];
    sample_count = blob[6];
    stream_len = tnt_read_le32(blob + 8);
    stream_off = tnt_read_le32(blob + 12);
    desc_off = tnt_read_le32(blob + 16);
    if (channels == 0 || channels > TNT_MUSIC_CHANNEL_COUNT ||
        sample_count == 0 || sample_count > TNT_MUSIC_MAX_SAMPLES ||
        stream_off > blob_size || stream_len > blob_size - stream_off ||
        desc_off > blob_size || (u32)sample_count * 24U > blob_size - desc_off)
        return -1;

    memset(&s_music_track, 0, sizeof(s_music_track));
    s_music_track.stream = blob + stream_off;
    s_music_track.stream_len = stream_len;
    s_music_track.channels = channels;
    s_music_track.sample_count = sample_count;
    for (i = 0; i < sample_count; i++) {
        const u8 *d = blob + desc_off + (i * 24);
        u32 data_off = tnt_read_le32(d + 16);
        u32 data_size = tnt_read_le32(d + 20);
        u32 sample_len = tnt_read_le32(d);
        u8 flags = (u8)tnt_read_le16(d + 12);
        u32 need = sample_len * ((flags & 1U) ? 2U : 1U);
        if (data_off > blob_size || data_size > blob_size - data_off || data_size < need) {
            /* All song data must originate from the user's validated ROM.
             * A corrupt sample table must fail rather than silently
             * reverting to an embedded commercial-game music copy.
             */
            memset(&s_music_track, 0, sizeof(s_music_track));
            s_music_active = FALSE;
            return -1;
        }
        s_music_track.sample[i] = blob + data_off;
        s_music_track.sample_len[i] = sample_len;
        s_music_track.loop_begin[i] = tnt_read_le32(d + 4);
        s_music_track.loop_end[i] = tnt_read_le32(d + 8);
        s_music_track.flags[i] = flags;
    }
    s_current_song = song;
    return 0;
}

static int tnt_music_select_song(u8 song) {
    /* Every original song, including TITLE song 0, is now read exclusively
     * from the 16-track archive generated from the user's own N64 ROM.
     * Test128 host parity proved this is byte-identical to embedded data.
     */
    return tnt_music_select_pack_song(song);
}

static void tnt_music_voice_reset(TNTDcmMusicVoice *voice) {
    memset(voice, 0, sizeof(*voice));
    voice->pitch = 1.0f;
    voice->step_q16 = (u32)(((u64)TNT_DCM_OUTPUT_RATE << 16) / TNT_AUDIO_RATE);
    voice->pan = 64;
    voice->selected_sample = 0;
    voice->playing_sample = 0;
}

static void tnt_music_reset_stream(void) {
    s32 i;
    s_music_stream_pos = 0;
    s_music_delay = 0;
    s_music_tick_period_q16 = (TNT_DCM_TICK_FRAMES << 16);
    for (i = 0; i < TNT_MUSIC_CHANNEL_COUNT; i++)
        tnt_music_voice_reset(&s_music_voice[i]);
}

static u8 tnt_music_read(u8 *out) {
    if (s_music_stream_pos >= s_music_track.stream_len)
        return FALSE;
    *out = s_music_track.stream[s_music_stream_pos++];
    return TRUE;
}

static void tnt_music_set_pitch(TNTDcmMusicVoice *voice, u16 raw) {
    voice->pitch = tnt_dcm_pitch_from_word(raw);
    voice->step_q16 = (u32)(voice->pitch *
        ((f32)TNT_DCM_OUTPUT_RATE / (f32)TNT_AUDIO_RATE) * 65536.0f + 0.5f);
}

static void tnt_music_set_tempo(u8 bpm) {
    u32 micros;
    if (bpm == 0)
        return;
    /* Canonical Dcm_VoiceHandler: 125 * 1,000,000 / (bpm * 50). */
    micros = 125000000U / ((u32)bpm * 50U);
    s_music_tick_period_q16 = (u32)(((u64)micros * TNT_AUDIO_RATE * 65536ULL) / 1000000ULL);
    if (s_music_tick_period_q16 < 65536U)
        s_music_tick_period_q16 = 65536U;
}

static void tnt_music_event(s32 channel_index) {
    TNTDcmMusicVoice *voice = &s_music_voice[channel_index];
    u8 op;
    u8 value;

    if (s_music_delay != 0) {
        s_music_delay--;
        return;
    }
    if (!tnt_music_read(&op))
        return;
    if (op & 0x80) {
        s_music_delay = op & 0x7F;
        return;
    }
    if (op & 0x40) {
        u8 lo, hi;
        if (!tnt_music_read(&lo) || !tnt_music_read(&hi)) return;
        tnt_music_set_pitch(voice, (u16)(lo | ((u16)hi << 8)));
    }
    if (op & 0x20) {
        if (!tnt_music_read(&voice->volume_index)) return;
        if (voice->volume_index == 0) voice->volume_index = 1;
    }
    if (op & 0x10) {
        if (!tnt_music_read(&value)) return;
        voice->pan = value >> 1;
        if (voice->pan > 127) voice->pan = 127;
    }
    if (op & 0x08) {
        if (!tnt_music_read(&value)) return;
        if (value < s_music_track.sample_count) {
            /* Selecting a new wavetable schedules the old N64 voice to fade/
             * stop. For this direct mixer, stop the old decoder immediately;
             * a simultaneous START below begins the new sample in this event. */
            if (voice->active)
                voice->active = FALSE;
            voice->selected_sample = value;
            voice->sample_offset = 0;
        }
    }
    if (op & 0x04) {
        u32 byte_offset;
        if (!tnt_music_read(&value)) return;
        byte_offset = (u32)value << 8;
        voice->sample_offset =
            (s_music_track.flags[voice->selected_sample] & 1U)
                ? (byte_offset >> 1) : byte_offset;
    }
    if (op & 0x02) {
        voice->playing_sample = voice->selected_sample;
        voice->phase_q16 = voice->sample_offset << 16;
        voice->active = TRUE;
    }
    if (op & 0x01) {
        u8 ext;
        if (!tnt_music_read(&ext)) return;
        if (ext & 0x40) {
            if (!tnt_music_read(&value)) return;
            tnt_music_set_tempo(value);
        }
        if (ext & 0x20) {
            if (!tnt_music_read(&value)) return;
        }
    }
    tnt_update_music_voice_gain(voice);
}

static void tnt_music_tick(void) {
    s32 i;
    if (!s_music_active)
        return;
    if (s_music_stream_pos >= s_music_track.stream_len) {
        /* g_songPlayer is initialized with restart enabled. The source stops
         * every voice, rewinds the DCM decoder, and resumes on the next tick. */
        tnt_music_reset_stream();
        return;
    }
    for (i = 0; i < s_music_track.channels; i++) {
        if (s_music_stream_pos >= s_music_track.stream_len)
            break;
        tnt_music_event(i);
    }
}


static void tnt_mix_music_voice_span(
    TNTDcmMusicVoice *voice,
    s32 *mix,
    u32 frame_offset,
    u32 frame_count
) {
    u8 sample_index;
    u8 flags;
    const u8 *sample;
    u32 sample_len;
    u32 loop_begin;
    u32 loop_end;
    u32 phase;
    u32 step;
    s32 gain_l;
    s32 gain_r;
    u32 frame;

    if (!voice->active || frame_count == 0)
        return;

    sample_index = voice->playing_sample;
    if (sample_index >= s_music_track.sample_count) {
        voice->active = FALSE;
        return;
    }

    sample = s_music_track.sample[sample_index];
    flags = s_music_track.flags[sample_index];
    sample_len = s_music_track.sample_len[sample_index];
    loop_begin = s_music_track.loop_begin[sample_index];
    loop_end = s_music_track.loop_end[sample_index];
    phase = voice->phase_q16;
    step = voice->step_q16;
    gain_l = voice->gain_l;
    gain_r = voice->gain_r;

    for (frame = 0; frame < frame_count; frame++) {
        u32 pos = phase >> 16;
        u32 next_pos;
        u32 frac;
        s32 s0;
        s32 s1;
        s32 pcm;
        u32 out_index;

        if ((flags & 4U) && loop_end > loop_begin) {
            if (pos >= loop_end) {
                u32 loop_len = loop_end - loop_begin;
                u32 frac_q16 = phase & 0xFFFFU;
                pos = loop_begin + ((pos - loop_end) % loop_len);
                phase = (pos << 16) | frac_q16;
            }
        } else if (pos >= sample_len) {
            voice->active = FALSE;
            break;
        }

        if (pos >= sample_len) {
            voice->active = FALSE;
            break;
        }

        frac = (phase >> 8) & 0xFFU;
        next_pos = pos + 1U;
        if ((flags & 4U) && loop_end > loop_begin && next_pos >= loop_end)
            next_pos = loop_begin;

        if (flags & 1U) {
            u32 b0 = pos << 1;
            u32 b1 = next_pos << 1;
            s0 = (s16)((u16)sample[b0] | ((u16)sample[b0 + 1] << 8));
            s1 = next_pos < sample_len
                ? (s16)((u16)sample[b1] | ((u16)sample[b1 + 1] << 8)) : 0;
        } else {
            s0 = ((s32)(s8)sample[pos]) << 8;
            s1 = next_pos < sample_len ? ((s32)(s8)sample[next_pos]) << 8 : 0;
        }

        pcm = s0 + (((s1 - s0) * (s32)frac) >> 8);
        out_index = (frame_offset + frame) << 1;
        mix[out_index] += (pcm * gain_l) >> 15;
        mix[out_index + 1] += (pcm * gain_r) >> 15;
        phase += step;
    }

    voice->phase_q16 = phase;
}

static u8 tnt_stream_read(TNTDcmSfxSlot *slot, u8 *out) {
    if (slot->stream_pos >= slot->stream_len) {
        tnt_slot_stop(slot);
        return FALSE;
    }
    *out = slot->stream[slot->stream_pos++];
    return TRUE;
}

static void tnt_slot_event(TNTDcmSfxSlot *slot, s32 channel_index) {
    TNTDcmVoice *voice;
    u8 op;
    u8 value;

    if (!slot->active)
        return;

    /* Dcm_VoiceHandler_2 uses one delay and one stream pointer per logical
     * effect while walking its two physical channels. */
    if (slot->delay != 0) {
        slot->delay--;
        return;
    }

    if (!tnt_stream_read(slot, &op))
        return;

    if (op & 0x80) {
        slot->delay = op & 0x7F;
        return;
    }

    voice = &slot->voice[channel_index];

    if (op & 0x40) {
        u8 lo, hi;
        if (!tnt_stream_read(slot, &lo) || !tnt_stream_read(slot, &hi))
            return;
        tnt_voice_set_pitch(voice, (u16)(lo | ((u16)hi << 8)));
    }
    if (op & 0x20) {
        if (!tnt_stream_read(slot, &voice->volume_index))
            return;
    }
    if (op & 0x10) {
        if (!tnt_stream_read(slot, &value))
            return;
        voice->pan = value >> 1;
        if (voice->pan > 127)
            voice->pan = 127;
    }
    if (op & 0x08) {
        s8 mapped;
        if (!tnt_stream_read(slot, &value))
            return;
        mapped = tnt_bank_sample_map(slot->bank_kind, slot->dcm_index, value);
        if (mapped >= 0)
            voice->sample_index = mapped;
    }
    if (op & 0x04) {
        if (!tnt_stream_read(slot, &value))
            return;
        voice->sample_offset = (u32)value << 8;
    } else if (op & 0x02) {
        voice->sample_offset = 0;
    }

    if (op & 0x02) {
        voice->phase_q16 = voice->sample_offset << 16;
        voice->active = TRUE;
    }

    if (op & 0x01) {
        u8 ext;
        if (!tnt_stream_read(slot, &ext))
            return;
        if (ext & 0x40) {
            /* N64 Dcm_VoiceHandler_2 changes the global SFX callback period:
             *   microtime = 125 * 1000000 / (value * 50)
             * The sand-intro streams all set value=75, i.e. ~30 Hz. */
            u32 microtime;
            u64 frames;
            if (!tnt_stream_read(slot, &value))
                return;
            if (value != 0) {
                microtime = 125000000U / ((u32)value * 50U);
                frames = (u64)TNT_AUDIO_RATE * (u64)microtime + 500000ULL;
                frames /= 1000000ULL;
                if (frames == 0)
                    frames = 1;
                s_sfx_tick_period_frames = (u32)frames;
            }
        }
        if (ext & 0x20) {
            if (!tnt_stream_read(slot, &value))
                return;
        }
    }
    tnt_update_sfx_voice_gain(voice);
}

static void tnt_dcm_tick(void) {
    s32 i;
    for (i = 0; i < TNT_AUDIO_SLOT_COUNT; i++) {
        TNTDcmSfxSlot *slot = &s_slots[i];
        if (!slot->active)
            continue;
        if (slot->restart)
            tnt_slot_reset_stream(slot);
        tnt_slot_event(slot, 0);
        tnt_slot_event(slot, 1);
    }
}

static s32 tnt_clamp_s16(s32 value) {
    if (value > 32767) return 32767;
    if (value < -32768) return -32768;
    return value;
}

static void tnt_mix_voice_span(
    const TNTDcmSfxSlot *slot,
    TNTDcmVoice *voice,
    s32 *mix,
    u32 frame_offset,
    u32 frame_count
) {
    const s8 *sample;
    u32 sample_len;
    u32 flags;
    u32 loop_begin;
    u32 loop_end;
    u32 absolute_loop_begin;
    u32 absolute_loop_end;
    u32 phase;
    u32 step;
    s32 gain_l;
    s32 gain_r;
    u32 frame;

    if (!voice->active || frame_count == 0)
        return;

    sample = tnt_bank_sample(slot->bank_kind, voice->sample_index);
    sample_len = tnt_bank_sample_len(slot->bank_kind, voice->sample_index);
    if (sample == NULL || sample_len == 0) {
        voice->active = FALSE;
        return;
    }

    flags = tnt_bank_sample_flags(slot->bank_kind, voice->sample_index);
    loop_begin = tnt_bank_loop_begin(slot->bank_kind, voice->sample_index);
    loop_end = tnt_bank_loop_end(slot->bank_kind, voice->sample_index);
    absolute_loop_begin = voice->sample_offset + loop_begin;
    absolute_loop_end = voice->sample_offset + loop_end;
    phase = voice->phase_q16;
    step = voice->step_q16;
    gain_l = voice->gain_l;
    gain_r = voice->gain_r;

    for (frame = 0; frame < frame_count; frame++) {
        u32 pos = phase >> 16;
        u32 frac;
        u32 next_pos;
        s32 s0;
        s32 s1;
        s32 pcm;
        u32 out_index;

        if ((flags & 0x04U) && loop_end > loop_begin) {
            if (pos >= absolute_loop_end) {
                u32 loop_len = loop_end - loop_begin;
                u32 frac_q16 = phase & 0xFFFFU;
                pos = absolute_loop_begin + ((pos - absolute_loop_end) % loop_len);
                phase = (pos << 16) | frac_q16;
            }
        } else if (pos >= sample_len) {
            voice->active = FALSE;
            break;
        }

        if (pos >= sample_len) {
            voice->active = FALSE;
            break;
        }

        frac = (phase >> 8) & 0xFFU;
        next_pos = pos + 1U;
        if ((flags & 0x04U) && loop_end > loop_begin && next_pos >= absolute_loop_end)
            next_pos = absolute_loop_begin;

        s0 = sample[pos];
        s1 = (next_pos < sample_len) ? sample[next_pos] : 0;
        pcm = s0 * 256 + (((s1 - s0) * (s32)frac));
        out_index = (frame_offset + frame) << 1;
        mix[out_index] += (pcm * gain_l) >> 15;
        mix[out_index + 1] += (pcm * gain_r) >> 15;
        phase += step;
    }

    voice->phase_q16 = phase;
}

static void tnt_mix_block(s16 *out) {
    s32 mix[TNT_AUDIO_BLOCK * 2];
    u32 sfx_level = TNT_Vita_SramGetSfxLevelStep() * 2340U;
    u32 music_level = TNT_Vita_SramGetMusicLevelStep() * 2340U;
    u32 cursor = 0;
    s32 i;

    if (sfx_level > 0x7FFFU) sfx_level = 0x7FFFU;
    if (music_level > 0x7FFFU) music_level = 0x7FFFU;

    if (sfx_level != s_current_sfx_level) {
        s_current_sfx_level = sfx_level;
        for (i = 0; i < TNT_AUDIO_SLOT_COUNT; i++) {
            tnt_update_sfx_voice_gain(&s_slots[i].voice[0]);
            tnt_update_sfx_voice_gain(&s_slots[i].voice[1]);
        }
    }
    if (music_level != s_current_music_level) {
        s_current_music_level = music_level;
        for (i = 0; i < TNT_MUSIC_CHANNEL_COUNT; i++)
            tnt_update_music_voice_gain(&s_music_voice[i]);
    }

    memset(mix, 0, sizeof(mix));

    /* Mix in spans bounded by control-rate events. The prior implementation
     * called the full voice routine for every output sample. DCM parameters
     * can only change on the SFX/music scheduler boundaries, so doing metadata
     * lookup once per voice per span is sample-identical while dramatically
     * reducing overhead when line-clear SFX add several simultaneous voices. */
    while (cursor < TNT_AUDIO_BLOCK) {
        u32 span = TNT_AUDIO_BLOCK - cursor;

        if (s_frames_to_tick == 0) {
            tnt_dcm_tick();
            s_frames_to_tick = s_sfx_tick_period_frames;
        }
        if (s_music_active && s_music_frames_to_tick_q16 <= 0) {
            tnt_music_tick();
            s_music_frames_to_tick_q16 += s_music_tick_period_q16;
        }

        if (s_frames_to_tick < span)
            span = s_frames_to_tick;

        if (s_music_active) {
            u64 q = (u64)s_music_frames_to_tick_q16;
            u32 music_span = (u32)((q + 65535ULL) >> 16);
            if (music_span == 0)
                music_span = 1;
            if (music_span < span)
                span = music_span;
        }

        if (span == 0)
            span = 1;

        if (s_music_active) {
            for (i = 0; i < TNT_MUSIC_CHANNEL_COUNT; i++)
                tnt_mix_music_voice_span(&s_music_voice[i], mix, cursor, span);
        }
        for (i = 0; i < TNT_AUDIO_SLOT_COUNT; i++) {
            if (s_slots[i].active) {
                tnt_mix_voice_span(&s_slots[i], &s_slots[i].voice[0], mix, cursor, span);
                tnt_mix_voice_span(&s_slots[i], &s_slots[i].voice[1], mix, cursor, span);
            }
        }

        cursor += span;
        s_frames_to_tick -= span;
        if (s_music_active)
            s_music_frames_to_tick_q16 -= (s64)span << 16;
    }

    for (cursor = 0; cursor < TNT_AUDIO_BLOCK; cursor++) {
        out[cursor * 2] = (s16)tnt_clamp_s16(mix[cursor * 2]);
        out[cursor * 2 + 1] = (s16)tnt_clamp_s16(mix[cursor * 2 + 1]);
    }
}

static int tnt_audio_thread(SceSize args, void *argp) {
    s16 out[TNT_AUDIO_BLOCK * 2];
    (void)args;
    (void)argp;

    while (s_audio_running) {
        memset(out, 0, sizeof(out));
        if (s_audio_mutex >= 0)
            sceKernelLockMutex(s_audio_mutex, 1, NULL);
        tnt_mix_block(out);
        if (s_audio_mutex >= 0)
            sceKernelUnlockMutex(s_audio_mutex, 1);
        if (s_audio_port >= 0)
            sceAudioOutOutput(s_audio_port, out);
    }
    return 0;
}

int TNT_Vita_AudioInit(void) {
    int volumes[2] = { SCE_AUDIO_OUT_MAX_VOL, SCE_AUDIO_OUT_MAX_VOL };

    memset(s_slots, 0, sizeof(s_slots));
    s_frames_to_tick = 0;
    s_sfx_tick_period_frames = TNT_DCM_TICK_FRAMES;
    s_slot_age = 1;
    s_current_bank = TNT_SFX_BANK_NONE;
    s_current_sfx_level = 0xFFFFFFFFU;
    s_current_music_level = 0xFFFFFFFFU;
    s_music_active = FALSE;
    s_music_frames_to_tick_q16 = 0;
    s_music_pack = NULL;
    s_music_pack_size = 0;
    /* Test129R: Require locally generated original-ROM music. The prior
     * embedded title fallback is intentionally absent from this binary.
     */
    if (tnt_music_pack_load() != 0)
        return -1;
    if (tnt_music_select_pack_song(0) != 0) {
        free(s_music_pack);
        s_music_pack = NULL;
        s_music_pack_size = 0;
        return -1;
    }
    tnt_music_reset_stream();

    s_audio_mutex = sceKernelCreateMutex("TNTV audio mutex", 0, 1, NULL);
    if (s_audio_mutex < 0)
        return -1;

    s_audio_port = sceAudioOutOpenPort(
        SCE_AUDIO_OUT_PORT_TYPE_BGM,
        TNT_AUDIO_BLOCK,
        TNT_AUDIO_RATE,
        SCE_AUDIO_OUT_MODE_STEREO);
    if (s_audio_port < 0) {
        sceKernelDeleteMutex(s_audio_mutex);
        s_audio_mutex = -1;
        return -1;
    }
    sceAudioOutSetVolume(
        s_audio_port,
        SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH,
        volumes);

    s_audio_thread = sceKernelCreateThread(
        "TNTV audio",
        tnt_audio_thread,
        0x10000100,
        0x10000,
        0,
        0,
        NULL);
    if (s_audio_thread < 0) {
        sceAudioOutReleasePort(s_audio_port);
        s_audio_port = -1;
        sceKernelDeleteMutex(s_audio_mutex);
        s_audio_mutex = -1;
        return -1;
    }

    s_audio_running = TRUE;
    if (sceKernelStartThread(s_audio_thread, 0, NULL) < 0) {
        s_audio_running = FALSE;
        sceKernelDeleteThread(s_audio_thread);
        s_audio_thread = -1;
        sceAudioOutReleasePort(s_audio_port);
        s_audio_port = -1;
        sceKernelDeleteMutex(s_audio_mutex);
        s_audio_mutex = -1;
        return -1;
    }
    return 0;
}

void TNT_Vita_AudioTerm(void) {
    if (s_audio_thread >= 0) {
        s_audio_running = FALSE;
        sceKernelWaitThreadEnd(s_audio_thread, NULL, NULL);
        sceKernelDeleteThread(s_audio_thread);
        s_audio_thread = -1;
    }
    if (s_audio_port >= 0) {
        sceAudioOutReleasePort(s_audio_port);
        s_audio_port = -1;
    }
    if (s_audio_mutex >= 0) {
        sceKernelDeleteMutex(s_audio_mutex);
        s_audio_mutex = -1;
    }
    if (s_music_pack != NULL) {
        free(s_music_pack);
        s_music_pack = NULL;
        s_music_pack_size = 0;
    }
}

int TNT_Vita_MusicPlaySong(u8 song) {
    int result;
    if (song >= TNT_MUSIC_PACK_SONGS)
        return -1;
    if (s_audio_mutex >= 0)
        sceKernelLockMutex(s_audio_mutex, 1, NULL);
    result = tnt_music_select_song(song);
    if (result == 0) {
        tnt_music_reset_stream();
        s_music_frames_to_tick_q16 = 0;
        s_music_active = TRUE;
    }
    if (s_audio_mutex >= 0)
        sceKernelUnlockMutex(s_audio_mutex, 1);
    return result;
}

u8 TNT_Vita_MusicCurrentSong(void) {
    return s_current_song;
}

void TNT_Vita_MusicPlayTitle(void) {
    (void)TNT_Vita_MusicPlaySong(0);
}

void TNT_Vita_MusicStop(void) {
    if (s_audio_mutex >= 0)
        sceKernelLockMutex(s_audio_mutex, 1, NULL);
    s_music_active = FALSE;
    tnt_music_reset_stream();
    if (s_audio_mutex >= 0)
        sceKernelUnlockMutex(s_audio_mutex, 1);
}

void TNT_Vita_SfxStopAll(void) {
    if (s_audio_mutex >= 0)
        sceKernelLockMutex(s_audio_mutex, 1, NULL);
    tnt_stop_all_slots();
    s_current_bank = TNT_SFX_BANK_NONE;
    if (s_audio_mutex >= 0)
        sceKernelUnlockMutex(s_audio_mutex, 1);
}

void Audio2_Play_SFX(SfxPlayer *player, SfxBank *bank, s32 sfx) {
    u8 bank_kind;
    (void)player;

    if (bank == &g_menuSfxBank)
        bank_kind = TNT_SFX_BANK_MENU;
    else if (bank == &g_gameSfxBank)
        bank_kind = TNT_SFX_BANK_GAME;
    else if (bank == &g_introSfxBank)
        bank_kind = TNT_SFX_BANK_INTRO;
    else
        return;

    if ((bank_kind == TNT_SFX_BANK_MENU && (sfx < 0 || sfx >= TNT_MENU_SFX_COUNT)) ||
        (bank_kind == TNT_SFX_BANK_GAME && (sfx < 0 || sfx >= TNT_GAME_SFX_COUNT)) ||
        (bank_kind == TNT_SFX_BANK_INTRO && (sfx < 0 || sfx >= TNT_INTRO_SFX_COUNT)))
        return;

    if (s_audio_mutex >= 0)
        sceKernelLockMutex(s_audio_mutex, 1, NULL);

    /* The N64 loads only one SFX bank at a time. TNTV does not yet route all
     * Audio_LoadSFX calls, so bank identity on Play_SFX is the authoritative
     * transition point and stops voices from the previous bank. */
    if (s_current_bank != bank_kind) {
        tnt_stop_all_slots();
        s_current_bank = bank_kind;
    }
    tnt_play_effect(bank_kind, (u8)sfx);

    if (s_audio_mutex >= 0)
        sceKernelUnlockMutex(s_audio_mutex, 1);
}
