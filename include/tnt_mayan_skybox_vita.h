#ifndef TNT_MAYAN_SKYBOX_VITA_H
#define TNT_MAYAN_SKYBOX_VITA_H

#include "tnt_wonder_vita.h"

typedef struct {
    u8 texture;
    u8 pad[3];
    TNTWonderVertex v[3];
} TNTMayanSkyboxTriangle;

extern TNTMayanSkyboxTriangle g_tnt_mayan_skybox_triangles[64];
extern u16 g_tnt_mayan_skybox_textures[8][1360];

#endif
