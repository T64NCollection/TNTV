#ifndef TNT_INTRO_VITA_H
#define TNT_INTRO_VITA_H

#include <stdint.h>
#include <psp2/gxm.h>

#define TNT_INTRO_NODE_COUNT 71
#define TNT_INTRO_TEXTURE_COUNT 37
#define TNT_INTRO_DRAW_COUNT 86
#define TNT_INTRO_VERTEX_COUNT 4548
#define TNT_INTRO_TRACK_COUNT 187
#define TNT_INTRO_TARGET_COUNT 60
#define TNT_INTRO_END_TICK 3950U

typedef struct { int16_t x,y,z,s,t; int8_t nx,ny,nz; uint8_t a; } TNTIntroVertex;
typedef struct { uint16_t flags; int8_t parent; uint8_t type; float v[9]; } TNTIntroNodeDef;
typedef struct { uint32_t data_offset; uint16_t size; uint8_t levels,u_addr,v_addr,flags; uint16_t texel_count; } TNTIntroTextureDef;
typedef struct { uint16_t first,count; uint8_t node,texture,material_flags,pad; float scale,r,g,b,transparency; } TNTIntroDrawDef;
typedef struct { int16_t tick; uint16_t mask; float v[4]; } TNTIntroKey;
typedef struct { uint32_t first_key; uint16_t key_count,end_tick; } TNTIntroTrackDef;
typedef struct { uint8_t kind,index; uint16_t bits; uint16_t first_track; uint8_t track_count,pad; } TNTIntroTargetDef;

extern TNTIntroNodeDef g_tnt_intro_nodes[TNT_INTRO_NODE_COUNT];
extern TNTIntroTextureDef g_tnt_intro_textures[TNT_INTRO_TEXTURE_COUNT];
extern uint16_t g_tnt_intro_texture_rgba16[39920];
extern TNTIntroVertex g_tnt_intro_vertices[TNT_INTRO_VERTEX_COUNT];
extern TNTIntroDrawDef g_tnt_intro_draws[TNT_INTRO_DRAW_COUNT];
extern TNTIntroKey g_tnt_intro_keys[6722];
extern TNTIntroTrackDef g_tnt_intro_tracks[TNT_INTRO_TRACK_COUNT];
extern TNTIntroTargetDef g_tnt_intro_targets[TNT_INTRO_TARGET_COUNT];
extern float g_tnt_intro_fog_initial[6];
extern float g_tnt_intro_camera_initial[4];
extern float g_tnt_intro_lights_initial[3][4];

typedef struct {
    float node[TNT_INTRO_NODE_COUNT][9];
    float fog[6];
    float camera[4];
    float light[3][4];
    float draw_transparency[TNT_INTRO_DRAW_COUNT];
    uint32_t tick;
} TNTIntroPose;

void TNT_Vita_IntroReset(void);
void TNT_Vita_IntroAdvance(uint32_t ticks);
uint32_t TNT_Vita_IntroTick(void);
const TNTIntroPose *TNT_Vita_IntroPose(void);
int TNT_Vita_IntroGxmRender(SceGxmColorSurface *surface,SceGxmSyncObject *sync,int32_t board_x,int32_t board_y,int32_t cell,uint8_t white_fade);

#endif
