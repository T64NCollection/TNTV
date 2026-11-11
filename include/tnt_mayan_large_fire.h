#ifndef TNT_MAYAN_LARGE_FIRE_H
#define TNT_MAYAN_LARGE_FIRE_H

#include "vita_compat.h"

typedef struct {
    u16 intensity;
    s16 intensity_delta;
    s16 y;
    s16 y_delta;
    u16 angle;
    s16 angle_delta;
    s16 radius;
    s16 radius_delta;
    s16 unused;
    u16 lifetime;
} TNTMayanFireParticle;

typedef struct {
    u8 buffers[2][17 * 67];
    TNTMayanFireParticle particles[128];
    u8 draw_buffer;
} TNTMayanLargeFire;

void TNT_Vita_MayanLargeFire_Init(TNTMayanLargeFire *fire);
void TNT_Vita_MayanLargeFire_Update(TNTMayanLargeFire *fire);
void TNT_Vita_MayanLargeFire_Render(
    void *buffer,
    s32 stride,
    s32 screen_width,
    s32 screen_height,
    s32 board_x,
    s32 board_y,
    s32 cell,
    TNTMayanLargeFire *fire
);

#endif
