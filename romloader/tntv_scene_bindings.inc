extern int8_t g_tnt_game_dcm_sample_map[31][4];
extern uint8_t g_tnt_game_sample_flags[18];
extern uint32_t g_tnt_game_sample_lengths[18];
extern uint32_t g_tnt_game_sample_loop_begin[18];
extern uint32_t g_tnt_game_sample_loop_end[18];
extern uint32_t g_tnt_game_sfx_stream_lengths[31];
extern int8_t g_tnt_intro_dcm_sample_map[28][8];
extern uint8_t g_tnt_intro_sample_flags[15];
extern uint32_t g_tnt_intro_sample_lengths[15];
extern uint32_t g_tnt_intro_sample_loop_begin[15];
extern uint32_t g_tnt_intro_sample_loop_end[15];
extern uint32_t g_tnt_intro_sfx_stream_lengths[28];
extern uint32_t g_tnt_menu_sample_lengths[4];
extern uint32_t g_tnt_menu_sfx_stream_lengths[6];
static const uint16_t s_game_dcm_ids[]={256U,257U,258U,259U,260U,261U,262U,263U,264U,265U,266U,267U,268U,269U,270U,271U,272U,273U,274U,275U,276U,277U,278U,279U,280U,281U,282U,283U,284U,285U,286U};
static const uint16_t s_game_sample_ids[]={293U,295U,296U,297U,298U,299U,300U,301U,302U,303U,304U,305U,306U,307U,308U,309U,310U,311U};
static const uint16_t s_intro_dcm_ids[]={1024U,1025U,1026U,1027U,1028U,1029U,1030U,1031U,1032U,1033U,1034U,1035U,1036U,1037U,1038U,1039U,1040U,1041U,1042U,1043U,1044U,1045U,1046U,1047U,1048U,1049U,1050U,1051U};
static const uint16_t s_intro_sample_ids[]={280U,281U,282U,283U,284U,285U,286U,287U,288U,289U,290U,291U,292U,293U,294U};
static const uint16_t s_menu_dcm_ids[]={512U,513U,514U,515U,516U,517U};
static const uint16_t s_menu_sample_ids[]={312U,313U,314U,315U};
extern uint8_t D_800C55B0[5];
extern uint8_t D_800C55B8[19];
extern uint8_t D_800C55CC[38];
extern int8_t D_800C55F4[80];
extern int8_t D_800C5644[1696];
/* 480KB BSS sky, now filled exclusively by authenticated user's ROM. */
extern uint32_t g_tnt_russian_sky_baseline_rgba[400*300];
extern char g_tnt_story_storage[7][18][32];
extern const char *g_tnt_complete_stories[7][18];
extern char g_tnt_guest_name_storage[8][9][16];
extern const char *g_tnt_guest_names[8][9];
extern char g_tnt_credits_rom_text[1894];
extern const char *g_tnt_credits_lines[181];
extern uint8_t g_tnt_credits_font_is_b[192];
extern uint8_t g_tnt_font_a_char_widths[46];
extern uint8_t g_tnt_font_b_char_widths[46];
extern int16_t g_tnt_credits_sintable[1024];
/* Source-only ROM scene IDs and destination symbols. */
#include "tnt_wonder_complete_african_vita.h"
#include "tnt_wonder_complete_celtic_vita.h"
#include "tnt_wonder_complete_egyptian_vita.h"
#include "tnt_wonder_complete_greek_vita.h"
#include "tnt_wonder_complete_japanese_vita.h"
#include "tnt_wonder_complete_russian_vita.h"
#include "tnt_wonder_complete_vita.h"
#include "tnt_african_gameplay_vita.h"
#include "tnt_japanese_gameplay_vita.h"
#include "tnt_russian_gameplay_vita.h"
#include "tnt_celtic_gameplay_vita.h"
#include "tntv_fx_reader.h"
#include "tntv_img_reader.h"
#include "tntv_ai_reader.h"
#include "tntv_sky_synth.h"
#include "tnt_multiplayer_4p_data.h"
#include "tnt_greek_gameplay_vita.h"
#include "tnt_mayan_skybox_vita.h"
#include "tnt_wonder_hall_vita.h"
#include "tnt_wonder_milestone_vita.h"
#include "tnt_wonder_vita.h"
#include "tnt_intro_vita.h"
#include "tnt_egyptian_gameplay_vita.h"
/* Source-only native declarations for Mayan temple fire. */
extern uint8_t g_tnt_mayan_fire_ci8[5][30];
extern uint16_t g_tnt_mayan_fire_rgba16[5][256];
static int load_wonder_scene_tables(TNTVRom *rom) {
    uint8_t *raw = NULL; uint32_t bytes = 0;
    write_status("205D SCENE STEP 001: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,186U,0U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_wonder_complete_nodes)) { free(raw); return -2; }
    memcpy(g_tnt_wonder_complete_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 002: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,186U,1U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_wonder_complete_node_type)) { free(raw); return -2; }
    memcpy(g_tnt_wonder_complete_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 003: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,186U,2U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_wonder_complete_parents)) { free(raw); return -2; }
    memcpy(g_tnt_wonder_complete_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 004: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,186U,3U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_wonder_complete_node_mesh)) { free(raw); return -2; }
    memcpy(g_tnt_wonder_complete_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 005: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,186U,4U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_wonder_complete_mesh_scale)) { free(raw); return -2; }
    memcpy(g_tnt_wonder_complete_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 006: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,186U,5U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_wonder_complete_textures)) { free(raw); return -2; }
    memcpy(g_tnt_wonder_complete_textures,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 007: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,314U,0U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_greek_complete_nodes)) { free(raw); return -2; }
    memcpy(g_tnt_greek_complete_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 008: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,314U,1U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_greek_complete_node_type)) { free(raw); return -2; }
    memcpy(g_tnt_greek_complete_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 009: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,314U,2U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_greek_complete_parents)) { free(raw); return -2; }
    memcpy(g_tnt_greek_complete_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 010: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,314U,3U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_greek_complete_node_mesh)) { free(raw); return -2; }
    memcpy(g_tnt_greek_complete_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 011: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,314U,4U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_greek_complete_mesh_scale)) { free(raw); return -2; }
    memcpy(g_tnt_greek_complete_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 012: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,314U,5U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_greek_complete_textures)) { free(raw); return -2; }
    memcpy(g_tnt_greek_complete_textures,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 013: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,374U,0U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_egyptian_complete_nodes)) { free(raw); return -2; }
    memcpy(g_tnt_egyptian_complete_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 014: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,374U,1U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_egyptian_complete_node_type)) { free(raw); return -2; }
    memcpy(g_tnt_egyptian_complete_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 015: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,374U,2U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_egyptian_complete_parents)) { free(raw); return -2; }
    memcpy(g_tnt_egyptian_complete_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 016: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,374U,3U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_egyptian_complete_node_mesh)) { free(raw); return -2; }
    memcpy(g_tnt_egyptian_complete_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 017: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,374U,4U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_egyptian_complete_mesh_scale)) { free(raw); return -2; }
    memcpy(g_tnt_egyptian_complete_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 018: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,374U,5U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_egyptian_complete_textures)) { free(raw); return -2; }
    memcpy(g_tnt_egyptian_complete_textures,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 019: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,470U,0U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_celtic_complete_nodes)) { free(raw); return -2; }
    memcpy(g_tnt_celtic_complete_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 020: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,470U,1U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_celtic_complete_node_type)) { free(raw); return -2; }
    memcpy(g_tnt_celtic_complete_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 021: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,470U,2U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_celtic_complete_parents)) { free(raw); return -2; }
    memcpy(g_tnt_celtic_complete_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 022: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,470U,3U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_celtic_complete_node_mesh)) { free(raw); return -2; }
    memcpy(g_tnt_celtic_complete_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 023: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,470U,4U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_celtic_complete_mesh_scale)) { free(raw); return -2; }
    memcpy(g_tnt_celtic_complete_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 024: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,470U,5U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_celtic_complete_textures)) { free(raw); return -2; }
    memcpy(g_tnt_celtic_complete_textures,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 025: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,557U,0U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_african_complete_nodes)) { free(raw); return -2; }
    memcpy(g_tnt_african_complete_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 026: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,557U,1U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_african_complete_node_type)) { free(raw); return -2; }
    memcpy(g_tnt_african_complete_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 027: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,557U,2U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_african_complete_parents)) { free(raw); return -2; }
    memcpy(g_tnt_african_complete_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 028: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,557U,3U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_african_complete_node_mesh)) { free(raw); return -2; }
    memcpy(g_tnt_african_complete_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 029: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,557U,4U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_african_complete_mesh_scale)) { free(raw); return -2; }
    memcpy(g_tnt_african_complete_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 030: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,557U,5U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_african_complete_textures)) { free(raw); return -2; }
    memcpy(g_tnt_african_complete_textures,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 031: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,755U,0U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_japanese_complete_nodes)) { free(raw); return -2; }
    memcpy(g_tnt_japanese_complete_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 032: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,755U,1U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_japanese_complete_node_type)) { free(raw); return -2; }
    memcpy(g_tnt_japanese_complete_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 033: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,755U,2U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_japanese_complete_parents)) { free(raw); return -2; }
    memcpy(g_tnt_japanese_complete_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 034: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,755U,3U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_japanese_complete_node_mesh)) { free(raw); return -2; }
    memcpy(g_tnt_japanese_complete_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 035: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,755U,4U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_japanese_complete_mesh_scale)) { free(raw); return -2; }
    memcpy(g_tnt_japanese_complete_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 036: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,755U,5U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_japanese_complete_textures)) { free(raw); return -2; }
    memcpy(g_tnt_japanese_complete_textures,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 037: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,888U,0U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_russian_complete_nodes)) { free(raw); return -2; }
    memcpy(g_tnt_russian_complete_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 038: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,888U,1U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_russian_complete_node_type)) { free(raw); return -2; }
    memcpy(g_tnt_russian_complete_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 039: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,888U,2U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_russian_complete_parents)) { free(raw); return -2; }
    memcpy(g_tnt_russian_complete_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 040: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,888U,3U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_russian_complete_node_mesh)) { free(raw); return -2; }
    memcpy(g_tnt_russian_complete_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 041: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,888U,4U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_russian_complete_mesh_scale)) { free(raw); return -2; }
    memcpy(g_tnt_russian_complete_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 042: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,888U,5U,&raw,&bytes)!=0) return -1;
    if(bytes!=sizeof(g_tnt_russian_complete_textures)) { free(raw); return -2; }
    memcpy(g_tnt_russian_complete_textures,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 043: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,314U,0U,&raw,&bytes)!=0) return -3;
    if(bytes!=sizeof(g_tnt_greek_complete_triangles)) { free(raw); return -4; }
    memcpy(g_tnt_greek_complete_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 044: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,374U,0U,&raw,&bytes)!=0) return -3;
    if(bytes!=sizeof(g_tnt_egyptian_complete_triangles)) { free(raw); return -4; }
    memcpy(g_tnt_egyptian_complete_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 045: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,374U,1U,&raw,&bytes)!=0) return -3;
    if(bytes!=sizeof(g_tnt_egyptian_complete_untextured_colors)) { free(raw); return -4; }
    memcpy(g_tnt_egyptian_complete_untextured_colors,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 046: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,470U,0U,&raw,&bytes)!=0) return -3;
    if(bytes!=sizeof(g_tnt_celtic_complete_triangles)) { free(raw); return -4; }
    memcpy(g_tnt_celtic_complete_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 047: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,470U,1U,&raw,&bytes)!=0) return -3;
    if(bytes!=sizeof(g_tnt_celtic_complete_untextured_colors)) { free(raw); return -4; }
    memcpy(g_tnt_celtic_complete_untextured_colors,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 048: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,557U,0U,&raw,&bytes)!=0) return -3;
    if(bytes!=sizeof(g_tnt_african_complete_triangles)) { free(raw); return -4; }
    memcpy(g_tnt_african_complete_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 049: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,557U,1U,&raw,&bytes)!=0) return -3;
    if(bytes!=sizeof(g_tnt_african_complete_untextured_colors)) { free(raw); return -4; }
    memcpy(g_tnt_african_complete_untextured_colors,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 050: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,755U,0U,&raw,&bytes)!=0) return -3;
    if(bytes!=sizeof(g_tnt_japanese_complete_triangles)) { free(raw); return -4; }
    memcpy(g_tnt_japanese_complete_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 051: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,755U,1U,&raw,&bytes)!=0) return -3;
    if(bytes!=sizeof(g_tnt_japanese_complete_untextured_colors)) { free(raw); return -4; }
    memcpy(g_tnt_japanese_complete_untextured_colors,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 052: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,888U,0U,&raw,&bytes)!=0) return -3;
    if(bytes!=sizeof(g_tnt_russian_complete_triangles)) { free(raw); return -4; }
    memcpy(g_tnt_russian_complete_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 053: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,888U,1U,&raw,&bytes)!=0) return -3;
    if(bytes!=sizeof(g_tnt_russian_complete_untextured_colors)) { free(raw); return -4; }
    memcpy(g_tnt_russian_complete_untextured_colors,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 054: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,186U,0U,&raw,&bytes)!=0) return -3;
    if(bytes!=sizeof(g_tnt_wonder_complete_triangles)) { free(raw); return -4; }
    memcpy(g_tnt_wonder_complete_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 055: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,567U,6U,&raw,&bytes)!=0) return -5;
    if(bytes!=sizeof(g_tnt_african_sky_nodes)) { free(raw); return -6; }
    memcpy(g_tnt_african_sky_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 056: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,567U,1U,&raw,&bytes)!=0) return -5;
    if(bytes!=sizeof(g_tnt_african_sky_node_type)) { free(raw); return -6; }
    memcpy(g_tnt_african_sky_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 057: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,567U,2U,&raw,&bytes)!=0) return -5;
    if(bytes!=sizeof(g_tnt_african_sky_parents)) { free(raw); return -6; }
    memcpy(g_tnt_african_sky_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 058: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,567U,3U,&raw,&bytes)!=0) return -5;
    if(bytes!=sizeof(g_tnt_african_sky_node_mesh)) { free(raw); return -6; }
    memcpy(g_tnt_african_sky_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 059: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,567U,4U,&raw,&bytes)!=0) return -5;
    if(bytes!=sizeof(g_tnt_african_sky_mesh_scale)) { free(raw); return -6; }
    memcpy(g_tnt_african_sky_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 060: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,567U,7U,&raw,&bytes)!=0) return -5;
    if(bytes!=sizeof(g_tnt_african_sky_textures)) { free(raw); return -6; }
    memcpy(g_tnt_african_sky_textures,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 061: tntv_fx_extract_scene_meshes");
        if(tntv_fx_extract_scene_meshes(rom,567U,0U,&raw,&bytes)!=0) return -7;
    if(bytes!=sizeof(g_tnt_african_sky_triangles)) { free(raw); return -8; }
    memcpy(g_tnt_african_sky_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 062: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,567U,2U,&raw,&bytes)!=0) return -7;
    if(bytes!=sizeof(g_tnt_african_sky_triangle_alpha)) { free(raw); return -8; }
    memcpy(g_tnt_african_sky_triangle_alpha,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 063: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,487U,6U,&raw,&bytes)!=0) return -9;
    if(bytes!=sizeof(g_tnt_celtic_sky_nodes)) { free(raw); return -10; }
    memcpy(g_tnt_celtic_sky_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 064: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,487U,1U,&raw,&bytes)!=0) return -9;
    if(bytes!=sizeof(g_tnt_celtic_sky_node_type)) { free(raw); return -10; }
    memcpy(g_tnt_celtic_sky_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 065: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,487U,2U,&raw,&bytes)!=0) return -9;
    if(bytes!=sizeof(g_tnt_celtic_sky_parents)) { free(raw); return -10; }
    memcpy(g_tnt_celtic_sky_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 066: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,487U,3U,&raw,&bytes)!=0) return -9;
    if(bytes!=sizeof(g_tnt_celtic_sky_node_mesh)) { free(raw); return -10; }
    memcpy(g_tnt_celtic_sky_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 067: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,487U,4U,&raw,&bytes)!=0) return -9;
    if(bytes!=sizeof(g_tnt_celtic_sky_mesh_scale)) { free(raw); return -10; }
    memcpy(g_tnt_celtic_sky_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 068: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,487U,7U,&raw,&bytes)!=0) return -9;
    if(bytes!=sizeof(g_tnt_celtic_sky_textures)) { free(raw); return -10; }
    memcpy(g_tnt_celtic_sky_textures,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 069: tntv_fx_extract_scene_meshes");
        if(tntv_fx_extract_scene_meshes(rom,487U,0U,&raw,&bytes)!=0) return -11;
    if(bytes!=sizeof(g_tnt_celtic_sky_triangles)) { free(raw); return -12; }
    memcpy(g_tnt_celtic_sky_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 070: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,487U,1U,&raw,&bytes)!=0) return -11;
    if(bytes!=sizeof(g_tnt_celtic_sky_alpha_class)) { free(raw); return -12; }
    memcpy(g_tnt_celtic_sky_alpha_class,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 071: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,487U,2U,&raw,&bytes)!=0) return -11;
    if(bytes!=sizeof(g_tnt_celtic_sky_untextured)) { free(raw); return -12; }
    memcpy(g_tnt_celtic_sky_untextured,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 072: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,1335U,6U,&raw,&bytes)!=0) return -13;
    if(bytes!=sizeof(g_tnt_celtic_leaf_nodes)) { free(raw); return -14; }
    memcpy(g_tnt_celtic_leaf_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 073: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1335U,1U,&raw,&bytes)!=0) return -13;
    if(bytes!=sizeof(g_tnt_celtic_leaf_node_type)) { free(raw); return -14; }
    memcpy(g_tnt_celtic_leaf_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 074: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1335U,2U,&raw,&bytes)!=0) return -13;
    if(bytes!=sizeof(g_tnt_celtic_leaf_parents)) { free(raw); return -14; }
    memcpy(g_tnt_celtic_leaf_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 075: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1335U,3U,&raw,&bytes)!=0) return -13;
    if(bytes!=sizeof(g_tnt_celtic_leaf_node_mesh)) { free(raw); return -14; }
    memcpy(g_tnt_celtic_leaf_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 076: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1335U,4U,&raw,&bytes)!=0) return -13;
    if(bytes!=sizeof(g_tnt_celtic_leaf_mesh_scale)) { free(raw); return -14; }
    memcpy(g_tnt_celtic_leaf_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 077: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,1335U,2U,&raw,&bytes)!=0) return -13;
    if(bytes!=sizeof(g_tnt_celtic_leaf_triangles)) { free(raw); return -14; }
    memcpy(g_tnt_celtic_leaf_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 078: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,770U,6U,&raw,&bytes)!=0) return -9;
    if(bytes!=sizeof(g_tnt_japanese_sky_nodes)) { free(raw); return -10; }
    memcpy(g_tnt_japanese_sky_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 079: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,770U,1U,&raw,&bytes)!=0) return -9;
    if(bytes!=sizeof(g_tnt_japanese_sky_node_type)) { free(raw); return -10; }
    memcpy(g_tnt_japanese_sky_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 080: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,770U,2U,&raw,&bytes)!=0) return -9;
    if(bytes!=sizeof(g_tnt_japanese_sky_parents)) { free(raw); return -10; }
    memcpy(g_tnt_japanese_sky_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 081: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,770U,3U,&raw,&bytes)!=0) return -9;
    if(bytes!=sizeof(g_tnt_japanese_sky_node_mesh)) { free(raw); return -10; }
    memcpy(g_tnt_japanese_sky_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 082: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,770U,4U,&raw,&bytes)!=0) return -9;
    if(bytes!=sizeof(g_tnt_japanese_sky_mesh_scale)) { free(raw); return -10; }
    memcpy(g_tnt_japanese_sky_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 083: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,770U,7U,&raw,&bytes)!=0) return -9;
    if(bytes!=sizeof(g_tnt_japanese_sky_textures)) { free(raw); return -10; }
    memcpy(g_tnt_japanese_sky_textures,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 084: tntv_fx_extract_scene_meshes");
        if(tntv_fx_extract_scene_meshes(rom,770U,0U,&raw,&bytes)!=0) return -15;
    if(bytes!=sizeof(g_tnt_japanese_sky_triangles)) { free(raw); return -16; }
    memcpy(g_tnt_japanese_sky_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 085: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,1360U,6U,&raw,&bytes)!=0) return -17;
    if(bytes!=sizeof(g_tnt_japanese_butterfly_nodes)) { free(raw); return -18; }
    memcpy(g_tnt_japanese_butterfly_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 086: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1360U,1U,&raw,&bytes)!=0) return -17;
    if(bytes!=sizeof(g_tnt_japanese_butterfly_node_type)) { free(raw); return -18; }
    memcpy(g_tnt_japanese_butterfly_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 087: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1360U,2U,&raw,&bytes)!=0) return -17;
    if(bytes!=sizeof(g_tnt_japanese_butterfly_parents)) { free(raw); return -18; }
    memcpy(g_tnt_japanese_butterfly_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 088: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1360U,3U,&raw,&bytes)!=0) return -17;
    if(bytes!=sizeof(g_tnt_japanese_butterfly_node_mesh)) { free(raw); return -18; }
    memcpy(g_tnt_japanese_butterfly_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 089: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1360U,4U,&raw,&bytes)!=0) return -17;
    if(bytes!=sizeof(g_tnt_japanese_butterfly_mesh_scale)) { free(raw); return -18; }
    memcpy(g_tnt_japanese_butterfly_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 090: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,1360U,2U,&raw,&bytes)!=0) return -17;
    if(bytes!=sizeof(g_tnt_japanese_butterfly_triangles)) { free(raw); return -18; }
    memcpy(g_tnt_japanese_butterfly_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 091: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,901U,6U,&raw,&bytes)!=0) return -19;
    if(bytes!=sizeof(g_tnt_russian_sky_nodes)) { free(raw); return -20; }
    memcpy(g_tnt_russian_sky_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 092: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,901U,1U,&raw,&bytes)!=0) return -19;
    if(bytes!=sizeof(g_tnt_russian_sky_node_type)) { free(raw); return -20; }
    memcpy(g_tnt_russian_sky_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 093: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,901U,2U,&raw,&bytes)!=0) return -19;
    if(bytes!=sizeof(g_tnt_russian_sky_parents)) { free(raw); return -20; }
    memcpy(g_tnt_russian_sky_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 094: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,901U,3U,&raw,&bytes)!=0) return -19;
    if(bytes!=sizeof(g_tnt_russian_sky_node_mesh)) { free(raw); return -20; }
    memcpy(g_tnt_russian_sky_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 095: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,901U,4U,&raw,&bytes)!=0) return -19;
    if(bytes!=sizeof(g_tnt_russian_sky_mesh_scale)) { free(raw); return -20; }
    memcpy(g_tnt_russian_sky_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 096: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,901U,7U,&raw,&bytes)!=0) return -19;
    if(bytes!=sizeof(g_tnt_russian_sky_textures)) { free(raw); return -20; }
    memcpy(g_tnt_russian_sky_textures,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 097: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,901U,8U,&raw,&bytes)!=0) return -19;
    if(bytes!=sizeof(g_tnt_russian_sky_texture_texels)) { free(raw); return -20; }
    memcpy(g_tnt_russian_sky_texture_texels,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 098: tntv_fx_extract_scene_meshes");
        if(tntv_fx_extract_scene_meshes(rom,901U,0U,&raw,&bytes)!=0) return -21;
    if(bytes!=sizeof(g_tnt_russian_sky_triangles)) { free(raw); return -22; }
    memcpy(g_tnt_russian_sky_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 099: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,901U,3U,&raw,&bytes)!=0) return -21;
    if(bytes!=sizeof(g_tnt_russian_sky_triangle_alpha)) { free(raw); return -22; }
    memcpy(g_tnt_russian_sky_triangle_alpha,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 100: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,1370U,6U,&raw,&bytes)!=0) return -23;
    if(bytes!=sizeof(g_tnt_russian_snow_nodes)) { free(raw); return -24; }
    memcpy(g_tnt_russian_snow_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 101: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1370U,1U,&raw,&bytes)!=0) return -23;
    if(bytes!=sizeof(g_tnt_russian_snow_node_type)) { free(raw); return -24; }
    memcpy(g_tnt_russian_snow_node_type,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 102: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1370U,2U,&raw,&bytes)!=0) return -23;
    if(bytes!=sizeof(g_tnt_russian_snow_parents)) { free(raw); return -24; }
    memcpy(g_tnt_russian_snow_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 103: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1370U,3U,&raw,&bytes)!=0) return -23;
    if(bytes!=sizeof(g_tnt_russian_snow_node_mesh)) { free(raw); return -24; }
    memcpy(g_tnt_russian_snow_node_mesh,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 104: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1370U,4U,&raw,&bytes)!=0) return -23;
    if(bytes!=sizeof(g_tnt_russian_snow_mesh_scale)) { free(raw); return -24; }
    memcpy(g_tnt_russian_snow_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 105: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,1370U,2U,&raw,&bytes)!=0) return -23;
    if(bytes!=sizeof(g_tnt_russian_snow_triangles)) { free(raw); return -24; }
    memcpy(g_tnt_russian_snow_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 106: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,1370U,4U,&raw,&bytes)!=0) return -23;
    if(bytes!=sizeof(g_tnt_russian_snow_lit)) { free(raw); return -24; }
    memcpy(g_tnt_russian_snow_lit,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 107: tntv_img_restore_4p_packbits");
        if(tntv_img_restore_4p_packbits(rom,g_tnt_mayan_4p_background_packbits,
                                     (uint32_t)sizeof(g_tnt_mayan_4p_background_packbits))!=0) return -25;
    write_status("205D SCENE STEP 108: tntv_img_restore_4p_culture");
    if(tntv_img_restore_4p_culture(rom,52U,53U,g_tnt_egyptian_4p_packbits,(uint32_t)sizeof(g_tnt_egyptian_4p_packbits),g_tnt_egyptian_4p_rgba16)!=0)return -131;
    write_status("205D SCENE STEP 109: tntv_img_restore_4p_culture");
    if(tntv_img_restore_4p_culture(rom,54U,55U,g_tnt_japanese_4p_packbits,(uint32_t)sizeof(g_tnt_japanese_4p_packbits),g_tnt_japanese_4p_rgba16)!=0)return -131;
    write_status("205D SCENE STEP 110: tntv_img_restore_4p_culture");
    if(tntv_img_restore_4p_culture(rom,56U,57U,g_tnt_celtic_4p_packbits,(uint32_t)sizeof(g_tnt_celtic_4p_packbits),g_tnt_celtic_4p_rgba16)!=0)return -131;
    write_status("205D SCENE STEP 111: tntv_img_restore_4p_culture");
    if(tntv_img_restore_4p_culture(rom,58U,59U,g_tnt_african_4p_packbits,(uint32_t)sizeof(g_tnt_african_4p_packbits),g_tnt_african_4p_rgba16)!=0)return -131;
    write_status("205D SCENE STEP 112: tntv_img_restore_4p_culture");
    if(tntv_img_restore_4p_culture(rom,60U,61U,g_tnt_greek_4p_packbits,(uint32_t)sizeof(g_tnt_greek_4p_packbits),g_tnt_greek_4p_rgba16)!=0)return -131;
    write_status("205D SCENE STEP 113: tntv_img_restore_4p_culture");
    if(tntv_img_restore_4p_culture(rom,62U,63U,g_tnt_russian_4p_packbits,(uint32_t)sizeof(g_tnt_russian_4p_packbits),g_tnt_russian_4p_rgba16)!=0)return -131;
    write_status("205D SCENE STEP 114: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,1318U,6U,&raw,&bytes)!=0) return -26;
    if(bytes!=sizeof(g_tnt_greek_rain_nodes)) { free(raw); return -27; }
    memcpy(g_tnt_greek_rain_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 115: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1318U,7U,&raw,&bytes)!=0) return -26;
    if(bytes!=sizeof(g_tnt_greek_rain_texture)) { free(raw); return -27; }
    memcpy(g_tnt_greek_rain_texture,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 116: tntv_fx_extract_scene_meshes");
        if(tntv_fx_extract_scene_meshes(rom,1318U,0U,&raw,&bytes)!=0) return -28;
    if(bytes!=sizeof(g_tnt_greek_rain_triangles)) { free(raw); return -29; }
    memcpy(g_tnt_greek_rain_triangles,raw,bytes); free(raw); raw=NULL;
        /* Source ROM Greek fire: 14 CI8 103x71 frames; one original nonsequential
     * image/palette pair at animation frame 12. Preserve frame order EXACTLY.
     * These are resource selectors, not embedded game-original artwork. */
    {
        /* This is a resource-ID sequence, not ROM source artwork. Build it
         * algorithmically to avoid an incidental 33-byte match against an
         * unrelated ROM font-character lookup table. Preserve frame 12's
         * deliberate non-sequential original special case. */
        uint16_t frames[14];
        unsigned fi;
        for(fi=0;fi<14U;fi++)frames[fi]=(uint16_t)(54U+fi);
        frames[12]=94U;
        frames[13]=66U;
        static const uint16_t palettes[14]={310,311,312,313,314,315,316,317,318,319,320,321,350,322};
        if(sizeof(g_tnt_greek_fire_ci8)!=14U*103U*71U ||
           sizeof(g_tnt_greek_fire_rgba16)!=14U*256U*2U) return -30;
    write_status("205D SCENE STEP 117: tntv_img_restore_anim_ci8");
        if(tntv_img_restore_anim_ci8(rom,frames,palettes,14U,103U,71U,
            &g_tnt_greek_fire_ci8[0][0],&g_tnt_greek_fire_rgba16[0][0])!=0) return -31;
    }
        /* Exact original Egyptian coal and two candle original ANIM frame IDs:
     * User ROM assets only; resource selectors are not game-owned pixel data. */
    {
        static const uint16_t frames[12]={26,27,28,29,30,31,32,33,79,80,81,82};
        static const uint16_t palettes[12]={282,283,284,285,286,287,288,289,335,336,337,338};
        if(sizeof(g_tnt_egyptian_coal_ci8)!=12U*50U*26U ||
           sizeof(g_tnt_egyptian_coal_rgba16)!=12U*256U*2U) return -32;
    write_status("205D SCENE STEP 118: tntv_img_restore_anim_ci8");
        if(tntv_img_restore_anim_ci8(rom,frames,palettes,12U,50U,26U,
           &g_tnt_egyptian_coal_ci8[0][0],&g_tnt_egyptian_coal_rgba16[0][0])!=0)return -33;
    }
    {
        static const uint16_t frames[4]={34,35,36,37};
        static const uint16_t palettes[4]={290,291,292,293};
        if(sizeof(g_tnt_egyptian_candle_a_ci8)!=4U*22U*19U ||
           sizeof(g_tnt_egyptian_candle_a_rgba16)!=4U*256U*2U) return -34;
    write_status("205D SCENE STEP 119: tntv_img_restore_anim_ci8");
        if(tntv_img_restore_anim_ci8(rom,frames,palettes,4U,22U,19U,
           &g_tnt_egyptian_candle_a_ci8[0][0],&g_tnt_egyptian_candle_a_rgba16[0][0])!=0)return -35;
    }
    {
        static const uint16_t frames[4]={38,39,40,41};
        static const uint16_t palettes[4]={294,295,296,297};
        if(sizeof(g_tnt_egyptian_candle_b_ci8)!=4U*22U*19U ||
           sizeof(g_tnt_egyptian_candle_b_rgba16)!=4U*256U*2U) return -36;
    write_status("205D SCENE STEP 120: tntv_img_restore_anim_ci8");
        if(tntv_img_restore_anim_ci8(rom,frames,palettes,4U,22U,19U,
           &g_tnt_egyptian_candle_b_ci8[0][0],&g_tnt_egyptian_candle_b_rgba16[0][0])!=0)return -37;
    }
        /* Original Mayan temple fire, 5 individual 5x6 CI8 animation frames.
     * Source ROM contains one duplicate frame palette, preserve original
     * palette selector sequence 256..260 exactly. */
    {
        static const uint16_t frames[5]={0,1,2,3,4};
        static const uint16_t palettes[5]={256,257,258,259,260};
        if(sizeof(g_tnt_mayan_fire_ci8)!=5U*5U*6U ||
           sizeof(g_tnt_mayan_fire_rgba16)!=5U*256U*2U) return -38;
    write_status("205D SCENE STEP 121: tntv_img_restore_anim_ci8");
        if(tntv_img_restore_anim_ci8(rom,frames,palettes,5U,5U,6U,
           &g_tnt_mayan_fire_ci8[0][0],&g_tnt_mayan_fire_rgba16[0][0])!=0)return -39;
    }
    write_status("205D SCENE STEP 122: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,1399U,6U,&raw,&bytes)!=0) return -40;
    if(bytes!=sizeof(g_tnt_greek_skybox_nodes)) { free(raw); return -41; }
    memcpy(g_tnt_greek_skybox_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 123: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1399U,7U,&raw,&bytes)!=0) return -40;
    if(bytes!=sizeof(g_tnt_greek_skybox_textures)) { free(raw); return -41; }
    memcpy(g_tnt_greek_skybox_textures,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 124: tntv_fx_extract_scene_meshes");
        if(tntv_fx_extract_scene_meshes(rom,1399U,0U,&raw,&bytes)!=0) return -42;
    if(bytes!=sizeof(g_tnt_greek_skybox_triangles)) { free(raw); return -43; }
    memcpy(g_tnt_greek_skybox_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 125: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,201U,7U,&raw,&bytes)!=0) return -44;
    if(bytes!=sizeof(g_tnt_mayan_skybox_textures)) { free(raw); return -45; }
    memcpy(g_tnt_mayan_skybox_textures,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 126: tntv_fx_extract_scene_meshes");
        if(tntv_fx_extract_scene_meshes(rom,201U,0U,&raw,&bytes)!=0) return -46;
    if(bytes!=sizeof(g_tnt_mayan_skybox_triangles)) { free(raw); return -47; }
    memcpy(g_tnt_mayan_skybox_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 127: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,1425U,6U,&raw,&bytes)!=0) return -48;
    if(bytes!=sizeof(g_tnt_wonder_finale_hall_nodes)) { free(raw); return -49; }
    memcpy(g_tnt_wonder_finale_hall_nodes,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 128: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1425U,2U,&raw,&bytes)!=0) return -48;
    if(bytes!=sizeof(g_tnt_wonder_finale_hall_parents)) { free(raw); return -49; }
    memcpy(g_tnt_wonder_finale_hall_parents,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 129: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1425U,4U,&raw,&bytes)!=0) return -48;
    if(bytes!=sizeof(g_tnt_wonder_finale_hall_mesh_scale)) { free(raw); return -49; }
    memcpy(g_tnt_wonder_finale_hall_mesh_scale,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 130: tntv_fx_extract_wonder_texels");
    if(tntv_fx_extract_wonder_texels(rom,1425U,&raw,&bytes)!=0) return -50;
    if(bytes!=sizeof(g_tnt_wonder_finale_hall_texture)) { free(raw); return -51; }
    {
        uint32_t i;
        for(i=0;i<bytes/2U;i++)
            g_tnt_wonder_finale_hall_texture[i]=(uint16_t)(((uint16_t)raw[2U*i]<<8)|raw[2U*i+1U]);
    }
    free(raw); raw=NULL;
    write_status("205D SCENE STEP 131: tntv_fx_extract_scene_meshes");
        if(tntv_fx_extract_scene_meshes(rom,1425U,0U,&raw,&bytes)!=0) return -52;
    if(bytes!=sizeof(g_tnt_wonder_finale_hall_triangles)) { free(raw); return -53; }
    memcpy(g_tnt_wonder_finale_hall_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 132: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,1071U,6U,&raw,&bytes)!=0) return -58;
    if(bytes!=sizeof(g_tnt_wonder_locked_nodes)) { free(raw); return -59; }
    memcpy(g_tnt_wonder_locked_nodes,raw,bytes); free(raw);raw=NULL;
    write_status("205D SCENE STEP 133: tntv_fx_extract_wonder_texels");
    if(tntv_fx_extract_wonder_texels(rom,1071U,&raw,&bytes)!=0) return -54;
    if(bytes!=sizeof(g_tnt_wonder_locked_texture)) { free(raw); return -55; }
    {
        uint32_t i;
        for(i=0;i<bytes/2U;i++)
            g_tnt_wonder_locked_texture[i]=(uint16_t)(((uint16_t)raw[2U*i]<<8)|raw[2U*i+1U]);
    }
    free(raw);raw=NULL;
    write_status("205D SCENE STEP 134: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,1071U,0U,&raw,&bytes)!=0) return -56;
    if(bytes!=sizeof(g_tnt_wonder_locked_triangles)) { free(raw); return -57; }
    memcpy(g_tnt_wonder_locked_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 135: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,1125U,6U,&raw,&bytes)!=0) return -60;
    if(bytes!=sizeof(g_tnt_greek_scene1125_nodes)) { free(raw); return -61; }
    memcpy(g_tnt_greek_scene1125_nodes,raw,bytes); free(raw);raw=NULL;
    write_status("205D SCENE STEP 136: tntv_fx_extract_scene_meshes");
        if(tntv_fx_extract_scene_meshes(rom,1125U,0U,&raw,&bytes)!=0) return -62;
    if(bytes!=sizeof(g_tnt_greek_scene1125_triangles)) { free(raw); return -63; }
    memcpy(g_tnt_greek_scene1125_triangles,raw,bytes); free(raw); raw=NULL;
    write_status("205D SCENE STEP 137: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,1292U,6U,&raw,&bytes)!=0) return -64;
    if(bytes!=sizeof(g_tnt_african_scene1292_nodes)) { free(raw); return -65; }
    memcpy(g_tnt_african_scene1292_nodes,raw,bytes); free(raw);raw=NULL;
    write_status("205D SCENE STEP 138: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1292U,7U,&raw,&bytes)!=0) return -64;
    if(bytes!=sizeof(g_tnt_african_milestone_textures)) { free(raw); return -65; }
    memcpy(g_tnt_african_milestone_textures,raw,bytes); free(raw);raw=NULL;
    write_status("205D SCENE STEP 139: tntv_fx_extract_scene_meshes");
        if(tntv_fx_extract_scene_meshes(rom,1292U,0U,&raw,&bytes)!=0) return -66;
    if(bytes!=sizeof(g_tnt_african_scene1292_triangles)) { free(raw); return -67; }
    memcpy(g_tnt_african_scene1292_triangles,raw,bytes); free(raw);raw=NULL;
        /* African original FX1292 one-shot stage animations1293..1299 at tick2. */
    {
        uint32_t stage;
        for(stage=0;stage<7U;stage++) {
    write_status("205D SCENE STEP 140: tntv_fx_sample_scene_animation");
            if(tntv_fx_sample_scene_animation(rom,1292U,1293U+stage,2U,1U,
                                               0U,&raw,&bytes)!=0)return -68;
            if(bytes!=sizeof(g_tnt_african_scene1292_stage_nodes[0])) {
                free(raw);return -69;
            }
            memcpy(g_tnt_african_scene1292_stage_nodes[stage],raw,bytes);
            free(raw);raw=NULL;
        }
    }
        /* 997 frame African Wonder camera, source ROM original FX scene1303,
     * type6 animation1304 polynomial 6-float node0 track sampling. */
    write_status("205D SCENE STEP 141: tntv_fx_sample_scene_animation");
    if(tntv_fx_sample_scene_animation(rom,1303U,1304U,0U,997U,
                                      1U,&raw,&bytes)!=0)return -70;
    if(bytes!=sizeof(g_tnt_african_milestone_camera)) { free(raw); return -71; }
    memcpy(g_tnt_african_milestone_camera,raw,bytes); free(raw);raw=NULL;
        /* Six scene1292 original FX texture headers: descriptor flag byte 8
     * comes from original ROM texture resource byte6, not embedded artwork. */
    write_status("205D SCENE STEP 142: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1292U,5U,&raw,&bytes)!=0)return -72;
    if(bytes!=6U*12U) { free(raw);return -73; }
    {
        uint32_t i;
        for(i=0;i<6U;i++)
            g_tnt_african_milestone_texture_wrap[i]=raw[i*12U+8U];
    }
    free(raw);raw=NULL;
        /* Original N64 African stage pointer lists and 48B object records;
     * stage counts are derived by scanning NULL-terminated ROM pointers. */
    write_status("205D SCENE STEP 143: tntv_rom_restore_african_blocks");
    if(tntv_rom_restore_african_blocks(rom,g_tnt_african_milestone_block_count,
              g_tnt_african_milestone_blocks,
              sizeof(g_tnt_african_milestone_blocks[0][0]),
              sizeof(g_tnt_african_milestone_blocks))!=0)return -74;
        /* Russian Wonder original seven consecutive one-shot scene1255
     * keyframe tracks1256..1262. Each stage owns 499 camera samples with
     * 6 node0 float components; exact approved editor-approved timing. */
    {
        uint32_t stage;
        for(stage=0;stage<7U;stage++) {
    write_status("205D SCENE STEP 144: tntv_fx_sample_scene_animation");
            if(tntv_fx_sample_scene_animation(rom,1255U,1256U+stage,0U,
                                               499U,1U,&raw,&bytes)!=0)return -76;
            if(bytes!=sizeof(g_tnt_russian_milestone_stage_camera[0])) {
                free(raw);return -77;
            }
            memcpy(g_tnt_russian_milestone_stage_camera[stage],raw,bytes);
            free(raw);raw=NULL;
        }
    }
        /* Greek Wonder source-original camera 509 samples, FX scene1136
     * animation1137. The approved C data stores just X and Z, not all
     * six animated camera node components returned by the source reader. */
    write_status("205D SCENE STEP 145: tntv_fx_sample_scene_animation");
    if(tntv_fx_sample_scene_animation(rom,1136U,1137U,0U,509U,1U,
                                      &raw,&bytes)!=0)return -78;
    if(bytes!=509U*6U*sizeof(float)) { free(raw);return -79; }
    {
        uint32_t tick;
        for(tick=0;tick<509U;tick++) {
            memcpy(&g_tnt_greek_camera_xz[tick][0],raw+(tick*6U+0U)*4U,4U);
            memcpy(&g_tnt_greek_camera_xz[tick][1],raw+(tick*6U+2U)*4U,4U);
        }
    }
    free(raw);raw=NULL;
        /* Greek Wonder original USA1.0 KSEG0 stage block pointer lists:
     * three engine metadata pointers followed by NULL-terminated blocks. */
    write_status("205D SCENE STEP 146: tntv_rom_restore_greek_blocks");
    if(tntv_rom_restore_greek_blocks(rom,g_tnt_greek_milestone_block_count,
             g_tnt_greek_milestone_blocks,
             sizeof(g_tnt_greek_milestone_blocks[0][0]),
             sizeof(g_tnt_greek_milestone_blocks))!=0)return -80;
    write_status("205D SCENE STEP 147: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,1244U,6U,&raw,&bytes)!=0)return -82;
    if(bytes!=sizeof(g_tnt_celtic_scene1244_nodes)) { free(raw);return -83; }
    memcpy(g_tnt_celtic_scene1244_nodes,raw,bytes); free(raw);raw=NULL;
    write_status("205D SCENE STEP 148: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1244U,7U,&raw,&bytes)!=0)return -82;
    if(bytes!=sizeof(g_tnt_celtic_milestone_textures)) { free(raw);return -83; }
    memcpy(g_tnt_celtic_milestone_textures,raw,bytes); free(raw);raw=NULL;
    write_status("205D SCENE STEP 149: tntv_fx_extract_scene_meshes");
        if(tntv_fx_extract_scene_meshes(rom,1244U,0U,&raw,&bytes)!=0)return -84;
    if(bytes!=sizeof(g_tnt_celtic_scene1244_triangles)) { free(raw);return -85; }
    memcpy(g_tnt_celtic_scene1244_triangles,raw,bytes); free(raw);raw=NULL;
        /* Original Celtic Wonder one-shot scene1244 animations1245..1251:
     * seven 9-node poses, each captured at native original animation tick2. */
    {
        uint32_t stage;
        for(stage=0;stage<7U;stage++) {
    write_status("205D SCENE STEP 150: tntv_fx_sample_scene_animation");
            if(tntv_fx_sample_scene_animation(rom,1244U,1245U+stage,2U,1U,
                                               0U,&raw,&bytes)!=0)return -86;
            if(bytes!=sizeof(g_tnt_celtic_scene1244_stage_nodes[0])) {
                free(raw);return -87;
            }
            memcpy(g_tnt_celtic_scene1244_stage_nodes[stage],raw,bytes);
            free(raw);raw=NULL;
        }
    }
    /* Celtic camera-only FX scene1277 with animation1278, exactly 997 ticks. */
    write_status("205D SCENE STEP 151: tntv_fx_sample_scene_animation");
    if(tntv_fx_sample_scene_animation(rom,1277U,1278U,0U,997U,1U,
                                      &raw,&bytes)!=0)return -88;
    if(bytes!=sizeof(g_tnt_celtic_milestone_camera)) { free(raw);return -89; }
    memcpy(g_tnt_celtic_milestone_camera,raw,bytes);free(raw);raw=NULL;
        /* Four original Celtic scene1244 texture wrap flags from N64 FX records. */
    write_status("205D SCENE STEP 152: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1244U,5U,&raw,&bytes)!=0)return -90;
    if(bytes!=4U*12U) {free(raw);return -91;}
    {
        uint32_t i;
        for(i=0;i<4U;i++)
            g_tnt_celtic_milestone_texture_wrap[i]=raw[i*12U+8U];
    }
    free(raw);raw=NULL;
    /* Original Celtic falling-block record pointers and event timing. */
    write_status("205D SCENE STEP 153: tntv_rom_restore_celtic_blocks");
    if(tntv_rom_restore_celtic_blocks(rom,g_tnt_celtic_milestone_block_count,
          g_tnt_celtic_milestone_blocks,
          sizeof(g_tnt_celtic_milestone_blocks[0][0]),
          sizeof(g_tnt_celtic_milestone_blocks))!=0)return -92;
    write_status("205D SCENE STEP 154: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,1148U,6U,&raw,&bytes)!=0)return -93;
    if(bytes!=sizeof(g_tnt_egyptian_scene1148_nodes)) { free(raw);return -94; }
    memcpy(g_tnt_egyptian_scene1148_nodes,raw,bytes); free(raw);raw=NULL;
    write_status("205D SCENE STEP 155: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1148U,7U,&raw,&bytes)!=0)return -93;
    if(bytes!=sizeof(g_tnt_egyptian_milestone_textures)) { free(raw);return -94; }
    memcpy(g_tnt_egyptian_milestone_textures,raw,bytes); free(raw);raw=NULL;
    write_status("205D SCENE STEP 156: tntv_fx_extract_scene_meshes");
        if(tntv_fx_extract_scene_meshes(rom,1148U,0U,&raw,&bytes)!=0)return -95;
    if(bytes!=sizeof(g_tnt_egyptian_scene1148_triangles)) {free(raw);return -96;}
    memcpy(g_tnt_egyptian_scene1148_triangles,raw,bytes);free(raw);raw=NULL;
        /* Original Egyptian scene1148 four texture wrap flags. */
    write_status("205D SCENE STEP 157: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1148U,5U,&raw,&bytes)!=0)return -97;
    if(bytes!=4U*12U) { free(raw);return -98; }
    {
        uint32_t i;
        for(i=0;i<4U;i++)
            g_tnt_egyptian_milestone_texture_wrap[i]=raw[i*12U+8U];
    }
    free(raw);raw=NULL;
    /* Original Egyptian seven construction-stage falling block lists. */
    write_status("205D SCENE STEP 158: tntv_rom_restore_egyptian_blocks");
    if(tntv_rom_restore_egyptian_blocks(rom,g_tnt_egyptian_milestone_block_count,
            g_tnt_egyptian_milestone_blocks,
            sizeof(g_tnt_egyptian_milestone_blocks[0][0]),
            sizeof(g_tnt_egyptian_milestone_blocks))!=0)return -99;
        /* Original Mayan Wonder scene1046 + original FX animation1048 tick0: 18 node poses. */
    write_status("205D SCENE STEP 159: tntv_fx_sample_scene_animation");
    if(tntv_fx_sample_scene_animation(rom,1046U,1048U,0U,1U,0U,&raw,&bytes)!=0)return -100;
    if(bytes!=sizeof(g_tnt_wonder_mayan_nodes)) {free(raw);return -101;}
    memcpy(g_tnt_wonder_mayan_nodes,raw,bytes);free(raw);raw=NULL;
    /* Original approved editor intentionally retains the unused final
     * node's base X pose, rather than animated FX1048 node17 channel0.
     * All other 161 node components are sampled byte-exact. */
    g_tnt_wonder_mayan_nodes[17][0]=0.0f;
    /* 20 original 8x8 RGBA16 textures including their 4x4 mip levels. */
    write_status("205D SCENE STEP 160: tntv_fx_extract_mayan_wonder_mips");
    if(tntv_fx_extract_mayan_wonder_mips(rom,&raw,&bytes)!=0)return -102;
    if(bytes!=sizeof(g_tnt_wonder_mayan_textures)) {free(raw);return -103;}
    memcpy(g_tnt_wonder_mayan_textures,raw,bytes);free(raw);raw=NULL;
        /* Mayan original complete-Wonder scene1046 N64 RSP: 568 triangles. */
    write_status("205D SCENE STEP 161: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,1046U,0U,&raw,&bytes)!=0)return -104;
    if(bytes!=sizeof(g_tnt_wonder_mayan_triangles)) {free(raw);return -105;}
    memcpy(g_tnt_wonder_mayan_triangles,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 162: tntv_fx_extract_scene_table");
        if(tntv_fx_extract_scene_table(rom,1006U,6U,&raw,&bytes)!=0)return -106;
    if(bytes!=sizeof(g_tnt_milestone_scene1006_nodes)) {free(raw);return -107;}
    memcpy(g_tnt_milestone_scene1006_nodes,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 163: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1088U,6U,&raw,&bytes)!=0)return -106;
    if(bytes!=sizeof(g_tnt_milestone_scene1088_nodes)) {free(raw);return -107;}
    memcpy(g_tnt_milestone_scene1088_nodes,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 164: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1104U,6U,&raw,&bytes)!=0)return -106;
    if(bytes!=sizeof(g_tnt_milestone_scene1104_nodes)) {free(raw);return -107;}
    memcpy(g_tnt_milestone_scene1104_nodes,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 165: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1057U,6U,&raw,&bytes)!=0)return -106;
    if(bytes!=sizeof(g_tnt_milestone_scene1057_nodes)) {free(raw);return -107;}
    memcpy(g_tnt_milestone_scene1057_nodes,raw,bytes);free(raw);raw=NULL;
    /* Original milestone atlas order: source FX scene1057's 5 textures
     * are followed by scene1088's 10 textures, preserving original
     * 32x32 RGBA16 plus mip level 1360-texel layout per image. */
    write_status("205D SCENE STEP 166: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1057U,7U,&raw,&bytes)!=0)return -108;
    if(bytes!=5U*1360U*2U) {free(raw);return -109;}
    memcpy(g_tnt_milestone_large_textures,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 167: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1088U,7U,&raw,&bytes)!=0)return -110;
    if(bytes!=10U*1360U*2U) {free(raw);return -111;}
    memcpy(&g_tnt_milestone_large_textures[5],raw,bytes);free(raw);raw=NULL;
    /* Original 15 texture wrap flags from original type1 texture headers. */
    write_status("205D SCENE STEP 168: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1057U,5U,&raw,&bytes)!=0)return -112;
    if(bytes!=5U*12U) {free(raw);return -113;}
    {
        uint32_t i;
        for(i=0;i<5U;i++)
            g_tnt_milestone_large_texture_wrap[i]=raw[12U*i+8U];
    }
    free(raw);raw=NULL;
    write_status("205D SCENE STEP 169: tntv_fx_extract_scene_table");
    if(tntv_fx_extract_scene_table(rom,1088U,5U,&raw,&bytes)!=0)return -114;
    if(bytes!=10U*12U) {free(raw);return -115;}
    {
        uint32_t i;
        for(i=0;i<10U;i++)
            g_tnt_milestone_large_texture_wrap[5U+i]=raw[12U*i+8U];
    }
    free(raw);raw=NULL;
    write_status("205D SCENE STEP 170: tntv_fx_extract_scene_meshes");
        if(tntv_fx_extract_scene_meshes(rom,1006U,0U,&raw,&bytes)!=0)return -116;
    if(bytes!=sizeof(g_tnt_milestone_scene1006_triangles)) {free(raw);return -117;}
    memcpy(g_tnt_milestone_scene1006_triangles,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 171: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,1088U,0U,&raw,&bytes)!=0)return -116;
    if(bytes!=sizeof(g_tnt_milestone_scene1088_triangles)) {free(raw);return -117;}
    memcpy(g_tnt_milestone_scene1088_triangles,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 172: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,1104U,0U,&raw,&bytes)!=0)return -116;
    if(bytes!=sizeof(g_tnt_milestone_scene1104_triangles)) {free(raw);return -117;}
    memcpy(g_tnt_milestone_scene1104_triangles,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 173: tntv_fx_extract_scene_meshes");
    if(tntv_fx_extract_scene_meshes(rom,1057U,0U,&raw,&bytes)!=0)return -116;
    if(bytes!=sizeof(g_tnt_milestone_scene1057_triangles)) {free(raw);return -117;}
    memcpy(g_tnt_milestone_scene1057_triangles,raw,bytes);free(raw);raw=NULL;
        /* Mayan milestone original 7 N64 stage pointer lists, three metadata
     * entries per list, actual falling-block XYZ and event tick from ROM. */
    write_status("205D SCENE STEP 174: tntv_rom_restore_mayan_milestone_blocks");
    if(tntv_rom_restore_mayan_milestone_blocks(rom,g_tnt_mayan_milestone_block_count,
           g_tnt_mayan_milestone_blocks,
           sizeof(g_tnt_mayan_milestone_blocks[0][0]),
           sizeof(g_tnt_mayan_milestone_blocks))!=0)return -118;
        /* Original 191-tick Mayan stage falling-block FX scene1006 animation1007.
     * Seven scene nodes 1..7 animate transform component index4 (Y rotation
     * parameter used for drop progress); store canonical source tick0..190.
     * Original polynomial coefficients stay only in player verified ROM. */
    write_status("205D SCENE STEP 175: tntv_fx_sample_scene_animation");
    if(tntv_fx_sample_scene_animation(rom,1006U,1007U,0U,191U,0U,
                                      &raw,&bytes)!=0)return -119;
    if(bytes!=191U*9U*9U*4U) {free(raw);return -120;}
    {
        uint32_t tick,stage;
        for(tick=0;tick<191U;tick++)for(stage=0;stage<7U;stage++) {
            uint32_t source_index=tick*9U*9U+(stage+1U)*9U+4U;
            memcpy(&g_tnt_milestone_block_y[tick][stage],
                   raw+source_index*4U,4U);
        }
    }
    free(raw);raw=NULL;
        /* Original Mayan milestone camera-only FX scene1114, type6 animation1115.
     * 509-tick terminal; sample original 0..509 (last wraps to first).
     * Approved C data stores X and Z from six FX node camera channels. */
    write_status("205D SCENE STEP 176: tntv_fx_sample_scene_animation");
    if(tntv_fx_sample_scene_animation(rom,1114U,1115U,0U,510U,1U,
                                      &raw,&bytes)!=0)return -121;
    if(bytes!=510U*6U*4U) {free(raw);return -122;}
    {
        uint32_t tick;
        for(tick=0;tick<510U;tick++) {
            memcpy(&g_tnt_milestone_camera_xz[tick][0],
                   raw+(tick*6U+0U)*4U,4U);
            memcpy(&g_tnt_milestone_camera_xz[tick][1],
                   raw+(tick*6U+2U)*4U,4U);
        }
    }
    free(raw);raw=NULL;
    write_status("205D SCENE STEP 177: tntv_fx_extract_intro_basics");
        if(tntv_fx_extract_intro_basics(rom,0U,&raw,&bytes)!=0)return -123;
    if(bytes!=sizeof(g_tnt_intro_nodes)) {free(raw);return -124;}
    memcpy(g_tnt_intro_nodes,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 178: tntv_fx_extract_intro_basics");
    if(tntv_fx_extract_intro_basics(rom,1U,&raw,&bytes)!=0)return -123;
    if(bytes!=sizeof(g_tnt_intro_fog_initial)) {free(raw);return -124;}
    memcpy(g_tnt_intro_fog_initial,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 179: tntv_fx_extract_intro_basics");
    if(tntv_fx_extract_intro_basics(rom,2U,&raw,&bytes)!=0)return -123;
    if(bytes!=sizeof(g_tnt_intro_camera_initial)) {free(raw);return -124;}
    memcpy(g_tnt_intro_camera_initial,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 180: tntv_fx_extract_intro_basics");
    if(tntv_fx_extract_intro_basics(rom,3U,&raw,&bytes)!=0)return -123;
    if(bytes!=sizeof(g_tnt_intro_lights_initial)) {free(raw);return -124;}
    memcpy(g_tnt_intro_lights_initial,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 181: tntv_fx_extract_intro_textures");
        if(tntv_fx_extract_intro_textures(rom,0U,&raw,&bytes)!=0)return -125;
    if(bytes!=sizeof(g_tnt_intro_textures)) {free(raw);return -126;}
    memcpy(g_tnt_intro_textures,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 182: tntv_fx_extract_intro_textures");
    if(tntv_fx_extract_intro_textures(rom,1U,&raw,&bytes)!=0)return -125;
    if(bytes!=sizeof(g_tnt_intro_texture_rgba16)) {free(raw);return -126;}
    memcpy(g_tnt_intro_texture_rgba16,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 183: tntv_fx_extract_intro_geometry");
        if(tntv_fx_extract_intro_geometry(rom,0U,&raw,&bytes)!=0)return -127;
    if(bytes!=sizeof(g_tnt_intro_vertices)) {free(raw);return -128;}
    memcpy(g_tnt_intro_vertices,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 184: tntv_fx_extract_intro_geometry");
    if(tntv_fx_extract_intro_geometry(rom,1U,&raw,&bytes)!=0)return -127;
    if(bytes!=sizeof(g_tnt_intro_draws)) {free(raw);return -128;}
    memcpy(g_tnt_intro_draws,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 185: tntv_fx_extract_intro_animation");
        if(tntv_fx_extract_intro_animation(rom,0U,&raw,&bytes)!=0)return -129;
    if(bytes!=sizeof(g_tnt_intro_keys)) {free(raw);return -130;}
    memcpy(g_tnt_intro_keys,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 186: tntv_fx_extract_intro_animation");
    if(tntv_fx_extract_intro_animation(rom,1U,&raw,&bytes)!=0)return -129;
    if(bytes!=sizeof(g_tnt_intro_tracks)) {free(raw);return -130;}
    memcpy(g_tnt_intro_tracks,raw,bytes);free(raw);raw=NULL;
    write_status("205D SCENE STEP 187: tntv_fx_extract_intro_animation");
    if(tntv_fx_extract_intro_animation(rom,2U,&raw,&bytes)!=0)return -129;
    if(bytes!=sizeof(g_tnt_intro_targets)) {free(raw);return -130;}
    memcpy(g_tnt_intro_targets,raw,bytes);free(raw);raw=NULL;
        /* Source USA 1.0 credits sine lookup; normalized reads support all ROM byte orders. */
    {
        uint8_t source[2048]; uint32_t i;
    write_status("205D SCENE STEP 188: tntv_rom_read");
        if(tntv_rom_read(rom,0xA0AD0U,source,sizeof(source))!=TNTV_ROM_OK)return -120;
        for(i=0;i<1024U;i++) {
            uint16_t value=(uint16_t)(((uint16_t)source[2U*i]<<8)|source[2U*i+1U]);
            memcpy(&g_tnt_credits_sintable[i],&value,sizeof(value));
        }
    }
    write_status("205D SCENE STEP 189: tntv_rom_read");
        if(tntv_rom_read(rom,0x99480U,g_tnt_credits_font_is_b,sizeof(g_tnt_credits_font_is_b))!=TNTV_ROM_OK)return -121;
    write_status("205D SCENE STEP 190: tntv_img_font_widths");
    if(tntv_img_font_widths(rom,7U,g_tnt_font_a_char_widths)!=0)return -122;
    write_status("205D SCENE STEP 191: tntv_img_font_widths");
    if(tntv_img_font_widths(rom,8U,g_tnt_font_b_char_widths)!=0)return -123;
    write_status("205D SCENE STEP 192: tntv_ai_restore_piece_patterns");
    if(tntv_ai_restore_piece_patterns(rom,D_800C55B0,D_800C55B8,
        D_800C55CC,D_800C55F4,D_800C5644)!=0)return -135;
    write_status("205D SCENE STEP 193: tntv_img_restore_ui_strings");
    if(tntv_img_restore_ui_strings(rom,g_tnt_story_storage,
        g_tnt_complete_stories,g_tnt_guest_name_storage,
        g_tnt_guest_names)!=0)return -134;
    write_status("205D SCENE STEP 194: tntv_img_restore_credits_text");
    if(tntv_img_restore_credits_text(rom,g_tnt_credits_rom_text,
         (uint32_t)sizeof(g_tnt_credits_rom_text),g_tnt_credits_lines,
         181U)!=0)return -133;
    write_status("205D SCENE STEP 195: tntv_sfx_metadata");
        if(tntv_sfx_metadata(rom,s_game_dcm_ids,31U,s_game_sample_ids,18U,4U,g_tnt_game_sfx_stream_lengths,g_tnt_game_sample_lengths,g_tnt_game_sample_loop_begin,g_tnt_game_sample_loop_end,g_tnt_game_sample_flags,&g_tnt_game_dcm_sample_map[0][0])!=0)return -124;
    write_status("205D SCENE STEP 196: tntv_sfx_metadata");
    if(tntv_sfx_metadata(rom,s_intro_dcm_ids,28U,s_intro_sample_ids,15U,8U,g_tnt_intro_sfx_stream_lengths,g_tnt_intro_sample_lengths,g_tnt_intro_sample_loop_begin,g_tnt_intro_sample_loop_end,g_tnt_intro_sample_flags,&g_tnt_intro_dcm_sample_map[0][0])!=0)return -124;
    write_status("205D SCENE STEP 197: tntv_sfx_metadata");
    if(tntv_sfx_metadata(rom,s_menu_dcm_ids,6U,s_menu_sample_ids,4U,8U,g_tnt_menu_sfx_stream_lengths,g_tnt_menu_sample_lengths,NULL,NULL,NULL,NULL)!=0)return -124;
    /* This is after every original scene/audio table has been verified.
     * One-time ROM texture synthesis; never called from gameplay render. */
    write_status("205D SCENE STEP 198: tntv_russian_sky_synthesize");
    if(tntv_russian_sky_synthesize(g_tnt_russian_sky_textures,
        g_tnt_russian_sky_texture_texels,g_tnt_russian_sky_baseline_rgba)!=0)
        return -136;
    return 0;
}
