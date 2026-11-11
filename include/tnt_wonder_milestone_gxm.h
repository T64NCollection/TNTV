#ifndef TNT_WONDER_MILESTONE_GXM_H
#define TNT_WONDER_MILESTONE_GXM_H

#include <psp2/gxm.h>
#include "vita_compat.h"
#include "tnt_wonder_milestone_vita.h"
#include "tnt_haluci_vita.h"

/* Temporary test25 timings: geometry, BeginScene, submit, Finish (us). */
extern u32 g_tnt_wonder_profile_us[4];
int TNT_Vita_WonderMilestoneGxmInit(void);
void TNT_Vita_WonderMilestoneGxmTerm(void);

int TNT_Vita_MayanSkyboxGxmRender(
    SceGxmColorSurface *surface,
    SceGxmSyncObject *sync,
    s32 board_x,
    s32 board_y,
    s32 cell,
    f32 angle_degrees
);

int TNT_Vita_GreekSkyboxGxmRender(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell, f32 angle_degrees
);
int TNT_Vita_GreekRainGxmRender(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell, u32 anim_ticks
);
int TNT_Vita_GreekGameplayGxmRender(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell, f32 angle_degrees,
    u32 rain_ticks, u8 fire_frame, const char *player_name
);

int TNT_Vita_CelticGameplayGxmRender(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell,
    u32 sky_ticks, u32 leaf_ticks, u8 lamp_frame,
    const char *player_name
);

int TNT_Vita_AfricanGameplayGxmRender(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell,
    u32 sky_ticks, u8 smoke_frame, const char *player_name
);

int TNT_Vita_JapaneseGameplayGxmRender(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell,
    f32 sky_angle, u32 effect_ticks, u32 butterfly_ticks,
    const char *player_name
);

void TNT_Vita_RussianGameplayGxmReset(void);
int TNT_Vita_RussianGameplayGxmRender(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell,
    u32 sky_ticks, u32 snow_ticks, const u8 *candle_frame,
    const char *player_name
);

int TNT_Vita_HaluciGxmRender(
    SceGxmColorSurface *surface,
    SceGxmSyncObject *sync,
    s32 board_x,
    s32 board_y,
    s32 cell,
    const TNTHaluciFrame *frame
);

int TNT_Vita_CreditsGxmRender(
    SceGxmColorSurface *surface,
    SceGxmSyncObject *sync,
    s32 board_x,
    s32 board_y,
    s32 cell,
    const u32 *rgba400x300
);

int TNT_Vita_FontCGxmRenderText(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell,
    s32 native_x, s32 native_y, const char *text
);

int TNT_Vita_PauseGxmRender(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell, u8 selected, f32 pulse
);

int TNT_Vita_WonderProgressGxmRender(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell, u16 anim_tick
);

int TNT_Vita_WonderMilestoneGxmRenderMayan(
    SceGxmColorSurface *surface,
    SceGxmSyncObject *sync,
    s32 board_x,
    s32 board_y,
    s32 cell,
    u8 milestone_index,
    u16 elapsed_ticks,
    const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]
);

int TNT_Vita_WonderMilestoneGxmRenderGreek(
    SceGxmColorSurface *surface,
    SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell,
    u8 greek_stage, u16 elapsed_ticks,
    const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]
);

int TNT_Vita_WonderMilestoneGxmRenderEgyptian(
    SceGxmColorSurface *surface,
    SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell,
    u8 egyptian_stage, u16 elapsed_ticks,
    const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]
);

int TNT_Vita_WonderMilestoneGxmRenderCeltic(
    SceGxmColorSurface *surface,
    SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell,
    u8 celtic_stage, u16 elapsed_ticks,
    const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]
);

int TNT_Vita_WonderMilestoneGxmRenderAfrican(
    SceGxmColorSurface *surface,
    SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell,
    u8 african_stage, u16 elapsed_ticks,
    const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]
);

int TNT_Vita_WonderMilestoneGxmRenderRussian(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell,
    u8 russian_stage, u16 elapsed_ticks,
    const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]
);

int TNT_Vita_WonderMilestoneGxmRenderJapanese(
    SceGxmColorSurface *surface,
    SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell,
    u8 japanese_stage, u16 elapsed_ticks,
    const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]
);

int TNT_Vita_WonderCompleteGxmRender(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell, u8 wonder,
    u8 anim_mode, u32 anim_ticks,
    const char *const *story, u32 story_line_count, u32 story_ticks, u8 story_visible
);

/* Upload one original 960x544 precomposed Hall page when the current card
 * changes; GPU restores it over both Hall framebuffer surfaces per frame. */
int TNT_Vita_WonderHallGxmSetBackground(const u32 *rgba);

/* Test212D: Hall-only scene1071 GPU question-mark, using original
 * texture-generated UVs and source-animated 236-triangle geometry. */
int TNT_Vita_WonderLockedHallGxmRender(
    SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u16 anim_tick);

/* Test212D: render the original current-Wonder pile with Hall's existing
 * GPU background inside the same scene. Pause -> PROGRESS unchanged. */
int TNT_Vita_WonderProgressHallGxmRender(
    SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u16 anim_tick);

/* Dedicated GPU scene1425 340-triangle trophy for interactive WONDERS Hall.
 * Returns negative if GXM unavailable so the existing software renderer
 * remains a safe fallback. No effect on Wonder completion movies. */
int TNT_Vita_WonderFinaleHallGxmRender(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u16 anim_tick);

/* Exact 960x544 pre-rasterized frame, source stride 1024 pixels. */
int TNT_Vita_StaticFrameGxmRestore(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    const u32 *rgba, u32 generation);

#endif
