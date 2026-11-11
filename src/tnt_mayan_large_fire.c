#include "tnt_mayan_large_fire.h"
#include "tnt_game_types.h"
#include <stdint.h>
#include <string.h>

#define FIRE_W 17
#define FIRE_H 67
#define FIRE_VISIBLE_H 64
#define FIRE_PARTICLES 128

static void fire_particle_reset(TNTMayanFireParticle *p) {
    s16 lifetime = (s16)((FUN_026900_PRNG_1() % 120) + 0x0F);
    s32 temp;

    p->lifetime = (u16)lifetime;
    p->y = (FIRE_H - 1) << 8;
    temp = p->y - (s32)(FUN_026900_PRNG_1() % (FIRE_H << 5));
    p->y_delta = (s16)(-temp / lifetime);
    p->intensity = (u16)((FUN_026900_PRNG_1() & 0xFF) << 8);
    p->intensity_delta = (s16)(-(s32)p->intensity / lifetime);
    p->angle = (u16)(FUN_026900_PRNG_1() & 0xFFFF);
    p->angle_delta = (s16)((FUN_026900_PRNG_1() & 0x3FF) - 0x200);
    p->radius = (s16)(FUN_026900_PRNG_1() % (FIRE_W * 120));
    p->radius_delta = (s16)(-p->radius / lifetime);
    p->unused = (s16)((FUN_026900_PRNG_1() & 0x3F) - 0x1F);
}

void TNT_Vita_MayanLargeFire_Init(TNTMayanLargeFire *fire) {
    s32 i;

    memset(fire, 0, sizeof(*fire));
    for (i = 0; i < FIRE_PARTICLES; i++) {
        fire_particle_reset(&fire->particles[i]);
    }
}

static void fire_diffuse(TNTMayanLargeFire *fire) {
    const u8 *src = fire->buffers[fire->draw_buffer ^ 1] + 1;
    u8 *dst = fire->buffers[fire->draw_buffer] + 1;
    s32 j;

    for (j = FIRE_H - 1; j > 0; j--, src += 2, dst += 2) {
        s32 i;
        for (i = FIRE_W - 2; i > 0; i--, src++, dst++) {
            s32 v = (src[FIRE_W - 1] + src[FIRE_W + 1] + src[FIRE_W] + *src) >> 2;
            v -= 4;
            if (v < 0) v = 0;
            *dst = (u8)v;
        }
    }
}

static void fire_move_particles(TNTMayanLargeFire *fire) {
    s32 i;

    for (i = 0; i < FIRE_PARTICLES; i++) {
        TNTMayanFireParticle *p = &fire->particles[i];
        p->intensity = (u16)(p->intensity + p->intensity_delta);
        p->radius = (s16)(p->radius + p->radius_delta);
        p->y = (s16)(p->y + p->y_delta);
        p->angle = (u16)(p->angle + p->angle_delta);
        p->lifetime--;
        if (p->lifetime == 0) {
            fire_particle_reset(p);
        }
    }
}

static void fire_plot_particles(TNTMayanLargeFire *fire) {
    s32 i;
    u8 *dst = fire->buffers[fire->draw_buffer];

    for (i = 0; i < FIRE_PARTICLES; i++) {
        TNTMayanFireParticle *p = &fire->particles[i];
        s32 x = FUN_026900_800608f4_oneliner_shift6_1(p->angle) * p->radius;
        s32 y;

        /* N64 sins() is Q15; the Vita helper is Q14. */
        x >>= 22;
        x += FIRE_W >> 1;
        y = p->y >> 8;

        /* Mirror the original early-return behavior exactly. */
        if (x < 0 || x >= FIRE_W || y < 0 || y >= FIRE_H) {
            return;
        }
        dst[x + y * FIRE_W] = (u8)(p->intensity >> 8);
    }
}

void TNT_Vita_MayanLargeFire_Update(TNTMayanLargeFire *fire) {
    fire_diffuse(fire);
    fire_move_particles(fire);
    fire_plot_particles(fire);
}

static uint32_t blend_fire_pixel(uint32_t dst, u8 intensity) {
    u32 dr = (dst >> 0) & 0xFF;
    u32 dg = (dst >> 8) & 0xFF;
    u32 db = (dst >> 16) & 0xFF;
    u32 a = intensity;
    u32 inv = 255 - a;
    u32 sr = 255;
    u32 sg = intensity;
    u32 sb = 0;
    u32 r = (sr * a + dr * inv + 127) / 255;
    u32 g = (sg * a + dg * inv + 127) / 255;
    u32 b = (sb * a + db * inv + 127) / 255;

    return r | (g << 8) | (b << 16) | 0xFF000000u;
}

static s32 fire_floor_div_s64(s64 n, s64 d) {
    if (n >= 0) return (s32)(n / d);
    return -(s32)(((-n) + d - 1) / d);
}

typedef struct {
    s16 x0[2][FIRE_W];
    s16 x1[2][FIRE_W];
    s16 y0[FIRE_VISIBLE_H];
    s16 y1[FIRE_VISIBLE_H];
    s32 board_x, board_y, cell, screen_width, screen_height;
    u8 ready;
} TNTMayanFireMapCache;

static TNTMayanFireMapCache s_fire_map;

static void fire_prepare_map(
    s32 board_x, s32 board_y, s32 cell, s32 screen_width, s32 screen_height
) {
    static const s32 centers_x[2] = {74, 205};
    const s32 native_y0 = 208 - FIRE_H + 3;
    s32 which, sx, sy;

    if (s_fire_map.ready && s_fire_map.board_x == board_x &&
        s_fire_map.board_y == board_y && s_fire_map.cell == cell &&
        s_fire_map.screen_width == screen_width &&
        s_fire_map.screen_height == screen_height)
        return;

    s_fire_map.board_x = board_x;
    s_fire_map.board_y = board_y;
    s_fire_map.cell = cell;
    s_fire_map.screen_width = screen_width;
    s_fire_map.screen_height = screen_height;

    for (sy = 0; sy < FIRE_VISIBLE_H; sy++) {
        s32 native_y = native_y0 + sy;
        s32 y0 = board_y + fire_floor_div_s64((s64)(native_y - 36) * cell, 11);
        s32 y1 = board_y + fire_floor_div_s64((s64)(native_y + 1 - 36) * cell + 10, 11);
        if (y1 <= y0) y1 = y0 + 1;
        if (y0 < 0) y0 = 0;
        if (y1 < 0) y1 = 0;
        if (y0 > screen_height) y0 = screen_height;
        if (y1 > screen_height) y1 = screen_height;
        s_fire_map.y0[sy] = (s16)y0;
        s_fire_map.y1[sy] = (s16)y1;
    }

    for (which = 0; which < 2; which++) {
        const s32 native_x0 = centers_x[which] - (FIRE_W >> 1);
        for (sx = 0; sx < FIRE_W; sx++) {
            s32 native_x = native_x0 + sx;
            s32 x0 = board_x + fire_floor_div_s64((s64)(native_x - 83) * cell, 11);
            s32 x1 = board_x + fire_floor_div_s64((s64)(native_x + 1 - 83) * cell + 10, 11);
            if (x1 <= x0) x1 = x0 + 1;
            if (x0 < 0) x0 = 0;
            if (x1 < 0) x1 = 0;
            if (x0 > screen_width) x0 = screen_width;
            if (x1 > screen_width) x1 = screen_width;
            s_fire_map.x0[which][sx] = (s16)x0;
            s_fire_map.x1[which][sx] = (s16)x1;
        }
    }
    s_fire_map.ready = 1;
}

static void fire_draw_one(
    void *buffer, s32 stride, const u8 *src, s32 which
) {
    uint32_t *pixels = (uint32_t *)buffer;
    s32 sy;

    for (sy = 0; sy < FIRE_VISIBLE_H; sy++) {
        const s32 dy0 = s_fire_map.y0[sy];
        const s32 dy1 = s_fire_map.y1[sy];
        s32 sx;
        if (dy0 >= dy1) continue;

        for (sx = 0; sx < FIRE_W; sx++) {
            const u8 intensity = src[sy * FIRE_W + sx];
            const s32 dx0 = s_fire_map.x0[which][sx];
            const s32 dx1 = s_fire_map.x1[which][sx];
            s32 py;
            if (intensity == 0 || dx0 >= dx1) continue;

            for (py = dy0; py < dy1; py++) {
                uint32_t *row = pixels + py * stride;
                s32 px;
                for (px = dx0; px < dx1; px++)
                    row[px] = blend_fire_pixel(row[px], intensity);
            }
        }
    }
}

/* TEST109A, isolated from the hardware-validated TEST107A:
 * Original fire_draw_one() performs hundreds of tiny reads/writes against
 * GXM's relatively expensive shared CDRAM framebuffer. Its source sprites
 * overlap, so repeated reads/writes of those same CDRAM pixels are costly.
 *
 * Copy JUST the flame's bounding rectangle to a small, ordinary RAM
 * scratch tile, execute the EXACT ORIGINAL source-pixel loops, blending
 * calculation and drawing order against that scratch tile, then copy the
 * entire finished rectangle back to CDRAM once. The unchanged original
 * function is a fail-safe fallback if geometry exceeds our local scratch.
 * This is NOT an approximation or a precomposed alpha texture. Sequential
 * overdraw of overlapping sources remains exactly preserved.
 *
 * 17*64*16 RGBA pixels = 69,632 bytes allocated in BSS. The normal 1P
 * 20px cell scale uses only roughly 31x116 pixels per flame; the
 * upper bound accommodates substantially enlarged geometry as well.
 */
#define TNT109_FIRE_SCRATCH_PIXELS (FIRE_W * FIRE_VISIBLE_H * 16U)
static uint32_t s_tnt109_fire_scratch[TNT109_FIRE_SCRATCH_PIXELS];

static void fire_draw_one_scratch(
    void *buffer, s32 stride, const u8 *src, s32 which
) {
    uint32_t *pixels = (uint32_t *)buffer;
    s32 minx = 32767, maxx = 0, miny = 32767, maxy = 0;
    s32 sx, sy;
    size_t width, height;

    /* Bounds are drawn from the SAME CLIPPED map as original fire_draw_one.
     * Keep both rectangles separated and compose one at a time so even
     * unusual future overlapping map geometry retains original order. */
    for (sx = 0; sx < FIRE_W; sx++) {
        if (s_fire_map.x0[which][sx] < minx)
            minx = s_fire_map.x0[which][sx];
        if (s_fire_map.x1[which][sx] > maxx)
            maxx = s_fire_map.x1[which][sx];
    }
    for (sy = 0; sy < FIRE_VISIBLE_H; sy++) {
        if (s_fire_map.y0[sy] < miny)
            miny = s_fire_map.y0[sy];
        if (s_fire_map.y1[sy] > maxy)
            maxy = s_fire_map.y1[sy];
    }
    if (maxx <= minx || maxy <= miny)
        return;
    width = (size_t)(maxx - minx);
    height = (size_t)(maxy - miny);
    if (width == 0 || height > TNT109_FIRE_SCRATCH_PIXELS / width) {
        fire_draw_one(buffer, stride, src, which);
        return;
    }

    for (sy = miny; sy < maxy; sy++) {
        memcpy(s_tnt109_fire_scratch + (size_t)(sy - miny)*width,
               pixels + (size_t)sy*stride + minx,
               width*sizeof(uint32_t));
    }

    /* SAME iteration order as fire_draw_one: sy then sx then py then px.
     * SAME source intensity; SAME original fire blend function; only the
     * backing pixel memory location changes. Preserve alpha exactly. */
    for (sy = 0; sy < FIRE_VISIBLE_H; sy++) {
        const s32 dy0 = s_fire_map.y0[sy];
        const s32 dy1 = s_fire_map.y1[sy];
        if (dy0 >= dy1) continue;
        for (sx = 0; sx < FIRE_W; sx++) {
            const u8 intensity = src[sy*FIRE_W + sx];
            const s32 dx0 = s_fire_map.x0[which][sx];
            const s32 dx1 = s_fire_map.x1[which][sx];
            s32 py;
            if (intensity == 0 || dx0 >= dx1) continue;
            for (py = dy0; py < dy1; py++) {
                uint32_t *row = s_tnt109_fire_scratch +
                                (size_t)(py-miny)*width;
                s32 px;
                for (px = dx0; px < dx1; px++)
                    row[px-minx] = blend_fire_pixel(
                        row[px-minx],intensity);
            }
        }
    }

    for (sy = miny; sy < maxy; sy++) {
        memcpy(pixels + (size_t)sy*stride + minx,
               s_tnt109_fire_scratch + (size_t)(sy-miny)*width,
               width*sizeof(uint32_t));
    }
}

void TNT_Vita_MayanLargeFire_Render(
    void *buffer,
    s32 stride,
    s32 screen_width,
    s32 screen_height,
    s32 board_x,
    s32 board_y,
    s32 cell,
    TNTMayanLargeFire *fire
) {
    const u8 *src = fire->buffers[fire->draw_buffer];

    fire_prepare_map(board_x, board_y, cell, screen_width, screen_height);
    fire_draw_one_scratch(buffer, stride, src, 0);
    fire_draw_one_scratch(buffer, stride, src, 1);

    fire->draw_buffer ^= 1;
}
