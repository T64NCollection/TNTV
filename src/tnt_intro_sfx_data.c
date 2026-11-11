#include "vita_compat.h"

/* ROM-derived sand-intro SFX DCM streams and signed 8-bit PCM samples. */

u8 g_tnt_intro_sfx_stream_0[1260]; /* user ROM */

u8 g_tnt_intro_sfx_stream_1[625]; /* user ROM */

u8 g_tnt_intro_sfx_stream_2[970]; /* user ROM */

u8 g_tnt_intro_sfx_stream_3[196]; /* user ROM */

u8 g_tnt_intro_sfx_stream_4[190]; /* user ROM */

u8 g_tnt_intro_sfx_stream_5[198]; /* user ROM */

u8 g_tnt_intro_sfx_stream_6[210]; /* user ROM */

u8 g_tnt_intro_sfx_stream_7[558]; /* user ROM */

u8 g_tnt_intro_sfx_stream_8[578]; /* user ROM */

u8 g_tnt_intro_sfx_stream_9[240]; /* user ROM */

u8 g_tnt_intro_sfx_stream_10[224]; /* user ROM */

u8 g_tnt_intro_sfx_stream_11[673]; /* user ROM */

u8 g_tnt_intro_sfx_stream_12[611]; /* user ROM */

u8 g_tnt_intro_sfx_stream_13[323]; /* user ROM */

u8 g_tnt_intro_sfx_stream_14[230]; /* user ROM */

u8 g_tnt_intro_sfx_stream_15[928]; /* user ROM */

u8 g_tnt_intro_sfx_stream_16[1126]; /* user ROM */

u8 g_tnt_intro_sfx_stream_17[977]; /* user ROM */

u8 g_tnt_intro_sfx_stream_18[142]; /* user ROM */

u8 g_tnt_intro_sfx_stream_19[141]; /* user ROM */

u8 g_tnt_intro_sfx_stream_20[311]; /* user ROM */

u8 g_tnt_intro_sfx_stream_21[335]; /* user ROM */

u8 g_tnt_intro_sfx_stream_22[204]; /* user ROM */

u8 g_tnt_intro_sfx_stream_23[647]; /* user ROM */

u8 g_tnt_intro_sfx_stream_24[533]; /* user ROM */

u8 g_tnt_intro_sfx_stream_25[531]; /* user ROM */

u8 g_tnt_intro_sfx_stream_26[963]; /* user ROM */

u8 g_tnt_intro_sfx_stream_27[445]; /* user ROM */

u8 g_tnt_intro_sample_118[5570]; /* user ROM */

u8 g_tnt_intro_sample_119[19138]; /* user ROM */

u8 g_tnt_intro_sample_11a[3381]; /* user ROM */

u8 g_tnt_intro_sample_11b[7207]; /* user ROM */

u8 g_tnt_intro_sample_11c[12529]; /* user ROM */

u8 g_tnt_intro_sample_11d[8853]; /* user ROM */

u8 g_tnt_intro_sample_11e[8912]; /* user ROM */

u8 g_tnt_intro_sample_11f[13708]; /* user ROM */

u8 g_tnt_intro_sample_120[10402]; /* user ROM */

u8 g_tnt_intro_sample_121[14210]; /* user ROM */

u8 g_tnt_intro_sample_122[7367]; /* user ROM */

u8 g_tnt_intro_sample_123[12981]; /* user ROM */

u8 g_tnt_intro_sample_124[9220]; /* user ROM */

u8 g_tnt_intro_sample_125[3606]; /* user ROM */

u8 g_tnt_intro_sample_126[22017]; /* user ROM */

const u8 *const g_tnt_intro_sfx_streams[28] = {
    g_tnt_intro_sfx_stream_0,
    g_tnt_intro_sfx_stream_1,
    g_tnt_intro_sfx_stream_2,
    g_tnt_intro_sfx_stream_3,
    g_tnt_intro_sfx_stream_4,
    g_tnt_intro_sfx_stream_5,
    g_tnt_intro_sfx_stream_6,
    g_tnt_intro_sfx_stream_7,
    g_tnt_intro_sfx_stream_8,
    g_tnt_intro_sfx_stream_9,
    g_tnt_intro_sfx_stream_10,
    g_tnt_intro_sfx_stream_11,
    g_tnt_intro_sfx_stream_12,
    g_tnt_intro_sfx_stream_13,
    g_tnt_intro_sfx_stream_14,
    g_tnt_intro_sfx_stream_15,
    g_tnt_intro_sfx_stream_16,
    g_tnt_intro_sfx_stream_17,
    g_tnt_intro_sfx_stream_18,
    g_tnt_intro_sfx_stream_19,
    g_tnt_intro_sfx_stream_20,
    g_tnt_intro_sfx_stream_21,
    g_tnt_intro_sfx_stream_22,
    g_tnt_intro_sfx_stream_23,
    g_tnt_intro_sfx_stream_24,
    g_tnt_intro_sfx_stream_25,
    g_tnt_intro_sfx_stream_26,
    g_tnt_intro_sfx_stream_27,
};
u32 g_tnt_intro_sfx_stream_lengths[28]; /* Verified player-ROM DCM metadata. */

const u8 *const g_tnt_intro_samples[15] = {
    g_tnt_intro_sample_118,
    g_tnt_intro_sample_119,
    g_tnt_intro_sample_11a,
    g_tnt_intro_sample_11b,
    g_tnt_intro_sample_11c,
    g_tnt_intro_sample_11d,
    g_tnt_intro_sample_11e,
    g_tnt_intro_sample_11f,
    g_tnt_intro_sample_120,
    g_tnt_intro_sample_121,
    g_tnt_intro_sample_122,
    g_tnt_intro_sample_123,
    g_tnt_intro_sample_124,
    g_tnt_intro_sample_125,
    g_tnt_intro_sample_126,
};
u32 g_tnt_intro_sample_lengths[15]; /* Verified player-ROM DCM metadata. */
u32 g_tnt_intro_sample_loop_begin[15]; /* Verified player-ROM DCM metadata. */
u32 g_tnt_intro_sample_loop_end[15]; /* Verified player-ROM DCM metadata. */
u8 g_tnt_intro_sample_flags[15]; /* Verified player-ROM DCM metadata. */

s8 g_tnt_intro_dcm_sample_map[28][8]; /* Verified player-ROM DCM metadata. */
