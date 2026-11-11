#include "tnt_celtic_gameplay_vita.h"
#include <string.h>

static u16 celtic_be16(const u8 *p){return (u16)(((u16)p[0]<<8)|p[1]);}
static u32 celtic_be32(const u8 *p){return ((u32)p[0]<<24)|((u32)p[1]<<16)|((u32)p[2]<<8)|(u32)p[3];}
static f32 celtic_bef32(const u8 *p){union{u32 u;f32 f;}v;v.u=celtic_be32(p);return v.f;}

static void celtic_load_first(const u8 *d,u32 ko,f32 p[4]){
    u16 m=celtic_be16(d+ko+2);u32 co=celtic_be32(d+ko+4);s32 i;
    memset(p,0,sizeof(f32)*4);
    for(i=0;i<4&&m;i++,m>>=1)if(m&1U){p[i]=celtic_bef32(d+co);co+=4;}
}
static u16 celtic_load_next(const u8 *d,u32 ko,f32 p[4]){
    u16 m=celtic_be16(d+ko+2),scan=m;u32 co=celtic_be32(d+ko+4);s32 i=0;
    while(i<4&&!(scan&1U)){i++;scan>>=1;}
    for(;i<4;i++,scan>>=1){if(scan&1U){p[i]=celtic_bef32(d+co);co+=4;}else p[i]=0.0f;}
    return m;
}
static void celtic_advance(f32 p[4],u16 m,u32 count){
    f32 n,c2,c3;if(!count||!(m&2U))return;n=(f32)count;c2=n*(n-1.0f)*0.5f;c3=n*(n-1.0f)*(n-2.0f)/6.0f;
    p[0]+=n*p[1];if(m&4U)p[0]+=c2*p[2];if(m&8U)p[0]+=c3*p[3];
    if(m&4U){p[1]+=n*p[2];if(m&8U)p[1]+=c2*p[3];}if(m&8U)p[2]+=n*p[3];
}
static f32 celtic_track_value(const u8 *d,u32 to,u32 ticks){
    u16 terminal=celtic_be16(d+to+4);u32 key=celtic_be32(d+to+12),cur=1;u16 mask=celtic_be16(d+key+2);f32 p[4];
    celtic_load_first(d,key,p);if(!terminal)return p[0];ticks%=terminal;if(!ticks)return p[0];
    while(cur<=ticks){u32 nk=key+8U;u16 nt=celtic_be16(d+nk);u32 stop=ticks+1U;
        if(nt==cur){key=nk;mask=celtic_load_next(d,key,p);celtic_advance(p,mask,1U);cur++;continue;}
        if(nt>cur&&nt<stop)stop=nt;if(stop>cur){u32 n=stop-cur;celtic_advance(p,mask,n);cur+=n;}
    }return p[0];
}
static void celtic_apply_nodes(const u8 *d,u32 ticks,f32 *nodes,u32 node_count){
    u32 count=celtic_be32(d+4),list=celtic_be32(d+8),i;
    for(i=0;i<count;i++){const u8 *e=d+list+i*0x24U;u32 type=celtic_be32(e+0x0c),target=celtic_be32(e+0x10),bits=celtic_be32(e+0x14),tc=celtic_be32(e+0x18),to=celtic_be32(e+0x1c),bit,tr=0;
        if(type!=1U||target>=node_count)continue;
        for(bit=0;bit<9U&&tr<tc;bit++)if(bits&(1U<<bit)){nodes[target*9U+bit]=celtic_track_value(d,to+tr*0x14U,ticks);tr++;}
    }
}

void TNT_Vita_CelticSkyBuildPose(u32 ticks,f32 out_nodes[TNT_CELTIC_SKY_NODE_COUNT][9],u8 *animated_alpha){
    const u8 *d=g_tnt_celtic_sky_anim_488;u32 count,list,i;f32 transparency=1.0f;
    memcpy(out_nodes,g_tnt_celtic_sky_nodes,sizeof(f32)*TNT_CELTIC_SKY_NODE_COUNT*9U);
    celtic_apply_nodes(d,ticks,&out_nodes[0][0],TNT_CELTIC_SKY_NODE_COUNT);
    count=celtic_be32(d+4);list=celtic_be32(d+8);
    for(i=0;i<count;i++){const u8 *e=d+list+i*0x24U;u32 type=celtic_be32(e+0x0c),bits=celtic_be32(e+0x14),tc=celtic_be32(e+0x18),to=celtic_be32(e+0x1c),bit,tr=0;
        if(type!=2U)continue;
        for(bit=0;bit<4U&&tr<tc;bit++)if(bits&(1U<<bit)){if(bit==3U)transparency=celtic_track_value(d,to+tr*0x14U,ticks);tr++;}
        break;
    }
    if(transparency<0.0f)transparency=0.0f;if(transparency>1.0f)transparency=1.0f;
    if(animated_alpha)*animated_alpha=(u8)((1.0f-transparency)*255.0f+0.5f);
}

void TNT_Vita_CelticLeavesBuildPose(u32 ticks,f32 out_nodes[TNT_CELTIC_LEAF_NODE_COUNT][9]){
    memcpy(out_nodes,g_tnt_celtic_leaf_nodes,sizeof(f32)*TNT_CELTIC_LEAF_NODE_COUNT*9U);
    celtic_apply_nodes(g_tnt_celtic_leaf_anim_1336,ticks,&out_nodes[0][0],TNT_CELTIC_LEAF_NODE_COUNT);
}
