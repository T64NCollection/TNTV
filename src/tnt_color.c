#include "vita_compat.h"
#include "tnt_game_types.h"
#include <string.h>

Color g_color_L;
Color g_color_J;
Color g_color_Z;
Color g_color_S;
Color g_color_T;
Color g_color_I;

/* Six palette records retain their exact original values from the player's
 * verified ROM; mutable game colors continue to use their usual globals. */
void TNTV_Vita_SetOriginalPieceColors(const unsigned char bytes[72]) {
    Color *const colors[6] = {
        &g_color_L,&g_color_J,&g_color_Z,&g_color_S,&g_color_T,&g_color_I
    };
    unsigned i;
    if(!bytes)return;
    for(i=0;i<6;i++)memcpy(colors[i],bytes+12*i,sizeof(Color));
}

Color g_color_O = { 0x78, 0x78, 0x78, 0xFF, 0xFF, 0xFF };

Color g_color_silver = { 0x320, 0x3C0, 0x500, 0xDC0, 0xF50, 0xFF0 };
Color g_color_gold = { 0x800, 0x500, 0x0, 0xFF0, 0xF50, 0xDC0 };
Color g_color_white = { 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0, 0xFF0 };
Color g_color_black = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
Color g_color_red = { 0xFF0, 0x0, 0x0, 0xFF0, 0x0, 0x0 };
Color g_color_metal = { 0x0, 0x0, 0x0, 0xFF0, 0xFF0, 0xFF0 };

// unused
void Color_Tint(Color *src, u32 alpha) {
  src->diffuse_red    = (((0xFF0 - src->diffuse_red)    * alpha) / 0xFF) + src->diffuse_red;
  src->diffuse_green  = (((0xFF0 - src->diffuse_green)  * alpha) / 0xFF) + src->diffuse_green;
  src->diffuse_blue   = (((0xFF0 - src->diffuse_blue)   * alpha) / 0xFF) + src->diffuse_blue;
  src->specular_red   = (((0xFF0 - src->specular_red)   * alpha) / 0xFF) + src->specular_red;
  src->specular_green = (((0xFF0 - src->specular_green) * alpha) / 0xFF) + src->specular_green;
  src->specular_blue  = (((0xFF0 - src->specular_blue)  * alpha) / 0xFF) + src->specular_blue;
}

// unused
void Color_Brighten(Color *src, u32 alpha) {
  src->diffuse_red    = ((src->diffuse_red    * alpha) / 0xFF) + src->diffuse_red;
  src->diffuse_green  = ((src->diffuse_green  * alpha) / 0xFF) + src->diffuse_green;
  src->diffuse_blue   = ((src->diffuse_blue   * alpha) / 0xFF) + src->diffuse_blue;
  src->specular_red   = ((src->specular_red   * alpha) / 0xFF) + src->specular_red;
  src->specular_green = ((src->specular_green * alpha) / 0xFF) + src->specular_green;
  src->specular_blue  = ((src->specular_blue  * alpha) / 0xFF) + src->specular_blue;
}

void Color_Blend(Color *src, Color *dst, u32 dst_alpha) {
  src->diffuse_red    = (((dst->diffuse_red    - src->diffuse_red)    * dst_alpha) / 0xFF) + src->diffuse_red;
  src->diffuse_green  = (((dst->diffuse_green  - src->diffuse_green)  * dst_alpha) / 0xFF) + src->diffuse_green;
  src->diffuse_blue   = (((dst->diffuse_blue   - src->diffuse_blue)   * dst_alpha) / 0xFF) + src->diffuse_blue;
  src->specular_red   = (((dst->specular_red   - src->specular_red)   * dst_alpha) / 0xFF) + src->specular_red;
  src->specular_green = (((dst->specular_green - src->specular_green) * dst_alpha) / 0xFF) + src->specular_green;
  src->specular_blue  = (((dst->specular_blue  - src->specular_blue)  * dst_alpha) / 0xFF) + src->specular_blue;
}
