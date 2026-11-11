#ifndef TNT_WONDER_MILESTONE_VITA_H
#define TNT_WONDER_MILESTONE_VITA_H

#include "tnt_wonder_vita.h"

#define TNT_WONDER_MILESTONE_MAX_BLOCKS 19

typedef struct {
    u8 texture;
    u8 node;
    u8 unlit;
    u8 pad;
    TNTWonderVertex v[3];
} TNTWonderMilestoneTriangle;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
    u16 start_tick;
    u16 pad;
} TNTWonderMilestoneBlock;

extern f32 g_tnt_milestone_scene1006_nodes[9][9];
extern f32 g_tnt_milestone_scene1088_nodes[4][9];
extern f32 g_tnt_milestone_scene1104_nodes[8][9];
extern f32 g_tnt_milestone_scene1057_nodes[2][9];
extern f32 g_tnt_greek_scene1125_nodes[8][9];
extern TNTWonderMilestoneTriangle g_tnt_milestone_scene1006_triangles[244];
extern TNTWonderMilestoneTriangle g_tnt_milestone_scene1088_triangles[126];
extern TNTWonderMilestoneTriangle g_tnt_milestone_scene1104_triangles[212];
extern TNTWonderMilestoneTriangle g_tnt_milestone_scene1057_triangles[10];
extern TNTWonderMilestoneTriangle g_tnt_greek_scene1125_triangles[432];
extern u16 g_tnt_milestone_large_textures[15][1360];
extern u8 g_tnt_milestone_large_texture_wrap[15];
extern f32 g_tnt_milestone_block_y[191][7];
extern f32 g_tnt_milestone_camera_xz[510][2];
extern f32 g_tnt_greek_camera_xz[509][2];
extern u8 g_tnt_mayan_milestone_block_count[7];
extern TNTWonderMilestoneBlock g_tnt_mayan_milestone_blocks[7][6];
extern u8 g_tnt_greek_milestone_block_count[7];
extern TNTWonderMilestoneBlock g_tnt_greek_milestone_blocks[7][19];
extern f32 g_tnt_egyptian_scene1148_nodes[8][9];
extern TNTWonderMilestoneTriangle g_tnt_egyptian_scene1148_triangles[486];
extern u16 g_tnt_egyptian_milestone_textures[4][1360];
extern u8 g_tnt_egyptian_milestone_texture_wrap[4];
extern u8 g_tnt_egyptian_milestone_block_count[7];
extern TNTWonderMilestoneBlock g_tnt_egyptian_milestone_blocks[7][TNT_WONDER_MILESTONE_MAX_BLOCKS];

extern f32 g_tnt_celtic_scene1244_nodes[9][9];
extern f32 g_tnt_celtic_scene1244_stage_nodes[7][9][9];
extern TNTWonderMilestoneTriangle g_tnt_celtic_scene1244_triangles[530];
extern u16 g_tnt_celtic_milestone_textures[4][1360];
extern u8 g_tnt_celtic_milestone_texture_wrap[4];
extern f32 g_tnt_celtic_milestone_camera[997][6];
extern u8 g_tnt_celtic_milestone_block_count[7];
extern TNTWonderMilestoneBlock g_tnt_celtic_milestone_blocks[7][TNT_WONDER_MILESTONE_MAX_BLOCKS];

extern f32 g_tnt_african_scene1292_nodes[8][9];
extern f32 g_tnt_african_scene1292_stage_nodes[7][8][9];
extern TNTWonderMilestoneTriangle g_tnt_african_scene1292_triangles[1758];
extern u16 g_tnt_african_milestone_textures[6][1360];
extern u8 g_tnt_african_milestone_texture_wrap[6];
extern f32 g_tnt_african_milestone_camera[997][6];
extern f32 g_tnt_russian_milestone_stage_camera[7][499][6];
extern u8 g_tnt_african_milestone_block_count[7];
extern TNTWonderMilestoneBlock g_tnt_african_milestone_blocks[7][TNT_WONDER_MILESTONE_MAX_BLOCKS];

void TNT_Vita_WonderMilestoneRenderMayan(
    void *buffer, s32 stride, s32 screen_width, s32 screen_height,
    s32 board_x, s32 board_y, s32 cell, u8 milestone_index,
    u16 elapsed_ticks, const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]);

void TNT_Vita_WonderMilestoneRenderGreek(
    void *buffer, s32 stride, s32 screen_width, s32 screen_height,
    s32 board_x, s32 board_y, s32 cell, u8 greek_stage,
    u16 elapsed_ticks, const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]);

void TNT_Vita_WonderMilestoneRenderEgyptian(
    void *buffer, s32 stride, s32 screen_width, s32 screen_height,
    s32 board_x, s32 board_y, s32 cell, u8 egyptian_stage,
    u16 elapsed_ticks, const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]);

void TNT_Vita_WonderMilestoneRenderCeltic(
    void *buffer, s32 stride, s32 screen_width, s32 screen_height,
    s32 board_x, s32 board_y, s32 cell, u8 celtic_stage,
    u16 elapsed_ticks, const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]);

void TNT_Vita_WonderMilestoneRenderAfrican(
    void *buffer, s32 stride, s32 screen_width, s32 screen_height,
    s32 board_x, s32 board_y, s32 cell, u8 african_stage,
    u16 elapsed_ticks, const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]);

void TNT_Vita_WonderMilestoneRenderRussian(
    void *buffer, s32 stride, s32 screen_width, s32 screen_height,
    s32 board_x, s32 board_y, s32 cell, u8 russian_stage,
    u16 elapsed_ticks, const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]);

void TNT_Vita_WonderMilestoneRenderJapanese(
    void *buffer, s32 stride, s32 screen_width, s32 screen_height,
    s32 board_x, s32 board_y, s32 cell, u8 japanese_stage,
    u16 elapsed_ticks, const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]);

#endif
