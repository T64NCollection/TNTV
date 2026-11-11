#include "tnt_wonder_milestone_gxm.h"
#include "tnt_wonder_milestone_vita.h"
#include "tnt_mayan_skybox_vita.h"
#include "tnt_wonder_complete_vita.h"
#include "tnt_wonder_hall_vita.h"
#include "tntv_rom_bridge.h"
#include "tnt_wonder_complete_greek_vita.h"
#include "tnt_wonder_complete_egyptian_vita.h"
#include "tnt_wonder_complete_celtic_vita.h"
#include "tnt_wonder_complete_african_vita.h"
#include "tnt_wonder_complete_japanese_vita.h"
#include "tnt_wonder_complete_russian_vita.h"
#include "tnt_greek_gameplay_vita.h"
#include "tnt_celtic_gameplay_vita.h"
#include "tnt_african_gameplay_vita.h"
#include "tnt_japanese_gameplay_vita.h"
#include "tnt_russian_gameplay_vita.h"
#include "tnt_intro_vita.h"
#include "tnt_haluci_vita.h"

#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/processmgr.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* TEST103A isolated measurement overlay. This wrapper deliberately keeps
 * the actual sceGxmFinish wait and scene ordering unchanged. The timer
 * records only its wall-time while ordinary 1P background draws. */
extern u8 g_tnt_test103a_measure_gxm;
extern u32 g_tnt_test103a_gpu_wait_us;
static void tnt103a_profiled_gxm_finish(SceGxmContext *context) {
    if (!g_tnt_test103a_measure_gxm) {
        sceGxmFinish(context);
        return;
    }
    u64 start = sceKernelGetProcessTimeWide();
    sceGxmFinish(context);
    g_tnt_test103a_gpu_wait_us +=
        (u32)(sceKernelGetProcessTimeWide() - start);
}

extern u8 g_tnt_mayan_moon_ia8[10206];

#define TNT_GXM_W 960
#define TNT_GXM_H 544
#define TNT_GXM_DEG2RAD (3.14159265358979323846f / 180.0f)
#define TNT_GXM_MILESTONE_TEXTURE_COUNT 49
#define TNT_GXM_SKYBOX_TEXTURE_BASE TNT_GXM_MILESTONE_TEXTURE_COUNT
#define TNT_GXM_SKYBOX_TEXTURE_COUNT 8
#define TNT_GXM_MAYAN_PLAYFIELD_TEXTURE (TNT_GXM_SKYBOX_TEXTURE_BASE + TNT_GXM_SKYBOX_TEXTURE_COUNT)
#define TNT_GXM_MAYAN_BACKGROUND_TEXTURE (TNT_GXM_MAYAN_PLAYFIELD_TEXTURE + 1)
#define TNT_GXM_CREDITS_TEXTURE (TNT_GXM_MAYAN_BACKGROUND_TEXTURE + 1)
#define TNT_GXM_COMPLETE_TEXTURE_BASE (TNT_GXM_CREDITS_TEXTURE + 1)
#define TNT_GXM_COMPLETE_TEXTURE_COUNT 147
#define TNT_GXM_GREEK_SKYBOX_TEXTURE_BASE (TNT_GXM_COMPLETE_TEXTURE_BASE + TNT_GXM_COMPLETE_TEXTURE_COUNT)
#define TNT_GXM_GREEK_SKYBOX_TEXTURE_COUNT 8
#define TNT_GXM_GREEK_RAIN_TEXTURE (TNT_GXM_GREEK_SKYBOX_TEXTURE_BASE + TNT_GXM_GREEK_SKYBOX_TEXTURE_COUNT)
#define TNT_GXM_GREEK_RAIN_ALPHA_TEXTURE (TNT_GXM_GREEK_RAIN_TEXTURE + 1)
#define TNT_GXM_GREEK_BACKGROUND_TEXTURE (TNT_GXM_GREEK_RAIN_ALPHA_TEXTURE + 1)
#define TNT_GXM_GREEK_FIRE_TEXTURE_BASE (TNT_GXM_GREEK_BACKGROUND_TEXTURE + 1)
#define TNT_GXM_GREEK_FIRE_TEXTURE_COUNT 14
#define TNT_GXM_GREEK_NAME_TEXTURE (TNT_GXM_GREEK_FIRE_TEXTURE_BASE + TNT_GXM_GREEK_FIRE_TEXTURE_COUNT)
#define TNT_GXM_MAYAN_CONTINUOUS_SKY_TEXTURE (TNT_GXM_GREEK_NAME_TEXTURE + 1)
#define TNT_GXM_INTRO_TEXTURE_BASE (TNT_GXM_MAYAN_CONTINUOUS_SKY_TEXTURE + 1)
#define TNT_GXM_INTRO_TEXTURE_COUNT 37
#define TNT_GXM_INTRO_FADE_TEXTURE_BASE (TNT_GXM_INTRO_TEXTURE_BASE + TNT_GXM_INTRO_TEXTURE_COUNT)
#define TNT_GXM_INTRO_FADE_TEXTURE_COUNT 5
#define TNT_GXM_GREEK_COMPLETE_TEXTURE_BASE (TNT_GXM_INTRO_FADE_TEXTURE_BASE + TNT_GXM_INTRO_FADE_TEXTURE_COUNT)
#define TNT_GXM_GREEK_COMPLETE_TEXTURE_COUNT TNT_GREEK_COMPLETE_TEXTURE_COUNT
#define TNT_GXM_EGYPTIAN_COMPLETE_TEXTURE_BASE (TNT_GXM_GREEK_COMPLETE_TEXTURE_BASE + TNT_GXM_GREEK_COMPLETE_TEXTURE_COUNT)
#define TNT_GXM_EGYPTIAN_COMPLETE_TEXTURE_COUNT TNT_EGYPTIAN_COMPLETE_TEXTURE_COUNT
#define TNT_GXM_CELTIC_COMPLETE_TEXTURE_BASE (TNT_GXM_EGYPTIAN_COMPLETE_TEXTURE_BASE + TNT_GXM_EGYPTIAN_COMPLETE_TEXTURE_COUNT)
#define TNT_GXM_CELTIC_COMPLETE_TEXTURE_COUNT TNT_CELTIC_COMPLETE_TEXTURE_COUNT
#define TNT_GXM_CELTIC_SKY_TEXTURE_BASE (TNT_GXM_CELTIC_COMPLETE_TEXTURE_BASE + TNT_GXM_CELTIC_COMPLETE_TEXTURE_COUNT)
#define TNT_GXM_CELTIC_SKY_TEXTURE_COUNT TNT_CELTIC_SKY_TEXTURE_COUNT
#define TNT_GXM_CELTIC_BACKGROUND_TEXTURE (TNT_GXM_CELTIC_SKY_TEXTURE_BASE + TNT_GXM_CELTIC_SKY_TEXTURE_COUNT)
#define TNT_GXM_CELTIC_LAMP_TEXTURE_BASE (TNT_GXM_CELTIC_BACKGROUND_TEXTURE + 1)
#define TNT_GXM_CELTIC_LAMP_TEXTURE_COUNT 4
#define TNT_GXM_AFRICAN_SKY_TEXTURE_BASE (TNT_GXM_CELTIC_LAMP_TEXTURE_BASE + TNT_GXM_CELTIC_LAMP_TEXTURE_COUNT)
#define TNT_GXM_AFRICAN_SKY_TEXTURE_COUNT TNT_AFRICAN_SKY_TEXTURE_COUNT
#define TNT_GXM_AFRICAN_BACKGROUND_TEXTURE (TNT_GXM_AFRICAN_SKY_TEXTURE_BASE + TNT_GXM_AFRICAN_SKY_TEXTURE_COUNT)
#define TNT_GXM_AFRICAN_SMOKE_TEXTURE_BASE (TNT_GXM_AFRICAN_BACKGROUND_TEXTURE + 1)
#define TNT_GXM_AFRICAN_SMOKE_TEXTURE_COUNT TNT_AFRICAN_SMOKE_FRAME_COUNT
#define TNT_GXM_AFRICAN_COMPLETE_TEXTURE_BASE (TNT_GXM_AFRICAN_SMOKE_TEXTURE_BASE + TNT_GXM_AFRICAN_SMOKE_TEXTURE_COUNT)
#define TNT_GXM_AFRICAN_COMPLETE_TEXTURE_COUNT TNT_AFRICAN_COMPLETE_TEXTURE_COUNT
#define TNT_GXM_AFRICAN_EFFECT_TEXTURE_BASE (TNT_GXM_AFRICAN_COMPLETE_TEXTURE_BASE + TNT_GXM_AFRICAN_COMPLETE_TEXTURE_COUNT)
#define TNT_GXM_AFRICAN_EFFECT_TEXTURE_COUNT 4
#define TNT_GXM_JAPANESE_SKY_TEXTURE_BASE (TNT_GXM_AFRICAN_EFFECT_TEXTURE_BASE + TNT_GXM_AFRICAN_EFFECT_TEXTURE_COUNT)
#define TNT_GXM_JAPANESE_SKY_TEXTURE_COUNT TNT_JAPANESE_SKY_TEXTURE_COUNT
#define TNT_GXM_JAPANESE_BACKGROUND_TEXTURE (TNT_GXM_JAPANESE_SKY_TEXTURE_BASE + TNT_GXM_JAPANESE_SKY_TEXTURE_COUNT)
#define TNT_GXM_JAPANESE_WATER_TEXTURE_BASE (TNT_GXM_JAPANESE_BACKGROUND_TEXTURE + 1)
#define TNT_GXM_JAPANESE_WATER_TEXTURE_COUNT TNT_JAPANESE_WATER_FRAME_COUNT
#define TNT_GXM_JAPANESE_SPARKLE_TEXTURE_BASE (TNT_GXM_JAPANESE_WATER_TEXTURE_BASE + TNT_GXM_JAPANESE_WATER_TEXTURE_COUNT)
#define TNT_GXM_JAPANESE_SPARKLE_TEXTURE_COUNT 5
#define TNT_GXM_JAPANESE_COMPLETE_TEXTURE_BASE (TNT_GXM_JAPANESE_SPARKLE_TEXTURE_BASE + TNT_GXM_JAPANESE_SPARKLE_TEXTURE_COUNT)
#define TNT_GXM_JAPANESE_COMPLETE_TEXTURE_COUNT TNT_JAPANESE_COMPLETE_TEXTURE_COUNT
#define TNT_GXM_JAPANESE_EFFECT_TEXTURE_BASE (TNT_GXM_JAPANESE_COMPLETE_TEXTURE_BASE + TNT_GXM_JAPANESE_COMPLETE_TEXTURE_COUNT)
#define TNT_GXM_JAPANESE_EFFECT_TEXTURE_COUNT 4
#define TNT_GXM_RUSSIAN_SKY_TEXTURE_BASE (TNT_GXM_JAPANESE_EFFECT_TEXTURE_BASE + TNT_GXM_JAPANESE_EFFECT_TEXTURE_COUNT)
#define TNT_GXM_RUSSIAN_SKY_TEXTURE_COUNT TNT_RUSSIAN_SKY_TEXTURE_COUNT
#define TNT_GXM_RUSSIAN_BACKGROUND_TEXTURE (TNT_GXM_RUSSIAN_SKY_TEXTURE_BASE + TNT_GXM_RUSSIAN_SKY_TEXTURE_COUNT)
#define TNT_GXM_RUSSIAN_CANDLE_TEXTURE_BASE (TNT_GXM_RUSSIAN_BACKGROUND_TEXTURE + 1)
#define TNT_GXM_RUSSIAN_CANDLE_TEXTURE_COUNT TNT_RUSSIAN_CANDLE_FRAME_COUNT
#define TNT_GXM_RUSSIAN_COMPLETE_TEXTURE_BASE (TNT_GXM_RUSSIAN_CANDLE_TEXTURE_BASE + TNT_GXM_RUSSIAN_CANDLE_TEXTURE_COUNT)
#define TNT_GXM_RUSSIAN_COMPLETE_TEXTURE_COUNT TNT_RUSSIAN_COMPLETE_TEXTURE_COUNT
#define TNT_GXM_MAYAN_MOON_TEXTURE (TNT_GXM_RUSSIAN_COMPLETE_TEXTURE_BASE + TNT_GXM_RUSSIAN_COMPLETE_TEXTURE_COUNT)
#define TNT_GXM_FONT_C_TEXTURE (TNT_GXM_MAYAN_MOON_TEXTURE + 1)
#define TNT_GXM_PAUSE_TEXT_TEXTURE (TNT_GXM_FONT_C_TEXTURE + 1)
#define TNT_GXM_STATIC_FRAME_TEXTURE (TNT_GXM_PAUSE_TEXT_TEXTURE + 1)
#define TNT_GXM_FINALE_HALL_TEXTURE (TNT_GXM_STATIC_FRAME_TEXTURE + 1)
#define TNT_GXM_HALL_BACKGROUND_TEXTURE (TNT_GXM_FINALE_HALL_TEXTURE + 1)
#define TNT_GXM_LOCKED_HALL_TEXTURE (TNT_GXM_HALL_BACKGROUND_TEXTURE + 1)
#define TNT_GXM_TEXTURE_COUNT (TNT_GXM_LOCKED_HALL_TEXTURE + 1)
#define TNT_GXM_GREEK_NAME_W 128
#define TNT_GXM_GREEK_NAME_H 16
#define TNT_GXM_OVERLAY_VERTEX_BASE 512
#define TNT_GXM_STORY_VERTEX_BASE 18000
#define TNT_GXM_STORY_VERTEX_CAPACITY (TNT_GXM_MAX_TEX_VERTS - TNT_GXM_STORY_VERTEX_BASE)
#define TNT_GXM_MAX_TEX_VERTS 20000
#define TNT_GXM_MAX_COLOR_VERTS 32000
#define TNT_GXM_MAX_RUNS 512
#define TNT_GXM_PATCHER_BUFFER_SIZE (64 * 1024)
#define TNT_GXM_PATCHER_VERTEX_USSE_SIZE (64 * 1024)
#define TNT_GXM_PATCHER_FRAGMENT_USSE_SIZE (64 * 1024)
#define ALIGN_UP(x,a) (((x)+((a)-1))&~((a)-1))

typedef struct { f32 m[4][4]; } TNTGxmMatrix;
typedef struct { f32 x,y,z,u,v; } TNTGxmTexVertex;
typedef struct { f32 x,y,z; u32 color; } TNTGxmColorVertex;
typedef struct { u16 first; u16 count; u16 texture; u8 pad; u8 pad2; } TNTGxmRun;

typedef struct {
    u8 initialized;
    u8 hall_background_ready;
    u8 static_frame_valid;
    u32 static_frame_generation;
    SceGxmContext *context;
    SceGxmRenderTarget *render_target;
    SceGxmDepthStencilSurface depth_surface;
    SceGxmShaderPatcher *patcher;
    SceGxmShaderPatcherId texture_v_id;
    SceGxmShaderPatcherId texture_f_id;
    SceGxmShaderPatcherId texedge_f_id;
    SceGxmShaderPatcherId color_v_id;
    SceGxmShaderPatcherId color_f_id;
    SceGxmVertexProgram *texture_vp;
    SceGxmFragmentProgram *texture_fp;
    SceGxmFragmentProgram *texture_alpha_fp;
    SceGxmFragmentProgram *texture_feedback_fp;
    SceGxmFragmentProgram *texedge_fp;
    SceGxmFragmentProgram *texedge_alpha_fp;
    SceGxmVertexProgram *color_vp;
    SceGxmFragmentProgram *color_fp;
    SceGxmFragmentProgram *color_alpha_fp;
    SceGxmFragmentProgram *color_multiply_fp;
    SceGxmFragmentProgram *color_nowrite_fp;
    const SceGxmProgramParameter *texture_wvp;
    const SceGxmProgramParameter *color_wvp;

    void *context_host;
    void *vdm_ring;
    void *vertex_ring;
    void *fragment_ring;
    void *fragment_usse_ring;
    void *patcher_buffer;
    void *patcher_vertex_usse;
    void *patcher_fragment_usse;
    void *depth_data;
    TNTGxmTexVertex *tex_vertices;
    TNTGxmColorVertex *color_vertices;
    TNTGxmColorVertex *clear_vertices;
    u16 *indices;
    void *texture_data[TNT_GXM_TEXTURE_COUNT];
    SceGxmTexture textures[TNT_GXM_TEXTURE_COUNT];
    void *complete_texture_pack;
    SceUID complete_texture_uid;
    void *greek_complete_texture_pack;
    SceUID greek_complete_texture_uid;
    void *egyptian_complete_texture_pack;
    SceUID egyptian_complete_texture_uid;
    void *celtic_complete_texture_pack;
    SceUID celtic_complete_texture_uid;
    void *african_complete_texture_pack;
    SceUID african_complete_texture_uid;
    void *japanese_complete_texture_pack;
    SceUID japanese_complete_texture_uid;
    void *russian_complete_texture_pack;
    SceUID russian_complete_texture_uid;

    SceUID vdm_uid;
    SceUID vertex_uid;
    SceUID fragment_uid;
    SceUID fragment_usse_ring_uid;
    SceUID patcher_buffer_uid;
    SceUID patcher_vertex_usse_uid;
    SceUID patcher_fragment_usse_uid;
    SceUID depth_uid;
    SceUID tex_vertices_uid;
    SceUID color_vertices_uid;
    SceUID clear_vertices_uid;
    SceUID indices_uid;
    SceUID texture_uid[TNT_GXM_TEXTURE_COUNT];
    unsigned int fragment_usse_ring_offset;
    unsigned int patcher_vertex_usse_offset;
    unsigned int patcher_fragment_usse_offset;

    TNTGxmRun runs[TNT_GXM_MAX_RUNS];
    u16 tex_count;
    u16 color_count;
    u16 run_count;
    u8 skybox_ready;
} TNTGxmMilestoneState;

static TNTGxmMilestoneState s_gxm;

#define TNT_INTRO_UNTEXTURED_DRAW_COUNT 8
static u16 s_intro_untex_first[TNT_INTRO_UNTEXTURED_DRAW_COUNT];
static u16 s_intro_untex_count[TNT_INTRO_UNTEXTURED_DRAW_COUNT];
static u8 s_intro_untex_alpha[TNT_INTRO_UNTEXTURED_DRAW_COUNT];
static u8 s_intro_untex_runs;
static u16 s_intro_shade_first,s_intro_shade_count;
static u16 s_intro_fog_first,s_intro_fog_count;
static u16 s_complete_untex_count;
static u16 s_celtic_run_shade_first[TNT_GXM_MAX_RUNS];
static u16 s_celtic_run_shade_count[TNT_GXM_MAX_RUNS];
static u16 s_african_run_shade_first[TNT_GXM_MAX_RUNS];
static u16 s_african_run_shade_count[TNT_GXM_MAX_RUNS];
static u16 s_japanese_run_shade_first[TNT_GXM_MAX_RUNS];
static u16 s_japanese_run_shade_count[TNT_GXM_MAX_RUNS];
static u16 s_complete_fog_first,s_complete_fog_count;
/* Consecutive completed-Wonder story frames often use an identical terminal
 * 3D pose. Keep the already-staged geometry until another geometry builder
 * invalidates it, so scrolling text does not rebuild thousands of triangles. */
static u8 s_complete_geometry_cache_valid;
static u8 s_complete_geometry_cache_wonder;
static u8 s_complete_geometry_cache_mode;
static u32 s_complete_geometry_cache_ticks;
static f32 s_complete_fov=66.6679992676f,s_complete_near=2.0f,s_complete_far=32000.0f,s_complete_aspect=1.33333337307f;
static u8 s_african_milestone_light_mode;

extern const SceGxmProgram texture_v_gxp_start;
extern const SceGxmProgram texture_f_gxp_start;
extern const SceGxmProgram tntv_texedge_f_gxp_start;
extern const SceGxmProgram color_v_gxp_start;
extern const SceGxmProgram color_f_gxp_start;

extern u8 g_tnt_mayan_playfield_ia8[102 * 204];
extern u8 g_tnt_mayan_background_ci8[400 * 300];
extern u16 g_tnt_mayan_background_rgba16[256];
extern u8 g_tnt_font_c_ia8[7176];
extern u8 g_tnt_font_a_ia8[19456];
extern u8 g_tnt_font_a_char_widths[46];

/* Exact Font C character map/widths from Font_Init46Char.  Keep a local copy
 * here so completed-Wonder story text can use the same source glyphs directly
 * on GXM instead of software alpha-blending thousands of Vita pixels. */


#define TNT_GXM_FONT_C_ATLAS_W 16U
#define TNT_GXM_FONT_C_ATLAS_H (46U * 12U)
#define TNT_GXM_PAUSE_TEX_W 256U
#define TNT_GXM_PAUSE_TEX_H 64U
#define TNT_GXM_PAUSE_NATIVE_X 72
#define TNT_GXM_PAUSE_NATIVE_Y 108

static char s_greek_name_cache[32];
static u8 s_greek_name_width;

static const f32 s_light_dir[2][3] = {
    {-0.675145930f, 0.170366159f, -0.717741837f},
    { 0.585220326f, 0.264633076f,  0.766476683f}
};
static const f32 s_light_rgb[2][3] = {
    {0.613497f, 0.644172f, 0.687117f},
    {0.785300f, 0.766871f, 0.742331f}
};
static const f32 s_ambient_rgb[3] = {0.251500f, 0.190184f, 0.110429f};

static unsigned int aligned_mem_size(SceKernelMemBlockType type, unsigned int size) {
    if (type == SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW)
        return ALIGN_UP(size, 256 * 1024);
    return ALIGN_UP(size, 4 * 1024);
}

static void *gpu_alloc(SceKernelMemBlockType type, unsigned int size,
                       unsigned int alignment, unsigned int attribs,
                       SceUID *uid) {
    void *mem = NULL;
    unsigned int aligned_size = aligned_mem_size(type, size);
    (void)alignment;
    *uid = sceKernelAllocMemBlock("tnt_milestone_gxm", type, aligned_size, NULL);
    if (*uid < 0)
        return NULL;
    if (sceKernelGetMemBlockBase(*uid, &mem) < 0) {
        sceKernelFreeMemBlock(*uid);
        *uid = -1;
        return NULL;
    }
    if (sceGxmMapMemory(mem, aligned_size, attribs) < 0) {
        sceKernelFreeMemBlock(*uid);
        *uid = -1;
        return NULL;
    }
    return mem;
}

static void *vertex_usse_alloc(unsigned int size, SceUID *uid,
                               unsigned int *usse_offset) {
    void *mem = NULL;
    size = ALIGN_UP(size, 4096);
    *uid = sceKernelAllocMemBlock("tnt_milestone_vusse",
        SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE, size, NULL);
    if (*uid < 0) return NULL;
    if (sceKernelGetMemBlockBase(*uid, &mem) < 0) {
        sceKernelFreeMemBlock(*uid);
        *uid = -1;
        return NULL;
    }
    if (sceGxmMapVertexUsseMemory(mem, size, usse_offset) < 0) {
        sceKernelFreeMemBlock(*uid);
        *uid = -1;
        return NULL;
    }
    return mem;
}

static void *fragment_usse_alloc(unsigned int size, SceUID *uid,
                                 unsigned int *usse_offset) {
    void *mem = NULL;
    size = ALIGN_UP(size, 4096);
    *uid = sceKernelAllocMemBlock("tnt_milestone_fusse",
        SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE, size, NULL);
    if (*uid < 0) return NULL;
    if (sceKernelGetMemBlockBase(*uid, &mem) < 0) {
        sceKernelFreeMemBlock(*uid);
        *uid = -1;
        return NULL;
    }
    if (sceGxmMapFragmentUsseMemory(mem, size, usse_offset) < 0) {
        sceKernelFreeMemBlock(*uid);
        *uid = -1;
        return NULL;
    }
    return mem;
}

static void *patcher_host_alloc(void *user_data, unsigned int size) {
    (void)user_data;
    return malloc(size);
}
static void patcher_host_free(void *user_data, void *mem) {
    (void)user_data;
    free(mem);
}

static void mat_identity(TNTGxmMatrix *m) {
    memset(m, 0, sizeof(*m));
    m->m[0][0] = m->m[1][1] = m->m[2][2] = m->m[3][3] = 1.0f;
}

static void mat_mult(TNTGxmMatrix *out, const TNTGxmMatrix *a,
                     const TNTGxmMatrix *b) {
    TNTGxmMatrix r;
    s32 row,col,k;
    for (row=0; row<4; row++) {
        for (col=0; col<4; col++) {
            f32 v=0.0f;
            for (k=0; k<4; k++) v += a->m[row][k] * b->m[k][col];
            r.m[row][col]=v;
        }
    }
    *out=r;
}

static void wrapper_matrix(f32 rx,f32 ry,f32 rz,f32 tx,f32 ty,f32 tz,
                           TNTGxmMatrix *m) {
    f32 a=rx*TNT_GXM_DEG2RAD,b=ry*TNT_GXM_DEG2RAD,c=rz*TNT_GXM_DEG2RAD;
    f32 ca=cosf(a),cb=cosf(b),cc=cosf(c),sa=sinf(a),sb=sinf(b),sc=sinf(c);
    mat_identity(m);
    m->m[0][0]=(ca*cc)+(sa*sb*sc);
    m->m[0][1]=(-ca*sc)+(sa*sb*cc);
    m->m[0][2]=sa*cb;
    m->m[1][0]=cb*sc;
    m->m[1][1]=cb*cc;
    m->m[1][2]=-sb;
    m->m[2][0]=(-sa*cc)+(ca*sb*sc);
    m->m[2][1]=(sa*sc)+(ca*sb*cc);
    m->m[2][2]=ca*cb;
    m->m[0][3]=tx;m->m[1][3]=ty;m->m[2][3]=tz;
}

static void node_type2(const f32 *v, TNTGxmMatrix *m) {
    f32 sx=sinf(-v[0]*TNT_GXM_DEG2RAD),cx=cosf(-v[0]*TNT_GXM_DEG2RAD);
    f32 sy=sinf(-v[1]*TNT_GXM_DEG2RAD),cy=cosf(-v[1]*TNT_GXM_DEG2RAD);
    f32 sz=sinf(-v[2]*TNT_GXM_DEG2RAD),cz=cosf(-v[2]*TNT_GXM_DEG2RAD);
    mat_identity(m);
    m->m[0][0]=(cz*cx)*v[6];
    m->m[0][1]=((sz*cy)+(cz*sx*sy))*v[7];
    m->m[0][2]=((sz*sy)-(cz*sx*cy))*v[8];
    m->m[1][0]=(-sz*cx)*v[6];
    m->m[1][1]=((cz*cy)-(sz*sx*sy))*v[7];
    m->m[1][2]=((cz*sy)+(sz*sx*cy))*v[8];
    m->m[2][0]=sx*v[6];
    m->m[2][1]=(-cx*sy)*v[7];
    m->m[2][2]=(cx*cy)*v[8];
    m->m[0][3]=v[3];m->m[1][3]=v[4];m->m[2][3]=v[5];
}

static f32 vec_len(f32 x,f32 y,f32 z) { return sqrtf(x*x+y*y+z*z); }
static void normalize3(f32 *x,f32 *y,f32 *z) {
    f32 n=vec_len(*x,*y,*z);
    if (n>0.000001f) { *x/=n;*y/=n;*z/=n; }
}

static void camera_view(u16 elapsed,TNTGxmMatrix *view) {
    u16 tick=(u16)(elapsed%509U);
    f32 ex=g_tnt_milestone_camera_xz[tick][0],ey=7.5f,ez=g_tnt_milestone_camera_xz[tick][1];
    const f32 tx=0.0f,ty=3.96f,tz=0.141f;
    f32 fx=ex-tx,fy=ey-ty,fz=ez-tz,rx,ry,rz,ux,uy,uz;
    if(fx==0.0f&&fy==0.0f&&fz==0.0f)fz=-1.0f;
    normalize3(&fx,&fy,&fz);rx=fz;ry=0.0f;rz=-fx;normalize3(&rx,&ry,&rz);
    ux=(fy*rz)-(ry*fz);uy=(fz*rx)-(rz*fx);uz=(fx*ry)-(rx*fy);
    mat_identity(view);
    view->m[0][0]=rx;view->m[0][1]=ry;view->m[0][2]=rz;
    view->m[1][0]=ux;view->m[1][1]=uy;view->m[1][2]=uz;
    view->m[2][0]=fx;view->m[2][1]=fy;view->m[2][2]=fz;
    ex*=16.0f;ey*=16.0f;ez*=16.0f;
    view->m[0][3]=-(rx*ex+ry*ey+rz*ez);
    view->m[1][3]=-(ux*ex+uy*ey+uz*ez);
    view->m[2][3]=-(fx*ex+fy*ey+fz*ez);
}

static void greek_camera_view(u16 elapsed,TNTGxmMatrix *view) {
    u16 tick=(u16)(elapsed%509U);
    f32 ex=g_tnt_greek_camera_xz[tick][0],ey=10.0f,ez=g_tnt_greek_camera_xz[tick][1];
    const f32 tx=0.0f,ty=8.0f,tz=0.0f;
    f32 fx=ex-tx,fy=ey-ty,fz=ez-tz,rx,ry,rz,ux,uy,uz;
    if(fx==0.0f&&fy==0.0f&&fz==0.0f)fz=-1.0f;
    normalize3(&fx,&fy,&fz);rx=fz;ry=0.0f;rz=-fx;normalize3(&rx,&ry,&rz);
    ux=(fy*rz)-(ry*fz);uy=(fz*rx)-(rz*fx);uz=(fx*ry)-(rx*fy);
    mat_identity(view);
    view->m[0][0]=rx;view->m[0][1]=ry;view->m[0][2]=rz;
    view->m[1][0]=ux;view->m[1][1]=uy;view->m[1][2]=uz;
    view->m[2][0]=fx;view->m[2][1]=fy;view->m[2][2]=fz;
    ex*=16.0f;ey*=16.0f;ez*=16.0f;
    view->m[0][3]=-(rx*ex+ry*ey+rz*ez);
    view->m[1][3]=-(ux*ex+uy*ey+uz*ez);
    view->m[2][3]=-(fx*ex+fy*ey+fz*ez);
}

static void egyptian_camera_view(u16 elapsed,TNTGxmMatrix *view) {
    u16 tick=(u16)(elapsed%509U);
    f32 ex=g_tnt_greek_camera_xz[tick][0],ey=16.0f,ez=g_tnt_greek_camera_xz[tick][1];
    const f32 tx=0.0f,ty=9.01200008f,tz=0.0f;
    f32 fx=ex-tx,fy=ey-ty,fz=ez-tz,rx,ry,rz,ux,uy,uz;
    if(fx==0.0f&&fy==0.0f&&fz==0.0f)fz=-1.0f;
    normalize3(&fx,&fy,&fz);rx=fz;ry=0.0f;rz=-fx;normalize3(&rx,&ry,&rz);
    ux=(fy*rz)-(ry*fz);uy=(fz*rx)-(rz*fx);uz=(fx*ry)-(rx*fy);
    mat_identity(view);
    view->m[0][0]=rx;view->m[0][1]=ry;view->m[0][2]=rz;
    view->m[1][0]=ux;view->m[1][1]=uy;view->m[1][2]=uz;
    view->m[2][0]=fx;view->m[2][1]=fy;view->m[2][2]=fz;
    ex*=16.0f;ey*=16.0f;ez*=16.0f;
    view->m[0][3]=-(rx*ex+ry*ey+rz*ez);
    view->m[1][3]=-(ux*ex+uy*ey+uz*ez);
    view->m[2][3]=-(fx*ex+fy*ey+fz*ez);
}

static void celtic_milestone_camera_view(u16 elapsed,TNTGxmMatrix *view) {
    const f32 *c=g_tnt_celtic_milestone_camera[elapsed%997U];
    f32 ex=c[0],ey=c[1],ez=c[2],tx=c[3],ty=c[4],tz=c[5];
    f32 fx=ex-tx,fy=ey-ty,fz=ez-tz,rx,ry,rz,ux,uy,uz;
    if(fx==0.0f&&fy==0.0f&&fz==0.0f)fz=-1.0f;
    normalize3(&fx,&fy,&fz);rx=fz;ry=0.0f;rz=-fx;normalize3(&rx,&ry,&rz);
    ux=(fy*rz)-(ry*fz);uy=(fz*rx)-(rz*fx);uz=(fx*ry)-(rx*fy);
    mat_identity(view);
    view->m[0][0]=rx;view->m[0][1]=ry;view->m[0][2]=rz;
    view->m[1][0]=ux;view->m[1][1]=uy;view->m[1][2]=uz;
    view->m[2][0]=fx;view->m[2][1]=fy;view->m[2][2]=fz;
    ex*=16.0f;ey*=16.0f;ez*=16.0f;
    view->m[0][3]=-(rx*ex+ry*ey+rz*ez);view->m[1][3]=-(ux*ex+uy*ey+uz*ez);view->m[2][3]=-(fx*ex+fy*ey+fz*ez);
}

static void african_milestone_camera_view(u16 elapsed,TNTGxmMatrix *view) {
    const f32 *c=g_tnt_african_milestone_camera[elapsed%997U];
    f32 ex=c[0],ey=c[1],ez=c[2],tx=c[3],ty=c[4],tz=c[5];
    f32 fx=ex-tx,fy=ey-ty,fz=ez-tz,rx,ry,rz,ux,uy,uz;
    if(fx==0.0f&&fy==0.0f&&fz==0.0f)fz=-1.0f;
    normalize3(&fx,&fy,&fz);rx=fz;ry=0.0f;rz=-fx;normalize3(&rx,&ry,&rz);
    ux=(fy*rz)-(ry*fz);uy=(fz*rx)-(rz*fx);uz=(fx*ry)-(rx*fy);
    mat_identity(view);
    view->m[0][0]=rx;view->m[0][1]=ry;view->m[0][2]=rz;
    view->m[1][0]=ux;view->m[1][1]=uy;view->m[1][2]=uz;
    view->m[2][0]=fx;view->m[2][1]=fy;view->m[2][2]=fz;
    ex*=16.0f;ey*=16.0f;ez*=16.0f;
    view->m[0][3]=-(rx*ex+ry*ey+rz*ez);view->m[1][3]=-(ux*ex+uy*ey+uz*ez);view->m[2][3]=-(fx*ex+fy*ey+fz*ez);
}

static void russian_milestone_camera_view(u8 stage,u16 elapsed,TNTGxmMatrix *view) {
    const f32 *c;
    if(stage>6U)stage=6U;
    if(elapsed<499U)c=g_tnt_russian_milestone_stage_camera[stage][elapsed];
    else c=g_tnt_african_milestone_camera[(elapsed-499U)%997U];
    f32 ex=c[0],ey=c[1],ez=c[2],tx=c[3],ty=c[4],tz=c[5];
    f32 fx=ex-tx,fy=ey-ty,fz=ez-tz,rx,ry,rz,ux,uy,uz;
    if(fx==0.0f&&fy==0.0f&&fz==0.0f)fz=-1.0f;
    normalize3(&fx,&fy,&fz);rx=fz;ry=0.0f;rz=-fx;normalize3(&rx,&ry,&rz);
    ux=(fy*rz)-(ry*fz);uy=(fz*rx)-(rz*fx);uz=(fx*ry)-(rx*fy);
    mat_identity(view);
    view->m[0][0]=rx;view->m[0][1]=ry;view->m[0][2]=rz;
    view->m[1][0]=ux;view->m[1][1]=uy;view->m[1][2]=uz;
    view->m[2][0]=fx;view->m[2][1]=fy;view->m[2][2]=fz;
    ex*=16.0f;ey*=16.0f;ez*=16.0f;
    view->m[0][3]=-(rx*ex+ry*ey+rz*ez);view->m[1][3]=-(ux*ex+uy*ey+uz*ez);view->m[2][3]=-(fx*ex+fy*ey+fz*ez);
}

static void transform_point(const TNTGxmMatrix *m,f32 x,f32 y,f32 z,
                            f32 *ox,f32 *oy,f32 *oz) {
    *ox=m->m[0][0]*x+m->m[0][1]*y+m->m[0][2]*z+m->m[0][3];
    *oy=m->m[1][0]*x+m->m[1][1]*y+m->m[1][2]*z+m->m[1][3];
    *oz=m->m[2][0]*x+m->m[2][1]*y+m->m[2][2]*z+m->m[2][3];
}
static void transform_vector(const TNTGxmMatrix *m,f32 x,f32 y,f32 z,
                             f32 *ox,f32 *oy,f32 *oz) {
    *ox=m->m[0][0]*x+m->m[0][1]*y+m->m[0][2]*z;
    *oy=m->m[1][0]*x+m->m[1][1]*y+m->m[1][2]*z;
    *oz=m->m[2][0]*x+m->m[2][1]*y+m->m[2][2]*z;
}

static void make_draw_model(const TNTGxmMatrix *world,f32 mesh_scale,TNTGxmMatrix *draw) {
    s32 r;*draw=*world;
    for(r=0;r<3;r++){
        draw->m[r][0]*=mesh_scale;draw->m[r][1]*=mesh_scale;draw->m[r][2]*=mesh_scale;
        draw->m[r][3]*=16.0f;
    }
}

static void prepare_models_1088(TNTGxmMatrix out[4]) {
    TNTGxmMatrix wrap,local,world;s32 i;wrapper_matrix(0,0,0,0,0,0,&wrap);
    for(i=0;i<4;i++)mat_identity(&out[i]);
    node_type2(g_tnt_milestone_scene1088_nodes[3],&local);mat_mult(&world,&wrap,&local);
    make_draw_model(&world,0.256172806f,&out[3]);
}
static void prepare_models_1104(TNTGxmMatrix out[8],u8 milestone_index) {
    TNTGxmMatrix wrap,local,world;f32 vals[9];s32 i;wrapper_matrix(0,0,0,0,0,0,&wrap);mat_identity(&out[0]);
    for(i=1;i<8;i++){memcpy(vals,g_tnt_milestone_scene1104_nodes[i],sizeof(vals));if(i<=(s32)milestone_index)vals[4]=0.0f;node_type2(vals,&local);mat_mult(&world,&wrap,&local);make_draw_model(&world,0.125f,&out[i]);}
}
static void prepare_models_1125(TNTGxmMatrix out[8],u8 greek_stage) {
    TNTGxmMatrix wrap,local,world;f32 vals[9];s32 i;
    wrapper_matrix(0,0,0,0,0,0,&wrap);mat_identity(&out[0]);
    for(i=1;i<8;i++){
        memcpy(vals,g_tnt_greek_scene1125_nodes[i],sizeof(vals));
        /* DCM 1126..1131 force two updates on entry. Greek stage N leaves
         * scene nodes 1..N at Y=0 and later sections parked at Y=50. */
        if(i<=(s32)greek_stage)vals[4]=0.0f;
        node_type2(vals,&local);mat_mult(&world,&wrap,&local);make_draw_model(&world,0.125f,&out[i]);
    }
}

static void prepare_models_1148(TNTGxmMatrix out[8],u8 egyptian_stage) {
    TNTGxmMatrix wrap,local,world;f32 vals[9];s32 i;
    wrapper_matrix(0,0,0,0,0,0,&wrap);mat_identity(&out[0]);
    for(i=1;i<8;i++){
        memcpy(vals,g_tnt_egyptian_scene1148_nodes[i],sizeof(vals));
        if(i<=(s32)egyptian_stage)vals[4]=0.0f;
        node_type2(vals,&local);mat_mult(&world,&wrap,&local);make_draw_model(&world,0.125f,&out[i]);
    }
}

static void prepare_models_1244(TNTGxmMatrix out[9],u8 celtic_stage) {
    TNTGxmMatrix wrap,local,world;s32 i;if(celtic_stage>6)celtic_stage=6;
    wrapper_matrix(0,0,0,0,0,0,&wrap);mat_identity(&out[0]);
    for(i=1;i<9;i++){node_type2(g_tnt_celtic_scene1244_stage_nodes[celtic_stage][i],&local);mat_mult(&world,&wrap,&local);make_draw_model(&world,0.125f,&out[i]);}
}

static void prepare_models_1292(TNTGxmMatrix out[8],u8 african_stage) {
    TNTGxmMatrix wrap,local,world;s32 i;if(african_stage>6)african_stage=6;
    wrapper_matrix(0,0,0,0,0,0,&wrap);mat_identity(&out[0]);
    for(i=1;i<8;i++){node_type2(g_tnt_african_scene1292_stage_nodes[african_stage][i],&local);mat_mult(&world,&wrap,&local);make_draw_model(&world,0.125f,&out[i]);}
}

static void prepare_models_1006(TNTGxmMatrix out[9],f32 tx,f32 ty,f32 tz,f32 rx,u16 local_tick) {
    TNTGxmMatrix wrap,root,scene_root,local,world;f32 vals[9];s32 i;
    wrapper_matrix(rx,0,0,tx,ty,tz,&wrap);node_type2(g_tnt_milestone_scene1006_nodes[8],&root);
    mat_mult(&scene_root,&wrap,&root);mat_identity(&out[0]);mat_identity(&out[8]);
    if(local_tick>190)local_tick=190;
    for(i=1;i<=7;i++){memcpy(vals,g_tnt_milestone_scene1006_nodes[i],sizeof(vals));vals[4]=g_tnt_milestone_block_y[local_tick][i-1];node_type2(vals,&local);mat_mult(&world,&scene_root,&local);make_draw_model(&world,0.125f,&out[i]);}
}
static void prepare_models_1057(TNTGxmMatrix out[2],f32 tx,f32 ty,f32 tz,f32 rx) {
    TNTGxmMatrix wrap,local,world;wrapper_matrix(rx,0,0,tx,ty,tz,&wrap);mat_identity(&out[0]);
    node_type2(g_tnt_milestone_scene1057_nodes[1],&local);mat_mult(&world,&wrap,&local);make_draw_model(&world,0.125f,&out[1]);
}

static u32 rgba16_to_rgba8(u16 p) {
    u32 r=(p>>11)&31U,g=(p>>6)&31U,b=(p>>1)&31U,a=(p&1U)?255U:0U;
    r=(r<<3)|(r>>2);g=(g<<3)|(g>>2);b=(b<<3)|(b>>2);
    return r|(g<<8)|(b<<16)|(a<<24);
}

static u32 rgba16_to_borg_lit_rgba8(u16 p,u8 light,u8 alpha) {
    u32 r=(p>>11)&31U,g=(p>>6)&31U,b=(p>>1)&31U;
    u32 a=(p&1U)?alpha:0U;
    r=(r<<3)|(r>>2);g=(g<<3)|(g>>2);b=(b<<3)|(b>>2);
    r=(r*(u32)light+127U)/255U;
    g=(g*(u32)light+127U)/255U;
    b=(b*(u32)light+127U)/255U;
    return r|(g<<8)|(b<<16)|(a<<24);
}

static u32 rgba16_to_sky_rgba8(u16 p) {
    /* Mayan scene 201: light resource 200 is 0.5 gray, which the N64 path
     * quantizes to 127/255. Its materials carry transparency 0.2 (alpha 204).
     * Keep those scene-specific values out of the reusable Borg decoder. */
    return rgba16_to_borg_lit_rgba8(p,127U,204U);
}

static int init_milestone_textures(void) {
    s32 t,i,x,y;
    for(t=0;t<TNT_GXM_MILESTONE_TEXTURE_COUNT;t++){
        const u8 small = (u8)(t < 20);
        const s32 size = small ? 8 : 32;
        const s32 mip_count = small ? 2 : 4;
        const s32 texel_count = small ? (64 + 8 * 4) : (1024 + 256 + 64 + 8 * 4);
        u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
            (unsigned int)(texel_count*4),SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[t]);
        if(!dst)return -1;
        memset(dst,0,(size_t)texel_count*4U);
        s_gxm.texture_data[t]=dst;
        if(small){
            for(i=0;i<64;i++)dst[i]=rgba16_to_rgba8(g_tnt_wonder_mayan_textures[t][i]);
            for(y=0;y<4;y++)for(x=0;x<4;x++)
                dst[64+y*8+x]=rgba16_to_rgba8(g_tnt_wonder_mayan_textures[t][64+y*4+x]);
        }else{
            const u16 *src=(t<35)
                ? g_tnt_milestone_large_textures[t-20]
                : (t<39 ? g_tnt_egyptian_milestone_textures[t-35]
                    : (t<43 ? g_tnt_celtic_milestone_textures[t-39]
                            : g_tnt_african_milestone_textures[t-43]));
            if(t>=25&&t<=32){
                /* Scene 1088's eight construction-sky sectors use the same
                 * RGBA16/DXT=0 TMEM layout as the Mayan gameplay cloud dome.
                 * Undo the RDP odd-row 32-bit-half permutation before GXM
                 * sampling; textures 33/34 are the ground and stay untouched.
                 *
                 * The narrow upper-cap transition is authored into scene1088:
                 * canonical cap/side texels match on the shared ring, but the
                 * planar cap remaps the cloud pattern above it. Preserve that
                 * source geometry rather than hiding it with a Vita-only warp. */
                static const s32 off[4]={0,1024,1280,1344};
                static const s32 level_size[4]={32,16,8,4};
                s32 level;
                for(level=0;level<3;level++){
                    s32 n=level_size[level],base=off[level];
                    for(y=0;y<n;y++)for(x=0;x<n;x++){
                        s32 sx=x^((y&1)?2:0);
                        dst[base+y*n+x]=rgba16_to_rgba8(src[base+y*n+sx]);
                    }
                }
                for(y=0;y<4;y++)for(x=0;x<4;x++){
                    s32 sx=x^((y&1)?2:0);
                    dst[1344+y*8+x]=rgba16_to_rgba8(src[1344+y*4+sx]);
                }
            }else{
                for(i=0;i<1024+256+64;i++)dst[i]=rgba16_to_rgba8(src[i]);
                for(y=0;y<4;y++)for(x=0;x<4;x++)dst[1344+y*8+x]=rgba16_to_rgba8(src[1344+y*4+x]);
            }
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[t],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
            (unsigned)size,(unsigned)size,(unsigned)mip_count)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[t],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[t],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[t],SCE_GXM_TEXTURE_MIP_FILTER_ENABLED);
        sceGxmTextureSetMipmapCount(&s_gxm.textures[t],(unsigned)mip_count);
        if(t>=25&&t<=32)
            sceGxmTextureSetLodBias(&s_gxm.textures[t],31);
        if(small){
            sceGxmTextureSetUAddrMode(&s_gxm.textures[t],SCE_GXM_TEXTURE_ADDR_CLAMP);
            sceGxmTextureSetVAddrMode(&s_gxm.textures[t],SCE_GXM_TEXTURE_ADDR_CLAMP);
        }else{
            u8 flags=(t<35)
                ? g_tnt_milestone_large_texture_wrap[t-20]
                : (t<39 ? g_tnt_egyptian_milestone_texture_wrap[t-35]
                    : (t<43 ? g_tnt_celtic_milestone_texture_wrap[t-39]
                            : g_tnt_african_milestone_texture_wrap[t-43]));
            sceGxmTextureSetUAddrMode(&s_gxm.textures[t],(flags&2)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
            sceGxmTextureSetVAddrMode(&s_gxm.textures[t],(flags&4)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        }
    }
    return 0;
}

static __attribute__((optimize("Os"))) u32 n64_three_point_sky_sample(const u16 *src,s32 size,f32 s,f32 t) {
    s32 x0=(s32)floorf(s),y0=(s32)floorf(t),x1=x0+1,y1=y0+1,c;
    f32 fx=s-(f32)x0,fy=t-(f32)y0;
    u32 p00,p10,p01,p11,out=0;
    if(x0<0)x0=0;
    if(y0<0)y0=0;
    if(x1<0)x1=0;
    if(y1<0)y1=0;
    if(x0>=size)x0=size-1;
    if(y0>=size)y0=size-1;
    if(x1>=size)x1=size-1;
    if(y1>=size)y1=size-1;
    p00=rgba16_to_sky_rgba8(src[y0*size+x0]);p10=rgba16_to_sky_rgba8(src[y0*size+x1]);
    p01=rgba16_to_sky_rgba8(src[y1*size+x0]);p11=rgba16_to_sky_rgba8(src[y1*size+x1]);
    for(c=0;c<4;c++){
        f32 a=(f32)((p00>>(c*8))&255U),b=(f32)((p10>>(c*8))&255U);
        f32 d=(f32)((p01>>(c*8))&255U),e=(f32)((p11>>(c*8))&255U),v;
        if(fx+fy<=1.0f)v=a+fx*(b-a)+fy*(d-a);
        else v=e+(1.0f-fx)*(d-e)+(1.0f-fy)*(b-e);
        if(v<0.0f)v=0.0f;
        if(v>255.0f)v=255.0f;
        out|=((u32)(v+0.5f))<<(c*8);
    }
    return out;
}

/* Native continuous Mayan cloud panorama.
 *
 * Scene 201 is geometrically a 16-sector cylinder with two vertical bands.
 * The original N64 renderer binds one 32x32 texture per sector; those source
 * tiles contain real RGB disagreement at their borders.  GXM then exposes
 * those authored discontinuities as a moving grid.  Rather than modifying
 * each tile independently, reconstruct one 512x64 periodic panorama here.
 * Every source tile remains unchanged away from a narrow join neighborhood;
 * neighboring tiles are cross-faded in one continuous image, and four-way
 * intersections are resolved by the same normalized 2D blend instead of by
 * stacking independent edge corrections.
 *
 * This is intentionally a Vita-native presentation fix.  It preserves the
 * original cloud artwork and animation speed while removing the concept of
 * an internal texture boundary from the sky dome itself. */
static f32 tnt_mayan_smooth01(f32 t) {
    if(t<0.0f)t=0.0f;
    if(t>1.0f)t=1.0f;
    return t*t*(3.0f-2.0f*t);
}

static u32 tnt_mayan_lerp_rgba(u32 a,u32 b,f32 t) {
    u32 out=0; s32 c;
    for(c=0;c<4;c++){
        f32 av=(f32)((a>>(c*8))&255U),bv=(f32)((b>>(c*8))&255U);
        u32 v=(u32)(av+(bv-av)*t+0.5f);
        if(v>255U)v=255U;
        out|=v<<(c*8);
    }
    return out;
}

static u32 tnt_mayan_sample_base_tile(u8 tex,f32 s,f32 t) {
    u32 *src=(u32*)s_gxm.texture_data[TNT_GXM_SKYBOX_TEXTURE_BASE+tex];
    s32 x0,y0,x1,y1,c; f32 fx,fy; u32 out=0;
    if(s<0.0f)s=0.0f;
    if(s>31.0f)s=31.0f;
    if(t<0.0f)t=0.0f;
    if(t>31.0f)t=31.0f;
    x0=(s32)floorf(s);y0=(s32)floorf(t);x1=x0<31?x0+1:31;y1=y0<31?y0+1:31;
    fx=s-(f32)x0;fy=t-(f32)y0;
    for(c=0;c<4;c++){
        f32 p00=(f32)((src[y0*32+x0]>>(c*8))&255U);
        f32 p10=(f32)((src[y0*32+x1]>>(c*8))&255U);
        f32 p01=(f32)((src[y1*32+x0]>>(c*8))&255U);
        f32 p11=(f32)((src[y1*32+x1]>>(c*8))&255U);
        f32 a=p00+(p10-p00)*fx,b=p01+(p11-p01)*fx;
        u32 v=(u32)(a+(b-a)*fy+0.5f);if(v>255U)v=255U;out|=v<<(c*8);
    }
    return out;
}

static u32 tnt_mayan_panorama_hsample(u8 band,f32 x,f32 t) {
    static const u8 tile[2][16]={
        {2,6,4,0,2,6,4,0,2,6,4,0,2,6,4,0},
        {7,3,5,1,7,3,5,1,7,3,5,1,7,3,5,1}
    };
    const f32 h=4.0f;
    s32 sec=(s32)floorf(x/32.0f);f32 local=x-(f32)sec*32.0f;
    sec&=15;
    if(local<h){
        s32 left=(sec+15)&15;f32 q=local;
        f32 wr=0.5f+0.5f*tnt_mayan_smooth01(q/h);
        u32 a=tnt_mayan_sample_base_tile(tile[band][left],31.0f-q,t);
        u32 b=tnt_mayan_sample_base_tile(tile[band][sec],q,t);
        return tnt_mayan_lerp_rgba(a,b,wr);
    }
    if(local>32.0f-h){
        s32 right=(sec+1)&15;f32 q=32.0f-local;
        f32 wr=0.5f-0.5f*tnt_mayan_smooth01(q/h);
        u32 a=tnt_mayan_sample_base_tile(tile[band][sec],31.0f-q,t);
        u32 b=tnt_mayan_sample_base_tile(tile[band][right],q,t);
        return tnt_mayan_lerp_rgba(a,b,wr);
    }
    return tnt_mayan_sample_base_tile(tile[band][sec],local-0.5f,t);
}

static int init_mayan_continuous_sky_texture(void) {
    const s32 index=TNT_GXM_MAYAN_CONTINUOUS_SKY_TEXTURE;
    const f32 h=4.0f;
    u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,512*64*4,
        SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
        &s_gxm.texture_uid[index]);
    s32 x,y;
    if(!dst)return -1;
    s_gxm.texture_data[index]=dst;
    for(y=0;y<64;y++){
        f32 yc=(f32)y+0.5f;
        for(x=0;x<512;x++){
            f32 xc=(f32)x+0.5f;
            if(yc<32.0f-h){
                dst[y*512+x]=tnt_mayan_panorama_hsample(0,xc,(f32)y);
            }else if(yc>32.0f+h){
                dst[y*512+x]=tnt_mayan_panorama_hsample(1,xc,(f32)(y-32));
            }else{
                f32 d=yc-32.0f,q=fabsf(d);
                f32 wt=(d>=0.0f)?(0.5f+0.5f*tnt_mayan_smooth01(q/h)):
                                     (0.5f-0.5f*tnt_mayan_smooth01(q/h));
                u32 a=tnt_mayan_panorama_hsample(0,xc,31.0f-q);
                u32 b=tnt_mayan_panorama_hsample(1,xc,q);
                dst[y*512+x]=tnt_mayan_lerp_rgba(a,b,wt);
            }
        }
    }
    if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,512,64,0)<0)return -1;
    if(sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR)<0)return -1;
    if(sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR)<0)return -1;
    if(sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED)<0)return -1;
    if(sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_REPEAT)<0)return -1;
    if(sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP)<0)return -1;
    return sceGxmTextureValidate(&s_gxm.textures[index])<0?-1:0;
}

static __attribute__((optimize("Os"))) int init_skybox_textures(void) {
    s32 t,i,x,y;
    const s32 texel_count=1024+256+64+32;
    for(t=0;t<TNT_GXM_SKYBOX_TEXTURE_COUNT;t++){
        const s32 index=TNT_GXM_SKYBOX_TEXTURE_BASE+t;
        const u16 *src=g_tnt_mayan_skybox_textures[t];
        u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
            (unsigned int)(texel_count*4),SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        if(!dst)return -1;
        memset(dst,0,(size_t)texel_count*4U);
        s_gxm.texture_data[index]=dst;
        /* The Borg uses a DXT=0 LoadBlock.  RGBA16 TMEM fetches still apply
         * the RDP odd-row 32-bit-half permutation: logical texel x on odd T
         * addresses source x^2 within each four-texel/64-bit group.  Convert
         * the authored TMEM payload into the logical image the RDP samples
         * before handing it to GXM. */
        {
            static const s32 off[4]={0,1024,1280,1344};
            static const s32 size[4]={32,16,8,4};
            s32 level;
            for(level=0;level<3;level++){
                s32 n=size[level],base=off[level];
                for(y=0;y<n;y++)for(x=0;x<n;x++){
                    s32 sx=x^((y&1)?2:0);
                    dst[base+y*n+x]=rgba16_to_sky_rgba8(src[base+y*n+sx]);
                }
            }
            for(y=0;y<4;y++)for(x=0;x<4;x++){
                s32 sx=x^((y&1)?2:0);
                dst[1344+y*8+x]=rgba16_to_sky_rgba8(src[1344+y*4+sx]);
            }
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,32,32,4)<0)return -1;
        if(sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR)<0)return -1;
        if(sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR)<0)return -1;
        if(sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_ENABLED)<0)return -1;
        if(sceGxmTextureSetMipmapCount(&s_gxm.textures[index],4)<0)return -1;
        if(sceGxmTextureSetLodBias(&s_gxm.textures[index],31)<0)return -1;
        if(sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP)<0)return -1;
        if(sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP)<0)return -1;
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    if(init_mayan_continuous_sky_texture()<0)return -1;
    {
        const s32 index=TNT_GXM_MAYAN_PLAYFIELD_TEXTURE;
        u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,102*204*4,SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        if(!dst)return -1;
        s_gxm.texture_data[index]=dst;
        for(i=0;i<102*204;i++){
            u8 ia=g_tnt_mayan_playfield_ia8[i];u32 v=(u32)((ia>>4)*17U);
            u32 a=(u32)((ia&15U)*17U);a=(a*0x64U+127U)/255U;
            dst[i]=v|(v<<8)|(v<<16)|(a<<24);
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,102,204,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    {
        const s32 index=TNT_GXM_MAYAN_BACKGROUND_TEXTURE;
        u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,400*300*4,SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        if(!dst)return -1;
        s_gxm.texture_data[index]=dst;
        for(i=0;i<400*300;i++)dst[i]=rgba16_to_rgba8(g_tnt_mayan_background_rgba16[g_tnt_mayan_background_ci8[i]]);
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,400,300,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    {
        const s32 index=TNT_GXM_MAYAN_MOON_TEXTURE;
        /* SCE_GXM_TEXTURE_LINEAR rounds each scanline up to an 8-pixel
         * stride.  The moon is 126 pixels wide, so upload it with a 128-pixel
         * physical row stride while keeping the logical texture 126x81.
         * A tightly-packed 126-wide upload shears every row by two pixels. */
        const s32 moon_w=126,moon_h=81,moon_stride=128;
        u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,moon_stride*moon_h*4,SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        s32 x,y;
        if(!dst)return -1;
        s_gxm.texture_data[index]=dst;
        memset(dst,0,(size_t)moon_stride*moon_h*4U);
        for(y=0;y<moon_h;y++)for(x=0;x<moon_w;x++){
            const u8 ia=g_tnt_mayan_moon_ia8[y*moon_w+x];
            const u32 v=(u32)((ia>>4)*17U);
            const u32 a=(u32)((ia&15U)*17U);
            dst[y*moon_stride+x]=v|(v<<8)|(v<<16)|(a<<24);
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,moon_w,moon_h,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    {
        const s32 index=TNT_GXM_CREDITS_TEXTURE;
        u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,400*300*4,SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        if(!dst)return -1;
        s_gxm.texture_data[index]=dst;
        memset(dst,0,400*300*4);
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,400,300,0)<0)return -1;
        /* Linear magnification lets the native 400x300 credits move through
         * the Vita's non-integer 20/11 presentation scale without CPU-side
         * per-output-pixel filtering. */
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    return 0;
}

static int init_borg_rgba16_mip_texture(s32 index,const u16 *src,u8 light,u8 alpha) {
    static const s32 off[4]={0,1024,1280,1344};
    static const s32 size[4]={32,16,8,4};
    const s32 texel_count=1024+256+64+32;
    u32 *dst; s32 x,y,level;
    dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
        (unsigned int)(texel_count*4),SCE_GXM_TEXTURE_ALIGNMENT,
        SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
    if(!dst)return -1;
    memset(dst,0,(size_t)texel_count*4U);s_gxm.texture_data[index]=dst;
    for(level=0;level<3;level++){
        s32 n=size[level],base=off[level];
        for(y=0;y<n;y++)for(x=0;x<n;x++){
            s32 sx=x^((y&1)?2:0);
            dst[base+y*n+x]=rgba16_to_borg_lit_rgba8(src[base+y*n+sx],light,alpha);
        }
    }
    for(y=0;y<4;y++)for(x=0;x<4;x++){
        s32 sx=x^((y&1)?2:0);
        dst[1344+y*8+x]=rgba16_to_borg_lit_rgba8(src[1344+y*4+sx],light,alpha);
    }
    if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,32,32,4)<0)return -1;
    sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
    sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
    sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_ENABLED);
    sceGxmTextureSetMipmapCount(&s_gxm.textures[index],4);
    sceGxmTextureSetLodBias(&s_gxm.textures[index],31);
    sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
    sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
    return sceGxmTextureValidate(&s_gxm.textures[index])<0?-1:0;
}

static int init_borg_rgba16_base_texture(s32 index,const u16 *src,u8 light,u8 alpha) {
    u32 *dst; s32 x,y;
    dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,32U*32U*4U,
        SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
        &s_gxm.texture_uid[index]);
    if(!dst)return -1;s_gxm.texture_data[index]=dst;
    for(y=0;y<32;y++)for(x=0;x<32;x++){
        s32 sx=x^((y&1)?2:0);
        dst[y*32+x]=rgba16_to_borg_lit_rgba8(src[y*32+sx],light,alpha);
    }
    if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,32,32,0)<0)return -1;
    sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
    sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
    sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
    sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
    sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
    return sceGxmTextureValidate(&s_gxm.textures[index])<0?-1:0;
}

/* Test209D: scene1425 32x32 trophy texture is rebuilt from the user's ROM
 * during asset bootstrap; map exactly those numeric N64 texels in row order.
 * Unlike a Borg mipmapped texture, this 1024-texel pack is already linear. */
static int init_finale_hall_texture(void) {
    const s32 index=TNT_GXM_FINALE_HALL_TEXTURE;
    u32 *dst;
    s32 i;
    dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,32U*32U*4U,
        SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
        &s_gxm.texture_uid[index]);
    if(!dst)return -1;
    s_gxm.texture_data[index]=dst;
    for(i=0;i<1024;i++){
        const u16 p=g_tnt_wonder_finale_hall_texture[i];
        const u32 r=(((u32)(p>>11)&31U)*255U+15U)/31U;
        const u32 g=(((u32)(p>>6)&31U)*255U+15U)/31U;
        const u32 bl=(((u32)(p>>1)&31U)*255U+15U)/31U;
        dst[i]=0xff000000U|(bl<<16)|(g<<8)|r;
    }
    if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,
        SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,32,32,0)<0)return -1;
    sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
    sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
    sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
    sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
    sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
    return sceGxmTextureValidate(&s_gxm.textures[index])<0?-1:0;
}

/* Test212D: exactly the user's scene-1071 32x32 gold question-mark
 * texture, no game data added to the distributable program. */
static int init_locked_hall_texture(void) {
    const s32 index=TNT_GXM_LOCKED_HALL_TEXTURE;
    u32 *dst;
    s32 i;
    dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
        32U*32U*4U,SCE_GXM_TEXTURE_ALIGNMENT,
        SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
        &s_gxm.texture_uid[index]);
    if(!dst)return -1;
    s_gxm.texture_data[index]=dst;
    for(i=0;i<1024;i++){
        const u16 pixel=g_tnt_wonder_locked_texture[i];
        const u32 r=((u32)((pixel>>11)&31U)*255U+15U)/31U;
        const u32 g=((u32)((pixel>>6)&31U)*255U+15U)/31U;
        const u32 bl=((u32)((pixel>>1)&31U)*255U+15U)/31U;
        dst[i]=0xff000000U|(bl<<16)|(g<<8)|r;
    }
    if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,
        SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,32,32,0)<0)return -1;
    sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
    sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
    sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
    sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
    sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
    return sceGxmTextureValidate(&s_gxm.textures[index])<0?-1:0;
}

static int init_font_c_texture(void) {
    const s32 index=TNT_GXM_FONT_C_TEXTURE;
    const u32 width=TNT_GXM_FONT_C_ATLAS_W,height=TNT_GXM_FONT_C_ATLAS_H;
    u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,width*height*4U,
        SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
        &s_gxm.texture_uid[index]);
    u32 glyph,x,y;
    if(!dst)return -1;
    s_gxm.texture_data[index]=dst;
    memset(dst,0,(size_t)width*height*4U);
    for(glyph=0;glyph<46U;glyph++)for(y=0;y<12U;y++)for(x=0;x<13U;x++){
        u8 ia=g_tnt_font_c_ia8[glyph*(13U*12U)+y*13U+x];
        u32 intensity=(u32)((ia>>4)&0x0fU)*17U;
        u32 alpha=(u32)(ia&0x0fU)*17U;
        dst[(glyph*12U+y)*width+x]=intensity|(intensity<<8)|(intensity<<16)|(alpha<<24);
    }
    if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
        width,height,0)<0)return -1;
    sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
    sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
    sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
    sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
    sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
    return sceGxmTextureValidate(&s_gxm.textures[index])<0?-1:0;
}

static int init_pause_text_texture(void) {
    const s32 index=TNT_GXM_PAUSE_TEXT_TEXTURE;
    const u32 width=TNT_GXM_PAUSE_TEX_W,height=TNT_GXM_PAUSE_TEX_H;
    u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,width*height*4U,
        SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
        &s_gxm.texture_uid[index]);
    if(!dst)return -1;
    s_gxm.texture_data[index]=dst;
    memset(dst,0,(size_t)width*height*4U);
    if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
        width,height,0)<0)return -1;
    sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
    sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
    sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
    sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
    sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
    return sceGxmTextureValidate(&s_gxm.textures[index])<0?-1:0;
}

static int font_a_text_width_gxm(const char *text) {
    int width=0;
    while(text&&*text){
        unsigned c=(unsigned char)*text++;
        if(c>=32U&&c<96U)width+=g_tnt_font_a_char_widths[g_tnt_font_c_map[c-32U]];
    }
    return width;
}

static void update_pause_text_texture(u8 selected,f32 pulse) {
    static const char *labels[3]={"CONTINUE","PROGRESS","EXIT"};
    static const s32 centers_y[3]={130,150,170};
    u32 *dst=(u32*)s_gxm.texture_data[TNT_GXM_PAUSE_TEXT_TEXTURE];
    s32 i;
    if(!dst)return;
    memset(dst,0,TNT_GXM_PAUSE_TEX_W*TNT_GXM_PAUSE_TEX_H*4U);
    for(i=0;i<3;i++){
        const char *text=labels[i];
        u8 red=255U,global_alpha=255U;
        s32 pen_x=200-(font_a_text_width_gxm(text)>>1);
        s32 pen_y=centers_y[i]-22;
        if(i==(s32)selected){
            f32 q=pulse*255.0f;
            if(q<0.0f)q=0.0f;if(q>255.0f)q=255.0f;
            red=(u8)q;global_alpha=red;
        }
        while(*text){
            unsigned c=(unsigned char)*text++;u8 glyph,glyph_w;s32 gx,gy;
            if(c<32U||c>=96U)continue;
            glyph=g_tnt_font_c_map[c-32U];glyph_w=g_tnt_font_a_char_widths[glyph];
            for(gy=0;gy<22;gy++)for(gx=0;gx<(s32)glyph_w;gx++){
                s32 dx=pen_x+gx-TNT_GXM_PAUSE_NATIVE_X;
                s32 dy=pen_y+gy-TNT_GXM_PAUSE_NATIVE_Y;
                u8 ia;u32 intensity,srca,r,a;
                if(dx<0||dy<0||dx>=(s32)TNT_GXM_PAUSE_TEX_W||dy>=(s32)TNT_GXM_PAUSE_TEX_H)continue;
                ia=g_tnt_font_a_ia8[(s32)glyph*(19*22)+gy*19+gx];
                if((ia&0x0fU)==0U)continue;
                intensity=(u32)((ia>>4)&0x0fU)*17U;
                srca=(u32)(ia&0x0fU)*17U;
                r=((u32)red*intensity+127U)/255U;
                a=(srca*(u32)global_alpha+127U)/255U;
                dst[(u32)dy*TNT_GXM_PAUSE_TEX_W+(u32)dx]=r|(intensity<<8)|(intensity<<16)|(a<<24);
            }
            pen_x+=glyph_w;
        }
    }
}

static int init_greek_gameplay_textures(void) {
    s32 t;
    /* Scene 1399 shares Mayan 201's RGBA16/DXT=0 TMEM layout, but not its
     * material state. Greek light 1398 is 0.8 gray and its eight sky materials
     * are opaque. Keep the proven odd-row decode/+0.5 sampling path while
     * applying the Greek source values instead of Mayan-specific lighting. */
    for(t=0;t<TNT_GXM_GREEK_SKYBOX_TEXTURE_COUNT;t++)
        if(init_borg_rgba16_mip_texture(TNT_GXM_GREEK_SKYBOX_TEXTURE_BASE+t,
            g_tnt_greek_skybox_textures[t],204U,255U)<0)return -1;
    /* Scene 1318 has exactly one scene light. Borg's one-light path uses that
     * resource as ambient and ALSO installs its built-in full-white +Y
     * directional light (D_800D6208). The old Vita path baked only the 0.21
     * ambient term into the texture, turning the source's pale/white rain
     * nearly black. The extracted rain vertices carry the source -1 normal
     * sentinel, so there is no faithful per-vertex normal available here;
     * preserve the authored RGBA16 rain color instead of the incorrect
     * ambient-only darkening. Mesh 1315 still gets its animated alpha below. */
    if(init_borg_rgba16_mip_texture(TNT_GXM_GREEK_RAIN_TEXTURE,
        g_tnt_greek_rain_texture,255U,255U)<0)return -1;
    if(init_borg_rgba16_mip_texture(TNT_GXM_GREEK_RAIN_ALPHA_TEXTURE,
        g_tnt_greek_rain_texture,255U,255U)<0)return -1;
    {
        const s32 index=TNT_GXM_GREEK_BACKGROUND_TEXTURE;
        u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,400*300*4,SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        s32 i;if(!dst)return -1;s_gxm.texture_data[index]=dst;
        for(i=0;i<400*300;i++)dst[i]=rgba16_to_rgba8(g_tnt_greek_background_rgba16[g_tnt_greek_background_ci8[i]]);
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,400,300,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    for(t=0;t<TNT_GXM_GREEK_FIRE_TEXTURE_COUNT;t++){
        const s32 index=TNT_GXM_GREEK_FIRE_TEXTURE_BASE+t;
        const u8 *src=g_tnt_greek_fire_ci8[t];const u16 *pal=g_tnt_greek_fire_rgba16[t];
        const s32 stride_pixels=104; /* SCE_GXM_TEXTURE_LINEAR aligns width to 8 pixels. */
        u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
            stride_pixels*71*4,SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        s32 x,y;if(!dst)return -1;s_gxm.texture_data[index]=dst;
        memset(dst,0,stride_pixels*71*4);
        for(y=0;y<71;y++)for(x=0;x<103;x++){
            u16 p=pal[src[y*103+x]];u32 q=rgba16_to_rgba8(p);
            /* The N64 keeps RGB even when the RGBA16 palette alpha bit is 0.
             * G_TF_AVERAGE filters those channels before alpha blending, so
             * preserve hidden RGB instead of turning transparent texels black. */
            dst[y*stride_pixels+x]=(q&0x00ffffffU)|((p&1U)?(0x7fU<<24):0U);
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,103,71,0)<0)return -1;
        /* func_8005BBFC selects G_TF_AVERAGE for these CI8 hardware sprites.
         * GXM has no exact RDP box-filter mode; linear sampling is the nearest
         * hardware analogue and, unlike POINT, preserves the authored soft edge. */
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    {
        const s32 index=TNT_GXM_GREEK_NAME_TEXTURE;
        u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
            TNT_GXM_GREEK_NAME_W*TNT_GXM_GREEK_NAME_H*4,SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        if(!dst)return -1;s_gxm.texture_data[index]=dst;
        memset(dst,0,TNT_GXM_GREEK_NAME_W*TNT_GXM_GREEK_NAME_H*4);
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
            TNT_GXM_GREEK_NAME_W,TNT_GXM_GREEK_NAME_H,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    s_greek_name_cache[0]='\0';s_greek_name_width=0;
    return 0;
}

static void celtic_decode_packbits(const u8 *src,u32 src_size,u8 *dst,u32 dst_size) {
    u32 si=0,di=0;
    while(si<src_size&&di<dst_size){u8 ctl=src[si++];
        if(ctl<=127U){u32 n=(u32)ctl+1U;while(n--&&si<src_size&&di<dst_size)dst[di++]=src[si++];}
        else if(ctl!=128U&&si<src_size){u32 n=257U-(u32)ctl;u8 v=src[si++];while(n--&&di<dst_size)dst[di++]=v;}
    }
}

static int init_celtic_gameplay_textures(void) {
    static const u8 sky_alpha[3]={255U,255U,190U};
    s32 t;
    for(t=0;t<TNT_GXM_CELTIC_SKY_TEXTURE_COUNT;t++){
        s32 index=TNT_GXM_CELTIC_SKY_TEXTURE_BASE+t;
        if(init_borg_rgba16_mip_texture(index,g_tnt_celtic_sky_textures[t],255U,sky_alpha[t])<0)return -1;
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_REPEAT);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],t==2?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    {
        const s32 index=TNT_GXM_CELTIC_BACKGROUND_TEXTURE;
        u8 *ci=(u8*)malloc(400U*300U);u32 *dst;s32 i;
        if(!ci)return -1;
        celtic_decode_packbits(g_tnt_celtic_background_packbits,g_tnt_celtic_background_packbits_size,ci,400U*300U);
        dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,400*300*4,SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        if(!dst){free(ci);return -1;}s_gxm.texture_data[index]=dst;
        for(i=0;i<400*300;i++)dst[i]=rgba16_to_rgba8(g_tnt_celtic_background_rgba16[ci[i]]);
        free(ci);
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,400,300,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    for(t=0;t<TNT_GXM_CELTIC_LAMP_TEXTURE_COUNT;t++){
        const s32 index=TNT_GXM_CELTIC_LAMP_TEXTURE_BASE+t;
        const u8 *src=g_tnt_celtic_lamp_ci8[t];const u16 *pal=g_tnt_celtic_lamp_rgba16[t];
        const s32 stride_pixels=64;u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
            stride_pixels*60*4,SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        s32 x,y;if(!dst)return -1;s_gxm.texture_data[index]=dst;memset(dst,0,stride_pixels*60*4);
        for(y=0;y<60;y++)for(x=0;x<61;x++)dst[y*stride_pixels+x]=rgba16_to_rgba8(pal[src[y*61+x]]);
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,61,60,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    return 0;
}

static int init_african_gameplay_textures(void) {
    s32 t;
    /* Scene 567 has two deliberately different Borg materials. With exactly
     * one scene light, func_800A7EFC installs light 566 as ambient #2 (0.3
     * gray) and the built-in D_800D6208 full-white +Y directional light as
     * light #1. Mesh 564's normal (0,125,19) faces that directional light
     * almost exactly, so ambient + diffuse clamps to full shade. Baking only
     * the 0.3 ambient term made the authored warm horizon nearly black. Mesh
     * 563 uses flags 0x5c (lighting disabled) and transparency 0.478788, i.e.
     * alpha 133/255. */
    if(init_borg_rgba16_mip_texture(TNT_GXM_AFRICAN_SKY_TEXTURE_BASE+0,
        g_tnt_african_sky_textures[0],255U,255U)<0)return -1;
    if(init_borg_rgba16_mip_texture(TNT_GXM_AFRICAN_SKY_TEXTURE_BASE+1,
        g_tnt_african_sky_textures[1],255U,133U)<0)return -1;
    sceGxmTextureSetUAddrMode(&s_gxm.textures[TNT_GXM_AFRICAN_SKY_TEXTURE_BASE+1],SCE_GXM_TEXTURE_ADDR_REPEAT);
    sceGxmTextureSetVAddrMode(&s_gxm.textures[TNT_GXM_AFRICAN_SKY_TEXTURE_BASE+1],SCE_GXM_TEXTURE_ADDR_REPEAT);
    if(sceGxmTextureValidate(&s_gxm.textures[TNT_GXM_AFRICAN_SKY_TEXTURE_BASE+1])<0)return -1;
    {
        const s32 index=TNT_GXM_AFRICAN_BACKGROUND_TEXTURE;
        u8 *ci=(u8*)malloc(400U*300U);u32 *dst;s32 i;
        if(!ci)return -1;
        celtic_decode_packbits(g_tnt_african_background_packbits,g_tnt_african_background_packbits_size,ci,400U*300U);
        dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,400*300*4,SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        if(!dst){free(ci);return -1;}s_gxm.texture_data[index]=dst;
        for(i=0;i<400*300;i++)dst[i]=rgba16_to_rgba8(g_tnt_african_background_rgba16[ci[i]]);
        free(ci);
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,400,300,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    for(t=0;t<TNT_GXM_AFRICAN_SMOKE_TEXTURE_COUNT;t++){
        const s32 index=TNT_GXM_AFRICAN_SMOKE_TEXTURE_BASE+t;
        const u8 *src=g_tnt_african_smoke_ci8[t];const u16 *pal=g_tnt_african_smoke_rgba16[t];
        const s32 stride_pixels=136;u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
            stride_pixels*106*4,SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        s32 x,y;if(!dst)return -1;s_gxm.texture_data[index]=dst;memset(dst,0,stride_pixels*106*4);
        for(y=0;y<106;y++)for(x=0;x<131;x++){u16 q=pal[src[y*131+x]];u32 rgba=rgba16_to_rgba8(q);
            /* G_TF_AVERAGE filters hidden RGB before alpha blending. */
            dst[y*stride_pixels+x]=(rgba&0x00ffffffU)|((q&1U)?0xff000000U:0U);
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,131,106,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    return 0;
}

static int init_russian_gameplay_textures(void) {
    static const u8 sky_alpha[3]={255U,148U,201U};
    s32 t;
    /* Scene 901 uses two full 32/16/8/4 RGBA16 mip chains and one base-only
     * 32x32 texture. All three materials are 0x5c (unlit), with authored
     * transparency 0, .418182, and .212121 respectively. */
    for(t=0;t<TNT_GXM_RUSSIAN_SKY_TEXTURE_COUNT;t++){
        s32 index=TNT_GXM_RUSSIAN_SKY_TEXTURE_BASE+t;
        int rc=(g_tnt_russian_sky_texture_texels[t]==1360U)
            ? init_borg_rgba16_mip_texture(index,g_tnt_russian_sky_textures[t],255U,sky_alpha[t])
            : init_borg_rgba16_base_texture(index,g_tnt_russian_sky_textures[t],255U,sky_alpha[t]);
        if(rc<0)return -1;
        /* Preserve the authored N64 tile address modes from the embedded
         * texture display lists. 892 mirrors S/T; 894 mirrors S and wraps T;
         * 893 is base-only and remains clamped by the generic initializer. */
        if(t==0){
            sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_MIRROR);
            sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_MIRROR);
            if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
        }else if(t==1){
            sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_MIRROR);
            sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_REPEAT);
            if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
        }
    }
    {
        const s32 index=TNT_GXM_RUSSIAN_BACKGROUND_TEXTURE;
        u8 *ci=(u8*)malloc(400U*300U);u32 *dst;s32 i;
        if(!ci)return -1;
        celtic_decode_packbits(g_tnt_russian_background_packbits,g_tnt_russian_background_packbits_size,ci,400U*300U);
        dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,400U*300U*4U,
            SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
            &s_gxm.texture_uid[index]);
        if(!dst){free(ci);return -1;}s_gxm.texture_data[index]=dst;
        for(i=0;i<400*300;i++)dst[i]=rgba16_to_rgba8(g_tnt_russian_background_rgba16[ci[i]]);
        free(ci);
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,400,300,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    for(t=0;t<TNT_GXM_RUSSIAN_CANDLE_TEXTURE_COUNT;t++){
        const s32 index=TNT_GXM_RUSSIAN_CANDLE_TEXTURE_BASE+t;
        const u8 *src=g_tnt_russian_candle_ci8[t];const u16 *pal=g_tnt_russian_candle_rgba16[t];
        const s32 stride_pixels=8;u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
            stride_pixels*6U*4U,SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        s32 x,y;if(!dst)return -1;s_gxm.texture_data[index]=dst;memset(dst,0,stride_pixels*6U*4U);
        for(y=0;y<6;y++)for(x=0;x<2;x++){
            u16 q=pal[src[y*2+x]];u32 rgba=rgba16_to_rgba8(q);
            dst[y*stride_pixels+x]=(rgba&0x00ffffffU)|((q&1U)?0xff000000U:0U);
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,2,6,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    return 0;
}

static int init_japanese_gameplay_textures(void) {
    s32 t;
    /* Scene 770 uses eight opaque 0x5c materials. That material class bypasses
     * Borg directional lighting, so preserve the source RGBA16 texels at full
     * intensity and only apply the wrapper/model transforms at draw time. */
    for(t=0;t<TNT_GXM_JAPANESE_SKY_TEXTURE_COUNT;t++)
        if(init_borg_rgba16_mip_texture(TNT_GXM_JAPANESE_SKY_TEXTURE_BASE+t,
            g_tnt_japanese_sky_textures[t],255U,255U)<0)return -1;
    {
        const s32 index=TNT_GXM_JAPANESE_BACKGROUND_TEXTURE;
        u8 *ci=(u8*)malloc(400U*300U);u32 *dst;s32 i;
        if(!ci)return -1;
        celtic_decode_packbits(g_tnt_japanese_background_packbits,
            g_tnt_japanese_background_packbits_size,ci,400U*300U);
        dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,400*300*4,
            SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
            &s_gxm.texture_uid[index]);
        if(!dst){free(ci);return -1;}s_gxm.texture_data[index]=dst;
        for(i=0;i<400*300;i++)dst[i]=rgba16_to_rgba8(g_tnt_japanese_background_rgba16[ci[i]]);
        /* The Japanese plate's lower-right ROM filler begins at native row
         * 285 (rows 286 onward collapse almost completely to #182918).
         * The animated water hides that defect on the left, but the static
         * right side exposes it on a flat panel.  Stretch only the genuinely
         * authored reflection rows 256..284 through the full 256..299 band;
         * never sample the ROM filler rows 285..299. */
        {
            s32 dy,x;
            for(dy=43;dy>=0;dy--){
                u32 pos=(u32)((dy*28*256)/43);
                s32 sy=(s32)(pos>>8);
                u32 frac=pos&0xFFU;
                s32 sy1=(sy<28)?sy+1:sy;
                for(x=0;x<400;x++){
                    u32 a=dst[(256+sy)*400+x];
                    u32 b=dst[(256+sy1)*400+x];
                    u32 q=0; s32 sh;
                    for(sh=0;sh<32;sh+=8){
                        u32 ca=(a>>sh)&0xFFU,cb=(b>>sh)&0xFFU;
                        u32 c=(ca*(256U-frac)+cb*frac+128U)>>8;
                        q|=c<<sh;
                    }
                    dst[(256+dy)*400+x]=q;
                }
            }
            /* The retail plate also leaves a one-texel dark-green seam at the
             * extreme CRT edge.  Vita's exact pixel mapping can expose that
             * terminal row/column immediately beside the black pillarbox.
             * Bleed only the nearest valid interior texels into those final
             * samples; do not widen the crop or alter the successful scene. */
            /* The approved 12-native-pixel pillarbox begins at x~=388 on
             * this 400-wide plate. The ROM defect is wider than one texel:
             * its final visible fringe is about six native columns. Carry the
             * last clean interior sample (x=381) through that narrow fringe;
             * columns beyond it are hidden by the pillarbox anyway. */
            for(dy=0;dy<300;dy++){
                for(x=382;x<400;x++)
                    dst[dy*400+x]=dst[dy*400+381];
            }
        }
        free(ci);
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,400,300,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    for(t=0;t<TNT_GXM_JAPANESE_WATER_TEXTURE_COUNT;t++){
        const s32 index=TNT_GXM_JAPANESE_WATER_TEXTURE_BASE+t;
        const u8 *src=g_tnt_japanese_water_ci8[t];const u16 *pal=g_tnt_japanese_water_rgba16[t];
        const s32 stride_pixels=256;u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
            stride_pixels*32*4,SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
            &s_gxm.texture_uid[index]);s32 x,y;
        if(!dst)return -1;s_gxm.texture_data[index]=dst;memset(dst,0,stride_pixels*32*4);
        for(y=0;y<32;y++)for(x=0;x<254;x++)dst[y*stride_pixels+x]=rgba16_to_rgba8(pal[src[y*254+x]]);
        /* Keep the canonical 254x32 animation. The draw quad extends to y=300,
         * so GXM's linear V filtering stretches these 32 authored rows through
         * the repaired 44-row lower reflection band without mirroring frames. */
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,254,32,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    /* Five private sparkle textures are updated each frame because the five
     * AnimParams instances share art but have independent delays/fade alpha. */
    for(t=0;t<TNT_GXM_JAPANESE_SPARKLE_TEXTURE_COUNT;t++){
        const s32 index=TNT_GXM_JAPANESE_SPARKLE_TEXTURE_BASE+t;
        const s32 stride_pixels=32;u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
            stride_pixels*29*4,SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
            &s_gxm.texture_uid[index]);
        if(!dst)return -1;s_gxm.texture_data[index]=dst;memset(dst,0,stride_pixels*29*4);
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,30,29,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    return 0;
}

static void update_japanese_sparkle_texture(u8 instance,u8 frame,u8 alpha) {
    const s32 stride_pixels=32;u32 *dst;const u8 *src;const u16 *pal;s32 x,y;
    if(instance>=TNT_GXM_JAPANESE_SPARKLE_TEXTURE_COUNT||frame>=TNT_JAPANESE_SPARKLE_FRAME_COUNT)return;
    dst=(u32*)s_gxm.texture_data[TNT_GXM_JAPANESE_SPARKLE_TEXTURE_BASE+instance];
    if(!dst)return;src=g_tnt_japanese_sparkle_ci8[frame];pal=g_tnt_japanese_sparkle_rgba16[frame];
    memset(dst,0,stride_pixels*29*4);
    for(y=0;y<29;y++)for(x=0;x<30;x++){
        u16 p=pal[src[y*30+x]];
        if(p&1U){u32 q=rgba16_to_rgba8(p);dst[y*stride_pixels+x]=(q&0x00ffffffU)|((u32)alpha<<24);}
        /* The sole transparent palette key is bright red. Zero hidden RGB so
         * Vita linear filtering cannot reproduce the red fringe failure seen
         * during African smoke work. */
    }
}

static void update_celtic_sky_alpha(u8 alpha) {
    static const s32 size[4]={32,16,8,4};static const s32 off[4]={0,1024,1280,1344};
    u32 *dst=(u32*)s_gxm.texture_data[TNT_GXM_CELTIC_SKY_TEXTURE_BASE+1];
    const u16 *src=g_tnt_celtic_sky_textures[1];s32 level,x,y;
    if(!dst)return;
    for(level=0;level<3;level++){s32 n=size[level],base=off[level];
        for(y=0;y<n;y++)for(x=0;x<n;x++){s32 sx=x^((y&1)?2:0);u16 p=src[base+y*n+sx];u32 a=(p&1U)?alpha:0U;
            dst[base+y*n+x]=(dst[base+y*n+x]&0x00ffffffU)|(a<<24);}
    }
    for(y=0;y<4;y++)for(x=0;x<4;x++){s32 sx=x^((y&1)?2:0);u16 p=src[1344+y*4+sx];u32 a=(p&1U)?alpha:0U;
        dst[1344+y*8+x]=(dst[1344+y*8+x]&0x00ffffffU)|(a<<24);}
}

static u8 update_greek_name_texture(const char *name) {

    static const u8 widths[46]={
        10,7,9,9,9,9,9,9,9,9,5,9,8,5,8,8,5,9,8,9,9,7,7,
        9,9,4,5,8,7,9,9,9,7,9,8,6,8,9,9,9,8,8,8,12,7,13
    };
    u32 *dst=(u32*)s_gxm.texture_data[TNT_GXM_GREEK_NAME_TEXTURE];
    s32 pen=0;
    const char *p=name?name:"";
    if(!dst)return 0;
    if(strcmp(s_greek_name_cache,p)==0)return s_greek_name_width;
    memset(dst,0,TNT_GXM_GREEK_NAME_W*TNT_GXM_GREEK_NAME_H*4);
    while(*p && pen<TNT_GXM_GREEK_NAME_W){
        unsigned c=(unsigned char)*p++;u8 glyph,w;s32 x,y;
        if(c<32||c>=96)continue;glyph=g_tnt_font_c_map[c-32];w=widths[glyph];
        for(y=0;y<12;y++)for(x=0;x<w&&pen+x<TNT_GXM_GREEK_NAME_W;x++){
            u8 ia=g_tnt_font_c_ia8[(s32)glyph*(13*12)+y*13+x];
            u32 v=(u32)(((ia>>4)&15U)*17U),a=(u32)((ia&15U)*17U);
            dst[y*TNT_GXM_GREEK_NAME_W+pen+x]=v|(v<<8)|(v<<16)|(a<<24);
        }
        pen+=w;
    }
    strncpy(s_greek_name_cache,name?name:"",sizeof(s_greek_name_cache)-1);
    s_greek_name_cache[sizeof(s_greek_name_cache)-1]='\0';
    if(pen>TNT_GXM_GREEK_NAME_W)pen=TNT_GXM_GREEK_NAME_W;
    s_greek_name_width=(u8)pen;
    return s_greek_name_width;
}

static int init_complete_textures(void) {
    u32 total_texels=0,cursor=0;u32 *pack;s32 t;
    for(t=0;t<TNT_GXM_COMPLETE_TEXTURE_COUNT;t++){
        const TNTWonderCompleteTexture *m=&g_tnt_wonder_complete_textures[t];
        u32 n=(u32)m->width*(u32)m->height;
        /* N64 mirror tiles use one source-width followed by its reversed copy.
         * Bake that period for Mayan TEX_EDGE textures because GXM MIRROR does
         * not match the RDP tile-mask behavior used by these resources. */
        if((m->wrap&0x20U)&&(m->wrap&0x08U))n*=2U;
        total_texels+=n;
    }
    pack=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,total_texels*sizeof(u32),
        SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
        &s_gxm.complete_texture_uid);
    if(!pack)return -1;s_gxm.complete_texture_pack=pack;memset(pack,0,total_texels*sizeof(u32));
    for(t=0;t<TNT_GXM_COMPLETE_TEXTURE_COUNT;t++){
        const TNTWonderCompleteTexture *meta=&g_tnt_wonder_complete_textures[t];
        const s32 index=TNT_GXM_COMPLETE_TEXTURE_BASE+t;u32 *src=pack+cursor;
        u32 outw=meta->width,outh=meta->height;s32 x,y;
        /* Scene 186 uses LoadBlock DXT=0 throughout. Undo the TMEM odd-row
         * two-texel word permutation before GXM samples the image linearly. */
        for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
            s32 sx=(meta->width>=4U)?(x^((y&1)?2:0)):x;
            src[(u32)y*(u32)meta->width+(u32)x]=rgba16_to_rgba8(
                g_tnt_wonder_complete_texels[meta->offset+(u32)y*(u32)meta->width+(u32)sx]);
        }
        if((meta->wrap&0x20U)&&(meta->wrap&0x08U)){
            u32 basew=meta->width;
            for(y=(s32)meta->height-1;y>=0;y--)for(x=(s32)basew-1;x>=0;x--){
                u32 *row=src+(u32)y*basew*2U,v=src[(u32)y*basew+(u32)x];
                row[x]=v;row[basew+(basew-1U-(u32)x)]=v;
            }
            outw=basew*2U;
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],src,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,outw,outh,0)<0)return -1;
        /* Source TEX_EDGE uses bilinear sampling followed by alpha coverage.
         * The dedicated fragment shader now provides the threshold/discard, so
         * retain linear filtering instead of the rejected point/4x workaround. */
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        if((meta->wrap&0x20U)&&(meta->wrap&0x08U))sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_REPEAT);
        else sceGxmTextureSetUAddrMode(&s_gxm.textures[index],(meta->wrap&2)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],(meta->wrap&4)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
        cursor+=outw*outh;
    }
    return 0;
}

static int init_greek_complete_textures(void) {
    const unsigned int bytes=(unsigned int)(TNT_GREEK_COMPLETE_TEXEL_COUNT*sizeof(u32));
    u32 *pack;s32 t;
    pack=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,bytes,
        SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
        &s_gxm.greek_complete_texture_uid);
    if(!pack)return -1;
    s_gxm.greek_complete_texture_pack=pack;
    for(t=0;t<TNT_GREEK_COMPLETE_TEXEL_COUNT;t++)pack[t]=rgba16_to_rgba8(g_tnt_greek_complete_texels[t]);
    for(t=0;t<TNT_GXM_GREEK_COMPLETE_TEXTURE_COUNT;t++){
        const TNTWonderCompleteTexture *meta=&g_tnt_greek_complete_textures[t];
        const s32 index=TNT_GXM_GREEK_COMPLETE_TEXTURE_BASE+t;
        u32 *src=pack+meta->offset;
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],src,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
            meta->width,meta->height,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],(meta->wrap&2)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],(meta->wrap&4)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    return 0;
}

static int init_egyptian_complete_textures(void) {
    const unsigned int bytes=(unsigned int)(TNT_EGYPTIAN_COMPLETE_TEXEL_COUNT*sizeof(u32));
    u32 *pack;s32 t;
    pack=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,bytes,
        SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
        &s_gxm.egyptian_complete_texture_uid);
    if(!pack)return -1;s_gxm.egyptian_complete_texture_pack=pack;memset(pack,0,bytes);
    for(t=0;t<TNT_GXM_EGYPTIAN_COMPLETE_TEXTURE_COUNT;t++){
        const TNTWonderCompleteTexture *meta=&g_tnt_egyptian_complete_textures[t];
        const s32 index=TNT_GXM_EGYPTIAN_COMPLETE_TEXTURE_BASE+t;u32 *src=pack+meta->offset,*gpu_src=src;u32 gpu_w=meta->width;s32 x,y;
        for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
            s32 sx=(meta->pad[0]&&meta->width>=4U)?(x^((y&1)?2:0)):x;
            src[y*(s32)meta->width+x]=rgba16_to_rgba8(
                g_tnt_egyptian_complete_texels[meta->offset+(u32)y*(u32)meta->width+(u32)sx]);
        }
        if(t==12||t==13){
            /* Resources 350/349 form the chest relief. One N64 mirror period
             * is original32 + reversed32; bake it because GXM MIRROR smeared. */
            u32 *mirror=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
                (unsigned int)meta->width*2U*(unsigned int)meta->height*sizeof(u32),
                SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
                &s_gxm.texture_uid[index]);
            if(!mirror)return -1;s_gxm.texture_data[index]=mirror;
            for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
                u32 v=src[(u32)y*(u32)meta->width+(u32)x],row=(u32)y*(u32)meta->width*2U;
                mirror[row+(u32)x]=v;mirror[row+(u32)meta->width*2U-1U-(u32)x]=v;
            }
            gpu_src=mirror;gpu_w=(u32)meta->width*2U;
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],gpu_src,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,gpu_w,meta->height,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],(t==12||t==13||(meta->wrap&2))?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],(meta->wrap&4)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    return 0;
}

static int init_celtic_complete_textures(void) {
    const unsigned int bytes=(unsigned int)(TNT_CELTIC_COMPLETE_TEXEL_COUNT*sizeof(u32));
    u32 *pack;s32 t;
    pack=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,bytes,
        SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
        &s_gxm.celtic_complete_texture_uid);
    if(!pack)return -1;s_gxm.celtic_complete_texture_pack=pack;memset(pack,0,bytes);
    for(t=0;t<TNT_GXM_CELTIC_COMPLETE_TEXTURE_COUNT;t++){
        const TNTWonderCompleteTexture *meta=&g_tnt_celtic_complete_textures[t];
        const s32 index=TNT_GXM_CELTIC_COMPLETE_TEXTURE_BASE+t;
        u32 *src=pack+meta->offset,*gpu_src=src;u32 gpu_w=meta->width;s32 x,y;
        for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
            s32 sx=(meta->pad[0]&&meta->width>=4U)?(x^((y&1)?2:0)):x;
            u32 rgba=rgba16_to_rgba8(
                g_tnt_celtic_complete_texels[meta->offset+(u32)y*(u32)meta->width+(u32)sx]);
            /* Keep source alpha intact. Scene 470's effect-material alpha is
             * animated by Borg animation type 2 and is applied per frame. */
            src[y*(s32)meta->width+x]=rgba;
        }
        if(meta->wrap&0x08U){
            /* Borg mirror address mode. Bake one forward+reverse U period;
             * this avoids the GXM mirror sampling artifact already seen on
             * the Egyptian relief while preserving the authored repetition. */
            u32 *mirror=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
                (unsigned int)meta->width*2U*(unsigned int)meta->height*sizeof(u32),
                SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
                &s_gxm.texture_uid[index]);
            if(!mirror)return -1;s_gxm.texture_data[index]=mirror;
            for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
                u32 v=src[(u32)y*(u32)meta->width+(u32)x],row=(u32)y*(u32)meta->width*2U;
                mirror[row+(u32)x]=v;mirror[row+(u32)meta->width*2U-1U-(u32)x]=v;
            }
            gpu_src=mirror;gpu_w=(u32)meta->width*2U;
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],gpu_src,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
            gpu_w,meta->height,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],((meta->wrap&0x08U)||(meta->wrap&2U))?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],(meta->wrap&4U)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    return 0;
}

static int init_african_complete_textures(void) {
    const unsigned int bytes=(unsigned int)(TNT_AFRICAN_COMPLETE_TEXEL_COUNT*sizeof(u32));
    static const u8 effect_source[4]={48U,48U,48U,47U};
    u32 *pack;s32 t;
    pack=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,bytes,
        SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
        &s_gxm.african_complete_texture_uid);
    if(!pack)return -1;s_gxm.african_complete_texture_pack=pack;memset(pack,0,bytes);
    for(t=0;t<TNT_GXM_AFRICAN_COMPLETE_TEXTURE_COUNT;t++){
        const TNTWonderCompleteTexture *meta=&g_tnt_african_complete_textures[t];
        const s32 index=TNT_GXM_AFRICAN_COMPLETE_TEXTURE_BASE+t;
        u32 *src=pack+meta->offset,*gpu_src=src;u32 gpu_w=meta->width;s32 x,y;
        for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
            s32 sx=(meta->pad[0]&&meta->width>=4U)?(x^((y&1)?2:0)):x;
            src[(u32)y*(u32)meta->width+(u32)x]=rgba16_to_rgba8(
                g_tnt_african_complete_texels[meta->offset+(u32)y*(u32)meta->width+(u32)sx]);
        }
        if(meta->wrap&0x08U){
            u32 *mirror=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
                (unsigned int)meta->width*2U*(unsigned int)meta->height*sizeof(u32),
                SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
                &s_gxm.texture_uid[index]);
            if(!mirror)return -1;s_gxm.texture_data[index]=mirror;
            for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
                u32 v=src[(u32)y*(u32)meta->width+(u32)x],row=(u32)y*(u32)meta->width*2U;
                mirror[row+(u32)x]=v;mirror[row+(u32)meta->width*2U-1U-(u32)x]=v;
            }
            gpu_src=mirror;gpu_w=(u32)meta->width*2U;
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],gpu_src,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
            gpu_w,meta->height,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],((meta->wrap&0x08U)||(meta->wrap&2U))?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],(meta->wrap&4U)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    /* Four independent effect copies are required because meshes 542/543/544
     * share source texture 48 but carry different animated transparency tracks. */
    for(t=0;t<TNT_GXM_AFRICAN_EFFECT_TEXTURE_COUNT;t++){
        const TNTWonderCompleteTexture *meta=&g_tnt_african_complete_textures[effect_source[t]];
        const s32 index=TNT_GXM_AFRICAN_EFFECT_TEXTURE_BASE+t;
        u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
            (unsigned int)meta->width*(unsigned int)meta->height*sizeof(u32),
            SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
            &s_gxm.texture_uid[index]);s32 x,y;
        if(!dst)return -1;s_gxm.texture_data[index]=dst;
        for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
            s32 sx=(meta->pad[0]&&meta->width>=4U)?(x^((y&1)?2:0)):x;
            dst[(u32)y*(u32)meta->width+(u32)x]=rgba16_to_rgba8(
                g_tnt_african_complete_texels[meta->offset+(u32)y*(u32)meta->width+(u32)sx]);
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
            meta->width,meta->height,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],(meta->wrap&2U)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],(meta->wrap&4U)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    return 0;
}

static int init_russian_complete_textures(void) {
    const unsigned int bytes=(unsigned int)(TNT_RUSSIAN_COMPLETE_TEXEL_COUNT*sizeof(u32));
    u32 *pack;s32 t;
    pack=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,bytes,
        SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
        &s_gxm.russian_complete_texture_uid);
    if(!pack)return -1;s_gxm.russian_complete_texture_pack=pack;memset(pack,0,bytes);
    for(t=0;t<TNT_GXM_RUSSIAN_COMPLETE_TEXTURE_COUNT;t++){
        const TNTWonderCompleteTexture *meta=&g_tnt_russian_complete_textures[t];
        const s32 index=TNT_GXM_RUSSIAN_COMPLETE_TEXTURE_BASE+t;
        u32 *src=pack+meta->offset,*gpu_src=src;u32 gpu_w=meta->width;s32 x,y;
        for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
            s32 sx=(meta->pad[0]&&meta->width>=4U)?(x^((y&1)?2:0)):x;
            src[(u32)y*(u32)meta->width+(u32)x]=rgba16_to_rgba8(
                g_tnt_russian_complete_texels[meta->offset+(u32)y*(u32)meta->width+(u32)sx]);
        }
        if(meta->wrap&0x08U){
            u32 *mirror=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
                (unsigned int)meta->width*2U*(unsigned int)meta->height*sizeof(u32),
                SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
                &s_gxm.texture_uid[index]);
            if(!mirror)return -1;s_gxm.texture_data[index]=mirror;
            for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
                u32 v=src[(u32)y*(u32)meta->width+(u32)x],row=(u32)y*(u32)meta->width*2U;
                mirror[row+(u32)x]=v;mirror[row+(u32)meta->width*2U-1U-(u32)x]=v;
            }
            gpu_src=mirror;gpu_w=(u32)meta->width*2U;
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],gpu_src,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
            gpu_w,meta->height,0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],((meta->wrap&0x08U)||(meta->wrap&2U))?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],(meta->wrap&4U)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    return 0;
}

static s32 intro_texture_storage_texels(const TNTIntroTextureDef *def) {
    s32 l,total=0;
    for(l=0;l<(s32)def->levels;l++){
        s32 n=(s32)def->size>>l;
        s32 stride=n<8?8:n;
        total+=stride*n;
    }
    return total;
}

static int init_japanese_complete_textures(void) {
    const unsigned int bytes=(unsigned int)(TNT_JAPANESE_COMPLETE_TEXEL_COUNT*sizeof(u32));
    u32 *pack;s32 t;
    pack=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,bytes,
        SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
        &s_gxm.japanese_complete_texture_uid);
    if(!pack)return -1;s_gxm.japanese_complete_texture_pack=pack;memset(pack,0,bytes);
    for(t=0;t<TNT_GXM_JAPANESE_COMPLETE_TEXTURE_COUNT;t++){
        const TNTWonderCompleteTexture *meta=&g_tnt_japanese_complete_textures[t];
        const s32 index=TNT_GXM_JAPANESE_COMPLETE_TEXTURE_BASE+t;
        u32 *src=pack+meta->offset,*gpu_src=src;u32 gpu_w=meta->width;u8 mip_count=0;s32 x,y;
        for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
            s32 sx=(meta->pad[0]&&meta->width>=4U)?(x^((y&1)?2:0)):x;
            u32 rgba=rgba16_to_rgba8(
                g_tnt_japanese_complete_texels[meta->offset+(u32)y*(u32)meta->width+(u32)sx]);
            src[y*(s32)meta->width+x]=rgba;
        }
        if(meta->wrap&0x08U){
            u32 *mirror=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
                (unsigned int)meta->width*2U*(unsigned int)meta->height*sizeof(u32),
                SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
                &s_gxm.texture_uid[index]);
            if(!mirror)return -1;s_gxm.texture_data[index]=mirror;
            for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
                u32 v=src[(u32)y*(u32)meta->width+(u32)x],row=(u32)y*(u32)meta->width*2U;
                mirror[row+(u32)x]=v;mirror[row+(u32)meta->width*2U-1U-(u32)x]=v;
            }
            gpu_src=mirror;gpu_w=(u32)meta->width*2U;
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],gpu_src,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
            gpu_w,meta->height,mip_count)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        if(mip_count){
            sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_ENABLED);
            sceGxmTextureSetMipmapCount(&s_gxm.textures[index],mip_count);
            sceGxmTextureSetLodBias(&s_gxm.textures[index],31);
        }else sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],((meta->wrap&0x08U)||(meta->wrap&2U))?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],(meta->wrap&4U)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    /* Finale animation 758 changes material transparency for four effect
     * groups.  Private copies let us fold that alpha into the source RGBA
     * without modifying temple textures shared by ordinary scene geometry. */
    {
        static const u8 effect_source[TNT_GXM_JAPANESE_EFFECT_TEXTURE_COUNT]={1U,3U,4U,5U};
        for(t=0;t<TNT_GXM_JAPANESE_EFFECT_TEXTURE_COUNT;t++){
            const TNTWonderCompleteTexture *meta=&g_tnt_japanese_complete_textures[effect_source[t]];
            const s32 index=TNT_GXM_JAPANESE_EFFECT_TEXTURE_BASE+t;
            u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
                (unsigned int)meta->width*(unsigned int)meta->height*sizeof(u32),
                SCE_GXM_TEXTURE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
                &s_gxm.texture_uid[index]);s32 x,y;
            if(!dst)return -1;s_gxm.texture_data[index]=dst;
            for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
                s32 sx=(meta->pad[0]&&meta->width>=4U)?(x^((y&1)?2:0)):x;
                dst[(u32)y*(u32)meta->width+(u32)x]=rgba16_to_rgba8(
                    g_tnt_japanese_complete_texels[meta->offset+(u32)y*(u32)meta->width+(u32)sx]);
            }
            if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
                meta->width,meta->height,0)<0)return -1;
            sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
            sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
            sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
            sceGxmTextureSetUAddrMode(&s_gxm.textures[index],(meta->wrap&2U)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
            sceGxmTextureSetVAddrMode(&s_gxm.textures[index],(meta->wrap&4U)?SCE_GXM_TEXTURE_ADDR_REPEAT:SCE_GXM_TEXTURE_ADDR_CLAMP);
            if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
        }
    }
    return 0;
}

static int init_intro_textures(void) {
    s32 t,l,x,y;
    for(t=0;t<TNT_GXM_INTRO_TEXTURE_COUNT;t++){
        const TNTIntroTextureDef *def=&g_tnt_intro_textures[t];
        const s32 index=TNT_GXM_INTRO_TEXTURE_BASE+t;
        const s32 storage=intro_texture_storage_texels(def);
        u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
            (unsigned int)storage*4U,SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        u32 srcbase=def->data_offset; s32 dstbase=0;
        if(!dst)return -1;
        memset(dst,0,(size_t)storage*4U);s_gxm.texture_data[index]=dst;
        for(l=0;l<(s32)def->levels;l++){
            s32 n=(s32)def->size>>l,stride=n<8?8:n;
            for(y=0;y<n;y++)for(x=0;x<n;x++){
                s32 sx=x^((y&1)?2:0);
                dst[dstbase+y*stride+x]=rgba16_to_rgba8(
                    g_tnt_intro_texture_rgba16[srcbase+(u32)y*(u32)n+(u32)sx]);
            }
            srcbase+=(u32)(n*n);dstbase+=stride*n;
        }
        /* Intro resource 943 (scene texture slots 33/34) is the pickaxe
         * blade.  The N64 draws it with TEX_EDGE: 992/1024 texels are fully
         * opaque and only the top padding row is alpha-zero.  Our stock GXM
         * shader cannot alpha-test/discard before Z update.  Copy the adjacent
         * metal row into that padding row so this source-solid blade can use
         * the normal opaque/depth-writing path without a black/sky fringe. */
        if(t==33 || t==34){
            for(x=0;x<(s32)def->size;x++)dst[x]=dst[(s32)def->size+x]|0xff000000U;
        }
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
            def->size,def->size,def->levels>1?def->levels:0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        if(def->levels>1){
            sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_ENABLED);
            sceGxmTextureSetMipmapCount(&s_gxm.textures[index],def->levels);
            sceGxmTextureSetLodBias(&s_gxm.textures[index],31);
        }else{
            sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        }
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],(SceGxmTextureAddrMode)def->u_addr);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],(SceGxmTextureAddrMode)def->v_addr);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    /* The five textured material-opacity tracks are source draws 73..77.
     * Give each one a private copy so its animated material alpha can be
     * folded into the texture alpha without requiring a custom shader. */
    for(t=0;t<TNT_GXM_INTRO_FADE_TEXTURE_COUNT;t++){
        const TNTIntroDrawDef *draw=&g_tnt_intro_draws[73+t];
        const TNTIntroTextureDef *def=&g_tnt_intro_textures[draw->texture];
        const s32 srcidx=TNT_GXM_INTRO_TEXTURE_BASE+draw->texture;
        const s32 index=TNT_GXM_INTRO_FADE_TEXTURE_BASE+t;
        const s32 storage=intro_texture_storage_texels(def);
        u32 *dst=(u32*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
            (unsigned int)storage*4U,SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.texture_uid[index]);
        if(!dst)return -1;
        memcpy(dst,s_gxm.texture_data[srcidx],(size_t)storage*4U);s_gxm.texture_data[index]=dst;
        if(sceGxmTextureInitLinear(&s_gxm.textures[index],dst,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
            def->size,def->size,def->levels>1?def->levels:0)<0)return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_LINEAR);
        if(def->levels>1){
            sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_ENABLED);
            sceGxmTextureSetMipmapCount(&s_gxm.textures[index],def->levels);
            sceGxmTextureSetLodBias(&s_gxm.textures[index],31);
        }else sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],(SceGxmTextureAddrMode)def->u_addr);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],(SceGxmTextureAddrMode)def->v_addr);
        if(sceGxmTextureValidate(&s_gxm.textures[index])<0)return -1;
    }
    return 0;
}

static void update_intro_fade_textures(const TNTIntroPose *pose) {
    s32 t,i;
    for(t=0;t<TNT_GXM_INTRO_FADE_TEXTURE_COUNT;t++){
        const TNTIntroDrawDef *draw=&g_tnt_intro_draws[73+t];
        const TNTIntroTextureDef *def=&g_tnt_intro_textures[draw->texture];
        const s32 storage=intro_texture_storage_texels(def);
        const u32 *src=(const u32*)s_gxm.texture_data[TNT_GXM_INTRO_TEXTURE_BASE+draw->texture];
        u32 *dst=(u32*)s_gxm.texture_data[TNT_GXM_INTRO_FADE_TEXTURE_BASE+t];
        f32 opacity=1.0f-pose->draw_transparency[73+t];
        u32 a;
        if(opacity<0.0f)opacity=0.0f;if(opacity>1.0f)opacity=1.0f;
        a=(u32)(opacity*255.0f+0.5f);
        for(i=0;i<storage;i++){
            u32 p=src[i],sa=p>>24;
            dst[i]=(p&0x00ffffffU)|((((sa*a)+127U)/255U)<<24);
        }
    }
}

static void projection_matrix(f32 m[16]) {
    const f32 nearz=4.0f,farz=8000.0f;
    const f32 half_h=nearz*tanf(20.0f*TNT_GXM_DEG2RAD);
    const f32 half_w=half_h*(4.0f/3.0f);
    memset(m,0,16*sizeof(f32));
    m[0]=(2.0f*nearz)/(2.0f*half_w);
    m[5]=(2.0f*nearz)/(2.0f*half_h);
    m[8]=0.0f;m[9]=0.0f;
    m[10]=-(farz+nearz)/(farz-nearz);
    m[14]=(-2.0f*farz*nearz)/(farz-nearz);
    m[11]=-1.0f;
}

static u32 shade_color(const TNTGxmMatrix *draw,const TNTWonderVertex *v) {
    f32 nx,ny,nz,nlen,d0,d1,r,g,b;
    transform_vector(draw,(f32)v->nx/127.0f,(f32)v->ny/127.0f,(f32)v->nz/127.0f,&nx,&ny,&nz);
    nlen=vec_len(nx,ny,nz);if(nlen>0.000001f){nx/=nlen;ny/=nlen;nz/=nlen;}
    d0=nx*s_light_dir[0][0]+ny*s_light_dir[0][1]+nz*s_light_dir[0][2];if(d0<0)d0=0;
    d1=nx*s_light_dir[1][0]+ny*s_light_dir[1][1]+nz*s_light_dir[1][2];if(d1<0)d1=0;
    r=s_ambient_rgb[0]+d0*s_light_rgb[0][0]+d1*s_light_rgb[1][0];
    g=s_ambient_rgb[1]+d0*s_light_rgb[0][1]+d1*s_light_rgb[1][1];
    b=s_ambient_rgb[2]+d0*s_light_rgb[0][2]+d1*s_light_rgb[1][2];
    if(r>1)r=1;
    if(g>1)g=1;
    if(b>1)b=1;
    if(r<0)r=0;
    if(g<0)g=0;
    if(b<0)b=0;
    return ((u32)(r*255.0f+0.5f))|((u32)(g*255.0f+0.5f)<<8)|((u32)(b*255.0f+0.5f)<<16)|0xff000000U;
}

/* Completed-Wonder meshes are triangle-flattened, so the same source normal
 * is repeated many times. Lighting depends only on the current node transform
 * plus that normal, not on position/UV. Cache exact per-frame results so Greek
 * and African do not repeat thousands of sqrt/div normalizations. */
#define TNT_COMPLETE_SHADE_CACHE_SIZE 2048U
static u32 s_complete_shade_cache_key[TNT_COMPLETE_SHADE_CACHE_SIZE];
static u32 s_complete_shade_cache_color[TNT_COMPLETE_SHADE_CACHE_SIZE];
static u8 s_complete_shade_cache_valid[TNT_COMPLETE_SHADE_CACHE_SIZE];

static void complete_shade_cache_begin(void) {
    memset(s_complete_shade_cache_valid,0,sizeof(s_complete_shade_cache_valid));
}

static u32 complete_shade_key(u8 node,const TNTWonderVertex *v) {
    return ((u32)node<<24)|((u32)(u8)v->nx<<16)|((u32)(u8)v->ny<<8)|(u32)(u8)v->nz;
}

static u8 complete_shade_cache_lookup(u32 key,u32 *color) {
    u32 slot=(key*2654435761U)&(TNT_COMPLETE_SHADE_CACHE_SIZE-1U),probe;
    for(probe=0;probe<TNT_COMPLETE_SHADE_CACHE_SIZE;probe++){
        if(!s_complete_shade_cache_valid[slot])return FALSE;
        if(s_complete_shade_cache_key[slot]==key){*color=s_complete_shade_cache_color[slot];return TRUE;}
        slot=(slot+1U)&(TNT_COMPLETE_SHADE_CACHE_SIZE-1U);
    }
    return FALSE;
}

static void complete_shade_cache_store(u32 key,u32 color) {
    u32 slot=(key*2654435761U)&(TNT_COMPLETE_SHADE_CACHE_SIZE-1U),probe;
    for(probe=0;probe<TNT_COMPLETE_SHADE_CACHE_SIZE;probe++){
        if(!s_complete_shade_cache_valid[slot]||s_complete_shade_cache_key[slot]==key){
            s_complete_shade_cache_valid[slot]=TRUE;s_complete_shade_cache_key[slot]=key;s_complete_shade_cache_color[slot]=color;return;
        }
        slot=(slot+1U)&(TNT_COMPLETE_SHADE_CACHE_SIZE-1U);
    }
}

static u32 shade_color_cached(u8 node,const TNTGxmMatrix *draw,const TNTWonderVertex *v) {
    u32 key=complete_shade_key(node,v),color;
    if(complete_shade_cache_lookup(key,&color))return color;
    color=shade_color(draw,v);complete_shade_cache_store(key,color);return color;
}

/* Completed-Wonder source meshes are flattened into triangle lists, so shared
 * mesh vertices are repeated several times.  Position transforms depend only
 * on the current node matrix and source XYZ.  Cache those exact results for
 * one geometry build; Greek repeats ~73% and Egyptian ~76% of node+position
 * pairs, making this a pure CPU reduction with no geometry approximation. */
#define TNT_COMPLETE_POS_CACHE_SIZE 8192U
static u64 s_complete_pos_cache_key[TNT_COMPLETE_POS_CACHE_SIZE];
static f32 s_complete_pos_cache_x[TNT_COMPLETE_POS_CACHE_SIZE];
static f32 s_complete_pos_cache_y[TNT_COMPLETE_POS_CACHE_SIZE];
static f32 s_complete_pos_cache_z[TNT_COMPLETE_POS_CACHE_SIZE];
static u8 s_complete_pos_cache_valid[TNT_COMPLETE_POS_CACHE_SIZE];

static void complete_position_cache_begin(void) {
    memset(s_complete_pos_cache_valid,0,sizeof(s_complete_pos_cache_valid));
}

static u64 complete_position_key(u8 node,const TNTWonderVertex *v) {
    return ((u64)node<<48)|((u64)(u16)v->x<<32)|((u64)(u16)v->y<<16)|(u64)(u16)v->z;
}

static u32 complete_position_hash(u64 key) {
    u32 h=(u32)key^(u32)(key>>32);
    return (h*2654435761U)&(TNT_COMPLETE_POS_CACHE_SIZE-1U);
}

static u8 complete_position_cache_lookup(u64 key,f32 *x,f32 *y,f32 *z) {
    u32 slot=complete_position_hash(key),probe;
    for(probe=0;probe<TNT_COMPLETE_POS_CACHE_SIZE;probe++){
        if(!s_complete_pos_cache_valid[slot])return FALSE;
        if(s_complete_pos_cache_key[slot]==key){
            *x=s_complete_pos_cache_x[slot];*y=s_complete_pos_cache_y[slot];*z=s_complete_pos_cache_z[slot];return TRUE;
        }
        slot=(slot+1U)&(TNT_COMPLETE_POS_CACHE_SIZE-1U);
    }
    return FALSE;
}

static void complete_position_cache_store(u64 key,f32 x,f32 y,f32 z) {
    u32 slot=complete_position_hash(key),probe;
    for(probe=0;probe<TNT_COMPLETE_POS_CACHE_SIZE;probe++){
        if(!s_complete_pos_cache_valid[slot]||s_complete_pos_cache_key[slot]==key){
            s_complete_pos_cache_valid[slot]=TRUE;s_complete_pos_cache_key[slot]=key;
            s_complete_pos_cache_x[slot]=x;s_complete_pos_cache_y[slot]=y;s_complete_pos_cache_z[slot]=z;return;
        }
        slot=(slot+1U)&(TNT_COMPLETE_POS_CACHE_SIZE-1U);
    }
}

static void complete_transform_point_cached(u8 node,const TNTGxmMatrix *mv,const TNTWonderVertex *v,
                                            f32 *x,f32 *y,f32 *z) {
    u64 key=complete_position_key(node,v);
    if(complete_position_cache_lookup(key,x,y,z))return;
    transform_point(mv,(f32)v->x,(f32)v->y,(f32)v->z,x,y,z);
    complete_position_cache_store(key,*x,*y,*z);
}

/* Greek scene 314 has 10,938 flattened vertex references but only ~2,947
 * distinct node+XYZ positions. Test29 discovered those duplicates with a hash
 * lookup for every vertex on every frame. Build that mapping once instead,
 * then transform each unique position once per frame and index it directly. */
#define TNT_GREEK_COMPLETE_VERTEX_COUNT (TNT_GREEK_COMPLETE_TRIANGLE_COUNT * 3U)
#define TNT_GREEK_COMPLETE_UNIQUE_POS_MAX 4096U
static u16 s_greek_pos_map[TNT_GREEK_COMPLETE_VERTEX_COUNT];
static const TNTWonderVertex *s_greek_pos_source[TNT_GREEK_COMPLETE_UNIQUE_POS_MAX];
static u8 s_greek_pos_node[TNT_GREEK_COMPLETE_UNIQUE_POS_MAX];
static f32 s_greek_pos_x[TNT_GREEK_COMPLETE_UNIQUE_POS_MAX];
static f32 s_greek_pos_y[TNT_GREEK_COMPLETE_UNIQUE_POS_MAX];
static f32 s_greek_pos_z[TNT_GREEK_COMPLETE_UNIQUE_POS_MAX];
static u16 s_greek_pos_unique_count;
static u16 s_greek_pos_hash_value[TNT_COMPLETE_POS_CACHE_SIZE];
#define TNT_GREEK_COMPLETE_UNIQUE_SHADE_MAX 1024U
static u16 s_greek_shade_map[TNT_GREEK_COMPLETE_VERTEX_COUNT];
static const TNTWonderVertex *s_greek_shade_source[TNT_GREEK_COMPLETE_UNIQUE_SHADE_MAX];
static u8 s_greek_shade_node[TNT_GREEK_COMPLETE_UNIQUE_SHADE_MAX];
static u32 s_greek_shade_color[TNT_GREEK_COMPLETE_UNIQUE_SHADE_MAX];
static u16 s_greek_shade_unique_count;
static u8 s_greek_pos_map_ready;

static int greek_complete_prepare_position_map(void) {
    u32 i,k;
    if(s_greek_pos_map_ready)return 0;
    memset(s_complete_pos_cache_valid,0,sizeof(s_complete_pos_cache_valid));
    memset(s_complete_shade_cache_valid,0,sizeof(s_complete_shade_cache_valid));
    s_greek_pos_unique_count=0;
    s_greek_shade_unique_count=0;
    for(i=0;i<TNT_GREEK_COMPLETE_TRIANGLE_COUNT;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_greek_complete_triangles[i];
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];
            u64 key=complete_position_key(tri->node,v);
            u32 slot=complete_position_hash(key),probe;
            u16 index=0xffffU;
            u32 skey,sslot;
            u16 sindex=0xffffU;
            for(probe=0;probe<TNT_COMPLETE_POS_CACHE_SIZE;probe++){
                if(!s_complete_pos_cache_valid[slot]){
                    if(s_greek_pos_unique_count>=TNT_GREEK_COMPLETE_UNIQUE_POS_MAX)return -1;
                    index=s_greek_pos_unique_count++;
                    s_complete_pos_cache_valid[slot]=TRUE;
                    s_complete_pos_cache_key[slot]=key;
                    s_greek_pos_hash_value[slot]=index;
                    s_greek_pos_source[index]=v;
                    s_greek_pos_node[index]=tri->node;
                    break;
                }
                if(s_complete_pos_cache_key[slot]==key){index=s_greek_pos_hash_value[slot];break;}
                slot=(slot+1U)&(TNT_COMPLETE_POS_CACHE_SIZE-1U);
            }
            if(index==0xffffU)return -1;
            s_greek_pos_map[i*3U+k]=index;

            skey=complete_shade_key(tri->node,v);
            sslot=(skey*2654435761U)&(TNT_COMPLETE_SHADE_CACHE_SIZE-1U);
            for(probe=0;probe<TNT_COMPLETE_SHADE_CACHE_SIZE;probe++){
                if(!s_complete_shade_cache_valid[sslot]){
                    if(s_greek_shade_unique_count>=TNT_GREEK_COMPLETE_UNIQUE_SHADE_MAX)return -1;
                    sindex=s_greek_shade_unique_count++;
                    s_complete_shade_cache_valid[sslot]=TRUE;
                    s_complete_shade_cache_key[sslot]=skey;
                    /* During one-time map construction, color[] is safe scratch
                     * storage for the direct unique-shade index. */
                    s_complete_shade_cache_color[sslot]=(u32)sindex;
                    s_greek_shade_source[sindex]=v;
                    s_greek_shade_node[sindex]=tri->node;
                    break;
                }
                if(s_complete_shade_cache_key[sslot]==skey){sindex=(u16)s_complete_shade_cache_color[sslot];break;}
                sslot=(sslot+1U)&(TNT_COMPLETE_SHADE_CACHE_SIZE-1U);
            }
            if(sindex==0xffffU)return -1;
            s_greek_shade_map[i*3U+k]=sindex;
        }
    }
    s_greek_pos_map_ready=TRUE;
    return 0;
}

static void greek_complete_transform_unique_positions(const TNTGxmMatrix mv[TNT_GREEK_COMPLETE_NODE_COUNT]) {
    u32 i;
    for(i=0;i<s_greek_pos_unique_count;i++){
        const TNTWonderVertex *v=s_greek_pos_source[i];
        transform_point(&mv[s_greek_pos_node[i]],(f32)v->x,(f32)v->y,(f32)v->z,
            &s_greek_pos_x[i],&s_greek_pos_y[i],&s_greek_pos_z[i]);
    }
}

static void greek_complete_shade_unique_normals(const TNTGxmMatrix draw[TNT_GREEK_COMPLETE_NODE_COUNT]) {
    u32 i;
    for(i=0;i<s_greek_shade_unique_count;i++)
        s_greek_shade_color[i]=shade_color(&draw[s_greek_shade_node[i]],s_greek_shade_source[i]);
}

/* Test211D: reusable one-time node/XYZ->index map for the two remaining
 * slow Hall models. The source scene triangles are flattened, so repeated
 * vertices were transformed separately for each texture and lighting pass.
 * This caches ONLY the mapping, not the animated positions: every display
 * frame still transforms each distinct point with the original view/model
 * matrix and renders original triangles. Applies only to Hall 0 and 3.
 * All cinematics retain their approved unmodified geometry paths. */
#define TNT_HALL_POS_VERT_MAX (3320U * 3U)
#define TNT_HALL_POS_HASH_SIZE 16384U
static u16 s_hall_pos_map[TNT_HALL_POS_VERT_MAX];
static u64 s_hall_pos_hash_key[TNT_HALL_POS_HASH_SIZE];
static u16 s_hall_pos_hash_index[TNT_HALL_POS_HASH_SIZE];
static u8 s_hall_pos_hash_used[TNT_HALL_POS_HASH_SIZE];
static const TNTWonderVertex *s_hall_pos_source[TNT_HALL_POS_VERT_MAX];
static u8 s_hall_pos_node[TNT_HALL_POS_VERT_MAX];
static f32 s_hall_pos_x[TNT_HALL_POS_VERT_MAX];
static f32 s_hall_pos_y[TNT_HALL_POS_VERT_MAX];
static f32 s_hall_pos_z[TNT_HALL_POS_VERT_MAX];
static u16 s_hall_pos_count;
static u8 s_hall_pos_ready;
static u8 s_hall_pos_wonder=255U;

static int hall_pos_prepare(u8 wonder,const TNTWonderCompleteTriangle *tris,u32 count) {
    u32 i,k;
    if(s_hall_pos_ready&&s_hall_pos_wonder==wonder)return 0;
    s_hall_pos_ready=FALSE;
    if(!tris || !count || count*3U>TNT_HALL_POS_VERT_MAX)return -1;
    s_hall_pos_count=0;
    memset(s_hall_pos_hash_used,0,sizeof(s_hall_pos_hash_used));
    for(i=0;i<count;i++){
        const TNTWonderCompleteTriangle *tri=&tris[i];
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];
            u64 key=complete_position_key(tri->node,v);
            u32 slot=(u32)(((u32)key ^ (u32)(key>>32))*2654435761U)&
                (TNT_HALL_POS_HASH_SIZE-1U),probe;
            u16 index=0xffffU;
            for(probe=0;probe<TNT_HALL_POS_HASH_SIZE;probe++){
                if(!s_hall_pos_hash_used[slot]){
                    if(s_hall_pos_count>=TNT_HALL_POS_VERT_MAX)return -1;
                    index=s_hall_pos_count++;
                    s_hall_pos_hash_used[slot]=TRUE;
                    s_hall_pos_hash_key[slot]=key;
                    s_hall_pos_hash_index[slot]=index;
                    s_hall_pos_source[index]=v;
                    s_hall_pos_node[index]=tri->node;
                    break;
                }
                if(s_hall_pos_hash_key[slot]==key){
                    index=s_hall_pos_hash_index[slot];
                    break;
                }
                slot=(slot+1U)&(TNT_HALL_POS_HASH_SIZE-1U);
            }
            if(index==0xffffU)return -1;
            s_hall_pos_map[i*3U+k]=index;
        }
    }
    s_hall_pos_wonder=wonder;
    s_hall_pos_ready=TRUE;
    return 0;
}

static void hall_pos_transform(const TNTGxmMatrix *mv) {
    u32 i;
    for(i=0;i<s_hall_pos_count;i++){
        const TNTWonderVertex *v=s_hall_pos_source[i];
        transform_point(&mv[s_hall_pos_node[i]],(f32)v->x,(f32)v->y,(f32)v->z,
            &s_hall_pos_x[i],&s_hall_pos_y[i],&s_hall_pos_z[i]);
    }
}

/* African construction scene 1292 has one 0.3-gray Borg scene light. With a
 * single scene light the original renderer installs that as ambient and adds
 * its built-in full-white +Y directional light (D_800D6208). */
static u32 shade_color_african_milestone(const TNTGxmMatrix *draw,const TNTWonderVertex *v) {
    f32 nx,ny,nz,nlen,d,c;
    transform_vector(draw,(f32)v->nx/127.0f,(f32)v->ny/127.0f,(f32)v->nz/127.0f,&nx,&ny,&nz);
    nlen=vec_len(nx,ny,nz);if(nlen>0.000001f){nx/=nlen;ny/=nlen;nz/=nlen;}
    d=ny;if(d<0.0f)d=0.0f;c=0.300000012f+d;if(c>1.0f)c=1.0f;
    return ((u32)(c*255.0f+0.5f))*0x00010101U|0xff000000U;
}

/* Scene 470 carries four Borg light resources: 466..468 are the three
 * directionals and 469 is the ambient light selected by gSPNumLights(3).
 * Their direction nodes are scene nodes 1..3 and are animated by resource
 * 471/472, so derive the light directions from the current pose rather than
 * reusing the shared two-light milestone approximation. */
static u32 shade_color_celtic_complete(const TNTGxmMatrix *draw,const TNTWonderVertex *v,
                                       const f32 nodes[TNT_CELTIC_COMPLETE_NODE_COUNT][12]) {
    static const f32 light_rgb[3][3]={
        {0.300000012f,0.314999998f,0.449999988f},
        {0.067500003f,0.073500000f,0.082500003f},
        {0.859000027f,0.828400016f,0.699500024f}
    };
    static const f32 ambient[3]={0.195999995f,0.141000003f,0.240999997f};
    f32 dir[3][3],nx,ny,nz,nlen,r=ambient[0],g=ambient[1],b=ambient[2];
    s32 q;
    transform_vector(draw,(f32)v->nx/127.0f,(f32)v->ny/127.0f,(f32)v->nz/127.0f,&nx,&ny,&nz);
    nlen=vec_len(nx,ny,nz);if(nlen>0.000001f){nx/=nlen;ny/=nlen;nz/=nlen;}
    for(q=0;q<3;q++){
        f32 dx=nodes[q+1][0],dy=nodes[q+1][1],dz=nodes[q+1][2],dl=vec_len(dx,dy,dz),dot;
        if(dl>0.000001f){dx/=dl;dy/=dl;dz/=dl;}else{dx=0.0f;dy=1.0f;dz=0.0f;}
        dir[q][0]=dx;dir[q][1]=dy;dir[q][2]=dz;
        dot=nx*dir[q][0]+ny*dir[q][1]+nz*dir[q][2];if(dot<0.0f)dot=0.0f;
        r+=dot*light_rgb[q][0];g+=dot*light_rgb[q][1];b+=dot*light_rgb[q][2];
    }
    if(r>1.0f)r=1.0f;if(g>1.0f)g=1.0f;if(b>1.0f)b=1.0f;
    if(r<0.0f)r=0.0f;if(g<0.0f)g=0.0f;if(b<0.0f)b=0.0f;
    return ((u32)(r*255.0f+0.5f))|((u32)(g*255.0f+0.5f)<<8)|
        ((u32)(b*255.0f+0.5f)<<16)|0xff000000U;
}

/* Celtic has three animated light directions. The old per-vertex shading
 * path normalizes all three directions for every triangle vertex (~6600 per
 * frame), even though they depend only on one 60Hz pose. Compute directions
 * once and memoize results by node+normal (the light environment is constant
 * within one frame). Source floating-point formula/order remains unchanged. */
static void celtic_hall_light_dirs(const f32 nodes[TNT_CELTIC_COMPLETE_NODE_COUNT][12],
                                  f32 dir[3][3]) {
    s32 q;
    for(q=0;q<3;q++){
        f32 dx=nodes[q+1][0],dy=nodes[q+1][1],dz=nodes[q+1][2];
        f32 dl=vec_len(dx,dy,dz);
        if(dl>0.000001f){dx/=dl;dy/=dl;dz/=dl;}
        else{dx=0.0f;dy=1.0f;dz=0.0f;}
        dir[q][0]=dx;dir[q][1]=dy;dir[q][2]=dz;
    }
}
static u32 shade_color_celtic_hall_cached(u8 node,const TNTGxmMatrix *draw,
        const TNTWonderVertex *v,const f32 dir[3][3]) {
    static const f32 light_rgb[3][3]={
        {0.300000012f,0.314999998f,0.449999988f},
        {0.067500003f,0.073500000f,0.082500003f},
        {0.859000027f,0.828400016f,0.699500024f}
    };
    static const f32 ambient[3]={0.195999995f,0.141000003f,0.240999997f};
    u32 key=complete_shade_key(node,v),color;
    f32 nx,ny,nz,nlen,r=ambient[0],g=ambient[1],b=ambient[2];
    s32 q;
    if(complete_shade_cache_lookup(key,&color))return color;
    transform_vector(draw,(f32)v->nx/127.0f,(f32)v->ny/127.0f,(f32)v->nz/127.0f,&nx,&ny,&nz);
    nlen=vec_len(nx,ny,nz);
    if(nlen>0.000001f){nx/=nlen;ny/=nlen;nz/=nlen;}
    for(q=0;q<3;q++){
        f32 dot=nx*dir[q][0]+ny*dir[q][1]+nz*dir[q][2];
        if(dot<0.0f)dot=0.0f;
        r+=dot*light_rgb[q][0];g+=dot*light_rgb[q][1];b+=dot*light_rgb[q][2];
    }
    if(r>1.0f)r=1.0f;if(g>1.0f)g=1.0f;if(b>1.0f)b=1.0f;
    if(r<0.0f)r=0.0f;if(g<0.0f)g=0.0f;if(b<0.0f)b=0.0f;
    color=((u32)(r*255.0f+0.5f))|((u32)(g*255.0f+0.5f)<<8)|
        ((u32)(b*255.0f+0.5f)<<16)|0xff000000U;
    complete_shade_cache_store(key,color);
    return color;
}

static void reset_geometry(void){s_complete_geometry_cache_valid=FALSE;s_gxm.tex_count=0;s_gxm.color_count=0;s_gxm.run_count=0;s_complete_untex_count=0;s_complete_fog_first=s_complete_fog_count=0;}

static int append_scene(const TNTWonderMilestoneTriangle *tris,s32 count,
                        const TNTGxmMatrix *models,s32 model_count,
                        const TNTGxmMatrix *view) {
    TNTGxmMatrix mv[9];s32 i,k;
    for(i=0;i<model_count;i++)mat_mult(&mv[i],view,&models[i]);
    for(i=0;i<count;i++){
        const TNTWonderMilestoneTriangle *tri=&tris[i];
        f32 cx[3],cy[3],cz[3];
        if(tri->node>=model_count)continue;
        /* Do not reject triangles merely because one vertex crosses the near
         * or far plane. The N64 RSP clips those polygons to the frustum; GXM
         * must see the original triangle to reproduce the visible remainder. */
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];
            transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cx[k],&cy[k],&cz[k]);
        }
        if(s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tri->texture){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;
            s_gxm.runs[s_gxm.run_count].count=0;
            s_gxm.runs[s_gxm.run_count].texture=tri->texture;
            s_gxm.run_count++;
        }
        for(k=0;k<3;k++){
            TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];
            const TNTWonderVertex *v=&tri->v[k];
            f32 base=(tri->texture<20)?8.0f:32.0f;
            f32 texel_center=(tri->texture>=25&&tri->texture<=32)?0.5f:0.0f;
            tv->x=cx[k];tv->y=cy[k];tv->z=cz[k];
            /* N64 bilerp addresses integer S/T at texel centers; normalized
             * GXM coordinates need +0.5 for the scene-1088 cloud sectors. */
            tv->u=(((f32)v->s/64.0f)+texel_center)/base;
            tv->v=(((f32)v->t/64.0f)+texel_center)/base;
        }
        s_gxm.runs[s_gxm.run_count-1].count+=3;
        if(!tri->unlit){
            if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
            for(k=0;k<3;k++){
                TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];
                cv->x=cx[k];cv->y=cy[k];cv->z=cz[k];cv->color=s_african_milestone_light_mode?shade_color_african_milestone(&models[tri->node],&tri->v[k]):shade_color(&models[tri->node],&tri->v[k]);
            }
        }
    }
    return 0;
}

static void complete_camera_view(const f32 *v,TNTGxmMatrix *view);

static void progress_projection_matrix(f32 m[16]) {
    const f32 nearz=4.0f,farz=3200.0f;
    const f32 half_h=nearz*tanf(15.0f*TNT_GXM_DEG2RAD);
    const f32 half_w=half_h*(4.0f/3.0f);
    memset(m,0,16*sizeof(f32));
    m[0]=nearz/half_w;m[5]=nearz/half_h;
    m[10]=-(farz+nearz)/(farz-nearz);
    m[14]=(-2.0f*farz*nearz)/(farz-nearz);
    m[11]=-1.0f;
}

static u32 shade_color_progress(const TNTGxmMatrix *draw,const TNTWonderVertex *v) {
    f32 nx,ny,nz,len,dot,shade;
    transform_vector(draw,(f32)v->nx/127.0f,(f32)v->ny/127.0f,(f32)v->nz/127.0f,&nx,&ny,&nz);
    len=vec_len(nx,ny,nz);if(len>0.000001f){nx/=len;ny/=len;nz/=len;}
    dot=ny*(120.0f/127.0f);if(dot<0.0f)dot=0.0f;
    shade=(127.0f/255.0f)+dot;if(shade>1.0f)shade=1.0f;
    {u32 q=(u32)(shade*255.0f+0.5f);return q|(q<<8)|(q<<16)|0xff000000U;}
}

static int build_progress_geometry(u16 anim_tick) {
    TNTGxmMatrix root,view,local,world[18],draw[18],mv[18];
    f32 root_values[9];s32 i,k;
    reset_geometry();
    memcpy(root_values,g_tnt_wonder_mayan_nodes[17],sizeof(root_values));
    root_values[0]=-1.40625f-0.703125f*(f32)(anim_tick&511U);
    node_type2(root_values,&root);
    /* The generic progress scene's node 0 is the same source type-4 camera
     * layout consumed by complete_camera_view (only fields 0..6 are used). */
    complete_camera_view(g_tnt_wonder_mayan_nodes[0],&view);
    world[17]=root;
    for(i=1;i<=16;i++){
        node_type2(g_tnt_wonder_mayan_nodes[i],&local);
        mat_mult(&world[i],&root,&local);
        make_draw_model(&world[i],0.125f,&draw[i]);
        mat_mult(&mv[i],&view,&draw[i]);
    }
    for(i=0;i<568;i++){
        const TNTWonderTriangle *tri=&g_tnt_wonder_mayan_triangles[i];
        if(tri->node==0||tri->node>16||tri->texture>=20)return -1;
        if(s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS||s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tri->texture){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;
            s_gxm.runs[s_gxm.run_count].count=0;
            s_gxm.runs[s_gxm.run_count].texture=tri->texture;
            s_gxm.run_count++;
        }
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];
            TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];
            TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];
            transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&tv->x,&tv->y,&tv->z);
            tv->u=((f32)v->s/64.0f)/8.0f;tv->v=((f32)v->t/64.0f)/8.0f;
            cv->x=tv->x;cv->y=tv->y;cv->z=tv->z;
            cv->color=shade_color_progress(&draw[tri->node],v);
        }
        s_gxm.runs[s_gxm.run_count-1].count+=3;
    }
    return 0;
}

static int build_geometry(u8 milestone_index,u16 elapsed,const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    TNTGxmMatrix view,m1088[4],m1104[8],m1006[9],m1057[2];const TNTWonderMilestoneBlock *blocks;s32 i;u8 block_count;
    if(milestone_index>6)return -1;
    blocks=g_tnt_mayan_milestone_blocks[milestone_index];block_count=g_tnt_mayan_milestone_block_count[milestone_index];
    reset_geometry();camera_view(elapsed,&view);
    prepare_models_1088(m1088);if(append_scene(g_tnt_milestone_scene1088_triangles,126,m1088,4,&view)<0)return -1;
    prepare_models_1104(m1104,milestone_index);if(append_scene(g_tnt_milestone_scene1104_triangles,212,m1104,8,&view)<0)return -1;
    for(i=0;i<block_count;i++){
        const TNTWonderMilestoneBlock *b=&blocks[i];
        if(elapsed>=(u16)(b->start_tick+190U)){
            prepare_models_1057(m1057,b->x,b->y,b->z,(f32)block_rotation_x[i]);
            if(append_scene(g_tnt_milestone_scene1057_triangles,10,m1057,2,&view)<0)return -1;
        }else{
            u16 lt=0;if(elapsed>=b->start_tick)lt=(u16)(elapsed-b->start_tick+1U);
            prepare_models_1006(m1006,b->x,b->y,b->z,(f32)block_rotation_x[i],lt);
            if(append_scene(g_tnt_milestone_scene1006_triangles,244,m1006,9,&view)<0)return -1;
        }
    }
    return 0;
}

static int build_greek_geometry(u8 greek_stage,u16 elapsed,
                                const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    TNTGxmMatrix view,m1088[4],m1125[8],m1006[9],m1057[2];
    const TNTWonderMilestoneBlock *blocks;s32 i;u8 block_count;
    if(greek_stage>6)return -1;
    blocks=g_tnt_greek_milestone_blocks[greek_stage];
    block_count=g_tnt_greek_milestone_block_count[greek_stage];
    reset_geometry();greek_camera_view(elapsed,&view);
    prepare_models_1088(m1088);
    if(append_scene(g_tnt_milestone_scene1088_triangles,126,m1088,4,&view)<0)return -1;
    prepare_models_1125(m1125,greek_stage);
    if(append_scene(g_tnt_greek_scene1125_triangles,432,m1125,8,&view)<0)return -1;
    for(i=0;i<block_count;i++){
        const TNTWonderMilestoneBlock *b=&blocks[i];
        if(elapsed>=(u16)(b->start_tick+190U)){
            prepare_models_1057(m1057,b->x,b->y,b->z,(f32)block_rotation_x[i]);
            if(append_scene(g_tnt_milestone_scene1057_triangles,10,m1057,2,&view)<0)return -1;
        }else{
            u16 lt=0;if(elapsed>=b->start_tick)lt=(u16)(elapsed-b->start_tick+1U);
            prepare_models_1006(m1006,b->x,b->y,b->z,(f32)block_rotation_x[i],lt);
            if(append_scene(g_tnt_milestone_scene1006_triangles,244,m1006,9,&view)<0)return -1;
        }
    }
    return 0;
}

static int build_egyptian_geometry(u8 egyptian_stage,u16 elapsed,
                                     const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    TNTGxmMatrix view,m1088[4],m1148[8],m1006[9],m1057[2];
    const TNTWonderMilestoneBlock *blocks;s32 i;u8 block_count;
    if(egyptian_stage>6)return -1;
    blocks=g_tnt_egyptian_milestone_blocks[egyptian_stage];
    block_count=g_tnt_egyptian_milestone_block_count[egyptian_stage];
    reset_geometry();egyptian_camera_view(elapsed,&view);
    prepare_models_1088(m1088);
    if(append_scene(g_tnt_milestone_scene1088_triangles,126,m1088,4,&view)<0)return -1;
    prepare_models_1148(m1148,egyptian_stage);
    if(append_scene(g_tnt_egyptian_scene1148_triangles,486,m1148,8,&view)<0)return -1;
    for(i=0;i<block_count;i++){
        const TNTWonderMilestoneBlock *b=&blocks[i];
        if(elapsed>=(u16)(b->start_tick+190U)){
            prepare_models_1057(m1057,b->x,b->y,b->z,(f32)block_rotation_x[i]);
            if(append_scene(g_tnt_milestone_scene1057_triangles,10,m1057,2,&view)<0)return -1;
        }else{
            u16 lt=0;if(elapsed>=b->start_tick)lt=(u16)(elapsed-b->start_tick+1U);
            prepare_models_1006(m1006,b->x,b->y,b->z,(f32)block_rotation_x[i],lt);
            if(append_scene(g_tnt_milestone_scene1006_triangles,244,m1006,9,&view)<0)return -1;
        }
    }
    return 0;
}

static int build_celtic_milestone_geometry(u8 celtic_stage,u16 elapsed,
                                     const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    TNTGxmMatrix view,m1088[4],m1244[9],m1006[9],m1057[2];
    const TNTWonderMilestoneBlock *blocks;s32 i;u8 block_count;
    if(celtic_stage>6)return -1;
    blocks=g_tnt_celtic_milestone_blocks[celtic_stage];block_count=g_tnt_celtic_milestone_block_count[celtic_stage];
    reset_geometry();celtic_milestone_camera_view(elapsed,&view);
    prepare_models_1088(m1088);if(append_scene(g_tnt_milestone_scene1088_triangles,126,m1088,4,&view)<0)return -1;
    prepare_models_1244(m1244,celtic_stage);if(append_scene(g_tnt_celtic_scene1244_triangles,530,m1244,9,&view)<0)return -1;
    for(i=0;i<block_count;i++){const TNTWonderMilestoneBlock *b=&blocks[i];
        if(elapsed>=(u16)(b->start_tick+190U)){prepare_models_1057(m1057,b->x,b->y,b->z,(f32)block_rotation_x[i]);if(append_scene(g_tnt_milestone_scene1057_triangles,10,m1057,2,&view)<0)return -1;}
        else{u16 lt=0;if(elapsed>=b->start_tick)lt=(u16)(elapsed-b->start_tick+1U);prepare_models_1006(m1006,b->x,b->y,b->z,(f32)block_rotation_x[i],lt);if(append_scene(g_tnt_milestone_scene1006_triangles,244,m1006,9,&view)<0)return -1;}
    }
    return 0;
}

static int build_african_milestone_geometry(u8 african_stage,u16 elapsed,
                                     const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    TNTGxmMatrix view,m1088[4],m1292[8],m1006[9],m1057[2];
    const TNTWonderMilestoneBlock *blocks;s32 i;u8 block_count;
    if(african_stage>6)return -1;
    blocks=g_tnt_african_milestone_blocks[african_stage];block_count=g_tnt_african_milestone_block_count[african_stage];
    reset_geometry();african_milestone_camera_view(elapsed,&view);
    prepare_models_1088(m1088);if(append_scene(g_tnt_milestone_scene1088_triangles,126,m1088,4,&view)<0)return -1;
    prepare_models_1292(m1292,african_stage);s_african_milestone_light_mode=1;
    if(append_scene(g_tnt_african_scene1292_triangles,1758,m1292,8,&view)<0){s_african_milestone_light_mode=0;return -1;}
    s_african_milestone_light_mode=0;
    for(i=0;i<block_count;i++){const TNTWonderMilestoneBlock *b=&blocks[i];
        if(elapsed>=(u16)(b->start_tick+190U)){prepare_models_1057(m1057,b->x,b->y,b->z,(f32)block_rotation_x[i]);if(append_scene(g_tnt_milestone_scene1057_triangles,10,m1057,2,&view)<0)return -1;}
        else{u16 lt=0;if(elapsed>=b->start_tick)lt=(u16)(elapsed-b->start_tick+1U);prepare_models_1006(m1006,b->x,b->y,b->z,(f32)block_rotation_x[i],lt);if(append_scene(g_tnt_milestone_scene1006_triangles,244,m1006,9,&view)<0)return -1;}
    }
    return 0;
}

static int build_russian_milestone_geometry(u8 russian_stage,u16 elapsed,
                                     const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    TNTGxmMatrix view,m1088[4],m1292[8],m1006[9],m1057[2];
    const TNTWonderMilestoneBlock *blocks;s32 i;u8 block_count;
    if(russian_stage>6)return -1;
    blocks=g_tnt_african_milestone_blocks[russian_stage];block_count=g_tnt_african_milestone_block_count[russian_stage];
    reset_geometry();russian_milestone_camera_view(russian_stage,elapsed,&view);
    prepare_models_1088(m1088);if(append_scene(g_tnt_milestone_scene1088_triangles,126,m1088,4,&view)<0)return -1;
    prepare_models_1292(m1292,russian_stage);s_african_milestone_light_mode=1;
    if(append_scene(g_tnt_african_scene1292_triangles,1758,m1292,8,&view)<0){s_african_milestone_light_mode=0;return -1;}
    s_african_milestone_light_mode=0;
    for(i=0;i<block_count;i++){const TNTWonderMilestoneBlock *b=&blocks[i];
        if(elapsed>=(u16)(b->start_tick+190U)){prepare_models_1057(m1057,b->x,b->y,b->z,(f32)block_rotation_x[i]);if(append_scene(g_tnt_milestone_scene1057_triangles,10,m1057,2,&view)<0)return -1;}
        else{u16 lt=0;if(elapsed>=b->start_tick)lt=(u16)(elapsed-b->start_tick+1U);prepare_models_1006(m1006,b->x,b->y,b->z,(f32)block_rotation_x[i],lt);if(append_scene(g_tnt_milestone_scene1006_triangles,244,m1006,9,&view)<0)return -1;}
    }
    return 0;
}

static void complete_projection_matrix(f32 m[16]) {
    const f32 nearz=s_complete_near,farz=s_complete_far;
    const f32 half_h=nearz*tanf((s_complete_fov*0.5f)*TNT_GXM_DEG2RAD);
    const f32 half_w=half_h*s_complete_aspect;
    memset(m,0,16*sizeof(f32));
    m[0]=nearz/half_w;m[5]=nearz/half_h;
    m[10]=-(farz+nearz)/(farz-nearz);
    m[14]=(-2.0f*farz*nearz)/(farz-nearz);
    m[11]=-1.0f;
}

static f32 complete_fog_factor_planes(f32 view_z,f32 fog_min,f32 fog_max,f32 nearz,f32 farz) {
    f32 clipz,w,ndc,depth;
    if(view_z>=-nearz||fog_max<=fog_min)return 0.0f;
    clipz=(-(farz+nearz)/(farz-nearz))*view_z+
        (-2.0f*farz*nearz)/(farz-nearz);
    w=-view_z;if(w<=0.000001f)return 0.0f;
    ndc=clipz/w;depth=(ndc*0.5f+0.5f)*1000.0f;
    depth=(depth-fog_min)/(fog_max-fog_min);if(depth<0.0f)depth=0.0f;if(depth>1.0f)depth=1.0f;return depth;
}

static f32 complete_fog_factor(f32 view_z,f32 fog_min,f32 fog_max) {
    return complete_fog_factor_planes(view_z,fog_min,fog_max,s_complete_near,s_complete_far);
}

static void complete_camera_view(const f32 *v,TNTGxmMatrix *view) {
    f32 ex=v[0],ey=v[1],ez=v[2],tx=v[3],ty=v[4],tz=v[5];
    f32 fx=ex-tx,fy=ey-ty,fz=ez-tz;
    f32 bx,by,bz,ux,uy,uz,rx,ry,rz;
    f32 c=cosf(v[6]*TNT_GXM_DEG2RAD),sn=sinf(v[6]*TNT_GXM_DEG2RAD);
    if(fx==0.0f&&fy==0.0f&&fz==0.0f)fz=-1.0f;
    normalize3(&fx,&fy,&fz);
    bx=fz;by=0.0f;bz=-fx;normalize3(&bx,&by,&bz);
    ux=(fy*bz)-(by*fz);uy=(fz*bx)-(bz*fx);uz=(fx*by)-(bx*fy);
    rx=bx*c+ux*sn;ry=by*c+uy*sn;rz=bz*c+uz*sn;
    ux=-bx*sn+ux*c;uy=-by*sn+uy*c;uz=-bz*sn+uz*c;
    mat_identity(view);
    view->m[0][0]=rx;view->m[0][1]=ry;view->m[0][2]=rz;
    view->m[1][0]=ux;view->m[1][1]=uy;view->m[1][2]=uz;
    view->m[2][0]=fx;view->m[2][1]=fy;view->m[2][2]=fz;
    ex*=16.0f;ey*=16.0f;ez*=16.0f;
    view->m[0][3]=-(rx*ex+ry*ey+rz*ez);
    view->m[1][3]=-(ux*ex+uy*ey+uz*ez);
    view->m[2][3]=-(fx*ex+fy*ey+fz*ez);
}

static void complete_world_node(s32 idx,const f32 nodes[99][12],TNTGxmMatrix world[99],u8 done[99]) {
    TNTGxmMatrix local;
    s32 parent;
    if(done[idx])return;
    node_type2(nodes[idx],&local);
    parent=g_tnt_wonder_complete_parents[idx];
    if(parent>=0 && parent<99 && g_tnt_wonder_complete_node_type[parent]==2){
        complete_world_node(parent,nodes,world,done);
        mat_mult(&world[idx],&world[parent],&local);
    }else{
        world[idx]=local;
    }
    done[idx]=1;
}

static int build_complete_geometry(u8 anim_mode,u32 anim_ticks) {
    TNTGxmMatrix view,world[99],draw[99],mv[99];
    f32 nodes[99][12];
    u8 done[99],hall_mapped=FALSE;s32 i,k;
    TNT_Vita_WonderCompleteBuildPose(anim_mode,anim_ticks,nodes);
    memset(done,0,sizeof(done));
    complete_camera_view(nodes[0],&view);
    for(i=0;i<99;i++){
        if(g_tnt_wonder_complete_node_mesh[i]!=255){
            complete_world_node(i,nodes,world,done);
            make_draw_model(&world[i],g_tnt_wonder_complete_mesh_scale[g_tnt_wonder_complete_node_mesh[i]],&draw[i]);
            mat_mult(&mv[i],&view,&draw[i]);
        }
    }
    reset_geometry();
    if(anim_mode==TNT_WONDER_COMPLETE_ANIM_HALL){
        complete_shade_cache_begin();
        if(hall_pos_prepare(0,g_tnt_wonder_complete_triangles,3320U)==0){
            hall_pos_transform(mv);
            hall_mapped=TRUE;
        }
    }
    for(i=0;i<3320;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_wonder_complete_triangles[i];
        const TNTWonderCompleteTexture *meta=&g_tnt_wonder_complete_textures[tri->texture];
        u8 tex=(u8)(TNT_GXM_COMPLETE_TEXTURE_BASE+tri->texture);
        f32 cx[3],cy[3],cz[3];
        if(s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];
            if(hall_mapped){
                const u16 pi=s_hall_pos_map[(u32)i*3U+(u32)k];
                cx[k]=s_hall_pos_x[pi];cy[k]=s_hall_pos_y[pi];cz[k]=s_hall_pos_z[pi];
            }else transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cx[k],&cy[k],&cz[k]);
        }
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tex){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;
            s_gxm.runs[s_gxm.run_count].count=0;
            s_gxm.runs[s_gxm.run_count].texture=tex;
            /* Scene 186 foliage/bush textures use Borg texture flag 0x20
             * (N64 TEX_EDGE). Treat them as cutouts just like the restored
             * Greek/Egyptian viewers so transparent billboard texels never
             * become solid depth/lighting/fog polygons. */
            s_gxm.runs[s_gxm.run_count].pad2=(u8)((meta->wrap&0x20U)!=0U);
            s_gxm.run_count++;
        }
        for(k=0;k<3;k++){
            TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];
            const TNTWonderVertex *v=&tri->v[k];
            tv->x=cx[k];tv->y=cy[k];tv->z=cz[k];
            tv->u=((f32)v->s/64.0f)/((f32)meta->width*(((meta->wrap&0x20U)&&(meta->wrap&0x08U))?2.0f:1.0f));
            tv->v=((f32)v->t/64.0f)/(f32)meta->height;
        }
        s_gxm.runs[s_gxm.run_count-1].count+=3;
        /* Do not redraw TEX_EDGE foliage as a solid lighting triangle. The
         * N64 alpha test discards transparent texels before shading; a full
         * polygon overlay exposes the card outline. Excluding these vertices
         * also keeps them out of the fog-copy pass below, matching the source
         * materials' no-fog behavior. */
        if(!(meta->wrap&0x20U)){
            if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
            for(k=0;k<3;k++){
                TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];
                cv->x=cx[k];cv->y=cy[k];cv->z=cz[k];
                cv->color=(anim_mode==TNT_WONDER_COMPLETE_ANIM_HALL)
                    ?shade_color_cached(tri->node,&draw[tri->node],&tri->v[k])
                    :shade_color(&draw[tri->node],&tri->v[k]);
            }
        }
    }
    /* Scene 186 fog: 995..999, RGB (0.590067,0.684236,0.693182).
     * As in the original, Hall/presentation models are unfogged; apply it only
     * to the full completed-Wonder viewer. Reuse the already-transformed
     * lighting vertices so the extra pass cannot disagree with scene geometry. */
    s_complete_fog_first=s_gxm.color_count;
    if(anim_mode==TNT_WONDER_COMPLETE_ANIM_FINAL || anim_mode==TNT_WONDER_COMPLETE_ANIM_FINALE){
        u16 shade_count=s_gxm.color_count,vi;
        for(vi=0;vi<shade_count;vi++){
            const TNTGxmColorVertex *src=&s_gxm.color_vertices[vi];
            /* Our separate alpha-overlay approximation reaches full fog a bit
             * sooner than the N64 G_FOG path in this long-range scene. Use a
             * slightly farther fog-only near plane while leaving geometry and
             * the authored 995..999/color values untouched. */
            f32 f=complete_fog_factor_planes(src->z,995.0f,999.0f,3.0f,s_complete_far);u32 a=(u32)(f*255.0f+0.5f);
            TNTGxmColorVertex *cv;
            if(s_gxm.color_count>=TNT_GXM_MAX_COLOR_VERTS)return -1;
            cv=&s_gxm.color_vertices[s_gxm.color_count++];if(a>255U)a=255U;
            cv->x=src->x;cv->y=src->y;cv->z=src->z;cv->color=0x00B1AE96U|(a<<24);
        }
    }
    s_complete_fog_count=(u16)(s_gxm.color_count-s_complete_fog_first);
    return 0;
}

static void greek_complete_world_node(s32 idx,const f32 nodes[TNT_GREEK_COMPLETE_NODE_COUNT][12],
                                      TNTGxmMatrix world[TNT_GREEK_COMPLETE_NODE_COUNT],
                                      u8 done[TNT_GREEK_COMPLETE_NODE_COUNT]) {
    TNTGxmMatrix local;s32 parent;
    if(done[idx])return;
    node_type2(nodes[idx],&local);
    parent=g_tnt_greek_complete_parents[idx];
    if(parent>=0 && parent<TNT_GREEK_COMPLETE_NODE_COUNT && g_tnt_greek_complete_node_type[parent]==2){
        greek_complete_world_node(parent,nodes,world,done);
        mat_mult(&world[idx],&world[parent],&local);
    }else world[idx]=local;
    done[idx]=1;
}

static int build_greek_complete_geometry(u8 anim_mode,u32 anim_ticks) {
    TNTGxmMatrix view,world[TNT_GREEK_COMPLETE_NODE_COUNT],draw[TNT_GREEK_COMPLETE_NODE_COUNT],mv[TNT_GREEK_COMPLETE_NODE_COUNT];
    f32 nodes[TNT_GREEK_COMPLETE_NODE_COUNT][12];u8 done[TNT_GREEK_COMPLETE_NODE_COUNT];s32 i,k;
    TNT_Vita_GreekWonderCompleteBuildPose(anim_mode,anim_ticks,nodes);
    memset(done,0,sizeof(done));complete_camera_view(nodes[0],&view);
    for(i=0;i<TNT_GREEK_COMPLETE_NODE_COUNT;i++){
        if(g_tnt_greek_complete_node_mesh[i]!=255){
            greek_complete_world_node(i,nodes,world,done);
            make_draw_model(&world[i],g_tnt_greek_complete_mesh_scale[g_tnt_greek_complete_node_mesh[i]],&draw[i]);
            mat_mult(&mv[i],&view,&draw[i]);
        }
    }
    reset_geometry();complete_shade_cache_begin();
    if(greek_complete_prepare_position_map()<0)return -1;
    greek_complete_transform_unique_positions(mv);
    greek_complete_shade_unique_normals(draw);
    for(i=0;i<TNT_GREEK_COMPLETE_TRIANGLE_COUNT;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_greek_complete_triangles[i];
        const TNTWonderCompleteTexture *meta=&g_tnt_greek_complete_textures[tri->texture];
        u16 tex=(u16)(TNT_GXM_GREEK_COMPLETE_TEXTURE_BASE+tri->texture);f32 cx[3],cy[3],cz[3];
        if(tri->node>=TNT_GREEK_COMPLETE_NODE_COUNT || s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        for(k=0;k<3;k++){u16 pi=s_greek_pos_map[(u32)i*3U+(u32)k];cx[k]=s_greek_pos_x[pi];cy[k]=s_greek_pos_y[pi];cz[k]=s_greek_pos_z[pi];}
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tex){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;s_gxm.runs[s_gxm.run_count].count=0;
            s_gxm.runs[s_gxm.run_count].texture=tex;
            /* Preserve the source RGBA16 texture's TEX_EDGE/cutout hint for
             * Greek completed-Wonder foliage.  The transparent texels must not
             * update depth, otherwise an invisible billboard still occludes
             * the temple and appears as a rectangular slab. */
            s_gxm.runs[s_gxm.run_count].pad2=(u8)((meta->wrap&0x20U)!=0U);
            s_gxm.run_count++;
        }
        for(k=0;k<3;k++){
            TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];const TNTWonderVertex *v=&tri->v[k];
            tv->x=cx[k];tv->y=cy[k];tv->z=cz[k];tv->u=((f32)v->s/64.0f)/(f32)meta->width;tv->v=((f32)v->t/64.0f)/(f32)meta->height;
        }
        s_gxm.runs[s_gxm.run_count-1].count+=3;
        /* Scene 314 foliage/billboard textures carry Borg texture flag 0x20.
         * The N64 renders these fully-opaque materials with TEX_EDGE, so
         * alpha-zero texels are discarded before depth/shading.  Our generic
         * completion lighting pass redraws the entire source triangle and was
         * therefore exposing the transparent billboard rectangle around each
         * tree.  Keep the texture cutout itself, but do not apply the full-quad
         * multiply overlay to source TEX_EDGE materials. */
        if(!(meta->wrap&0x20U)){
            if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
            for(k=0;k<3;k++){
                TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];
                u16 si=s_greek_shade_map[(u32)i*3U+(u32)k];
                cv->x=cx[k];cv->y=cy[k];cv->z=cz[k];cv->color=s_greek_shade_color[si];
            }
        }
    }
    /* Scene 314 fog: 995..999, RGB (0.706800,0.774436,0.849624).
     * The source Hall/story object has FogParams nulled, so only the full
     * Delphi viewer gets this pass. TEX_EDGE foliage is already absent from
     * color_vertices, avoiding solid fog rectangles around tree billboards. */
    s_complete_fog_first=s_gxm.color_count;
    if(anim_mode==TNT_WONDER_COMPLETE_ANIM_FINAL || anim_mode==TNT_WONDER_COMPLETE_ANIM_FINALE){
        u16 shade_pos=0;
        for(i=0;i<TNT_GREEK_COMPLETE_TRIANGLE_COUNT;i++){
            const TNTWonderCompleteTriangle *tri=&g_tnt_greek_complete_triangles[i];
            const TNTWonderCompleteTexture *meta=&g_tnt_greek_complete_textures[tri->texture];u32 alpha[3];
            if(meta->wrap&0x20U)continue;
            for(k=0;k<3;k++){
                const TNTGxmColorVertex *src=&s_gxm.color_vertices[shade_pos+k];
                f32 f=complete_fog_factor(src->z,995.0f,999.0f);alpha[k]=(u32)(f*255.0f+0.5f);if(alpha[k]>255U)alpha[k]=255U;
            }
            if((alpha[0]|alpha[1]|alpha[2])!=0U){
                if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
                for(k=0;k<3;k++){
                    const TNTGxmColorVertex *src=&s_gxm.color_vertices[shade_pos+k];TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];
                    cv->x=src->x;cv->y=src->y;cv->z=src->z;cv->color=0x00D9C5B4U|(alpha[k]<<24);
                }
            }
            shade_pos=(u16)(shade_pos+3U);
        }
    }
    s_complete_fog_count=(u16)(s_gxm.color_count-s_complete_fog_first);
    return 0;
}

static void egyptian_complete_world_node(s32 idx,const f32 nodes[TNT_EGYPTIAN_COMPLETE_NODE_COUNT][12],
                                      TNTGxmMatrix world[TNT_EGYPTIAN_COMPLETE_NODE_COUNT],
                                      u8 done[TNT_EGYPTIAN_COMPLETE_NODE_COUNT]) {
    TNTGxmMatrix local;s32 parent;
    if(done[idx])return;
    node_type2(nodes[idx],&local);
    parent=g_tnt_egyptian_complete_parents[idx];
    if(parent>=0 && parent<TNT_EGYPTIAN_COMPLETE_NODE_COUNT && g_tnt_egyptian_complete_node_type[parent]==2){
        egyptian_complete_world_node(parent,nodes,world,done);
        mat_mult(&world[idx],&world[parent],&local);
    }else world[idx]=local;
    done[idx]=1;
}

static void egyptian_spheremap_basis(const f32 camera[12],const TNTGxmMatrix *world,
                                    f32 right[3],f32 up[3]) {
    /* Canonical Borg path calls guLookAtReflect(camera eye, current node world
     * position, +Y) for materials carrying flag bit 0 (G_TEXTURE_GEN). */
    f32 lx=world->m[0][3]-camera[0];
    f32 ly=world->m[1][3]-camera[1];
    f32 lz=world->m[2][3]-camera[2];
    f32 look_len=vec_len(lx,ly,lz),rlen;
    f32 lookx,looky,lookz;
    if(look_len<0.000001f){lx=0.0f;ly=0.0f;lz=-1.0f;look_len=1.0f;}
    /* guLookAtReflect negates the eye->at vector because +Z is behind. */
    lookx=-lx/look_len;looky=-ly/look_len;lookz=-lz/look_len;
    right[0]=lookz;right[1]=0.0f;right[2]=-lookx;
    rlen=vec_len(right[0],right[1],right[2]);
    if(rlen<0.000001f){right[0]=1.0f;right[1]=0.0f;right[2]=0.0f;rlen=1.0f;}
    right[0]/=rlen;right[1]/=rlen;right[2]/=rlen;
    /* Up = Look x Right, matching ultralib guLookAtReflectF. */
    up[0]=looky*right[2]-lookz*right[1];
    up[1]=lookz*right[0]-lookx*right[2];
    up[2]=lookx*right[1]-looky*right[0];
    rlen=vec_len(up[0],up[1],up[2]);
    if(rlen>0.000001f){up[0]/=rlen;up[1]/=rlen;up[2]/=rlen;}
}

static int build_egyptian_complete_geometry(u8 anim_mode,u32 anim_ticks) {
    TNTGxmMatrix view,world[TNT_EGYPTIAN_COMPLETE_NODE_COUNT],draw[TNT_EGYPTIAN_COMPLETE_NODE_COUNT],mv[TNT_EGYPTIAN_COMPLETE_NODE_COUNT];
    f32 nodes[TNT_EGYPTIAN_COMPLETE_NODE_COUNT][12];u8 done[TNT_EGYPTIAN_COMPLETE_NODE_COUNT];s32 i,k;
    f32 env_right[TNT_EGYPTIAN_COMPLETE_NODE_COUNT][3],env_up[TNT_EGYPTIAN_COMPLETE_NODE_COUNT][3];
    u16 untex_index=0;
    TNT_Vita_EgyptianWonderCompleteBuildPose(anim_mode,anim_ticks,nodes);
    memset(done,0,sizeof(done));complete_camera_view(nodes[0],&view);
    for(i=0;i<TNT_EGYPTIAN_COMPLETE_NODE_COUNT;i++){
        if(g_tnt_egyptian_complete_node_mesh[i]!=255){
            egyptian_complete_world_node(i,nodes,world,done);
            make_draw_model(&world[i],g_tnt_egyptian_complete_mesh_scale[g_tnt_egyptian_complete_node_mesh[i]],&draw[i]);
            mat_mult(&mv[i],&view,&draw[i]);
            egyptian_spheremap_basis(nodes[0],&world[i],env_right[i],env_up[i]);
        }
    }
    reset_geometry();complete_shade_cache_begin();complete_position_cache_begin();

    /* Scene 374 begins with 30 texture-off triangles. Twenty-eight form the
     * large authored Egyptian sky enclosure and two form a small black source
     * polygon. Keep them in the canonical untextured color path: texture 255
     * is a Borg/N64 "no texture bound" sentinel, never a GXM texture index. */
    for(i=0;i<TNT_EGYPTIAN_COMPLETE_TRIANGLE_COUNT;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_egyptian_complete_triangles[i];
        u16 color_index;f32 cx[3],cy[3],cz[3];
        if(tri->texture!=255U)continue;
        color_index=untex_index++;
        if(color_index>=TNT_EGYPTIAN_COMPLETE_UNTEXTURED_TRIANGLE_COUNT || tri->node>=TNT_EGYPTIAN_COMPLETE_NODE_COUNT)return -1;
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];
            complete_transform_point_cached(tri->node,&mv[tri->node],v,&cx[k],&cy[k],&cz[k]);
        }
        if(cz[0]>=-2.0f && cz[1]>=-2.0f && cz[2]>=-2.0f)continue;
        if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
        for(k=0;k<3;k++){
            TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];
            cv->x=cx[k];cv->y=cy[k];cv->z=cz[k];
            cv->color=g_tnt_egyptian_complete_untextured_colors[color_index][k];
        }
    }
    if(untex_index!=TNT_EGYPTIAN_COMPLETE_UNTEXTURED_TRIANGLE_COUNT)return -1;
    s_complete_untex_count=s_gxm.color_count;

    for(i=0;i<TNT_EGYPTIAN_COMPLETE_TRIANGLE_COUNT;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_egyptian_complete_triangles[i];
        const TNTWonderCompleteTexture *meta;
        u16 tex;f32 cx[3],cy[3],cz[3];
        if(tri->texture==255U)continue;
        if(tri->node>=TNT_EGYPTIAN_COMPLETE_NODE_COUNT || tri->texture>=TNT_EGYPTIAN_COMPLETE_TEXTURE_COUNT || s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        meta=&g_tnt_egyptian_complete_textures[tri->texture];
        tex=(u16)(TNT_GXM_EGYPTIAN_COMPLETE_TEXTURE_BASE+tri->texture);
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];
            complete_transform_point_cached(tri->node,&mv[tri->node],v,&cx[k],&cy[k],&cz[k]);
        }
        /* Resource 375 drives the camera through/around the completed Sphinx.
         * Discard only polygons wholly behind the near plane; GXM clips the
         * source triangles that merely cross it. */
        if(cz[0]>=-2.0f && cz[1]>=-2.0f && cz[2]>=-2.0f)continue;
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tex){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;s_gxm.runs[s_gxm.run_count].count=0;
            s_gxm.runs[s_gxm.run_count].texture=tex;
            /* Borg texture flag 0x20 denotes the TEX_EDGE/cutout path. */
            s_gxm.runs[s_gxm.run_count].pad2=(u8)((meta->wrap&0x20U)!=0U);
            s_gxm.run_count++;
        }
        for(k=0;k<3;k++){
            TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];const TNTWonderVertex *v=&tri->v[k];
            tv->x=cx[k];tv->y=cy[k];tv->z=cz[k];
            if(tri->pad[0]&1U){
                f32 nx,ny,nz,nlen,du,dv;
                transform_vector(&world[tri->node],(f32)v->nx/127.0f,(f32)v->ny/127.0f,(f32)v->nz/127.0f,&nx,&ny,&nz);
                nlen=vec_len(nx,ny,nz);if(nlen>0.000001f){nx/=nlen;ny/=nlen;nz/=nlen;}
                du=nx*env_right[tri->node][0]+ny*env_right[tri->node][1]+nz*env_right[tri->node][2];
                dv=nx*env_up[tri->node][0]+ny*env_up[tri->node][1]+nz*env_up[tri->node][2];
                if(du<-1.0f)du=-1.0f;if(du>1.0f)du=1.0f;if(dv<-1.0f)dv=-1.0f;if(dv>1.0f)dv=1.0f;
                /* F3DEX2 G_TEXTURE_GEN (without LINEAR) sphere map. */
                tv->u=0.5f*(du+1.0f);tv->v=0.5f*(dv+1.0f);
            }else{
                tv->u=((f32)v->s/64.0f)/((f32)meta->width*((tri->texture==12U||tri->texture==13U)?2.0f:1.0f));
                tv->v=((f32)v->t/64.0f)/(f32)meta->height;
            }
        }
        s_gxm.runs[s_gxm.run_count-1].count+=3;
        /* Do not redraw source TEX_EDGE quads as solid lighting polygons; that
         * would expose the transparent rectangle surrounding a cutout. */
        if(!(meta->wrap&0x20U)){
            if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
            for(k=0;k<3;k++){
                TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];
                cv->x=cx[k];cv->y=cy[k];cv->z=cz[k];cv->color=shade_color_cached(tri->node,&draw[tri->node],&tri->v[k]);
            }
        }
    }

    /* Scene 374 carries canonical N64 fog 995..999, RGB
     * (0.781955, 0.736842, 0.571429). The source explicitly NULLs FogParams
     * both for the small black-background story model and for Wonders Hall
     * display objects, so fog belongs only to the full finished Sphinx viewer.
     * TEX_EDGE triangles remain excluded because a solid fog overlay would
     * reveal their transparent billboard rectangle. */
    s_complete_fog_first=s_gxm.color_count;
    if(anim_mode==TNT_WONDER_COMPLETE_ANIM_FINAL || anim_mode==TNT_WONDER_COMPLETE_ANIM_FINALE) for(i=0;i<TNT_EGYPTIAN_COMPLETE_TRIANGLE_COUNT;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_egyptian_complete_triangles[i];
        const TNTWonderCompleteTexture *meta=NULL;f32 cx[3],cy[3],cz[3];
        if(tri->node>=TNT_EGYPTIAN_COMPLETE_NODE_COUNT)return -1;
        if(tri->texture!=255U){
            if(tri->texture>=TNT_EGYPTIAN_COMPLETE_TEXTURE_COUNT)return -1;
            meta=&g_tnt_egyptian_complete_textures[tri->texture];
            if(meta->wrap&0x20U)continue;
        }
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];
            complete_transform_point_cached(tri->node,&mv[tri->node],v,&cx[k],&cy[k],&cz[k]);
        }
        if(cz[0]>=-s_complete_near&&cz[1]>=-s_complete_near&&cz[2]>=-s_complete_near)continue;
        {
            u32 alpha[3];
            for(k=0;k<3;k++){
                /* Keep the approved 1.0 projection near plane so the opening
                 * boat does not clip, but use Borg's 2.0 minimum for fog depth. */
                f32 f=complete_fog_factor_planes(cz[k],995.0f,999.0f,2.0f,s_complete_far);
                alpha[k]=(u32)(f*255.0f+0.5f);if(alpha[k]>255U)alpha[k]=255U;
            }
            /* A depth-equal alpha pass with zero alpha at all three vertices is
             * mathematically invisible. Do not stage or submit those triangles. */
            if((alpha[0]|alpha[1]|alpha[2])==0U)continue;
            if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
            for(k=0;k<3;k++){
                TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];
                cv->x=cx[k];cv->y=cy[k];cv->z=cz[k];
                cv->color=0x0092BCC7U|(alpha[k]<<24); /* R=199,G=188,B=146 in ABGR byte layout */
            }
        }
    }
    s_complete_fog_count=(u16)(s_gxm.color_count-s_complete_fog_first);
    return 0;
}

static void celtic_complete_world_node(s32 idx,const f32 nodes[TNT_CELTIC_COMPLETE_NODE_COUNT][12],
                                      TNTGxmMatrix world[TNT_CELTIC_COMPLETE_NODE_COUNT],
                                      u8 done[TNT_CELTIC_COMPLETE_NODE_COUNT]) {
    TNTGxmMatrix local;s32 parent;
    if(done[idx])return;
    node_type2(nodes[idx],&local);
    parent=g_tnt_celtic_complete_parents[idx];
    if(parent>=0 && parent<TNT_CELTIC_COMPLETE_NODE_COUNT && g_tnt_celtic_complete_node_type[parent]==2){
        celtic_complete_world_node(parent,nodes,world,done);
        mat_mult(&world[idx],&world[parent],&local);
    }else world[idx]=local;
    done[idx]=1;
}

static u8 celtic_complete_material_alpha(f32 transparency) {
    s32 a=(s32)((1.0f-transparency)*255.0f);
    if(a<0)a=0;
    if(a>255)a=255;
    return (u8)a;
}

static void celtic_complete_apply_effect_alpha(const f32 material[6][4]) {
    /* FX 428/429/430 share texture 54 and identical alpha tracks in all three
     * Celtic animations. FX 431, 433, 434 own textures 0, 6, 53. The source
     * textures themselves are RGBA16-opaque; Borg multiplies their alpha by
     * (1 - animated material transparency) at draw time. Updating these four
     * tiny 32x32 texture copies reproduces that combiner without a new shader. */
    static const u8 slot[4]={54U,0U,6U,53U};
    static const u8 group[4]={0U,3U,4U,5U};
    u32 *pack=(u32*)s_gxm.celtic_complete_texture_pack;
    s32 q;
    if(!pack)return;
    for(q=0;q<4;q++){
        const TNTWonderCompleteTexture *meta=&g_tnt_celtic_complete_textures[slot[q]];
        u32 *dst=pack+meta->offset;
        u8 alpha=celtic_complete_material_alpha(material[group[q]][3]);
        s32 x,y;
        for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
            s32 sx=(meta->pad[0]&&meta->width>=4U)?(x^((y&1)?2:0)):x;
            u16 source=g_tnt_celtic_complete_texels[
                meta->offset+(u32)y*(u32)meta->width+(u32)sx];
            u32 a=(source&1U)?(u32)alpha:0U;
            dst[(u32)y*(u32)meta->width+(u32)x]=
                (dst[(u32)y*(u32)meta->width+(u32)x]&0x00ffffffU)|(a<<24);
        }
    }
}

static int build_celtic_complete_geometry(u8 anim_mode,u32 anim_ticks) {
    TNTGxmMatrix view,world[TNT_CELTIC_COMPLETE_NODE_COUNT],draw[TNT_CELTIC_COMPLETE_NODE_COUNT],mv[TNT_CELTIC_COMPLETE_NODE_COUNT];
    f32 nodes[TNT_CELTIC_COMPLETE_NODE_COUNT][12];u8 done[TNT_CELTIC_COMPLETE_NODE_COUNT];s32 i,k;
    f32 effect_material[6][4],hall_light[3][3];
    u8 effect_texture[3],hall_mapped=FALSE;u16 untex_index=0;
    TNT_Vita_CelticWonderCompleteBuildPose(anim_mode,anim_ticks,nodes);
    TNT_Vita_CelticWonderCompleteEffectState(anim_mode,anim_ticks,effect_material,effect_texture);
    celtic_complete_apply_effect_alpha(effect_material);
    memset(done,0,sizeof(done));complete_camera_view(nodes[0],&view);
    for(i=0;i<TNT_CELTIC_COMPLETE_NODE_COUNT;i++){
        if(g_tnt_celtic_complete_node_mesh[i]!=255){
            celtic_complete_world_node(i,nodes,world,done);
            make_draw_model(&world[i],g_tnt_celtic_complete_mesh_scale[g_tnt_celtic_complete_node_mesh[i]],&draw[i]);
            mat_mult(&mv[i],&view,&draw[i]);
        }
    }
    reset_geometry();
    if(anim_mode==TNT_WONDER_COMPLETE_ANIM_HALL){
        celtic_hall_light_dirs(nodes,hall_light);
        complete_shade_cache_begin();
        if(hall_pos_prepare(3,g_tnt_celtic_complete_triangles,
                            TNT_CELTIC_COMPLETE_TRIANGLE_COUNT)==0){
            hall_pos_transform(mv);
            hall_mapped=TRUE;
        }
    }
    memset(s_celtic_run_shade_first,0,sizeof(s_celtic_run_shade_first));
    memset(s_celtic_run_shade_count,0,sizeof(s_celtic_run_shade_count));
    for(i=0;i<TNT_CELTIC_COMPLETE_TRIANGLE_COUNT;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_celtic_complete_triangles[i];
        u16 color_index;f32 cx[3],cy[3],cz[3];
        if(tri->texture!=255U)continue;
        color_index=untex_index++;
        if(color_index>=TNT_CELTIC_COMPLETE_UNTEXTURED_TRIANGLE_COUNT || tri->node>=TNT_CELTIC_COMPLETE_NODE_COUNT)return -1;
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];
            if(hall_mapped){
                const u16 pi=s_hall_pos_map[(u32)i*3U+(u32)k];
                cx[k]=s_hall_pos_x[pi];cy[k]=s_hall_pos_y[pi];cz[k]=s_hall_pos_z[pi];
            }else transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cx[k],&cy[k],&cz[k]);
        }
        if(cz[0]>=-s_complete_near && cz[1]>=-s_complete_near && cz[2]>=-s_complete_near)continue;
        if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
        for(k=0;k<3;k++){
            TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];cv->x=cx[k];cv->y=cy[k];cv->z=cz[k];
            cv->color=g_tnt_celtic_complete_untextured_colors[color_index][k];
        }
    }
    if(untex_index!=TNT_CELTIC_COMPLETE_UNTEXTURED_TRIANGLE_COUNT)return -1;
    s_complete_untex_count=s_gxm.color_count;
    for(i=0;i<TNT_CELTIC_COMPLETE_TRIANGLE_COUNT;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_celtic_complete_triangles[i];
        const TNTWonderCompleteTexture *meta;u16 tex;f32 cx[3],cy[3],cz[3];
        u8 source_tex=tri->texture,resolved_tex,group,no_depth,run_class;
        if(source_tex==255U)continue;
        if(source_tex>=TNT_CELTIC_COMPLETE_DYNAMIC_TEXTURE_BASE && source_tex<TNT_CELTIC_COMPLETE_DYNAMIC_TEXTURE_BASE+3U)
            resolved_tex=effect_texture[source_tex-TNT_CELTIC_COMPLETE_DYNAMIC_TEXTURE_BASE];
        else resolved_tex=source_tex;
        if(tri->node>=TNT_CELTIC_COMPLETE_NODE_COUNT || resolved_tex>=TNT_CELTIC_COMPLETE_TEXTURE_COUNT || s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        meta=&g_tnt_celtic_complete_textures[resolved_tex];
        tex=(u16)(TNT_GXM_CELTIC_COMPLETE_TEXTURE_BASE+resolved_tex);
        group=(u8)(tri->pad[1]&TNT_CELTIC_COMPLETE_EFFECT_GROUP_MASK);
        no_depth=(u8)((tri->pad[1]&TNT_CELTIC_COMPLETE_NO_DEPTH)!=0U);
        run_class=(u8)((group?1U:0U)|(no_depth?2U:0U));
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];
            if(hall_mapped){
                const u16 pi=s_hall_pos_map[(u32)i*3U+(u32)k];
                cx[k]=s_hall_pos_x[pi];cy[k]=s_hall_pos_y[pi];cz[k]=s_hall_pos_z[pi];
            }else transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cx[k],&cy[k],&cz[k]);
        }
        if(cz[0]>=-s_complete_near && cz[1]>=-s_complete_near && cz[2]>=-s_complete_near)continue;
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tex||
           s_gxm.runs[s_gxm.run_count-1].pad!=run_class||
           s_gxm.runs[s_gxm.run_count-1].pad2!=(u8)((meta->wrap&0x20U)!=0U)){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;s_gxm.runs[s_gxm.run_count].count=0;
            s_gxm.runs[s_gxm.run_count].texture=tex;s_gxm.runs[s_gxm.run_count].pad=run_class;
            s_gxm.runs[s_gxm.run_count].pad2=(u8)((meta->wrap&0x20U)!=0U);
            s_celtic_run_shade_first[s_gxm.run_count]=s_gxm.color_count;
            s_celtic_run_shade_count[s_gxm.run_count]=0;
            s_gxm.run_count++;
        }
        for(k=0;k<3;k++){
            TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];const TNTWonderVertex *v=&tri->v[k];
            tv->x=cx[k];tv->y=cy[k];tv->z=cz[k];
            tv->u=((f32)v->s/64.0f)/((f32)meta->width*((meta->wrap&0x08U)?2.0f:1.0f));
            tv->v=((f32)v->t/64.0f)/(f32)meta->height;
        }
        s_gxm.runs[s_gxm.run_count-1].count+=3;
        if(!(meta->wrap&0x20U)){
            if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
            for(k=0;k<3;k++){
                const TNTWonderVertex *v=&tri->v[k];TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];
                cv->x=cx[k];cv->y=cy[k];cv->z=cz[k];
                if(tri->pad[0]&0x08U)cv->color=(u32)(u8)v->nx|((u32)(u8)v->ny<<8)|((u32)(u8)v->nz<<16)|0xff000000U;
                else cv->color=(anim_mode==TNT_WONDER_COMPLETE_ANIM_HALL)
                    ?shade_color_celtic_hall_cached(tri->node,&draw[tri->node],v,hall_light)
                    :shade_color_celtic_complete(&draw[tri->node],v,nodes);
            }
            s_celtic_run_shade_count[s_gxm.run_count-1]=(u16)(s_celtic_run_shade_count[s_gxm.run_count-1]+3U);
        }
    }
    s_complete_fog_first=s_gxm.color_count;s_complete_fog_count=0;
    return 0;
}

static void african_complete_world_node(s32 idx,const f32 nodes[TNT_AFRICAN_COMPLETE_NODE_COUNT][12],
                                        TNTGxmMatrix world[TNT_AFRICAN_COMPLETE_NODE_COUNT],
                                        u8 done[TNT_AFRICAN_COMPLETE_NODE_COUNT]) {
    TNTGxmMatrix local;s32 parent;
    if(done[idx])return;
    node_type2(nodes[idx],&local);
    parent=g_tnt_african_complete_parents[idx];
    if(parent>=0 && parent<TNT_AFRICAN_COMPLETE_NODE_COUNT && g_tnt_african_complete_node_type[parent]==2){
        african_complete_world_node(parent,nodes,world,done);
        mat_mult(&world[idx],&world[parent],&local);
    }else world[idx]=local;
    done[idx]=1;
}

static void african_complete_light_dirs(const f32 nodes[TNT_AFRICAN_COMPLETE_NODE_COUNT][12],f32 out[2][3]) {
    s32 q;
    for(q=0;q<2;q++){
        f32 dx=nodes[q+1][0],dy=nodes[q+1][1],dz=nodes[q+1][2],dl=vec_len(dx,dy,dz);
        if(dl>0.000001f){dx/=dl;dy/=dl;dz/=dl;}else{dx=0.0f;dy=1.0f;dz=0.0f;}
        out[q][0]=dx;out[q][1]=dy;out[q][2]=dz;
    }
}

static u32 shade_color_african_complete(const TNTGxmMatrix *draw,const TNTWonderVertex *v,
                                         const f32 light_dir[2][3],const f32 light[3][4]) {
    f32 nx,ny,nz,nlen,r=light[2][0]*light[2][1],g=light[2][0]*light[2][2],b=light[2][0]*light[2][3];
    s32 q;
    transform_vector(draw,(f32)v->nx/127.0f,(f32)v->ny/127.0f,(f32)v->nz/127.0f,&nx,&ny,&nz);
    nlen=vec_len(nx,ny,nz);if(nlen>0.000001f){nx/=nlen;ny/=nlen;nz/=nlen;}
    for(q=0;q<2;q++){
        f32 dot=nx*light_dir[q][0]+ny*light_dir[q][1]+nz*light_dir[q][2];if(dot<0.0f)dot=0.0f;
        r+=dot*light[q][0]*light[q][1];g+=dot*light[q][0]*light[q][2];b+=dot*light[q][0]*light[q][3];
    }
    if(r>1.0f)r=1.0f;if(g>1.0f)g=1.0f;if(b>1.0f)b=1.0f;
    if(r<0.0f)r=0.0f;if(g<0.0f)g=0.0f;if(b<0.0f)b=0.0f;
    return ((u32)(r*255.0f+0.5f))|((u32)(g*255.0f+0.5f)<<8)|((u32)(b*255.0f+0.5f)<<16)|0xff000000U;
}

static u32 shade_color_african_complete_cached(u8 node,const TNTGxmMatrix *draw,const TNTWonderVertex *v,
                                                const f32 light_dir[2][3],const f32 light[3][4]) {
    u32 key=complete_shade_key(node,v),color;
    if(complete_shade_cache_lookup(key,&color))return color;
    color=shade_color_african_complete(draw,v,light_dir,light);complete_shade_cache_store(key,color);return color;
}

static void african_complete_apply_effect_alpha(const f32 material[4][4]) {
    static const u8 source_slot[4]={48U,48U,48U,47U};s32 q;
    for(q=0;q<4;q++){
        const TNTWonderCompleteTexture *meta=&g_tnt_african_complete_textures[source_slot[q]];
        u32 *dst=(u32*)s_gxm.texture_data[TNT_GXM_AFRICAN_EFFECT_TEXTURE_BASE+q];
        f32 opacity=1.0f-material[q][3];u32 alpha;s32 x,y;
        if(!dst)continue;if(opacity<0.0f)opacity=0.0f;if(opacity>1.0f)opacity=1.0f;
        alpha=(u32)(opacity*255.0f+0.5f);
        for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
            s32 sx=(meta->pad[0]&&meta->width>=4U)?(x^((y&1)?2:0)):x;
            u16 source=g_tnt_african_complete_texels[meta->offset+(u32)y*(u32)meta->width+(u32)sx];
            u32 p=rgba16_to_rgba8(source),sa=p>>24;
            dst[(u32)y*(u32)meta->width+(u32)x]=(p&0x00ffffffU)|((((sa*alpha)+127U)/255U)<<24);
        }
    }
}

static int build_african_complete_geometry(u8 anim_mode,u32 anim_ticks) {
    TNTGxmMatrix view,world[TNT_AFRICAN_COMPLETE_NODE_COUNT],draw[TNT_AFRICAN_COMPLETE_NODE_COUNT],mv[TNT_AFRICAN_COMPLETE_NODE_COUNT];
    f32 nodes[TNT_AFRICAN_COMPLETE_NODE_COUNT][12],effect_material[4][4],light[3][4],light_dir[2][3];
    u8 done[TNT_AFRICAN_COMPLETE_NODE_COUNT],effect_texture,visible[TNT_AFRICAN_COMPLETE_TRIANGLE_COUNT];u16 untex_index=0;s32 i,k;
    TNT_Vita_AfricanWonderCompleteBuildPose(anim_mode,anim_ticks,nodes);
    TNT_Vita_AfricanWonderCompleteEffectState(anim_mode,anim_ticks,effect_material,&effect_texture);
    TNT_Vita_AfricanWonderCompleteLightState(anim_mode,anim_ticks,light);
    african_complete_light_dirs(nodes,light_dir);
    african_complete_apply_effect_alpha(effect_material);
    memset(done,0,sizeof(done));complete_camera_view(nodes[0],&view);
    for(i=0;i<TNT_AFRICAN_COMPLETE_NODE_COUNT;i++){
        if(g_tnt_african_complete_node_mesh[i]!=255){
            african_complete_world_node(i,nodes,world,done);
            make_draw_model(&world[i],g_tnt_african_complete_mesh_scale[g_tnt_african_complete_node_mesh[i]],&draw[i]);
            mat_mult(&mv[i],&view,&draw[i]);
        }
    }
    reset_geometry();complete_shade_cache_begin();memset(visible,0,sizeof(visible));memset(s_african_run_shade_first,0,sizeof(s_african_run_shade_first));memset(s_african_run_shade_count,0,sizeof(s_african_run_shade_count));
    /* Scene 557's untextured commands are source-authored solid pieces. */
    for(i=0;i<TNT_AFRICAN_COMPLETE_TRIANGLE_COUNT;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_african_complete_triangles[i];u16 ci;f32 cx[3],cy[3],cz[3];
        if(tri->texture!=255U)continue;ci=untex_index++;
        if(ci>=TNT_AFRICAN_COMPLETE_UNTEXTURED_TRIANGLE_COUNT||tri->node>=TNT_AFRICAN_COMPLETE_NODE_COUNT)return -1;
        for(k=0;k<3;k++){const TNTWonderVertex *v=&tri->v[k];transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cx[k],&cy[k],&cz[k]);}
        if(cz[0]>=-s_complete_near&&cz[1]>=-s_complete_near&&cz[2]>=-s_complete_near)continue;
        visible[i]=TRUE;
        if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
        for(k=0;k<3;k++){TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];cv->x=cx[k];cv->y=cy[k];cv->z=cz[k];cv->color=g_tnt_african_complete_untextured_colors[ci][k];}
    }
    if(untex_index!=TNT_AFRICAN_COMPLETE_UNTEXTURED_TRIANGLE_COUNT)return -1;s_complete_untex_count=s_gxm.color_count;
    for(i=0;i<TNT_AFRICAN_COMPLETE_TRIANGLE_COUNT;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_african_complete_triangles[i];
        const TNTWonderCompleteTexture *meta;u16 gpu_tex;u8 source_tex=tri->texture,resolved_tex,group;f32 cx[3],cy[3],cz[3];
        if(source_tex==255U)continue;group=(u8)(tri->pad[1]&TNT_AFRICAN_COMPLETE_EFFECT_GROUP_MASK);
        if(source_tex==TNT_AFRICAN_COMPLETE_DYNAMIC_TEXTURE_BASE)resolved_tex=effect_texture;else resolved_tex=source_tex;
        if(tri->node>=TNT_AFRICAN_COMPLETE_NODE_COUNT||resolved_tex>=TNT_AFRICAN_COMPLETE_TEXTURE_COUNT||s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        meta=&g_tnt_african_complete_textures[resolved_tex];
        gpu_tex=group?(u16)(TNT_GXM_AFRICAN_EFFECT_TEXTURE_BASE+group-1U):(u16)(TNT_GXM_AFRICAN_COMPLETE_TEXTURE_BASE+resolved_tex);
        for(k=0;k<3;k++){const TNTWonderVertex *v=&tri->v[k];transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cx[k],&cy[k],&cz[k]);}
        if(cz[0]>=-s_complete_near&&cz[1]>=-s_complete_near&&cz[2]>=-s_complete_near)continue;
        visible[i]=TRUE;
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=gpu_tex||s_gxm.runs[s_gxm.run_count-1].pad!=(u8)(group?1U:0U)||s_gxm.runs[s_gxm.run_count-1].pad2!=(u8)((meta->wrap&0x20U)!=0U)){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;s_gxm.runs[s_gxm.run_count].count=0;s_gxm.runs[s_gxm.run_count].texture=gpu_tex;
            s_gxm.runs[s_gxm.run_count].pad=(u8)(group?1U:0U);s_gxm.runs[s_gxm.run_count].pad2=(u8)((meta->wrap&0x20U)!=0U);
            s_african_run_shade_first[s_gxm.run_count]=s_gxm.color_count;s_african_run_shade_count[s_gxm.run_count]=0;s_gxm.run_count++;
        }
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];
            tv->x=cx[k];tv->y=cy[k];tv->z=cz[k];tv->u=((f32)v->s/64.0f)/((f32)meta->width*((meta->wrap&0x08U)?2.0f:1.0f));tv->v=((f32)v->t/64.0f)/(f32)meta->height;
        }
        s_gxm.runs[s_gxm.run_count-1].count+=3;
        if(!(meta->wrap&0x20U)){
            if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
            for(k=0;k<3;k++){
                const TNTWonderVertex *v=&tri->v[k];TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];
                cv->x=cx[k];cv->y=cy[k];cv->z=cz[k];
                if(tri->pad[0]&0x08U)cv->color=(u32)(u8)v->nx|((u32)(u8)v->ny<<8)|((u32)(u8)v->nz<<16)|0xff000000U;
                else cv->color=shade_color_african_complete_cached(tri->node,&draw[tri->node],v,light_dir,light);
            }
            s_african_run_shade_count[s_gxm.run_count-1]=(u16)(s_african_run_shade_count[s_gxm.run_count-1]+3U);
        }
    }
    /* Hall/presentation explicitly clear FogParams in source. The finished
     * viewer keeps scene557's 995..999 warm dust fog. FINAL/FINALE used to
     * transform the complete 3044-triangle scene a second time solely for this
     * fog overlay. Reuse the positions already staged above, preserving source
     * triangle order and the exact fog equation while avoiding duplicate matrix
     * work. Fully transparent fog triangles are omitted because they cannot
     * affect color or depth. */
    s_complete_fog_first=s_gxm.color_count;s_complete_fog_count=0;
    if(anim_mode==TNT_WONDER_COMPLETE_ANIM_FINAL || anim_mode==TNT_WONDER_COMPLETE_ANIM_FINALE){
        u16 untex_pos=0,tex_pos=0;
        for(i=0;i<TNT_AFRICAN_COMPLETE_TRIANGLE_COUNT;i++){
            const TNTWonderCompleteTriangle *tri=&g_tnt_african_complete_triangles[i];
            const TNTWonderCompleteTexture *meta=NULL;f32 cx[3],cy[3],cz[3];u32 alpha[3];
            if(!visible[i])continue;
            if(tri->texture==255U){
                for(k=0;k<3;k++){const TNTGxmColorVertex *src=&s_gxm.color_vertices[untex_pos+k];cx[k]=src->x;cy[k]=src->y;cz[k]=src->z;}
                untex_pos=(u16)(untex_pos+3U);
            }else{
                u8 rt=(tri->texture==TNT_AFRICAN_COMPLETE_DYNAMIC_TEXTURE_BASE)?effect_texture:tri->texture;
                for(k=0;k<3;k++){const TNTGxmTexVertex *src=&s_gxm.tex_vertices[tex_pos+k];cx[k]=src->x;cy[k]=src->y;cz[k]=src->z;}
                tex_pos=(u16)(tex_pos+3U);
                if(rt>=TNT_AFRICAN_COMPLETE_TEXTURE_COUNT)return -1;meta=&g_tnt_african_complete_textures[rt];if(meta->wrap&0x20U)continue;
            }
            for(k=0;k<3;k++){f32 f=complete_fog_factor(cz[k],995.0f,999.0f);alpha[k]=(u32)(f*255.0f+0.5f);if(alpha[k]>255U)alpha[k]=255U;}
            if((alpha[0]|alpha[1]|alpha[2])==0U)continue;
            if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
            for(k=0;k<3;k++){
                TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];
                cv->x=cx[k];cv->y=cy[k];cv->z=cz[k];cv->color=0x0096B2C2U|(alpha[k]<<24);
            }
        }
        s_complete_fog_count=(u16)(s_gxm.color_count-s_complete_fog_first);
    }
    return 0;
}

static void japanese_complete_world_node(s32 idx,const f32 nodes[TNT_JAPANESE_COMPLETE_NODE_COUNT][12],
                                            TNTGxmMatrix world[TNT_JAPANESE_COMPLETE_NODE_COUNT],
                                            u8 done[TNT_JAPANESE_COMPLETE_NODE_COUNT]) {
    TNTGxmMatrix local;s32 parent;
    if(done[idx])return;
    node_type2(nodes[idx],&local);
    parent=g_tnt_japanese_complete_parents[idx];
    if(parent>=0&&parent<TNT_JAPANESE_COMPLETE_NODE_COUNT&&g_tnt_japanese_complete_node_type[parent]==2){
        japanese_complete_world_node(parent,nodes,world,done);mat_mult(&world[idx],&world[parent],&local);
    }else world[idx]=local;
    done[idx]=1;
}

static void japanese_complete_apply_effect_alpha(const f32 transparency[4]) {
    static const u8 source_slot[4]={1U,3U,4U,5U};s32 q;
    for(q=0;q<4;q++){
        const TNTWonderCompleteTexture *meta=&g_tnt_japanese_complete_textures[source_slot[q]];
        u32 *dst=(u32*)s_gxm.texture_data[TNT_GXM_JAPANESE_EFFECT_TEXTURE_BASE+q];
        f32 opacity=1.0f-transparency[q];u32 alpha;s32 x,y;
        if(!dst)continue;if(opacity<0.0f)opacity=0.0f;if(opacity>1.0f)opacity=1.0f;
        alpha=(u32)(opacity*255.0f+0.5f);
        for(y=0;y<(s32)meta->height;y++)for(x=0;x<(s32)meta->width;x++){
            s32 sx=(meta->pad[0]&&meta->width>=4U)?(x^((y&1)?2:0)):x;
            u16 source=g_tnt_japanese_complete_texels[meta->offset+(u32)y*(u32)meta->width+(u32)sx];
            u32 px=rgba16_to_rgba8(source),sa=px>>24;
            dst[(u32)y*(u32)meta->width+(u32)x]=(px&0x00ffffffU)|((((sa*alpha)+127U)/255U)<<24);
        }
    }
}

static int build_japanese_complete_geometry(u8 anim_mode,u32 anim_ticks) {
    TNTGxmMatrix view,world[TNT_JAPANESE_COMPLETE_NODE_COUNT],draw[TNT_JAPANESE_COMPLETE_NODE_COUNT],mv[TNT_JAPANESE_COMPLETE_NODE_COUNT];
    f32 nodes[TNT_JAPANESE_COMPLETE_NODE_COUNT][12],effect_transparency[4];u8 done[TNT_JAPANESE_COMPLETE_NODE_COUNT];u16 untex_index=0;s32 i,k;
    TNT_Vita_JapaneseWonderCompleteBuildPose(anim_mode,anim_ticks,nodes);
    if(anim_mode==TNT_WONDER_COMPLETE_ANIM_FINALE){
        TNT_Vita_JapaneseWonderCompleteEffectState(anim_mode,anim_ticks,effect_transparency);
        japanese_complete_apply_effect_alpha(effect_transparency);
    }
    memset(done,0,sizeof(done));complete_camera_view(nodes[0],&view);
    for(i=0;i<TNT_JAPANESE_COMPLETE_NODE_COUNT;i++)if(g_tnt_japanese_complete_node_mesh[i]!=255){
        japanese_complete_world_node(i,nodes,world,done);
        make_draw_model(&world[i],g_tnt_japanese_complete_mesh_scale[g_tnt_japanese_complete_node_mesh[i]],&draw[i]);
        mat_mult(&mv[i],&view,&draw[i]);
    }
    reset_geometry();memset(s_japanese_run_shade_first,0,sizeof(s_japanese_run_shade_first));memset(s_japanese_run_shade_count,0,sizeof(s_japanese_run_shade_count));
    for(i=0;i<TNT_JAPANESE_COMPLETE_TRIANGLE_COUNT;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_japanese_complete_triangles[i];u16 ci;f32 cx[3],cy[3],cz[3];
        if(tri->texture!=255U)continue;ci=untex_index++;
        if(ci>=TNT_JAPANESE_COMPLETE_UNTEXTURED_TRIANGLE_COUNT||tri->node>=TNT_JAPANESE_COMPLETE_NODE_COUNT)return -1;
        for(k=0;k<3;k++){const TNTWonderVertex *v=&tri->v[k];transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cx[k],&cy[k],&cz[k]);}
        if(cz[0]>=-s_complete_near&&cz[1]>=-s_complete_near&&cz[2]>=-s_complete_near)continue;
        if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
        for(k=0;k<3;k++){TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];cv->x=cx[k];cv->y=cy[k];cv->z=cz[k];cv->color=g_tnt_japanese_complete_untextured_colors[ci][k];}
    }
    if(untex_index!=TNT_JAPANESE_COMPLETE_UNTEXTURED_TRIANGLE_COUNT)return -1;s_complete_untex_count=s_gxm.color_count;
    for(i=0;i<TNT_JAPANESE_COMPLETE_TRIANGLE_COUNT;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_japanese_complete_triangles[i];const TNTWonderCompleteTexture *meta;u16 tex;u8 effect_group=0;f32 cx[3],cy[3],cz[3];
        if(tri->texture==255U)continue;
        /* 756/757 keep butterfly/secondary-effect materials fully transparent,
         * so omit them there. Finale 758 animates those channels and must render
         * them. Nodes 76..78 stay suppressed in every mode: Vita hardware QA
         * verified those are only the incorrect rainbow streamer meshes. */
        if(anim_mode!=TNT_WONDER_COMPLETE_ANIM_FINALE&&
           ((tri->node>=28U&&tri->node<=40U)||tri->node==42U))continue;
        if(tri->node>=76U&&tri->node<=78U)continue;
        if(tri->node>=TNT_JAPANESE_COMPLETE_NODE_COUNT||tri->texture>=TNT_JAPANESE_COMPLETE_TEXTURE_COUNT||s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        meta=&g_tnt_japanese_complete_textures[tri->texture];
        if(anim_mode==TNT_WONDER_COMPLETE_ANIM_FINALE){
            if(tri->node==73U&&tri->texture==1U)effect_group=1U;
            else if(tri->node==42U&&tri->texture==3U)effect_group=2U;
            else if(tri->node==42U&&tri->texture==4U)effect_group=3U;
            else if(tri->node>=28U&&tri->node<=40U&&tri->texture==5U)effect_group=4U;
        }
        tex=effect_group?(u16)(TNT_GXM_JAPANESE_EFFECT_TEXTURE_BASE+effect_group-1U):
            (u16)(TNT_GXM_JAPANESE_COMPLETE_TEXTURE_BASE+tri->texture);
        for(k=0;k<3;k++){const TNTWonderVertex *v=&tri->v[k];transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cx[k],&cy[k],&cz[k]);}
        if(cz[0]>=-s_complete_near&&cz[1]>=-s_complete_near&&cz[2]>=-s_complete_near)continue;
        {
            u8 japanese_alpha=(u8)(tri->texture>=76U && tri->texture<=85U);
            u8 edge_or_alpha=effect_group?0U:(u8)(((meta->wrap&0x20U)!=0U) || japanese_alpha);
            u8 blend=(u8)(effect_group?1U:0U);
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tex||
           s_gxm.runs[s_gxm.run_count-1].pad!=blend||s_gxm.runs[s_gxm.run_count-1].pad2!=edge_or_alpha){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;s_gxm.runs[s_gxm.run_count].count=0;s_gxm.runs[s_gxm.run_count].texture=tex;
            s_gxm.runs[s_gxm.run_count].pad=blend;s_gxm.runs[s_gxm.run_count].pad2=edge_or_alpha;
            s_japanese_run_shade_first[s_gxm.run_count]=s_gxm.color_count;s_japanese_run_shade_count[s_gxm.run_count]=0;s_gxm.run_count++;
        }
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];
            tv->x=cx[k];tv->y=cy[k];tv->z=cz[k];
            tv->u=((f32)v->s/64.0f)/((f32)meta->width*((meta->wrap&0x08U)?2.0f:1.0f));
            tv->v=((f32)v->t/64.0f)/(f32)meta->height;
        }
        s_gxm.runs[s_gxm.run_count-1].count+=3;

        }
    }
    s_complete_fog_first=s_gxm.color_count;s_complete_fog_count=0;
    return 0;
}

static void russian_complete_world_node(s32 idx,const f32 nodes[TNT_RUSSIAN_COMPLETE_NODE_COUNT][12],
                                          TNTGxmMatrix world[TNT_RUSSIAN_COMPLETE_NODE_COUNT],
                                          u8 done[TNT_RUSSIAN_COMPLETE_NODE_COUNT]) {
    TNTGxmMatrix local;s32 parent;
    if(done[idx])return;
    node_type2(nodes[idx],&local);
    parent=g_tnt_russian_complete_parents[idx];
    if(parent>=0&&parent<TNT_RUSSIAN_COMPLETE_NODE_COUNT&&g_tnt_russian_complete_node_type[parent]==2){
        russian_complete_world_node(parent,nodes,world,done);mat_mult(&world[idx],&world[parent],&local);
    }else world[idx]=local;
    done[idx]=1;
}

static int build_russian_complete_geometry(u8 anim_mode,u32 anim_ticks) {
    TNTGxmMatrix view,world[TNT_RUSSIAN_COMPLETE_NODE_COUNT],draw[TNT_RUSSIAN_COMPLETE_NODE_COUNT],mv[TNT_RUSSIAN_COMPLETE_NODE_COUNT];
    f32 nodes[TNT_RUSSIAN_COMPLETE_NODE_COUNT][12];u8 done[TNT_RUSSIAN_COMPLETE_NODE_COUNT];u16 untex_index=0;s32 i,k;
    TNT_Vita_RussianWonderCompleteBuildPose(anim_mode,anim_ticks,nodes);
    memset(done,0,sizeof(done));complete_camera_view(nodes[0],&view);
    for(i=0;i<TNT_RUSSIAN_COMPLETE_NODE_COUNT;i++)if(g_tnt_russian_complete_node_mesh[i]!=255){
        russian_complete_world_node(i,nodes,world,done);
        make_draw_model(&world[i],g_tnt_russian_complete_mesh_scale[g_tnt_russian_complete_node_mesh[i]],&draw[i]);
        mat_mult(&mv[i],&view,&draw[i]);
    }
    reset_geometry();
    for(i=0;i<TNT_RUSSIAN_COMPLETE_TRIANGLE_COUNT;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_russian_complete_triangles[i];u16 ci;f32 cx[3],cy[3],cz[3];
        if(tri->texture!=255U)continue;ci=untex_index++;
        if(ci>=TNT_RUSSIAN_COMPLETE_UNTEXTURED_TRIANGLE_COUNT||tri->node>=TNT_RUSSIAN_COMPLETE_NODE_COUNT)return -1;
        for(k=0;k<3;k++){const TNTWonderVertex *v=&tri->v[k];transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cx[k],&cy[k],&cz[k]);}
        if(cz[0]>=-s_complete_near&&cz[1]>=-s_complete_near&&cz[2]>=-s_complete_near)continue;
        if(s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
        for(k=0;k<3;k++){TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];cv->x=cx[k];cv->y=cy[k];cv->z=cz[k];cv->color=g_tnt_russian_complete_untextured_colors[ci][k];}
    }
    if(untex_index!=TNT_RUSSIAN_COMPLETE_UNTEXTURED_TRIANGLE_COUNT)return -1;s_complete_untex_count=s_gxm.color_count;
    for(i=0;i<TNT_RUSSIAN_COMPLETE_TRIANGLE_COUNT;i++){
        const TNTWonderCompleteTriangle *tri=&g_tnt_russian_complete_triangles[i];const TNTWonderCompleteTexture *meta;u16 tex;f32 cx[3],cy[3],cz[3];u8 edge;
        if(tri->texture==255U)continue;
        if(tri->node>=TNT_RUSSIAN_COMPLETE_NODE_COUNT||tri->texture>=TNT_RUSSIAN_COMPLETE_TEXTURE_COUNT||s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        meta=&g_tnt_russian_complete_textures[tri->texture];tex=(u16)(TNT_GXM_RUSSIAN_COMPLETE_TEXTURE_BASE+tri->texture);edge=(u8)((meta->wrap&0x20U)!=0U);
        for(k=0;k<3;k++){const TNTWonderVertex *v=&tri->v[k];transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cx[k],&cy[k],&cz[k]);}
        if(cz[0]>=-s_complete_near&&cz[1]>=-s_complete_near&&cz[2]>=-s_complete_near)continue;
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tex||s_gxm.runs[s_gxm.run_count-1].pad2!=edge){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;s_gxm.runs[s_gxm.run_count].count=0;s_gxm.runs[s_gxm.run_count].texture=tex;
            s_gxm.runs[s_gxm.run_count].pad=0;s_gxm.runs[s_gxm.run_count].pad2=edge;s_gxm.run_count++;
        }
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];
            tv->x=cx[k];tv->y=cy[k];tv->z=cz[k];
            tv->u=((f32)v->s/64.0f)/((f32)meta->width*((meta->wrap&0x08U)?2.0f:1.0f));
            tv->v=((f32)v->t/64.0f)/(f32)meta->height;
        }
        s_gxm.runs[s_gxm.run_count-1].count+=3;
    }
    s_complete_fog_first=s_gxm.color_count;s_complete_fog_count=0;
    return 0;
}

static void skybox_projection_matrix(f32 m[16]) {
    const f32 nearz=4.0f,farz=1600.0f;
    const f32 half_h=nearz*tanf(25.0f*TNT_GXM_DEG2RAD);
    const f32 half_w=half_h*(4.0f/3.0f);
    memset(m,0,16*sizeof(f32));
    m[0]=nearz/half_w;m[5]=nearz/half_h;
    m[10]=-(farz+nearz)/(farz-nearz);
    m[14]=(-2.0f*farz*nearz)/(farz-nearz);
    m[11]=-1.0f;
}

static void intro_camera_view(const TNTIntroPose *pose,TNTGxmMatrix *view) {
    const f32 *v=pose->node[0];
    f32 bx=v[0]-v[3],by=v[1]-v[4],bz=v[2]-v[5];
    f32 rx,ry,rz,ux,uy,uz,c,s,rrx,rry,rrz,uux,uuy,uuz;
    f32 ex=v[0]*16.0f,ey=v[1]*16.0f,ez=v[2]*16.0f;
    if(bx==0.0f&&by==0.0f&&bz==0.0f)bz=-1.0f;
    normalize3(&bx,&by,&bz);
    rx=bz;ry=0.0f;rz=-bx;normalize3(&rx,&ry,&rz);
    ux=(by*rz)-(ry*bz);uy=(bz*rx)-(rz*bx);uz=(bx*ry)-(rx*by);
    c=cosf(v[6]*TNT_GXM_DEG2RAD);s=sinf(v[6]*TNT_GXM_DEG2RAD);
    rrx=rx*c+ux*s;rry=ry*c+uy*s;rrz=rz*c+uz*s;
    uux=-rx*s+ux*c;uuy=-ry*s+uy*c;uuz=-rz*s+uz*c;
    mat_identity(view);
    view->m[0][0]=rrx;view->m[0][1]=rry;view->m[0][2]=rrz;
    view->m[1][0]=uux;view->m[1][1]=uuy;view->m[1][2]=uuz;
    view->m[2][0]=bx; view->m[2][1]=by; view->m[2][2]=bz;
    view->m[0][3]=-(rrx*ex+rry*ey+rrz*ez);
    view->m[1][3]=-(uux*ex+uuy*ey+uuz*ez);
    view->m[2][3]=-(bx*ex+by*ey+bz*ez);
}

static void intro_projection_matrix(const TNTIntroPose *pose,f32 m[16],f32 *near_out,f32 *far_out) {
    f32 nearz=pose->camera[1]*16.0f,farz=pose->camera[2]*16.0f;
    f32 fovy=pose->camera[0],aspect=pose->camera[3];
    f32 half_h,half_w;
    if(nearz<2.0f)nearz=2.0f;if(nearz>4.0f)nearz=4.0f;
    if(farz>32000.0f)farz=32000.0f;if(farz<nearz+1.0f)farz=nearz+1.0f;
    if(fovy<1.0f)fovy=1.0f;if(fovy>170.0f)fovy=170.0f;
    if(aspect<0.1f)aspect=4.0f/3.0f;
    half_h=nearz*tanf(fovy*0.5f*TNT_GXM_DEG2RAD);half_w=half_h*aspect;
    memset(m,0,16*sizeof(f32));m[0]=nearz/half_w;m[5]=nearz/half_h;
    m[10]=-(farz+nearz)/(farz-nearz);m[14]=(-2.0f*farz*nearz)/(farz-nearz);m[11]=-1.0f;
    if(near_out)*near_out=nearz;if(far_out)*far_out=farz;
}

static void intro_node_world_one(u8 index,const TNTIntroPose *pose,TNTGxmMatrix world[TNT_INTRO_NODE_COUNT],u8 done[TNT_INTRO_NODE_COUNT]) {
    TNTGxmMatrix local;
    s8 parent;
    if(done[index])return;
    if(g_tnt_intro_nodes[index].type==2)node_type2(pose->node[index],&local);
    else mat_identity(&local);
    parent=g_tnt_intro_nodes[index].parent;
    if(parent>=0){intro_node_world_one((u8)parent,pose,world,done);mat_mult(&world[index],&world[(u8)parent],&local);}
    else world[index]=local;
    done[index]=TRUE;
}

static u32 intro_pack_rgba(f32 r,f32 g,f32 b,f32 a) {
    u32 ri,gi,bi,ai;
    if(r<0)r=0;if(r>1)r=1;if(g<0)g=0;if(g>1)g=1;if(b<0)b=0;if(b>1)b=1;if(a<0)a=0;if(a>1)a=1;
    ri=(u32)(r*255.0f+0.5f);gi=(u32)(g*255.0f+0.5f);bi=(u32)(b*255.0f+0.5f);ai=(u32)(a*255.0f+0.5f);
    return ri|(gi<<8)|(bi<<16)|(ai<<24);
}

static void intro_vertex_shade(const TNTIntroPose *pose,const TNTIntroDrawDef *draw,const TNTGxmMatrix *model,
                               const TNTIntroVertex *v,f32 *r,f32 *g,f32 *b) {
    if(draw->material_flags&0x08U){
        *r=(f32)(u8)v->nx/255.0f;*g=(f32)(u8)v->ny/255.0f;*b=(f32)(u8)v->nz/255.0f;
    }else{
        f32 nx,ny,nz,d0,d1;
        f32 l0x=pose->node[1][0],l0y=pose->node[1][1],l0z=pose->node[1][2];
        f32 l1x=pose->node[2][0],l1y=pose->node[2][1],l1z=pose->node[2][2];
        transform_vector(model,(f32)v->nx/127.0f,(f32)v->ny/127.0f,(f32)v->nz/127.0f,&nx,&ny,&nz);
        normalize3(&nx,&ny,&nz);normalize3(&l0x,&l0y,&l0z);normalize3(&l1x,&l1y,&l1z);
        d0=nx*l0x+ny*l0y+nz*l0z;if(d0<0)d0=0;
        d1=nx*l1x+ny*l1y+nz*l1z;if(d1<0)d1=0;
        *r=pose->light[2][0]*pose->light[2][1]+d0*pose->light[0][0]*pose->light[0][1]+d1*pose->light[1][0]*pose->light[1][1];
        *g=pose->light[2][0]*pose->light[2][2]+d0*pose->light[0][0]*pose->light[0][2]+d1*pose->light[1][0]*pose->light[1][2];
        *b=pose->light[2][0]*pose->light[2][3]+d0*pose->light[0][0]*pose->light[0][3]+d1*pose->light[1][0]*pose->light[1][3];
        if(*r>1)*r=1;if(*g>1)*g=1;if(*b>1)*b=1;if(*r<0)*r=0;if(*g<0)*g=0;if(*b<0)*b=0;
    }
}

static f32 intro_fog_factor(f32 view_z,f32 nearz,f32 farz,f32 fog_min,f32 fog_max) {
    f32 clipz,w,ndc,depth;
    if(view_z>=-nearz||fog_max<=fog_min)return 0.0f;
    clipz=(-(farz+nearz)/(farz-nearz))*view_z+(-2.0f*farz*nearz)/(farz-nearz);
    w=-view_z;if(w<=0.000001f)return 0.0f;
    ndc=clipz/w;depth=(ndc*0.5f+0.5f)*1000.0f;
    depth=(depth-fog_min)/(fog_max-fog_min);if(depth<0)depth=0;if(depth>1)depth=1;return depth;
}

static int build_intro_geometry(f32 proj[16]) {
    const TNTIntroPose *pose=TNT_Vita_IntroPose();
    TNTGxmMatrix world[TNT_INTRO_NODE_COUNT],view,model,mv;
    u8 done[TNT_INTRO_NODE_COUNT]={0};
    f32 nearz,farz;u32 i,j;
    reset_geometry();s_intro_untex_runs=0;s_intro_shade_first=s_intro_shade_count=s_intro_fog_first=s_intro_fog_count=0;
    update_intro_fade_textures(pose);intro_camera_view(pose,&view);intro_projection_matrix(pose,proj,&nearz,&farz);
    for(i=0;i<TNT_INTRO_NODE_COUNT;i++)intro_node_world_one((u8)i,pose,world,done);

    /* Textured source draws stay in original command order. */
    for(i=0;i<TNT_INTRO_DRAW_COUNT;i++){
        const TNTIntroDrawDef *d=&g_tnt_intro_draws[i];
        const TNTIntroTextureDef *td;TNTGxmRun *run;u16 gpu_tex;
        if(d->texture==255||pose->draw_transparency[i]>=1.0f)continue;
        if(s_gxm.run_count>=TNT_GXM_MAX_RUNS||s_gxm.tex_count+d->count>TNT_GXM_MAX_TEX_VERTS)return -1;
        make_draw_model(&world[d->node],d->scale,&model);mat_mult(&mv,&view,&model);
        td=&g_tnt_intro_textures[d->texture];
        gpu_tex=(i>=73&&i<=77)?(u16)(TNT_GXM_INTRO_FADE_TEXTURE_BASE+(i-73)):(u16)(TNT_GXM_INTRO_TEXTURE_BASE+d->texture);
        run=&s_gxm.runs[s_gxm.run_count++];run->first=s_gxm.tex_count;run->count=d->count;run->texture=gpu_tex;
        run->pad=(u8)(i>=73&&i<=77);
        run->pad2=0;
        for(j=0;j<d->count;j++){
            const TNTIntroVertex *v=&g_tnt_intro_vertices[d->first+j];TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];
            transform_point(&mv,(f32)v->x,(f32)v->y,(f32)v->z,&tv->x,&tv->y,&tv->z);
            tv->u=(((f32)v->s/64.0f)+0.5f)/(f32)td->size;tv->v=(((f32)v->t/64.0f)+0.5f)/(f32)td->size;
        }
    }

    /* Untextured draws 78..85 are last in the canonical Borg command stream. */
    for(i=0;i<TNT_INTRO_DRAW_COUNT;i++){
        const TNTIntroDrawDef *d=&g_tnt_intro_draws[i];
        if(d->texture!=255||pose->draw_transparency[i]>=1.0f)continue;
        if(s_intro_untex_runs>=TNT_INTRO_UNTEXTURED_DRAW_COUNT||s_gxm.color_count+d->count>TNT_GXM_MAX_COLOR_VERTS)return -1;
        make_draw_model(&world[d->node],d->scale,&model);mat_mult(&mv,&view,&model);
        s_intro_untex_first[s_intro_untex_runs]=s_gxm.color_count;s_intro_untex_count[s_intro_untex_runs]=d->count;s_intro_untex_alpha[s_intro_untex_runs]=1;s_intro_untex_runs++;
        for(j=0;j<d->count;j++){
            const TNTIntroVertex *v=&g_tnt_intro_vertices[d->first+j];TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];f32 r,g,b,opacity;
            transform_point(&mv,(f32)v->x,(f32)v->y,(f32)v->z,&cv->x,&cv->y,&cv->z);
            intro_vertex_shade(pose,d,&model,v,&r,&g,&b);r*=d->r;g*=d->g;b*=d->b;
            opacity=(1.0f-pose->draw_transparency[i])*((f32)v->a/255.0f);cv->color=intro_pack_rgba(r,g,b,opacity);
        }
    }

    /* Texture lighting / authored vertex-color modulation.  Material-fade
     * surfaces remain excluded because their texture alpha is dynamic.  The
     * resource-943 pickaxe blade now uses an opaque base pass and therefore
     * receives the same N64 TEXEL0 * SHADE modulation as other lit textures. */
    s_intro_shade_first=s_gxm.color_count;
    for(i=0;i<TNT_INTRO_DRAW_COUNT;i++){
        const TNTIntroDrawDef *d=&g_tnt_intro_draws[i];
        if(d->texture==255||pose->draw_transparency[i]>=1.0f||(i>=73&&i<=77))continue;
        if(s_gxm.color_count+d->count>TNT_GXM_MAX_COLOR_VERTS)return -1;
        make_draw_model(&world[d->node],d->scale,&model);mat_mult(&mv,&view,&model);
        for(j=0;j<d->count;j++){
            const TNTIntroVertex *v=&g_tnt_intro_vertices[d->first+j];TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];f32 r,g,b;
            transform_point(&mv,(f32)v->x,(f32)v->y,(f32)v->z,&cv->x,&cv->y,&cv->z);intro_vertex_shade(pose,d,&model,v,&r,&g,&b);cv->color=intro_pack_rgba(r,g,b,1.0f);
        }
    }
    s_intro_shade_count=s_gxm.color_count-s_intro_shade_first;

    /* Source fog is at the extreme far plane (initially 995..999). A final
     * depth-equal alpha pass reproduces the visual blend without a fog shader. */
    s_intro_fog_first=s_gxm.color_count;
    for(i=0;i<TNT_INTRO_DRAW_COUNT;i++){
        const TNTIntroDrawDef *d=&g_tnt_intro_draws[i];
        if(pose->draw_transparency[i]>=1.0f||(i>=73&&i<=77))continue;
        if(i>=84)continue; /* two translucent untextured fades */
        if(s_gxm.color_count+d->count>TNT_GXM_MAX_COLOR_VERTS)return -1;
        make_draw_model(&world[d->node],d->scale,&model);mat_mult(&mv,&view,&model);
        for(j=0;j<d->count;j++){
            const TNTIntroVertex *v=&g_tnt_intro_vertices[d->first+j];TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];f32 f;
            transform_point(&mv,(f32)v->x,(f32)v->y,(f32)v->z,&cv->x,&cv->y,&cv->z);
            f=intro_fog_factor(cv->z,nearz,farz,pose->fog[0],pose->fog[1])*pose->fog[5];
            cv->color=intro_pack_rgba(pose->fog[2],pose->fog[3],pose->fog[4],f);
        }
    }
    s_intro_fog_count=s_gxm.color_count-s_intro_fog_first;
    return 0;
}


static int build_skybox_geometry(f32 angle_degrees) {
    static const f32 node[9]={0.0f,0.0f,0.0f,0.0f,1.350417018f,0.0f,1.0f,0.916000783f,1.0f};
    static f32 cached_u[64][3];
    static u8 cached_u_ready;
    TNTGxmMatrix wrap,local,world,draw,view,mv;
    s32 i,k;

    /* Skybox U is a cylindrical projection of each source vertex's fixed X/Z
     * position. Rotation changes only the transformed position, never the UV.
     * The old path repeated 192 atan2f() calls every gameplay frame. Build the
     * exact same seam-adjusted values once and reuse them thereafter. */
    if(!cached_u_ready){
        for(i=0;i<64;i++){
            const TNTMayanSkyboxTriangle *tri=&g_tnt_mayan_skybox_triangles[i];
            f32 umin=2.0f,umax=-1.0f;
            for(k=0;k<3;k++){
                const TNTWonderVertex *v=&tri->v[k];
                f32 a=atan2f((f32)v->z,(f32)v->x);
                if(a<0.0f)a+=2.0f*3.14159265358979323846f;
                cached_u[i][k]=a/(2.0f*3.14159265358979323846f);
                if(cached_u[i][k]<umin)umin=cached_u[i][k];
                if(cached_u[i][k]>umax)umax=cached_u[i][k];
            }
            if(umax-umin>0.5f)
                for(k=0;k<3;k++)if(cached_u[i][k]<0.5f)cached_u[i][k]+=1.0f;
        }
        cached_u_ready=1;
    }

    reset_geometry();
    wrapper_matrix(angle_degrees,0.0f,0.0f,0.0f,0.0f,0.0f,&wrap);
    node_type2(node,&local);mat_mult(&world,&wrap,&local);make_draw_model(&world,0.125f,&draw);
    mat_identity(&view);view.m[0][0]=-1.0f;view.m[1][1]=1.0f;view.m[2][2]=-1.0f;
    mat_mult(&mv,&view,&draw);
    for(i=0;i<64;i++){
        const TNTMayanSkyboxTriangle *tri=&g_tnt_mayan_skybox_triangles[i];
        const u8 tex=TNT_GXM_MAYAN_CONTINUOUS_SKY_TEXTURE;
        if(s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tex){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;
            s_gxm.runs[s_gxm.run_count].count=0;
            s_gxm.runs[s_gxm.run_count].texture=tex;
            s_gxm.run_count++;
        }
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];
            transform_point(&mv,(f32)v->x,(f32)v->y,(f32)v->z,&tv->x,&tv->y,&tv->z);
            tv->u=cached_u[i][k];
            /* y=-256 -> bottom edge, y=0 -> exact band join, y=255 ->
             * just inside the clamped top edge of the 64-row panorama. */
            tv->v=((f32)v->y+256.0f)/512.0f;
        }
        s_gxm.runs[s_gxm.run_count-1].count+=3;
    }
    return 0;
}

static int build_greek_skybox_geometry(f32 angle_degrees) {
    TNTGxmMatrix wrap,local,world,draw,view,mv; s32 i,k;
    reset_geometry();
    wrapper_matrix(angle_degrees,0.0f,0.0f,0.0f,0.0f,0.0f,&wrap);
    node_type2(g_tnt_greek_skybox_nodes[1],&local);mat_mult(&world,&wrap,&local);
    make_draw_model(&world,0.125f,&draw);
    /* Scene 1399 camera is eye (0,0,0), target (0,0,-5), roll 0. */
    mat_identity(&view);mat_mult(&mv,&view,&draw);
    for(i=0;i<64;i++){
        const TNTWonderTriangle *tri=&g_tnt_greek_skybox_triangles[i];
        u8 tex=(u8)(TNT_GXM_GREEK_SKYBOX_TEXTURE_BASE+tri->texture);
        if(s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tex){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;s_gxm.runs[s_gxm.run_count].count=0;
            s_gxm.runs[s_gxm.run_count].texture=tex;s_gxm.run_count++;
        }
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];
            transform_point(&mv,(f32)v->x,(f32)v->y,(f32)v->z,&tv->x,&tv->y,&tv->z);
            /* Test216D native-only Greek sky ceiling coverage correction:
             * The ROM's 50-degree camera is correct, but the truncated scene
             * 1399 dome ends about 19 native pixels below the upper frustum.
             * This leaves the base (20,20,40) clear exposed as a dark band.
             * Extend only positive model-space sky height by 16 percent;
             * horizon/ground, cloud S/T, original ROM texture and the 400x300
             * foreground remain unchanged. Measured at 24 rotations and
             * across nine screen columns to cover y=0 before hardware QA. */
            if (tv->y > 0.0f) tv->y *= 1.16f;
            tv->u=(((f32)v->s/64.0f)+0.5f)/32.0f;tv->v=(((f32)v->t/64.0f)+0.5f)/32.0f;
        }
        s_gxm.runs[s_gxm.run_count-1].count+=3;
    }
    return 0;
}

static int build_greek_rain_geometry(u32 anim_ticks,u8 *material_alpha,u8 append) {
    TNTGxmMatrix view,local,draw[8],mv[8];f32 nodes[8][9];s32 i,k;
    TNT_Vita_GreekRainBuildPose(anim_ticks,nodes,material_alpha);complete_camera_view(nodes[0],&view);
    for(i=1;i<8;i++){node_type2(nodes[i],&local);make_draw_model(&local,0.125f,&draw[i]);mat_mult(&mv[i],&view,&draw[i]);}
    if(!append)reset_geometry();
    for(i=0;i<23;i++){
        const TNTWonderTriangle *tri=&g_tnt_greek_rain_triangles[i];
        u8 tex=(u8)(tri->node==7?TNT_GXM_GREEK_RAIN_ALPHA_TEXTURE:TNT_GXM_GREEK_RAIN_TEXTURE);
        if(tri->node==0||tri->node>=8||s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tex){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;s_gxm.runs[s_gxm.run_count].count=0;
            s_gxm.runs[s_gxm.run_count].texture=tex;s_gxm.run_count++;
        }
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];
            transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&tv->x,&tv->y,&tv->z);
            tv->u=(((f32)v->s/64.0f)+0.5f)/32.0f;tv->v=(((f32)v->t/64.0f)+0.5f)/32.0f;
        }
        s_gxm.runs[s_gxm.run_count-1].count+=3;
    }
    return 0;
}

static void celtic_sky_world_node(s32 idx,const f32 nodes[TNT_CELTIC_SKY_NODE_COUNT][9],TNTGxmMatrix world[TNT_CELTIC_SKY_NODE_COUNT],u8 done[TNT_CELTIC_SKY_NODE_COUNT]) {
    TNTGxmMatrix local;s32 parent;if(done[idx])return;node_type2(nodes[idx],&local);parent=g_tnt_celtic_sky_parents[idx];
    if(parent>=0&&parent<TNT_CELTIC_SKY_NODE_COUNT&&g_tnt_celtic_sky_node_type[parent]==2){celtic_sky_world_node(parent,nodes,world,done);mat_mult(&world[idx],&world[parent],&local);}else world[idx]=local;done[idx]=1;
}

static int build_celtic_sky_geometry(u32 ticks,u8 *animated_alpha,u16 *untex_count) {
    f32 nodes[TNT_CELTIC_SKY_NODE_COUNT][9];TNTGxmMatrix view,world[TNT_CELTIC_SKY_NODE_COUNT],draw[TNT_CELTIC_SKY_NODE_COUNT],mv[TNT_CELTIC_SKY_NODE_COUNT];
    u8 done[TNT_CELTIC_SKY_NODE_COUNT];s32 i,k;u8 alpha=255U;
    TNT_Vita_CelticSkyBuildPose(ticks,nodes,&alpha);memset(done,0,sizeof(done));complete_camera_view(nodes[0],&view);
    for(i=0;i<TNT_CELTIC_SKY_NODE_COUNT;i++)if(g_tnt_celtic_sky_node_mesh[i]!=255U){u8 mi=g_tnt_celtic_sky_node_mesh[i];celtic_sky_world_node(i,nodes,world,done);make_draw_model(&world[i],g_tnt_celtic_sky_mesh_scale[mi],&draw[i]);mat_mult(&mv[i],&view,&draw[i]);}
    reset_geometry();
    for(i=0;i<TNT_CELTIC_SKY_UNTEXTURED_TRIANGLE_COUNT;i++){
        const TNTWonderTriangle *tri=&g_tnt_celtic_sky_untextured[i];if(tri->node>=TNT_CELTIC_SKY_NODE_COUNT||s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
        for(k=0;k<3;k++){const TNTWonderVertex *v=&tri->v[k];TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cv->x,&cv->y,&cv->z);cv->color=0xffffffffU;}
    }
    if(untex_count)*untex_count=s_gxm.color_count;
    for(i=0;i<TNT_CELTIC_SKY_TRIANGLE_COUNT;i++){
        const TNTWonderTriangle *tri=&g_tnt_celtic_sky_triangles[i];u8 cls=g_tnt_celtic_sky_alpha_class[i];u16 tex=(u16)(TNT_GXM_CELTIC_SKY_TEXTURE_BASE+tri->texture);
        if(tri->node>=TNT_CELTIC_SKY_NODE_COUNT||s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tex||s_gxm.runs[s_gxm.run_count-1].pad!=cls){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;s_gxm.runs[s_gxm.run_count].count=0;s_gxm.runs[s_gxm.run_count].texture=tex;s_gxm.runs[s_gxm.run_count].pad=cls;s_gxm.runs[s_gxm.run_count].pad2=0;s_gxm.run_count++;
        }
        for(k=0;k<3;k++){const TNTWonderVertex *v=&tri->v[k];TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&tv->x,&tv->y,&tv->z);
            /* Material 0x5c uses Borg's non-texgen gSPTexture(0x8000).
             * F3DEX vertex S/T is 5-bit fractional, then the 0x8000 texture
             * scale halves it: effective texel coordinate = raw / 64.  The
             * previous /128 conversion applied that half-scale twice, sampling
             * only half of the authored Celtic sky gradient. */
            tv->u=(((f32)v->s/64.0f)+0.5f)/32.0f;tv->v=(((f32)v->t/64.0f)+0.5f)/32.0f;}
        s_gxm.runs[s_gxm.run_count-1].count+=3;
    }
    update_celtic_sky_alpha(alpha);if(animated_alpha)*animated_alpha=alpha;return 0;
}

static void african_sky_world_node(s32 idx,const f32 nodes[TNT_AFRICAN_SKY_NODE_COUNT][9],TNTGxmMatrix world[TNT_AFRICAN_SKY_NODE_COUNT],u8 done[TNT_AFRICAN_SKY_NODE_COUNT]) {
    TNTGxmMatrix local;s32 parent;if(done[idx])return;node_type2(nodes[idx],&local);parent=g_tnt_african_sky_parents[idx];
    if(parent>=0&&parent<TNT_AFRICAN_SKY_NODE_COUNT&&g_tnt_african_sky_node_type[parent]==2){african_sky_world_node(parent,nodes,world,done);mat_mult(&world[idx],&world[parent],&local);}else world[idx]=local;done[idx]=1;
}

static int build_african_sky_geometry(u32 ticks) {
    f32 nodes[TNT_AFRICAN_SKY_NODE_COUNT][9];TNTGxmMatrix view,world[TNT_AFRICAN_SKY_NODE_COUNT],draw[TNT_AFRICAN_SKY_NODE_COUNT],mv[TNT_AFRICAN_SKY_NODE_COUNT];
    u8 done[TNT_AFRICAN_SKY_NODE_COUNT];s32 i,k;
    TNT_Vita_AfricanSkyBuildPose(ticks,nodes);memset(done,0,sizeof(done));complete_camera_view(nodes[0],&view);
    for(i=0;i<TNT_AFRICAN_SKY_NODE_COUNT;i++)if(g_tnt_african_sky_node_mesh[i]!=255U){u8 mi=g_tnt_african_sky_node_mesh[i];african_sky_world_node(i,nodes,world,done);make_draw_model(&world[i],g_tnt_african_sky_mesh_scale[mi],&draw[i]);mat_mult(&mv[i],&view,&draw[i]);}
    reset_geometry();
    for(i=0;i<TNT_AFRICAN_SKY_TRIANGLE_COUNT;i++){
        const TNTWonderTriangle *tri=&g_tnt_african_sky_triangles[i];u8 blend=(u8)(g_tnt_african_sky_triangle_alpha[i]<255U);u16 tex=(u16)(TNT_GXM_AFRICAN_SKY_TEXTURE_BASE+tri->texture);
        if(tri->node>=TNT_AFRICAN_SKY_NODE_COUNT||s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tex||s_gxm.runs[s_gxm.run_count-1].pad!=blend){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;s_gxm.runs[s_gxm.run_count].count=0;s_gxm.runs[s_gxm.run_count].texture=tex;s_gxm.runs[s_gxm.run_count].pad=blend;s_gxm.runs[s_gxm.run_count].pad2=0;s_gxm.run_count++;
        }
        for(k=0;k<3;k++){const TNTWonderVertex *v=&tri->v[k];TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&tv->x,&tv->y,&tv->z);
            /* Both African materials use Borg's non-texgen 0x8000 texture scale:
             * F3DEX S/T 5-bit fractions therefore map to effective raw/64. */
            tv->u=(((f32)v->s/64.0f)+0.5f)/32.0f;tv->v=(((f32)v->t/64.0f)+0.5f)/32.0f;}
        s_gxm.runs[s_gxm.run_count-1].count+=3;
    }
    return 0;
}

static void russian_world_node(s32 idx,const f32 (*nodes)[9],const s8 *parents,const u8 *types,
    s32 count,TNTGxmMatrix *world,u8 *done) {
    TNTGxmMatrix local;s32 parent;if(done[idx])return;node_type2(nodes[idx],&local);parent=parents[idx];
    if(parent>=0&&parent<count&&types[parent]==2){russian_world_node(parent,nodes,parents,types,count,world,done);mat_mult(&world[idx],&world[parent],&local);}else world[idx]=local;done[idx]=1;
}

static int build_russian_sky_geometry(u32 ticks) {
    f32 nodes[TNT_RUSSIAN_SKY_NODE_COUNT][9];TNTGxmMatrix view,world[TNT_RUSSIAN_SKY_NODE_COUNT],draw[TNT_RUSSIAN_SKY_NODE_COUNT],mv[TNT_RUSSIAN_SKY_NODE_COUNT];
    u8 done[TNT_RUSSIAN_SKY_NODE_COUNT];s32 i,k;
    TNT_Vita_RussianSkyBuildPose(ticks,nodes);memset(done,0,sizeof(done));complete_camera_view(nodes[0],&view);reset_geometry();
    for(i=0;i<TNT_RUSSIAN_SKY_NODE_COUNT;i++)if(g_tnt_russian_sky_node_mesh[i]!=255U){u8 mi=g_tnt_russian_sky_node_mesh[i];russian_world_node(i,nodes,g_tnt_russian_sky_parents,g_tnt_russian_sky_node_type,TNT_RUSSIAN_SKY_NODE_COUNT,world,done);make_draw_model(&world[i],g_tnt_russian_sky_mesh_scale[mi],&draw[i]);mat_mult(&mv[i],&view,&draw[i]);}
    for(i=0;i<TNT_RUSSIAN_SKY_TRIANGLE_COUNT;i++){
        const TNTWonderTriangle *tri=&g_tnt_russian_sky_triangles[i];u8 blend=(u8)(g_tnt_russian_sky_triangle_alpha[i]<255U);u16 tex=(u16)(TNT_GXM_RUSSIAN_SKY_TEXTURE_BASE+tri->texture);
        if(tri->node>=TNT_RUSSIAN_SKY_NODE_COUNT||s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tex||s_gxm.runs[s_gxm.run_count-1].pad!=blend){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;s_gxm.runs[s_gxm.run_count].count=0;s_gxm.runs[s_gxm.run_count].texture=tex;s_gxm.runs[s_gxm.run_count].pad=blend;s_gxm.runs[s_gxm.run_count].pad2=0;s_gxm.run_count++;
        }
        for(k=0;k<3;k++){const TNTWonderVertex *v=&tri->v[k];TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&tv->x,&tv->y,&tv->z);tv->u=(((f32)v->s/64.0f)+0.5f)/32.0f;tv->v=(((f32)v->t/64.0f)+0.5f)/32.0f;}
        s_gxm.runs[s_gxm.run_count-1].count+=3;
    }
    return 0;
}

static int build_russian_snow_geometry(u32 ticks,u16 *first,u16 *count) {
    f32 nodes[TNT_RUSSIAN_SNOW_NODE_COUNT][9];TNTGxmMatrix view,world[TNT_RUSSIAN_SNOW_NODE_COUNT],draw[TNT_RUSSIAN_SNOW_NODE_COUNT],mv[TNT_RUSSIAN_SNOW_NODE_COUNT];
    u8 done[TNT_RUSSIAN_SNOW_NODE_COUNT];s32 i,k;u16 start=s_gxm.color_count;
    TNT_Vita_RussianSnowBuildPose(ticks,nodes);memset(done,0,sizeof(done));complete_camera_view(nodes[0],&view);
    for(i=1;i<TNT_RUSSIAN_SNOW_NODE_COUNT;i++)if(g_tnt_russian_snow_node_mesh[i]!=255U){u8 mi=g_tnt_russian_snow_node_mesh[i];russian_world_node(i,nodes,g_tnt_russian_snow_parents,g_tnt_russian_snow_node_type,TNT_RUSSIAN_SNOW_NODE_COUNT,world,done);make_draw_model(&world[i],g_tnt_russian_snow_mesh_scale[mi],&draw[i]);mat_mult(&mv[i],&view,&draw[i]);}
    for(i=0;i<TNT_RUSSIAN_SNOW_TRIANGLE_COUNT;i++){
        const TNTWonderTriangle *tri=&g_tnt_russian_snow_triangles[i];
        /* Resource 1362/node 1 is the giant distant carrier sheet.  On Vita it
         * obscures the board instead of behaving like the N64 depth backdrop;
         * keep the source foreground snow meshes and suppress only that sheet. */
        if(tri->node==1U)continue;
        if(tri->node==0||tri->node>=TNT_RUSSIAN_SNOW_NODE_COUNT||s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];u32 color=0xfffffdffU;
            transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cv->x,&cv->y,&cv->z);
            if(g_tnt_russian_snow_lit[i]){
                f32 shade=0.21f+((f32)v->ny/127.0f);u32 c;
                if(shade<0.21f)shade=0.21f;if(shade>1.0f)shade=1.0f;
                c=(u32)(178.0f*shade+0.5f);if(c>255U)c=255U;color=0xff000000U|(c<<16)|(c<<8)|c;
            }
            cv->color=color;
        }
    }
    if(first)*first=start;if(count)*count=(u16)(s_gxm.color_count-start);return 0;
}

static int build_japanese_sky_geometry(f32 angle_degrees) {
    TNTGxmMatrix wrap,local,world,draw,view,mv;s32 i,k;
    reset_geometry();
    /* D_800D02E8 applies translation (0.2,-0.2,-0.7) and decrements its
     * first Euler angle by 0.05 degrees per source presentation update. */
    wrapper_matrix(angle_degrees,0.0f,0.0f,0.2f,-0.2f,-0.7f,&wrap);
    node_type2(g_tnt_japanese_sky_nodes[2],&local);mat_mult(&world,&wrap,&local);
    make_draw_model(&world,g_tnt_japanese_sky_mesh_scale[0],&draw);
    /* Scene 770 owns camera node 0 (Borg type 4).  The N64 renderer builds
     * that camera's affine transform and then inverts it before projection;
     * for the authored zero eye/target this resolves to (-X,+Y,-Z), not an
     * identity view.  Using identity exposed the curved edge of the sky dome. */
    complete_camera_view(g_tnt_japanese_sky_nodes[0],&view);
    mat_mult(&mv,&view,&draw);
    for(i=0;i<TNT_JAPANESE_SKY_TRIANGLE_COUNT;i++){
        const TNTWonderTriangle *tri=&g_tnt_japanese_sky_triangles[i];
        u16 tex=(u16)(TNT_GXM_JAPANESE_SKY_TEXTURE_BASE+tri->texture);
        if(s_gxm.tex_count+3>TNT_GXM_MAX_TEX_VERTS)return -1;
        if(s_gxm.run_count==0||s_gxm.runs[s_gxm.run_count-1].texture!=tex){
            if(s_gxm.run_count>=TNT_GXM_MAX_RUNS)return -1;
            s_gxm.runs[s_gxm.run_count].first=s_gxm.tex_count;s_gxm.runs[s_gxm.run_count].count=0;
            s_gxm.runs[s_gxm.run_count].texture=tex;s_gxm.runs[s_gxm.run_count].pad=0;s_gxm.runs[s_gxm.run_count].pad2=0;s_gxm.run_count++;
        }
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];TNTGxmTexVertex *tv=&s_gxm.tex_vertices[s_gxm.tex_count++];
            transform_point(&mv,(f32)v->x,(f32)v->y,(f32)v->z,&tv->x,&tv->y,&tv->z);
            tv->u=(((f32)v->s/64.0f)+0.5f)/32.0f;tv->v=(((f32)v->t/64.0f)+0.5f)/32.0f;
        }
        s_gxm.runs[s_gxm.run_count-1].count+=3;
    }
    return 0;
}

static void japanese_butterfly_world_node(s32 idx,const f32 nodes[TNT_JAPANESE_BUTTERFLY_NODE_COUNT][9],TNTGxmMatrix world[TNT_JAPANESE_BUTTERFLY_NODE_COUNT],u8 done[TNT_JAPANESE_BUTTERFLY_NODE_COUNT]) {
    TNTGxmMatrix local;s32 parent;if(done[idx])return;node_type2(nodes[idx],&local);parent=g_tnt_japanese_butterfly_parents[idx];
    if(parent>=0&&parent<TNT_JAPANESE_BUTTERFLY_NODE_COUNT&&g_tnt_japanese_butterfly_node_type[parent]==2){japanese_butterfly_world_node(parent,nodes,world,done);mat_mult(&world[idx],&world[parent],&local);}else world[idx]=local;done[idx]=1;
}

static int build_japanese_butterfly_geometry(u32 ticks,u16 *first,u16 *count) {
    f32 nodes[TNT_JAPANESE_BUTTERFLY_NODE_COUNT][9];TNTGxmMatrix view,world[TNT_JAPANESE_BUTTERFLY_NODE_COUNT],draw[TNT_JAPANESE_BUTTERFLY_NODE_COUNT],mv[TNT_JAPANESE_BUTTERFLY_NODE_COUNT];
    u8 done[TNT_JAPANESE_BUTTERFLY_NODE_COUNT];s32 i,k;u16 start=s_gxm.color_count;
    /* Light 1359 is ambient 0.3 gray. The butterfly card normals face +/-Z,
     * while Borg's implicit white directional is +Y, so only ambient reaches
     * material RGB (1.0, 0.666666, 0.892655). */
    const u32 butterfly_color=0xff44334dU;
    TNT_Vita_JapaneseButterfliesBuildPose(ticks,nodes);memset(done,0,sizeof(done));complete_camera_view(nodes[0],&view);
    for(i=1;i<TNT_JAPANESE_BUTTERFLY_NODE_COUNT;i++)if(g_tnt_japanese_butterfly_node_mesh[i]!=255U){u8 mi=g_tnt_japanese_butterfly_node_mesh[i];japanese_butterfly_world_node(i,nodes,world,done);make_draw_model(&world[i],g_tnt_japanese_butterfly_mesh_scale[mi],&draw[i]);mat_mult(&mv[i],&view,&draw[i]);}
    for(i=0;i<TNT_JAPANESE_BUTTERFLY_TRIANGLE_COUNT;i++){
        const TNTWonderTriangle *tri=&g_tnt_japanese_butterfly_triangles[i];if(tri->node==0||tri->node>=TNT_JAPANESE_BUTTERFLY_NODE_COUNT||s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
        for(k=0;k<3;k++){const TNTWonderVertex *v=&tri->v[k];TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cv->x,&cv->y,&cv->z);cv->color=butterfly_color;}
    }
    if(first)*first=start;if(count)*count=(u16)(s_gxm.color_count-start);return 0;
}

static void celtic_leaf_world_node(s32 idx,const f32 nodes[TNT_CELTIC_LEAF_NODE_COUNT][9],TNTGxmMatrix world[TNT_CELTIC_LEAF_NODE_COUNT],u8 done[TNT_CELTIC_LEAF_NODE_COUNT]) {
    TNTGxmMatrix local;s32 parent;if(done[idx])return;node_type2(nodes[idx],&local);parent=g_tnt_celtic_leaf_parents[idx];
    if(parent>=0&&parent<TNT_CELTIC_LEAF_NODE_COUNT&&g_tnt_celtic_leaf_node_type[parent]==2){celtic_leaf_world_node(parent,nodes,world,done);mat_mult(&world[idx],&world[parent],&local);}else world[idx]=local;done[idx]=1;
}

static int build_celtic_leaf_geometry(u32 ticks,u16 *first,u16 *count) {
    f32 nodes[TNT_CELTIC_LEAF_NODE_COUNT][9];TNTGxmMatrix view,world[TNT_CELTIC_LEAF_NODE_COUNT],draw[TNT_CELTIC_LEAF_NODE_COUNT],mv[TNT_CELTIC_LEAF_NODE_COUNT];
    u8 done[TNT_CELTIC_LEAF_NODE_COUNT];s32 i,k;u16 start=s_gxm.color_count;
    /* Scene light 1334 is ambient 0.3 gray. Material 0x54 primitive RGB is
     * (0.678,0.506,0.395), yielding the source PRIMITIVE*SHADE leaf color. */
    const u32 leaf_color=0xff1d2633U;
    TNT_Vita_CelticLeavesBuildPose(ticks,nodes);memset(done,0,sizeof(done));complete_camera_view(nodes[0],&view);
    for(i=1;i<TNT_CELTIC_LEAF_NODE_COUNT;i++)if(g_tnt_celtic_leaf_node_mesh[i]!=255U){u8 mi=g_tnt_celtic_leaf_node_mesh[i];celtic_leaf_world_node(i,nodes,world,done);make_draw_model(&world[i],g_tnt_celtic_leaf_mesh_scale[mi],&draw[i]);mat_mult(&mv[i],&view,&draw[i]);}
    for(i=0;i<TNT_CELTIC_LEAF_TRIANGLE_COUNT;i++){
        const TNTWonderTriangle *tri=&g_tnt_celtic_leaf_triangles[i];if(tri->node==0||tri->node>=TNT_CELTIC_LEAF_NODE_COUNT||s_gxm.color_count+3>TNT_GXM_MAX_COLOR_VERTS)return -1;
        for(k=0;k<3;k++){const TNTWonderVertex *v=&tri->v[k];TNTGxmColorVertex *cv=&s_gxm.color_vertices[s_gxm.color_count++];transform_point(&mv[tri->node],(f32)v->x,(f32)v->y,(f32)v->z,&cv->x,&cv->y,&cv->z);cv->color=leaf_color;}
    }
    if(first)*first=start;if(count)*count=(u16)(s_gxm.color_count-start);return 0;
}

static void celtic_sky_projection_matrix(f32 m[16]) {
    const f32 nearz=4.0f,farz=4000.0f,fovy=41.5389976501f,aspect=1.26999998093f;f32 hh=nearz*tanf((fovy*0.5f)*TNT_GXM_DEG2RAD),hw=hh*aspect;
    memset(m,0,16*sizeof(f32));m[0]=nearz/hw;m[5]=nearz/hh;m[10]=-(farz+nearz)/(farz-nearz);m[14]=(-2.0f*farz*nearz)/(farz-nearz);m[11]=-1.0f;
}
static void african_sky_projection_matrix(f32 m[16]) {
    /* Camera 565: fovy 140, near 1, far 100, aspect 1.27. Borg multiplies
     * near/far by 16 then clamps near to [2,4], yielding 4..1600. */
    const f32 nearz=4.0f,farz=1600.0f,fovy=140.0f,aspect=1.26999998093f;f32 hh=nearz*tanf((fovy*0.5f)*TNT_GXM_DEG2RAD),hw=hh*aspect;
    memset(m,0,16*sizeof(f32));m[0]=nearz/hw;m[5]=nearz/hh;m[10]=-(farz+nearz)/(farz-nearz);m[14]=(-2.0f*farz*nearz)/(farz-nearz);m[11]=-1.0f;
}

static void russian_sky_projection_matrix(f32 m[16]) {
    /* Camera 898: fovy 150, near 1, far 100, aspect 1.27. Borg scales the
     * clip planes by 16 and clamps near to 4, yielding 4..1600. */
    const f32 nearz=4.0f,farz=1600.0f,fovy=150.0000152588f,aspect=1.26999998093f;f32 hh=nearz*tanf((fovy*0.5f)*TNT_GXM_DEG2RAD),hw=hh*aspect;
    memset(m,0,16*sizeof(f32));m[0]=nearz/hw;m[5]=nearz/hh;m[10]=-(farz+nearz)/(farz-nearz);m[14]=(-2.0f*farz*nearz)/(farz-nearz);m[11]=-1.0f;
}
static void russian_snow_projection_matrix(f32 m[16]) {
    /* Camera 1368: fovy 41.539, near 1, far 200, 4:3. */
    const f32 nearz=4.0f,farz=3200.0f,fovy=41.5389976501f,aspect=1.33333301544f;f32 hh=nearz*tanf((fovy*0.5f)*TNT_GXM_DEG2RAD),hw=hh*aspect;
    memset(m,0,16*sizeof(f32));m[0]=nearz/hw;m[5]=nearz/hh;m[10]=-(farz+nearz)/(farz-nearz);m[14]=(-2.0f*farz*nearz)/(farz-nearz);m[11]=-1.0f;
}

static void japanese_sky_projection_matrix(f32 m[16]) {
    /* Camera 199: fovy 50, near 1, far 100, aspect 1.3333. Borg scales
     * near/far by 16 and clamps near into [2,4], yielding 4..1600. */
    const f32 nearz=4.0f,farz=1600.0f,fovy=49.9999961853f,aspect=1.33329999447f;f32 hh=nearz*tanf((fovy*0.5f)*TNT_GXM_DEG2RAD),hw=hh*aspect;
    memset(m,0,16*sizeof(f32));m[0]=nearz/hw;m[5]=nearz/hh;m[10]=-(farz+nearz)/(farz-nearz);m[14]=(-2.0f*farz*nearz)/(farz-nearz);m[11]=-1.0f;
}
static void japanese_butterfly_projection_matrix(f32 m[16]) {
    /* Camera 1358: fovy 60, near .01, far 1000, aspect 1.27. Borg's exact
     * clamp makes .01*16 become near 2; far becomes 16000. */
    const f32 nearz=2.0f,farz=16000.0f,fovy=60.0000305176f,aspect=1.26999998093f;f32 hh=nearz*tanf((fovy*0.5f)*TNT_GXM_DEG2RAD),hw=hh*aspect;
    memset(m,0,16*sizeof(f32));m[0]=nearz/hw;m[5]=nearz/hh;m[10]=-(farz+nearz)/(farz-nearz);m[14]=(-2.0f*farz*nearz)/(farz-nearz);m[11]=-1.0f;
}

static void celtic_leaf_projection_matrix(f32 m[16]) {
    const f32 nearz=4.0f,farz=3200.0f,fovy=60.0000305176f,aspect=1.33333337307f;f32 hh=nearz*tanf((fovy*0.5f)*TNT_GXM_DEG2RAD),hw=hh*aspect;
    memset(m,0,16*sizeof(f32));m[0]=nearz/hw;m[5]=nearz/hh;m[10]=-(farz+nearz)/(farz-nearz);m[14]=(-2.0f*farz*nearz)/(farz-nearz);m[11]=-1.0f;
}

static void greek_rain_projection_matrix(f32 m[16]) {
    const f32 nearz=4.0f,farz=3200.0f;
    const f32 half_h=nearz*tanf((41.5389976501f*0.5f)*TNT_GXM_DEG2RAD),half_w=half_h*(4.0f/3.0f);
    memset(m,0,16*sizeof(f32));m[0]=nearz/half_w;m[5]=nearz/half_h;
    m[10]=-(farz+nearz)/(farz-nearz);m[14]=(-2.0f*farz*nearz)/(farz-nearz);m[11]=-1.0f;
}

static void set_overlay_quad_uv(TNTGxmTexVertex *v,f32 x0,f32 y0,f32 x1,f32 y1,f32 u1,f32 v1) {
    f32 l=(x0/200.0f)-1.0f,r=(x1/200.0f)-1.0f;
    f32 t=1.0f-(y0/150.0f),b=1.0f-(y1/150.0f);
    v[0]=(TNTGxmTexVertex){l,t,0.0f,0.0f,0.0f};v[1]=(TNTGxmTexVertex){l,b,0.0f,0.0f,v1};v[2]=(TNTGxmTexVertex){r,t,0.0f,u1,0.0f};
    v[3]=(TNTGxmTexVertex){r,t,0.0f,u1,0.0f};v[4]=(TNTGxmTexVertex){l,b,0.0f,0.0f,v1};v[5]=(TNTGxmTexVertex){r,b,0.0f,u1,v1};
}

static void set_overlay_quad(TNTGxmTexVertex *v,f32 x0,f32 y0,f32 x1,f32 y1) {
    set_overlay_quad_uv(v,x0,y0,x1,y1,1.0f,1.0f);
}

/* Test210D: the 960x544 Hall backdrop only changes when the selected
 * Wonder changes. Instead of CPU memcpy of its entire 2.2 MiB backing scene
 * every frame, upload once per Hall card and restore it as a GPU fullscreen
 * primitive inside the *same* scene that renders the animated 3D object.
 * This also clears the previous frame's object on both swap buffers. */
int TNT_Vita_WonderHallGxmSetBackground(const u32 *rgba) {
    const s32 idx=TNT_GXM_HALL_BACKGROUND_TEXTURE;
    u32 *dst;
    s32 y;
    if(!s_gxm.initialized || !rgba)return -1;
    s_gxm.hall_background_ready=FALSE;
    if(!s_gxm.texture_data[idx]) {
        s_gxm.texture_data[idx]=gpu_alloc(
            SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
            1024U*TNT_GXM_H*4U,SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
            &s_gxm.texture_uid[idx]);
    }
    dst=(u32*)s_gxm.texture_data[idx];
    if(!dst)return -1;
    for(y=0;y<TNT_GXM_H;y++)
        memcpy(dst+y*1024U,rgba+y*1024U,TNT_GXM_W*4U);
    if(sceGxmTextureInitLinearStrided(&s_gxm.textures[idx],dst,
        SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,TNT_GXM_W,TNT_GXM_H,4096U)<0)
        return -1;
    sceGxmTextureSetMinFilter(&s_gxm.textures[idx],SCE_GXM_TEXTURE_FILTER_POINT);
    sceGxmTextureSetMagFilter(&s_gxm.textures[idx],SCE_GXM_TEXTURE_FILTER_POINT);
    sceGxmTextureSetMipFilter(&s_gxm.textures[idx],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
    sceGxmTextureSetUAddrMode(&s_gxm.textures[idx],SCE_GXM_TEXTURE_ADDR_CLAMP);
    sceGxmTextureSetVAddrMode(&s_gxm.textures[idx],SCE_GXM_TEXTURE_ADDR_CLAMP);
    if(sceGxmTextureValidate(&s_gxm.textures[idx])<0)return -1;
    s_gxm.hall_background_ready=TRUE;
    return 0;
}

static void tnt210_hall_draw_background(void) {
    f32 ident[16];
    void *ub;
    const s32 idx=TNT_GXM_HALL_BACKGROUND_TEXTURE;
    TNTGxmTexVertex *quad=s_gxm.tex_vertices+TNT_GXM_STORY_VERTEX_BASE;
    if(!s_gxm.hall_background_ready)return;
    memset(ident,0,sizeof(ident));
    ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    set_overlay_quad(quad,0.0f,0.0f,400.0f,300.0f);
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,
        0,0,TNT_GXM_W-1,TNT_GXM_H-1);
    sceGxmSetViewport(s_gxm.context,TNT_GXM_W*0.5f,TNT_GXM_W*0.5f,
        TNT_GXM_H*0.5f,-TNT_GXM_H*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);
    sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[idx]);
    sceGxmSetVertexStream(s_gxm.context,0,quad);
    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,
        SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
}

static void set_overlay_quad_uv4(TNTGxmTexVertex *v,f32 x0,f32 y0,f32 x1,f32 y1,
    f32 u_tl,f32 v_tl,f32 u_tr,f32 v_tr,f32 u_bl,f32 v_bl,f32 u_br,f32 v_br) {
    f32 l=(x0/200.0f)-1.0f,r=(x1/200.0f)-1.0f;
    f32 t=1.0f-(y0/150.0f),b=1.0f-(y1/150.0f);
    v[0]=(TNTGxmTexVertex){l,t,0.0f,u_tl,v_tl};
    v[1]=(TNTGxmTexVertex){l,b,0.0f,u_bl,v_bl};
    v[2]=(TNTGxmTexVertex){r,t,0.0f,u_tr,v_tr};
    v[3]=(TNTGxmTexVertex){r,t,0.0f,u_tr,v_tr};
    v[4]=(TNTGxmTexVertex){l,b,0.0f,u_bl,v_bl};
    v[5]=(TNTGxmTexVertex){r,b,0.0f,u_br,v_br};
}

static u16 build_font_c_line_vertices(const char *text,f32 pen_x,f32 y0) {
    TNTGxmTexVertex *out=s_gxm.tex_vertices+TNT_GXM_STORY_VERTEX_BASE;
    u16 count=0;
    while(text&&*text!='\0'){
        unsigned c=(unsigned char)*text++;u8 glyph,glyph_w;
        if(c<32U||c>=96U)continue;
        glyph=g_tnt_font_c_map[c-32U];glyph_w=g_tnt_font_c_char_widths[glyph];
        if(c!=' '&&count+6U<=TNT_GXM_STORY_VERTEX_CAPACITY){
            f32 u0=0.0f,u1=(f32)glyph_w/(f32)TNT_GXM_FONT_C_ATLAS_W;
            f32 v0=(f32)((u32)glyph*12U)/(f32)TNT_GXM_FONT_C_ATLAS_H;
            f32 v1=(f32)((u32)glyph*12U+12U)/(f32)TNT_GXM_FONT_C_ATLAS_H;
            TNTGxmTexVertex *v=out+count;
            f32 l=(pen_x/200.0f)-1.0f,rr=((pen_x+(f32)glyph_w)/200.0f)-1.0f;
            f32 t=1.0f-(y0/150.0f),b=1.0f-((y0+12.0f)/150.0f);
            v[0]=(TNTGxmTexVertex){l,t,0.0f,u0,v0};v[1]=(TNTGxmTexVertex){l,b,0.0f,u0,v1};v[2]=(TNTGxmTexVertex){rr,t,0.0f,u1,v0};
            v[3]=(TNTGxmTexVertex){rr,t,0.0f,u1,v0};v[4]=(TNTGxmTexVertex){l,b,0.0f,u0,v1};v[5]=(TNTGxmTexVertex){rr,b,0.0f,u1,v1};
            count=(u16)(count+6U);
        }
        pen_x+=(f32)glyph_w;
    }
    return count;
}

static u16 build_wonder_story_vertices(const char *const *story,u32 line_count,u32 ticks) {
    TNTGxmTexVertex *out=s_gxm.tex_vertices+TNT_GXM_STORY_VERTEX_BASE;
    u32 scroll_px,line_advance;int pixel_offset,i;u16 count=0;
    if(!story||line_count==0U)return 0;
    scroll_px=ticks/8U;line_advance=scroll_px/12U;pixel_offset=(int)(scroll_px%12U);
    for(i=0;i<5;i++){
        u32 index=((line_count-4U)+line_advance+(u32)i)%line_count;
        const char *text=story[index];
        f32 pen_x=80.0f,y0=(f32)(30-pixel_offset+i*12);
        while(text&&*text!='\0'){
            unsigned c=(unsigned char)*text++;u8 glyph;u8 glyph_w;
            if(c<32U||c>=96U)continue;
            glyph=g_tnt_font_c_map[c-32U];glyph_w=g_tnt_font_c_char_widths[glyph];
            if(c!=' ' && count+6U<=TNT_GXM_STORY_VERTEX_CAPACITY){
                f32 u0=0.0f,u1=(f32)glyph_w/(f32)TNT_GXM_FONT_C_ATLAS_W;
                f32 v0=(f32)((u32)glyph*12U)/(f32)TNT_GXM_FONT_C_ATLAS_H;
                f32 v1=(f32)((u32)glyph*12U+12U)/(f32)TNT_GXM_FONT_C_ATLAS_H;
                TNTGxmTexVertex *v=out+count;
                f32 l=(pen_x/200.0f)-1.0f,rr=((pen_x+(f32)glyph_w)/200.0f)-1.0f;
                f32 t=1.0f-(y0/150.0f),b=1.0f-((y0+12.0f)/150.0f);
                v[0]=(TNTGxmTexVertex){l,t,0.0f,u0,v0};v[1]=(TNTGxmTexVertex){l,b,0.0f,u0,v1};v[2]=(TNTGxmTexVertex){rr,t,0.0f,u1,v0};
                v[3]=(TNTGxmTexVertex){rr,t,0.0f,u1,v0};v[4]=(TNTGxmTexVertex){l,b,0.0f,u0,v1};v[5]=(TNTGxmTexVertex){rr,b,0.0f,u1,v1};
                count=(u16)(count+6U);
            }
            pen_x+=(f32)glyph_w;
        }
    }
    return count;
}

static __attribute__((optimize("Os"))) int create_programs(void) {
    const SceGxmProgram *tv=&texture_v_gxp_start,*tf=&texture_f_gxp_start,*tef=&tntv_texedge_f_gxp_start,*cv=&color_v_gxp_start,*cf=&color_f_gxp_start;
    SceGxmVertexAttribute attrs[2];SceGxmVertexStream stream;
    const SceGxmProgramParameter *p;
    SceGxmBlendInfo mul={0},nowrite={0},alpha={0},feedback={0};
    if(sceGxmProgramCheck(tv)<0||sceGxmProgramCheck(tf)<0||sceGxmProgramCheck(tef)<0||sceGxmProgramCheck(cv)<0||sceGxmProgramCheck(cf)<0)return -1;
    if(sceGxmShaderPatcherRegisterProgram(s_gxm.patcher,tv,&s_gxm.texture_v_id)<0)return -1;
    if(sceGxmShaderPatcherRegisterProgram(s_gxm.patcher,tf,&s_gxm.texture_f_id)<0)return -1;
    if(sceGxmShaderPatcherRegisterProgram(s_gxm.patcher,tef,&s_gxm.texedge_f_id)<0)return -1;
    if(sceGxmShaderPatcherRegisterProgram(s_gxm.patcher,cv,&s_gxm.color_v_id)<0)return -1;
    if(sceGxmShaderPatcherRegisterProgram(s_gxm.patcher,cf,&s_gxm.color_f_id)<0)return -1;

    memset(attrs,0,sizeof(attrs));memset(&stream,0,sizeof(stream));
    p=sceGxmProgramFindParameterByName(tv,"aPosition");if(!p)return -1;
    attrs[0].streamIndex=0;attrs[0].offset=0;attrs[0].format=SCE_GXM_ATTRIBUTE_FORMAT_F32;attrs[0].componentCount=3;attrs[0].regIndex=sceGxmProgramParameterGetResourceIndex(p);
    p=sceGxmProgramFindParameterByName(tv,"aTexcoord");if(!p)return -1;
    attrs[1].streamIndex=0;attrs[1].offset=12;attrs[1].format=SCE_GXM_ATTRIBUTE_FORMAT_F32;attrs[1].componentCount=2;attrs[1].regIndex=sceGxmProgramParameterGetResourceIndex(p);
    stream.stride=sizeof(TNTGxmTexVertex);stream.indexSource=SCE_GXM_INDEX_SOURCE_INDEX_16BIT;
    if(sceGxmShaderPatcherCreateVertexProgram(s_gxm.patcher,s_gxm.texture_v_id,attrs,2,&stream,1,&s_gxm.texture_vp)<0)return -1;
    if(sceGxmShaderPatcherCreateFragmentProgram(s_gxm.patcher,s_gxm.texture_f_id,SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,SCE_GXM_MULTISAMPLE_NONE,NULL,tv,&s_gxm.texture_fp)<0)return -1;
    alpha.colorMask=SCE_GXM_COLOR_MASK_ALL;alpha.colorFunc=SCE_GXM_BLEND_FUNC_ADD;alpha.alphaFunc=SCE_GXM_BLEND_FUNC_ADD;
    alpha.colorSrc=SCE_GXM_BLEND_FACTOR_SRC_ALPHA;alpha.colorDst=SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    alpha.alphaSrc=SCE_GXM_BLEND_FACTOR_ONE;alpha.alphaDst=SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    if(sceGxmShaderPatcherCreateFragmentProgram(s_gxm.patcher,s_gxm.texture_f_id,SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,SCE_GXM_MULTISAMPLE_NONE,&alpha,tv,&s_gxm.texture_alpha_fp)<0)s_gxm.texture_alpha_fp=NULL;
    /* HALUCI feedback renders onto a black target whose alpha is preloaded to
     * 188/255. DST_ALPHA therefore supplies the combined source N64 feedback
     * attenuation (0xFA/255 * 0xC0/255 ~= 188/255) without a custom shader. */
    feedback.colorMask=SCE_GXM_COLOR_MASK_ALL;feedback.colorFunc=SCE_GXM_BLEND_FUNC_ADD;feedback.alphaFunc=SCE_GXM_BLEND_FUNC_ADD;
    feedback.colorSrc=SCE_GXM_BLEND_FACTOR_DST_ALPHA;feedback.colorDst=SCE_GXM_BLEND_FACTOR_ZERO;
    feedback.alphaSrc=SCE_GXM_BLEND_FACTOR_ONE;feedback.alphaDst=SCE_GXM_BLEND_FACTOR_ZERO;
    if(sceGxmShaderPatcherCreateFragmentProgram(s_gxm.patcher,s_gxm.texture_f_id,SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,SCE_GXM_MULTISAMPLE_NONE,&feedback,tv,&s_gxm.texture_feedback_fp)<0)s_gxm.texture_feedback_fp=NULL;
    if(sceGxmShaderPatcherCreateFragmentProgram(s_gxm.patcher,s_gxm.texedge_f_id,SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,SCE_GXM_MULTISAMPLE_NONE,NULL,tv,&s_gxm.texedge_fp)<0)return -1;
    /* N64 hardware sprites use G_AC_THRESHOLD together with XLU_SURF.  Keep
     * the TEX_EDGE alpha kill but retain normal source-alpha blending for
     * filtered sprite silhouettes such as the African volcano smoke. */
    if(sceGxmShaderPatcherCreateFragmentProgram(s_gxm.patcher,s_gxm.texedge_f_id,SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,SCE_GXM_MULTISAMPLE_NONE,&alpha,tv,&s_gxm.texedge_alpha_fp)<0)s_gxm.texedge_alpha_fp=NULL;
    s_gxm.texture_wvp=sceGxmProgramFindParameterByName(tv,"wvp");if(!s_gxm.texture_wvp)return -1;

    memset(attrs,0,sizeof(attrs));memset(&stream,0,sizeof(stream));
    p=sceGxmProgramFindParameterByName(cv,"aPosition");if(!p)return -1;
    attrs[0].streamIndex=0;attrs[0].offset=0;attrs[0].format=SCE_GXM_ATTRIBUTE_FORMAT_F32;attrs[0].componentCount=3;attrs[0].regIndex=sceGxmProgramParameterGetResourceIndex(p);
    p=sceGxmProgramFindParameterByName(cv,"aColor");if(!p)return -1;
    attrs[1].streamIndex=0;attrs[1].offset=12;attrs[1].format=SCE_GXM_ATTRIBUTE_FORMAT_U8N;attrs[1].componentCount=4;attrs[1].regIndex=sceGxmProgramParameterGetResourceIndex(p);
    stream.stride=sizeof(TNTGxmColorVertex);stream.indexSource=SCE_GXM_INDEX_SOURCE_INDEX_16BIT;
    if(sceGxmShaderPatcherCreateVertexProgram(s_gxm.patcher,s_gxm.color_v_id,attrs,2,&stream,1,&s_gxm.color_vp)<0)return -1;
    s_gxm.color_wvp=sceGxmProgramFindParameterByName(cv,"wvp");if(!s_gxm.color_wvp)return -1;
    if(sceGxmShaderPatcherCreateFragmentProgram(s_gxm.patcher,s_gxm.color_f_id,SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,SCE_GXM_MULTISAMPLE_NONE,NULL,cv,&s_gxm.color_fp)<0)return -1;
    if(sceGxmShaderPatcherCreateFragmentProgram(s_gxm.patcher,s_gxm.color_f_id,SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,SCE_GXM_MULTISAMPLE_NONE,&alpha,cv,&s_gxm.color_alpha_fp)<0)return -1;

    mul.colorMask=SCE_GXM_COLOR_MASK_ALL;mul.colorFunc=SCE_GXM_BLEND_FUNC_ADD;mul.alphaFunc=SCE_GXM_BLEND_FUNC_ADD;
    mul.colorSrc=SCE_GXM_BLEND_FACTOR_DST_COLOR;mul.colorDst=SCE_GXM_BLEND_FACTOR_ZERO;
    mul.alphaSrc=SCE_GXM_BLEND_FACTOR_ONE;mul.alphaDst=SCE_GXM_BLEND_FACTOR_ZERO;
    if(sceGxmShaderPatcherCreateFragmentProgram(s_gxm.patcher,s_gxm.color_f_id,SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,SCE_GXM_MULTISAMPLE_NONE,&mul,cv,&s_gxm.color_multiply_fp)<0)return -1;

    nowrite.colorMask=SCE_GXM_COLOR_MASK_NONE;nowrite.colorFunc=SCE_GXM_BLEND_FUNC_ADD;nowrite.alphaFunc=SCE_GXM_BLEND_FUNC_ADD;
    nowrite.colorSrc=SCE_GXM_BLEND_FACTOR_ONE;nowrite.colorDst=SCE_GXM_BLEND_FACTOR_ZERO;
    nowrite.alphaSrc=SCE_GXM_BLEND_FACTOR_ONE;nowrite.alphaDst=SCE_GXM_BLEND_FACTOR_ZERO;
    if(sceGxmShaderPatcherCreateFragmentProgram(s_gxm.patcher,s_gxm.color_f_id,SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,SCE_GXM_MULTISAMPLE_NONE,&nowrite,cv,&s_gxm.color_nowrite_fp)<0)return -1;
    return 0;
}

static void gpu_release(void **mem, SceUID *uid) {
    if (*mem) {
        sceGxmUnmapMemory(*mem);
        *mem = NULL;
    }
    if (*uid >= 0) {
        sceKernelFreeMemBlock(*uid);
        *uid = -1;
    }
}

static void vertex_usse_release(void **mem, SceUID *uid) {
    if (*mem) {
        sceGxmUnmapVertexUsseMemory(*mem);
        *mem = NULL;
    }
    if (*uid >= 0) {
        sceKernelFreeMemBlock(*uid);
        *uid = -1;
    }
}

static void fragment_usse_release(void **mem, SceUID *uid) {
    if (*mem) {
        sceGxmUnmapFragmentUsseMemory(*mem);
        *mem = NULL;
    }
    if (*uid >= 0) {
        sceKernelFreeMemBlock(*uid);
        *uid = -1;
    }
}

static __attribute__((optimize("Os"))) void milestone_gxm_cleanup(void) {
    s32 i;
    if (s_gxm.context) tnt103a_profiled_gxm_finish(s_gxm.context);
    if (s_gxm.patcher) {
        if(s_gxm.texture_fp)sceGxmShaderPatcherReleaseFragmentProgram(s_gxm.patcher,s_gxm.texture_fp);
        if(s_gxm.texture_alpha_fp)sceGxmShaderPatcherReleaseFragmentProgram(s_gxm.patcher,s_gxm.texture_alpha_fp);
        if(s_gxm.texture_feedback_fp)sceGxmShaderPatcherReleaseFragmentProgram(s_gxm.patcher,s_gxm.texture_feedback_fp);
        if(s_gxm.texedge_fp)sceGxmShaderPatcherReleaseFragmentProgram(s_gxm.patcher,s_gxm.texedge_fp);
        if(s_gxm.texedge_alpha_fp)sceGxmShaderPatcherReleaseFragmentProgram(s_gxm.patcher,s_gxm.texedge_alpha_fp);
        if(s_gxm.color_fp)sceGxmShaderPatcherReleaseFragmentProgram(s_gxm.patcher,s_gxm.color_fp);
        if(s_gxm.color_alpha_fp)sceGxmShaderPatcherReleaseFragmentProgram(s_gxm.patcher,s_gxm.color_alpha_fp);
        if(s_gxm.color_multiply_fp)sceGxmShaderPatcherReleaseFragmentProgram(s_gxm.patcher,s_gxm.color_multiply_fp);
        if(s_gxm.color_nowrite_fp)sceGxmShaderPatcherReleaseFragmentProgram(s_gxm.patcher,s_gxm.color_nowrite_fp);
        if(s_gxm.texture_vp)sceGxmShaderPatcherReleaseVertexProgram(s_gxm.patcher,s_gxm.texture_vp);
        if(s_gxm.color_vp)sceGxmShaderPatcherReleaseVertexProgram(s_gxm.patcher,s_gxm.color_vp);
        /* Registered GXP IDs are separate patcher resources from the created
         * vertex/fragment programs.  Release created programs first so their
         * IDs are no longer in use, then unregister every ID that reached a
         * successful registration before destroying the patcher. */
        if(s_gxm.texture_v_id)sceGxmShaderPatcherUnregisterProgram(s_gxm.patcher,s_gxm.texture_v_id);
        if(s_gxm.texture_f_id)sceGxmShaderPatcherUnregisterProgram(s_gxm.patcher,s_gxm.texture_f_id);
        if(s_gxm.texedge_f_id)sceGxmShaderPatcherUnregisterProgram(s_gxm.patcher,s_gxm.texedge_f_id);
        if(s_gxm.color_v_id)sceGxmShaderPatcherUnregisterProgram(s_gxm.patcher,s_gxm.color_v_id);
        if(s_gxm.color_f_id)sceGxmShaderPatcherUnregisterProgram(s_gxm.patcher,s_gxm.color_f_id);
        sceGxmShaderPatcherDestroy(s_gxm.patcher);
        s_gxm.patcher=NULL;
    }
    if(s_gxm.render_target){sceGxmDestroyRenderTarget(s_gxm.render_target);s_gxm.render_target=NULL;}
    if(s_gxm.context){sceGxmDestroyContext(s_gxm.context);s_gxm.context=NULL;}
    gpu_release(&s_gxm.complete_texture_pack,&s_gxm.complete_texture_uid);
    gpu_release(&s_gxm.greek_complete_texture_pack,&s_gxm.greek_complete_texture_uid);
    gpu_release(&s_gxm.egyptian_complete_texture_pack,&s_gxm.egyptian_complete_texture_uid);
    gpu_release(&s_gxm.celtic_complete_texture_pack,&s_gxm.celtic_complete_texture_uid);
    gpu_release(&s_gxm.african_complete_texture_pack,&s_gxm.african_complete_texture_uid);
    gpu_release(&s_gxm.japanese_complete_texture_pack,&s_gxm.japanese_complete_texture_uid);
    gpu_release(&s_gxm.russian_complete_texture_pack,&s_gxm.russian_complete_texture_uid);
    for(i=0;i<TNT_GXM_TEXTURE_COUNT;i++)gpu_release(&s_gxm.texture_data[i],&s_gxm.texture_uid[i]);
    gpu_release((void**)&s_gxm.indices,&s_gxm.indices_uid);
    gpu_release((void**)&s_gxm.clear_vertices,&s_gxm.clear_vertices_uid);
    gpu_release((void**)&s_gxm.color_vertices,&s_gxm.color_vertices_uid);
    gpu_release((void**)&s_gxm.tex_vertices,&s_gxm.tex_vertices_uid);
    fragment_usse_release(&s_gxm.patcher_fragment_usse,&s_gxm.patcher_fragment_usse_uid);
    vertex_usse_release(&s_gxm.patcher_vertex_usse,&s_gxm.patcher_vertex_usse_uid);
    gpu_release(&s_gxm.patcher_buffer,&s_gxm.patcher_buffer_uid);
    gpu_release(&s_gxm.depth_data,&s_gxm.depth_uid);
    fragment_usse_release(&s_gxm.fragment_usse_ring,&s_gxm.fragment_usse_ring_uid);
    gpu_release(&s_gxm.fragment_ring,&s_gxm.fragment_uid);
    gpu_release(&s_gxm.vertex_ring,&s_gxm.vertex_uid);
    gpu_release(&s_gxm.vdm_ring,&s_gxm.vdm_uid);
    free(s_gxm.context_host);s_gxm.context_host=NULL;
    s_gxm.texture_fp=s_gxm.texture_alpha_fp=s_gxm.texture_feedback_fp=s_gxm.texedge_fp=s_gxm.texedge_alpha_fp=s_gxm.color_fp=s_gxm.color_alpha_fp=s_gxm.color_multiply_fp=s_gxm.color_nowrite_fp=NULL;
    s_gxm.texture_vp=s_gxm.color_vp=NULL;
    s_gxm.texture_v_id=s_gxm.texture_f_id=s_gxm.texedge_f_id=s_gxm.color_v_id=s_gxm.color_f_id=NULL;
    s_gxm.skybox_ready=FALSE;s_gxm.initialized=FALSE;
}

__attribute__((optimize("Os"))) int TNT_Vita_WonderMilestoneGxmInit(void) {
    SceGxmContextParams cp;SceGxmRenderTargetParams rp;SceGxmShaderPatcherParams sp;
    unsigned int aligned_w=ALIGN_UP(TNT_GXM_W,SCE_GXM_TILE_SIZEX),aligned_h=ALIGN_UP(TNT_GXM_H,SCE_GXM_TILE_SIZEY);
    unsigned int samples=aligned_w*aligned_h;s32 i;
    if(s_gxm.initialized)return 0;
    memset(&s_gxm,0,sizeof(s_gxm));
    s_gxm.vdm_uid=s_gxm.vertex_uid=s_gxm.fragment_uid=s_gxm.fragment_usse_ring_uid=-1;
    s_gxm.patcher_buffer_uid=s_gxm.patcher_vertex_usse_uid=s_gxm.patcher_fragment_usse_uid=-1;
    s_gxm.depth_uid=-1;
    s_gxm.complete_texture_uid=-1;
    s_gxm.greek_complete_texture_uid=-1;
    s_gxm.egyptian_complete_texture_uid=-1;
    s_gxm.celtic_complete_texture_uid=-1;
    s_gxm.african_complete_texture_uid=-1;
    s_gxm.japanese_complete_texture_uid=-1;
    s_gxm.russian_complete_texture_uid=-1;
    s_gxm.tex_vertices_uid=s_gxm.color_vertices_uid=s_gxm.clear_vertices_uid=s_gxm.indices_uid=-1;
    for(i=0;i<TNT_GXM_TEXTURE_COUNT;i++)s_gxm.texture_uid[i]=-1;

    s_gxm.vdm_ring=gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE,4,SCE_GXM_MEMORY_ATTRIB_READ,&s_gxm.vdm_uid);
    s_gxm.vertex_ring=gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE,4,SCE_GXM_MEMORY_ATTRIB_READ,&s_gxm.vertex_uid);
    s_gxm.fragment_ring=gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE,4,SCE_GXM_MEMORY_ATTRIB_READ,&s_gxm.fragment_uid);
    s_gxm.fragment_usse_ring=fragment_usse_alloc(SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE,&s_gxm.fragment_usse_ring_uid,&s_gxm.fragment_usse_ring_offset);
    if(!s_gxm.vdm_ring||!s_gxm.vertex_ring||!s_gxm.fragment_ring||!s_gxm.fragment_usse_ring)goto fail;
    memset(&cp,0,sizeof(cp));s_gxm.context_host=malloc(SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE);if(!s_gxm.context_host)goto fail;
    cp.hostMem=s_gxm.context_host;cp.hostMemSize=SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE;
    cp.vdmRingBufferMem=s_gxm.vdm_ring;cp.vdmRingBufferMemSize=SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE;
    cp.vertexRingBufferMem=s_gxm.vertex_ring;cp.vertexRingBufferMemSize=SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE;
    cp.fragmentRingBufferMem=s_gxm.fragment_ring;cp.fragmentRingBufferMemSize=SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE;
    cp.fragmentUsseRingBufferMem=s_gxm.fragment_usse_ring;cp.fragmentUsseRingBufferMemSize=SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE;cp.fragmentUsseRingBufferOffset=s_gxm.fragment_usse_ring_offset;
    if(sceGxmCreateContext(&cp,&s_gxm.context)<0)goto fail;

    memset(&rp,0,sizeof(rp));rp.width=TNT_GXM_W;rp.height=TNT_GXM_H;rp.scenesPerFrame=1;rp.multisampleMode=SCE_GXM_MULTISAMPLE_NONE;rp.driverMemBlock=-1;
    if(sceGxmCreateRenderTarget(&rp,&s_gxm.render_target)<0)goto fail;
    s_gxm.depth_data=gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,4*samples,SCE_GXM_DEPTHSTENCIL_SURFACE_ALIGNMENT,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.depth_uid);
    if(!s_gxm.depth_data)goto fail;
    if(sceGxmDepthStencilSurfaceInit(&s_gxm.depth_surface,SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24,SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,aligned_w,s_gxm.depth_data,NULL)<0)goto fail;


    s_gxm.patcher_buffer=gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,TNT_GXM_PATCHER_BUFFER_SIZE,4,SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,&s_gxm.patcher_buffer_uid);
    s_gxm.patcher_vertex_usse=vertex_usse_alloc(TNT_GXM_PATCHER_VERTEX_USSE_SIZE,&s_gxm.patcher_vertex_usse_uid,&s_gxm.patcher_vertex_usse_offset);
    s_gxm.patcher_fragment_usse=fragment_usse_alloc(TNT_GXM_PATCHER_FRAGMENT_USSE_SIZE,&s_gxm.patcher_fragment_usse_uid,&s_gxm.patcher_fragment_usse_offset);
    if(!s_gxm.patcher_buffer||!s_gxm.patcher_vertex_usse||!s_gxm.patcher_fragment_usse)goto fail;
    memset(&sp,0,sizeof(sp));sp.hostAllocCallback=patcher_host_alloc;sp.hostFreeCallback=patcher_host_free;
    sp.bufferMem=s_gxm.patcher_buffer;sp.bufferMemSize=TNT_GXM_PATCHER_BUFFER_SIZE;
    sp.vertexUsseMem=s_gxm.patcher_vertex_usse;sp.vertexUsseMemSize=TNT_GXM_PATCHER_VERTEX_USSE_SIZE;sp.vertexUsseOffset=s_gxm.patcher_vertex_usse_offset;
    sp.fragmentUsseMem=s_gxm.patcher_fragment_usse;sp.fragmentUsseMemSize=TNT_GXM_PATCHER_FRAGMENT_USSE_SIZE;sp.fragmentUsseOffset=s_gxm.patcher_fragment_usse_offset;
    if(sceGxmShaderPatcherCreate(&sp,&s_gxm.patcher)<0)goto fail;
    if(create_programs()<0)goto fail;

    s_gxm.tex_vertices=(TNTGxmTexVertex*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,sizeof(TNTGxmTexVertex)*TNT_GXM_MAX_TEX_VERTS,4,SCE_GXM_MEMORY_ATTRIB_READ,&s_gxm.tex_vertices_uid);
    s_gxm.color_vertices=(TNTGxmColorVertex*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,sizeof(TNTGxmColorVertex)*TNT_GXM_MAX_COLOR_VERTS,4,SCE_GXM_MEMORY_ATTRIB_READ,&s_gxm.color_vertices_uid);
    s_gxm.clear_vertices=(TNTGxmColorVertex*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,sizeof(TNTGxmColorVertex)*3,4,SCE_GXM_MEMORY_ATTRIB_READ,&s_gxm.clear_vertices_uid);
    s_gxm.indices=(u16*)gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,sizeof(u16)*TNT_GXM_MAX_TEX_VERTS,2,SCE_GXM_MEMORY_ATTRIB_READ,&s_gxm.indices_uid);
    if(!s_gxm.tex_vertices||!s_gxm.color_vertices||!s_gxm.clear_vertices||!s_gxm.indices)goto fail;
    for(i=0;i<TNT_GXM_MAX_TEX_VERTS;i++)s_gxm.indices[i]=(u16)i;
    s_gxm.clear_vertices[0]=(TNTGxmColorVertex){-1.0f,-1.0f,1.0f,0xffffffffU};
    s_gxm.clear_vertices[1]=(TNTGxmColorVertex){ 3.0f,-1.0f,1.0f,0xffffffffU};
    s_gxm.clear_vertices[2]=(TNTGxmColorVertex){-1.0f, 3.0f,1.0f,0xffffffffU};
    if(init_milestone_textures()<0)goto fail;
    s_gxm.skybox_ready=(u8)(s_gxm.texture_alpha_fp!=NULL&&init_skybox_textures()==0);
    if(init_greek_gameplay_textures()<0)goto fail;
    if(init_celtic_gameplay_textures()<0)goto fail;
    if(init_african_gameplay_textures()<0)goto fail;
    if(init_japanese_gameplay_textures()<0)goto fail;
    if(init_russian_gameplay_textures()<0)goto fail;
    if(init_intro_textures()<0)goto fail;
    if(init_font_c_texture()<0)goto fail;
    if(init_pause_text_texture()<0)goto fail;
    if(init_finale_hall_texture()<0)goto fail;
    if(init_locked_hall_texture()<0)goto fail;
    if(init_complete_textures()<0)goto fail;
    if(init_greek_complete_textures()<0)goto fail;
    if(init_egyptian_complete_textures()<0)goto fail;
    if(init_celtic_complete_textures()<0)goto fail;
    if(init_african_complete_textures()<0)goto fail;
    if(init_japanese_complete_textures()<0)goto fail;
    if(init_russian_complete_textures()<0)goto fail;
    s_gxm.initialized=TRUE;
    return 0;
fail:
    milestone_gxm_cleanup();
    return -1;
}

void TNT_Vita_WonderMilestoneGxmTerm(void) {
    milestone_gxm_cleanup();
}

int TNT_Vita_IntroGxmRender(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u8 white_fade) {
    f32 proj[16],ident[16];void *ub;u16 r;u8 ur;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f);
    f32 x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f);
    f32 y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1;
    s32 clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||!s_gxm.color_fp||!s_gxm.color_alpha_fp||build_intro_geometry(proj)<0)return -1;
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;

    /* Intro owns the whole frame. Clear color and depth on the GPU so the
     * 4:3 source scene sits on black without a 2 MB CPU framebuffer sweep. */
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,0,0,TNT_GXM_W-1,TNT_GXM_H-1);
    sceGxmSetViewport(s_gxm.context,TNT_GXM_W*0.5f,TNT_GXM_W*0.5f,TNT_GXM_H*0.5f,-TNT_GXM_H*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    s_gxm.clear_vertices[0].color=s_gxm.clear_vertices[1].color=s_gxm.clear_vertices[2].color=0xff000000U;
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);

    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    /* Resource 981 is authored for the full 400x300 viewport. Keep the exact
     * source horizontal mapping. TNTV repairs only truly untouched overscan
     * clear columns after GXM completes rather than stretching the 3D image. */
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
    for(r=0;r<s_gxm.run_count;r++){
        TNTGxmRun *run=&s_gxm.runs[r];
        sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
        sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
        sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
        sceGxmSetFragmentProgram(s_gxm.context,run->pad?s_gxm.texture_alpha_fp:s_gxm.texture_fp);
        sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);
    }

    /* Canonical untextured draws are last in the Borg command stream. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_alpha_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);
    for(ur=0;ur<s_intro_untex_runs;ur++){
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices+s_intro_untex_first[ur]);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,s_intro_untex_count[ur]);
    }

    if(s_intro_shade_count){
        sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);
        sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
        sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_multiply_fp);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices+s_intro_shade_first);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,s_intro_shade_count);
    }
    if(s_intro_fog_count){
        sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_alpha_fp);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices+s_intro_fog_first);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,s_intro_fog_count);
    }

    if(white_fade){
        u32 c=0x00ffffffU|((u32)white_fade<<24);
        /* Preserve the source 400x300 fade. The frontend pillarbox is drawn
         * afterward and never participates in the fade. */
        sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
        sceGxmSetViewport(s_gxm.context,TNT_GXM_W*0.5f,TNT_GXM_W*0.5f,TNT_GXM_H*0.5f,-TNT_GXM_H*0.5f,0.5f,0.5f);
        sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
        sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
        sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
        sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_alpha_fp);
        sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
        s_gxm.clear_vertices[0].color=s_gxm.clear_vertices[1].color=s_gxm.clear_vertices[2].color=c;
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);
    }

    /* Stable frontend pillarbox: draw black last so no intro geometry, fade,
     * natural completion, or user skip can leak sand-colored pixels into the
     * Vita-only side regions. */
    sceGxmSetViewport(s_gxm.context,TNT_GXM_W*0.5f,TNT_GXM_W*0.5f,TNT_GXM_H*0.5f,-TNT_GXM_H*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    s_gxm.clear_vertices[0].color=s_gxm.clear_vertices[1].color=s_gxm.clear_vertices[2].color=0xff000000U;
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);
    if(clip_x0>0){
        sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,0,0,(unsigned)(clip_x0-1),TNT_GXM_H-1);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);
    }
    if(clip_x1<TNT_GXM_W-1){
        sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)(clip_x1+1),0,TNT_GXM_W-1,TNT_GXM_H-1);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);
    }
    sceGxmEndScene(s_gxm.context,NULL,NULL);tnt103a_profiled_gxm_finish(s_gxm.context);return 0;
}

__attribute__((optimize("O2"))) int TNT_Vita_MayanSkyboxGxmRender(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,f32 angle_degrees) {
    f32 proj[16],ident[16];void *ub;u16 r;
    TNTGxmTexVertex *overlay=s_gxm.tex_vertices+TNT_GXM_OVERLAY_VERTEX_BASE;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f);
    f32 x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f);
    f32 y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1;
    s32 clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||!s_gxm.skybox_ready||build_skybox_geometry(angle_degrees)<0)return -1;
    if(clip_x0<0)clip_x0=0;
    if(clip_y0<0)clip_y0=0;
    if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;
    if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    skybox_projection_matrix(proj);memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    set_overlay_quad(overlay,87.0f,30.0f,189.0f,234.0f);
    set_overlay_quad(overlay+6,0.0f,0.0f,400.0f,300.0f);
    set_overlay_quad(overlay+12,210.0f,20.0f,336.0f,101.0f);
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    /* Mayan source order begins with the moon on the dark base, then the
     * translucent Borg skybox.  Draw that underlay on GXM instead of copying
     * a 960x544 CPU framebuffer (~2 MiB) every frame. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    s_gxm.clear_vertices[0].color=s_gxm.clear_vertices[1].color=s_gxm.clear_vertices[2].color=0xff281414U;
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);

    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_MAYAN_MOON_TEXTURE]);
    sceGxmSetVertexStream(s_gxm.context,0,overlay+12);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);

    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
    for(r=0;r<s_gxm.run_count;r++){
        TNTGxmRun *run=&s_gxm.runs[r];sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);
    }

    /* Source order after the Borg skybox is the semitransparent playfield mask
     * followed by the alpha-masked 400x300 Mayan foreground artwork. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_MAYAN_PLAYFIELD_TEXTURE]);
    sceGxmSetVertexStream(s_gxm.context,0,overlay);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_MAYAN_BACKGROUND_TEXTURE]);
    sceGxmSetVertexStream(s_gxm.context,0,overlay+6);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    sceGxmEndScene(s_gxm.context,NULL,NULL);tnt103a_profiled_gxm_finish(s_gxm.context);
    return 0;
}


static int render_greek_borg_pass(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,const f32 proj[16],u8 alpha_blend,s32 overlay_texture) {
    f32 ident[16];void *ub;u16 r;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f),x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f),y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1,clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);
    sceGxmSetFragmentProgram(s_gxm.context,alpha_blend?s_gxm.texture_alpha_fp:s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
    for(r=0;r<s_gxm.run_count;r++){TNTGxmRun *run=&s_gxm.runs[r];sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);}
    if(overlay_texture>=0){
        TNTGxmTexVertex *overlay=s_gxm.tex_vertices+TNT_GXM_OVERLAY_VERTEX_BASE;
        set_overlay_quad(overlay,0.0f,0.0f,400.0f,300.0f);
        sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
        sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
        sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
        sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp);
        sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
        sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[overlay_texture]);sceGxmSetVertexStream(s_gxm.context,0,overlay);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    }
    sceGxmEndScene(s_gxm.context,NULL,NULL);tnt103a_profiled_gxm_finish(s_gxm.context);return 0;
}

int TNT_Vita_GreekSkyboxGxmRender(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,f32 angle_degrees) {
    f32 proj[16];if(!s_gxm.initialized||build_greek_skybox_geometry(angle_degrees)<0)return -1;
    skybox_projection_matrix(proj);return render_greek_borg_pass(surface,sync,board_x,board_y,cell,proj,FALSE,TNT_GXM_GREEK_BACKGROUND_TEXTURE);
}

int TNT_Vita_GreekRainGxmRender(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u32 anim_ticks) {
    f32 proj[16];u8 alpha=255;u32 i;
    u32 *full=(u32*)s_gxm.texture_data[TNT_GXM_GREEK_RAIN_TEXTURE],*fade=(u32*)s_gxm.texture_data[TNT_GXM_GREEK_RAIN_ALPHA_TEXTURE];
    if(!s_gxm.initialized||!s_gxm.texture_alpha_fp||!full||!fade||build_greek_rain_geometry(anim_ticks,&alpha,FALSE)<0)return -1;
    for(i=0;i<1376U;i++)fade[i]=(full[i]&0x00ffffffU)|((u32)alpha<<24);
    greek_rain_projection_matrix(proj);return render_greek_borg_pass(surface,sync,board_x,board_y,cell,proj,TRUE,-1);
}


/* TEST117A: nearest, opaque, one-to-one restore of CPU-rasterized artwork.
 * Texture participates in the existing allocation/termination lifecycle. */
int TNT_Vita_StaticFrameGxmRestore(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    const u32 *rgba, u32 generation
) {
    const u32 index = TNT_GXM_STATIC_FRAME_TEXTURE;
    f32 ident[16]; void *ub; u32 y;
    TNTGxmTexVertex *quad;
    if (!s_gxm.initialized || !s_gxm.texture_fp || !rgba) return -1;
    if (!s_gxm.texture_data[index]) {
        s_gxm.static_frame_valid = FALSE;
        s_gxm.texture_data[index] = gpu_alloc(
            SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE, 1024U*TNT_GXM_H*4U,
            SCE_GXM_TEXTURE_ALIGNMENT,
            SCE_GXM_MEMORY_ATTRIB_READ|SCE_GXM_MEMORY_ATTRIB_WRITE,
            &s_gxm.texture_uid[index]);
        if (!s_gxm.texture_data[index]) return -1;
    }
    if (!s_gxm.static_frame_valid ||
        s_gxm.static_frame_generation != generation) {
        u32 *dst = (u32 *)s_gxm.texture_data[index];
        /* Previous restores finish before returning, so texture is idle. */
        for (y=0; y<TNT_GXM_H; y++)
            memcpy(dst+y*1024U, rgba+y*1024U, TNT_GXM_W*4U);
        if (sceGxmTextureInitLinearStrided(&s_gxm.textures[index], dst,
            SCE_GXM_TEXTURE_FORMAT_A8B8G8R8, TNT_GXM_W, TNT_GXM_H, 1024U*4U)<0)
            return -1;
        sceGxmTextureSetMinFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMagFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_FILTER_POINT);
        sceGxmTextureSetMipFilter(&s_gxm.textures[index],SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
        sceGxmTextureSetUAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(&s_gxm.textures[index],SCE_GXM_TEXTURE_ADDR_CLAMP);
        if (sceGxmTextureValidate(&s_gxm.textures[index])<0) return -1;
        s_gxm.static_frame_generation = generation;
        s_gxm.static_frame_valid = TRUE;
    }
    memset(ident,0,sizeof(ident)); ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    quad=s_gxm.tex_vertices+TNT_GXM_OVERLAY_VERTEX_BASE;
    set_overlay_quad(quad,0.0f,0.0f,400.0f,300.0f);
    if (sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,
        sync,surface,&s_gxm.depth_surface)<0) return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,0,0,TNT_GXM_W-1,TNT_GXM_H-1);
    sceGxmSetViewport(s_gxm.context,TNT_GXM_W*0.5f,TNT_GXM_W*0.5f,
        TNT_GXM_H*0.5f,-TNT_GXM_H*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);
    sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[index]);
    sceGxmSetVertexStream(s_gxm.context,0,quad);
    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,
        SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    sceGxmEndScene(s_gxm.context,NULL,NULL);
    tnt103a_profiled_gxm_finish(s_gxm.context);
    return 0;
}

int TNT_Vita_GreekGameplayGxmRender(
    SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,f32 angle_degrees,
    u32 rain_ticks,u8 fire_frame,const char *player_name
) {
    f32 sky_proj[16],rain_proj[16],ident[16];void *ub;u16 r,sky_runs,rain_run_start;
    u8 rain_alpha=255,name_w;u32 i;
    u32 *full=(u32*)s_gxm.texture_data[TNT_GXM_GREEK_RAIN_TEXTURE];
    u32 *fade=(u32*)s_gxm.texture_data[TNT_GXM_GREEK_RAIN_ALPHA_TEXTURE];
    TNTGxmTexVertex *overlay=s_gxm.tex_vertices+TNT_GXM_OVERLAY_VERTEX_BASE;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f),x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f),y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1,clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||!s_gxm.color_fp||!s_gxm.texture_alpha_fp||!full||!fade||fire_frame>=TNT_GXM_GREEK_FIRE_TEXTURE_COUNT)return -1;
    if(build_greek_skybox_geometry(angle_degrees)<0)return -1;
    sky_runs=s_gxm.run_count;
    if(build_greek_rain_geometry(rain_ticks,&rain_alpha,TRUE)<0)return -1;
    rain_run_start=sky_runs;
    for(i=0;i<1376U;i++)fade[i]=(full[i]&0x00ffffffU)|((u32)rain_alpha<<24);
    name_w=update_greek_name_texture(player_name);
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    skybox_projection_matrix(sky_proj);greek_rain_projection_matrix(rain_proj);
    memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    set_overlay_quad(overlay,0.0f,0.0f,400.0f,300.0f);
    if(name_w)set_overlay_quad_uv(overlay+6,293.0f,201.0f,293.0f+(f32)name_w,213.0f,
        (f32)name_w/(f32)TNT_GXM_GREEK_NAME_W,12.0f/(f32)TNT_GXM_GREEK_NAME_H);
    set_overlay_quad(overlay+12,271.0f,15.0f,374.0f,86.0f);

    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);

    /* One GXM scene owns the complete source order: 1399 sky -> static
     * foreground -> player name -> fire -> 1318 rain. This avoids the old
     * sky/CPU/rain split that forced two sceGxmFinish waits per gameplay frame. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    /* TEST116A: replace the CPU uniform-base memcpy with a full visible
     * surface clear. Exact COLOR(20,20,40), no blending. Preserve the
     * existing depth clear and restore source viewport before sky drawing. */
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,0,0,TNT_GXM_W-1,TNT_GXM_H-1);
    sceGxmSetViewport(s_gxm.context,TNT_GXM_W*0.5f,TNT_GXM_W*0.5f,TNT_GXM_H*0.5f,-TNT_GXM_H*0.5f,0.5f,0.5f);
    s_gxm.clear_vertices[0].color=s_gxm.clear_vertices[1].color=s_gxm.clear_vertices[2].color=0xff281414U;
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);

    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,sky_proj);
    for(r=0;r<sky_runs;r++){TNTGxmRun *run=&s_gxm.runs[r];sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);}

    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_GREEK_BACKGROUND_TEXTURE]);sceGxmSetVertexStream(s_gxm.context,0,overlay);
    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    if(name_w){sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_GREEK_NAME_TEXTURE]);sceGxmSetVertexStream(s_gxm.context,0,overlay+6);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);}
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_GREEK_FIRE_TEXTURE_BASE+fire_frame]);sceGxmSetVertexStream(s_gxm.context,0,overlay+12);
    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);

    /* Rain's old separate GXM scene started from a fresh depth buffer. Clear
     * depth only here so combining submissions preserves that behavior without
     * touching the color layers already composed above. */
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);

    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,rain_proj);
    for(r=rain_run_start;r<s_gxm.run_count;r++){TNTGxmRun *run=&s_gxm.runs[r];sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);}
    sceGxmEndScene(s_gxm.context,NULL,NULL);tnt103a_profiled_gxm_finish(s_gxm.context);
    return 0;
}

int TNT_Vita_CelticGameplayGxmRender(
    SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u32 sky_ticks,u32 leaf_ticks,
    u8 lamp_frame,const char *player_name
) {
    f32 sky_proj[16],leaf_proj[16],ident[16];void *ub;u16 r,sky_runs,sky_untex_count=0,leaf_first=0,leaf_count=0;u8 sky_alpha=255U,name_w;
    TNTGxmTexVertex *overlay=s_gxm.tex_vertices+TNT_GXM_OVERLAY_VERTEX_BASE;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f),x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f),y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1,clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||!s_gxm.color_fp||!s_gxm.texture_alpha_fp||lamp_frame>=TNT_GXM_CELTIC_LAMP_TEXTURE_COUNT)return -1;
    if(build_celtic_sky_geometry(sky_ticks,&sky_alpha,&sky_untex_count)<0)return -1;
    sky_runs=s_gxm.run_count;
    if(build_celtic_leaf_geometry(leaf_ticks,&leaf_first,&leaf_count)<0)return -1;
    name_w=update_greek_name_texture(player_name);
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    celtic_sky_projection_matrix(sky_proj);celtic_leaf_projection_matrix(leaf_proj);memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    set_overlay_quad(overlay,0.0f,0.0f,400.0f,300.0f);
    if(name_w)set_overlay_quad_uv(overlay+6,293.0f,202.0f,293.0f+(f32)name_w,214.0f,(f32)name_w/(f32)TNT_GXM_GREEK_NAME_W,12.0f/(f32)TNT_GXM_GREEK_NAME_H);
    set_overlay_quad(overlay+12,212.0f,176.0f,273.0f,236.0f);

    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);

    /* TEST118A: exact uniform base and depth clear before original Celtic layers. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    /* TEST118A: replace the CPU uniform-base memcpy with a full visible
     * surface clear. Exact COLOR(20,20,40), no blending. Preserve the
     * existing depth clear and restore source viewport before sky drawing. */
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,0,0,TNT_GXM_W-1,TNT_GXM_H-1);
    sceGxmSetViewport(s_gxm.context,TNT_GXM_W*0.5f,TNT_GXM_W*0.5f,TNT_GXM_H*0.5f,-TNT_GXM_H*0.5f,0.5f,0.5f);
    s_gxm.clear_vertices[0].color=s_gxm.clear_vertices[1].color=s_gxm.clear_vertices[2].color=0xff281414U;
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);

    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,sky_proj);
    if(sky_untex_count){sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,sky_untex_count);}

    /* The four large cloud/sky meshes animate primitive alpha. N64 selects
     * XLU_SURF | Z_UPD, so translucent runs blend but still write depth. */
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,sky_proj);
    for(r=0;r<sky_runs;r++){
        TNTGxmRun *run=&s_gxm.runs[r];sceGxmSetFragmentProgram(s_gxm.context,run->pad?s_gxm.texture_alpha_fp:s_gxm.texture_fp);
        sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);
    }

    /* Static Celtic foreground, player name and D_800D2008 lamp. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_CELTIC_BACKGROUND_TEXTURE]);sceGxmSetVertexStream(s_gxm.context,0,overlay);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    if(name_w){sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_GREEK_NAME_TEXTURE]);sceGxmSetVertexStream(s_gxm.context,0,overlay+6);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);}
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_CELTIC_LAMP_TEXTURE_BASE+lamp_frame]);sceGxmSetVertexStream(s_gxm.context,0,overlay+12);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);

    /* func_8005BE40 occurs immediately before scene 1335. Give the leaves a
     * fresh depth field while retaining all already-composited color layers. */
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);

    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,leaf_proj);
    if(leaf_count){sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices+leaf_first);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,leaf_count);}
    sceGxmEndScene(s_gxm.context,NULL,NULL);tnt103a_profiled_gxm_finish(s_gxm.context);return 0;
}

int TNT_Vita_AfricanGameplayGxmRender(
    SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u32 sky_ticks,u8 smoke_frame,
    const char *player_name
) {
    f32 sky_proj[16],ident[16];void *ub;u16 r;u8 name_w;
    TNTGxmTexVertex *overlay=s_gxm.tex_vertices+TNT_GXM_OVERLAY_VERTEX_BASE;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f),x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f),y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1,clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||!s_gxm.color_fp||!s_gxm.texture_alpha_fp||!s_gxm.texedge_alpha_fp||smoke_frame>=TNT_GXM_AFRICAN_SMOKE_TEXTURE_COUNT)return -1;
    if(build_african_sky_geometry(sky_ticks)<0)return -1;
    name_w=update_greek_name_texture(player_name);
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    african_sky_projection_matrix(sky_proj);memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    set_overlay_quad(overlay,0.0f,0.0f,400.0f,300.0f);
    if(name_w)set_overlay_quad_uv(overlay+6,293.0f,202.0f,293.0f+(f32)name_w,214.0f,(f32)name_w/(f32)TNT_GXM_GREEK_NAME_W,12.0f/(f32)TNT_GXM_GREEK_NAME_H);
    set_overlay_quad(overlay+12,256.0f,13.0f,387.0f,119.0f);

    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);

    /* TEST119A: exact uniform base and depth before original African layers. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    /* TEST119A: replace the CPU uniform-base memcpy with a full visible
     * surface clear. Exact COLOR(20,20,40), no blending. Preserve the
     * existing depth clear and restore source viewport before sky drawing. */
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,0,0,TNT_GXM_W-1,TNT_GXM_H-1);
    sceGxmSetViewport(s_gxm.context,TNT_GXM_W*0.5f,TNT_GXM_W*0.5f,TNT_GXM_H*0.5f,-TNT_GXM_H*0.5f,0.5f,0.5f);
    s_gxm.clear_vertices[0].color=s_gxm.clear_vertices[1].color=s_gxm.clear_vertices[2].color=0xff281414U;
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);

    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,sky_proj);
    /* The translucent African layer is XLU_SURF | Z_UPD on N64, so keep
     * depth writes enabled while selecting alpha blending for that run. */
    for(r=0;r<s_gxm.run_count;r++){TNTGxmRun *run=&s_gxm.runs[r];
        sceGxmSetFragmentProgram(s_gxm.context,run->pad?s_gxm.texture_alpha_fp:s_gxm.texture_fp);
        sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);
    }

    /* Static African foreground -> player name -> D_800D2C38 volcano smoke. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_AFRICAN_BACKGROUND_TEXTURE]);sceGxmSetVertexStream(s_gxm.context,0,overlay);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    if(name_w){sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_GREEK_NAME_TEXTURE]);sceGxmSetVertexStream(s_gxm.context,0,overlay+6);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);}
    /* func_8005BBFC enables G_AC_THRESHOLD before the original CI8 smoke
     * sprite.  The only transparent TLUT entry is the hidden #FF0042 key;
     * threshold it after linear filtering so that color cannot leak out as
     * the red halo seen with plain straight-alpha blending. */
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texedge_alpha_fp);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_AFRICAN_SMOKE_TEXTURE_BASE+smoke_frame]);sceGxmSetVertexStream(s_gxm.context,0,overlay+12);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    sceGxmEndScene(s_gxm.context,NULL,NULL);tnt103a_profiled_gxm_finish(s_gxm.context);return 0;
}

static u8 japanese_sparkle_state(u32 ticks,u16 delay,u8 *frame,u8 *alpha) {
    u32 cycle=(u32)delay+14U,phase,idx,step;
    if(!ticks)return FALSE;
    phase=((ticks-1U)%cycle)+1U;
    if(phase<(u32)delay+2U||phase>(u32)delay+13U)return FALSE;
    idx=phase-((u32)delay+2U);
    *frame=(u8)((idx+1U)%TNT_JAPANESE_SPARKLE_FRAME_COUNT);
    step=idx<6U?idx+1U:11U-idx;
    *alpha=(u8)((step*255U)/6U);
    return TRUE;
}

int TNT_Vita_JapaneseGameplayGxmRender(
    SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,f32 sky_angle,u32 effect_ticks,
    u32 butterfly_ticks,const char *player_name
) {
    static const u16 sparkle_delay[5]={60U,80U,100U,115U,70U};
    static const s16 sparkle_x[5]={17,105,220,237,301};
    static const s16 sparkle_y[5]={252,258,210,246,257};
    f32 sky_proj[16],butterfly_proj[16],ident[16];void *ub;
    u16 r,sky_runs,butterfly_first=0,butterfly_count=0;u8 name_w,water_frame;
    u8 sparkle_visible[5]={0},sparkle_frame[5]={0},sparkle_alpha[5]={0};s32 i;
    TNTGxmTexVertex *overlay=s_gxm.tex_vertices+TNT_GXM_OVERLAY_VERTEX_BASE;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f),x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f),y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1,clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||!s_gxm.color_fp||!s_gxm.texture_alpha_fp||!s_gxm.texedge_alpha_fp)return -1;
    if(build_japanese_sky_geometry(sky_angle)<0)return -1;sky_runs=s_gxm.run_count;
    if(build_japanese_butterfly_geometry(butterfly_ticks,&butterfly_first,&butterfly_count)<0)return -1;
    water_frame=(u8)(effect_ticks?(((effect_ticks+5U)/6U)%TNT_JAPANESE_WATER_FRAME_COUNT):0U);
    for(i=0;i<5;i++){
        sparkle_visible[i]=japanese_sparkle_state(effect_ticks,sparkle_delay[i],&sparkle_frame[i],&sparkle_alpha[i]);
        if(sparkle_visible[i])update_japanese_sparkle_texture((u8)i,sparkle_frame[i],sparkle_alpha[i]);
    }
    name_w=update_greek_name_texture(player_name);
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    japanese_sky_projection_matrix(sky_proj);japanese_butterfly_projection_matrix(butterfly_proj);
    memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    set_overlay_quad(overlay,0.0f,0.0f,400.0f,300.0f);
    if(name_w)set_overlay_quad_uv(overlay+6,293.0f,202.0f,293.0f+(f32)name_w,214.0f,(f32)name_w/(f32)TNT_GXM_GREEK_NAME_W,12.0f/(f32)TNT_GXM_GREEK_NAME_H);
    set_overlay_quad(overlay+12,4.0f,256.0f,258.0f,300.0f);
    for(i=0;i<5;i++)set_overlay_quad(overlay+18+i*6,(f32)sparkle_x[i],(f32)sparkle_y[i],(f32)sparkle_x[i]+30.0f,(f32)sparkle_y[i]+29.0f);

    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);

    /* TEST120A: exact uniform-color and depth clear before Japanese sky. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    /* TEST120A: replace the CPU uniform-base memcpy with a full visible
     * surface clear. Exact COLOR(20,20,40), no blending. Preserve the
     * existing depth clear and restore source viewport before sky drawing. */
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,0,0,TNT_GXM_W-1,TNT_GXM_H-1);
    sceGxmSetViewport(s_gxm.context,TNT_GXM_W*0.5f,TNT_GXM_W*0.5f,TNT_GXM_H*0.5f,-TNT_GXM_H*0.5f,0.5f,0.5f);
    s_gxm.clear_vertices[0].color=s_gxm.clear_vertices[1].color=s_gxm.clear_vertices[2].color=0xff281414U;
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);

    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,sky_proj);
    for(r=0;r<sky_runs;r++){TNTGxmRun *run=&s_gxm.runs[r];sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);}

    /* Source order: static foreground -> name -> water -> five sparkle instances. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
    /* func_8005BBFC sets G_AC_THRESHOLD + G_RM_XLU_SURF before this CI8
     * background. Palette entries 247/248 are the transparent #FF0042 key;
     * drawing this quad opaquely was the solid red field seen on test1. */
    /* Japanese foreground is point-filtered CI8 with binary RGBA16 alpha.
     * TEX_EDGE already discards the transparent #FF0042 key, so blending the
     * surviving alpha-255 texels is unnecessary and can expose the underlying
     * buffer along the repaired flat-panel bottom fringe.  Write those pixels
     * directly/opaquely while retaining the alpha kill. */
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texedge_fp);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_JAPANESE_BACKGROUND_TEXTURE]);sceGxmSetVertexStream(s_gxm.context,0,overlay);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp);
    if(name_w){sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_GREEK_NAME_TEXTURE]);sceGxmSetVertexStream(s_gxm.context,0,overlay+6);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);}
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_JAPANESE_WATER_TEXTURE_BASE+water_frame]);sceGxmSetVertexStream(s_gxm.context,0,overlay+12);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp);
    for(i=0;i<5;i++)if(sparkle_visible[i]){sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_JAPANESE_SPARKLE_TEXTURE_BASE+i]);sceGxmSetVertexStream(s_gxm.context,0,overlay+18+i*6);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);}

    /* func_8005BE40 precedes scene 1360/animation 1361, so butterflies begin
     * with a fresh depth field without disturbing the already composed color. */
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,butterfly_proj);
    if(butterfly_count){sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices+butterfly_first);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,butterfly_count);}
    sceGxmEndScene(s_gxm.context,NULL,NULL);tnt103a_profiled_gxm_finish(s_gxm.context);return 0;
}

/* Russian gameplay production baseline (validated Test 26).  The source ROM's
 * scene-901 cloud presentation does not reproduce safely on Vita and can erase
 * the gameplay board.  Keep the source-derived/passable 400x300 sky plus the
 * isolated 892 flare baked into one flat image; the original cathedral/name/
 * candles and foreground snow remain live.  Revisit only during polish. */
/* EXPERIMENTAL TEST200Q: 400x300 source-ROM material is synthesized
 * before any game/attract frame. Original accepted baked bitmap is NOT
 * shipped. All prior Russian gameplay/sky/cathedral/snow GPU geometry
 * and timing below are preserved byte-exact; only texture pixels differ. */
u32 g_tnt_russian_sky_baseline_rgba[400*300];

void TNT_Vita_RussianGameplayGxmReset(void) {
    /* Russian uses the credits 400x300 allocation as a shared scratch texture.
     * Upload the approved baked sky at gameplay-entry time, not conditionally
     * on the first rendered tick. Attract setup can legitimately consume 2+
     * retraces before its first frame, which previously skipped a sky_ticks<2
     * upload and exposed stale/blank scratch contents. */
    if(s_gxm.texture_data[TNT_GXM_CREDITS_TEXTURE])
        memcpy(s_gxm.texture_data[TNT_GXM_CREDITS_TEXTURE],g_tnt_russian_sky_baseline_rgba,sizeof(g_tnt_russian_sky_baseline_rgba));
}

int TNT_Vita_RussianGameplayGxmRender(
    SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u32 sky_ticks,u32 snow_ticks,
    const u8 *candle_frame,const char *player_name
) {
    f32 snow_proj[16],ident[16];void *ub;u16 snow_first=0,snow_count=0;u8 name_w;s32 i;
    TNTGxmTexVertex *overlay=s_gxm.tex_vertices+TNT_GXM_OVERLAY_VERTEX_BASE;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f),x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f),y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1,clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||!s_gxm.color_fp||!s_gxm.texture_alpha_fp||!s_gxm.texedge_fp||!s_gxm.texedge_alpha_fp||!candle_frame||!s_gxm.texture_data[TNT_GXM_CREDITS_TEXTURE])return -1;
    /* The 400x300 credits slot is an intentional shared scratch texture.  It
     * is never used by credits and Russian gameplay at the same time. */
    if(sky_ticks<2U)memcpy(s_gxm.texture_data[TNT_GXM_CREDITS_TEXTURE],g_tnt_russian_sky_baseline_rgba,sizeof(g_tnt_russian_sky_baseline_rgba));
    reset_geometry();
    if(build_russian_snow_geometry(snow_ticks,&snow_first,&snow_count)<0)return -1;
    name_w=update_greek_name_texture(player_name);
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    russian_snow_projection_matrix(snow_proj);memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    set_overlay_quad(overlay,0.0f,0.0f,400.0f,300.0f);
    if(name_w)set_overlay_quad_uv(overlay+6,293.0f,202.0f,293.0f+(f32)name_w,214.0f,(f32)name_w/(f32)TNT_GXM_GREEK_NAME_W,12.0f/(f32)TNT_GXM_GREEK_NAME_H);
    for(i=0;i<TNT_RUSSIAN_CANDLE_INSTANCE_COUNT;i++)set_overlay_quad(overlay+12+i*6,(f32)g_tnt_russian_candle_x[i],(f32)g_tnt_russian_candle_y[i],(f32)g_tnt_russian_candle_x[i]+2.0f,(f32)g_tnt_russian_candle_y[i]+6.0f);
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    /* TEST121A: Russian approved baked sky/cathedral/candles/snow remain unchanged.
     * Clear exact full-visible uniform underlay by GPU inside current scene.
     * Keep separate fresh-depth snow pass later in original renderer. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    /* TEST121A: replace the CPU uniform-base memcpy with a full visible
     * surface clear. Exact COLOR(20,20,40), no blending. Preserve the
     * existing depth clear and restore source viewport before sky drawing. */
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,0,0,TNT_GXM_W-1,TNT_GXM_H-1);
    sceGxmSetViewport(s_gxm.context,TNT_GXM_W*0.5f,TNT_GXM_W*0.5f,TNT_GXM_H*0.5f,-TNT_GXM_H*0.5f,0.5f,0.5f);
    s_gxm.clear_vertices[0].color=s_gxm.clear_vertices[1].color=s_gxm.clear_vertices[2].color=0xff281414U;
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);

    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_CREDITS_TEXTURE]);sceGxmSetVertexStream(s_gxm.context,0,overlay);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texedge_fp);sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_RUSSIAN_BACKGROUND_TEXTURE]);sceGxmSetVertexStream(s_gxm.context,0,overlay);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp);if(name_w){sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_GREEK_NAME_TEXTURE]);sceGxmSetVertexStream(s_gxm.context,0,overlay+6);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);}
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texedge_alpha_fp);
    for(i=0;i<TNT_RUSSIAN_CANDLE_INSTANCE_COUNT;i++){u8 fr=(u8)(candle_frame[i]%TNT_RUSSIAN_CANDLE_FRAME_COUNT);sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_RUSSIAN_CANDLE_TEXTURE_BASE+fr]);sceGxmSetVertexStream(s_gxm.context,0,overlay+12+i*6);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);}
    /* Snow gets a clean depth field.  This is the proven Test-14/Test-26 shell
     * that keeps the gameplay board safe while retaining the visible flakes. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,snow_proj);
    if(snow_count){sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices+snow_first);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,snow_count);}
    sceGxmEndScene(s_gxm.context,NULL,NULL);tnt103a_profiled_gxm_finish(s_gxm.context);return 0;
}

static u32 haluci_pack_rgba(u8 r,u8 g,u8 b,u8 a) {
    return (u32)r|((u32)g<<8)|((u32)b<<16)|((u32)a<<24);
}

static u32 haluci_modulated_color(const TNTHaluciParticle *p,f32 intensity) {
    u32 r=(u32)((f32)p->r*intensity+0.5f);
    u32 g=(u32)((f32)p->g*intensity+0.5f);
    u32 b=(u32)((f32)p->b*intensity+0.5f);
    u32 a=(u32)(153.0f*intensity+0.5f); /* source particle alpha = 0.6 */
    if(r>255U)r=255U;if(g>255U)g=255U;if(b>255U)b=255U;if(a>255U)a=255U;
    return haluci_pack_rgba((u8)r,(u8)g,(u8)b,(u8)a);
}

static int build_haluci_color_geometry(const TNTHaluciFrame *frame) {
    static const f32 cs[9]={1.0f,0.70710678f,0.0f,-0.70710678f,-1.0f,-0.70710678f,0.0f,0.70710678f,1.0f};
    static const f32 sn[9]={0.0f,0.70710678f,1.0f,0.70710678f,0.0f,-0.70710678f,-1.0f,-0.70710678f,0.0f};
    u16 i;
    /* HALUCI shares the completion color staging buffer but builds it without
     * reset_geometry(). Invalidate any remembered completed-Wonder geometry
     * before overwriting that storage so a later identical story pose can
     * never reuse HALUCI vertices. */
    s_complete_geometry_cache_valid=FALSE;
    s_gxm.color_count=0;
    if(!frame)return -1;
    for(i=0;i<frame->particle_count;i++){
        const TNTHaluciParticle *p=&frame->particle[i];
        f32 inner=p->half*0.60f,outer=p->half;
        f32 cx=(p->cx/200.0f)-1.0f;
        f32 cy=1.0f-(p->cy/150.0f);
        f32 irx=inner/200.0f,iry=inner/150.0f;
        f32 orx=outer/200.0f,ory=outer/150.0f;
        u32 cc=haluci_modulated_color(p,0.20f);
        u32 ci=haluci_modulated_color(p,1.00f);
        u32 co=haluci_modulated_color(p,0.00f);
        int seg;
        if((u32)s_gxm.color_count+72U>TNT_GXM_MAX_COLOR_VERTS)return -1;
        for(seg=0;seg<8;seg++){
            f32 i0x=cx+cs[seg]*irx,i0y=cy-sn[seg]*iry;
            f32 i1x=cx+cs[seg+1]*irx,i1y=cy-sn[seg+1]*iry;
            f32 o0x=cx+cs[seg]*orx,o0y=cy-sn[seg]*ory;
            f32 o1x=cx+cs[seg+1]*orx,o1y=cy-sn[seg+1]*ory;
            TNTGxmColorVertex *v=&s_gxm.color_vertices[s_gxm.color_count];
            /* Dim center -> bright source ring. */
            v[0]=(TNTGxmColorVertex){cx,cy,0.0f,cc};
            v[1]=(TNTGxmColorVertex){i0x,i0y,0.0f,ci};
            v[2]=(TNTGxmColorVertex){i1x,i1y,0.0f,ci};
            /* Bright ring -> transparent outer edge. */
            v[3]=(TNTGxmColorVertex){i0x,i0y,0.0f,ci};
            v[4]=(TNTGxmColorVertex){o0x,o0y,0.0f,co};
            v[5]=(TNTGxmColorVertex){i1x,i1y,0.0f,ci};
            v[6]=(TNTGxmColorVertex){i1x,i1y,0.0f,ci};
            v[7]=(TNTGxmColorVertex){o0x,o0y,0.0f,co};
            v[8]=(TNTGxmColorVertex){o1x,o1y,0.0f,co};
            s_gxm.color_count=(u16)(s_gxm.color_count+9);
        }
    }
    return 0;
}

static void haluci_feedback_uv(f32 x,f32 y,s32 board_x,s32 board_y,s32 cell,
    u16 fb_width,u16 fb_height,f32 *u,f32 *v) {
    const f32 c=0.9950041653f,sn=0.0998334166f,scale=1.1f;
    const f32 cx=199.5f,cy=149.5f;
    f32 dx=x-cx,dy=y-cy;
    f32 srcx=(c*dx+sn*dy)/scale+cx;
    f32 srcy=(-sn*dx+c*dy)/scale+cy;
    f32 px=(f32)board_x+((srcx-83.0f)*(f32)cell/11.0f);
    f32 py=(f32)board_y+((srcy-36.0f)*(f32)cell/11.0f);
    *u=(px+0.5f)/(f32)fb_width;
    *v=(py+0.5f)/(f32)fb_height;
}

int TNT_Vita_HaluciGxmRender(
    SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,const TNTHaluciFrame *frame
) {
    f32 ident[16];void *ub;
    TNTGxmTexVertex *quad=s_gxm.tex_vertices+TNT_GXM_OVERLAY_VERTEX_BASE;
    SceGxmTexture feedback_tex;
    u8 feedback_ready=FALSE;
    f32 u_tl=0.0f,v_tl=0.0f,u_tr=1.0f,v_tr=0.0f;
    f32 u_bl=0.0f,v_bl=1.0f,u_br=1.0f,v_br=1.0f;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f);
    f32 x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f);
    f32 y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1;
    s32 clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||!frame||!s_gxm.color_alpha_fp||
       !s_gxm.texture_feedback_fp||build_haluci_color_geometry(frame)<0)return -1;

    /* The previous display surface is already GXM-mapped CDRAM. Treat it as
     * a linear-strided texture directly; no CPU resample or texture upload. */
    if(frame->feedback_this_frame&&frame->feedback_framebuffer&&
       frame->feedback_width&&frame->feedback_height&&frame->feedback_stride){
        if(sceGxmTextureInitLinearStrided(&feedback_tex,frame->feedback_framebuffer,
                SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
                frame->feedback_width,frame->feedback_height,
                (unsigned)frame->feedback_stride*4U)>=0){
            sceGxmTextureSetMinFilter(&feedback_tex,SCE_GXM_TEXTURE_FILTER_LINEAR);
            sceGxmTextureSetMagFilter(&feedback_tex,SCE_GXM_TEXTURE_FILTER_LINEAR);
            sceGxmTextureSetMipFilter(&feedback_tex,SCE_GXM_TEXTURE_MIP_FILTER_DISABLED);
            sceGxmTextureSetUAddrMode(&feedback_tex,SCE_GXM_TEXTURE_ADDR_CLAMP);
            sceGxmTextureSetVAddrMode(&feedback_tex,SCE_GXM_TEXTURE_ADDR_CLAMP);
            if(sceGxmTextureValidate(&feedback_tex)>=0){
                haluci_feedback_uv(0.0f,0.0f,board_x,board_y,cell,
                    frame->feedback_width,frame->feedback_height,&u_tl,&v_tl);
                haluci_feedback_uv(400.0f,0.0f,board_x,board_y,cell,
                    frame->feedback_width,frame->feedback_height,&u_tr,&v_tr);
                haluci_feedback_uv(0.0f,300.0f,board_x,board_y,cell,
                    frame->feedback_width,frame->feedback_height,&u_bl,&v_bl);
                haluci_feedback_uv(400.0f,300.0f,board_x,board_y,cell,
                    frame->feedback_width,frame->feedback_height,&u_br,&v_br);
                feedback_ready=TRUE;
            }
        }
    }

    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;
    if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    set_overlay_quad_uv4(quad,0.0f,0.0f,400.0f,300.0f,
        u_tl,v_tl,u_tr,v_tr,u_bl,v_bl,u_br,v_br);
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;

    /* HALUCI owns the frame.  Alpha 188 is intentional: the feedback fragment
     * program uses destination alpha as a constant source multiplier. 188/255
     * closely equals the N64's 0xFA RGB tint times 0xC0 feedback alpha. */
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,0,0,TNT_GXM_W-1,TNT_GXM_H-1);
    sceGxmSetViewport(s_gxm.context,TNT_GXM_W*0.5f,TNT_GXM_W*0.5f,TNT_GXM_H*0.5f,-TNT_GXM_H*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    s_gxm.clear_vertices[0].color=s_gxm.clear_vertices[1].color=s_gxm.clear_vertices[2].color=0xbc000000U;
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);

    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);

    if(feedback_ready){
        sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_feedback_fp);
        sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
        sceGxmSetFragmentTexture(s_gxm.context,0,&feedback_tex);
        sceGxmSetVertexStream(s_gxm.context,0,quad);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    }

    if(s_gxm.color_count){
        sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_alpha_fp);
        sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,s_gxm.color_count);
    }
    sceGxmEndScene(s_gxm.context,NULL,NULL);
    tnt103a_profiled_gxm_finish(s_gxm.context);
    return 0;
}

int TNT_Vita_CreditsGxmRender(
    SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    s32 board_x, s32 board_y, s32 cell, const u32 *rgba400x300
) {
    f32 ident[16];
    void *ub;
    TNTGxmTexVertex *quad=s_gxm.tex_vertices+TNT_GXM_OVERLAY_VERTEX_BASE;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f);
    f32 x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f);
    f32 y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1;
    s32 clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||!rgba400x300||!s_gxm.texture_data[TNT_GXM_CREDITS_TEXTURE])return -1;
    memcpy(s_gxm.texture_data[TNT_GXM_CREDITS_TEXTURE],rgba400x300,400*300*4);
    if(clip_x0<0)clip_x0=0;
    if(clip_y0<0)clip_y0=0;
    if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;
    if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    set_overlay_quad(quad,0.0f,0.0f,400.0f,300.0f);
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_CREDITS_TEXTURE]);
    sceGxmSetVertexStream(s_gxm.context,0,quad);
    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    sceGxmEndScene(s_gxm.context,NULL,NULL);
    tnt103a_profiled_gxm_finish(s_gxm.context);
    return 0;
}

int TNT_Vita_FontCGxmRenderText(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,s32 native_x,s32 native_y,const char *text) {
    f32 ident[16];void *ub;u16 count;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f);
    f32 x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f);
    f32 y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1;
    s32 clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||!s_gxm.texture_alpha_fp||!text)return -1;
    count=build_font_c_line_vertices(text,(f32)native_x,(f32)native_y);if(!count)return 0;
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_FONT_C_TEXTURE]);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+TNT_GXM_STORY_VERTEX_BASE);
    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,count);
    sceGxmEndScene(s_gxm.context,NULL,NULL);tnt103a_profiled_gxm_finish(s_gxm.context);return 0;
}

int TNT_Vita_PauseGxmRender(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u8 selected,f32 pulse) {
    f32 ident[16];void *ub;TNTGxmTexVertex *quad=s_gxm.tex_vertices+TNT_GXM_OVERLAY_VERTEX_BASE;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f);
    f32 x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f);
    f32 y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1;
    s32 clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||!s_gxm.texture_alpha_fp||!s_gxm.texture_data[TNT_GXM_PAUSE_TEXT_TEXTURE])return -1;
    update_pause_text_texture(selected,pulse);
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    set_overlay_quad(quad,(f32)TNT_GXM_PAUSE_NATIVE_X,(f32)TNT_GXM_PAUSE_NATIVE_Y,
        (f32)(TNT_GXM_PAUSE_NATIVE_X+TNT_GXM_PAUSE_TEX_W),(f32)(TNT_GXM_PAUSE_NATIVE_Y+TNT_GXM_PAUSE_TEX_H));
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
    sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_PAUSE_TEXT_TEXTURE]);
    sceGxmSetVertexStream(s_gxm.context,0,quad);
    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,6);
    sceGxmEndScene(s_gxm.context,NULL,NULL);tnt103a_profiled_gxm_finish(s_gxm.context);return 0;
}

static int tnt212_wonder_progress_gxm_render(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u16 anim_tick,u8 hall_background) {
    f32 proj[16],ident[16];void *ub;u16 r;
    u64 start=sceKernelGetProcessTimeWide(),mark;
    extern u32 g_tnt_wonder_profile_us[4];
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f);
    f32 x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f);
    f32 y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1;
    s32 clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||build_progress_geometry(anim_tick)<0)return -1;
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    progress_projection_matrix(proj);memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    mark=sceKernelGetProcessTimeWide();
    g_tnt_wonder_profile_us[0]=(u32)(mark-start);
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    g_tnt_wonder_profile_us[1]=(u32)(sceKernelGetProcessTimeWide()-mark);
    start=sceKernelGetProcessTimeWide();
    if(hall_background)tnt210_hall_draw_background();
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    /* Preserve the cached 2D PROGRESS page; clear depth only. */
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
    for(r=0;r<s_gxm.run_count;r++){
        TNTGxmRun *run=&s_gxm.runs[r];
        sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);
    }
    if(s_gxm.color_count){
        sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);
        sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
        sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_multiply_fp);
        sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,s_gxm.color_count);
    }
    sceGxmEndScene(s_gxm.context,NULL,NULL);
    g_tnt_wonder_profile_us[2]=(u32)(sceKernelGetProcessTimeWide()-start);
    start=sceKernelGetProcessTimeWide();
    tnt103a_profiled_gxm_finish(s_gxm.context);
    g_tnt_wonder_profile_us[3]=(u32)(sceKernelGetProcessTimeWide()-start);
    return 0;
}

/* Hall-only draw restores the approved card background through the GPU
 * before progress geometry. Pause -> PROGRESS retains its exact old path. */
int TNT_Vita_WonderProgressHallGxmRender(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u16 anim_tick) {
    return tnt212_wonder_progress_gxm_render(
        surface,sync,board_x,board_y,cell,anim_tick,TRUE);
}
int TNT_Vita_WonderProgressGxmRender(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u16 anim_tick) {
    return tnt212_wonder_progress_gxm_render(
        surface,sync,board_x,board_y,cell,anim_tick,FALSE);
}

int TNT_Vita_WonderMilestoneGxmRenderMayan(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u8 milestone_index,u16 elapsed_ticks,const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    f32 proj[16],ident[16];void *ub;u16 r;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f);
    f32 x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f);
    f32 y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1;
    s32 clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||build_geometry(milestone_index,elapsed_ticks,block_rotation_x)<0)return -1;
    if(clip_x0<0)clip_x0=0;
    if(clip_y0<0)clip_y0=0;
    if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;
    if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    projection_matrix(proj);memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    /* N64 materials in this milestone use G_CULL_BACK. Our display viewport
     * has a negative Y scale, so source CCW front faces map to GXM's display
     * convention by culling clockwise primitives. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);

    /* Clear only depth: full-screen clip triangle, fragment color writes masked off. */
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);

    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
    for(r=0;r<s_gxm.run_count;r++){
        TNTGxmRun *run=&s_gxm.runs[r];
        sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);
    }

    if(s_gxm.color_count){
        sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);
        sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
        sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_multiply_fp);
        sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,s_gxm.color_count);
    }
    sceGxmEndScene(s_gxm.context,NULL,NULL);
    /* The original Wonder1 scene fills the full 400x300 viewport and then
     * overlays its text. Finish this GPU pass before the CPU font renderer
     * blends into the same display buffer. */
    tnt103a_profiled_gxm_finish(s_gxm.context);
    return 0;
}

int TNT_Vita_WonderMilestoneGxmRenderGreek(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u8 greek_stage,u16 elapsed_ticks,const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    f32 proj[16],ident[16];void *ub;u16 r;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f);
    f32 x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f);
    f32 y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1;
    s32 clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||build_greek_geometry(greek_stage,elapsed_ticks,block_rotation_x)<0)return -1;
    if(clip_x0<0)clip_x0=0;
    if(clip_y0<0)clip_y0=0;
    if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;
    if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    projection_matrix(proj);memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    /* N64 materials in this milestone use G_CULL_BACK. Our display viewport
     * has a negative Y scale, so source CCW front faces map to GXM's display
     * convention by culling clockwise primitives. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);

    /* Clear only depth: full-screen clip triangle, fragment color writes masked off. */
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);

    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
    for(r=0;r<s_gxm.run_count;r++){
        TNTGxmRun *run=&s_gxm.runs[r];
        sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);
    }

    if(s_gxm.color_count){
        sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);
        sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
        sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_multiply_fp);
        sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,s_gxm.color_count);
    }
    sceGxmEndScene(s_gxm.context,NULL,NULL);
    /* The original Wonder1 scene fills the full 400x300 viewport and then
     * overlays its text. Finish this GPU pass before the CPU font renderer
     * blends into the same display buffer. */
    tnt103a_profiled_gxm_finish(s_gxm.context);
    return 0;
}

int TNT_Vita_WonderMilestoneGxmRenderEgyptian(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u8 egyptian_stage,u16 elapsed_ticks,const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    f32 proj[16],ident[16];void *ub;u16 r;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f);
    f32 x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f);
    f32 y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1;
    s32 clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||build_egyptian_geometry(egyptian_stage,elapsed_ticks,block_rotation_x)<0)return -1;
    if(clip_x0<0)clip_x0=0;
    if(clip_y0<0)clip_y0=0;
    if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;
    if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    projection_matrix(proj);memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    /* N64 materials in this milestone use G_CULL_BACK. Our display viewport
     * has a negative Y scale, so source CCW front faces map to GXM's display
     * convention by culling clockwise primitives. */
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);

    /* Clear only depth: full-screen clip triangle, fragment color writes masked off. */
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);

    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
    for(r=0;r<s_gxm.run_count;r++){
        TNTGxmRun *run=&s_gxm.runs[r];
        sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);
    }

    if(s_gxm.color_count){
        sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);
        sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
        sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_multiply_fp);
        sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,s_gxm.color_count);
    }
    sceGxmEndScene(s_gxm.context,NULL,NULL);
    /* The original Wonder1 scene fills the full 400x300 viewport and then
     * overlays its text. Finish this GPU pass before the CPU font renderer
     * blends into the same display buffer. */
    tnt103a_profiled_gxm_finish(s_gxm.context);
    return 0;
}

int TNT_Vita_WonderMilestoneGxmRenderCeltic(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u8 celtic_stage,u16 elapsed_ticks,const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    f32 proj[16],ident[16];void *ub;u16 r;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f),x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f),y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1,clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||build_celtic_milestone_geometry(celtic_stage,elapsed_ticks,block_rotation_x)<0)return -1;
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    projection_matrix(proj);memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
    for(r=0;r<s_gxm.run_count;r++){TNTGxmRun *run=&s_gxm.runs[r];sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);}
    if(s_gxm.color_count){sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_multiply_fp);sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,s_gxm.color_count);}
    sceGxmEndScene(s_gxm.context,NULL,NULL);tnt103a_profiled_gxm_finish(s_gxm.context);return 0;
}

int TNT_Vita_WonderMilestoneGxmRenderAfrican(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u8 african_stage,u16 elapsed_ticks,const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    f32 proj[16],ident[16];void *ub;u16 r;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f),x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f),y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1,clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||build_african_milestone_geometry(african_stage,elapsed_ticks,block_rotation_x)<0)return -1;
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    projection_matrix(proj);memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
    for(r=0;r<s_gxm.run_count;r++){TNTGxmRun *run=&s_gxm.runs[r];sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);}
    if(s_gxm.color_count){sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_multiply_fp);sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,s_gxm.color_count);}
    sceGxmEndScene(s_gxm.context,NULL,NULL);tnt103a_profiled_gxm_finish(s_gxm.context);return 0;
}

int TNT_Vita_WonderMilestoneGxmRenderRussian(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u8 russian_stage,u16 elapsed_ticks,const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    f32 proj[16],ident[16];void *ub;u16 r;
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f),x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f),y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1,clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized||build_russian_milestone_geometry(russian_stage,elapsed_ticks,block_rotation_x)<0)return -1;
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    projection_matrix(proj);memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
    for(r=0;r<s_gxm.run_count;r++){TNTGxmRun *run=&s_gxm.runs[r];sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);}
    if(s_gxm.color_count){sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_multiply_fp);sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,s_gxm.color_count);}
    sceGxmEndScene(s_gxm.context,NULL,NULL);tnt103a_profiled_gxm_finish(s_gxm.context);return 0;
}

int TNT_Vita_WonderMilestoneGxmRenderJapanese(SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u8 japanese_stage,u16 elapsed_ticks,const u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS]) {
    /* Canonical Japanese construction is the scene-1244 dataset already
     * resident in the milestone texture pool. Reuse that exact GPU path so
     * milestones 35..41 introduce no duplicate textures or staging buffers. */
    return TNT_Vita_WonderMilestoneGxmRenderCeltic(
        surface,sync,board_x,board_y,cell,japanese_stage,elapsed_ticks,block_rotation_x);
}


u32 g_tnt_wonder_profile_us[4];

static u32 complete_geometry_effective_ticks(u8 wonder,u8 anim_mode,u32 ticks) {
    if(anim_mode==TNT_WONDER_COMPLETE_ANIM_PRESENTATION)return 0U;
    if(anim_mode==TNT_WONDER_COMPLETE_ANIM_FINALE){
        static const u16 last_tick[7]={1998U,1900U,2416U,1918U,1878U,2318U,2358U};
        if(wonder<7U && ticks>(u32)last_tick[wonder])return (u32)last_tick[wonder];
    }
    if(anim_mode==TNT_WONDER_COMPLETE_ANIM_FINAL){
        /* Mayan and Russian intentionally switch to a second still-running
         * source animation when their first viewer track ends, so never freeze
         * those two. The other five hold their terminal pose under story text. */
        static const u16 last_tick[7]={0U,3998U,3438U,4394U,2998U,2398U,0U};
        if(wonder>0U && wonder<6U && ticks>(u32)last_tick[wonder])return (u32)last_tick[wonder];
    }
    return ticks;
}

int TNT_Vita_WonderCompleteGxmRender(
    SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u8 wonder,u8 anim_mode,u32 anim_ticks,
    const char *const *story,u32 story_line_count,u32 story_ticks,u8 story_visible
) {
    u64 profile_start = sceKernelGetProcessTimeWide(), profile_mark;
    f32 proj[16],ident[16];void *ub;s32 r;
    u8 reuse_geometry;u16 story_vertex_count=0;
    anim_ticks=complete_geometry_effective_ticks(wonder,anim_mode,anim_ticks);
    reuse_geometry=(u8)(s_complete_geometry_cache_valid &&
        s_complete_geometry_cache_wonder==wonder &&
        s_complete_geometry_cache_mode==anim_mode &&
        s_complete_geometry_cache_ticks==anim_ticks);
    memset(g_tnt_wonder_profile_us,0,sizeof(g_tnt_wonder_profile_us));
    f32 x0=(f32)board_x+((0.0f-83.0f)*(f32)cell/11.0f);
    f32 x1=(f32)board_x+((400.0f-83.0f)*(f32)cell/11.0f);
    f32 y0=(f32)board_y+((0.0f-36.0f)*(f32)cell/11.0f);
    f32 y1=(f32)board_y+((300.0f-36.0f)*(f32)cell/11.0f);
    s32 clip_x0=(s32)floorf(x0),clip_x1=(s32)ceilf(x1)-1;
    s32 clip_y0=(s32)floorf(y0),clip_y1=(s32)ceilf(y1)-1;
    if(!s_gxm.initialized)return -1;
    /* Preserve the already-approved Mayan/Greek projection for this focused
     * Egypt QA.  Egyptian camera resource 370 uses its own animated FOV and a
     * 200-unit far clip (scaled by the scene renderer's 16x camera space). */
    s_complete_fov=66.6679992676f;s_complete_near=2.0f;s_complete_far=32000.0f;s_complete_aspect=1.33333337307f;
    if(wonder==0 && anim_mode==TNT_WONDER_COMPLETE_ANIM_FINALE){
        f32 src_near,src_far;
        TNT_Vita_MayanWonderCompleteCamera(anim_mode,anim_ticks,&s_complete_fov,&src_near,&src_far,&s_complete_aspect);
        s_complete_near=src_near*16.0f;if(s_complete_near<2.0f)s_complete_near=2.0f;if(s_complete_near>4.0f)s_complete_near=4.0f;
        s_complete_far=src_far*16.0f;if(s_complete_far>32000.0f)s_complete_far=32000.0f;if(s_complete_far<s_complete_near+1.0f)s_complete_far=s_complete_near+1.0f;
    }
    if(wonder==1 && anim_mode==TNT_WONDER_COMPLETE_ANIM_FINALE){
        f32 src_near,src_far;
        TNT_Vita_GreekWonderCompleteCamera(anim_mode,anim_ticks,&s_complete_fov,&src_near,&src_far,&s_complete_aspect);
        s_complete_near=src_near*16.0f;if(s_complete_near<2.0f)s_complete_near=2.0f;if(s_complete_near>4.0f)s_complete_near=4.0f;
        s_complete_far=src_far*16.0f;if(s_complete_far>32000.0f)s_complete_far=32000.0f;if(s_complete_far<s_complete_near+1.0f)s_complete_far=s_complete_near+1.0f;
    } else if(wonder==1 && anim_mode==TNT_WONDER_COMPLETE_ANIM_FINAL){
        /* Greek camera resource 310 is constant in 315/317: 40deg FOV,
         * near 0.02, far 200.133865, 4:3. Apply the scene's 16x camera scale
         * so its canonical 995..999 fog occupies the same depth range as N64. */
        s_complete_fov=40.0f;
        /* Canonical Borg camera setup scales near by 16, then clamps it to
         * 2..4 before guPerspectiveF. Greek's raw 0.02 therefore becomes 2.0,
         * not 0.32. The smaller value made the 995..999 fog band saturate far
         * too early and washed the Delphi viewer in pale fog. */
        s_complete_near=2.0f;
        s_complete_far=200.1338653564f*16.0f;
        s_complete_aspect=1.33333337307f;
    }
    if(wonder==2){
        f32 src_near,src_far;
        TNT_Vita_EgyptianWonderCompleteCamera(anim_mode,anim_ticks,&s_complete_fov,&src_near,&src_far,&s_complete_aspect);
        /* Camera 370's 0.0625 source near plane maps to 1.0 in this
         * renderer's 16x camera space.  Do not re-clamp it to 2.0: the opening
         * boat shot intentionally moves close enough for that to clip early. */
        s_complete_near=src_near*16.0f;if(s_complete_near<0.25f)s_complete_near=0.25f;
        s_complete_far=src_far*16.0f;if(s_complete_far<s_complete_near+1.0f)s_complete_far=s_complete_near+1.0f;
    }
    if(wonder==3){
        f32 src_near,src_far;
        TNT_Vita_CelticWonderCompleteCamera(anim_mode,anim_ticks,&s_complete_fov,&src_near,&src_far,&s_complete_aspect);
        /* Canonical Borg projection scales near/far by 16 and clamps the near
         * plane to 2..4. Camera 465 starts at 0.2, yielding 3.2 here. */
        s_complete_near=src_near*16.0f;if(s_complete_near<2.0f)s_complete_near=2.0f;if(s_complete_near>4.0f)s_complete_near=4.0f;
        s_complete_far=src_far*16.0f;if(s_complete_far>32000.0f)s_complete_far=32000.0f;if(s_complete_far<s_complete_near+1.0f)s_complete_far=s_complete_near+1.0f;
    }
    if(wonder==4){
        f32 src_near,src_far;
        TNT_Vita_AfricanWonderCompleteCamera(anim_mode,anim_ticks,&s_complete_fov,&src_near,&src_far,&s_complete_aspect);
        /* Camera 553 starts at near 0.625/far 625. Borg scales by 16 and
         * clamps the projection near plane to 2..4, so African uses 4..10000
         * in this renderer's camera-space units. */
        s_complete_near=src_near*16.0f;if(s_complete_near<2.0f)s_complete_near=2.0f;if(s_complete_near>4.0f)s_complete_near=4.0f;
        s_complete_far=src_far*16.0f;if(s_complete_far>32000.0f)s_complete_far=32000.0f;if(s_complete_far<s_complete_near+1.0f)s_complete_far=s_complete_near+1.0f;
    }
    if(wonder==5){
        f32 src_near,src_far;
        TNT_Vita_JapaneseWonderCompleteCamera(anim_mode,anim_ticks,&s_complete_fov,&src_near,&src_far,&s_complete_aspect);
        s_complete_near=src_near*16.0f;if(s_complete_near<0.25f)s_complete_near=0.25f;if(s_complete_near>4.0f)s_complete_near=4.0f;
        s_complete_far=src_far*16.0f;if(s_complete_far>32000.0f)s_complete_far=32000.0f;if(s_complete_far<s_complete_near+1.0f)s_complete_far=s_complete_near+1.0f;
    }
    if(wonder==6){
        f32 src_near,src_far;
        TNT_Vita_RussianWonderCompleteCamera(anim_mode,anim_ticks,&s_complete_fov,&src_near,&src_far,&s_complete_aspect);
        /* Camera 884 uses near 0.8/far 314.009.  Borg's scene renderer scales
         * by 16 and clamps near to 2..4, so the Vita projection is 4..5024. */
        s_complete_near=src_near*16.0f;if(s_complete_near<2.0f)s_complete_near=2.0f;if(s_complete_near>4.0f)s_complete_near=4.0f;
        s_complete_far=src_far*16.0f;if(s_complete_far>32000.0f)s_complete_far=32000.0f;if(s_complete_far<s_complete_near+1.0f)s_complete_far=s_complete_near+1.0f;
    }
    if(!reuse_geometry){
        if(wonder==0){
            if(!s_gxm.complete_texture_pack||build_complete_geometry(anim_mode,anim_ticks)<0)return -1;
        }else if(wonder==1){
            if(!s_gxm.greek_complete_texture_pack||build_greek_complete_geometry(anim_mode,anim_ticks)<0)return -1;
        }else if(wonder==2){
            if(!s_gxm.egyptian_complete_texture_pack||build_egyptian_complete_geometry(anim_mode,anim_ticks)<0)return -1;
        }else if(wonder==3){
            if(!s_gxm.celtic_complete_texture_pack||build_celtic_complete_geometry(anim_mode,anim_ticks)<0)return -1;
        }else if(wonder==4){
            if(!s_gxm.african_complete_texture_pack||build_african_complete_geometry(anim_mode,anim_ticks)<0)return -1;
        }else if(wonder==5){
            if(!s_gxm.japanese_complete_texture_pack||build_japanese_complete_geometry(anim_mode,anim_ticks)<0)return -1;
        }else if(wonder==6){
            if(!s_gxm.russian_complete_texture_pack||build_russian_complete_geometry(anim_mode,anim_ticks)<0)return -1;
        }else return -1;
        /* Every build_*() calls reset_geometry(), which invalidates the old
         * cache before staging the new arrays. They are now complete and may
         * be reused by an identical consecutive story frame. */
        s_complete_geometry_cache_valid=TRUE;
        s_complete_geometry_cache_wonder=wonder;
        s_complete_geometry_cache_mode=anim_mode;
        s_complete_geometry_cache_ticks=anim_ticks;
    }
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    complete_projection_matrix(proj);memset(ident,0,sizeof(ident));ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    if(story_visible)story_vertex_count=build_wonder_story_vertices(story,story_line_count,story_ticks);
    profile_mark=sceKernelGetProcessTimeWide();
    g_tnt_wonder_profile_us[0]=(u32)(profile_mark-profile_start);
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,sync,surface,&s_gxm.depth_surface)<0)return -1;
    profile_start=sceKernelGetProcessTimeWide();
    g_tnt_wonder_profile_us[1]=(u32)(profile_start-profile_mark);
    if(anim_mode==TNT_WONDER_COMPLETE_ANIM_HALL)
        tnt210_hall_draw_background();
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    /* Hall draws over its cached menu: preserve that color surface. Full
     * cinematics own the frame, so combine their black clear with the existing
     * depth-clear triangle instead of sweeping 2 MiB from the CPU. The intro
     * renderer already uses this same full-surface GXM clear sequence. */
    if(anim_mode!=TNT_WONDER_COMPLETE_ANIM_HALL){
        sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,0,0,TNT_GXM_W-1,TNT_GXM_H-1);
        sceGxmSetViewport(s_gxm.context,TNT_GXM_W*0.5f,TNT_GXM_W*0.5f,TNT_GXM_H*0.5f,-TNT_GXM_H*0.5f,0.5f,0.5f);
        sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
        s_gxm.clear_vertices[0].color=s_gxm.clear_vertices[1].color=s_gxm.clear_vertices[2].color=0xff000000U;
    }
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);
    sceGxmSetFragmentProgram(s_gxm.context,anim_mode==TNT_WONDER_COMPLETE_ANIM_HALL?s_gxm.color_nowrite_fp:s_gxm.color_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);
    if(anim_mode!=TNT_WONDER_COMPLETE_ANIM_HALL){
        sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
        sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
        sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    }
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    if(s_complete_untex_count){
        sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
        sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_fp);
        sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,s_complete_untex_count);
    }
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
    if(wonder==3 || wonder==4 || wonder==5){
        /* Celtic 470, African 557, and Japanese ribbon nodes 76..78 apply TEXEL0*SHADE in source draw order.
         * Keep the shade multiply adjacent to each texture run so later scene
         * geometry cannot replace depth before the EQUAL lighting pass lands. */
        for(r=0;r<s_gxm.run_count;r++){
            TNTGxmRun *run=&s_gxm.runs[r];u8 no_depth=(u8)((run->pad&2U)!=0U);
            sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);
            sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
            sceGxmSetFrontDepthFunc(s_gxm.context,no_depth?SCE_GXM_DEPTH_FUNC_ALWAYS:SCE_GXM_DEPTH_FUNC_LESS);
            sceGxmSetBackDepthFunc(s_gxm.context,no_depth?SCE_GXM_DEPTH_FUNC_ALWAYS:SCE_GXM_DEPTH_FUNC_LESS);
            sceGxmSetFrontDepthWriteEnable(s_gxm.context,no_depth?SCE_GXM_DEPTH_WRITE_DISABLED:SCE_GXM_DEPTH_WRITE_ENABLED);
            sceGxmSetBackDepthWriteEnable(s_gxm.context,no_depth?SCE_GXM_DEPTH_WRITE_DISABLED:SCE_GXM_DEPTH_WRITE_ENABLED);
            if(run->pad2) sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texedge_fp);
            else if(run->pad&1U) sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp?s_gxm.texture_alpha_fp:s_gxm.texture_fp);
            else sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
            sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);
            sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);
            sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);
            {
                u16 shade_first=(wonder==3)?s_celtic_run_shade_first[r]:(wonder==4?s_african_run_shade_first[r]:s_japanese_run_shade_first[r]);
                u16 shade_count=(wonder==3)?s_celtic_run_shade_count[r]:(wonder==4?s_african_run_shade_count[r]:s_japanese_run_shade_count[r]);
                if(shade_count && !no_depth){
                    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);
                    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
                    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_multiply_fp);
                    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);
                    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices+shade_first);
                    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,shade_count);
                }
            }
        }
        sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
        sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    }else{
        /* Preserve the already-approved Mayan/Greek/Egyptian completion paths. */
        sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
        sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
        for(r=0;r<s_gxm.run_count;r++){
            TNTGxmRun *run=&s_gxm.runs[r];if(run->pad2)continue;
            sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);
            sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);
        }
        /* Mayan/Egyptian vegetation is source TEX_EDGE: bilinear sample, discard
         * low-alpha coverage, and let surviving fragments write depth as opaque.
         * Greek stays on its already-approved legacy alpha path. Mayan texture
         * 146 is the source-authored half-alpha butterfly card. */
        for(r=0;r<s_gxm.run_count;r++){
            TNTGxmRun *run=&s_gxm.runs[r];u8 legacy_alpha;
            if(!run->pad2)continue;
            legacy_alpha=(u8)(wonder==1 ||
                (wonder==0 && run->texture==(TNT_GXM_COMPLETE_TEXTURE_BASE+146)) ||
                (wonder==5 && run->texture>=(TNT_GXM_JAPANESE_COMPLETE_TEXTURE_BASE+76) &&
                              run->texture<=(TNT_GXM_JAPANESE_COMPLETE_TEXTURE_BASE+85)));
            if(legacy_alpha){
                sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp?s_gxm.texture_alpha_fp:s_gxm.texture_fp);
                sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
            }else{
                sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texedge_fp);
                sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
            }
            sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[run->texture]);sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+run->first);
            sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,run->count);
        }
    }
    /* Restore normal depth writes for the lighting pass / next scene. */
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    if(wonder!=3 && wonder!=4 && wonder!=5 && (s_complete_fog_count?s_complete_fog_first:s_gxm.color_count)>s_complete_untex_count){
        u16 shade_end=s_complete_fog_count?s_complete_fog_first:s_gxm.color_count;
        u16 shade_count=(u16)(shade_end-s_complete_untex_count);
        sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);
        sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
        sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_multiply_fp);
        sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices+s_complete_untex_count);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,shade_count);
    }
    if(s_complete_fog_count){
        sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);
        sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
        sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_alpha_fp);
        sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices+s_complete_fog_first);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,s_complete_fog_count);
    }
    if(story_vertex_count){
        /* The five scrolling Font-C lines are 2D source overlays. Rendering
         * their original IA8 glyphs here avoids ~50 ms/frame of CPU software
         * alpha blending while retaining the same native coordinates/timing. */
        sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,(unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
        sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,(y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
        sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_NONE);
        sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
        sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
        sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);
        sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_alpha_fp?s_gxm.texture_alpha_fp:s_gxm.texture_fp);
        sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,ident);
        sceGxmSetFragmentTexture(s_gxm.context,0,&s_gxm.textures[TNT_GXM_FONT_C_TEXTURE]);
        sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices+TNT_GXM_STORY_VERTEX_BASE);
        sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,story_vertex_count);
    }
    sceGxmEndScene(s_gxm.context,NULL,NULL);
    profile_mark=sceKernelGetProcessTimeWide();
    g_tnt_wonder_profile_us[2]=(u32)(profile_mark-profile_start);
    tnt103a_profiled_gxm_finish(s_gxm.context);
    g_tnt_wonder_profile_us[3]=(u32)(sceKernelGetProcessTimeWide()-profile_mark);
    return 0;
}

/* Test209D: FINALE Hall only. The seven normal Hall models already use
 * TNT_Vita_WonderCompleteGxmRender; this replaces the 340-triangle CPU
 * pixel rasterizer that quantized to 20 FPS for the final rotating trophy.
 * Scene1425's source pose, animation1426 rotation, FOV, UVs, and +Y light
 * are reconstructed from the user's ROM exactly as in the former CPU path.
 * The 2D Hall background remains the already-precomposed CPU layer. */
int TNT_Vita_WonderFinaleHallGxmRender(
    SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u16 anim_tick
) {
    TNTGxmMatrix view,local[4],world[4],draw[4],mv[4];
    f32 vals[4][9],proj[16],ident[16];
    f32 x0,x1,y0,y1;
    f32 nearz=2.0f,farz=4000.0f,half_h,half_w;
    s32 clip_x0,clip_x1,clip_y0,clip_y1,i,k;
    void *ub;
    const u16 count=340U*3U;
    if(!s_gxm.initialized || !s_gxm.texture_data[TNT_GXM_FINALE_HALL_TEXTURE] ||
       !surface || !sync)return -1;
    /* The normal complete-Wonder geometry cache shares this staging pool.
     * Avoid accidentally reusing trophy vertices when switching back to the
     * other Hall cards. */
    s_complete_geometry_cache_valid=FALSE;
    memcpy(vals,g_tnt_wonder_finale_hall_nodes,sizeof(vals));
    vals[3][0]=-0.70588237047f*(f32)(anim_tick%511U);
    complete_camera_view(vals[0],&view);
    for(i=0;i<4;i++)node_type2(vals[i],&local[i]);
    world[3]=local[3];
    mat_mult(&world[1],&world[3],&local[1]);
    mat_mult(&world[2],&world[3],&local[2]);
    for(i=1;i<=3;i++){
        make_draw_model(&world[i],g_tnt_wonder_finale_hall_mesh_scale[i-1],&draw[i]);
        mat_mult(&mv[i],&view,&draw[i]);
    }
    /* A single GPU draw for the 340 triangles and one equal-depth
     * multiplicative light pass, no per-pixel CPU division/texture bilinear
     * sampling. Source light: ambient 0.3, +Y diffuse 120/127. */
    for(i=0;i<340;i++){
        const TNTWonderTriangle *tri=&g_tnt_wonder_finale_hall_triangles[i];
        const u8 node=tri->node;
        if(node<1U || node>3U)return -1;
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];
            TNTGxmTexVertex *tv=&s_gxm.tex_vertices[i*3+k];
            TNTGxmColorVertex *cv=&s_gxm.color_vertices[i*3+k];
            f32 nx,ny,nz,brightness;
            transform_point(&mv[node],(f32)v->x,(f32)v->y,(f32)v->z,
                            &tv->x,&tv->y,&tv->z);
            tv->u=((f32)v->s/64.0f)/32.0f;
            tv->v=((f32)v->t/64.0f)/32.0f;
            cv->x=tv->x;cv->y=tv->y;cv->z=tv->z;
            transform_vector(&world[node],(f32)v->nx/127.0f,
                (f32)v->ny/127.0f,(f32)v->nz/127.0f,&nx,&ny,&nz);
            normalize3(&nx,&ny,&nz);
            brightness=0.3f+fmaxf(0.0f,ny*(120.0f/127.0f));
            if(brightness>1.0f)brightness=1.0f;
            {
                u32 shade=(u32)(brightness*255.0f+0.5f);
                cv->color=0xff000000U|(shade<<16)|(shade<<8)|shade;
            }
        }
    }

    half_h=nearz*tanf(21.3204402924f*0.5f*TNT_GXM_DEG2RAD);
    half_w=half_h*(4.0f/3.0f);
    memset(proj,0,sizeof(proj));
    proj[0]=nearz/half_w;proj[5]=nearz/half_h;
    proj[10]=-(farz+nearz)/(farz-nearz);
    proj[14]=-2.0f*farz*nearz/(farz-nearz);
    proj[11]=-1.0f;
    memset(ident,0,sizeof(ident));
    ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    x0=(f32)board_x+(0.0f-83.0f)*(f32)cell/11.0f;
    x1=(f32)board_x+(400.0f-83.0f)*(f32)cell/11.0f;
    y0=(f32)board_y+(0.0f-36.0f)*(f32)cell/11.0f;
    y1=(f32)board_y+(300.0f-36.0f)*(f32)cell/11.0f;
    clip_x0=(s32)floorf(x0);clip_x1=(s32)ceilf(x1)-1;
    clip_y0=(s32)floorf(y0);clip_y1=(s32)ceilf(y1)-1;
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;
    if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;
    if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,
                        sync,surface,&s_gxm.depth_surface)<0)return -1;
    tnt210_hall_draw_background();
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,
        (unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,
        (y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);
    sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);
    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,
               SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);
    sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
    sceGxmSetFragmentTexture(s_gxm.context,0,
                             &s_gxm.textures[TNT_GXM_FINALE_HALL_TEXTURE]);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices);
    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,
               SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,count);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);
    sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_multiply_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);
    sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices);
    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,
               SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,count);
    sceGxmEndScene(s_gxm.context,NULL,NULL);
    tnt103a_profiled_gxm_finish(s_gxm.context);
    return 0;
}

/* Test212D: scene1071 gold question mark is a 236-triangle textured
 * reflected object. Render with the SAME GXM Hall background/depth/texture/
 * lighting sequence as the accepted FINALE (Test209D). The old 400x300
 * CPU per-pixel software rasterizer is retained as emergency fallback.
 * Source texturing is N64 G_TEXTURE_GEN: reflection normal dot original
 * look-at right/up, mapped from [-1,1] onto 32x32. Reflection and light
 * update every 60Hz source pose; original geometry/UV and background stay.
 * This function is for the interactive WONDERS Hall ONLY. */
int TNT_Vita_WonderLockedHallGxmRender(
    SceGxmColorSurface *surface,SceGxmSyncObject *sync,
    s32 board_x,s32 board_y,s32 cell,u16 anim_tick
) {
    TNTGxmMatrix view,local,model,mv;
    f32 vals[9],proj[16],ident[16],half_h,half_w;
    f32 x0,x1,y0,y1;
    const f32 nearz=2.0f,farz=32000.0f;
    s32 clip_x0,clip_x1,clip_y0,clip_y1,i,k;
    void *ub;
    u64 start=sceKernelGetProcessTimeWide(),mark;
    extern u32 g_tnt_wonder_profile_us[4];
    const u16 count=236U*3U;
    if(!s_gxm.initialized || !surface || !sync ||
        !s_gxm.texture_data[TNT_GXM_LOCKED_HALL_TEXTURE])return -1;
    s_complete_geometry_cache_valid=FALSE;
    memcpy(vals,g_tnt_wonder_locked_nodes[1],sizeof(vals));
    vals[0]=-1.40625f-0.703125f*(f32)(anim_tick%511U);
    complete_camera_view(g_tnt_wonder_locked_nodes[0],&view);
    node_type2(vals,&local);
    make_draw_model(&local,0.125f,&model);
    mat_mult(&mv,&view,&model);
    for(i=0;i<236;i++){
        const TNTWonderTriangle *tri=&g_tnt_wonder_locked_triangles[i];
        for(k=0;k<3;k++){
            const TNTWonderVertex *v=&tri->v[k];
            TNTGxmTexVertex *tv=&s_gxm.tex_vertices[i*3+k];
            TNTGxmColorVertex *cv=&s_gxm.color_vertices[i*3+k];
            f32 nx,ny,nz,dot;
            transform_point(&mv,(f32)v->x,(f32)v->y,(f32)v->z,
                            &tv->x,&tv->y,&tv->z);
            /* For normals, use original unscaled Borg node matrix: uniform
             * scale cancels in normalize3 and source's software renderer
             * computes G_TEXTURE_GEN and shading from this exact node. */
            transform_vector(&local,(f32)v->nx/127.0f,
                (f32)v->ny/127.0f,(f32)v->nz/127.0f,&nx,&ny,&nz);
            normalize3(&nx,&ny,&nz);
            dot=nx*view.m[0][0]+ny*view.m[0][1]+nz*view.m[0][2];
            tv->u=(dot+1.0f)*0.5f;
            dot=nx*view.m[1][0]+ny*view.m[1][1]+nz*view.m[1][2];
            tv->v=(dot+1.0f)*0.5f;
            cv->x=tv->x;cv->y=tv->y;cv->z=tv->z;
            dot=ny*(120.0f/127.0f);
            if(dot<0.0f)dot=0.0f;
            dot+=0.5f;
            if(dot>1.0f)dot=1.0f;
            {
                u32 shade=(u32)(dot*255.0f+0.5f);
                cv->color=0xff000000U|(shade<<16)|(shade<<8)|shade;
            }
        }
    }
    half_h=nearz*tanf(41.5394402f*0.5f*TNT_GXM_DEG2RAD);
    half_w=half_h*(4.0f/3.0f);
    memset(proj,0,sizeof(proj));
    proj[0]=nearz/half_w;proj[5]=nearz/half_h;
    proj[10]=-(farz+nearz)/(farz-nearz);
    proj[14]=-2.0f*farz*nearz/(farz-nearz);
    proj[11]=-1.0f;
    memset(ident,0,sizeof(ident));
    ident[0]=ident[5]=ident[10]=ident[15]=1.0f;
    x0=(f32)board_x+(0.0f-83.0f)*(f32)cell/11.0f;
    x1=(f32)board_x+(400.0f-83.0f)*(f32)cell/11.0f;
    y0=(f32)board_y+(0.0f-36.0f)*(f32)cell/11.0f;
    y1=(f32)board_y+(300.0f-36.0f)*(f32)cell/11.0f;
    clip_x0=(s32)floorf(x0);clip_x1=(s32)ceilf(x1)-1;
    clip_y0=(s32)floorf(y0);clip_y1=(s32)ceilf(y1)-1;
    if(clip_x0<0)clip_x0=0;if(clip_y0<0)clip_y0=0;
    if(clip_x1>=TNT_GXM_W)clip_x1=TNT_GXM_W-1;
    if(clip_y1>=TNT_GXM_H)clip_y1=TNT_GXM_H-1;
    mark=sceKernelGetProcessTimeWide();
    g_tnt_wonder_profile_us[0]=(u32)(mark-start);
    if(sceGxmBeginScene(s_gxm.context,0,s_gxm.render_target,NULL,NULL,
                        sync,surface,&s_gxm.depth_surface)<0)return -1;
    g_tnt_wonder_profile_us[1]=(u32)(sceKernelGetProcessTimeWide()-mark);
    start=sceKernelGetProcessTimeWide();
    tnt210_hall_draw_background();
    sceGxmSetRegionClip(s_gxm.context,SCE_GXM_REGION_CLIP_INSIDE,
        (unsigned)clip_x0,(unsigned)clip_y0,(unsigned)clip_x1,(unsigned)clip_y1);
    sceGxmSetViewport(s_gxm.context,(x0+x1)*0.5f,(x1-x0)*0.5f,
        (y0+y1)*0.5f,-(y1-y0)*0.5f,0.5f,0.5f);
    sceGxmSetCullMode(s_gxm.context,SCE_GXM_CULL_CW);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_ALWAYS);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_ENABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_nowrite_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);
    sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,ident);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.clear_vertices);
    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,
        SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,3);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_LESS);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.texture_vp);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.texture_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);
    sceGxmSetUniformDataF(ub,s_gxm.texture_wvp,0,16,proj);
    sceGxmSetFragmentTexture(s_gxm.context,0,
        &s_gxm.textures[TNT_GXM_LOCKED_HALL_TEXTURE]);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.tex_vertices);
    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,
        SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,count);
    sceGxmSetFrontDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);
    sceGxmSetBackDepthFunc(s_gxm.context,SCE_GXM_DEPTH_FUNC_EQUAL);
    sceGxmSetFrontDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetBackDepthWriteEnable(s_gxm.context,SCE_GXM_DEPTH_WRITE_DISABLED);
    sceGxmSetVertexProgram(s_gxm.context,s_gxm.color_vp);
    sceGxmSetFragmentProgram(s_gxm.context,s_gxm.color_multiply_fp);
    sceGxmReserveVertexDefaultUniformBuffer(s_gxm.context,&ub);
    sceGxmSetUniformDataF(ub,s_gxm.color_wvp,0,16,proj);
    sceGxmSetVertexStream(s_gxm.context,0,s_gxm.color_vertices);
    sceGxmDraw(s_gxm.context,SCE_GXM_PRIMITIVE_TRIANGLES,
        SCE_GXM_INDEX_FORMAT_U16,s_gxm.indices,count);
    sceGxmEndScene(s_gxm.context,NULL,NULL);
    g_tnt_wonder_profile_us[2]=(u32)(sceKernelGetProcessTimeWide()-start);
    start=sceKernelGetProcessTimeWide();
    tnt103a_profiled_gxm_finish(s_gxm.context);
    g_tnt_wonder_profile_us[3]=(u32)(sceKernelGetProcessTimeWide()-start);
    return 0;
}
