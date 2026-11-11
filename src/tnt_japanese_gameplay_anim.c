#include "tnt_japanese_gameplay_vita.h"
#include <string.h>

static u16 japanese_be16(const u8 *p){return (u16)(((u16)p[0]<<8)|p[1]);}
static u32 japanese_be32(const u8 *p){return ((u32)p[0]<<24)|((u32)p[1]<<16)|((u32)p[2]<<8)|(u32)p[3];}
static f32 japanese_bef32(const u8 *p){union{u32 u;f32 f;}v;v.u=japanese_be32(p);return v.f;}

static void japanese_load_first(const u8 *d,u32 ko,f32 p[4]){
    u16 m=japanese_be16(d+ko+2);u32 co=japanese_be32(d+ko+4);s32 i;
    memset(p,0,sizeof(f32)*4);
    for(i=0;i<4&&m;i++,m>>=1)if(m&1U){p[i]=japanese_bef32(d+co);co+=4;}
}
static u16 japanese_load_next(const u8 *d,u32 ko,f32 p[4]){
    u16 m=japanese_be16(d+ko+2),scan=m;u32 co=japanese_be32(d+ko+4);s32 i=0;
    while(i<4&&!(scan&1U)){i++;scan>>=1;}
    for(;i<4;i++,scan>>=1){if(scan&1U){p[i]=japanese_bef32(d+co);co+=4;}else p[i]=0.0f;}
    return m;
}
static void japanese_advance(f32 p[4],u16 m,u32 count){
    f32 n,c2,c3;if(!count||!(m&2U))return;n=(f32)count;c2=n*(n-1.0f)*0.5f;c3=n*(n-1.0f)*(n-2.0f)/6.0f;
    p[0]+=n*p[1];if(m&4U)p[0]+=c2*p[2];if(m&8U)p[0]+=c3*p[3];
    if(m&4U){p[1]+=n*p[2];if(m&8U)p[1]+=c2*p[3];}if(m&8U)p[2]+=n*p[3];
}
static f32 japanese_track_value(const u8 *d,u32 to,u32 ticks){
    u16 terminal=japanese_be16(d+to+4);u32 key=japanese_be32(d+to+12),cur=1;u16 mask=japanese_be16(d+key+2);f32 p[4];
    japanese_load_first(d,key,p);if(!terminal)return p[0];ticks%=terminal;if(!ticks)return p[0];
    while(cur<=ticks){u32 nk=key+8U;u16 nt=japanese_be16(d+nk);u32 stop=ticks+1U;
        if(nt==cur){key=nk;mask=japanese_load_next(d,key,p);japanese_advance(p,mask,1U);cur++;continue;}
        if(nt>cur&&nt<stop)stop=nt;if(stop>cur){u32 n=stop-cur;japanese_advance(p,mask,n);cur+=n;}
    }return p[0];
}
static void japanese_apply_nodes(const u8 *d,u32 ticks,f32 *nodes,u32 node_count){
    u32 count=japanese_be32(d+4),list=japanese_be32(d+8),i;
    for(i=0;i<count;i++){const u8 *e=d+list+i*0x24U;u32 type=japanese_be32(e+0x0c),target=japanese_be32(e+0x10),bits=japanese_be32(e+0x14),tc=japanese_be32(e+0x18),to=japanese_be32(e+0x1c),bit,tr=0;
        if(type!=1U||target>=node_count)continue;
        for(bit=0;bit<9U&&tr<tc;bit++)if(bits&(1U<<bit)){nodes[target*9U+bit]=japanese_track_value(d,to+tr*0x14U,ticks);tr++;}
    }
}

void TNT_Vita_JapaneseButterfliesBuildPose(
    u32 ticks, f32 out_nodes[TNT_JAPANESE_BUTTERFLY_NODE_COUNT][9]
) {
    memcpy(out_nodes, g_tnt_japanese_butterfly_nodes,
           sizeof(f32) * TNT_JAPANESE_BUTTERFLY_NODE_COUNT * 9U);
    japanese_apply_nodes(
        g_tnt_japanese_butterfly_anim_1361, ticks,
        &out_nodes[0][0], TNT_JAPANESE_BUTTERFLY_NODE_COUNT);
}
