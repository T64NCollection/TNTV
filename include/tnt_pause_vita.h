#ifndef TNT_PAUSE_VITA_H
#define TNT_PAUSE_VITA_H


#define TNT_VITA_PAUSE_ACTION_NONE      0
#define TNT_VITA_PAUSE_ACTION_CONTINUE  1
#define TNT_VITA_PAUSE_ACTION_PROGRESS  2
#define TNT_VITA_PAUSE_ACTION_EXIT      4

typedef struct {
    u8 active;
    u8 selected;
    u8 just_entered;
    u8 fade_active;
    f32 pulse;
    f32 pulse_rate;
    UnkStruct_2 fade;
} TNTVitaPauseMenu;

void TNT_Vita_PauseInit(TNTVitaPauseMenu *menu);
void TNT_Vita_SetGameplayAlpha(Tetris *tetris_ptr, u8 alpha);
int TNT_Vita_FontA_TextWidth(const char *text);
void TNT_Vita_FontA_DrawText(
    void *buffer,
    s32 stride,
    s32 screen_width,
    s32 screen_height,
    s32 board_x,
    s32 board_y,
    s32 cell,
    s32 native_x,
    s32 native_y,
    const char *text,
    u8 red,
    u8 green,
    u8 blue,
    u8 global_alpha
);
int TNT_Vita_FontB_TextWidth(const char *text);
void TNT_Vita_FontB_DrawText(
    void *buffer,
    s32 stride,
    s32 screen_width,
    s32 screen_height,
    s32 board_x,
    s32 board_y,
    s32 cell,
    s32 native_x,
    s32 native_y,
    const char *text,
    u8 red,
    u8 green,
    u8 blue,
    u8 global_alpha
);
void TNT_Vita_PauseStart(TNTVitaPauseMenu *menu, Tetris *tetris_ptr);
void TNT_Vita_PauseResumeGame(
    TNTVitaPauseMenu *menu,
    Tetris *tetris_ptr,
    PlayerVars *pv_ptr
);
s32 TNT_Vita_PauseUpdate(
    TNTVitaPauseMenu *menu,
    Tetris *tetris_ptr,
    PlayerVars *pv_ptr
);
u8 TNT_Vita_PauseIsActive(const TNTVitaPauseMenu *menu);
void TNT_Vita_PauseRender(
    void *buffer,
    s32 stride,
    s32 screen_width,
    s32 screen_height,
    s32 board_x,
    s32 board_y,
    s32 cell,
    const TNTVitaPauseMenu *menu
);

#endif
