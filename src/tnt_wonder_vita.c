#include "tnt_wonder_vita.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#define TNT_WONDER_TRIANGLE_COUNT 568
#define TNT_WONDER_TEXTURE_COUNT 20
#define TNT_WONDER_DEG2RAD (3.14159265358979323846f / 180.0f)

typedef struct {
    f32 m[4][4];
} TNTWonderMatrix;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 u;
    f32 v;
    f32 shade;
} TNTWonderProjectedVertex;

/* The original Borg scene is rasterized by the N64 at 400x300. Keep the
 * expensive 3D stage in that native space, then map its finished pixels into
 * the Vita framebuffer with the same 400x300 coordinate conversion used by
 * the rest of the port. This is both source-faithful and substantially less
 * CPU work than rasterizing the model directly at 960x544. */
/* q stores reciprocal depth. A frame stamp avoids clearing the depth buffer
 * every frame: untouched native pixels simply have an older stamp. */
static f32 s_tnt_wonder_q[400 * 300];
static u16 s_tnt_wonder_stamp[400 * 300];
static u16 s_tnt_wonder_frame_stamp;
static uint32_t s_tnt_wonder_color[400 * 300];
static f32 s_tnt_wonder_texture_rgb[TNT_WONDER_TEXTURE_COUNT][80][3];
static u8 s_tnt_wonder_textures_ready;

/* Destination-to-source coordinate maps remove per-pixel signed division from
 * the native-to-Vita composite. The viewer layout is normally constant, but
 * cache parameters so the helper remains correct if that ever changes. */
static s16 s_tnt_wonder_map_x[960];
static s16 s_tnt_wonder_map_y[544];
static s32 s_tnt_wonder_map_board_x = 0x7FFFFFFF;
static s32 s_tnt_wonder_map_board_y = 0x7FFFFFFF;
static s32 s_tnt_wonder_map_cell = -1;
static s32 s_tnt_wonder_map_width = -1;
static s32 s_tnt_wonder_map_height = -1;

static void wonder_matrix_identity(TNTWonderMatrix *m) {
    memset(m, 0, sizeof(*m));
    m->m[0][0] = 1.0f;
    m->m[1][1] = 1.0f;
    m->m[2][2] = 1.0f;
    m->m[3][3] = 1.0f;
}

static void wonder_matrix_mult(
    TNTWonderMatrix *out,
    const TNTWonderMatrix *a,
    const TNTWonderMatrix *b
) {
    TNTWonderMatrix r;
    s32 row, col, k;

    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            f32 v = 0.0f;
            for (k = 0; k < 4; k++)
                v += a->m[row][k] * b->m[k][col];
            r.m[row][col] = v;
        }
    }
    *out = r;
}

/* Exact transform type 2 used by Mayan scene nodes, from func_800A5114. */
static void wonder_matrix_type2(const f32 *v, TNTWonderMatrix *m) {
    f32 sx = sinf(-v[0] * TNT_WONDER_DEG2RAD);
    f32 cx = cosf(-v[0] * TNT_WONDER_DEG2RAD);
    f32 sy = sinf(-v[1] * TNT_WONDER_DEG2RAD);
    f32 cy = cosf(-v[1] * TNT_WONDER_DEG2RAD);
    f32 sz = sinf(-v[2] * TNT_WONDER_DEG2RAD);
    f32 cz = cosf(-v[2] * TNT_WONDER_DEG2RAD);

    wonder_matrix_identity(m);
    m->m[0][0] = cz * cx;
    m->m[0][1] = (sz * cy) + (cz * sx * sy);
    m->m[0][2] = (sz * sy) - (cz * sx * cy);
    m->m[1][0] = -sz * cx;
    m->m[1][1] = (cz * cy) - (sz * sx * sy);
    m->m[1][2] = (cz * sy) + (sz * sx * cy);
    m->m[2][0] = sx;
    m->m[2][1] = -cx * sy;
    m->m[2][2] = cx * cy;
    m->m[0][3] = v[3];
    m->m[1][3] = v[4];
    m->m[2][3] = v[5];
}

static f32 wonder_vec_len(f32 x, f32 y, f32 z) {
    return sqrtf(x * x + y * y + z * z);
}

static void wonder_normalize(f32 *x, f32 *y, f32 *z) {
    f32 len = wonder_vec_len(*x, *y, *z);
    if (len > 0.000001f) {
        *x /= len;
        *y /= len;
        *z /= len;
    }
}

/* Exact type-4 camera orientation from func_800A53E0, followed by the source
 * renderer's 16x camera-translation conversion and rigid inverse. */
static void wonder_camera_view(TNTWonderMatrix *view) {
    const f32 *v = g_tnt_wonder_mayan_nodes[0];
    f32 ex = v[0], ey = v[1], ez = v[2];
    f32 fx = ex - v[3], fy = ey - v[4], fz = ez - v[5];
    f32 rx, ry, rz;
    f32 ux, uy, uz;
    f32 roll_c, roll_s;
    f32 rrx, rry, rrz, rux, ruy, ruz;

    if (fx == 0.0f && fy == 0.0f && fz == 0.0f)
        fz = -1.0f;
    wonder_normalize(&fx, &fy, &fz);

    rx = fz;
    ry = 0.0f;
    rz = -fx;
    wonder_normalize(&rx, &ry, &rz);

    ux = (fy * rz) - (ry * fz);
    uy = (fz * rx) - (rz * fx);
    uz = (fx * ry) - (rx * fy);

    roll_c = cosf(v[6] * TNT_WONDER_DEG2RAD);
    roll_s = sinf(v[6] * TNT_WONDER_DEG2RAD);
    rrx = (rx * roll_c) + (ux * roll_s);
    rry = (ry * roll_c) + (uy * roll_s);
    rrz = (rz * roll_c) + (uz * roll_s);
    rux = (-rx * roll_s) + (ux * roll_c);
    ruy = (-ry * roll_s) + (uy * roll_c);
    ruz = (-rz * roll_s) + (uz * roll_c);

    wonder_matrix_identity(view);
    /* Inverse of the source camera-to-world matrix. */
    view->m[0][0] = rrx;
    view->m[0][1] = rry;
    view->m[0][2] = rrz;
    view->m[1][0] = rux;
    view->m[1][1] = ruy;
    view->m[1][2] = ruz;
    view->m[2][0] = fx;
    view->m[2][1] = fy;
    view->m[2][2] = fz;

    ex *= 16.0f;
    ey *= 16.0f;
    ez *= 16.0f;
    view->m[0][3] = -(rrx * ex + rry * ey + rrz * ez);
    view->m[1][3] = -(rux * ex + ruy * ey + ruz * ez);
    view->m[2][3] = -(fx * ex + fy * ey + fz * ez);
}

static void wonder_transform_point(
    const TNTWonderMatrix *m,
    f32 x, f32 y, f32 z,
    f32 *ox, f32 *oy, f32 *oz
) {
    *ox = m->m[0][0] * x + m->m[0][1] * y + m->m[0][2] * z + m->m[0][3];
    *oy = m->m[1][0] * x + m->m[1][1] * y + m->m[1][2] * z + m->m[1][3];
    *oz = m->m[2][0] * x + m->m[2][1] * y + m->m[2][2] * z + m->m[2][3];
}

static void wonder_transform_vector(
    const TNTWonderMatrix *m,
    f32 x, f32 y, f32 z,
    f32 *ox, f32 *oy, f32 *oz
) {
    *ox = m->m[0][0] * x + m->m[0][1] * y + m->m[0][2] * z;
    *oy = m->m[1][0] * x + m->m[1][1] * y + m->m[1][2] * z;
    *oz = m->m[2][0] * x + m->m[2][1] * y + m->m[2][2] * z;
}

static void wonder_prepare_textures(void) {
    s32 texture, texel;

    if (s_tnt_wonder_textures_ready)
        return;

    for (texture = 0; texture < TNT_WONDER_TEXTURE_COUNT; texture++) {
        for (texel = 0; texel < 80; texel++) {
            u16 p = g_tnt_wonder_mayan_textures[texture][texel];
            s_tnt_wonder_texture_rgb[texture][texel][0] =
                (f32)((p >> 11) & 0x1F) * (255.0f / 31.0f);
            s_tnt_wonder_texture_rgb[texture][texel][1] =
                (f32)((p >> 6) & 0x1F) * (255.0f / 31.0f);
            s_tnt_wonder_texture_rgb[texture][texel][2] =
                (f32)((p >> 1) & 0x1F) * (255.0f / 31.0f);
        }
    }
    s_tnt_wonder_textures_ready = TRUE;
}

static void wonder_sample_level(
    const f32 (*tex)[3],
    s32 size,
    f32 u,
    f32 v,
    f32 *r,
    f32 *g,
    f32 *b
) {
    s32 x0, y0, x1, y1;
    f32 fx, fy;
    const f32 *c00, *c10, *c01, *c11;

    if (u < 0.0f) u = 0.0f;
    if (v < 0.0f) v = 0.0f;
    if (u > (f32)(size - 1)) u = (f32)(size - 1);
    if (v > (f32)(size - 1)) v = (f32)(size - 1);
    x0 = (s32)u;
    y0 = (s32)v;
    x1 = x0 + 1 < size ? x0 + 1 : x0;
    y1 = y0 + 1 < size ? y0 + 1 : y0;
    fx = u - (f32)x0;
    fy = v - (f32)y0;

    c00 = tex[y0 * size + x0];
    c10 = tex[y0 * size + x1];
    c01 = tex[y1 * size + x0];
    c11 = tex[y1 * size + x1];
    *r = (c00[0] + (c10[0] - c00[0]) * fx) * (1.0f - fy) +
         (c01[0] + (c11[0] - c01[0]) * fx) * fy;
    *g = (c00[1] + (c10[1] - c00[1]) * fx) * (1.0f - fy) +
         (c01[1] + (c11[1] - c01[1]) * fx) * fy;
    *b = (c00[2] + (c10[2] - c00[2]) * fx) * (1.0f - fy) +
         (c01[2] + (c11[2] - c01[2]) * fx) * fy;
}

static void wonder_sample_texture(
    u8 texture,
    f32 u,
    f32 v,
    f32 lod,
    f32 *r,
    f32 *g,
    f32 *b
) {
    f32 r0, g0, b0, r1, g1, b1;
    const f32 (*src)[3] = s_tnt_wonder_texture_rgb[texture];

    wonder_sample_level(src, 8, u, v, &r0, &g0, &b0);
    if (lod <= 0.0f) {
        *r = r0; *g = g0; *b = b0;
        return;
    }
    if (lod > 1.0f) lod = 1.0f;
    wonder_sample_level(src + 64, 4, u * 0.5f, v * 0.5f, &r1, &g1, &b1);
    *r = r0 + (r1 - r0) * lod;
    *g = g0 + (g1 - g0) * lod;
    *b = b0 + (b1 - b0) * lod;
}

static s32 wonder_floor_div_s64(s64 n, s64 d) {
    if (n >= 0)
        return (s32)(n / d);
    return -(s32)(((-n) + d - 1) / d);
}

static void wonder_prepare_native_map(
    s32 board_x,
    s32 board_y,
    s32 cell,
    s32 screen_width,
    s32 screen_height
) {
    s32 i;

    if (s_tnt_wonder_map_board_x == board_x &&
        s_tnt_wonder_map_board_y == board_y &&
        s_tnt_wonder_map_cell == cell &&
        s_tnt_wonder_map_width == screen_width &&
        s_tnt_wonder_map_height == screen_height)
        return;

    for (i = 0; i < screen_width && i < 960; i++)
        s_tnt_wonder_map_x[i] = (s16)(83 +
            wonder_floor_div_s64((s64)(i - board_x) * 11, cell));
    for (i = 0; i < screen_height && i < 544; i++)
        s_tnt_wonder_map_y[i] = (s16)(36 +
            wonder_floor_div_s64((s64)(i - board_y) * 11, cell));

    s_tnt_wonder_map_board_x = board_x;
    s_tnt_wonder_map_board_y = board_y;
    s_tnt_wonder_map_cell = cell;
    s_tnt_wonder_map_width = screen_width;
    s_tnt_wonder_map_height = screen_height;
}

static f32 wonder_triangle_lod(const TNTWonderProjectedVertex *v) {
    f32 dx1=v[1].x-v[0].x, dy1=v[1].y-v[0].y;
    f32 dx2=v[2].x-v[0].x, dy2=v[2].y-v[0].y;
    f32 du1=v[1].u-v[0].u, dv1=v[1].v-v[0].v;
    f32 du2=v[2].u-v[0].u, dv2=v[2].v-v[0].v;
    f32 det=dx1*dy2-dx2*dy1;
    f32 dudx,dudy,dvdx,dvdy,rho,lod;
    if (fabsf(det)<0.00001f) return 0.0f;
    dudx=(du1*dy2-du2*dy1)/det;
    dudy=(du2*dx1-du1*dx2)/det;
    dvdx=(dv1*dy2-dv2*dy1)/det;
    dvdy=(dv2*dx1-dv1*dx2)/det;
    rho=fmaxf(sqrtf(dudx*dudx+dvdx*dvdx),sqrtf(dudy*dudy+dvdy*dvdy));
    if (rho<=1.0f) return 0.0f;
    lod=log2f(rho);
    if (lod>1.0f) lod=1.0f;
    return lod;
}

void TNT_Vita_WonderRenderMayan(
    void *buffer,
    s32 stride,
    s32 screen_width,
    s32 screen_height,
    s32 board_x,
    s32 board_y,
    s32 cell,
    u16 anim_tick
) {
    TNTWonderMatrix root, view, local, world[18], draw_model[18], mv[18];
    f32 root_values[9];
    uint32_t *dst = (uint32_t *)buffer;
    f32 focal = 1.0f / tanf(15.0f * TNT_WONDER_DEG2RAD);
    s32 i, y;
    u16 frame_stamp;
    /* Exhaustive 512-tick sweep: actual native bounds are approximately
     * x=130.28..316.72, y=121.95..236.78. Keep a small safety margin. */
    const s32 clear_x0 = 125;
    const s32 clear_y0 = 118;
    const s32 clear_x1 = 322;
    const s32 clear_y1 = 241;

    wonder_prepare_textures();
    wonder_prepare_native_map(
        board_x, board_y, cell, screen_width, screen_height);

    frame_stamp = (u16)(s_tnt_wonder_frame_stamp + 1U);
    if (frame_stamp == 0) {
        memset(s_tnt_wonder_stamp, 0, sizeof(s_tnt_wonder_stamp));
        frame_stamp = 1;
    }
    s_tnt_wonder_frame_stamp = frame_stamp;

    memcpy(root_values, g_tnt_wonder_mayan_nodes[17], sizeof(root_values));
    root_values[0] = -1.40625f - 0.703125f * (f32)(anim_tick & 511U);
    wonder_matrix_type2(root_values, &root);
    wonder_camera_view(&view);

    world[17] = root;
    for (i = 1; i <= 16; i++) {
        s32 k;
        wonder_matrix_type2(g_tnt_wonder_mayan_nodes[i], &local);
        wonder_matrix_mult(&world[i], &root, &local);

        /* Borg applies the selected mesh's common 0.125 scale to the 3x3
         * basis and converts node translation to renderer units by 16 before
         * submitting the model-view matrix. All Mayan meshes share that scale. */
        draw_model[i] = world[i];
        for (k = 0; k < 3; k++) {
            draw_model[i].m[k][0] *= 0.125f;
            draw_model[i].m[k][1] *= 0.125f;
            draw_model[i].m[k][2] *= 0.125f;
            draw_model[i].m[k][3] *= 16.0f;
        }
        wonder_matrix_mult(&mv[i], &view, &draw_model[i]);
    }

    for (i = 0; i < TNT_WONDER_TRIANGLE_COUNT; i++) {
        const TNTWonderTriangle *tri = &g_tnt_wonder_mayan_triangles[i];
        const TNTWonderMatrix *model = &draw_model[tri->node];
        const TNTWonderMatrix *model_view = &mv[tri->node];
        TNTWonderProjectedVertex p[3];
        f32 area, lod;
        f32 iz[3];
        s32 k, minx, maxx, miny, maxy;

        for (k = 0; k < 3; k++) {
            const TNTWonderVertex *v = &tri->v[k];
            f32 cx, cy, cz, nx, ny, nz, len, dot, depth;

            wonder_transform_point(
                model_view, (f32)v->x, (f32)v->y, (f32)v->z,
                &cx, &cy, &cz);
            depth = -cz;
            if (depth < 4.0f || depth > 3200.0f)
                goto skip_triangle;

            p[k].x = 200.0f + (cx * focal / (4.0f / 3.0f) / depth) * 200.0f;
            p[k].y = 150.0f - (cy * focal / depth) * 150.0f;
            p[k].z = depth;
            p[k].u = (f32)v->s / 64.0f;
            p[k].v = (f32)v->t / 64.0f;
            iz[k] = 1.0f / depth;

            wonder_transform_vector(
                model,
                (f32)v->nx / 127.0f,
                (f32)v->ny / 127.0f,
                (f32)v->nz / 127.0f,
                &nx, &ny, &nz);
            len = wonder_vec_len(nx, ny, nz);
            if (len > 0.000001f) {
                nx /= len; ny /= len; nz /= len;
            }
            dot = ny * (120.0f / 127.0f);
            if (dot < 0.0f) dot = 0.0f;
            p[k].shade = (127.0f / 255.0f) + dot;
            if (p[k].shade > 1.0f) p[k].shade = 1.0f;
        }

        area = (p[1].x - p[0].x) * (p[2].y - p[0].y) -
               (p[1].y - p[0].y) * (p[2].x - p[0].x);
        if (area <= 0.0f)
            continue;
        lod = wonder_triangle_lod(p);

        minx = (s32)floorf(fminf(p[0].x, fminf(p[1].x, p[2].x)));
        maxx = (s32)ceilf(fmaxf(p[0].x, fmaxf(p[1].x, p[2].x)));
        miny = (s32)floorf(fminf(p[0].y, fminf(p[1].y, p[2].y)));
        maxy = (s32)ceilf(fmaxf(p[0].y, fmaxf(p[1].y, p[2].y)));
        if (minx < clear_x0) minx = clear_x0;
        if (maxx >= clear_x1) maxx = clear_x1 - 1;
        if (miny < clear_y0) miny = clear_y0;
        if (maxy >= clear_y1) maxy = clear_y1 - 1;
        if (minx > maxx || miny > maxy)
            continue;

        {
            f32 inv_area = 1.0f / area;
            f32 dw0dx = (p[1].y - p[2].y) * inv_area;
            f32 dw0dy = (p[2].x - p[1].x) * inv_area;
            f32 dw1dx = (p[2].y - p[0].y) * inv_area;
            f32 dw1dy = (p[0].x - p[2].x) * inv_area;
            f32 dw2dx = -dw0dx - dw1dx;
            f32 dw2dy = -dw0dy - dw1dy;
            f32 px0 = (f32)minx + 0.5f;
            f32 py0 = (f32)miny + 0.5f;
            f32 w0_row = ((p[1].x - px0) * (p[2].y - py0) -
                          (p[1].y - py0) * (p[2].x - px0)) * inv_area;
            f32 w1_row = ((p[2].x - px0) * (p[0].y - py0) -
                          (p[2].y - py0) * (p[0].x - px0)) * inv_area;
            f32 w2_row = 1.0f - w0_row - w1_row;
            f32 uq[3] = {p[0].u * iz[0], p[1].u * iz[1], p[2].u * iz[2]};
            f32 vq[3] = {p[0].v * iz[0], p[1].v * iz[1], p[2].v * iz[2]};
            f32 sq[3] = {p[0].shade * iz[0], p[1].shade * iz[1], p[2].shade * iz[2]};
            f32 q_row = w0_row * iz[0] + w1_row * iz[1] + w2_row * iz[2];
            f32 u_row = w0_row * uq[0] + w1_row * uq[1] + w2_row * uq[2];
            f32 v_row = w0_row * vq[0] + w1_row * vq[1] + w2_row * vq[2];
            f32 s_row = w0_row * sq[0] + w1_row * sq[1] + w2_row * sq[2];
            f32 dqdx = dw0dx * iz[0] + dw1dx * iz[1] + dw2dx * iz[2];
            f32 dqdy = dw0dy * iz[0] + dw1dy * iz[1] + dw2dy * iz[2];
            f32 dudx = dw0dx * uq[0] + dw1dx * uq[1] + dw2dx * uq[2];
            f32 dudy = dw0dy * uq[0] + dw1dy * uq[1] + dw2dy * uq[2];
            f32 dvdx = dw0dx * vq[0] + dw1dx * vq[1] + dw2dx * vq[2];
            f32 dvdy = dw0dy * vq[0] + dw1dy * vq[1] + dw2dy * vq[2];
            f32 dsdx = dw0dx * sq[0] + dw1dx * sq[1] + dw2dx * sq[2];
            f32 dsdy = dw0dy * sq[0] + dw1dy * sq[1] + dw2dy * sq[2];

            for (y = miny; y <= maxy; y++) {
                s32 x;
                f32 w0 = w0_row, w1 = w1_row, w2 = w2_row;
                f32 q = q_row, u_over_z = u_row, v_over_z = v_row;
                f32 shade_over_z = s_row;

                for (x = minx; x <= maxx; x++) {
                    s32 pos = y * 400 + x;

                    if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f && q > 0.0f &&
                        (s_tnt_wonder_stamp[pos] != frame_stamp ||
                         q > s_tnt_wonder_q[pos])) {
                        f32 invq = 1.0f / q;
                        f32 u = u_over_z * invq;
                        f32 v = v_over_z * invq;
                        f32 shade = shade_over_z * invq;
                        f32 r, g, b;

                        wonder_sample_texture(
                            tri->texture, u, v, lod, &r, &g, &b);
                        r *= shade; g *= shade; b *= shade;
                        if (r > 255.0f) r = 255.0f;
                        if (g > 255.0f) g = 255.0f;
                        if (b > 255.0f) b = 255.0f;
                        s_tnt_wonder_color[pos] =
                            0xFF000000U |
                            ((uint32_t)b << 16) |
                            ((uint32_t)g << 8) |
                            (uint32_t)r;
                        s_tnt_wonder_q[pos] = q;
                        s_tnt_wonder_stamp[pos] = frame_stamp;
                    }

                    w0 += dw0dx; w1 += dw1dx; w2 += dw2dx;
                    q += dqdx; u_over_z += dudx; v_over_z += dvdx;
                    shade_over_z += dsdx;
                }

                w0_row += dw0dy; w1_row += dw1dy; w2_row += dw2dy;
                q_row += dqdy; u_row += dudy; v_row += dvdy; s_row += dsdy;
            }
        }
skip_triangle:
        ;
    }

    /* Composite the completed 400x300 render using the exact source/native
     * mapping convention used by the port's sprite renderers. */
    {
        s32 dx0 = board_x + wonder_floor_div_s64((s64)(clear_x0 - 83) * cell, 11);
        s32 dy0 = board_y + wonder_floor_div_s64((s64)(clear_y0 - 36) * cell, 11);
        s32 dx1 = board_x + wonder_floor_div_s64((s64)(clear_x1 - 83) * cell + 10, 11);
        s32 dy1 = board_y + wonder_floor_div_s64((s64)(clear_y1 - 36) * cell + 10, 11);

        if (dx0 < 0) dx0 = 0;
        if (dy0 < 0) dy0 = 0;
        if (dx1 > screen_width) dx1 = screen_width;
        if (dy1 > screen_height) dy1 = screen_height;

        for (y = dy0; y < dy1; y++) {
            s32 native_y = s_tnt_wonder_map_y[y];
            s32 x;
            if (native_y < clear_y0 || native_y >= clear_y1)
                continue;

            for (x = dx0; x < dx1; x++) {
                s32 native_x = s_tnt_wonder_map_x[x];
                s32 pos;
                if (native_x < clear_x0 || native_x >= clear_x1)
                    continue;
                pos = native_y * 400 + native_x;
                if (s_tnt_wonder_stamp[pos] == frame_stamp)
                    dst[y * stride + x] = s_tnt_wonder_color[pos];
            }
        }
    }
}
