#ifndef TNT_HALUCI_VITA_H
#define TNT_HALUCI_VITA_H

#include "vita_compat.h"

#define TNT_HALUCI_PARTICLE_COUNT 350

typedef struct {
    f32 cx;
    f32 cy;
    f32 half;
    u8 r;
    u8 g;
    u8 b;
    u8 pad;
} TNTHaluciParticle;

typedef struct {
    /* GXM samples the previous Vita display buffer directly for the feedback
     * pass. Keeping the mapped framebuffer here avoids the old 400x300 CPU
     * resample and texture-upload copy on every feedback frame. */
    const u32 *feedback_framebuffer;
    u16 feedback_stride;
    u16 feedback_width;
    u16 feedback_height;
    u8 feedback_this_frame;
    u16 particle_count;
    TNTHaluciParticle particle[TNT_HALUCI_PARTICLE_COUNT];
} TNTHaluciFrame;

void TNT_Vita_HaluciInit(void);

/* Fast path: retain only the source animation/state math on CPU. Feedback is
 * sampled directly from the previous mapped Vita display buffer by GXM, while
 * CPU prepares the 350 projected particle descriptors. */
void TNT_Vita_HaluciPrepareFrame(
    TNTHaluciFrame *frame,
    const u32 *previous_framebuffer,
    int previous_stride,
    int previous_width,
    int previous_height,
    int board_x,
    int board_y,
    int cell,
    u8 start_pressed,
    u32 retrace_delta
);

/* Original Test-1 software renderer retained as a fallback while the GXM
 * implementation is validated on real hardware. */
void TNT_Vita_HaluciRender(
    void *buffer,
    int stride,
    int width,
    int height,
    int board_x,
    int board_y,
    int cell,
    u8 start_pressed,
    u32 retrace_delta
);

#endif
