#include "tnt_wonder_complete_japanese_vita.h"

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
static void apply_type1(const u8 *d,u32 ticks,f32 out[TNT_JAPANESE_COMPLETE_NODE_COUNT][12]){
    u32 count=be32(d+4),list=be32(d+8),i;
    for(i=0;i<count;i++){const u8 *e=d+list+i*0x24U;u32 type=be32(e+0x0c),target=be32(e+0x10),bits=be32(e+0x14),tc=be32(e+0x18),tr=be32(e+0x1c),bit,t=0;
        if(type!=1U||target>=TNT_JAPANESE_COMPLETE_NODE_COUNT)continue;
        for(bit=0;bit<12U&&t<tc;bit++)if(bits&(1U<<bit)){out[target][bit]=track_value(d,tr+t*0x14U,ticks);t++;}
    }
}
static const u8 *mode_anim(u8 mode,u32 *ticks){
    if(mode==TNT_WONDER_COMPLETE_ANIM_PRESENTATION){*ticks=2U;return g_tnt_japanese_complete_anim_757;}
    if(mode==TNT_WONDER_COMPLETE_ANIM_HALL)return g_tnt_japanese_complete_anim_757;
    if(mode==TNT_WONDER_COMPLETE_ANIM_FINALE){if(*ticks>=2319U)*ticks=2318U;return g_tnt_japanese_complete_anim_758;}
    if(*ticks>=2399U)*ticks=2398U;return g_tnt_japanese_complete_anim_756;
}

void TNT_Vita_JapaneseWonderCompleteEffectState(u8 mode,u32 ticks,f32 out_transparency[4]){
    /* Private Vita effect groups: node73/asset714 material0 (slot1),
     * node42/asset691 materials0/1 (slots3/4), and the thirteen
     * butterfly cards assets677..689 (shared slot5).  All four materials
     * author a 0.5 base transparency; animation 758 replaces that channel. */
    const u8 *d=mode_anim(mode,&ticks);u32 count=be32(d+4),list=be32(d+8),i;
    out_transparency[0]=0.5f;out_transparency[1]=0.5f;
    out_transparency[2]=0.5f;out_transparency[3]=0.5f;
    for(i=0;i<count;i++){
        const u8 *e=d+list+i*0x24U;u32 asset=be32(e),type=be32(e+0x0c),target=be32(e+0x10);
        u32 bits=be32(e+0x14),tc=be32(e+0x18),tr=be32(e+0x1c),bit,t=0,group=4U;
        if(type!=2U)continue;
        if(asset==714U&&target==0U)group=0U;
        else if(asset==691U&&target==0U)group=1U;
        else if(asset==691U&&target==1U)group=2U;
        else if(asset>=677U&&asset<=689U&&target==0U)group=3U;
        if(group>=4U)continue;
        for(bit=0;bit<4U&&t<tc;bit++)if(bits&(1U<<bit)){
            f32 v=track_value(d,tr+t*0x14U,ticks);t++;
            if(bit==3U)out_transparency[group]=v;
        }
    }
    for(i=0;i<4U;i++){if(out_transparency[i]<0.0f)out_transparency[i]=0.0f;if(out_transparency[i]>1.0f)out_transparency[i]=1.0f;}
}

void TNT_Vita_JapaneseWonderCompleteCamera(u8 mode,u32 ticks,f32 *fovy,f32 *near_plane,f32 *far_plane,f32 *aspect){
    const u8 *d=mode_anim(mode,&ticks);u32 count=be32(d+4),list=be32(d+8),i;
    f32 fv=30.0f,np=0.0562499985f,fp=46.875f,ar=1.33333301544f;
    /* Camera resource 750 target 4.  Its animation channels map to the same
     * camera fields used by the intro evaluator: bit 0 is vertical FOV; bit 2
     * is far clip when present.  Near/aspect remain at the resource defaults. */
    for(i=0;i<count;i++){
        const u8 *e=d+list+i*0x24U;u32 asset=be32(e),type=be32(e+0x0c),target=be32(e+0x10),bits=be32(e+0x14),tc=be32(e+0x18),tr=be32(e+0x1c),bit,t=0;
        if(asset!=750U||type!=0U||target!=4U)continue;
        for(bit=0;bit<4U&&t<tc;bit++)if(bits&(1U<<bit)){
            f32 v=track_value(d,tr+t*0x14U,ticks);t++;
            if(bit==0U)fv=v;else if(bit==1U)np=v;else if(bit==2U)fp=v;
        }
        break;
    }
    if(fv<1.0f)fv=1.0f;if(fv>170.0f)fv=170.0f;
    if(np<=0.0f)np=0.0562499985f;if(fp<=np)fp=46.875f;
    if(fovy)*fovy=fv;if(near_plane)*near_plane=np;if(far_plane)*far_plane=fp;if(aspect)*aspect=ar;
}

void TNT_Vita_JapaneseWonderCompleteBuildPose(u8 mode,u32 ticks,f32 out[TNT_JAPANESE_COMPLETE_NODE_COUNT][12]){
    memcpy(out,g_tnt_japanese_complete_nodes,sizeof(g_tnt_japanese_complete_nodes));
    if(mode==TNT_WONDER_COMPLETE_ANIM_PRESENTATION){
        /* wonders1_80043c7c forces two updates then stops resource 757. */
        apply_type1(g_tnt_japanese_complete_anim_757,2U,out);
    }else if(mode==TNT_WONDER_COMPLETE_ANIM_HALL){
        /* Completed Japanese Hall/presentation data uses resource 757. */
        apply_type1(g_tnt_japanese_complete_anim_757,ticks,out);
    }else if(mode==TNT_WONDER_COMPLETE_ANIM_FINALE){
        /* Resource 758 is the distinct all-Wonders finale sequence. */
        if(ticks>=2319U)ticks=2318U;
        apply_type1(g_tnt_japanese_complete_anim_758,ticks,out);
    }else{
        /* Finished Horyu Ji viewer runs resource 756 through terminal tick 2399. */
        if(ticks>=2399U)ticks=2398U;
        apply_type1(g_tnt_japanese_complete_anim_756,ticks,out);
    }
}
