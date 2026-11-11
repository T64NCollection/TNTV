#include "tnt_wonder_milestone_vita.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#define TNT_MILESTONE_NATIVE_W 400
#define TNT_MILESTONE_NATIVE_H 300
#define TNT_MILESTONE_DEG2RAD (3.14159265358979323846f / 180.0f)
#define TNT_MILESTONE_SMALL_TEXTURE_COUNT 20
#define TNT_MILESTONE_LARGE_TEXTURE_COUNT 29

typedef struct {
    f32 m[4][4];
} TNTMilestoneMatrix;

typedef struct {
    f32 x, y, z;
    f32 u, v;
    f32 r, g, b;
} TNTMilestoneProjectedVertex;

static f32 s_milestone_q[TNT_MILESTONE_NATIVE_W * TNT_MILESTONE_NATIVE_H];
static u16 s_milestone_stamp[TNT_MILESTONE_NATIVE_W * TNT_MILESTONE_NATIVE_H];
static u16 s_milestone_frame_stamp;
static uint32_t s_milestone_color[TNT_MILESTONE_NATIVE_W * TNT_MILESTONE_NATIVE_H];
static f32 s_small_rgb[TNT_MILESTONE_SMALL_TEXTURE_COUNT][80][3];
static f32 s_large_rgb[TNT_MILESTONE_LARGE_TEXTURE_COUNT][1360][3];
static u8 s_textures_ready;
static u8 s_african_light_mode;

static s32 s_frame_minx, s_frame_miny, s_frame_maxx, s_frame_maxy;

static s16 s_map_x[960];
static s16 s_map_y[544];
static s32 s_map_board_x = 0x7fffffff;
static s32 s_map_board_y = 0x7fffffff;
static s32 s_map_cell = -1;
static s32 s_map_width = -1;
static s32 s_map_height = -1;

static const f32 s_light_dir_normalized[2][3] = {
    {-0.675145930f, 0.170366159f, -0.717741837f},
    { 0.585220326f, 0.264633076f,  0.766476683f}
};
static const f32 s_light_rgb[2][3] = {
    {0.613497f, 0.644172f, 0.687117f},
    {0.785300f, 0.766871f, 0.742331f}
};
static const f32 s_ambient_rgb[3] = {0.251500f, 0.190184f, 0.110429f};

static void mat_identity(TNTMilestoneMatrix *m) {
    memset(m, 0, sizeof(*m));
    m->m[0][0] = m->m[1][1] = m->m[2][2] = m->m[3][3] = 1.0f;
}

static void mat_mult(TNTMilestoneMatrix *out, const TNTMilestoneMatrix *a, const TNTMilestoneMatrix *b) {
    TNTMilestoneMatrix r;
    s32 row, col, k;
    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            f32 v = 0.0f;
            for (k = 0; k < 4; k++) v += a->m[row][k] * b->m[k][col];
            r.m[row][col] = v;
        }
    }
    *out = r;
}

/* func_800B036C: wrapper rotation used by UnkStruct_11. */
static void wrapper_matrix(f32 rx, f32 ry, f32 rz, f32 tx, f32 ty, f32 tz, TNTMilestoneMatrix *m) {
    f32 a = rx * TNT_MILESTONE_DEG2RAD;
    f32 b = ry * TNT_MILESTONE_DEG2RAD;
    f32 c = rz * TNT_MILESTONE_DEG2RAD;
    f32 ca = cosf(a), cb = cosf(b), cc = cosf(c);
    f32 sa = sinf(a), sb = sinf(b), sc = sinf(c);

    mat_identity(m);
    m->m[0][0] = (ca * cc) + (sa * sb * sc);
    m->m[0][1] = (-ca * sc) + (sa * sb * cc);
    m->m[0][2] = sa * cb;
    m->m[1][0] = cb * sc;
    m->m[1][1] = cb * cc;
    m->m[1][2] = -sb;
    m->m[2][0] = (-sa * cc) + (ca * sb * sc);
    m->m[2][1] = (sa * sc) + (ca * sb * cc);
    m->m[2][2] = ca * cb;
    m->m[0][3] = tx;
    m->m[1][3] = ty;
    m->m[2][3] = tz;
}

/* func_800A5114 + func_800A56E4: type-2 node including per-axis scale. */
static void node_type2(const f32 *v, TNTMilestoneMatrix *m) {
    f32 sx = sinf(-v[0] * TNT_MILESTONE_DEG2RAD);
    f32 cx = cosf(-v[0] * TNT_MILESTONE_DEG2RAD);
    f32 sy = sinf(-v[1] * TNT_MILESTONE_DEG2RAD);
    f32 cy = cosf(-v[1] * TNT_MILESTONE_DEG2RAD);
    f32 sz = sinf(-v[2] * TNT_MILESTONE_DEG2RAD);
    f32 cz = cosf(-v[2] * TNT_MILESTONE_DEG2RAD);

    mat_identity(m);
    m->m[0][0] = (cz * cx) * v[6];
    m->m[0][1] = ((sz * cy) + (cz * sx * sy)) * v[7];
    m->m[0][2] = ((sz * sy) - (cz * sx * cy)) * v[8];
    m->m[1][0] = (-sz * cx) * v[6];
    m->m[1][1] = ((cz * cy) - (sz * sx * sy)) * v[7];
    m->m[1][2] = ((cz * sy) + (sz * sx * cy)) * v[8];
    m->m[2][0] = sx * v[6];
    m->m[2][1] = (-cx * sy) * v[7];
    m->m[2][2] = (cx * cy) * v[8];
    m->m[0][3] = v[3];
    m->m[1][3] = v[4];
    m->m[2][3] = v[5];
}

static f32 vec_len(f32 x, f32 y, f32 z) { return sqrtf(x*x + y*y + z*z); }
static void normalize3(f32 *x, f32 *y, f32 *z) {
    f32 n = vec_len(*x, *y, *z);
    if (n > 0.000001f) { *x /= n; *y /= n; *z /= n; }
}

/* Camera object 1115 drives the type-4 camera node in scene 1114. */
static void camera_view(u16 elapsed, TNTMilestoneMatrix *view) {
    u16 tick = (u16)(elapsed % 509U);
    f32 ex = g_tnt_milestone_camera_xz[tick][0];
    f32 ey = 7.5f;
    f32 ez = g_tnt_milestone_camera_xz[tick][1];
    const f32 tx = 0.0f, ty = 3.96f, tz = 0.141f;
    f32 fx = ex - tx, fy = ey - ty, fz = ez - tz;
    f32 rx, ry, rz, ux, uy, uz;

    if (fx == 0.0f && fy == 0.0f && fz == 0.0f) fz = -1.0f;
    normalize3(&fx, &fy, &fz);
    rx = fz; ry = 0.0f; rz = -fx;
    normalize3(&rx, &ry, &rz);
    ux = (fy * rz) - (ry * fz);
    uy = (fz * rx) - (rz * fx);
    uz = (fx * ry) - (rx * fy);

    mat_identity(view);
    view->m[0][0] = rx; view->m[0][1] = ry; view->m[0][2] = rz;
    view->m[1][0] = ux; view->m[1][1] = uy; view->m[1][2] = uz;
    view->m[2][0] = fx; view->m[2][1] = fy; view->m[2][2] = fz;
    ex *= 16.0f; ey *= 16.0f; ez *= 16.0f;
    view->m[0][3] = -(rx*ex + ry*ey + rz*ez);
    view->m[1][3] = -(ux*ex + uy*ey + uz*ez);
    view->m[2][3] = -(fx*ex + fy*ey + fz*ez);
}

static void greek_camera_view(u16 elapsed, TNTMilestoneMatrix *view) {
    u16 tick = (u16)(elapsed % 509U);
    f32 ex = g_tnt_greek_camera_xz[tick][0];
    f32 ey = 10.0f;
    f32 ez = g_tnt_greek_camera_xz[tick][1];
    const f32 tx = 0.0f, ty = 8.0f, tz = 0.0f;
    f32 fx = ex - tx, fy = ey - ty, fz = ez - tz;
    f32 rx, ry, rz, ux, uy, uz;
    if (fx == 0.0f && fy == 0.0f && fz == 0.0f) fz = -1.0f;
    normalize3(&fx, &fy, &fz);
    rx = fz; ry = 0.0f; rz = -fx;
    normalize3(&rx, &ry, &rz);
    ux = (fy * rz) - (ry * fz);
    uy = (fz * rx) - (rz * fx);
    uz = (fx * ry) - (rx * fy);
    mat_identity(view);
    view->m[0][0] = rx; view->m[0][1] = ry; view->m[0][2] = rz;
    view->m[1][0] = ux; view->m[1][1] = uy; view->m[1][2] = uz;
    view->m[2][0] = fx; view->m[2][1] = fy; view->m[2][2] = fz;
    ex *= 16.0f; ey *= 16.0f; ez *= 16.0f;
    view->m[0][3] = -(rx*ex + ry*ey + rz*ez);
    view->m[1][3] = -(ux*ex + uy*ey + uz*ez);
    view->m[2][3] = -(fx*ex + fy*ey + fz*ez);
}

static void egyptian_camera_view(u16 elapsed, TNTMilestoneMatrix *view) {
    u16 tick = (u16)(elapsed % 509U);
    f32 ex = g_tnt_greek_camera_xz[tick][0];
    f32 ey = 16.0f;
    f32 ez = g_tnt_greek_camera_xz[tick][1];
    const f32 tx = 0.0f, ty = 9.01200008f, tz = 0.0f;
    f32 fx = ex - tx, fy = ey - ty, fz = ez - tz;
    f32 rx, ry, rz, ux, uy, uz;
    if (fx == 0.0f && fy == 0.0f && fz == 0.0f) fz = -1.0f;
    normalize3(&fx, &fy, &fz);
    rx = fz; ry = 0.0f; rz = -fx;
    normalize3(&rx, &ry, &rz);
    ux = (fy * rz) - (ry * fz);
    uy = (fz * rx) - (rz * fx);
    uz = (fx * ry) - (rx * fy);
    mat_identity(view);
    view->m[0][0] = rx; view->m[0][1] = ry; view->m[0][2] = rz;
    view->m[1][0] = ux; view->m[1][1] = uy; view->m[1][2] = uz;
    view->m[2][0] = fx; view->m[2][1] = fy; view->m[2][2] = fz;
    ex *= 16.0f; ey *= 16.0f; ez *= 16.0f;
    view->m[0][3] = -(rx*ex + ry*ey + rz*ez);
    view->m[1][3] = -(ux*ex + uy*ey + uz*ez);
    view->m[2][3] = -(fx*ex + fy*ey + fz*ez);
}

static void celtic_milestone_camera_view(u16 elapsed, TNTMilestoneMatrix *view) {
    const f32 *c = g_tnt_celtic_milestone_camera[elapsed % 997U];
    f32 ex=c[0],ey=c[1],ez=c[2],tx=c[3],ty=c[4],tz=c[5];
    f32 fx=ex-tx,fy=ey-ty,fz=ez-tz,rx,ry,rz,ux,uy,uz;
    if (fx==0.0f && fy==0.0f && fz==0.0f) fz=-1.0f;
    normalize3(&fx,&fy,&fz); rx=fz;ry=0.0f;rz=-fx; normalize3(&rx,&ry,&rz);
    ux=(fy*rz)-(ry*fz); uy=(fz*rx)-(rz*fx); uz=(fx*ry)-(rx*fy);
    mat_identity(view);
    view->m[0][0]=rx;view->m[0][1]=ry;view->m[0][2]=rz;
    view->m[1][0]=ux;view->m[1][1]=uy;view->m[1][2]=uz;
    view->m[2][0]=fx;view->m[2][1]=fy;view->m[2][2]=fz;
    ex*=16.0f;ey*=16.0f;ez*=16.0f;
    view->m[0][3]=-(rx*ex+ry*ey+rz*ez);
    view->m[1][3]=-(ux*ex+uy*ey+uz*ez);
    view->m[2][3]=-(fx*ex+fy*ey+fz*ez);
}

static void african_milestone_camera_view(u16 elapsed, TNTMilestoneMatrix *view) {
    const f32 *c = g_tnt_african_milestone_camera[elapsed % 997U];
    f32 ex=c[0],ey=c[1],ez=c[2],tx=c[3],ty=c[4],tz=c[5];
    f32 fx=ex-tx,fy=ey-ty,fz=ez-tz,rx,ry,rz,ux,uy,uz;
    if (fx==0.0f && fy==0.0f && fz==0.0f) fz=-1.0f;
    normalize3(&fx,&fy,&fz); rx=fz;ry=0.0f;rz=-fx; normalize3(&rx,&ry,&rz);
    ux=(fy*rz)-(ry*fz); uy=(fz*rx)-(rz*fx); uz=(fx*ry)-(rx*fy);
    mat_identity(view);
    view->m[0][0]=rx;view->m[0][1]=ry;view->m[0][2]=rz;
    view->m[1][0]=ux;view->m[1][1]=uy;view->m[1][2]=uz;
    view->m[2][0]=fx;view->m[2][1]=fy;view->m[2][2]=fz;
    ex*=16.0f;ey*=16.0f;ez*=16.0f;
    view->m[0][3]=-(rx*ex+ry*ey+rz*ez);
    view->m[1][3]=-(ux*ex+uy*ey+uz*ez);
    view->m[2][3]=-(fx*ex+fy*ey+fz*ez);
}

static void russian_milestone_camera_view(u8 stage, u16 elapsed, TNTMilestoneMatrix *view) {
    const f32 *c;
    if (stage > 6U) stage = 6U;
    if (elapsed < 499U) c = g_tnt_russian_milestone_stage_camera[stage][elapsed];
    else c = g_tnt_african_milestone_camera[(elapsed - 499U) % 997U];
    f32 ex=c[0],ey=c[1],ez=c[2],tx=c[3],ty=c[4],tz=c[5];
    f32 fx=ex-tx,fy=ey-ty,fz=ez-tz,rx,ry,rz,ux,uy,uz;
    if (fx==0.0f && fy==0.0f && fz==0.0f) fz=-1.0f;
    normalize3(&fx,&fy,&fz); rx=fz;ry=0.0f;rz=-fx; normalize3(&rx,&ry,&rz);
    ux=(fy*rz)-(ry*fz); uy=(fz*rx)-(rz*fx); uz=(fx*ry)-(rx*fy);
    mat_identity(view);
    view->m[0][0]=rx;view->m[0][1]=ry;view->m[0][2]=rz;
    view->m[1][0]=ux;view->m[1][1]=uy;view->m[1][2]=uz;
    view->m[2][0]=fx;view->m[2][1]=fy;view->m[2][2]=fz;
    ex*=16.0f;ey*=16.0f;ez*=16.0f;
    view->m[0][3]=-(rx*ex+ry*ey+rz*ez);
    view->m[1][3]=-(ux*ex+uy*ey+uz*ez);
    view->m[2][3]=-(fx*ex+fy*ey+fz*ez);
}

static void transform_point(const TNTMilestoneMatrix *m, f32 x, f32 y, f32 z, f32 *ox, f32 *oy, f32 *oz) {
    *ox = m->m[0][0]*x + m->m[0][1]*y + m->m[0][2]*z + m->m[0][3];
    *oy = m->m[1][0]*x + m->m[1][1]*y + m->m[1][2]*z + m->m[1][3];
    *oz = m->m[2][0]*x + m->m[2][1]*y + m->m[2][2]*z + m->m[2][3];
}

static void transform_vector(const TNTMilestoneMatrix *m, f32 x, f32 y, f32 z, f32 *ox, f32 *oy, f32 *oz) {
    *ox = m->m[0][0]*x + m->m[0][1]*y + m->m[0][2]*z;
    *oy = m->m[1][0]*x + m->m[1][1]*y + m->m[1][2]*z;
    *oz = m->m[2][0]*x + m->m[2][1]*y + m->m[2][2]*z;
}

static void prepare_textures(void) {
    s32 t, i;
    if (s_textures_ready) return;
    for (t = 0; t < TNT_MILESTONE_SMALL_TEXTURE_COUNT; t++) {
        for (i = 0; i < 80; i++) {
            u16 p = g_tnt_wonder_mayan_textures[t][i];
            s_small_rgb[t][i][0] = (f32)((p >> 11) & 31) * (255.0f/31.0f);
            s_small_rgb[t][i][1] = (f32)((p >> 6) & 31) * (255.0f/31.0f);
            s_small_rgb[t][i][2] = (f32)((p >> 1) & 31) * (255.0f/31.0f);
        }
    }
    for (t = 0; t < TNT_MILESTONE_LARGE_TEXTURE_COUNT; t++) {
        const u16 *source = t < 15
            ? g_tnt_milestone_large_textures[t]
            : (t < 19
                ? g_tnt_egyptian_milestone_textures[t - 15]
                : (t < 23
                    ? g_tnt_celtic_milestone_textures[t - 19]
                    : g_tnt_african_milestone_textures[t - 23]));
        if (t >= 5 && t <= 12) {
            static const s32 off[4] = {0, 1024, 1280, 1344};
            static const s32 level_size[4] = {32, 16, 8, 4};
            s32 level, x, y;
            for (level = 0; level < 4; level++) {
                s32 n = level_size[level], base = off[level];
                for (y = 0; y < n; y++) for (x = 0; x < n; x++) {
                    s32 sx = x ^ ((y & 1) ? 2 : 0);
                    s32 di = base + y*n + x, si = base + y*n + sx;
                    u16 p = source[si];
                    s_large_rgb[t][di][0] = (f32)((p >> 11) & 31) * (255.0f/31.0f);
                    s_large_rgb[t][di][1] = (f32)((p >> 6) & 31) * (255.0f/31.0f);
                    s_large_rgb[t][di][2] = (f32)((p >> 1) & 31) * (255.0f/31.0f);
                }
            }
        } else {
            for (i = 0; i < 1360; i++) {
                u16 p = source[i];
                s_large_rgb[t][i][0] = (f32)((p >> 11) & 31) * (255.0f/31.0f);
                s_large_rgb[t][i][1] = (f32)((p >> 6) & 31) * (255.0f/31.0f);
                s_large_rgb[t][i][2] = (f32)((p >> 1) & 31) * (255.0f/31.0f);
            }
        }
    }
    s_textures_ready = TRUE;
}

static f32 wrap_coord(f32 v, s32 size) {
    v = fmodf(v, (f32)size);
    if (v < 0.0f) v += (f32)size;
    return v;
}

static inline __attribute__((always_inline)) void sample_level(const f32 (*tex)[3], s32 size, f32 u, f32 v, u8 wrap_s, u8 wrap_t, f32 *r, f32 *g, f32 *b) {
    s32 x0, y0, x1, y1;
    f32 fx, fy;
    const f32 *c00, *c10, *c01, *c11;

    if (wrap_s) u = wrap_coord(u, size);
    else { if (u < 0.0f) u = 0.0f; if (u > size-1) u = (f32)(size-1); }
    if (wrap_t) v = wrap_coord(v, size);
    else { if (v < 0.0f) v = 0.0f; if (v > size-1) v = (f32)(size-1); }

    x0 = (s32)floorf(u); y0 = (s32)floorf(v);
    x1 = x0 + 1; y1 = y0 + 1;
    if (wrap_s) x1 %= size; else if (x1 >= size) x1 = size-1;
    if (wrap_t) y1 %= size; else if (y1 >= size) y1 = size-1;
    fx = u - floorf(u); fy = v - floorf(v);
    c00=tex[y0*size+x0]; c10=tex[y0*size+x1]; c01=tex[y1*size+x0]; c11=tex[y1*size+x1];
    *r=(c00[0]+(c10[0]-c00[0])*fx)*(1.0f-fy)+(c01[0]+(c11[0]-c01[0])*fx)*fy;
    *g=(c00[1]+(c10[1]-c00[1])*fx)*(1.0f-fy)+(c01[1]+(c11[1]-c01[1])*fx)*fy;
    *b=(c00[2]+(c10[2]-c00[2])*fx)*(1.0f-fy)+(c01[2]+(c11[2]-c01[2])*fx)*fy;
}

static void sample_texture(u8 texture, f32 u, f32 v, f32 lod, f32 *r, f32 *g, f32 *b) {
    if (texture < 20) {
        f32 r0,g0,b0,r1,g1,b1;
        const f32 (*src)[3]=s_small_rgb[texture];
        if (lod < 0.0f) lod=0.0f;
        if (lod > 1.0f) lod=1.0f;
        sample_level(src,8,u,v,0,0,&r0,&g0,&b0);
        if (lod <= 0.0f) { *r=r0;*g=g0;*b=b0; return; }
        sample_level(src+64,4,u*0.5f,v*0.5f,0,0,&r1,&g1,&b1);
        *r=r0+(r1-r0)*lod; *g=g0+(g1-g0)*lod; *b=b0+(b1-b0)*lod;
    } else {
        s32 ti=(s32)texture-20;
        s32 l0,l1,size0,size1,off0,off1;
        f32 frac,r0,g0,b0,r1,g1,b1,scale0,scale1;
        u8 flags;
        static const s32 sizes[4]={32,16,8,4};
        static const s32 offs[4]={0,1024,1280,1344};
        if (ti < 0 || ti >= TNT_MILESTONE_LARGE_TEXTURE_COUNT) { *r=*g=*b=255.0f; return; }
        if (lod < 0.0f) lod=0.0f;
        if (lod > 3.0f) lod=3.0f;
        l0=(s32)floorf(lod); l1=l0<3?l0+1:l0; frac=lod-(f32)l0;
        size0=sizes[l0]; size1=sizes[l1]; off0=offs[l0]; off1=offs[l1];
        scale0=1.0f/(f32)(1<<l0); scale1=1.0f/(f32)(1<<l1);
        flags = ti < 15 ? g_tnt_milestone_large_texture_wrap[ti]
            : (ti < 19 ? g_tnt_egyptian_milestone_texture_wrap[ti - 15]
                : (ti < 23 ? g_tnt_celtic_milestone_texture_wrap[ti - 19]
                           : g_tnt_african_milestone_texture_wrap[ti - 23]));
        sample_level(s_large_rgb[ti]+off0,size0,u*scale0,v*scale0,(u8)(flags&2),(u8)(flags&4),&r0,&g0,&b0);
        if (l1==l0) { *r=r0;*g=g0;*b=b0; return; }
        sample_level(s_large_rgb[ti]+off1,size1,u*scale1,v*scale1,(u8)(flags&2),(u8)(flags&4),&r1,&g1,&b1);
        *r=r0+(r1-r0)*frac; *g=g0+(g1-g0)*frac; *b=b0+(b1-b0)*frac;
    }
}

static f32 triangle_lod(const TNTMilestoneProjectedVertex *v, f32 max_lod) {
    f32 dx1=v[1].x-v[0].x, dy1=v[1].y-v[0].y;
    f32 dx2=v[2].x-v[0].x, dy2=v[2].y-v[0].y;
    f32 du1=v[1].u-v[0].u, dv1=v[1].v-v[0].v;
    f32 du2=v[2].u-v[0].u, dv2=v[2].v-v[0].v;
    f32 det=dx1*dy2-dx2*dy1,dudx,dudy,dvdx,dvdy,rho,lod;
    if (fabsf(det)<0.00001f) return 0.0f;
    dudx=(du1*dy2-du2*dy1)/det; dudy=(du2*dx1-du1*dx2)/det;
    dvdx=(dv1*dy2-dv2*dy1)/det; dvdy=(dv2*dx1-dv1*dx2)/det;
    rho=fmaxf(sqrtf(dudx*dudx+dvdx*dvdx),sqrtf(dudy*dudy+dvdy*dvdy));
    if (rho<=1.0f) return 0.0f;
    lod=log2f(rho); if (lod>max_lod) lod=max_lod; return lod;
}

static void make_draw_model(const TNTMilestoneMatrix *world, f32 mesh_scale, TNTMilestoneMatrix *draw) {
    s32 r;
    *draw=*world;
    for (r=0;r<3;r++) {
        draw->m[r][0]*=mesh_scale; draw->m[r][1]*=mesh_scale; draw->m[r][2]*=mesh_scale;
        draw->m[r][3]*=16.0f;
    }
}

static void draw_triangles(const TNTWonderMilestoneTriangle *tris, s32 count,
                           const TNTMilestoneMatrix *draw_models, s32 model_count,
                           const TNTMilestoneMatrix *view, u16 frame_stamp) {
    const f32 focal=1.0f/tanf(20.0f*TNT_MILESTONE_DEG2RAD);
    TNTMilestoneMatrix mv_models[9];
    s32 i,k;

    for (i=0;i<model_count;i++)
        mat_mult(&mv_models[i],view,&draw_models[i]);

    for (i=0;i<count;i++) {
        const TNTWonderMilestoneTriangle *tri=&tris[i];
        const TNTMilestoneMatrix *mv;
        TNTMilestoneProjectedVertex p[3];
        f32 iz[3],area,lod;
        s32 minx,maxx,miny,maxy,y;
        if (tri->node>=model_count) continue;
        mv=&mv_models[tri->node];
        for (k=0;k<3;k++) {
            const TNTWonderVertex *v=&tri->v[k];
            f32 cx,cy,cz,depth,nx,ny,nz,nlen,d0,d1;
            transform_point(mv,(f32)v->x,(f32)v->y,(f32)v->z,&cx,&cy,&cz);
            depth=-cz;
            if (depth<4.0f || depth>8000.0f) goto skip_tri;
            p[k].x=200.0f+(cx*focal/(4.0f/3.0f)/depth)*200.0f;
            p[k].y=150.0f-(cy*focal/depth)*150.0f;
            p[k].z=depth; p[k].u=(f32)v->s/64.0f; p[k].v=(f32)v->t/64.0f; iz[k]=1.0f/depth;
            if (tri->unlit) { p[k].r=p[k].g=p[k].b=1.0f; }
            else {
                transform_vector(&draw_models[tri->node],(f32)v->nx/127.0f,(f32)v->ny/127.0f,(f32)v->nz/127.0f,&nx,&ny,&nz);
                nlen=vec_len(nx,ny,nz); if(nlen>0.000001f){nx/=nlen;ny/=nlen;nz/=nlen;}
                if (s_african_light_mode) {
                    d0=ny;if(d0<0.0f)d0=0.0f;
                    p[k].r=p[k].g=p[k].b=0.300000012f+d0;
                } else {
                    d0=nx*s_light_dir_normalized[0][0]+ny*s_light_dir_normalized[0][1]+nz*s_light_dir_normalized[0][2]; if(d0<0.0f)d0=0.0f;
                    d1=nx*s_light_dir_normalized[1][0]+ny*s_light_dir_normalized[1][1]+nz*s_light_dir_normalized[1][2]; if(d1<0.0f)d1=0.0f;
                    p[k].r=s_ambient_rgb[0]+d0*s_light_rgb[0][0]+d1*s_light_rgb[1][0];
                    p[k].g=s_ambient_rgb[1]+d0*s_light_rgb[0][1]+d1*s_light_rgb[1][1];
                    p[k].b=s_ambient_rgb[2]+d0*s_light_rgb[0][2]+d1*s_light_rgb[1][2];
                }
                if(p[k].r>1.0f)p[k].r=1.0f;
                if(p[k].g>1.0f)p[k].g=1.0f;
                if(p[k].b>1.0f)p[k].b=1.0f;
            }
        }
        area=(p[1].x-p[0].x)*(p[2].y-p[0].y)-(p[1].y-p[0].y)*(p[2].x-p[0].x);
        /* RSP back-face culling is decided before the final Y-down screen map;
         * our raster coordinates are already Y-flipped, so source front faces
         * have negative signed area here. */
        if(area>=0.0f) continue;
        lod=triangle_lod(p,tri->texture<20?1.0f:3.0f);
        minx=(s32)floorf(fminf(p[0].x,fminf(p[1].x,p[2].x))); maxx=(s32)ceilf(fmaxf(p[0].x,fmaxf(p[1].x,p[2].x)));
        miny=(s32)floorf(fminf(p[0].y,fminf(p[1].y,p[2].y))); maxy=(s32)ceilf(fmaxf(p[0].y,fmaxf(p[1].y,p[2].y)));
        if(minx<0)minx=0;
        if(maxx>=400)maxx=399;
        if(miny<0)miny=0;
        if(maxy>=300)maxy=299;
        if(minx>maxx||miny>maxy)continue;
        if(minx<s_frame_minx)s_frame_minx=minx;
        if(miny<s_frame_miny)s_frame_miny=miny;
        if(maxx>s_frame_maxx)s_frame_maxx=maxx;
        if(maxy>s_frame_maxy)s_frame_maxy=maxy;
        {
            f32 ia=1.0f/area;
            f32 dw0dx=(p[1].y-p[2].y)*ia,dw0dy=(p[2].x-p[1].x)*ia;
            f32 dw1dx=(p[2].y-p[0].y)*ia,dw1dy=(p[0].x-p[2].x)*ia;
            f32 dw2dx=-dw0dx-dw1dx,dw2dy=-dw0dy-dw1dy;
            f32 px0=(f32)minx+0.5f,py0=(f32)miny+0.5f;
            f32 w0r=((p[1].x-px0)*(p[2].y-py0)-(p[1].y-py0)*(p[2].x-px0))*ia;
            f32 w1r=((p[2].x-px0)*(p[0].y-py0)-(p[2].y-py0)*(p[0].x-px0))*ia;
            f32 w2r=1.0f-w0r-w1r;
            f32 uq[3]={p[0].u*iz[0],p[1].u*iz[1],p[2].u*iz[2]},vq[3]={p[0].v*iz[0],p[1].v*iz[1],p[2].v*iz[2]};
            f32 rq[3]={0,0,0},gq[3]={0,0,0},bq[3]={0,0,0};
            f32 qr=w0r*iz[0]+w1r*iz[1]+w2r*iz[2],ur=w0r*uq[0]+w1r*uq[1]+w2r*uq[2],vr=w0r*vq[0]+w1r*vq[1]+w2r*vq[2];
            f32 rr=0,gr=0,br=0,drdx=0,drdy=0,dgdx=0,dgdy=0,dbdx=0,dbdy=0;
            f32 dqdx=dw0dx*iz[0]+dw1dx*iz[1]+dw2dx*iz[2],dqdy=dw0dy*iz[0]+dw1dy*iz[1]+dw2dy*iz[2];
            f32 dudx=dw0dx*uq[0]+dw1dx*uq[1]+dw2dx*uq[2],dudy=dw0dy*uq[0]+dw1dy*uq[1]+dw2dy*uq[2];
            f32 dvdx=dw0dx*vq[0]+dw1dx*vq[1]+dw2dx*vq[2],dvdy=dw0dy*vq[0]+dw1dy*vq[1]+dw2dy*vq[2];
            if(!tri->unlit){
                rq[0]=p[0].r*iz[0];rq[1]=p[1].r*iz[1];rq[2]=p[2].r*iz[2];
                gq[0]=p[0].g*iz[0];gq[1]=p[1].g*iz[1];gq[2]=p[2].g*iz[2];
                bq[0]=p[0].b*iz[0];bq[1]=p[1].b*iz[1];bq[2]=p[2].b*iz[2];
                rr=w0r*rq[0]+w1r*rq[1]+w2r*rq[2];gr=w0r*gq[0]+w1r*gq[1]+w2r*gq[2];br=w0r*bq[0]+w1r*bq[1]+w2r*bq[2];
                drdx=dw0dx*rq[0]+dw1dx*rq[1]+dw2dx*rq[2];drdy=dw0dy*rq[0]+dw1dy*rq[1]+dw2dy*rq[2];
                dgdx=dw0dx*gq[0]+dw1dx*gq[1]+dw2dx*gq[2];dgdy=dw0dy*gq[0]+dw1dy*gq[1]+dw2dy*gq[2];
                dbdx=dw0dx*bq[0]+dw1dx*bq[1]+dw2dx*bq[2];dbdy=dw0dy*bq[0]+dw1dy*bq[1]+dw2dy*bq[2];
            }
            for(y=miny;y<=maxy;y++) {
                s32 x; f32 w0=w0r,w1=w1r,w2=w2r,q=qr,uo=ur,vo=vr,ro=rr,go=gr,bo=br;
                for(x=minx;x<=maxx;x++) {
                    s32 pos=y*400+x;
                    if(w0>=0.0f&&w1>=0.0f&&w2>=0.0f&&q>0.0f&&(s_milestone_stamp[pos]!=frame_stamp||q>s_milestone_q[pos])) {
                        f32 invq=1.0f/q,u=uo*invq,v=vo*invq,tr,tg,tb;
                        sample_texture(tri->texture,u,v,lod,&tr,&tg,&tb);
                        if(!tri->unlit){
                            f32 lr=ro*invq,lg=go*invq,lb=bo*invq;
                            tr*=lr;tg*=lg;tb*=lb;
                        }
                        if(tr>255)tr=255;
                        if(tg>255)tg=255;
                        if(tb>255)tb=255;
                        if(tr<0)tr=0;
                        if(tg<0)tg=0;
                        if(tb<0)tb=0;
                        s_milestone_color[pos]=0xff000000U|((uint32_t)tb<<16)|((uint32_t)tg<<8)|(uint32_t)tr;
                        s_milestone_q[pos]=q;s_milestone_stamp[pos]=frame_stamp;
                    }
                    w0+=dw0dx;w1+=dw1dx;w2+=dw2dx;q+=dqdx;uo+=dudx;vo+=dvdx;
                    if(!tri->unlit){ro+=drdx;go+=dgdx;bo+=dbdx;}
                }
                w0r+=dw0dy;w1r+=dw1dy;w2r+=dw2dy;qr+=dqdy;ur+=dudy;vr+=dvdy;
                if(!tri->unlit){rr+=drdy;gr+=dgdy;br+=dbdy;}
            }
        }
skip_tri: ;
    }
}

static void prepare_models_1088(TNTMilestoneMatrix out[4]) {
    TNTMilestoneMatrix wrap,local,world;
    s32 i; wrapper_matrix(0,0,0,0,0,0,&wrap);
    for(i=0;i<4;i++) mat_identity(&out[i]);
    node_type2(g_tnt_milestone_scene1088_nodes[3],&local);mat_mult(&world,&wrap,&local);make_draw_model(&world,0.256172806f,&out[3]);
}

static void prepare_models_1104(TNTMilestoneMatrix out[8], u8 milestone_index) {
    TNTMilestoneMatrix wrap,local,world; f32 vals[9]; s32 i;
    wrapper_matrix(0,0,0,0,0,0,&wrap); mat_identity(&out[0]);
    for(i=1;i<8;i++){
        memcpy(vals,g_tnt_milestone_scene1104_nodes[i],sizeof(vals));
        /* DCM 1105..1110 all target scene 1104. On their forced two-tick
         * initialization, milestone N leaves scene nodes 1..N at Y=0 while
         * later construction nodes remain parked at the authored Y=50. */
        if(i <= (s32)milestone_index) vals[4]=0.0f;
        node_type2(vals,&local);mat_mult(&world,&wrap,&local);make_draw_model(&world,0.125f,&out[i]);
    }
}

static void prepare_models_1125(TNTMilestoneMatrix out[8], u8 greek_stage) {
    TNTMilestoneMatrix wrap,local,world; f32 vals[9]; s32 i;
    wrapper_matrix(0,0,0,0,0,0,&wrap); mat_identity(&out[0]);
    for(i=1;i<8;i++){
        memcpy(vals,g_tnt_greek_scene1125_nodes[i],sizeof(vals));
        if(i <= (s32)greek_stage) vals[4]=0.0f;
        node_type2(vals,&local);mat_mult(&world,&wrap,&local);make_draw_model(&world,0.125f,&out[i]);
    }
}

static void prepare_models_1148(TNTMilestoneMatrix out[8], u8 egyptian_stage) {
    TNTMilestoneMatrix wrap,local,world; f32 vals[9]; s32 i;
    wrapper_matrix(0,0,0,0,0,0,&wrap); mat_identity(&out[0]);
    for(i=1;i<8;i++){
        memcpy(vals,g_tnt_egyptian_scene1148_nodes[i],sizeof(vals));
        if(i <= (s32)egyptian_stage) vals[4]=0.0f;
        node_type2(vals,&local);mat_mult(&world,&wrap,&local);make_draw_model(&world,0.125f,&out[i]);
    }
}

static void prepare_models_1244(TNTMilestoneMatrix out[9], u8 celtic_stage) {
    TNTMilestoneMatrix wrap,local,world; s32 i;
    if(celtic_stage>6)celtic_stage=6;
    wrapper_matrix(0,0,0,0,0,0,&wrap); mat_identity(&out[0]);
    for(i=1;i<9;i++){
        node_type2(g_tnt_celtic_scene1244_stage_nodes[celtic_stage][i],&local);
        mat_mult(&world,&wrap,&local);make_draw_model(&world,0.125f,&out[i]);
    }
}

static void prepare_models_1292(TNTMilestoneMatrix out[8], u8 african_stage) {
    TNTMilestoneMatrix wrap,local,world; s32 i;
    if(african_stage>6)african_stage=6;
    wrapper_matrix(0,0,0,0,0,0,&wrap); mat_identity(&out[0]);
    for(i=1;i<8;i++){
        node_type2(g_tnt_african_scene1292_stage_nodes[african_stage][i],&local);
        mat_mult(&world,&wrap,&local);make_draw_model(&world,0.125f,&out[i]);
    }
}

static void prepare_models_1006(TNTMilestoneMatrix out[9], f32 tx, f32 ty, f32 tz, f32 rx, u16 local_tick) {
    TNTMilestoneMatrix wrap,root,scene_root,local,world; f32 vals[9]; s32 i;
    wrapper_matrix(rx,0,0,tx,ty,tz,&wrap); node_type2(g_tnt_milestone_scene1006_nodes[8],&root); mat_mult(&scene_root,&wrap,&root); mat_identity(&out[0]);mat_identity(&out[8]);
    if(local_tick>190)local_tick=190;
    for(i=1;i<=7;i++){memcpy(vals,g_tnt_milestone_scene1006_nodes[i],sizeof(vals));vals[4]=g_tnt_milestone_block_y[local_tick][i-1];node_type2(vals,&local);mat_mult(&world,&scene_root,&local);make_draw_model(&world,0.125f,&out[i]);}
}

static void prepare_models_1057(TNTMilestoneMatrix out[2], f32 tx, f32 ty, f32 tz, f32 rx) {
    TNTMilestoneMatrix wrap,local,world; wrapper_matrix(rx,0,0,tx,ty,tz,&wrap); mat_identity(&out[0]); node_type2(g_tnt_milestone_scene1057_nodes[1],&local);mat_mult(&world,&wrap,&local);make_draw_model(&world,0.125f,&out[1]);
}

static s32 floor_div_s64(s64 n,s64 d){if(n>=0)return(s32)(n/d);return-(s32)(((-n)+d-1)/d);}
static void prepare_map(s32 board_x,s32 board_y,s32 cell,s32 w,s32 h){s32 i;if(s_map_board_x==board_x&&s_map_board_y==board_y&&s_map_cell==cell&&s_map_width==w&&s_map_height==h)return;for(i=0;i<w&&i<960;i++)s_map_x[i]=(s16)(83+floor_div_s64((s64)(i-board_x)*11,cell));for(i=0;i<h&&i<544;i++)s_map_y[i]=(s16)(36+floor_div_s64((s64)(i-board_y)*11,cell));s_map_board_x=board_x;s_map_board_y=board_y;s_map_cell=cell;s_map_width=w;s_map_height=h;}

void TNT_Vita_WonderMilestoneRenderMayan(void *buffer,s32 stride,s32 screen_width,s32 screen_height,s32 board_x,s32 board_y,s32 cell,u8 milestone_index,u16 elapsed_ticks,const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    TNTMilestoneMatrix view,m1088[4],m1104[8],m1006[9],m1057[2];
    const TNTWonderMilestoneBlock *blocks;
    uint32_t *dst=(uint32_t*)buffer;u16 fs=(u16)(s_milestone_frame_stamp+1U);s32 i,y;u8 block_count;
    if(milestone_index>6) return;
    blocks=g_tnt_mayan_milestone_blocks[milestone_index];
    block_count=g_tnt_mayan_milestone_block_count[milestone_index];
    prepare_textures();prepare_map(board_x,board_y,cell,screen_width,screen_height);if(fs==0){memset(s_milestone_stamp,0,sizeof(s_milestone_stamp));fs=1;}s_milestone_frame_stamp=fs;
    s_frame_minx=400;s_frame_miny=300;s_frame_maxx=-1;s_frame_maxy=-1;
    camera_view(elapsed_ticks,&view);
    prepare_models_1088(m1088);draw_triangles(g_tnt_milestone_scene1088_triangles,126,m1088,4,&view,fs);
    prepare_models_1104(m1104,milestone_index);draw_triangles(g_tnt_milestone_scene1104_triangles,212,m1104,8,&view,fs);
    for(i=0;i<block_count;i++){
        const TNTWonderMilestoneBlock *b=&blocks[i];
        if(elapsed_ticks >= (u16)(b->start_tick+190U)) {prepare_models_1057(m1057,b->x,b->y,b->z,(f32)block_rotation_x[i]);draw_triangles(g_tnt_milestone_scene1057_triangles,10,m1057,2,&view,fs);} else {u16 lt=0;if(elapsed_ticks>=b->start_tick)lt=(u16)(elapsed_ticks-b->start_tick+1U);prepare_models_1006(m1006,b->x,b->y,b->z,(f32)block_rotation_x[i],lt);draw_triangles(g_tnt_milestone_scene1006_triangles,244,m1006,9,&view,fs);}
    }
    if(s_frame_maxx>=s_frame_minx&&s_frame_maxy>=s_frame_miny){
        s32 dx0=board_x+floor_div_s64((s64)(s_frame_minx-83)*cell,11);
        s32 dy0=board_y+floor_div_s64((s64)(s_frame_miny-36)*cell,11);
        s32 dx1=board_x+floor_div_s64((s64)(s_frame_maxx+1-83)*cell+10,11);
        s32 dy1=board_y+floor_div_s64((s64)(s_frame_maxy+1-36)*cell+10,11);
        if(dx0<0)dx0=0;
        if(dy0<0)dy0=0;
        if(dx1>screen_width)dx1=screen_width;
        if(dy1>screen_height)dy1=screen_height;
        for(y=dy0;y<dy1;y++){s32 ny=s_map_y[y],x;if(ny<s_frame_miny||ny>s_frame_maxy)continue;for(x=dx0;x<dx1;x++){s32 nx=s_map_x[x],pos;if(nx<s_frame_minx||nx>s_frame_maxx)continue;pos=ny*400+nx;if(s_milestone_stamp[pos]==fs)dst[y*stride+x]=s_milestone_color[pos];}}
    }
}

void TNT_Vita_WonderMilestoneRenderGreek(void *buffer,s32 stride,s32 screen_width,s32 screen_height,
    s32 board_x,s32 board_y,s32 cell,u8 greek_stage,u16 elapsed_ticks,
    const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    TNTMilestoneMatrix view,m1088[4],m1125[8],m1006[9],m1057[2];
    const TNTWonderMilestoneBlock *blocks;
    uint32_t *dst=(uint32_t*)buffer;u16 fs=(u16)(s_milestone_frame_stamp+1U);s32 i,y;u8 block_count;
    if(greek_stage>6) return;
    blocks=g_tnt_greek_milestone_blocks[greek_stage];
    block_count=g_tnt_greek_milestone_block_count[greek_stage];
    prepare_textures();prepare_map(board_x,board_y,cell,screen_width,screen_height);
    if(fs==0){memset(s_milestone_stamp,0,sizeof(s_milestone_stamp));fs=1;}s_milestone_frame_stamp=fs;
    s_frame_minx=400;s_frame_miny=300;s_frame_maxx=-1;s_frame_maxy=-1;
    greek_camera_view(elapsed_ticks,&view);
    prepare_models_1088(m1088);draw_triangles(g_tnt_milestone_scene1088_triangles,126,m1088,4,&view,fs);
    prepare_models_1125(m1125,greek_stage);draw_triangles(g_tnt_greek_scene1125_triangles,432,m1125,8,&view,fs);
    for(i=0;i<block_count;i++){
        const TNTWonderMilestoneBlock *b=&blocks[i];
        if(elapsed_ticks >= (u16)(b->start_tick+190U)) {
            prepare_models_1057(m1057,b->x,b->y,b->z,(f32)block_rotation_x[i]);
            draw_triangles(g_tnt_milestone_scene1057_triangles,10,m1057,2,&view,fs);
        } else {
            u16 lt=0;if(elapsed_ticks>=b->start_tick)lt=(u16)(elapsed_ticks-b->start_tick+1U);
            prepare_models_1006(m1006,b->x,b->y,b->z,(f32)block_rotation_x[i],lt);
            draw_triangles(g_tnt_milestone_scene1006_triangles,244,m1006,9,&view,fs);
        }
    }
    if(s_frame_maxx>=s_frame_minx&&s_frame_maxy>=s_frame_miny){
        s32 dx0=board_x+floor_div_s64((s64)(s_frame_minx-83)*cell,11);
        s32 dy0=board_y+floor_div_s64((s64)(s_frame_miny-36)*cell,11);
        s32 dx1=board_x+floor_div_s64((s64)(s_frame_maxx+1-83)*cell+10,11);
        s32 dy1=board_y+floor_div_s64((s64)(s_frame_maxy+1-36)*cell+10,11);
        if(dx0<0)dx0=0;
        if(dy0<0)dy0=0;
        if(dx1>screen_width)dx1=screen_width;
        if(dy1>screen_height)dy1=screen_height;
        for(y=dy0;y<dy1;y++){
            s32 ny=s_map_y[y],x;if(ny<s_frame_miny||ny>s_frame_maxy)continue;
            for(x=dx0;x<dx1;x++){
                s32 nx=s_map_x[x],pos;if(nx<s_frame_minx||nx>s_frame_maxx)continue;
                pos=ny*400+nx;if(s_milestone_stamp[pos]==fs)dst[y*stride+x]=s_milestone_color[pos];
            }
        }
    }
}
void TNT_Vita_WonderMilestoneRenderEgyptian(void *buffer,s32 stride,s32 screen_width,s32 screen_height,
    s32 board_x,s32 board_y,s32 cell,u8 egyptian_stage,u16 elapsed_ticks,
    const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    TNTMilestoneMatrix view,m1088[4],m1148[8],m1006[9],m1057[2];
    const TNTWonderMilestoneBlock *blocks;
    uint32_t *dst=(uint32_t*)buffer;u16 fs=(u16)(s_milestone_frame_stamp+1U);s32 i,y;u8 block_count;
    if(egyptian_stage>6) return;
    blocks=g_tnt_egyptian_milestone_blocks[egyptian_stage];
    block_count=g_tnt_egyptian_milestone_block_count[egyptian_stage];
    prepare_textures();prepare_map(board_x,board_y,cell,screen_width,screen_height);
    if(fs==0){memset(s_milestone_stamp,0,sizeof(s_milestone_stamp));fs=1;}s_milestone_frame_stamp=fs;
    s_frame_minx=400;s_frame_miny=300;s_frame_maxx=-1;s_frame_maxy=-1;
    egyptian_camera_view(elapsed_ticks,&view);
    prepare_models_1088(m1088);draw_triangles(g_tnt_milestone_scene1088_triangles,126,m1088,4,&view,fs);
    prepare_models_1148(m1148,egyptian_stage);draw_triangles(g_tnt_egyptian_scene1148_triangles,486,m1148,8,&view,fs);
    for(i=0;i<block_count;i++){
        const TNTWonderMilestoneBlock *b=&blocks[i];
        if(elapsed_ticks >= (u16)(b->start_tick+190U)) {
            prepare_models_1057(m1057,b->x,b->y,b->z,(f32)block_rotation_x[i]);
            draw_triangles(g_tnt_milestone_scene1057_triangles,10,m1057,2,&view,fs);
        } else {
            u16 lt=0;if(elapsed_ticks>=b->start_tick)lt=(u16)(elapsed_ticks-b->start_tick+1U);
            prepare_models_1006(m1006,b->x,b->y,b->z,(f32)block_rotation_x[i],lt);
            draw_triangles(g_tnt_milestone_scene1006_triangles,244,m1006,9,&view,fs);
        }
    }
    if(s_frame_maxx>=s_frame_minx&&s_frame_maxy>=s_frame_miny){
        s32 dx0=board_x+floor_div_s64((s64)(s_frame_minx-83)*cell,11);
        s32 dy0=board_y+floor_div_s64((s64)(s_frame_miny-36)*cell,11);
        s32 dx1=board_x+floor_div_s64((s64)(s_frame_maxx+1-83)*cell+10,11);
        s32 dy1=board_y+floor_div_s64((s64)(s_frame_maxy+1-36)*cell+10,11);
        if(dx0<0)dx0=0;if(dy0<0)dy0=0;if(dx1>screen_width)dx1=screen_width;if(dy1>screen_height)dy1=screen_height;
        for(y=dy0;y<dy1;y++){s32 ny=s_map_y[y],x;if(ny<s_frame_miny||ny>s_frame_maxy)continue;for(x=dx0;x<dx1;x++){s32 nx=s_map_x[x],pos;if(nx<s_frame_minx||nx>s_frame_maxx)continue;pos=ny*400+nx;if(s_milestone_stamp[pos]==fs)dst[y*stride+x]=s_milestone_color[pos];}}
    }
}

void TNT_Vita_WonderMilestoneRenderCeltic(void *buffer,s32 stride,s32 screen_width,s32 screen_height,
    s32 board_x,s32 board_y,s32 cell,u8 celtic_stage,u16 elapsed_ticks,
    const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    TNTMilestoneMatrix view,m1088[4],m1244[9],m1006[9],m1057[2];
    const TNTWonderMilestoneBlock *blocks;
    uint32_t *dst=(uint32_t*)buffer;u16 fs=(u16)(s_milestone_frame_stamp+1U);s32 i,y;u8 block_count;
    if(celtic_stage>6)return;
    blocks=g_tnt_celtic_milestone_blocks[celtic_stage];
    block_count=g_tnt_celtic_milestone_block_count[celtic_stage];
    prepare_textures();prepare_map(board_x,board_y,cell,screen_width,screen_height);
    if(fs==0){memset(s_milestone_stamp,0,sizeof(s_milestone_stamp));fs=1;}s_milestone_frame_stamp=fs;
    s_frame_minx=400;s_frame_miny=300;s_frame_maxx=-1;s_frame_maxy=-1;
    celtic_milestone_camera_view(elapsed_ticks,&view);
    prepare_models_1088(m1088);draw_triangles(g_tnt_milestone_scene1088_triangles,126,m1088,4,&view,fs);
    prepare_models_1244(m1244,celtic_stage);draw_triangles(g_tnt_celtic_scene1244_triangles,530,m1244,9,&view,fs);
    for(i=0;i<block_count;i++){
        const TNTWonderMilestoneBlock *b=&blocks[i];
        if(elapsed_ticks >= (u16)(b->start_tick+190U)){prepare_models_1057(m1057,b->x,b->y,b->z,(f32)block_rotation_x[i]);draw_triangles(g_tnt_milestone_scene1057_triangles,10,m1057,2,&view,fs);}
        else{u16 lt=0;if(elapsed_ticks>=b->start_tick)lt=(u16)(elapsed_ticks-b->start_tick+1U);prepare_models_1006(m1006,b->x,b->y,b->z,(f32)block_rotation_x[i],lt);draw_triangles(g_tnt_milestone_scene1006_triangles,244,m1006,9,&view,fs);}
    }
    if(s_frame_maxx>=s_frame_minx&&s_frame_maxy>=s_frame_miny){
        s32 dx0=board_x+floor_div_s64((s64)(s_frame_minx-83)*cell,11),dy0=board_y+floor_div_s64((s64)(s_frame_miny-36)*cell,11);
        s32 dx1=board_x+floor_div_s64((s64)(s_frame_maxx+1-83)*cell+10,11),dy1=board_y+floor_div_s64((s64)(s_frame_maxy+1-36)*cell+10,11);
        if(dx0<0)dx0=0;if(dy0<0)dy0=0;if(dx1>screen_width)dx1=screen_width;if(dy1>screen_height)dy1=screen_height;
        for(y=dy0;y<dy1;y++){s32 ny=s_map_y[y],x;if(ny<s_frame_miny||ny>s_frame_maxy)continue;for(x=dx0;x<dx1;x++){s32 nx=s_map_x[x],q;if(nx<s_frame_minx||nx>s_frame_maxx)continue;q=ny*400+nx;if(s_milestone_stamp[q]==fs)dst[y*stride+x]=s_milestone_color[q];}}
    }
}

void TNT_Vita_WonderMilestoneRenderAfrican(void *buffer,s32 stride,s32 screen_width,s32 screen_height,
    s32 board_x,s32 board_y,s32 cell,u8 african_stage,u16 elapsed_ticks,
    const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    TNTMilestoneMatrix view,m1088[4],m1292[8],m1006[9],m1057[2];
    const TNTWonderMilestoneBlock *blocks;
    uint32_t *dst=(uint32_t*)buffer;u16 fs=(u16)(s_milestone_frame_stamp+1U);s32 i,y;u8 block_count;
    if(african_stage>6)return;
    blocks=g_tnt_african_milestone_blocks[african_stage];
    block_count=g_tnt_african_milestone_block_count[african_stage];
    prepare_textures();prepare_map(board_x,board_y,cell,screen_width,screen_height);
    if(fs==0){memset(s_milestone_stamp,0,sizeof(s_milestone_stamp));fs=1;}s_milestone_frame_stamp=fs;
    s_frame_minx=400;s_frame_miny=300;s_frame_maxx=-1;s_frame_maxy=-1;
    african_milestone_camera_view(elapsed_ticks,&view);
    prepare_models_1088(m1088);draw_triangles(g_tnt_milestone_scene1088_triangles,126,m1088,4,&view,fs);
    prepare_models_1292(m1292,african_stage);s_african_light_mode=1;draw_triangles(g_tnt_african_scene1292_triangles,1758,m1292,8,&view,fs);s_african_light_mode=0;
    for(i=0;i<block_count;i++){
        const TNTWonderMilestoneBlock *b=&blocks[i];
        if(elapsed_ticks >= (u16)(b->start_tick+190U)){prepare_models_1057(m1057,b->x,b->y,b->z,(f32)block_rotation_x[i]);draw_triangles(g_tnt_milestone_scene1057_triangles,10,m1057,2,&view,fs);}
        else{u16 lt=0;if(elapsed_ticks>=b->start_tick)lt=(u16)(elapsed_ticks-b->start_tick+1U);prepare_models_1006(m1006,b->x,b->y,b->z,(f32)block_rotation_x[i],lt);draw_triangles(g_tnt_milestone_scene1006_triangles,244,m1006,9,&view,fs);}
    }
    if(s_frame_maxx>=s_frame_minx&&s_frame_maxy>=s_frame_miny){
        s32 dx0=board_x+floor_div_s64((s64)(s_frame_minx-83)*cell,11),dy0=board_y+floor_div_s64((s64)(s_frame_miny-36)*cell,11);
        s32 dx1=board_x+floor_div_s64((s64)(s_frame_maxx+1-83)*cell+10,11),dy1=board_y+floor_div_s64((s64)(s_frame_maxy+1-36)*cell+10,11);
        if(dx0<0)dx0=0;if(dy0<0)dy0=0;if(dx1>screen_width)dx1=screen_width;if(dy1>screen_height)dy1=screen_height;
        for(y=dy0;y<dy1;y++){s32 ny=s_map_y[y],x;if(ny<s_frame_miny||ny>s_frame_maxy)continue;for(x=dx0;x<dx1;x++){s32 nx=s_map_x[x],q;if(nx<s_frame_minx||nx>s_frame_maxx)continue;q=ny*400+nx;if(s_milestone_stamp[q]==fs)dst[y*stride+x]=s_milestone_color[q];}}
    }
}
void TNT_Vita_WonderMilestoneRenderRussian(void *buffer,s32 stride,s32 screen_width,s32 screen_height,
    s32 board_x,s32 board_y,s32 cell,u8 russian_stage,u16 elapsed_ticks,
    const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    TNTMilestoneMatrix view,m1088[4],m1292[8],m1006[9],m1057[2];
    const TNTWonderMilestoneBlock *blocks;
    uint32_t *dst=(uint32_t*)buffer;u16 fs=(u16)(s_milestone_frame_stamp+1U);s32 i,y;u8 block_count;
    if(russian_stage>6)return;
    blocks=g_tnt_african_milestone_blocks[russian_stage];
    block_count=g_tnt_african_milestone_block_count[russian_stage];
    prepare_textures();prepare_map(board_x,board_y,cell,screen_width,screen_height);
    if(fs==0){memset(s_milestone_stamp,0,sizeof(s_milestone_stamp));fs=1;}s_milestone_frame_stamp=fs;
    s_frame_minx=400;s_frame_miny=300;s_frame_maxx=-1;s_frame_maxy=-1;
    russian_milestone_camera_view(russian_stage,elapsed_ticks,&view);
    prepare_models_1088(m1088);draw_triangles(g_tnt_milestone_scene1088_triangles,126,m1088,4,&view,fs);
    prepare_models_1292(m1292,russian_stage);s_african_light_mode=1;draw_triangles(g_tnt_african_scene1292_triangles,1758,m1292,8,&view,fs);s_african_light_mode=0;
    for(i=0;i<block_count;i++){
        const TNTWonderMilestoneBlock *b=&blocks[i];
        if(elapsed_ticks >= (u16)(b->start_tick+190U)){prepare_models_1057(m1057,b->x,b->y,b->z,(f32)block_rotation_x[i]);draw_triangles(g_tnt_milestone_scene1057_triangles,10,m1057,2,&view,fs);}
        else{u16 lt=0;if(elapsed_ticks>=b->start_tick)lt=(u16)(elapsed_ticks-b->start_tick+1U);prepare_models_1006(m1006,b->x,b->y,b->z,(f32)block_rotation_x[i],lt);draw_triangles(g_tnt_milestone_scene1006_triangles,244,m1006,9,&view,fs);}
    }
    if(s_frame_maxx>=s_frame_minx&&s_frame_maxy>=s_frame_miny){
        s32 dx0=board_x+floor_div_s64((s64)(s_frame_minx-83)*cell,11),dy0=board_y+floor_div_s64((s64)(s_frame_miny-36)*cell,11);
        s32 dx1=board_x+floor_div_s64((s64)(s_frame_maxx+1-83)*cell+10,11),dy1=board_y+floor_div_s64((s64)(s_frame_maxy+1-36)*cell+10,11);
        if(dx0<0)dx0=0;if(dy0<0)dy0=0;if(dx1>screen_width)dx1=screen_width;if(dy1>screen_height)dy1=screen_height;
        for(y=dy0;y<dy1;y++){s32 ny=s_map_y[y],x;if(ny<s_frame_miny||ny>s_frame_maxy)continue;for(x=dx0;x<dx1;x++){s32 nx=s_map_x[x],q;if(nx<s_frame_minx||nx>s_frame_maxx)continue;q=ny*400+nx;if(s_milestone_stamp[q]==fs)dst[y*stride+x]=s_milestone_color[q];}}
    }
}
void TNT_Vita_WonderMilestoneRenderJapanese(void *buffer,s32 stride,s32 screen_width,s32 screen_height,
    s32 board_x,s32 board_y,s32 cell,u8 japanese_stage,u16 elapsed_ticks,
    const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    /* Source milestones 35..41 are scene 1244 / animations 1245..1251,
     * with common construction camera animation 1278.  This dataset was
     * originally extracted under the provisional Celtic label; expose it by
     * its canonical Japanese Wonder identity while older QA paths remain
     * untouched until their own source realignment pass. */
    TNT_Vita_WonderMilestoneRenderCeltic(
        buffer,stride,screen_width,screen_height,board_x,board_y,cell,
        japanese_stage,elapsed_ticks,block_rotation_x);
}

