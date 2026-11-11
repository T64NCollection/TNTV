/* Last native game scenery frame dimensions. */
#include "tnt_egyptian_gameplay_vita.h"
#include "tnt_greek_gameplay_vita.h"
#include "tnt_finale_gameplay_vita.h"
/* Compile-time array sizes for ROM-derived stage backgrounds/effects. */
#include "tnt_african_gameplay_vita.h"
#include "tnt_celtic_gameplay_vita.h"
#include "tnt_japanese_gameplay_vita.h"
#include "tnt_russian_gameplay_vita.h"
/* TNTV 220 source-only art/SFX/FX/complete-Wonder texel symbols. */
extern uint8_t g_tnt_african_complete_anim_558[6420];
extern uint8_t g_tnt_african_complete_anim_559[5292];
extern uint8_t g_tnt_african_complete_anim_560[4140];
extern uint16_t g_tnt_african_complete_texels[45376];
extern uint8_t g_tnt_african_sky_anim_568[100];
extern uint8_t g_tnt_audio_panel_ci8[103490];
extern uint16_t g_tnt_audio_panel_rgba16[256];
extern uint8_t g_tnt_audio_shadow_ia8[107640];
extern uint16_t g_tnt_audio_slider_feather_rgba16[160];
extern uint16_t g_tnt_audio_slider_wood_rgba16[3950];
extern uint8_t g_tnt_celtic_complete_anim_471[13796];
extern uint8_t g_tnt_celtic_complete_anim_472[4588];
extern uint8_t g_tnt_celtic_complete_anim_473[5584];
extern uint16_t g_tnt_celtic_complete_texels[48640];
extern uint8_t g_tnt_celtic_leaf_anim_1336[36160];
extern uint8_t g_tnt_celtic_sky_anim_488[3532];
extern uint8_t g_tnt_credits_background_ci8[30000];
extern uint8_t g_tnt_credits_background_rgb[768];
extern uint8_t g_tnt_credits_height_i8[30000];
extern uint8_t g_tnt_credits_spotlight_ci8[16384];
extern uint8_t g_tnt_credits_spotlight_rgb[768];
extern uint8_t g_tnt_data_panel_ci8[100584];
extern uint16_t g_tnt_data_panel_rgba16[256];
extern uint8_t g_tnt_data_shadow_ia8[110760];
extern uint8_t g_tnt_developer_ci8[120000];
extern uint16_t g_tnt_developer_rgba16[256];
extern uint8_t g_tnt_egyptian_complete_anim_375[10496];
extern uint8_t g_tnt_egyptian_complete_anim_376[3592];
extern uint8_t g_tnt_egyptian_complete_anim_377[4848];
extern uint16_t g_tnt_egyptian_complete_texels[44096];
extern uint16_t g_tnt_finish_game_over_rgba16[2800];
extern uint16_t g_tnt_finish_lose_rgba16[1044];
extern uint16_t g_tnt_finish_win_rgba16[1120];
extern uint8_t g_tnt_font_a_ia8[19456];
extern uint8_t g_tnt_font_b_ia8[10304];
extern uint8_t g_tnt_font_c_ia8[7176];
extern uint8_t g_tnt_game_sample_125[3606];
extern uint8_t g_tnt_game_sample_127[2356];
extern uint8_t g_tnt_game_sample_128[2146];
extern uint8_t g_tnt_game_sample_129[10805];
extern uint8_t g_tnt_game_sample_12a[6241];
extern uint8_t g_tnt_game_sample_12b[1005];
extern uint8_t g_tnt_game_sample_12c[4655];
extern uint8_t g_tnt_game_sample_12d[1309];
extern uint8_t g_tnt_game_sample_12e[1972];
extern uint8_t g_tnt_game_sample_12f[10642];
extern uint8_t g_tnt_game_sample_130[14164];
extern uint8_t g_tnt_game_sample_131[7405];
extern uint8_t g_tnt_game_sample_132[9798];
extern uint8_t g_tnt_game_sample_133[9120];
extern uint8_t g_tnt_game_sample_134[5432];
extern uint8_t g_tnt_game_sample_135[12698];
extern uint8_t g_tnt_game_sample_136[14632];
extern uint8_t g_tnt_game_sample_137[19049];
extern uint8_t g_tnt_game_sfx_stream_0[24];
extern uint8_t g_tnt_game_sfx_stream_1[16];
extern uint8_t g_tnt_game_sfx_stream_10[291];
extern uint8_t g_tnt_game_sfx_stream_11[289];
extern uint8_t g_tnt_game_sfx_stream_12[18];
extern uint8_t g_tnt_game_sfx_stream_13[18];
extern uint8_t g_tnt_game_sfx_stream_14[18];
extern uint8_t g_tnt_game_sfx_stream_15[18];
extern uint8_t g_tnt_game_sfx_stream_16[195];
extern uint8_t g_tnt_game_sfx_stream_17[21];
extern uint8_t g_tnt_game_sfx_stream_18[499];
extern uint8_t g_tnt_game_sfx_stream_19[283];
extern uint8_t g_tnt_game_sfx_stream_2[280];
extern uint8_t g_tnt_game_sfx_stream_20[557];
extern uint8_t g_tnt_game_sfx_stream_21[19];
extern uint8_t g_tnt_game_sfx_stream_22[560];
extern uint8_t g_tnt_game_sfx_stream_23[308];
extern uint8_t g_tnt_game_sfx_stream_24[549];
extern uint8_t g_tnt_game_sfx_stream_25[266];
extern uint8_t g_tnt_game_sfx_stream_26[293];
extern uint8_t g_tnt_game_sfx_stream_27[295];
extern uint8_t g_tnt_game_sfx_stream_28[495];
extern uint8_t g_tnt_game_sfx_stream_29[241];
extern uint8_t g_tnt_game_sfx_stream_3[32];
extern uint8_t g_tnt_game_sfx_stream_30[310];
extern uint8_t g_tnt_game_sfx_stream_4[20];
extern uint8_t g_tnt_game_sfx_stream_5[193];
extern uint8_t g_tnt_game_sfx_stream_6[17];
extern uint8_t g_tnt_game_sfx_stream_7[563];
extern uint8_t g_tnt_game_sfx_stream_8[532];
extern uint8_t g_tnt_game_sfx_stream_9[518];
extern uint8_t g_tnt_greek_complete_anim_315[8300];
extern uint8_t g_tnt_greek_complete_anim_316[5996];
extern uint8_t g_tnt_greek_complete_anim_317[3980];
extern uint16_t g_tnt_greek_complete_texels[75520];
extern uint8_t g_tnt_greek_rain_anim_1319[3024];
extern uint8_t g_tnt_intro_sample_118[5570];
extern uint8_t g_tnt_intro_sample_119[19138];
extern uint8_t g_tnt_intro_sample_11a[3381];
extern uint8_t g_tnt_intro_sample_11b[7207];
extern uint8_t g_tnt_intro_sample_11c[12529];
extern uint8_t g_tnt_intro_sample_11d[8853];
extern uint8_t g_tnt_intro_sample_11e[8912];
extern uint8_t g_tnt_intro_sample_11f[13708];
extern uint8_t g_tnt_intro_sample_120[10402];
extern uint8_t g_tnt_intro_sample_121[14210];
extern uint8_t g_tnt_intro_sample_122[7367];
extern uint8_t g_tnt_intro_sample_123[12981];
extern uint8_t g_tnt_intro_sample_124[9220];
extern uint8_t g_tnt_intro_sample_125[3606];
extern uint8_t g_tnt_intro_sample_126[22017];
extern uint8_t g_tnt_intro_sfx_stream_0[1260];
extern uint8_t g_tnt_intro_sfx_stream_1[625];
extern uint8_t g_tnt_intro_sfx_stream_10[224];
extern uint8_t g_tnt_intro_sfx_stream_11[673];
extern uint8_t g_tnt_intro_sfx_stream_12[611];
extern uint8_t g_tnt_intro_sfx_stream_13[323];
extern uint8_t g_tnt_intro_sfx_stream_14[230];
extern uint8_t g_tnt_intro_sfx_stream_15[928];
extern uint8_t g_tnt_intro_sfx_stream_16[1126];
extern uint8_t g_tnt_intro_sfx_stream_17[977];
extern uint8_t g_tnt_intro_sfx_stream_18[142];
extern uint8_t g_tnt_intro_sfx_stream_19[141];
extern uint8_t g_tnt_intro_sfx_stream_2[970];
extern uint8_t g_tnt_intro_sfx_stream_20[311];
extern uint8_t g_tnt_intro_sfx_stream_21[335];
extern uint8_t g_tnt_intro_sfx_stream_22[204];
extern uint8_t g_tnt_intro_sfx_stream_23[647];
extern uint8_t g_tnt_intro_sfx_stream_24[533];
extern uint8_t g_tnt_intro_sfx_stream_25[531];
extern uint8_t g_tnt_intro_sfx_stream_26[963];
extern uint8_t g_tnt_intro_sfx_stream_27[445];
extern uint8_t g_tnt_intro_sfx_stream_3[196];
extern uint8_t g_tnt_intro_sfx_stream_4[190];
extern uint8_t g_tnt_intro_sfx_stream_5[198];
extern uint8_t g_tnt_intro_sfx_stream_6[210];
extern uint8_t g_tnt_intro_sfx_stream_7[558];
extern uint8_t g_tnt_intro_sfx_stream_8[578];
extern uint8_t g_tnt_intro_sfx_stream_9[240];
extern uint8_t g_tnt_japanese_butterfly_anim_1361[71960];
extern uint8_t g_tnt_japanese_complete_anim_756[31976];
extern uint8_t g_tnt_japanese_complete_anim_757[16560];
extern uint8_t g_tnt_japanese_complete_anim_758[35824];
extern uint16_t g_tnt_japanese_complete_texels[54800];
extern uint16_t g_tnt_main_menu_a_accept_rgba16[1065];
extern uint8_t g_tnt_main_menu_background_ci8[120000];
extern uint16_t g_tnt_main_menu_background_rgba16[256];
extern uint8_t g_tnt_main_menu_panel_ci8[65520];
extern uint16_t g_tnt_main_menu_panel_rgba16[256];
extern uint8_t g_tnt_main_menu_shadow_ia8[67308];
extern uint16_t g_tnt_mayan_4p_background_rgba16[256];
extern uint8_t g_tnt_mayan_background_ci8[120000];
extern uint16_t g_tnt_mayan_background_rgba16[256];
extern uint8_t g_tnt_mayan_moon_ia8[10206];
extern uint8_t g_tnt_mayan_playfield_ia8[20808];
extern uint8_t g_tnt_menu_sample_138[15094];
extern uint8_t g_tnt_menu_sample_139[2514];
extern uint8_t g_tnt_menu_sample_13a[5642];
extern uint8_t g_tnt_menu_sample_13b[2394];
extern uint8_t g_tnt_menu_sfx_stream_0[18];
extern uint8_t g_tnt_menu_sfx_stream_1[28];
extern uint8_t g_tnt_menu_sfx_stream_2[23];
extern uint8_t g_tnt_menu_sfx_stream_3[22];
extern uint8_t g_tnt_menu_sfx_stream_4[21];
extern uint8_t g_tnt_menu_sfx_stream_5[38];
extern uint8_t g_tnt_modal_rectangle_ia8[30000];
extern uint16_t g_tnt_multi_accept_back_rgba16[132*15];
extern uint8_t g_tnt_multi_panel_ci8[400*255];
extern uint16_t g_tnt_multi_panel_rgba16[256];
extern uint8_t g_tnt_multi_selection_box_ia8[154*30];
extern uint8_t g_tnt_multi_shadow_ia8[388*252];
extern uint16_t g_tnt_onep_a_accept_b_back_rgba16[1980];
extern uint8_t g_tnt_onep_cursor_ia8[676];
extern uint8_t g_tnt_onep_panel_ci8[99647];
extern uint16_t g_tnt_onep_panel_rgba16[256];
extern uint8_t g_tnt_onep_selection_box_ia8[4620];
extern uint8_t g_tnt_onep_shadow_ia8[99540];
extern uint8_t g_tnt_options_panel_ci8[66456];
extern uint16_t g_tnt_options_panel_rgba16[256];
extern uint8_t g_tnt_options_shadow_ia8[70720];
extern uint8_t g_tnt_russian_complete_anim_1400[8712];
extern uint8_t g_tnt_russian_complete_anim_889[20748];
extern uint8_t g_tnt_russian_complete_anim_890[10628];
extern uint8_t g_tnt_russian_complete_anim_891[8356];
extern uint16_t g_tnt_russian_complete_texels[56064];
extern uint8_t g_tnt_russian_sky_anim_902[100];
extern uint8_t g_tnt_russian_snow_anim_1371[1312];
extern uint8_t g_tnt_scores_gamecart_ia8[832];
extern uint8_t g_tnt_scores_panel_ci8[105465];
extern uint16_t g_tnt_scores_panel_rgba16[256];
extern uint8_t g_tnt_scores_shadow_ia8[110714];
extern uint8_t g_tnt_spotlight_128x128[16384];
extern uint8_t g_tnt_square_glow_ia8[66 * 66];
extern uint8_t g_tnt_tally_panel_ci8[99898];
extern uint16_t g_tnt_tally_panel_rgba16[256];
extern uint8_t g_tnt_tally_shadow_ia8[101400];
extern uint16_t g_tnt_tally_trophy_rgba16[28*30];
extern uint8_t g_tnt_title_ci8[120000];
extern uint16_t g_tnt_title_rgba16[256];
extern uint8_t g_tnt_wonder_arrow_left_ci8[1700];
extern uint16_t g_tnt_wonder_arrow_left_rgba16[256];
extern uint8_t g_tnt_wonder_arrow_right_ci8[1617];
extern uint16_t g_tnt_wonder_arrow_right_rgba16[256];
extern uint8_t g_tnt_wonder_complete_anim_1396[13384];
extern uint8_t g_tnt_wonder_complete_anim_187[18312];
extern uint8_t g_tnt_wonder_complete_anim_188[36176];
extern uint8_t g_tnt_wonder_complete_anim_189[13472];
extern uint16_t g_tnt_wonder_complete_texels[130320];
extern uint8_t g_tnt_wonder_overlay_ci8[100838];
extern uint16_t g_tnt_wonder_overlay_rgba16[256];
extern uint8_t g_tnt_wonder_pie_a_ci8[7630];
extern uint16_t g_tnt_wonder_pie_a_rgba16[256];
extern uint8_t g_tnt_wonder_pie_b_ci8[7630];
extern uint16_t g_tnt_wonder_pie_b_rgba16[256];
extern uint8_t g_tnt_wonder_pie_c_ci8[7630];
extern uint16_t g_tnt_wonder_pie_c_rgba16[256];
extern uint8_t g_tnt_wonder_pie_d_ci8[7630];
extern uint16_t g_tnt_wonder_pie_d_rgba16[256];
extern uint8_t g_tnt_wonder_pie_e_ci8[7630];
extern uint16_t g_tnt_wonder_pie_e_rgba16[256];
extern uint8_t g_tnt_wonder_pie_f_ci8[7630];
extern uint16_t g_tnt_wonder_pie_f_rgba16[256];
extern uint8_t g_tnt_wonder_pie_g_ci8[7630];
extern uint16_t g_tnt_wonder_pie_g_rgba16[256];
extern uint8_t g_tnt_wonder_pie_h_ci8[7630];
extern uint16_t g_tnt_wonder_pie_h_rgba16[256];

extern uint8_t g_tnt_african_background_packbits[87473];
extern uint16_t g_tnt_african_background_rgba16[256];
extern uint8_t g_tnt_african_smoke_ci8[TNT_AFRICAN_SMOKE_FRAME_COUNT][131*106];
extern uint16_t g_tnt_african_smoke_rgba16[TNT_AFRICAN_SMOKE_FRAME_COUNT][256];
extern uint8_t g_tnt_celtic_background_packbits[91883];
extern uint16_t g_tnt_celtic_background_rgba16[256];
extern uint8_t g_tnt_celtic_lamp_ci8[4][61*60];
extern uint16_t g_tnt_celtic_lamp_rgba16[4][256];
extern uint8_t g_tnt_japanese_background_packbits[67534];
extern uint16_t g_tnt_japanese_background_rgba16[256];
extern uint8_t g_tnt_japanese_sparkle_ci8[TNT_JAPANESE_SPARKLE_FRAME_COUNT][30*29];
extern uint16_t g_tnt_japanese_sparkle_rgba16[TNT_JAPANESE_SPARKLE_FRAME_COUNT][256];
extern uint8_t g_tnt_japanese_water_ci8[TNT_JAPANESE_WATER_FRAME_COUNT][254*32];
extern uint16_t g_tnt_japanese_water_rgba16[TNT_JAPANESE_WATER_FRAME_COUNT][256];
extern uint8_t g_tnt_russian_background_packbits[86608];
extern uint16_t g_tnt_russian_background_rgba16[256];
extern uint8_t g_tnt_russian_candle_ci8[TNT_RUSSIAN_CANDLE_FRAME_COUNT][2*6];
extern uint16_t g_tnt_russian_candle_rgba16[TNT_RUSSIAN_CANDLE_FRAME_COUNT][256];

/* Private cached 2P ROM resources. Original pointer tables remain unchanged. */
extern uint8_t s_african_2p_bg_packbits[118734];
extern uint16_t s_african_2p_bg_pal[256];
extern uint8_t s_african_2p_time[1920];
extern uint16_t s_african_2p_time_pal[256];
extern uint8_t s_celtic_2p_bg_packbits[113825];
extern uint16_t s_celtic_2p_bg_pal[256];
extern uint8_t s_celtic_2p_time[1920];
extern uint16_t s_celtic_2p_time_pal[256];
extern uint8_t s_egyptian_2p_bg_packbits[115869];
extern uint16_t s_egyptian_2p_bg_pal[256];
extern uint8_t s_egyptian_2p_time[2176];
extern uint16_t s_egyptian_2p_time_pal[256];
extern uint8_t s_greek_2p_bg_packbits[114336];
extern uint16_t s_greek_2p_bg_pal[256];
extern uint8_t s_greek_2p_time[1920];
extern uint16_t s_greek_2p_time_pal[256];
extern uint8_t s_japanese_2p_bg_packbits[111196];
extern uint16_t s_japanese_2p_bg_pal[256];
extern uint8_t s_japanese_2p_time[1920];
extern uint16_t s_japanese_2p_time_pal[256];
extern uint8_t s_mayan_2p_bg_packbits[114331];
extern uint16_t s_mayan_2p_bg_pal[256];
extern uint8_t s_mayan_2p_time[1920];
extern uint16_t s_mayan_2p_time_pal[256];
extern uint8_t s_russian_2p_bg_packbits[111221];
extern uint16_t s_russian_2p_bg_pal[256];
extern uint8_t s_russian_2p_time[1920];
extern uint16_t s_russian_2p_time_pal[256];

/* Original V6 ROM-cache art, never bundled in executable. */
extern uint8_t g_tnt_egyptian_background_packbits[105326];
extern uint16_t g_tnt_egyptian_background_rgba16[256];
extern uint8_t g_tnt_finale_background_packbits[101702];
extern uint16_t g_tnt_finale_background_rgba16[256];
extern uint8_t g_tnt_finale_boiler_ci8[121*119];
extern uint16_t g_tnt_finale_boiler_rgba16[256];
extern uint8_t g_tnt_finale_bulb_ci8[21*21];
extern uint16_t g_tnt_finale_bulb_rgba16[256];
extern uint8_t g_tnt_finale_gauge_ci8[TNT_FINALE_GAUGE_FRAME_COUNT][10*9];
extern uint16_t g_tnt_finale_gauge_rgba16[TNT_FINALE_GAUGE_FRAME_COUNT][256];
extern uint8_t g_tnt_greek_background_ci8[400 * 300];
extern uint16_t g_tnt_greek_background_rgba16[256];

static const TNTVBridgeBinding g_tntv_bridge_bindings[] = {
    {"g_tnt_african_complete_anim_558",(void*)g_tnt_african_complete_anim_558,sizeof(g_tnt_african_complete_anim_558),0},
    {"g_tnt_african_complete_anim_559",(void*)g_tnt_african_complete_anim_559,sizeof(g_tnt_african_complete_anim_559),0},
    {"g_tnt_african_complete_anim_560",(void*)g_tnt_african_complete_anim_560,sizeof(g_tnt_african_complete_anim_560),0},
    {"g_tnt_african_complete_texels",(void*)g_tnt_african_complete_texels,sizeof(g_tnt_african_complete_texels),1},
    {"g_tnt_african_sky_anim_568",(void*)g_tnt_african_sky_anim_568,sizeof(g_tnt_african_sky_anim_568),0},
    {"g_tnt_audio_panel_ci8",(void*)g_tnt_audio_panel_ci8,sizeof(g_tnt_audio_panel_ci8),0},
    {"g_tnt_audio_panel_rgba16",(void*)g_tnt_audio_panel_rgba16,sizeof(g_tnt_audio_panel_rgba16),1},
    {"g_tnt_audio_shadow_ia8",(void*)g_tnt_audio_shadow_ia8,sizeof(g_tnt_audio_shadow_ia8),0},
    {"g_tnt_audio_slider_feather_rgba16",(void*)g_tnt_audio_slider_feather_rgba16,sizeof(g_tnt_audio_slider_feather_rgba16),1},
    {"g_tnt_audio_slider_wood_rgba16",(void*)g_tnt_audio_slider_wood_rgba16,sizeof(g_tnt_audio_slider_wood_rgba16),1},
    {"g_tnt_celtic_complete_anim_471",(void*)g_tnt_celtic_complete_anim_471,sizeof(g_tnt_celtic_complete_anim_471),0},
    {"g_tnt_celtic_complete_anim_472",(void*)g_tnt_celtic_complete_anim_472,sizeof(g_tnt_celtic_complete_anim_472),0},
    {"g_tnt_celtic_complete_anim_473",(void*)g_tnt_celtic_complete_anim_473,sizeof(g_tnt_celtic_complete_anim_473),0},
    {"g_tnt_celtic_complete_texels",(void*)g_tnt_celtic_complete_texels,sizeof(g_tnt_celtic_complete_texels),1},
    {"g_tnt_celtic_leaf_anim_1336",(void*)g_tnt_celtic_leaf_anim_1336,sizeof(g_tnt_celtic_leaf_anim_1336),0},
    {"g_tnt_celtic_sky_anim_488",(void*)g_tnt_celtic_sky_anim_488,sizeof(g_tnt_celtic_sky_anim_488),0},
    {"g_tnt_credits_background_ci8",(void*)g_tnt_credits_background_ci8,sizeof(g_tnt_credits_background_ci8),0},
    {"g_tnt_credits_background_rgb",(void*)g_tnt_credits_background_rgb,sizeof(g_tnt_credits_background_rgb),0},
    {"g_tnt_credits_height_i8",(void*)g_tnt_credits_height_i8,sizeof(g_tnt_credits_height_i8),0},
    {"g_tnt_credits_spotlight_ci8",(void*)g_tnt_credits_spotlight_ci8,sizeof(g_tnt_credits_spotlight_ci8),0},
    {"g_tnt_credits_spotlight_rgb",(void*)g_tnt_credits_spotlight_rgb,sizeof(g_tnt_credits_spotlight_rgb),0},
    {"g_tnt_data_panel_ci8",(void*)g_tnt_data_panel_ci8,sizeof(g_tnt_data_panel_ci8),0},
    {"g_tnt_data_panel_rgba16",(void*)g_tnt_data_panel_rgba16,sizeof(g_tnt_data_panel_rgba16),1},
    {"g_tnt_data_shadow_ia8",(void*)g_tnt_data_shadow_ia8,sizeof(g_tnt_data_shadow_ia8),0},
    {"g_tnt_developer_ci8",(void*)g_tnt_developer_ci8,sizeof(g_tnt_developer_ci8),0},
    {"g_tnt_developer_rgba16",(void*)g_tnt_developer_rgba16,sizeof(g_tnt_developer_rgba16),1},
    {"g_tnt_egyptian_complete_anim_375",(void*)g_tnt_egyptian_complete_anim_375,sizeof(g_tnt_egyptian_complete_anim_375),0},
    {"g_tnt_egyptian_complete_anim_376",(void*)g_tnt_egyptian_complete_anim_376,sizeof(g_tnt_egyptian_complete_anim_376),0},
    {"g_tnt_egyptian_complete_anim_377",(void*)g_tnt_egyptian_complete_anim_377,sizeof(g_tnt_egyptian_complete_anim_377),0},
    {"g_tnt_egyptian_complete_texels",(void*)g_tnt_egyptian_complete_texels,sizeof(g_tnt_egyptian_complete_texels),1},
    {"g_tnt_finish_game_over_rgba16",(void*)g_tnt_finish_game_over_rgba16,sizeof(g_tnt_finish_game_over_rgba16),1},
    {"g_tnt_finish_lose_rgba16",(void*)g_tnt_finish_lose_rgba16,sizeof(g_tnt_finish_lose_rgba16),1},
    {"g_tnt_finish_win_rgba16",(void*)g_tnt_finish_win_rgba16,sizeof(g_tnt_finish_win_rgba16),1},
    {"g_tnt_font_a_ia8",(void*)g_tnt_font_a_ia8,sizeof(g_tnt_font_a_ia8),0},
    {"g_tnt_font_b_ia8",(void*)g_tnt_font_b_ia8,sizeof(g_tnt_font_b_ia8),0},
    {"g_tnt_font_c_ia8",(void*)g_tnt_font_c_ia8,sizeof(g_tnt_font_c_ia8),0},
    {"g_tnt_game_sample_125",(void*)g_tnt_game_sample_125,sizeof(g_tnt_game_sample_125),0},
    {"g_tnt_game_sample_127",(void*)g_tnt_game_sample_127,sizeof(g_tnt_game_sample_127),0},
    {"g_tnt_game_sample_128",(void*)g_tnt_game_sample_128,sizeof(g_tnt_game_sample_128),0},
    {"g_tnt_game_sample_129",(void*)g_tnt_game_sample_129,sizeof(g_tnt_game_sample_129),0},
    {"g_tnt_game_sample_12a",(void*)g_tnt_game_sample_12a,sizeof(g_tnt_game_sample_12a),0},
    {"g_tnt_game_sample_12b",(void*)g_tnt_game_sample_12b,sizeof(g_tnt_game_sample_12b),0},
    {"g_tnt_game_sample_12c",(void*)g_tnt_game_sample_12c,sizeof(g_tnt_game_sample_12c),0},
    {"g_tnt_game_sample_12d",(void*)g_tnt_game_sample_12d,sizeof(g_tnt_game_sample_12d),0},
    {"g_tnt_game_sample_12e",(void*)g_tnt_game_sample_12e,sizeof(g_tnt_game_sample_12e),0},
    {"g_tnt_game_sample_12f",(void*)g_tnt_game_sample_12f,sizeof(g_tnt_game_sample_12f),0},
    {"g_tnt_game_sample_130",(void*)g_tnt_game_sample_130,sizeof(g_tnt_game_sample_130),0},
    {"g_tnt_game_sample_131",(void*)g_tnt_game_sample_131,sizeof(g_tnt_game_sample_131),0},
    {"g_tnt_game_sample_132",(void*)g_tnt_game_sample_132,sizeof(g_tnt_game_sample_132),0},
    {"g_tnt_game_sample_133",(void*)g_tnt_game_sample_133,sizeof(g_tnt_game_sample_133),0},
    {"g_tnt_game_sample_134",(void*)g_tnt_game_sample_134,sizeof(g_tnt_game_sample_134),0},
    {"g_tnt_game_sample_135",(void*)g_tnt_game_sample_135,sizeof(g_tnt_game_sample_135),0},
    {"g_tnt_game_sample_136",(void*)g_tnt_game_sample_136,sizeof(g_tnt_game_sample_136),0},
    {"g_tnt_game_sample_137",(void*)g_tnt_game_sample_137,sizeof(g_tnt_game_sample_137),0},
    {"g_tnt_game_sfx_stream_0",(void*)g_tnt_game_sfx_stream_0,sizeof(g_tnt_game_sfx_stream_0),0},
    {"g_tnt_game_sfx_stream_1",(void*)g_tnt_game_sfx_stream_1,sizeof(g_tnt_game_sfx_stream_1),0},
    {"g_tnt_game_sfx_stream_10",(void*)g_tnt_game_sfx_stream_10,sizeof(g_tnt_game_sfx_stream_10),0},
    {"g_tnt_game_sfx_stream_11",(void*)g_tnt_game_sfx_stream_11,sizeof(g_tnt_game_sfx_stream_11),0},
    {"g_tnt_game_sfx_stream_12",(void*)g_tnt_game_sfx_stream_12,sizeof(g_tnt_game_sfx_stream_12),0},
    {"g_tnt_game_sfx_stream_13",(void*)g_tnt_game_sfx_stream_13,sizeof(g_tnt_game_sfx_stream_13),0},
    {"g_tnt_game_sfx_stream_14",(void*)g_tnt_game_sfx_stream_14,sizeof(g_tnt_game_sfx_stream_14),0},
    {"g_tnt_game_sfx_stream_15",(void*)g_tnt_game_sfx_stream_15,sizeof(g_tnt_game_sfx_stream_15),0},
    {"g_tnt_game_sfx_stream_16",(void*)g_tnt_game_sfx_stream_16,sizeof(g_tnt_game_sfx_stream_16),0},
    {"g_tnt_game_sfx_stream_17",(void*)g_tnt_game_sfx_stream_17,sizeof(g_tnt_game_sfx_stream_17),0},
    {"g_tnt_game_sfx_stream_18",(void*)g_tnt_game_sfx_stream_18,sizeof(g_tnt_game_sfx_stream_18),0},
    {"g_tnt_game_sfx_stream_19",(void*)g_tnt_game_sfx_stream_19,sizeof(g_tnt_game_sfx_stream_19),0},
    {"g_tnt_game_sfx_stream_2",(void*)g_tnt_game_sfx_stream_2,sizeof(g_tnt_game_sfx_stream_2),0},
    {"g_tnt_game_sfx_stream_20",(void*)g_tnt_game_sfx_stream_20,sizeof(g_tnt_game_sfx_stream_20),0},
    {"g_tnt_game_sfx_stream_21",(void*)g_tnt_game_sfx_stream_21,sizeof(g_tnt_game_sfx_stream_21),0},
    {"g_tnt_game_sfx_stream_22",(void*)g_tnt_game_sfx_stream_22,sizeof(g_tnt_game_sfx_stream_22),0},
    {"g_tnt_game_sfx_stream_23",(void*)g_tnt_game_sfx_stream_23,sizeof(g_tnt_game_sfx_stream_23),0},
    {"g_tnt_game_sfx_stream_24",(void*)g_tnt_game_sfx_stream_24,sizeof(g_tnt_game_sfx_stream_24),0},
    {"g_tnt_game_sfx_stream_25",(void*)g_tnt_game_sfx_stream_25,sizeof(g_tnt_game_sfx_stream_25),0},
    {"g_tnt_game_sfx_stream_26",(void*)g_tnt_game_sfx_stream_26,sizeof(g_tnt_game_sfx_stream_26),0},
    {"g_tnt_game_sfx_stream_27",(void*)g_tnt_game_sfx_stream_27,sizeof(g_tnt_game_sfx_stream_27),0},
    {"g_tnt_game_sfx_stream_28",(void*)g_tnt_game_sfx_stream_28,sizeof(g_tnt_game_sfx_stream_28),0},
    {"g_tnt_game_sfx_stream_29",(void*)g_tnt_game_sfx_stream_29,sizeof(g_tnt_game_sfx_stream_29),0},
    {"g_tnt_game_sfx_stream_3",(void*)g_tnt_game_sfx_stream_3,sizeof(g_tnt_game_sfx_stream_3),0},
    {"g_tnt_game_sfx_stream_30",(void*)g_tnt_game_sfx_stream_30,sizeof(g_tnt_game_sfx_stream_30),0},
    {"g_tnt_game_sfx_stream_4",(void*)g_tnt_game_sfx_stream_4,sizeof(g_tnt_game_sfx_stream_4),0},
    {"g_tnt_game_sfx_stream_5",(void*)g_tnt_game_sfx_stream_5,sizeof(g_tnt_game_sfx_stream_5),0},
    {"g_tnt_game_sfx_stream_6",(void*)g_tnt_game_sfx_stream_6,sizeof(g_tnt_game_sfx_stream_6),0},
    {"g_tnt_game_sfx_stream_7",(void*)g_tnt_game_sfx_stream_7,sizeof(g_tnt_game_sfx_stream_7),0},
    {"g_tnt_game_sfx_stream_8",(void*)g_tnt_game_sfx_stream_8,sizeof(g_tnt_game_sfx_stream_8),0},
    {"g_tnt_game_sfx_stream_9",(void*)g_tnt_game_sfx_stream_9,sizeof(g_tnt_game_sfx_stream_9),0},
    {"g_tnt_greek_complete_anim_315",(void*)g_tnt_greek_complete_anim_315,sizeof(g_tnt_greek_complete_anim_315),0},
    {"g_tnt_greek_complete_anim_316",(void*)g_tnt_greek_complete_anim_316,sizeof(g_tnt_greek_complete_anim_316),0},
    {"g_tnt_greek_complete_anim_317",(void*)g_tnt_greek_complete_anim_317,sizeof(g_tnt_greek_complete_anim_317),0},
    {"g_tnt_greek_complete_texels",(void*)g_tnt_greek_complete_texels,sizeof(g_tnt_greek_complete_texels),1},
    {"g_tnt_greek_rain_anim_1319",(void*)g_tnt_greek_rain_anim_1319,sizeof(g_tnt_greek_rain_anim_1319),0},
    {"g_tnt_intro_sample_118",(void*)g_tnt_intro_sample_118,sizeof(g_tnt_intro_sample_118),0},
    {"g_tnt_intro_sample_119",(void*)g_tnt_intro_sample_119,sizeof(g_tnt_intro_sample_119),0},
    {"g_tnt_intro_sample_11a",(void*)g_tnt_intro_sample_11a,sizeof(g_tnt_intro_sample_11a),0},
    {"g_tnt_intro_sample_11b",(void*)g_tnt_intro_sample_11b,sizeof(g_tnt_intro_sample_11b),0},
    {"g_tnt_intro_sample_11c",(void*)g_tnt_intro_sample_11c,sizeof(g_tnt_intro_sample_11c),0},
    {"g_tnt_intro_sample_11d",(void*)g_tnt_intro_sample_11d,sizeof(g_tnt_intro_sample_11d),0},
    {"g_tnt_intro_sample_11e",(void*)g_tnt_intro_sample_11e,sizeof(g_tnt_intro_sample_11e),0},
    {"g_tnt_intro_sample_11f",(void*)g_tnt_intro_sample_11f,sizeof(g_tnt_intro_sample_11f),0},
    {"g_tnt_intro_sample_120",(void*)g_tnt_intro_sample_120,sizeof(g_tnt_intro_sample_120),0},
    {"g_tnt_intro_sample_121",(void*)g_tnt_intro_sample_121,sizeof(g_tnt_intro_sample_121),0},
    {"g_tnt_intro_sample_122",(void*)g_tnt_intro_sample_122,sizeof(g_tnt_intro_sample_122),0},
    {"g_tnt_intro_sample_123",(void*)g_tnt_intro_sample_123,sizeof(g_tnt_intro_sample_123),0},
    {"g_tnt_intro_sample_124",(void*)g_tnt_intro_sample_124,sizeof(g_tnt_intro_sample_124),0},
    {"g_tnt_intro_sample_125",(void*)g_tnt_intro_sample_125,sizeof(g_tnt_intro_sample_125),0},
    {"g_tnt_intro_sample_126",(void*)g_tnt_intro_sample_126,sizeof(g_tnt_intro_sample_126),0},
    {"g_tnt_intro_sfx_stream_0",(void*)g_tnt_intro_sfx_stream_0,sizeof(g_tnt_intro_sfx_stream_0),0},
    {"g_tnt_intro_sfx_stream_1",(void*)g_tnt_intro_sfx_stream_1,sizeof(g_tnt_intro_sfx_stream_1),0},
    {"g_tnt_intro_sfx_stream_10",(void*)g_tnt_intro_sfx_stream_10,sizeof(g_tnt_intro_sfx_stream_10),0},
    {"g_tnt_intro_sfx_stream_11",(void*)g_tnt_intro_sfx_stream_11,sizeof(g_tnt_intro_sfx_stream_11),0},
    {"g_tnt_intro_sfx_stream_12",(void*)g_tnt_intro_sfx_stream_12,sizeof(g_tnt_intro_sfx_stream_12),0},
    {"g_tnt_intro_sfx_stream_13",(void*)g_tnt_intro_sfx_stream_13,sizeof(g_tnt_intro_sfx_stream_13),0},
    {"g_tnt_intro_sfx_stream_14",(void*)g_tnt_intro_sfx_stream_14,sizeof(g_tnt_intro_sfx_stream_14),0},
    {"g_tnt_intro_sfx_stream_15",(void*)g_tnt_intro_sfx_stream_15,sizeof(g_tnt_intro_sfx_stream_15),0},
    {"g_tnt_intro_sfx_stream_16",(void*)g_tnt_intro_sfx_stream_16,sizeof(g_tnt_intro_sfx_stream_16),0},
    {"g_tnt_intro_sfx_stream_17",(void*)g_tnt_intro_sfx_stream_17,sizeof(g_tnt_intro_sfx_stream_17),0},
    {"g_tnt_intro_sfx_stream_18",(void*)g_tnt_intro_sfx_stream_18,sizeof(g_tnt_intro_sfx_stream_18),0},
    {"g_tnt_intro_sfx_stream_19",(void*)g_tnt_intro_sfx_stream_19,sizeof(g_tnt_intro_sfx_stream_19),0},
    {"g_tnt_intro_sfx_stream_2",(void*)g_tnt_intro_sfx_stream_2,sizeof(g_tnt_intro_sfx_stream_2),0},
    {"g_tnt_intro_sfx_stream_20",(void*)g_tnt_intro_sfx_stream_20,sizeof(g_tnt_intro_sfx_stream_20),0},
    {"g_tnt_intro_sfx_stream_21",(void*)g_tnt_intro_sfx_stream_21,sizeof(g_tnt_intro_sfx_stream_21),0},
    {"g_tnt_intro_sfx_stream_22",(void*)g_tnt_intro_sfx_stream_22,sizeof(g_tnt_intro_sfx_stream_22),0},
    {"g_tnt_intro_sfx_stream_23",(void*)g_tnt_intro_sfx_stream_23,sizeof(g_tnt_intro_sfx_stream_23),0},
    {"g_tnt_intro_sfx_stream_24",(void*)g_tnt_intro_sfx_stream_24,sizeof(g_tnt_intro_sfx_stream_24),0},
    {"g_tnt_intro_sfx_stream_25",(void*)g_tnt_intro_sfx_stream_25,sizeof(g_tnt_intro_sfx_stream_25),0},
    {"g_tnt_intro_sfx_stream_26",(void*)g_tnt_intro_sfx_stream_26,sizeof(g_tnt_intro_sfx_stream_26),0},
    {"g_tnt_intro_sfx_stream_27",(void*)g_tnt_intro_sfx_stream_27,sizeof(g_tnt_intro_sfx_stream_27),0},
    {"g_tnt_intro_sfx_stream_3",(void*)g_tnt_intro_sfx_stream_3,sizeof(g_tnt_intro_sfx_stream_3),0},
    {"g_tnt_intro_sfx_stream_4",(void*)g_tnt_intro_sfx_stream_4,sizeof(g_tnt_intro_sfx_stream_4),0},
    {"g_tnt_intro_sfx_stream_5",(void*)g_tnt_intro_sfx_stream_5,sizeof(g_tnt_intro_sfx_stream_5),0},
    {"g_tnt_intro_sfx_stream_6",(void*)g_tnt_intro_sfx_stream_6,sizeof(g_tnt_intro_sfx_stream_6),0},
    {"g_tnt_intro_sfx_stream_7",(void*)g_tnt_intro_sfx_stream_7,sizeof(g_tnt_intro_sfx_stream_7),0},
    {"g_tnt_intro_sfx_stream_8",(void*)g_tnt_intro_sfx_stream_8,sizeof(g_tnt_intro_sfx_stream_8),0},
    {"g_tnt_intro_sfx_stream_9",(void*)g_tnt_intro_sfx_stream_9,sizeof(g_tnt_intro_sfx_stream_9),0},
    {"g_tnt_japanese_butterfly_anim_1361",(void*)g_tnt_japanese_butterfly_anim_1361,sizeof(g_tnt_japanese_butterfly_anim_1361),0},
    {"g_tnt_japanese_complete_anim_756",(void*)g_tnt_japanese_complete_anim_756,sizeof(g_tnt_japanese_complete_anim_756),0},
    {"g_tnt_japanese_complete_anim_757",(void*)g_tnt_japanese_complete_anim_757,sizeof(g_tnt_japanese_complete_anim_757),0},
    {"g_tnt_japanese_complete_anim_758",(void*)g_tnt_japanese_complete_anim_758,sizeof(g_tnt_japanese_complete_anim_758),0},
    {"g_tnt_japanese_complete_texels",(void*)g_tnt_japanese_complete_texels,sizeof(g_tnt_japanese_complete_texels),1},
    {"g_tnt_main_menu_a_accept_rgba16",(void*)g_tnt_main_menu_a_accept_rgba16,sizeof(g_tnt_main_menu_a_accept_rgba16),1},
    {"g_tnt_main_menu_background_ci8",(void*)g_tnt_main_menu_background_ci8,sizeof(g_tnt_main_menu_background_ci8),0},
    {"g_tnt_main_menu_background_rgba16",(void*)g_tnt_main_menu_background_rgba16,sizeof(g_tnt_main_menu_background_rgba16),1},
    {"g_tnt_main_menu_panel_ci8",(void*)g_tnt_main_menu_panel_ci8,sizeof(g_tnt_main_menu_panel_ci8),0},
    {"g_tnt_main_menu_panel_rgba16",(void*)g_tnt_main_menu_panel_rgba16,sizeof(g_tnt_main_menu_panel_rgba16),1},
    {"g_tnt_main_menu_shadow_ia8",(void*)g_tnt_main_menu_shadow_ia8,sizeof(g_tnt_main_menu_shadow_ia8),0},
    {"g_tnt_mayan_4p_background_rgba16",(void*)g_tnt_mayan_4p_background_rgba16,sizeof(g_tnt_mayan_4p_background_rgba16),1},
    {"g_tnt_mayan_background_ci8",(void*)g_tnt_mayan_background_ci8,sizeof(g_tnt_mayan_background_ci8),0},
    {"g_tnt_mayan_background_rgba16",(void*)g_tnt_mayan_background_rgba16,sizeof(g_tnt_mayan_background_rgba16),1},
    {"g_tnt_mayan_moon_ia8",(void*)g_tnt_mayan_moon_ia8,sizeof(g_tnt_mayan_moon_ia8),0},
    {"g_tnt_mayan_playfield_ia8",(void*)g_tnt_mayan_playfield_ia8,sizeof(g_tnt_mayan_playfield_ia8),0},
    {"g_tnt_menu_sample_138",(void*)g_tnt_menu_sample_138,sizeof(g_tnt_menu_sample_138),0},
    {"g_tnt_menu_sample_139",(void*)g_tnt_menu_sample_139,sizeof(g_tnt_menu_sample_139),0},
    {"g_tnt_menu_sample_13a",(void*)g_tnt_menu_sample_13a,sizeof(g_tnt_menu_sample_13a),0},
    {"g_tnt_menu_sample_13b",(void*)g_tnt_menu_sample_13b,sizeof(g_tnt_menu_sample_13b),0},
    {"g_tnt_menu_sfx_stream_0",(void*)g_tnt_menu_sfx_stream_0,sizeof(g_tnt_menu_sfx_stream_0),0},
    {"g_tnt_menu_sfx_stream_1",(void*)g_tnt_menu_sfx_stream_1,sizeof(g_tnt_menu_sfx_stream_1),0},
    {"g_tnt_menu_sfx_stream_2",(void*)g_tnt_menu_sfx_stream_2,sizeof(g_tnt_menu_sfx_stream_2),0},
    {"g_tnt_menu_sfx_stream_3",(void*)g_tnt_menu_sfx_stream_3,sizeof(g_tnt_menu_sfx_stream_3),0},
    {"g_tnt_menu_sfx_stream_4",(void*)g_tnt_menu_sfx_stream_4,sizeof(g_tnt_menu_sfx_stream_4),0},
    {"g_tnt_menu_sfx_stream_5",(void*)g_tnt_menu_sfx_stream_5,sizeof(g_tnt_menu_sfx_stream_5),0},
    {"g_tnt_modal_rectangle_ia8",(void*)g_tnt_modal_rectangle_ia8,sizeof(g_tnt_modal_rectangle_ia8),0},
    {"g_tnt_multi_accept_back_rgba16",(void*)g_tnt_multi_accept_back_rgba16,sizeof(g_tnt_multi_accept_back_rgba16),1},
    {"g_tnt_multi_panel_ci8",(void*)g_tnt_multi_panel_ci8,sizeof(g_tnt_multi_panel_ci8),0},
    {"g_tnt_multi_panel_rgba16",(void*)g_tnt_multi_panel_rgba16,sizeof(g_tnt_multi_panel_rgba16),1},
    {"g_tnt_multi_selection_box_ia8",(void*)g_tnt_multi_selection_box_ia8,sizeof(g_tnt_multi_selection_box_ia8),0},
    {"g_tnt_multi_shadow_ia8",(void*)g_tnt_multi_shadow_ia8,sizeof(g_tnt_multi_shadow_ia8),0},
    {"g_tnt_onep_a_accept_b_back_rgba16",(void*)g_tnt_onep_a_accept_b_back_rgba16,sizeof(g_tnt_onep_a_accept_b_back_rgba16),1},
    {"g_tnt_onep_cursor_ia8",(void*)g_tnt_onep_cursor_ia8,sizeof(g_tnt_onep_cursor_ia8),0},
    {"g_tnt_onep_panel_ci8",(void*)g_tnt_onep_panel_ci8,sizeof(g_tnt_onep_panel_ci8),0},
    {"g_tnt_onep_panel_rgba16",(void*)g_tnt_onep_panel_rgba16,sizeof(g_tnt_onep_panel_rgba16),1},
    {"g_tnt_onep_selection_box_ia8",(void*)g_tnt_onep_selection_box_ia8,sizeof(g_tnt_onep_selection_box_ia8),0},
    {"g_tnt_onep_shadow_ia8",(void*)g_tnt_onep_shadow_ia8,sizeof(g_tnt_onep_shadow_ia8),0},
    {"g_tnt_options_panel_ci8",(void*)g_tnt_options_panel_ci8,sizeof(g_tnt_options_panel_ci8),0},
    {"g_tnt_options_panel_rgba16",(void*)g_tnt_options_panel_rgba16,sizeof(g_tnt_options_panel_rgba16),1},
    {"g_tnt_options_shadow_ia8",(void*)g_tnt_options_shadow_ia8,sizeof(g_tnt_options_shadow_ia8),0},
    {"g_tnt_russian_complete_anim_1400",(void*)g_tnt_russian_complete_anim_1400,sizeof(g_tnt_russian_complete_anim_1400),0},
    {"g_tnt_russian_complete_anim_889",(void*)g_tnt_russian_complete_anim_889,sizeof(g_tnt_russian_complete_anim_889),0},
    {"g_tnt_russian_complete_anim_890",(void*)g_tnt_russian_complete_anim_890,sizeof(g_tnt_russian_complete_anim_890),0},
    {"g_tnt_russian_complete_anim_891",(void*)g_tnt_russian_complete_anim_891,sizeof(g_tnt_russian_complete_anim_891),0},
    {"g_tnt_russian_complete_texels",(void*)g_tnt_russian_complete_texels,sizeof(g_tnt_russian_complete_texels),1},
    {"g_tnt_russian_sky_anim_902",(void*)g_tnt_russian_sky_anim_902,sizeof(g_tnt_russian_sky_anim_902),0},
    {"g_tnt_russian_snow_anim_1371",(void*)g_tnt_russian_snow_anim_1371,sizeof(g_tnt_russian_snow_anim_1371),0},
    {"g_tnt_scores_gamecart_ia8",(void*)g_tnt_scores_gamecart_ia8,sizeof(g_tnt_scores_gamecart_ia8),0},
    {"g_tnt_scores_panel_ci8",(void*)g_tnt_scores_panel_ci8,sizeof(g_tnt_scores_panel_ci8),0},
    {"g_tnt_scores_panel_rgba16",(void*)g_tnt_scores_panel_rgba16,sizeof(g_tnt_scores_panel_rgba16),1},
    {"g_tnt_scores_shadow_ia8",(void*)g_tnt_scores_shadow_ia8,sizeof(g_tnt_scores_shadow_ia8),0},
    {"g_tnt_spotlight_128x128",(void*)g_tnt_spotlight_128x128,sizeof(g_tnt_spotlight_128x128),0},
    {"g_tnt_square_glow_ia8",(void*)g_tnt_square_glow_ia8,sizeof(g_tnt_square_glow_ia8),0},
    {"g_tnt_tally_panel_ci8",(void*)g_tnt_tally_panel_ci8,sizeof(g_tnt_tally_panel_ci8),0},
    {"g_tnt_tally_panel_rgba16",(void*)g_tnt_tally_panel_rgba16,sizeof(g_tnt_tally_panel_rgba16),1},
    {"g_tnt_tally_shadow_ia8",(void*)g_tnt_tally_shadow_ia8,sizeof(g_tnt_tally_shadow_ia8),0},
    {"g_tnt_tally_trophy_rgba16",(void*)g_tnt_tally_trophy_rgba16,sizeof(g_tnt_tally_trophy_rgba16),1},
    {"g_tnt_title_ci8",(void*)g_tnt_title_ci8,sizeof(g_tnt_title_ci8),0},
    {"g_tnt_title_rgba16",(void*)g_tnt_title_rgba16,sizeof(g_tnt_title_rgba16),1},
    {"g_tnt_wonder_arrow_left_ci8",(void*)g_tnt_wonder_arrow_left_ci8,sizeof(g_tnt_wonder_arrow_left_ci8),0},
    {"g_tnt_wonder_arrow_left_rgba16",(void*)g_tnt_wonder_arrow_left_rgba16,sizeof(g_tnt_wonder_arrow_left_rgba16),1},
    {"g_tnt_wonder_arrow_right_ci8",(void*)g_tnt_wonder_arrow_right_ci8,sizeof(g_tnt_wonder_arrow_right_ci8),0},
    {"g_tnt_wonder_arrow_right_rgba16",(void*)g_tnt_wonder_arrow_right_rgba16,sizeof(g_tnt_wonder_arrow_right_rgba16),1},
    {"g_tnt_wonder_complete_anim_1396",(void*)g_tnt_wonder_complete_anim_1396,sizeof(g_tnt_wonder_complete_anim_1396),0},
    {"g_tnt_wonder_complete_anim_187",(void*)g_tnt_wonder_complete_anim_187,sizeof(g_tnt_wonder_complete_anim_187),0},
    {"g_tnt_wonder_complete_anim_188",(void*)g_tnt_wonder_complete_anim_188,sizeof(g_tnt_wonder_complete_anim_188),0},
    {"g_tnt_wonder_complete_anim_189",(void*)g_tnt_wonder_complete_anim_189,sizeof(g_tnt_wonder_complete_anim_189),0},
    {"g_tnt_wonder_complete_texels",(void*)g_tnt_wonder_complete_texels,sizeof(g_tnt_wonder_complete_texels),1},
    {"g_tnt_wonder_overlay_ci8",(void*)g_tnt_wonder_overlay_ci8,sizeof(g_tnt_wonder_overlay_ci8),0},
    {"g_tnt_wonder_overlay_rgba16",(void*)g_tnt_wonder_overlay_rgba16,sizeof(g_tnt_wonder_overlay_rgba16),1},
    {"g_tnt_wonder_pie_a_ci8",(void*)g_tnt_wonder_pie_a_ci8,sizeof(g_tnt_wonder_pie_a_ci8),0},
    {"g_tnt_wonder_pie_a_rgba16",(void*)g_tnt_wonder_pie_a_rgba16,sizeof(g_tnt_wonder_pie_a_rgba16),1},
    {"g_tnt_wonder_pie_b_ci8",(void*)g_tnt_wonder_pie_b_ci8,sizeof(g_tnt_wonder_pie_b_ci8),0},
    {"g_tnt_wonder_pie_b_rgba16",(void*)g_tnt_wonder_pie_b_rgba16,sizeof(g_tnt_wonder_pie_b_rgba16),1},
    {"g_tnt_wonder_pie_c_ci8",(void*)g_tnt_wonder_pie_c_ci8,sizeof(g_tnt_wonder_pie_c_ci8),0},
    {"g_tnt_wonder_pie_c_rgba16",(void*)g_tnt_wonder_pie_c_rgba16,sizeof(g_tnt_wonder_pie_c_rgba16),1},
    {"g_tnt_wonder_pie_d_ci8",(void*)g_tnt_wonder_pie_d_ci8,sizeof(g_tnt_wonder_pie_d_ci8),0},
    {"g_tnt_wonder_pie_d_rgba16",(void*)g_tnt_wonder_pie_d_rgba16,sizeof(g_tnt_wonder_pie_d_rgba16),1},
    {"g_tnt_wonder_pie_e_ci8",(void*)g_tnt_wonder_pie_e_ci8,sizeof(g_tnt_wonder_pie_e_ci8),0},
    {"g_tnt_wonder_pie_e_rgba16",(void*)g_tnt_wonder_pie_e_rgba16,sizeof(g_tnt_wonder_pie_e_rgba16),1},
    {"g_tnt_wonder_pie_f_ci8",(void*)g_tnt_wonder_pie_f_ci8,sizeof(g_tnt_wonder_pie_f_ci8),0},
    {"g_tnt_wonder_pie_f_rgba16",(void*)g_tnt_wonder_pie_f_rgba16,sizeof(g_tnt_wonder_pie_f_rgba16),1},
    {"g_tnt_wonder_pie_g_ci8",(void*)g_tnt_wonder_pie_g_ci8,sizeof(g_tnt_wonder_pie_g_ci8),0},
    {"g_tnt_wonder_pie_g_rgba16",(void*)g_tnt_wonder_pie_g_rgba16,sizeof(g_tnt_wonder_pie_g_rgba16),1},
    {"g_tnt_wonder_pie_h_ci8",(void*)g_tnt_wonder_pie_h_ci8,sizeof(g_tnt_wonder_pie_h_ci8),0},
    {"g_tnt_wonder_pie_h_rgba16",(void*)g_tnt_wonder_pie_h_rgba16,sizeof(g_tnt_wonder_pie_h_rgba16),1},
    {"g_tnt_african_background_packbits",(void*)g_tnt_african_background_packbits,sizeof(g_tnt_african_background_packbits),0},
    {"g_tnt_african_background_rgba16",(void*)g_tnt_african_background_rgba16,sizeof(g_tnt_african_background_rgba16),1},
    {"g_tnt_african_smoke_ci8",(void*)g_tnt_african_smoke_ci8,sizeof(g_tnt_african_smoke_ci8),0},
    {"g_tnt_african_smoke_rgba16",(void*)g_tnt_african_smoke_rgba16,sizeof(g_tnt_african_smoke_rgba16),1},
    {"g_tnt_celtic_background_packbits",(void*)g_tnt_celtic_background_packbits,sizeof(g_tnt_celtic_background_packbits),0},
    {"g_tnt_celtic_background_rgba16",(void*)g_tnt_celtic_background_rgba16,sizeof(g_tnt_celtic_background_rgba16),1},
    {"g_tnt_celtic_lamp_ci8",(void*)g_tnt_celtic_lamp_ci8,sizeof(g_tnt_celtic_lamp_ci8),0},
    {"g_tnt_celtic_lamp_rgba16",(void*)g_tnt_celtic_lamp_rgba16,sizeof(g_tnt_celtic_lamp_rgba16),1},
    {"g_tnt_japanese_background_packbits",(void*)g_tnt_japanese_background_packbits,sizeof(g_tnt_japanese_background_packbits),0},
    {"g_tnt_japanese_background_rgba16",(void*)g_tnt_japanese_background_rgba16,sizeof(g_tnt_japanese_background_rgba16),1},
    {"g_tnt_japanese_sparkle_ci8",(void*)g_tnt_japanese_sparkle_ci8,sizeof(g_tnt_japanese_sparkle_ci8),0},
    {"g_tnt_japanese_sparkle_rgba16",(void*)g_tnt_japanese_sparkle_rgba16,sizeof(g_tnt_japanese_sparkle_rgba16),1},
    {"g_tnt_japanese_water_ci8",(void*)g_tnt_japanese_water_ci8,sizeof(g_tnt_japanese_water_ci8),0},
    {"g_tnt_japanese_water_rgba16",(void*)g_tnt_japanese_water_rgba16,sizeof(g_tnt_japanese_water_rgba16),1},
    {"g_tnt_russian_background_packbits",(void*)g_tnt_russian_background_packbits,sizeof(g_tnt_russian_background_packbits),0},
    {"g_tnt_russian_background_rgba16",(void*)g_tnt_russian_background_rgba16,sizeof(g_tnt_russian_background_rgba16),1},
    {"g_tnt_russian_candle_ci8",(void*)g_tnt_russian_candle_ci8,sizeof(g_tnt_russian_candle_ci8),0},
    {"g_tnt_russian_candle_rgba16",(void*)g_tnt_russian_candle_rgba16,sizeof(g_tnt_russian_candle_rgba16),1},
    {"s_african_2p_bg_packbits",(void*)s_african_2p_bg_packbits,sizeof(s_african_2p_bg_packbits),0},
    {"s_african_2p_bg_pal",(void*)s_african_2p_bg_pal,sizeof(s_african_2p_bg_pal),1},
    {"s_african_2p_time",(void*)s_african_2p_time,sizeof(s_african_2p_time),0},
    {"s_african_2p_time_pal",(void*)s_african_2p_time_pal,sizeof(s_african_2p_time_pal),1},
    {"s_celtic_2p_bg_packbits",(void*)s_celtic_2p_bg_packbits,sizeof(s_celtic_2p_bg_packbits),0},
    {"s_celtic_2p_bg_pal",(void*)s_celtic_2p_bg_pal,sizeof(s_celtic_2p_bg_pal),1},
    {"s_celtic_2p_time",(void*)s_celtic_2p_time,sizeof(s_celtic_2p_time),0},
    {"s_celtic_2p_time_pal",(void*)s_celtic_2p_time_pal,sizeof(s_celtic_2p_time_pal),1},
    {"s_egyptian_2p_bg_packbits",(void*)s_egyptian_2p_bg_packbits,sizeof(s_egyptian_2p_bg_packbits),0},
    {"s_egyptian_2p_bg_pal",(void*)s_egyptian_2p_bg_pal,sizeof(s_egyptian_2p_bg_pal),1},
    {"s_egyptian_2p_time",(void*)s_egyptian_2p_time,sizeof(s_egyptian_2p_time),0},
    {"s_egyptian_2p_time_pal",(void*)s_egyptian_2p_time_pal,sizeof(s_egyptian_2p_time_pal),1},
    {"s_greek_2p_bg_packbits",(void*)s_greek_2p_bg_packbits,sizeof(s_greek_2p_bg_packbits),0},
    {"s_greek_2p_bg_pal",(void*)s_greek_2p_bg_pal,sizeof(s_greek_2p_bg_pal),1},
    {"s_greek_2p_time",(void*)s_greek_2p_time,sizeof(s_greek_2p_time),0},
    {"s_greek_2p_time_pal",(void*)s_greek_2p_time_pal,sizeof(s_greek_2p_time_pal),1},
    {"s_japanese_2p_bg_packbits",(void*)s_japanese_2p_bg_packbits,sizeof(s_japanese_2p_bg_packbits),0},
    {"s_japanese_2p_bg_pal",(void*)s_japanese_2p_bg_pal,sizeof(s_japanese_2p_bg_pal),1},
    {"s_japanese_2p_time",(void*)s_japanese_2p_time,sizeof(s_japanese_2p_time),0},
    {"s_japanese_2p_time_pal",(void*)s_japanese_2p_time_pal,sizeof(s_japanese_2p_time_pal),1},
    {"s_mayan_2p_bg_packbits",(void*)s_mayan_2p_bg_packbits,sizeof(s_mayan_2p_bg_packbits),0},
    {"s_mayan_2p_bg_pal",(void*)s_mayan_2p_bg_pal,sizeof(s_mayan_2p_bg_pal),1},
    {"s_mayan_2p_time",(void*)s_mayan_2p_time,sizeof(s_mayan_2p_time),0},
    {"s_mayan_2p_time_pal",(void*)s_mayan_2p_time_pal,sizeof(s_mayan_2p_time_pal),1},
    {"s_russian_2p_bg_packbits",(void*)s_russian_2p_bg_packbits,sizeof(s_russian_2p_bg_packbits),0},
    {"s_russian_2p_bg_pal",(void*)s_russian_2p_bg_pal,sizeof(s_russian_2p_bg_pal),1},
    {"s_russian_2p_time",(void*)s_russian_2p_time,sizeof(s_russian_2p_time),0},
    {"s_russian_2p_time_pal",(void*)s_russian_2p_time_pal,sizeof(s_russian_2p_time_pal),1},
    {"g_tnt_egyptian_background_packbits",(void*)g_tnt_egyptian_background_packbits,sizeof(g_tnt_egyptian_background_packbits),0},
    {"g_tnt_egyptian_background_rgba16",(void*)g_tnt_egyptian_background_rgba16,sizeof(g_tnt_egyptian_background_rgba16),1},
    {"g_tnt_finale_background_packbits",(void*)g_tnt_finale_background_packbits,sizeof(g_tnt_finale_background_packbits),0},
    {"g_tnt_finale_background_rgba16",(void*)g_tnt_finale_background_rgba16,sizeof(g_tnt_finale_background_rgba16),1},
    {"g_tnt_finale_boiler_ci8",(void*)g_tnt_finale_boiler_ci8,sizeof(g_tnt_finale_boiler_ci8),0},
    {"g_tnt_finale_boiler_rgba16",(void*)g_tnt_finale_boiler_rgba16,sizeof(g_tnt_finale_boiler_rgba16),1},
    {"g_tnt_finale_bulb_ci8",(void*)g_tnt_finale_bulb_ci8,sizeof(g_tnt_finale_bulb_ci8),0},
    {"g_tnt_finale_bulb_rgba16",(void*)g_tnt_finale_bulb_rgba16,sizeof(g_tnt_finale_bulb_rgba16),1},
    {"g_tnt_finale_gauge_ci8",(void*)g_tnt_finale_gauge_ci8,sizeof(g_tnt_finale_gauge_ci8),0},
    {"g_tnt_finale_gauge_rgba16",(void*)g_tnt_finale_gauge_rgba16,sizeof(g_tnt_finale_gauge_rgba16),1},
    {"g_tnt_greek_background_ci8",(void*)g_tnt_greek_background_ci8,sizeof(g_tnt_greek_background_ci8),0},
    {"g_tnt_greek_background_rgba16",(void*)g_tnt_greek_background_rgba16,sizeof(g_tnt_greek_background_rgba16),1},
};
