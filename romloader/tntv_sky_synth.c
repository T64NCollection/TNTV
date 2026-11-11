#include "tntv_sky_synth.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Test200Q EXPERIMENTAL:
 * Synthesize the native Russian gameplay sky from original verified ROM FX
 * scene901 texture IDs 892/894/893 and scalar editorial color controls.
 * No copied sky pixels, flare bitmap, original art array or PNG is shipped.
 * This is a first-pass *alternative look*, not an accepted pixel match.
 *
 * IMPORTANT: The approved GXM 400x300 flat-quad renderer, cathedral,
 * foreground snow, candles, name, board and FPS path remain intact.
 * Synthesize ONCE at verified ROM launch, never per frame.
 */
enum { TNTV_SKY_N=32, TNTV_SKY_CHANNELS=3, TNTV_SKY_PIXEL_COUNT=400*300 };
#define TNTV_SKY_TEXELEMS (32U*32U*3U)
#define TNTV_SKY_TEXIDX(tex,y,x,channel) \
    ((size_t)(tex)*TNTV_SKY_TEXELEMS+((size_t)(y)*32U+(size_t)(x))*3U+(channel))

typedef struct { int y; float rgb[3]; } TNTVSkyTone;

/* Hand-selected color/height controls, NOT original game pixels. The
 * authenticated ROM texture material supplies all cloud detail. */
static const TNTVSkyTone sky_tones[]={
    {0,{107,81,97}},
    {15,{95,74,94}},
    {24,{94,71,91}},
    {34,{99,77,95}},
    {42,{98,76,95}},
    {55,{93,73,93}},
    {65,{98,76,97}},
    {75,{107,84,104}},
    {85,{109,86,102}},
    {96,{95,82,101}},
    {107,{86,78,94}},
    {115,{82,78,93}},
    {123,{72,71,88}},
    {136,{113,95,107}},
    {147,{154,120,140}},
    {157,{140,117,143}},
    {166,{106,99,121}},
    {176,{109,102,122}},
    {186,{109,102,124}},
    {201,{104,96,118}},
    {215,{95,91,109}},
    {250,{102,98,119}},
    {299,{98,97,114}}
};

static int sky_limit(int k,int low,int high){
    if(k<low)return low;
    if(k>high)return high;
    return k;
}
static float sky_flimit(float n,float low,float high){
    if(n<low)return low;
    if(n>high)return high;
    return n;
}
static void sky_tone_at(int y,float rgb[3]){
    unsigned i=0;
    while(i+1U<sizeof(sky_tones)/sizeof(sky_tones[0]) &&
          y>sky_tones[i+1U].y)i++;
    unsigned j=(i+1U<sizeof(sky_tones)/sizeof(sky_tones[0]))
        ?i+1U:i;
    float t=j==i?0.f:(float)(y-sky_tones[i].y)/
                         (float)(sky_tones[j].y-sky_tones[i].y);
    for(unsigned c=0;c<3;c++)
        rgb[c]=sky_tones[i].rgb[c]*(1.f-t)+sky_tones[j].rgb[c]*t;
}
static void sky_decode_rom_textures(const uint16_t rgba16[3][1360],float *raw){
    for(unsigned t=0;t<3;t++)for(unsigned y=0;y<32;y++)
        for(unsigned x=0;x<32;x++){
            unsigned sx=x^((y&1U)?2U:0U); /* N64 DXT=0 odd-row half-swap */
            uint16_t q=rgba16[t][y*32U+sx];
            raw[TNTV_SKY_TEXIDX(t,y,x,0)]=(float)((q>>11)&31U)*255.f/31.f;
            raw[TNTV_SKY_TEXIDX(t,y,x,1)]=(float)((q>>6)&31U)*255.f/31.f;
            raw[TNTV_SKY_TEXIDX(t,y,x,2)]=(float)((q>>1)&31U)*255.f/31.f;
        }
}
/* Original textures are only 32x32. Applying a gentle separable low-pass
 * on the original material avoids square texels at 400x300 resolution. */
static void sky_soften(const float *raw,float *smooth,int texture,int half){
    static const float w3[3]={1.f,2.f,1.f};
    static const float w5[5]={1.f,4.f,6.f,4.f,1.f};
    const float *weights=half==1?w3:w5;
    const float total=half==1?16.f:256.f;
    for(int y=0;y<32;y++)for(int x=0;x<32;x++){
        for(unsigned c=0;c<3;c++){
            float acc=0.f;
            for(int dy=-half;dy<=half;dy++)
                for(int dx=-half;dx<=half;dx++){
                    int yy=sky_limit(y+dy,0,31);
                    int xx=sky_limit(x+dx,0,31);
                    acc+=raw[TNTV_SKY_TEXIDX(texture,yy,xx,c)]*
                         weights[dy+half]*weights[dx+half];
                }
            smooth[TNTV_SKY_TEXIDX(texture,y,x,c)]=acc/total;
        }
    }
}
static void sky_bilinear(const float *pixels,unsigned tex,float x,float y,float rgb[3]){
    x=sky_flimit(x,0.f,31.f);y=sky_flimit(y,0.f,31.f);
    int x0=(int)x,y0=(int)y;
    int x1=sky_limit(x0+1,0,31),y1=sky_limit(y0+1,0,31);
    float xf=x-x0,yf=y-y0;
    for(unsigned c=0;c<3;c++){
        float p00=pixels[TNTV_SKY_TEXIDX(tex,y0,x0,c)];
        float p10=pixels[TNTV_SKY_TEXIDX(tex,y0,x1,c)];
        float p01=pixels[TNTV_SKY_TEXIDX(tex,y1,x0,c)];
        float p11=pixels[TNTV_SKY_TEXIDX(tex,y1,x1,c)];
        rgb[c]=(p00+(p10-p00)*xf)*(1.f-yf)+
               (p01+(p11-p01)*xf)*yf;
    }
}
static void sky_main_source(const float *smooth,float sx,int y,float rgb[3]){
    float sky1[3],sky2[3];
    if(y<175){
        /* Vertical stretch of the original ROM cloud texture 894. */
        float sky_scale=(y<125)?250.f:250.f-30.f*(float)(y-125)/(175.f-125.f);
        float yy=(float)y*sky_scale/175.f*31.f/224.f;
        sky_bilinear(smooth,1U,sx,yy,rgb);
    }else{
        /* Blend scene901 ROM cloud material 894 into the 893 lower sky. */
        float sy1=(220.f+(float)(y-175)/3.f)*31.f/224.f;
        float sy2=(208.f+(float)(y-175)*0.5f)*31.f/299.f;
        float blend=sky_flimit((float)(y-175)/30.f,0.f,1.f);
        sky_bilinear(smooth,1U,sx,sy1,sky1);
        sky_bilinear(smooth,2U,sx,sy2,sky2);
        for(unsigned c=0;c<3;c++)
            rgb[c]=sky1[c]*(1.f-blend)+sky2[c]*blend;
    }
}
/* Source N64 892 lens-flare texture + analytic layered elliptical glow:
 * no hard square core, no baked pixels. Coefficients express a small set of
 * natural flare geometry parameters fitted from private visual A/B, not a
 * source-asset pixel grid. All flare effects are made on startup only. */
static float sky_flares(float x,float y,const float *smooth,float rgb[3]){
    float source[3];
    float tx=x*31.f/399.f,ty=y*31.f/299.f;
    sky_bilinear(smooth,0U,tx,ty,source);
    float brightness=(source[0]+source[1]+source[2])/(3.f*255.f);
    float u=(x-61.4448f)/20.48914f, v=(y-33.f)/17.13820f;
    float main_glow=123.556f*expf(-0.5f*(u*u+v*v-0.59170f*u*v));
    float a=(x-41.59203f)/17.88184f;
    float b=(y-19.14723f)/24.08438f;
    float spread=31.79055f*expf(-0.5f*(a*a+b*b));
    /* Sparse curved crescent to the right of the source star.
     * Its strength is deliberately low so it never becomes a halo ring. */
    float dx=(x-111.f)/35.f,dy=(y-47.f)/39.f;
    float radius=sqrtf(dx*dx+dy*dy);
    float band=(radius-1.f)/0.085f;
    float crescent=4.18351f*expf(-0.5f*band*band)*
       sky_flimit((x-115.f)/26.f,0.f,1.f);
    /* The 892 texture contributes slight irregularity but cannot create a
     * hard 32x32 white square: all channels receive additive, soft energy. */
    float energy=(main_glow+spread)*(0.965f+0.07f*brightness)+crescent;
    rgb[0]+=energy;
    rgb[1]+=energy*1.025f;
    rgb[2]+=energy*.965f;
    return energy;
}
int tntv_russian_sky_synthesize(
    const uint16_t rgba16[3][1360],const uint16_t valid_texels[3],
    uint32_t output[TNTV_SKY_PIXEL_COUNT]){
    float *raw,*smooth;
    int y,x;
    if(!rgba16 || !valid_texels || !output)return -1;
    /* Never synthesize from zero/partial/unverified texture tables. */
    for(unsigned t=0;t<3;t++)
        if(valid_texels[t]<1024U || valid_texels[t]>1360U)return -2;
    raw=(float*)malloc(sizeof(float)*3U*TNTV_SKY_TEXELEMS);
    smooth=(float*)malloc(sizeof(float)*3U*TNTV_SKY_TEXELEMS);
    if(!raw || !smooth){free(raw);free(smooth);return -3;}
    sky_decode_rom_textures(rgba16,raw);
    sky_soften(raw,smooth,0,2);
    sky_soften(raw,smooth,1,1);
    sky_soften(raw,smooth,2,2);
    for(y=0;y<300;y++){
        float base_mean[3]={0.f,0.f,0.f};
        float detail_mean[3]={0.f,0.f,0.f};
        float tone[3],factor=y<70?0.55f:(y<135?0.80f:(y<190?1.00f:0.55f));
        float dety=(float)y*31.f/299.f;
        sky_tone_at(y,tone);
        for(x=0;x<400;x++){
            float sx=(float)x*31.f/399.f,base[3],detail[3];
            sky_main_source(smooth,sky_flimit(sx+(1.0f),0.f,31.f),y,base);
            sky_bilinear(raw,1U,sky_flimit(sx+(-2.0f),0.f,31.f),dety,detail);
            for(unsigned c=0;c<3;c++){
                base_mean[c]+=base[c];detail_mean[c]+=detail[c];
            }
        }
        for(unsigned c=0;c<3;c++){
            base_mean[c]*=1.f/400.f;detail_mean[c]*=1.f/400.f;
        }
        for(x=0;x<400;x++){
            float sx=(float)x*31.f/399.f,base[3],detail[3],color[3];
            sky_main_source(smooth,sky_flimit(sx+(1.0f),0.f,31.f),y,base);
            sky_bilinear(raw,1U,sky_flimit(sx+(-2.0f),0.f,31.f),dety,detail);
            for(unsigned c=0;c<3;c++)
                color[c]=tone[c]+(base[c]-base_mean[c])*factor+
                         (detail[c]-detail_mean[c])*0.18f;
            /* Restrict expensive flare exponentials to its actual sky area;
             * all lower pixel output comes from the original 893/894 texture. */
            if(y<100 && x<180)
                sky_flares((float)x,(float)y,smooth,color);
            uint32_t rgb[3];
            for(unsigned c=0;c<3;c++)
                rgb[c]=(uint32_t)(sky_flimit(color[c],0.f,255.f)+0.5f);
            output[y*400+x]=rgb[0]|(rgb[1]<<8)|(rgb[2]<<16)|0xff000000U;
        }
    }
    free(raw);
    free(smooth);
    return 0;
}
