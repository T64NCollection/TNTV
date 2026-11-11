#include "tnt_wonder_complete_african_vita.h"

#include <string.h>

static u16 be16(const u8 *p){return (u16)(((u16)p[0]<<8)|p[1]);}
static u32 be32(const u8 *p){return ((u32)p[0]<<24)|((u32)p[1]<<16)|((u32)p[2]<<8)|(u32)p[3];}
static f32 bef32(const u8 *p){union{u32 u;f32 f;}v;v.u=be32(p);return v.f;}

static void load_first(const u8 *d,u32 ko,f32 p[4]){
    u16 mask=be16(d+ko+2);u32 co=be32(d+ko+4);int i;
    memset(p,0,sizeof(f32)*4);
    for(i=0;i<4&&mask;i++,mask>>=1)if(mask&1U){p[i]=bef32(d+co);co+=4;}
}
static void load_next(const u8 *d,u32 ko,f32 p[4],u16 *mask_out){
    u16 mask=be16(d+ko+2),scan=mask;u32 co=be32(d+ko+4);int i=0;
    while(i<4&&!(scan&1U)){i++;scan>>=1;}
    for(;i<4;i++,scan>>=1){if(scan&1U){p[i]=bef32(d+co);co+=4;}else p[i]=0.0f;}
    *mask_out=mask;
}
static void advance_diff(f32 p[4],u16 mask,u32 count){
    f32 n,c2,c3;if(count==0||!(mask&2U))return;n=(f32)count;
    c2=count>=2U?n*(n-1.0f)*0.5f:0.0f;c3=count>=3U?n*(n-1.0f)*(n-2.0f)/6.0f:0.0f;
    p[0]+=n*p[1];if(mask&4U)p[0]+=c2*p[2];if(mask&8U)p[0]+=c3*p[3];
    if(mask&4U){p[1]+=n*p[2];if(mask&8U)p[1]+=c2*p[3];}if(mask&8U)p[2]+=n*p[3];
}
static f32 track_value(const u8 *d,u32 to,u32 ticks){
    u16 terminal=be16(d+to+4);u32 key=be32(d+to+12),ct=1;u16 mask=be16(d+key+2);f32 p[4];
    load_first(d,key,p);if(terminal==0)return p[0];ticks%=terminal;if(ticks==0)return p[0];
    while(ct<=ticks){u32 nk=key+8U;u16 nt=be16(d+nk);u32 stop=ticks+1U;
        if(nt==ct){key=nk;load_next(d,key,p,&mask);advance_diff(p,mask,1U);ct++;continue;}
        if(nt>ct&&nt<stop)stop=nt;if(stop>ct){u32 n=stop-ct;advance_diff(p,mask,n);ct+=n;}
    }return p[0];
}
static void apply_type1(const u8 *d,u32 ticks,f32 out[TNT_AFRICAN_COMPLETE_NODE_COUNT][12]){
    u32 count=be32(d+4),list=be32(d+8),i;
    for(i=0;i<count;i++){const u8 *e=d+list+i*0x24U;u32 type=be32(e+0x0c),target=be32(e+0x10),bits=be32(e+0x14),tc=be32(e+0x18),tr=be32(e+0x1c),bit,t=0;
        if(type!=1U||target>=TNT_AFRICAN_COMPLETE_NODE_COUNT)continue;
        for(bit=0;bit<12U&&t<tc;bit++)if(bits&(1U<<bit)){out[target][bit]=track_value(d,tr+t*0x14U,ticks);t++;}
    }
}
static const u8 *mode_anim(u8 mode,u32 *ticks){
    if(mode==TNT_WONDER_COMPLETE_ANIM_PRESENTATION){*ticks=2U;return g_tnt_african_complete_anim_560;}
    if(mode==TNT_WONDER_COMPLETE_ANIM_HALL)return g_tnt_african_complete_anim_560;
    if(mode==TNT_WONDER_COMPLETE_ANIM_FINALE){if(*ticks>=1879U)*ticks=1878U;return g_tnt_african_complete_anim_559;}
    if(*ticks>=2999U)*ticks=2998U;return g_tnt_african_complete_anim_558;
}

void TNT_Vita_AfricanWonderCompleteEffectState(
    u8 mode,u32 ticks,f32 out_material[4][4],u8 *out_texture
){
    static const f32 base_material[4][4]={
        {0.7f,0.7f,0.7f,0.5f},
        {0.7f,0.7f,0.7f,0.5f},
        {0.7f,0.7f,0.7f,0.5f},
        {0.748760998f,0.638921022f,0.751148999f,0.5f}
    };
    static const u32 effect_asset[4]={542U,543U,544U,1394U};
    const u8 *d=mode_anim(mode,&ticks);u32 count=be32(d+4),list=be32(d+8),i,g;
    memcpy(out_material,base_material,sizeof(base_material));if(out_texture)*out_texture=39U;
    for(i=0;i<count;i++){
        const u8 *e=d+list+i*0x24U;u32 asset=be32(e),type=be32(e+0x0c),target=be32(e+0x10);
        u32 bits=be32(e+0x14),tc=be32(e+0x18),tr=be32(e+0x1c);
        if(type==2U&&target==0U){
            for(g=0;g<4U;g++)if(effect_asset[g]==asset){u32 bit,t=0;
                for(bit=0;bit<4U&&t<tc;bit++)if(bits&(1U<<bit)){out_material[g][bit]=track_value(d,tr+t*0x14U,ticks);t++;}
                break;
            }
        }else if(type==3U&&asset==557U&&target==0U&&tc>0U&&out_texture){
            s32 frame=(s32)track_value(d,tr,ticks);if(frame<0)frame=0;if(frame>3)frame=3;*out_texture=(u8)(39U+(u8)frame);
        }
    }
}

void TNT_Vita_AfricanWonderCompleteLightState(u8 mode,u32 ticks,f32 out_light[3][4]){
    static const f32 base[3][4]={
        {1.0f,0.600000024f,0.591455281f,0.479989827f},
        {1.0f,0.785275996f,0.730085850f,0.662576616f},
        {1.0f,0.300000012f,0.300000012f,0.300000012f}
    };
    const u8 *d=mode_anim(mode,&ticks);u32 count=be32(d+4),list=be32(d+8),i;
    memcpy(out_light,base,sizeof(base));
    for(i=0;i<count;i++){
        const u8 *e=d+list+i*0x24U;u32 asset=be32(e),type=be32(e+0x0c),target=be32(e+0x10),bits=be32(e+0x14),tc=be32(e+0x18),tr=be32(e+0x1c),bit,t=0;
        if(asset!=554U||type!=0U||target!=4U)continue;
        for(bit=0;bit<3U&&t<tc;bit++)if(bits&(1U<<bit)){out_light[0][bit+1U]=track_value(d,tr+t*0x14U,ticks);t++;}
        break;
    }
}

void TNT_Vita_AfricanWonderCompleteCamera(u8 mode,u32 ticks,f32 *fovy,f32 *near_plane,f32 *far_plane,f32 *aspect){
    const u8 *d=mode_anim(mode,&ticks);u32 count=be32(d+4),list=be32(d+8),i;
    f32 fv=26.726512909f,np=0.625f,fp=625.0f,ar=1.333333015f;
    for(i=0;i<count;i++){
        const u8 *e=d+list+i*0x24U;u32 asset=be32(e),type=be32(e+0x0c),target=be32(e+0x10),bits=be32(e+0x14),tc=be32(e+0x18),tr=be32(e+0x1c),bit,t=0;
        if(asset!=553U||type!=0U||target!=4U)continue;
        for(bit=0;bit<4U&&t<tc;bit++)if(bits&(1U<<bit)){f32 v=track_value(d,tr+t*0x14U,ticks);t++;if(bit==0U)fv=v;else if(bit==1U)np=v;else if(bit==2U)fp=v;}
        break;
    }
    if(fv<1.0f)fv=1.0f;if(fv>170.0f)fv=170.0f;if(np<=0.0f)np=0.625f;if(fp<=np)fp=625.0f;
    if(fovy)*fovy=fv;if(near_plane)*near_plane=np;if(far_plane)*far_plane=fp;if(aspect)*aspect=ar;
}

void TNT_Vita_AfricanWonderCompleteBuildPose(u8 mode,u32 ticks,f32 out[TNT_AFRICAN_COMPLETE_NODE_COUNT][12]){
    memcpy(out,g_tnt_african_complete_nodes,sizeof(g_tnt_african_complete_nodes));
    if(mode==TNT_WONDER_COMPLETE_ANIM_PRESENTATION)apply_type1(g_tnt_african_complete_anim_560,2U,out);
    else if(mode==TNT_WONDER_COMPLETE_ANIM_HALL)apply_type1(g_tnt_african_complete_anim_560,ticks,out);
    else if(mode==TNT_WONDER_COMPLETE_ANIM_FINALE){if(ticks>=1879U)ticks=1878U;apply_type1(g_tnt_african_complete_anim_559,ticks,out);}
    else{if(ticks>=2999U)ticks=2998U;apply_type1(g_tnt_african_complete_anim_558,ticks,out);}
}
