#include "vita_compat.h"

/* ROM-derived The New Tetris game SFX DCM streams and signed 8-bit PCM samples. */

u8 g_tnt_game_sfx_stream_0[24]; /* user ROM */

u8 g_tnt_game_sfx_stream_1[16]; /* user ROM */

u8 g_tnt_game_sfx_stream_2[280]; /* user ROM */

u8 g_tnt_game_sfx_stream_3[32]; /* user ROM */

u8 g_tnt_game_sfx_stream_4[20]; /* user ROM */

u8 g_tnt_game_sfx_stream_5[193]; /* user ROM */

u8 g_tnt_game_sfx_stream_6[17]; /* user ROM */

u8 g_tnt_game_sfx_stream_7[563]; /* user ROM */

u8 g_tnt_game_sfx_stream_8[532]; /* user ROM */

u8 g_tnt_game_sfx_stream_9[518]; /* user ROM */

u8 g_tnt_game_sfx_stream_10[291]; /* user ROM */

u8 g_tnt_game_sfx_stream_11[289]; /* user ROM */

u8 g_tnt_game_sfx_stream_12[18]; /* user ROM */

u8 g_tnt_game_sfx_stream_13[18]; /* user ROM */

u8 g_tnt_game_sfx_stream_14[18]; /* user ROM */

u8 g_tnt_game_sfx_stream_15[18]; /* user ROM */

u8 g_tnt_game_sfx_stream_16[195]; /* user ROM */

u8 g_tnt_game_sfx_stream_17[21]; /* user ROM */

u8 g_tnt_game_sfx_stream_18[499]; /* user ROM */

u8 g_tnt_game_sfx_stream_19[283]; /* user ROM */

u8 g_tnt_game_sfx_stream_20[557]; /* user ROM */

u8 g_tnt_game_sfx_stream_21[19]; /* user ROM */

u8 g_tnt_game_sfx_stream_22[560]; /* user ROM */

u8 g_tnt_game_sfx_stream_23[308]; /* user ROM */

u8 g_tnt_game_sfx_stream_24[549]; /* user ROM */

u8 g_tnt_game_sfx_stream_25[266]; /* user ROM */

u8 g_tnt_game_sfx_stream_26[293]; /* user ROM */

u8 g_tnt_game_sfx_stream_27[295]; /* user ROM */

u8 g_tnt_game_sfx_stream_28[495]; /* user ROM */

u8 g_tnt_game_sfx_stream_29[241]; /* user ROM */

u8 g_tnt_game_sfx_stream_30[310]; /* user ROM */

u8 g_tnt_game_sample_125[3606]; /* user ROM */

u8 g_tnt_game_sample_127[2356]; /* user ROM */

/* Writable placement is intentional: it leaves Vita SCE metadata room after the RO segment. The sample is never modified. */
u8 g_tnt_game_sample_128[2146]; /* user ROM */

u8 g_tnt_game_sample_129[10805]; /* user ROM */

u8 g_tnt_game_sample_12a[6241]; /* user ROM */

u8 g_tnt_game_sample_12b[1005]; /* user ROM */

u8 g_tnt_game_sample_12c[4655]; /* user ROM */

u8 g_tnt_game_sample_12d[1309]; /* user ROM */

u8 g_tnt_game_sample_12e[1972]; /* user ROM */

u8 g_tnt_game_sample_12f[10642]; /* user ROM */

u8 g_tnt_game_sample_130[14164]; /* user ROM */

u8 g_tnt_game_sample_131[7405]; /* user ROM */

u8 g_tnt_game_sample_132[9798]; /* user ROM */

u8 g_tnt_game_sample_133[9120]; /* user ROM */

u8 g_tnt_game_sample_134[5432]; /* user ROM */

u8 g_tnt_game_sample_135[12698]; /* user ROM */

u8 g_tnt_game_sample_136[14632]; /* user ROM */

u8 g_tnt_game_sample_137[19049]; /* user ROM */

const u8 *const g_tnt_game_sfx_streams[31] = {
    g_tnt_game_sfx_stream_0,
    g_tnt_game_sfx_stream_1,
    g_tnt_game_sfx_stream_2,
    g_tnt_game_sfx_stream_3,
    g_tnt_game_sfx_stream_4,
    g_tnt_game_sfx_stream_5,
    g_tnt_game_sfx_stream_6,
    g_tnt_game_sfx_stream_7,
    g_tnt_game_sfx_stream_8,
    g_tnt_game_sfx_stream_9,
    g_tnt_game_sfx_stream_10,
    g_tnt_game_sfx_stream_11,
    g_tnt_game_sfx_stream_12,
    g_tnt_game_sfx_stream_13,
    g_tnt_game_sfx_stream_14,
    g_tnt_game_sfx_stream_15,
    g_tnt_game_sfx_stream_16,
    g_tnt_game_sfx_stream_17,
    g_tnt_game_sfx_stream_18,
    g_tnt_game_sfx_stream_19,
    g_tnt_game_sfx_stream_20,
    g_tnt_game_sfx_stream_21,
    g_tnt_game_sfx_stream_22,
    g_tnt_game_sfx_stream_23,
    g_tnt_game_sfx_stream_24,
    g_tnt_game_sfx_stream_25,
    g_tnt_game_sfx_stream_26,
    g_tnt_game_sfx_stream_27,
    g_tnt_game_sfx_stream_28,
    g_tnt_game_sfx_stream_29,
    g_tnt_game_sfx_stream_30,
};
u32 g_tnt_game_sfx_stream_lengths[31]; /* Verified player-ROM DCM metadata. */

const u8 *const g_tnt_game_samples[18] = {
    g_tnt_game_sample_125,
    g_tnt_game_sample_127,
    g_tnt_game_sample_128,
    g_tnt_game_sample_129,
    g_tnt_game_sample_12a,
    g_tnt_game_sample_12b,
    g_tnt_game_sample_12c,
    g_tnt_game_sample_12d,
    g_tnt_game_sample_12e,
    g_tnt_game_sample_12f,
    g_tnt_game_sample_130,
    g_tnt_game_sample_131,
    g_tnt_game_sample_132,
    g_tnt_game_sample_133,
    g_tnt_game_sample_134,
    g_tnt_game_sample_135,
    g_tnt_game_sample_136,
    g_tnt_game_sample_137,
};
u32 g_tnt_game_sample_lengths[18]; /* Verified player-ROM DCM metadata. */
u32 g_tnt_game_sample_loop_begin[18]; /* Verified player-ROM DCM metadata. */
u32 g_tnt_game_sample_loop_end[18]; /* Verified player-ROM DCM metadata. */
u8 g_tnt_game_sample_flags[18]; /* Verified player-ROM DCM metadata. */

s8 g_tnt_game_dcm_sample_map[31][4]; /* Verified player-ROM DCM metadata. */
