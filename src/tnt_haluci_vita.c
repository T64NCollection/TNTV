#include "tnt_haluci_vita.h"

#include <math.h>
#include <string.h>

#define HALUCI_W 400
#define HALUCI_H 300
#define HALUCI_COUNT 350
#define HALUCI_TWO_PI 6.2831853f
#define HALUCI_COLOR(r,g,b) ((u32)(0xFF000000u | ((u32)(b) << 16) | ((u32)(g) << 8) | (u32)(r)))

typedef struct {
    u8 initialized;
    u8 feedback;
    f32 shape_phase;       /* D_800D315C */
    f32 shape_phase_rate;  /* D_800D3160 */
    f32 wave_phase;        /* D_800D316C */
    f32 color_phase;       /* D_800D3178 */
    u8 radial[16 * 16];
    u32 color_rng;
} TNTHaluciState;

static TNTHaluciState s_haluci;
static u32 s_native[HALUCI_W * HALUCI_H];
static u32 s_previous[HALUCI_W * HALUCI_H];

static u8 s_haluci_gpu_first_frame;

static int haluci_floor_div(long long n, int d) {
    if (n >= 0)
        return (int)(n / d);
    return -(int)((-n + d - 1) / d);
}

static u8 haluci_clamp_u8(int x) {
    if (x < 0) return 0;
    if (x > 255) return 255;
    return (u8)x;
}

static u32 haluci_blend(u32 dst, u8 r, u8 g, u8 b, u8 a) {
    u32 ia = 255u - a;
    u8 dr = (u8)(dst & 0xFFu);
    u8 dg = (u8)((dst >> 8) & 0xFFu);
    u8 db = (u8)((dst >> 16) & 0xFFu);
    return HALUCI_COLOR(
        (u8)(((u32)r * a + (u32)dr * ia + 127u) / 255u),
        (u8)(((u32)g * a + (u32)dg * ia + 127u) / 255u),
        (u8)(((u32)b * a + (u32)db * ia + 127u) / 255u));
}

/* Source func_800B110C adds a tiny guRandom()-driven perturbation before
 * generating each HSL channel. Preserve the same +/-0.01 shimmer locally so
 * HALUCI does not disturb TNTV's gameplay RNG state. */
static f32 haluci_color_wave(f32 x) {
    f32 jitter;
    s_haluci.color_rng = s_haluci.color_rng * 1664525u + 1013904223u;
    jitter = ((f32)((s_haluci.color_rng >> 8) & 0xFFFFu) / 32767.5f) - 1.0f;
    x = fabsf(x + jitter * 0.01f);
    if (x > 1.0f) return 0.0f;
    if (x < 0.5f) return 1.0f;
    x -= 0.5f;
    return 1.0f - 2.0f * x;
}

static void haluci_hsl_to_rgb(f32 hue, f32 lightness, f32 saturation,
                              u8 *r, u8 *g, u8 *b) {
    f32 h = hue * 3.0f;
    f32 ch0, ch1, ch2;
    f32 factor, white = 0.0f;
    while (h > 3.0f) h -= 3.0f;
    while (h < 0.0f) h += 3.0f;
    ch0 = haluci_color_wave(h - 3.0f) + haluci_color_wave(h);
    ch1 = haluci_color_wave(h - 1.0f);
    ch2 = haluci_color_wave(h - 2.0f);
    if (lightness <= 0.5f) {
        factor = lightness * 2.0f * saturation;
    } else {
        f32 t = 2.0f * (lightness - 0.5f);
        factor = (1.0f - t) * saturation;
        white = t;
    }
    *r = haluci_clamp_u8((int)((ch0 * factor + white) * 255.0f));
    *g = haluci_clamp_u8((int)((ch1 * factor + white) * 255.0f));
    *b = haluci_clamp_u8((int)((ch2 * factor + white) * 255.0f));
}

static void haluci_build_radial(void) {
    int y, x;
    for (y = 0; y < 16; y++) {
        f32 fy = (f32)((y * 2) - 16) / 16.0f;
        for (x = 0; x < 16; x++) {
            f32 fx = (f32)((x * 2) - 16) / 16.0f;
            f32 v = sqrtf(fx * fx + fy * fy);
            if (v > 0.6f) {
                v -= 0.6f;
                v = v * -(0.6f / 0.4f) + 0.6f;
                v *= (0.5f / 0.3f);
            } else {
                v *= (0.4f / 0.3f);
                v += 0.2f;
            }
            if (v < 0.0f) v = 0.0f;
            if (v > 1.0f) v = 1.0f;
            s_haluci.radial[y * 16 + x] = (u8)(v * 255.0f);
        }
    }
}

static void haluci_feedback_pass(void) {
    const f32 angle = 0.1f;  /* D_800D3180 */
    const f32 sx = 1.1f;     /* D_800D3150.x */
    const f32 sy = 1.1f;     /* D_800D3150.y */
    const f32 c = 0.9950041653f;
    const f32 s = 0.0998334166f;
    const f32 cx = 199.5f;
    const f32 cy = 149.5f;
    const u8 alpha = 0xC0;
    int y, x;
    (void)angle;

    memset(s_native, 0, sizeof(s_native));
    for (y = 0; y < HALUCI_H; y++) {
        f32 dy = (f32)y - cy;
        for (x = 0; x < HALUCI_W; x++) {
            f32 dx = (f32)x - cx;
            /* Inverse of source func_800AE70C's center-preserving
             * rotate(0.1) * scale(1.1) previous-frame transform. */
            f32 srcx = ( c * dx + s * dy) / sx + cx;
            f32 srcy = (-s * dx + c * dy) / sy + cy;
            int ix = (int)floorf(srcx + 0.5f);
            int iy = (int)floorf(srcy + 0.5f);
            if ((unsigned)ix < HALUCI_W && (unsigned)iy < HALUCI_H) {
                u32 src = s_previous[iy * HALUCI_W + ix];
                u8 r = (u8)(((src & 0xFFu) * 0xFAu) / 255u);
                u8 g = (u8)((((src >> 8) & 0xFFu) * 0xFAu) / 255u);
                u8 b = (u8)((((src >> 16) & 0xFFu) * 0xFAu) / 255u);
                s_native[y * HALUCI_W + x] = haluci_blend(0, r, g, b, alpha);
            }
        }
    }
}

static void haluci_draw_disc(f32 cx, f32 cy, f32 half,
                             u8 cr, u8 cg, u8 cb) {
    int x0, y0, x1, y1, y, x;
    if (half < 0.35f)
        half = 0.35f;
    x0 = (int)floorf(cx - half);
    y0 = (int)floorf(cy - half);
    x1 = (int)ceilf(cx + half);
    y1 = (int)ceilf(cy + half);
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 >= HALUCI_W) x1 = HALUCI_W - 1;
    if (y1 >= HALUCI_H) y1 = HALUCI_H - 1;
    if (x0 > x1 || y0 > y1)
        return;

    for (y = y0; y <= y1; y++) {
        f32 v = (((f32)y + 0.5f) - (cy - half)) / (2.0f * half);
        int tv = (int)(v * 16.0f);
        if (tv < 0) tv = 0;
        if (tv > 15) tv = 15;
        for (x = x0; x <= x1; x++) {
            f32 u = (((f32)x + 0.5f) - (cx - half)) / (2.0f * half);
            int tu = (int)(u * 16.0f);
            u8 intensity;
            u8 a, r, g, b;
            u32 *dst;
            if (tu < 0) tu = 0;
            if (tu > 15) tu = 15;
            intensity = s_haluci.radial[tv * 16 + tu];
            if (intensity == 0)
                continue;
            /* I8 under G_CC_MODULATEIA_PRIM: intensity modulates both the
             * primitive RGB and its source alpha (source alpha is 0.6). */
            a = (u8)(((u32)intensity * 153u) / 255u);
            r = (u8)(((u32)cr * intensity) / 255u);
            g = (u8)(((u32)cg * intensity) / 255u);
            b = (u8)(((u32)cb * intensity) / 255u);
            dst = &s_native[y * HALUCI_W + x];
            *dst = haluci_blend(*dst, r, g, b, a);
        }
    }
}

static void haluci_draw_particles(void) {
    const f32 fovy = 110.0f;
    const f32 focal = (HALUCI_H * 0.5f) /
        tanf((fovy * 0.5f) * (HALUCI_TWO_PI / 360.0f));
    const f32 windings = s_haluci.shape_phase * 20.0f + 50.0f;
    const f32 theta_step = windings * HALUCI_TWO_PI / (f32)HALUCI_COUNT;
    const f32 freq = sinf(s_haluci.wave_phase) * 2.05f + 1.001f;
    const f32 phase = s_haluci.shape_phase * 120.0f;
    f32 radius[HALUCI_COUNT];
    f32 scale[HALUCI_COUNT];
    f32 r = 7.0f;
    f32 sc = 1.8f;
    int i;

    for (i = 0; i < HALUCI_COUNT; i++) {
        radius[i] = r;
        scale[i] = sc + 0.1f;
        r *= 0.99006392f;
        sc *= 0.99006392f;
    }

    /* func_800AD2FC traverses the circular particle pool newest-to-oldest,
     * which is index 349 down to 0 immediately after Haluci initialization. */
    for (i = HALUCI_COUNT - 1; i >= 0; i--) {
        f32 theta = (f32)i * theta_step;
        f32 z = (sinf(theta * freq) + 1.0f) * 2.0f;
        f32 x = cosf(theta + phase) * radius[i];
        f32 y = sinf(theta + phase) * radius[i];
        f32 distance = z + 5.0f;
        f32 sx, sy, half;
        f32 age = (f32)i / (f32)HALUCI_COUNT;
        f32 hue = s_haluci.color_phase + 1.5f * age;
        u8 cr, cg, cb;
        if (distance < 0.01f)
            continue;
        sx = HALUCI_W * 0.5f + x * focal / distance;
        sy = HALUCI_H * 0.5f - y * focal / distance;
        half = scale[i] * focal / distance;
        haluci_hsl_to_rgb(hue, 0.5f, 1.0f, &cr, &cg, &cb);
        haluci_draw_disc(sx, sy, half, cr, cg, cb);
    }
}

static void haluci_advance_one_tick(void) {
    s_haluci.shape_phase += s_haluci.shape_phase_rate;
    if (s_haluci.shape_phase >= 1.0f || s_haluci.shape_phase < -1.0f) {
        s_haluci.shape_phase -= s_haluci.shape_phase_rate;
        s_haluci.shape_phase_rate *= -1.0f;
    }
    s_haluci.wave_phase += 0.003681554f;
    if (s_haluci.wave_phase > HALUCI_TWO_PI)
        s_haluci.wave_phase -= HALUCI_TWO_PI;
    s_haluci.color_phase += 0.003f;
    if (s_haluci.color_phase > 1.0f)
        s_haluci.color_phase -= 1.0f;
}

static void haluci_scale_to_vita(void *buffer, int stride, int width, int height,
                                 int board_x, int board_y, int cell) {
    u32 *dst = (u32 *)buffer;
    int dx0 = board_x + haluci_floor_div((long long)(0 - 83) * cell, 11);
    int dy0 = board_y + haluci_floor_div((long long)(0 - 36) * cell, 11);
    int dx1 = board_x + haluci_floor_div((long long)(400 - 83) * cell + 10, 11);
    int dy1 = board_y + haluci_floor_div((long long)(300 - 36) * cell + 10, 11);
    int y, x;
    memset(dst, 0, (size_t)stride * (size_t)height * sizeof(u32));
    if (dx0 < 0) dx0 = 0;
    if (dy0 < 0) dy0 = 0;
    if (dx1 > width) dx1 = width;
    if (dy1 > height) dy1 = height;
    for (y = dy0; y < dy1; y++) {
        int sy = 36 + haluci_floor_div((long long)(y - board_y) * 11, cell);
        if ((unsigned)sy >= HALUCI_H) continue;
        for (x = dx0; x < dx1; x++) {
            int sx = 83 + haluci_floor_div((long long)(x - board_x) * 11, cell);
            if ((unsigned)sx < HALUCI_W)
                dst[y * stride + x] = s_native[sy * HALUCI_W + sx];
        }
    }
}

/* Test 2 cached and resampled a 400x300 feedback image on CPU. Test 3 leaves
 * the previous framebuffer untouched and lets GXM perform the affine feedback
 * transform directly while sampling it as a texture. */

static void haluci_prepare_particles(TNTHaluciFrame *frame) {
    const f32 fovy = 110.0f;
    const f32 focal = (HALUCI_H * 0.5f) /
        tanf((fovy * 0.5f) * (HALUCI_TWO_PI / 360.0f));
    const f32 windings = s_haluci.shape_phase * 20.0f + 50.0f;
    const f32 theta_step = windings * HALUCI_TWO_PI / (f32)HALUCI_COUNT;
    const f32 freq = sinf(s_haluci.wave_phase) * 2.05f + 1.001f;
    const f32 phase = s_haluci.shape_phase * 120.0f;
    f32 radius[HALUCI_COUNT];
    f32 scale[HALUCI_COUNT];
    f32 r = 7.0f;
    f32 sc = 1.8f;
    int i;

    for (i = 0; i < HALUCI_COUNT; i++) {
        radius[i] = r;
        scale[i] = sc + 0.1f;
        r *= 0.99006392f;
        sc *= 0.99006392f;
    }

    frame->particle_count = 0;
    for (i = HALUCI_COUNT - 1; i >= 0; i--) {
        f32 theta = (f32)i * theta_step;
        f32 z = (sinf(theta * freq) + 1.0f) * 2.0f;
        f32 x = cosf(theta + phase) * radius[i];
        f32 y = sinf(theta + phase) * radius[i];
        f32 distance = z + 5.0f;
        f32 age = (f32)i / (f32)HALUCI_COUNT;
        f32 hue = s_haluci.color_phase + 1.5f * age;
        TNTHaluciParticle *p;
        if (distance < 0.01f)
            continue;
        p = &frame->particle[frame->particle_count++];
        p->cx = HALUCI_W * 0.5f + x * focal / distance;
        p->cy = HALUCI_H * 0.5f - y * focal / distance;
        p->half = scale[i] * focal / distance;
        if (p->half < 0.35f) p->half = 0.35f;
        haluci_hsl_to_rgb(hue, 0.5f, 1.0f, &p->r, &p->g, &p->b);
        p->pad = 0;
    }
}

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
) {
    u32 i;
    if (!frame)
        return;
    if (!s_haluci.initialized)
        TNT_Vita_HaluciInit();
    memset(frame, 0, sizeof(*frame));

    /* Source draws feedback using the old toggle value, then START changes the
     * flag before the current particle spiral is drawn. Preserve that order. */
    frame->feedback_this_frame =
        (u8)(s_haluci.feedback && !s_haluci_gpu_first_frame && previous_framebuffer != NULL);
    if (frame->feedback_this_frame) {
        frame->feedback_framebuffer = previous_framebuffer;
        frame->feedback_stride = (u16)previous_stride;
        frame->feedback_width = (u16)previous_width;
        frame->feedback_height = (u16)previous_height;
    }
    (void)board_x;
    (void)board_y;
    (void)cell;
    if (start_pressed)
        s_haluci.feedback ^= 1U;

    haluci_prepare_particles(frame);
    s_haluci_gpu_first_frame = FALSE;

    if (retrace_delta == 0) retrace_delta = 1;
    if (retrace_delta > 4) retrace_delta = 4;
    for (i = 0; i < retrace_delta; i++)
        haluci_advance_one_tick();
}

void TNT_Vita_HaluciInit(void) {
    memset(&s_haluci, 0, sizeof(s_haluci));
    memset(s_native, 0, sizeof(s_native));
    memset(s_previous, 0, sizeof(s_previous));
    s_haluci.initialized = TRUE;
    s_haluci_gpu_first_frame = TRUE;
    s_haluci.feedback = TRUE;          /* func_8007CF40(1) */
    s_haluci.shape_phase = 0.0f;
    s_haluci.shape_phase_rate = 0.00003f;
    s_haluci.wave_phase = 0.0f;
    s_haluci.color_phase = 0.0f;
    s_haluci.color_rng = 0x13579BDFu;
    haluci_build_radial();
}

void TNT_Vita_HaluciRender(void *buffer, int stride, int width, int height,
                           int board_x, int board_y, int cell,
                           u8 start_pressed, u32 retrace_delta) {
    u32 i;
    if (!s_haluci.initialized)
        TNT_Vita_HaluciInit();

    /* Source applies one last feedback pass on the frame START is pressed,
     * then toggles D_800D3040 before drawing the current spiral. */
    if (s_haluci.feedback)
        haluci_feedback_pass();
    else
        memset(s_native, 0, sizeof(s_native));
    if (start_pressed)
        s_haluci.feedback ^= 1U;

    haluci_draw_particles();
    memcpy(s_previous, s_native, sizeof(s_previous));
    haluci_scale_to_vita(buffer, stride, width, height, board_x, board_y, cell);

    if (retrace_delta == 0) retrace_delta = 1;
    if (retrace_delta > 4) retrace_delta = 4;
    for (i = 0; i < retrace_delta; i++)
        haluci_advance_one_tick();
}
