/* User-owned The New Tetris USA 1.0 FX scene/texture/animation extraction.
 * FX uses fixed 16-byte big-endian resource records with type, compression,
 * compressed length, output length, and payload offset. An original ROM is
 * required at runtime; this source contains no original game asset bytes.
 */
#include "tntv_fx_reader.h"
#include "third_party/lzokay/lzokay-c/lzokay-c.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>

#define TNTV_FX_DIRECTORY 0x000A8190U
#define TNTV_FX_PAYLOADS  0x000ADAE0U
#define TNTV_FX_HEADER_BYTES 8U
#define TNTV_FX_ENTRY_BYTES 16U
#define TNTV_FX_TYPE_TEXTURE 1U
#define TNTV_FX_TYPE_SCENE 5U
#define TNTV_FX_TYPE_ANIM 6U
#define TNTV_FX_MAX_ID 1700U
#define TNTV_FX_MAX_COMPRESSED (4U*1024U*1024U)
#define TNTV_FX_MAX_OUTPUT (4U*1024U*1024U)
#define TNTV_FX_MAX_SCENE_TEXTURES 256U
#define TNTV_FX_MAX_WONDER_TEXELS_BYTES (2U*1024U*1024U)

static uint16_t read_be16(const uint8_t *p) {
    return (uint16_t)(((uint16_t)p[0]<<8)|p[1]);
}
static uint32_t read_be32(const uint8_t *p) {
    return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|
           ((uint32_t)p[2]<<8)|p[3];
}

/* Reads one original ROM FX resource into a private owned buffer. The
 * expected type prevents accidentally treating meshes/scenes as textures. */
static int fx_read(TNTVRom *rom,uint32_t id,uint16_t expected_type,
                   uint8_t **out,uint32_t *outlen) {
    uint8_t desc[16];
    uint16_t type,compression;
    uint32_t encoded_bytes,decoded_bytes,offset;
    uint8_t *encoded=NULL,*decoded=NULL;
    size_t got;
    int rc=-1;
    lzokay_EResult status;
    if(!rom||!rom->file||!out||!outlen)return -1;
    *out=NULL;
    *outlen=0;
    if(id>TNTV_FX_MAX_ID)return -2;
    if(tntv_rom_read(rom,TNTV_FX_DIRECTORY+TNTV_FX_HEADER_BYTES+
                     TNTV_FX_ENTRY_BYTES*id,desc,sizeof(desc))!=TNTV_ROM_OK)
        return -3;
    type=read_be16(desc);
    compression=read_be16(desc+2);
    encoded_bytes=read_be32(desc+4);
    decoded_bytes=read_be32(desc+8);
    offset=read_be32(desc+12);
    if(type!=expected_type || compression>1U ||
       encoded_bytes==0 || encoded_bytes>TNTV_FX_MAX_COMPRESSED ||
       decoded_bytes==0 || decoded_bytes>TNTV_FX_MAX_OUTPUT ||
       TNTV_FX_PAYLOADS>rom->length ||
       offset>rom->length-TNTV_FX_PAYLOADS ||
       encoded_bytes>rom->length-(TNTV_FX_PAYLOADS+offset))
        return -4;
    decoded=(uint8_t*)malloc(decoded_bytes);
    if(!decoded)return -5;
    if(compression==0){
        if(encoded_bytes!=decoded_bytes){rc=-6;goto done;}
        if(tntv_rom_read(rom,TNTV_FX_PAYLOADS+offset,decoded,
                         decoded_bytes)!=TNTV_ROM_OK){rc=-7;goto done;}
    }else{
        encoded=(uint8_t*)malloc(encoded_bytes);
        if(!encoded){rc=-8;goto done;}
        if(tntv_rom_read(rom,TNTV_FX_PAYLOADS+offset,encoded,
                         encoded_bytes)!=TNTV_ROM_OK){rc=-9;goto done;}
        got=decoded_bytes;
        status=lzokay_decompress(encoded,encoded_bytes,decoded,&got);
        if((status!=EResult_Success && status!=EResult_InputNotConsumed) ||
           got!=decoded_bytes){rc=-10;goto done;}
    }
    *out=decoded;
    *outlen=decoded_bytes;
    decoded=NULL;
    rc=0;
done:
    free(encoded);
    free(decoded);
    return rc;
}

int tntv_fx_extract_animation(TNTVRom *rom,uint32_t id,
                              uint8_t **out,uint32_t *outlen) {
    return fx_read(rom,id,TNTV_FX_TYPE_ANIM,out,outlen);
}

/* The completed-Wonder scene stores a BE32 list of texture FX IDs at its
 * scene texture-list offset. Every referenced original type1 texture has
 * BE16 width/height and BE32 pixel byte offset. Concatenate the raw BE16
 * texels. This preserves exact approved source values and ordering even for
 * N64 textures with extra mip/tile/swizzle data, since only level0 w*h
 * texels were previously embedded in the canonical TNTV source array.
 * The existing cache loader swaps these BE16 values to Vita-native u16.
 */
int tntv_fx_extract_wonder_texels(TNTVRom *rom,uint32_t scene_id,
                                  uint8_t **out,uint32_t *outlen) {
    uint8_t *scene=NULL,*tex=NULL,*result=NULL,*grown=NULL;
    uint32_t scene_bytes=0,tex_bytes=0,table,count,total=0,i;
    int rc=-1;
    if(!out||!outlen||!rom)return -1;
    *out=NULL;
    *outlen=0;
    rc=fx_read(rom,scene_id,TNTV_FX_TYPE_SCENE,&scene,&scene_bytes);
    if(rc!=0)return -2;
    if(scene_bytes<0x3CU){rc=-3;goto fail;}
    count=read_be32(scene+0x0C);
    table=read_be32(scene+0x30);
    if(count==0 || count>TNTV_FX_MAX_SCENE_TEXTURES ||
       table>scene_bytes || count>(scene_bytes-table)/4U) {
        rc=-4;goto fail;
    }
    for(i=0;i<count;i++){
        uint32_t id=read_be32(scene+table+4U*i);
        uint32_t width,height,offset,needed;
        rc=fx_read(rom,id,TNTV_FX_TYPE_TEXTURE,&tex,&tex_bytes);
        if(rc!=0){rc=-5;goto fail;}
        if(tex_bytes<16U){rc=-6;goto fail;}
        width=read_be16(tex+2);
        height=read_be16(tex+4);
        offset=read_be32(tex+12);
        if(!width||!height||width>256U||height>256U||
           width>UINT32_MAX/(height*2U)){rc=-7;goto fail;}
        needed=width*height*2U;
        if(offset>tex_bytes || needed>tex_bytes-offset ||
           needed>TNTV_FX_MAX_WONDER_TEXELS_BYTES-total) {
            rc=-8;goto fail;
        }
        grown=(uint8_t*)realloc(result,total+needed);
        if(!grown){rc=-9;goto fail;}
        result=grown;
        grown=NULL;
        memcpy(result+total,tex+offset,needed);
        total+=needed;
        free(tex);
        tex=NULL;
    }
    *out=result;
    *outlen=total;
    result=NULL;
    rc=0;
fail:
    free(scene);
    free(tex);
    free(result);
    return rc;
}

/* Native scene table reconstruction. No original game data is embedded.
 * Kinds: 0 node floats, 1 node types, 2 parent indices, 3 mesh indices,
 * 4 mesh scales, 5 texture descriptors, 6 gameplay nodes [9 floats],
 * 7 gameplay RGBA16 texels [1360], zero-pad a verified 1024-only texture;
 * 8 actual per-texture RGBA16 texel count [u16]. Output native endian.
 */
static int scene_range(uint32_t size,uint32_t off,uint32_t count,uint32_t stride) {
    return off<=size && stride && count<=(size-off)/stride;
}
static void scene_float(uint8_t *dst,const uint8_t *src,int canonicalize) {
    uint32_t bits=read_be32(src);
    float value;
    memcpy(&value,&bits,4);
    /* Match the historical extractor's canonicalization before .9g output. */
    if(canonicalize && value<5e-12 && value> -5e-12) value=0.0f;
    memcpy(dst,&value,4);
}
int tntv_fx_extract_scene_table(TNTVRom *rom,uint32_t scene_id,uint32_t kind,
                               uint8_t **out,uint32_t *outlen) {
    uint8_t *s=NULL,*mesh=NULL,*result=NULL;
    uint32_t size=0,meshsize=0,nn,nm,nt,nc,no,mo,to,co,i,j,total=0;
    int rc=-1;
    if(!out||!outlen||!rom)return -1;
    *out=NULL; *outlen=0;
    if(sizeof(float)!=4 || kind>8)return -1;
    if(kind==8 && scene_id!=901)return -1;
    if(fx_read(rom,scene_id,TNTV_FX_TYPE_SCENE,&s,&size)!=0)return -2;
    if(size<0x3c)goto done;
    nn=read_be32(s);nm=read_be32(s+8);nt=read_be32(s+12);
    nc=read_be32(s+0x14);no=read_be32(s+0x18);
    mo=read_be32(s+0x2c);to=read_be32(s+0x30);co=read_be32(s+0x38);
    if(!nn||nn>128||!nm||nm>255||nt>255||(!nt && (kind==5 || kind==7 || kind==8))||
       !scene_range(size,no,nn,64)||!scene_range(size,mo,nm,4)||
       !scene_range(size,to,nt,4)||!scene_range(size,co,nc,2))goto done;
    total=kind==6?nn*36:kind==7?nt*2720:kind==8?nt*2:kind==0?nn*48:kind<=3?nn:kind==4?nm*4:nt*12;
    if(total>4U*1024U*1024U)goto done;
    result=(uint8_t*)calloc(total,1);
    if(!result){rc=-3;goto done;}
    if(kind==0 || kind==6) {
        const uint32_t stride=kind==6?36U:48U, components=kind==6?9U:12U;
        for(i=0;i<nn;i++)for(j=0;j<components;j++)
            scene_float(result+i*stride+j*4,s+no+i*64+16+j*4,scene_id!=186);
    } else if(kind==1) {
        for(i=0;i<nn;i++)result[i]=s[no+i*64+2];
    } else if(kind==2) {
        memset(result,255,total);
        for(i=0;i<nn;i++) {
            uint32_t count=s[no+i*64+3],off=read_be32(s+no+i*64+4);
            if(!scene_range(size,off,count,4))goto done;
            for(j=0;j<count;j++){
                uint32_t child=read_be32(s+off+j*4);
                if(child>=nn)goto done;
                result[child]=(uint8_t)i;
            }
        }
    } else if(kind==3) {
        uint32_t current_mesh=0,current_node=0;
        memset(result,255,total);
        for(i=0;i<nc;i++) {
            uint32_t command=read_be16(s+co+2*i),op=command>>8,v=command&255;
            if(op==4){if(v>=nm)goto done;current_mesh=v;}
            else if(op==2){if(v>=nn)goto done;current_node=v;result[v]=(uint8_t)current_mesh;}
            else if(op==5)result[current_node]=(uint8_t)current_mesh;
            else if(op!=1 && op!=3)goto done; /* Texture selectors do not change nodes. */
        }
    } else if(kind==4) {
        for(i=0;i<nm;i++) {
            if(fx_read(rom,read_be32(s+mo+4*i),2,&mesh,&meshsize)!=0)goto done;
            if(meshsize<12)goto done;
            scene_float(result+4*i,mesh+8,scene_id!=186);
            free(mesh);mesh=NULL;
        }
    } else if(kind==7 || kind==8) {
        /* Source texture records are 32x32 RGBA16. Most carry exactly 1360
         * texels (all mips); Russian sky scene901's third has ONLY 1024.
         * The approved port pads the missing 336 texels with zeros, retaining
         * the 1360-texel in-memory stride and exposes raw source length.
         * Validate the record boundary BEFORE reading any texels. */
        for(i=0;i<nt;i++) {
            uint32_t source_id=read_be32(s+to+i*4),start,count;
            if(fx_read(rom,source_id,1,&mesh,&meshsize)!=0)goto done;
            if(meshsize<16 || read_be16(mesh+2)!=32 || read_be16(mesh+4)!=32)goto done;
            start=read_be32(mesh+12);
            if(start>meshsize || (meshsize-start)%2U)goto done;
            count=(meshsize-start)/2U;
            if(count!=1360U && !(scene_id==901 && i==2 && count==1024U))goto done;
            if(kind==8) {
                uint16_t actual=(uint16_t)count;
                memcpy(result+i*2,&actual,2);
            } else {
                /* calloc initializes the missing final 336 pixels to zero. */
                for(j=0;j<count;j++) {
                    uint16_t sample=read_be16(mesh+start+j*2);
                    memcpy(result+(i*1360+j)*2,&sample,2);
                }
            }
            free(mesh);mesh=NULL;
        }
    } else {
        uint32_t texel_offset=0;
        for(i=0;i<nt;i++) {
            uint32_t width,height,pixels,offset;
            uint16_t w,h;
            if(fx_read(rom,read_be32(s+to+4*i),1,&mesh,&meshsize)!=0)goto done;
            if(meshsize<16)goto done;
            width=read_be16(mesh+2);height=read_be16(mesh+4);offset=read_be32(mesh+12);
            if(!width||!height||width>256||height>256)goto done;
            pixels=width*height;
            if(!scene_range(meshsize,offset,pixels,2)||
               pixels>UINT32_MAX-texel_offset)goto done;
            w=(uint16_t)width;h=(uint16_t)height;
            memcpy(result+12*i,&texel_offset,4);
            memcpy(result+12*i+4,&w,2);memcpy(result+12*i+6,&h,2);
            result[12*i+8]=mesh[6];
            /* Later culture renderers consume original TMEM layout and
             * mip levels in descriptor pad[0..1]. Mayan/Greek keep zero. */
            if(scene_id!=186 && scene_id!=314) {
                uint32_t pc=read_be32(mesh+8),steps=0,dxt=UINT32_MAX;
                for(;;) {
                    uint32_t w0,w1,op;
                    if(!scene_range(meshsize,pc,1,8)||++steps>meshsize/8)goto done;
                    w0=read_be32(mesh+pc);w1=read_be32(mesh+pc+4);pc+=8;op=w0>>24;
                    if(op==0xf3)dxt=w1&0xfff;
                    if(op==0xdf)break;
                }
                result[12*i+9]=(uint8_t)(dxt==0);
                result[12*i+10]=mesh[7];
            }
            texel_offset+=pixels;
            free(mesh);mesh=NULL;
        }
    }
    *out=result;*outlen=total;result=NULL;rc=0;
done:
    free(s);free(mesh);free(result);
    return rc;
}

/* Reconstruct six completed-Wonder triangle/color streams from original
 * FX mesh display lists. Source-only format logic; no original vertices. */
typedef struct {
    int16_t xyzst[5];
    uint8_t rgba[4];
    int valid,modified;
} SceneVertex;
typedef struct { uint8_t *data; uint32_t size,capacity; } SceneOutput;
static int scene_append(SceneOutput *out,const void *data,uint32_t bytes) {
    uint32_t cap;uint8_t *p;
    if(bytes>4U*1024U*1024U-out->size)return -1;
    if(out->size+bytes>out->capacity) {
        cap=out->capacity?out->capacity*2:4096;
        if(cap<out->size+bytes)cap=out->size+bytes;
        p=(uint8_t*)realloc(out->data,cap);
        if(!p)return -1;
        out->data=p;out->capacity=cap;
    }
    memcpy(out->data+out->size,data,bytes);out->size+=bytes;return 0;
}
static double scene_material_float(const uint8_t *p) {
    uint32_t bits=read_be32(p);float f;memcpy(&f,&bits,4);return (double)f;
}
static uint32_t scene_color_byte(double x) {
    if(x<0.0)x=0.0;
    if(x>1.0)x=1.0;
    return (uint32_t)(x*255.0);
}
/* Replicate Python extractor's clamp and round-half-to-even, not C round(). */
static uint8_t scene_round_stage_alpha(double value) {
    uint32_t whole;
    double fractional;
    if(value<0.0)value=0.0;
    if(value>255.0)value=255.0;
    whole=(uint32_t)value;
    fractional=value-(double)whole;
    if(fractional>0.5 || (fractional==0.5 && (whole&1U)))whole++;
    return (uint8_t)whole;
}
static uint8_t scene_effect_group(uint32_t scene,uint32_t mesh,uint32_t mat) {
    if(mat)return 0;
    if(scene==470) {
        switch(mesh){case 428:return 1;case 429:return 2;case 430:return 3;
        case 431:return 4;case 433:return 5;case 434:return 6;default:return 0;}
    }
    if(scene==557) {
        switch(mesh){case 542:return 1;case 543:return 2;case 544:return 3;
        case 1394:return 4;default:return 0;}
    }
    return 0;
}
int tntv_fx_extract_scene_meshes(TNTVRom *rom,uint32_t scene_id,uint32_t colors,
                                uint8_t **out,uint32_t *outlen) {
    uint8_t *s=NULL,*mesh=NULL;uint32_t size=0,ms=0;
    uint32_t nn,nm,nt,nc,mo,co,ci,curmesh=0,node=0,texture=255;
    SceneOutput output={NULL,0,0};int rc=-1;
    if(!out||!outlen||!rom)return -1;
    *out=NULL;*outlen=0;
    if(colors>4 || (colors==4 && scene_id!=1370) ||
       (colors==3 && scene_id!=901) ||
       (colors==2 && scene_id!=1370 && scene_id!=1360 && scene_id!=1335 && scene_id!=567 && scene_id!=487) ||
       (colors==1 && (scene_id==1370 || scene_id==1360 || scene_id==567 || scene_id==1335 || scene_id==770 || scene_id==901)) ||
       (scene_id!=1006 && scene_id!=1088 && scene_id!=1104 && scene_id!=1057 &&
        scene_id!=1046 && scene_id!=1148 && scene_id!=1244 && scene_id!=1292 && scene_id!=1125 && scene_id!=1071 && scene_id!=1425 && scene_id!=201 && scene_id!=1399 && scene_id!=1318 && scene_id!=1370 && scene_id!=901 && scene_id!=1360 && scene_id!=770 && scene_id!=1335 && scene_id!=487 && scene_id!=567 && scene_id!=186 && scene_id!=314 && scene_id!=374 && scene_id!=470 &&
                   scene_id!=557 && scene_id!=755 && scene_id!=888))return -1;
    if(fx_read(rom,scene_id,5,&s,&size)!=0)return -2;
    if(size<0x3c)goto done;
    nn=read_be32(s);nm=read_be32(s+8);nt=read_be32(s+12);
    nc=read_be32(s+20);mo=read_be32(s+44);co=read_be32(s+56);
    if(!nn||nn>255||!nm||nm>255||nt>255||
       !scene_range(size,mo,nm,4)||!scene_range(size,co,nc,2))goto done;
    for(ci=0;ci<nc;ci++) {
        uint32_t command=read_be16(s+co+ci*2),op=command>>8,value=command&255;
        if(op==1){if(value>=nt && value!=255)goto done;texture=value;}
        else if(op==2){if(value>=nn)goto done;node=value;}
        else if(op==4){if(value>=nm)goto done;curmesh=value;}
        else if(op==3) {
            /* Accepted Mayan conversion leaves the current static texture
             * selected across animated selector commands. Preserve it exactly. */
            if(scene_id==186) {
                uint32_t count=read_be32(s+16),table=read_be32(s+52);
                if(value>=count||!scene_range(size,table,count,8))goto done;
            } else if((scene_id==470 && value<3)||(scene_id==557 && value<1))texture=252+value;
            else goto done;
        } else if(op==5) {
            uint32_t rid=read_be32(s+mo+curmesh*4),mc,dlo,vbo,mato,mat;
            SceneVertex cache[32];memset(cache,0,sizeof(cache));
            if(fx_read(rom,rid,2,&mesh,&ms)!=0)goto done;
            if(ms<32)goto done;
            mc=read_be32(mesh+4);dlo=read_be32(mesh+12);
            vbo=read_be32(mesh+16);mato=read_be32(mesh+28);
            if(value>=mc||mc>256||!scene_range(ms,dlo,mc,4)||
               (scene_id!=186 && scene_id!=314&&!scene_range(ms,mato,mc,20)))goto done;
            /* RSP vertex cache persists across material display lists.
             * Replay predecessors but emit only the scene-selected material. */
            for(mat=0;mat<=value;mat++) {
                uint32_t pc=read_be32(mesh+dlo+mat*4),steps=0,mf=0;
                double material[4]={1,1,1,0};
                if(scene_id!=186 && scene_id!=314) {
                    uint32_t k;mf=read_be32(mesh+mato+mat*20);
                    for(k=0;k<4;k++)material[k]=scene_material_float(mesh+mato+mat*20+4+k*4);
                }
                if(scene_id==770 && mat==value && (mf!=0x5c || texture==255))goto done;
                if(scene_id==1360 && mat==value && (mf!=0x54 || texture!=255))goto done;
                if(scene_id==901 && mat==value && (mf!=0x5c || texture==255))goto done;
                if(scene_id==1370 && mat==value && (texture!=255 || (mf!=0x54 && mf!=0x5c)))goto done;
                if(scene_id==1318 && mat==value && (texture==255 || mf!=0x4c))goto done;
                if(scene_id==1399 && mat==value && (texture==255 || mf!=0x54))goto done;
                if(scene_id==201 && mat==value && (texture==255 || mf!=0x54))goto done;
                if(scene_id==1425 && mat==value && (texture==255 || mf!=0x55))goto done;
                if(scene_id==1071 && mat==value && (texture==255 || mf!=0x55))goto done;
                if(scene_id==1125 && mat==value && (texture>=3U || mf!=0x54))goto done;
                if(scene_id==1292 && mat==value && (texture>=6U || mf!=0x54))goto done;
                if(scene_id==1244 && mat==value && (texture>=4U || mf!=0x54))goto done;
                if(scene_id==1148 && mat==value && (texture>=4U || mf!=0x54))goto done;
                if(scene_id==1046 && mat==value && (texture>=20U || mf!=0x54))goto done;
                if((scene_id==1006 || scene_id==1104 || scene_id==1057) &&
                   mat==value && (texture>=nt || mf!=0x54))goto done;
                if(scene_id==1088 && mat==value && (texture>=nt || mf!=0x5c))goto done;
                for(;;) {
                    uint32_t w0,w1,cmd;
                    if(!scene_range(ms,pc,1,8)||++steps>ms/8)goto done;
                    w0=read_be32(mesh+pc);w1=read_be32(mesh+pc+4);pc+=8;cmd=w0>>24;
                    if(cmd==1) {
                        uint32_t n=(w0>>12)&255,end=(w0>>1)&127,off=w1&0xffffff,j;
                        if(end>32||n>end||vbo>ms||off>ms-vbo||
                           !scene_range(ms,vbo+off,n,16))goto done;
                        for(j=0;j<n;j++){
                            SceneVertex *v=&cache[end-n+j];const uint8_t *p=mesh+vbo+off+16*j;
                            uint32_t k;
                            for(k=0;k<3;k++)v->xyzst[k]=(int16_t)read_be16(p+2*k);
                            v->xyzst[3]=(int16_t)read_be16(p+8);
                            v->xyzst[4]=(int16_t)read_be16(p+10);
                            memcpy(v->rgba,p+12,4);v->valid=1;v->modified=0;
                        }
                    } else if(cmd==2) {
                        uint32_t idx=(w0&65535)/2;
                        if(((w0>>16)&255)!=0x14||(w0&1)||idx>=32||!cache[idx].valid)goto done;
                        cache[idx].xyzst[3]=(int16_t)(w1>>16);
                        cache[idx].xyzst[4]=(int16_t)w1;cache[idx].modified=1;
                    } else if(cmd==5) {
                        uint8_t tri[46]={0};uint32_t color[3]={0},j;
                        tri[0]=(uint8_t)texture;tri[1]=(uint8_t)node;
                        if(scene_id!=186 && scene_id!=314)tri[2]=(uint8_t)mf;
                        if(scene_id==470||scene_id==557)
                            tri[3]=scene_effect_group(scene_id,rid,mat)|(mf&0x200?8:0);
                        for(j=0;j<3;j++) {
                            uint32_t encoded=(w0>>(16-8*j))&255,idx=encoded/2,k;
                            SceneVertex *v;
                            if((encoded&1)||idx>=32||!cache[idx].valid)goto done;
                            v=&cache[idx];
                            for(k=0;k<5;k++){
                                int value16=v->xyzst[k];uint16_t bits;
                                /* Greek milestone scene1125 uses original raw
                                 * G_MODIFYVTX ST, unlike the double-modified
                                 * S/T effective coordinates of Greek/Mayan sky. */
                                if(scene_id!=186 && scene_id!=1125 && scene_id!=1046 &&
                                   scene_id!=1006 && scene_id!=1104 &&
                                   k>=3 && v->modified)value16*=2;
                                bits=(uint16_t)value16;
                                memcpy(tri+4+j*14+k*2,&bits,2);
                            }
                            memcpy(tri+4+j*14+10,v->rgba,4);
                            if(scene_id==186)tri[4+j*14+13]=0; /* Original native vertex padding. */
                            if(colors && texture==255) {
                                double vr=1,vg=1,vb=1;
                                if(mf&8){vr=v->rgba[0]/255.0;vg=v->rgba[1]/255.0;vb=v->rgba[2]/255.0;}
                                color[j]=scene_color_byte(vr*material[0])|
                                  (scene_color_byte(vg*material[1])<<8)|
                                  (scene_color_byte(vb*material[2])<<16)|
                                  (scene_color_byte((1.0-material[3])*(v->rgba[3]/255.0))<<24);
                            }
                        }
                        if(mat==value) {
                            if(scene_id==487 || scene_id==1335 || scene_id==770 ||
                               scene_id==1360 || scene_id==901 || scene_id==1370 || scene_id==1318 ||
                               scene_id==1399 || scene_id==201 || scene_id==1425 || scene_id==1071) {
                                if(scene_id==201 && colors==0 && texture!=255 && mf==0x54) {
                                    uint8_t mayan_tri[46]={0};
                                    /* TNTMayanSkyboxTriangle: texture byte and THREE zero
                                     * header pad bytes, then exactly three 14B vertices.
                                     * Previous scene1399 uses a DIFFERENT 44B type. */
                                    /* Original scene201 command order is mesh materials
                                     * 1,2,3,4,5,6,7,0, already approved and faithful;
                                     * preserve its original texture selector unchanged. */
                                    mayan_tri[0]=(uint8_t)texture;
                                    memcpy(mayan_tri+4,tri+4,42);
                                    /* The approved TNTV Mayan skybox intentionally
                                     * stores a zero vertex alpha for every vertex. */
                                    {
                                        uint32_t vi;
                                        for(vi=0;vi<3U;vi++)
                                            mayan_tri[4U+vi*14U+13U]=0;
                                    }
                                    if(scene_append(&output,mayan_tri,sizeof(mayan_tri))!=0)goto done;
                                } else
                                if((scene_id==487 && colors==0 && texture!=255) ||
                                   (scene_id==770 && colors==0 && texture!=255 && mf==0x5c) ||
                                   (scene_id==901 && colors==0 && texture!=255 && mf==0x5c) ||
                                   (scene_id==1318 && colors==0 && texture!=255 && mf==0x4c) ||
                                   (scene_id==1399 && colors==0 && texture!=255 && mf==0x54) ||
                                   (scene_id==1425 && colors==0 && texture!=255 && mf==0x55) ||
                                   (scene_id==1071 && colors==0 && texture!=255 && mf==0x55) ||
                                   (colors==2 && texture==255 &&
                                    (scene_id!=1360 || mf==0x54) &&
                                    (scene_id!=1370 || mf==0x54 || mf==0x5c))) {
                                    uint8_t sky_tri[44];
                                    /* Standard TNTWonderTriangle: 2-byte material/node
                                     * header, followed by three 14-byte N64 vertices. */
                                    memcpy(sky_tri,tri,2);
                                    memcpy(sky_tri+2,tri+4,42);
                                    /* Approved locked Wonder ornament uses zero
                                     * vertex alpha, unlike ROM G_VTX bytes255.
                                     * Keep original normal, XYZ, UV and draw order. */
                                    if(scene_id==1071) {
                                        uint32_t vi;
                                        for(vi=0;vi<3U;vi++)
                                            sky_tri[2U+14U*vi+13U]=0;
                                    }
                                    if(scene_append(&output,sky_tri,sizeof(sky_tri))!=0)goto done;
                                } else if(scene_id==1370 && colors==4 && texture==255) {
                                    uint8_t lit=(uint8_t)(mf==0x54);
                                    if(scene_append(&output,&lit,1)!=0)goto done;
                                } else if(scene_id==901 && colors==3 && texture!=255 && mf==0x5c) {
                                    uint8_t a=scene_round_stage_alpha((1.0-material[3])*255.0);
                                    if(scene_append(&output,&a,1)!=0)goto done;
                                } else if(scene_id==487 && colors==1 && texture!=255) {
                                    /* Historical Celtic: class1 animated sky meshes;
                                     * class2 translucent otherwise, class0 ordinary. */
                                    uint8_t cls=(rid==477||rid==479||rid==480||rid==481)?
                                        1U:(material[3]>0.0001?2U:0U);
                                    if(scene_append(&output,&cls,1)!=0)goto done;
                                }
                            } else if(scene_id==567 || scene_id==1046) {
                                if(colors==0) {
                                    uint8_t sky_tri[44];
                                    /* TNTWonderTriangle has two header bytes and
                                     * three native 14B original N64 RSP vertices.
                                     * Mayan completed Wonder source keeps RSP
                                     * vertex alpha/padding byte zero. */
                                    memcpy(sky_tri,tri,2);
                                    memcpy(sky_tri+2,tri+4,42);
                                    if(scene_id==1046) {
                                        uint32_t vi;
                                        for(vi=0;vi<3U;vi++)
                                            sky_tri[2U+vi*14U+13U]=0;
                                    }
                                    if(scene_append(&output,sky_tri,sizeof(sky_tri))!=0)goto done;
                                } else if(scene_id==567 && colors==2) {
                                    uint8_t a=scene_round_stage_alpha((1.0-material[3])*255.0);
                                    if(scene_append(&output,&a,1)!=0)goto done;
                                }
                            } else {
                                if(!colors && (scene_id==1006 || scene_id==1088 ||
                                               scene_id==1104 || scene_id==1057)) {
                                    uint32_t index,texoff,resource_id;
                                    texoff=read_be32(s+48U);
                                    if(texture>=nt || !scene_range(size,texoff,nt,4U))goto done;
                                    resource_id=read_be32(s+texoff+4U*texture);
                                    /* The original Wonder milestone textures are
                                     * addressed by resource identity, NOT the
                                     * scene's arbitrary command selector order.
                                     * Small Mayan atlas IDs1008..1027 go to0..19;
                                     * large 5-image atlas IDs1049..1053 to20..24;
                                     * big scene1088 atlas IDs1073..1082 to25..34. */
                                    if(scene_id==1006) {
                                        if(resource_id<1008U||resource_id>1027U)goto done;
                                        index=resource_id-1008U;
                                    } else if(scene_id==1088) {
                                        if(resource_id<1073U||resource_id>1082U)goto done;
                                        index=resource_id-1073U+25U;
                                    } else {
                                        if(resource_id<1049U||resource_id>1053U)goto done;
                                        index=resource_id-1049U+20U;
                                    }
                                    tri[0]=(uint8_t)index;
                                    tri[2]=(uint8_t)(mf==0x5c);
                                }
                                if(!colors && scene_id==1148) {
                                    /* Original Egyptian Wonder construction atlas
                                     * maps four FX scene texture selectors 0..3
                                     * into approved atlas slots35..38. */
                                    tri[0]=(uint8_t)(texture+35U);
                                    tri[2]=0;
                                }
                                if(!colors && scene_id==1244) {
                                    /* Celtic Wonder accepted milestone atlas slots
                                     * 39..42 from original N64 scene texture order.
                                     * Native header unlit and pad are zero, vertex
                                     * ST preserves original G_MODIFYVTX doubling. */
                                    tri[0]=(uint8_t)(texture+39U);
                                    tri[2]=0;
                                }
                                if(!colors && scene_id==1292) {
                                    /* African milestone texture atlas slots43..48
                                     * match original scene1292 FX selector order.
                                     * Milestone header: unlit=0/pad=0 (not 0x54). */
                                    tri[0]=(uint8_t)(texture+43U);
                                    tri[2]=0;
                                }
                                if(!colors && scene_id==1125) {
                                    /* Scene1125's three FX texture indices 0..2
                                     * map to milestone atlas texture slots 20..22.
                                     * The approved triangle's unlit byte is zero;
                                     * raw mesh material flag 0x54 is NOT copied
                                     * into this milestone-specific header. */
                                    /* Original scene's texture ID order is
                                     * 1050,1051,1049; approved milestone atlas
                                     * mapping is slots21,22,20, respectively. */
                                    tri[0]=(uint8_t)(20U+((texture+1U)%3U));
                                    tri[2]=0;
                                }
                                if(!colors && scene_append(&output,tri,sizeof(tri))!=0)goto done;
                                if(colors && texture==255 && scene_append(&output,color,sizeof(color))!=0)goto done;
                            }
                        }
                    } else if(cmd==0xdf)break;
                    else goto done;
                }
            }
            free(mesh);mesh=NULL;
        } else goto done;
    }
    if(!output.size)goto done;
    *out=output.data;*outlen=output.size;output.data=NULL;rc=0;
done:
    free(mesh);free(s);free(output.data);return rc;
}

/* Original N64 FX type6 polynomial keyframe track sampling.
 * All source samples are read from a user-verified original ROM, not embedded.
 * Reconstructs the approved one-shot African Wonder node poses and camera.
 * This code is appended to the source-only FX reader.
 */
static double tntv_anim_be_float(const uint8_t *p) {
    uint32_t bits=read_be32(p);
    float value;
    memcpy(&value,&bits,sizeof(value));
    return (double)value;
}
static int tntv_anim_track_coefficients(const uint8_t *d,uint32_t size,
                                        uint32_t key,int subsequent,
                                        uint32_t *mask,double p[4]) {
    uint32_t off=read_be32(d+key+4),m=read_be16(d+key+2),i,skip=0;
    if(m>15U || off>size)return -1;
    if(!subsequent){for(i=0;i<4;i++)p[i]=0;}
    else while(skip<4U && !(m&(1U<<skip)))skip++;
    for(i=skip;i<4U;i++) {
        if(m&(1U<<i)) {
            if(!scene_range(size,off,1,4))return -1;
            p[i]=tntv_anim_be_float(d+off);
            off+=4U;
        } else p[i]=0.0;
    }
    *mask=m;
    return 0;
}
static void tntv_anim_advance(double p[4],uint32_t mask,double ticks) {
    double choose2=ticks*(ticks-1.0)*0.5;
    double choose3=ticks*(ticks-1.0)*(ticks-2.0)/6.0;
    p[0]+=ticks*p[1];
    if(mask&4U)p[0]+=choose2*p[2];
    if(mask&8U)p[0]+=choose3*p[3];
    if(mask&4U) {
        p[1]+=ticks*p[2];
        if(mask&8U)p[1]+=choose2*p[3];
    }
    if(mask&8U)p[2]+=ticks*p[3];
}
static int tntv_anim_track_value(const uint8_t *d,uint32_t size,
                                 uint32_t track,uint32_t ticks,double *result) {
    uint32_t terminal,key,mask=0,cur;
    double p[4]={0,0,0,0};
    if(!result||!scene_range(size,track,1,20))return -1;
    terminal=read_be16(d+track+4);
    key=read_be32(d+track+12);
    if(!scene_range(size,key,1,8) ||
       tntv_anim_track_coefficients(d,size,key,0,&mask,p)!=0)return -2;
    if(!terminal){*result=p[0];return 0;}
    ticks%=terminal;
    if(!ticks){*result=p[0];return 0;}
    cur=1U;
    while(cur<=ticks) {
        uint32_t nk=key+8U,nt,stop=ticks+1U;
        if(!scene_range(size,nk,1,8))return -3;
        nt=read_be16(d+nk);
        if(nt==cur) {
            key=nk;
            if(tntv_anim_track_coefficients(d,size,key,1,&mask,p)!=0)return -4;
            tntv_anim_advance(p,mask,1.0);
            cur++;
            continue;
        }
        if(nt>cur && nt<stop)stop=nt;
        if(stop<=cur)return -5;
        tntv_anim_advance(p,mask,(double)(stop-cur));
        cur=stop;
    }
    *result=p[0];
    return 0;
}
/* Source C original arrays used 9 significant decimal digits: equivalent
 * float32 round-trip, after historical near-zero canonicalization.
 * This is a numeric FORMAT rule, not original game asset data. */
static int tntv_anim_float32(double v,float *out) {
    char digits[64],*end=NULL;
    float rounded;
    if(!out||!isfinite(v))return -1;
    if(v>-5e-12 && v<5e-12)v=0.0;
    if(snprintf(digits,sizeof(digits),"%.9g",v)<0)return -2;
    rounded=strtof(digits,&end);
    if(!end || *end!='\0' || !isfinite((double)rounded))return -3;
    *out=rounded;
    return 0;
}
/* Each sample starts from original FX type5 scene base node floats and then
 * applies all matching type6 node animation channels. First-node mode outputs
 * only the first node's six camera components; otherwise all 9 components of
 * every scene node, in approved C layout. */
int tntv_fx_sample_scene_animation(TNTVRom *rom,uint32_t scene_id,
                                    uint32_t animation_id,uint32_t first_tick,
                                    uint32_t sample_count,uint32_t first_node_only,
                                    uint8_t **out,uint32_t *outlen) {
    uint8_t *base=NULL,*scene=NULL,*anim=NULL,*result=NULL;
    uint32_t base_bytes=0,scene_bytes=0,anim_bytes=0,nn,entry_count,list;
    uint32_t each_nodes,comps,per,whole,tick,rc=-1;
    if(!rom||!out||!outlen||first_node_only>1U||
       !sample_count||sample_count>1000U||first_tick>10000U)return -1;
    *out=NULL;*outlen=0;
    /* Camera-only original FX scene1303 has no meshes or textures;
     * the standard scene-table reader correctly rejects such scenes.
     * Here we require only a valid type5 node array. */
    if(fx_read(rom,scene_id,TNTV_FX_TYPE_SCENE,&scene,&scene_bytes)!=0)return -2;
    if(scene_bytes<60U)goto done;
    nn=read_be32(scene);
    {
        uint32_t no=read_be32(scene+24U),i,j;
        if(!nn||nn>128U||!scene_range(scene_bytes,no,nn,64U))goto done;
        base_bytes=nn*36U;
        base=(uint8_t*)malloc(base_bytes);
        if(!base){rc=-3;goto done;}
        for(i=0;i<nn;i++)for(j=0;j<9U;j++)
            scene_float(base+i*36U+j*4U,scene+no+i*64U+16U+j*4U,
                        scene_id!=186U);
    }
    if(fx_read(rom,animation_id,TNTV_FX_TYPE_ANIM,&anim,&anim_bytes)!=0)goto done;
    if(anim_bytes<12U)goto done;
    entry_count=read_be32(anim+4);
    list=read_be32(anim+8);
    if(entry_count>128U || !scene_range(anim_bytes,list,entry_count,36U))goto done;
    each_nodes=first_node_only?1U:nn;
    comps=first_node_only?6U:9U;
    per=each_nodes*comps*4U;
    if(per>UINT32_MAX/sample_count || per*sample_count>4U*1024U*1024U)goto done;
    whole=per*sample_count;
    result=(uint8_t*)malloc(whole);
    if(!result){rc=-3;goto done;}
    for(tick=0;tick<sample_count;tick++) {
        double nodes[128U*9U];
        uint32_t ei,i,j;
        for(i=0;i<nn*9U;i++) {
            float native;
            memcpy(&native,base+i*4U,4);
            nodes[i]=(double)native;
        }
        for(ei=0;ei<entry_count;ei++) {
            uint32_t e=list+ei*36U,type=read_be32(anim+e+12U);
            uint32_t target=read_be32(anim+e+16U),bits=read_be32(anim+e+20U);
            uint32_t tracks=read_be32(anim+e+24U),trbase=read_be32(anim+e+28U);
            uint32_t track_index=0U,bit;
            if(type!=1U || target>=nn)continue;
            if(bits>511U || tracks>9U || !scene_range(anim_bytes,trbase,tracks,20U))goto done;
            for(bit=0;bit<9U;bit++) {
                if((bits&(1U<<bit)) && track_index<tracks) {
                    double value;
                    if(tntv_anim_track_value(anim,anim_bytes,trbase+20U*track_index,
                                              first_tick+tick,&value)!=0)goto done;
                    nodes[target*9U+bit]=value;
                    track_index++;
                }
            }
        }
        for(i=0;i<each_nodes;i++)for(j=0;j<comps;j++) {
            float value;
            if(tntv_anim_float32(nodes[i*9U+j],&value)!=0)goto done;
            memcpy(result+tick*per+(i*comps+j)*4U,&value,4);
        }
    }
    *out=result;*outlen=whole;result=NULL;rc=0;
done:
    free(base);free(scene);free(anim);free(result);
    return (int)rc;
}


/* Original seven African milestone pointer lists, represented as ROM OFFSETS,
 * not source asset contents. Each list has four non-block metadata pointers,
 * N actual block-struct pointers, then a NULL terminator.
 *
 * ROM pointer values are KSEG0 virtual addresses with the original game-code
 * mapping VRAM 0x80039D80 => ROM 0. Each 0x30-byte original static object
 * contains float XYZ at +8,+12,+16 and a float construction tick at +36.
 * The player's SHA-validated original retail ROM is the ONLY data source. */
int tntv_rom_restore_african_blocks(TNTVRom *rom,
                                     uint8_t counts[7],void *blocks,
                                     uint32_t block_stride,uint32_t block_bytes) {
    static const uint32_t list_rom_offsets[7] = {
        0x00094118U,0x000944A4U,0x00094854U,0x00094BD8U,
        0x00094F58U,0x000952D8U,0x00095658U
    };
    const uint32_t ram_to_rom=0x80039D80U;
    uint32_t stage,i,ptr,off,j;
    uint8_t word[4],record[48];
    uint8_t *dst=(uint8_t*)blocks;
    if(!rom || !rom->file || !counts || !blocks ||
       block_stride!=16U || block_bytes!=7U*19U*16U)return -1;
    memset(dst,0,block_bytes);
    memset(counts,0,7U);
    for(stage=0;stage<7U;stage++) {
        const uint32_t table=list_rom_offsets[stage];
        uint32_t count=0U;
        /* Four source-state/controller references preceding the blocks.
         * Each original list has a trailing zero-pointer sentinel. */
        for(i=4U;i<=23U;i++) {
            float value[4];
            uint16_t tick;
            if(tntv_rom_read(rom,table+i*4U,word,4U)!=TNTV_ROM_OK)return -2;
            ptr=read_be32(word);
            if(!ptr)break;
            if(count>=19U || ptr<ram_to_rom || ptr>0x81000000U)return -3;
            off=ptr-ram_to_rom;
            if(off>rom->length || 48U>rom->length-off)return -4;
            if(tntv_rom_read(rom,off,record,sizeof(record))!=TNTV_ROM_OK)return -5;
            for(j=0;j<3U;j++) {
                uint32_t bits=read_be32(record+8U+j*4U);
                memcpy(&value[j],&bits,4U);
                if(!isfinite((double)value[j]))return -6;
            }
            {
                uint32_t bits=read_be32(record+36U);
                memcpy(&value[3],&bits,4U);
            }
            if(!isfinite((double)value[3]) || value[3]<0.0f ||
               value[3]>65535.0f || (float)(uint16_t)value[3]!=value[3])
               return -7;
            tick=(uint16_t)value[3];
            memcpy(dst+(stage*19U+count)*16U,value,12U);
            memcpy(dst+(stage*19U+count)*16U+12U,&tick,2U);
            count++;
        }
        if(i>23U || !count)return -8;
        counts[stage]=(uint8_t)count;
    }
    return 0;
}

/* Greek construction stage original 7 pointer lists:
 * first 3 pointers reference engine state, then variable count of 48B
 * original N64 block records, ending in a NULL pointer.
 * Original KSEG0 VRAM maps to USA1.0 ROM via 0x80039d80.
 * Constant offsets are locators, not game asset data. */
int tntv_rom_restore_greek_blocks(TNTVRom *rom,uint8_t counts[7],
                                   void *blocks,uint32_t stride,uint32_t capacity) {
    static const uint32_t stages[7] = {
        0x0008cf94U,0x0008d260U,0x0008d600U,0x0008d8c4U,
        0x0008db74U,0x0008de24U,0x0008e0d4U
    };
    const uint32_t delta=0x80039d80U;
    uint8_t *dst=(uint8_t*)blocks;
    uint32_t stage,i,pointer,rom_offset,n,j;
    uint8_t word[4],record[48];
    if(!rom||!rom->file||!counts||!blocks||stride!=16U||
       capacity!=7U*19U*16U)return -1;
    memset(counts,0,7U);
    memset(dst,0,capacity);
    for(stage=0;stage<7U;stage++) {
        n=0;
        for(i=3U;i<=22U;i++) {
            float values[3],ticks;
            uint16_t tick16;
            if(tntv_rom_read(rom,stages[stage]+4U*i,word,4U)!=TNTV_ROM_OK)return -2;
            pointer=read_be32(word);
            if(!pointer)break;
            if(n>=19U||pointer<delta||pointer>0x81000000U)return -3;
            rom_offset=pointer-delta;
            if(rom_offset>rom->length||48U>rom->length-rom_offset)return -4;
            if(tntv_rom_read(rom,rom_offset,record,48U)!=TNTV_ROM_OK)return -5;
            for(j=0;j<3U;j++) {
                uint32_t bits=read_be32(record+8U+4U*j);
                memcpy(&values[j],&bits,sizeof(bits));
                if(!isfinite((double)values[j]))return -6;
            }
            {
                uint32_t bits=read_be32(record+36U);
                memcpy(&ticks,&bits,4U);
            }
            if(!isfinite((double)ticks)||ticks<0.0f||ticks>65535.0f||
               (float)(uint16_t)ticks!=ticks)return -7;
            tick16=(uint16_t)ticks;
            memcpy(dst+(stage*19U+n)*16U,values,12U);
            memcpy(dst+(stage*19U+n)*16U+12U,&tick16,2U);
            n++;
        }
        if(i>22U||!n)return -8;
        counts[stage]=(uint8_t)n;
    }
    return 0;
}

/* Original user-N64 ROM stage-list metadata: seven Celtic milestone pointer
 * lists, each with four fixed engine metadata pointers, followed by N
 * 48-byte original N64 falling-block records then a zero terminator.
 * VRAM to ROM mapping 0x80039D80, original object XYZ and integer tick
 * remain entirely in the player's verified original ROM. */
int tntv_rom_restore_celtic_blocks(TNTVRom *rom,uint8_t counts[7],
                                    void *blocks,uint32_t stride,uint32_t bytes) {
    static const uint32_t stage_offsets[7]={
        0x0009272cU,0x00092a7cU,0x00092d98U,0x00093110U,
        0x000934c0U,0x00093844U,0x00093bf4U
    };
    const uint32_t delta=0x80039d80U;
    uint8_t *dst=(uint8_t*)blocks;
    uint8_t word[4],record[48];
    uint32_t stage,i,n,ptr,off,j;
    if(!rom||!rom->file||!counts||!dst||stride!=16U||
       bytes!=7U*19U*16U)return -1;
    memset(counts,0,7U);
    memset(dst,0,bytes);
    for(stage=0;stage<7U;stage++) {
        n=0;
        for(i=4U;i<=23U;i++) {
            float xyz[3],tick;
            uint16_t time;
            if(tntv_rom_read(rom,stage_offsets[stage]+i*4U,word,4U)!=TNTV_ROM_OK)return -2;
            ptr=read_be32(word);
            if(!ptr)break;
            if(n>=19U||ptr<delta||ptr>0x81000000U)return -3;
            off=ptr-delta;
            if(off>rom->length||48U>rom->length-off)return -4;
            if(tntv_rom_read(rom,off,record,48U)!=TNTV_ROM_OK)return -5;
            for(j=0;j<3U;j++) {
                uint32_t bits=read_be32(record+8U+j*4U);
                memcpy(&xyz[j],&bits,4U);
                if(!isfinite((double)xyz[j]))return -6;
            }
            {
                uint32_t bits=read_be32(record+36U);
                memcpy(&tick,&bits,4U);
            }
            if(!isfinite((double)tick)||tick<0.0f||tick>65535.0f||
               (float)(uint16_t)tick!=tick)return -7;
            time=(uint16_t)tick;
            memcpy(dst+(stage*19U+n)*16U,xyz,12U);
            memcpy(dst+(stage*19U+n)*16U+12U,&time,2U);
            n++;
        }
        if(i>23U||!n)return -8;
        counts[stage]=(uint8_t)n;
    }
    return 0;
}

/* Seven original Egyptian milestone stage ROM pointer tables, each with
 * exactly three fixed engine metadata pointers before N original 48-byte
 * block objects and a NULL terminator. Source-only coordinate locators,
 * no original artwork, block positions or timing records embedded. */
int tntv_rom_restore_egyptian_blocks(TNTVRom *rom,uint8_t counts[7],
                                      void *blocks,uint32_t stride,uint32_t bytes) {
    static const uint32_t stage_offsets[7]={
        0x0008e4c4U,0x0008e7e0U,0x0008eb58U,0x0008ed58U,
        0x0008ef38U,0x0008f058U,0x0008f168U
    };
    const uint32_t delta=0x80039d80U;
    uint8_t *dst=(uint8_t*)blocks;
    uint8_t word[4],record[48];
    uint32_t stage,i,n,ptr,off,j;
    if(!rom||!rom->file||!counts||!dst||stride!=16U||bytes!=7U*19U*16U)return -1;
    memset(counts,0,7U);
    memset(dst,0,bytes);
    for(stage=0;stage<7U;stage++) {
        n=0;
        for(i=3U;i<=22U;i++) {
            float xyz[3],tick;
            uint16_t time;
            if(tntv_rom_read(rom,stage_offsets[stage]+i*4U,word,4U)!=TNTV_ROM_OK)return -2;
            ptr=read_be32(word);
            if(!ptr)break;
            if(n>=19U||ptr<delta||ptr>0x81000000U)return -3;
            off=ptr-delta;
            if(off>rom->length||48U>rom->length-off)return -4;
            if(tntv_rom_read(rom,off,record,48U)!=TNTV_ROM_OK)return -5;
            for(j=0;j<3U;j++) {
                uint32_t bits=read_be32(record+8U+j*4U);
                memcpy(&xyz[j],&bits,4U);
                if(!isfinite((double)xyz[j]))return -6;
            }
            {
                uint32_t bits=read_be32(record+36U);
                memcpy(&tick,&bits,4U);
            }
            if(!isfinite((double)tick)||tick<0.0f||tick>65535.0f||
               (float)(uint16_t)tick!=tick)return -7;
            time=(uint16_t)tick;
            memcpy(dst+(stage*19U+n)*16U,xyz,12U);
            memcpy(dst+(stage*19U+n)*16U+12U,&time,2U);
            n++;
        }
        if(i>22U||!n)return -8;
        counts[stage]=(uint8_t)n;
    }
    return 0;
}

/* Original Mayan Wonder completed/construction source scene1046.
 * Unlike the 32x32 main Wonder textures, these 8x8 RGBA16 images retain
 * 64 base texels plus 16 mip texels. Do not include 20x80 original pixels
 * in the GitHub source or VPK; decode them from the player's verified ROM. */
int tntv_fx_extract_mayan_wonder_mips(TNTVRom *rom,uint8_t **out,
                                       uint32_t *outlen) {
    uint8_t *scene=NULL,*texture=NULL,*result=NULL;
    uint32_t sn=0,tn=0,to,i,j;
    int rc=-1;
    if(!rom||!out||!outlen)return -1;
    *out=NULL;*outlen=0;
    if(fx_read(rom,1046U,TNTV_FX_TYPE_SCENE,&scene,&sn)!=0)return -2;
    if(sn<60U || read_be32(scene)!=18U || read_be32(scene+12U)!=20U)goto done;
    to=read_be32(scene+48U);
    if(!scene_range(sn,to,20U,4U))goto done;
    result=(uint8_t*)malloc(20U*80U*2U);
    if(!result){rc=-3;goto done;}
    for(i=0;i<20U;i++) {
        uint32_t rid=read_be32(scene+to+i*4U);
        uint32_t texoff;
        if(fx_read(rom,rid,TNTV_FX_TYPE_TEXTURE,&texture,&tn)!=0)goto done;
        if(tn<16U || read_be16(texture+2U)!=8U ||
           read_be16(texture+4U)!=8U)goto done;
        texoff=read_be32(texture+12U);
        if(texoff>tn || tn-texoff!=80U*2U)goto done;
        for(j=0;j<80U;j++) {
            uint16_t value=read_be16(texture+texoff+j*2U);
            memcpy(result+(i*80U+j)*2U,&value,2U);
        }
        free(texture);texture=NULL;
    }
    *out=result;*outlen=20U*80U*2U;
    result=NULL;rc=0;
done:
    free(scene);free(texture);free(result);
    return rc;
}

/* Original NTSC USA Mayan stage pointer lists 0..6.
 * Each has THREE game engine metadata pointers followed by N genuine block
 * pointers, a NULL terminator, and native zero padding to six blocks/stage.
 * Original block XYZ and construction ticks exist only in the verified ROM. */
int tntv_rom_restore_mayan_milestone_blocks(TNTVRom *rom,uint8_t counts[7],
                                             void *blocks,uint32_t stride,uint32_t bytes) {
    static const uint32_t original_stage_rom_offsets[7]={
        0x0008c364U,0x0008c4acU,0x0008c620U,0x0008c768U,
        0x0008c8acU,0x0008c9f0U,0x0008cb04U
    };
    const uint32_t delta=0x80039d80U;
    uint8_t *dst=(uint8_t*)blocks;
    uint32_t stage,i,n,ptr,off,j;
    uint8_t word[4],record[48];
    if(!rom||!rom->file||!counts||!dst||stride!=16U||
       bytes!=7U*6U*16U)return -1;
    memset(counts,0,7U);
    memset(dst,0,bytes);
    for(stage=0;stage<7U;stage++) {
        n=0U;
        for(i=3U;i<=9U;i++) {
            float xyz[3],tick;
            uint16_t event_tick;
            if(tntv_rom_read(rom,original_stage_rom_offsets[stage]+i*4U,
                             word,4U)!=TNTV_ROM_OK)return -2;
            ptr=read_be32(word);
            if(!ptr)break;
            if(n>=6U||ptr<delta||ptr>0x81000000U)return -3;
            off=ptr-delta;
            if(off>rom->length||48U>rom->length-off)return -4;
            if(tntv_rom_read(rom,off,record,48U)!=TNTV_ROM_OK)return -5;
            for(j=0;j<3U;j++) {
                uint32_t bits=read_be32(record+8U+j*4U);
                memcpy(&xyz[j],&bits,4U);
                if(!isfinite((double)xyz[j]))return -6;
            }
            {
                uint32_t bits=read_be32(record+36U);
                memcpy(&tick,&bits,4U);
            }
            if(!isfinite((double)tick)||tick<0.0f||tick>65535.0f||
               (float)(uint16_t)tick!=tick)return -7;
            event_tick=(uint16_t)tick;
            memcpy(dst+(stage*6U+n)*16U,xyz,12U);
            memcpy(dst+(stage*6U+n)*16U+12U,&event_tick,2U);
            n++;
        }
        if(i>9U||!n)return -8;
        counts[stage]=(uint8_t)n;
    }
    return 0;
}

/* Private original N64 intro scene985 basic initial scene state.
 * kind0: 71 native TNTIntroNodeDef, each flags16/parent8/type8/9 float32.
 * kind1: six fog floats; kind2: four camera floats; kind3: three lights.
 * All bytes originate only from the user's verified original USA ROM.
 * Every scene offset and nested FX resource is strictly bounds checked. */
int tntv_fx_extract_intro_basics(TNTVRom *rom,uint32_t kind,
                                  uint8_t **out,uint32_t *outlen) {
    uint8_t *scene=NULL,*resource=NULL,*result=NULL;
    uint32_t sn=0,rn=0,no,nn,nlights,light_table,camid,foff,i,j,total=0;
    int rc=-1;
    if(!rom||!out||!outlen||kind>3U)return -1;
    *out=NULL;*outlen=0;
    if(fx_read(rom,985U,TNTV_FX_TYPE_SCENE,&scene,&sn)!=0)return -2;
    if(sn<60U)goto done;
    nn=read_be32(scene);
    nlights=read_be32(scene+4U);
    no=read_be32(scene+24U);
    foff=read_be32(scene+32U);
    camid=read_be32(scene+36U);
    light_table=read_be32(scene+40U);
    if(nn!=71U||nlights!=3U||!scene_range(sn,no,nn,64U)||
       !scene_range(sn,foff,6U,4U)||
       !scene_range(sn,light_table,nlights,4U))goto done;
    total=kind==0U?71U*40U:kind==1U?6U*4U:kind==2U?4U*4U:3U*4U*4U;
    result=(uint8_t*)calloc(total,1U);
    if(!result){rc=-3;goto done;}
    if(kind==0U) {
        for(i=0;i<nn;i++) {
            uint8_t *dst=result+i*40U;
            const uint8_t *node=scene+no+i*64U;
            uint16_t flags=read_be16(node);
            memcpy(dst,&flags,2U);
            dst[2]=255U; /* signed parent -1 until child graph resolves */
            dst[3]=node[2];
            for(j=0;j<9U;j++)
                scene_float(dst+4U+4U*j,node+16U+4U*j,1);
        }
        for(i=0;i<nn;i++) {
            const uint8_t *node=scene+no+i*64U;
            uint32_t count=node[3],child_off=read_be32(node+4U);
            if(!scene_range(sn,child_off,count,4U))goto done;
            for(j=0;j<count;j++) {
                uint32_t child=read_be32(scene+child_off+4U*j);
                if(child>=nn||result[child*40U+2U]!=255U)goto done;
                result[child*40U+2U]=(uint8_t)i;
            }
        }
    }else if(kind==1U) {
        for(i=0;i<6U;i++)scene_float(result+4U*i,scene+foff+4U*i,1);
    }else if(kind==2U) {
        static const uint32_t camera_float_offsets[4]={4U,8U,12U,20U};
        if(fx_read(rom,camid,3U,&resource,&rn)!=0)goto done;
        if(rn<24U)goto done;
        for(i=0;i<4U;i++)scene_float(result+4U*i,resource+camera_float_offsets[i],1);
    }else {
        for(i=0;i<3U;i++) {
            uint32_t rid=read_be32(scene+light_table+4U*i);
            if(fx_read(rom,rid,4U,&resource,&rn)!=0)goto done;
            if(rn<16U)goto done;
            for(j=0;j<4U;j++)scene_float(result+i*16U+j*4U,resource+j*4U,1);
            free(resource);resource=NULL;
        }
    }
    *out=result;*outlen=total;result=NULL;rc=0;
done:
    free(scene);free(resource);free(result);
    return rc;
}

/* Original FX985 37 RGBA16 source textures, levels and tile0 RSP addressing.
 * For each texture original type1 resource: mip count = tex[7]+1,
 * pixel count sum (width >> L)^2; output native 16bit values.
 * Native TNTIntroTextureDef is 12 bytes: data_offset32, size16, 4x u8
 * levels, addressing U/V and flags, plus texel_count16. This function
 * contains zero original copyrighted texture payload bytes. */
int tntv_fx_extract_intro_textures(TNTVRom *rom,uint32_t kind,
                                   uint8_t **out,uint32_t *outlen) {
    uint8_t *scene=NULL,*tex=NULL,*result=NULL;
    uint32_t sn=0,tn=0,table,nt,i,j,total=0,texels=0;
    int rc=-1;
    if(!rom||!out||!outlen||kind>1U)return -1;
    *out=NULL;*outlen=0;
    if(fx_read(rom,985U,TNTV_FX_TYPE_SCENE,&scene,&sn)!=0)return -2;
    if(sn<60U)goto done;
    nt=read_be32(scene+12U);
    table=read_be32(scene+48U);
    if(nt!=37U || !scene_range(sn,table,nt,4U))goto done;
    total=kind==0U?37U*12U:39920U*2U;
    result=(uint8_t*)calloc(total,1U);
    if(!result){rc=-3;goto done;}
    for(i=0;i<nt;i++) {
        uint32_t rid=read_be32(scene+table+4U*i);
        uint32_t w,h,off,lev,source_texel_count=0,dl,pc,steps=0,limit;
        uint8_t u_addr=2U,v_addr=2U;
        if(fx_read(rom,rid,TNTV_FX_TYPE_TEXTURE,&tex,&tn)!=0)goto done;
        if(tn<16U)goto done;
        w=read_be16(tex+2U);h=read_be16(tex+4U);
        off=read_be32(tex+12U);
        dl=read_be32(tex+8U);
        lev=(uint32_t)tex[7U]+1U;
        if(!w||w>256U||w!=h||lev<1U||lev>9U)goto done;
        for(j=0;j<lev;j++) {
            uint32_t side=w>>j;
            if(!side||source_texel_count>UINT16_MAX-side*side)goto done;
            source_texel_count+=side*side;
        }
        if(!scene_range(tn,off,source_texel_count,2U))goto done;
        if(source_texel_count>39920U || texels>39920U-source_texel_count)goto done;
        pc=dl;limit=tn/8U;
        for(steps=0;steps<limit;steps++) {
            uint32_t w0,w1,op;
            if(!scene_range(tn,pc,1U,8U))goto done;
            w0=read_be32(tex+pc);w1=read_be32(tex+pc+4U);pc+=8U;
            op=w0>>24;
            if(op==0xf5U && ((w1>>24)&7U)==0U && (w1&0xffffffU)!=0U) {
                u_addr=(uint8_t)((w1>>8)&3U);
                v_addr=(uint8_t)((w1>>18)&3U);
                break;
            }
            if(op==0xdfU)break;
        }
        if(steps==limit)goto done;
        if(kind==0U) {
            uint16_t width=(uint16_t)w,count=(uint16_t)source_texel_count;
            memcpy(result+i*12U,&texels,4U);
            memcpy(result+i*12U+4U,&width,2U);
            result[i*12U+6U]=(uint8_t)lev;
            result[i*12U+7U]=u_addr;
            result[i*12U+8U]=v_addr;
            result[i*12U+9U]=tex[6U];
            memcpy(result+i*12U+10U,&count,2U);
        }else {
            for(j=0;j<source_texel_count;j++) {
                uint16_t value=read_be16(tex+off+j*2U);
                memcpy(result+(texels+j)*2U,&value,2U);
            }
        }
        texels+=source_texel_count;
        free(tex);tex=NULL;
    }
    if(texels!=39920U)goto done;
    *out=result;*outlen=total;result=NULL;rc=0;
done:
    free(scene);free(tex);free(result);
    return rc;
}

/* Exact, bounded source-only replay of original FX985 intro N64 type5 scene
 * and type2 mesh G_VTX/G_MODIFYVTX/G_TRI1 display-list commands.
 * No original vertex coordinates, triangle ordering, per-material properties,
 * or texture data stored in source. All come from the user-verified ROM.
 * Mode0 exports 4548 x 14B TNTIntroVertex.
 * Mode1 exports 86 x 28B TNTIntroDrawDef. Replaying predecessor material
 * display lists is essential: G_MODIFYVTX can inherit cached vertices. */
int tntv_fx_extract_intro_geometry(TNTVRom *rom,uint32_t kind,
                                   uint8_t **out,uint32_t *outlen) {
    struct IntroRspVertex {
        int16_t v[5];
        uint8_t rgba[4],valid,modified;
    };
    uint8_t *scene=NULL,*mesh=NULL,*result=NULL;
    uint32_t sn=0,mn=0,nn,nmesh,ntex,ncmd,mo,co;
    uint32_t ci,curmesh=0,curnode=0,curtex=255U,vertex_count=0,draw_count=0,total=0;
    int rc=-1;
    if(!rom||!out||!outlen||kind>1U)return -1;
    *out=NULL;*outlen=0;
    if(fx_read(rom,985U,TNTV_FX_TYPE_SCENE,&scene,&sn)!=0)return -2;
    if(sn<60U)goto done;
    nn=read_be32(scene);nmesh=read_be32(scene+8U);
    ntex=read_be32(scene+12U);
    ncmd=read_be32(scene+20U);
    mo=read_be32(scene+44U);co=read_be32(scene+56U);
    if(nn!=71U || nmesh!=45U || ntex!=37U ||
       !scene_range(sn,mo,nmesh,4U) || !scene_range(sn,co,ncmd,2U))goto done;
    total=kind==0U?4548U*14U:86U*28U;
    result=(uint8_t*)calloc(total,1U);
    if(!result){rc=-3;goto done;}
    for(ci=0;ci<ncmd;ci++) {
        uint32_t command=read_be16(scene+co+2U*ci);
        uint32_t op=command>>8U,v=command&255U;
        if(op==1U) {
            if(v>=ntex && v!=255U)goto done;
            curtex=v;
        }else if(op==2U) {
            if(v>=nn)goto done;
            curnode=v;
        }else if(op==4U) {
            if(v>=nmesh)goto done;
            curmesh=v;
        }else if(op==5U) {
            struct IntroRspVertex cache[32];
            uint32_t mesh_id=read_be32(scene+mo+4U*curmesh),mc,dlo,vbo,mato;
            uint32_t mat,first=vertex_count,mf;
            uint8_t *draw;
            memset(cache,0,sizeof(cache));
            if(draw_count>=86U)goto done;
            if(fx_read(rom,mesh_id,2U,&mesh,&mn)!=0)goto done;
            if(mn<32U)goto done;
            mc=read_be32(mesh+4U);
            dlo=read_be32(mesh+12U);
            vbo=read_be32(mesh+16U);
            mato=read_be32(mesh+28U);
            if(!mc||v>=mc||!scene_range(mn,dlo,mc,4U)||
               !scene_range(mn,mato,mc,20U))goto done;
            mf=read_be32(mesh+mato+v*20U);
            draw=result+28U*draw_count;
            if(kind==1U) {
                uint16_t first16=(uint16_t)first;
                memcpy(draw,&first16,2U);
                draw[4U]=(uint8_t)curnode;
                draw[5U]=(uint8_t)curtex;
                draw[6U]=(uint8_t)mf;draw[7U]=0U;
                scene_float(draw+8U,mesh+8U,1);
                scene_float(draw+12U,mesh+mato+20U*v+4U,1);
                scene_float(draw+16U,mesh+mato+20U*v+8U,1);
                scene_float(draw+20U,mesh+mato+20U*v+12U,1);
                scene_float(draw+24U,mesh+mato+20U*v+16U,1);
            }
            for(mat=0;mat<=v;mat++) {
                uint32_t pc=read_be32(mesh+dlo+mat*4U),steps=0;
                int ended=0;
                while(!ended) {
                    uint32_t w0,w1,cmd;
                    if(!scene_range(mn,pc,1U,8U)||++steps>mn/8U)goto done;
                    w0=read_be32(mesh+pc);w1=read_be32(mesh+pc+4U);pc+=8U;
                    cmd=w0>>24U;
                    if(cmd==1U) {
                        uint32_t n=(w0>>12)&255U,end=(w0>>1)&127U,off=w1&0xffffffU,j,k;
                        if(!n||end>32U||n>end||vbo>mn||off>mn-vbo||
                           !scene_range(mn,vbo+off,n,16U))goto done;
                        for(j=0;j<n;j++) {
                            struct IntroRspVertex *slot=&cache[end-n+j];
                            const uint8_t *rv=mesh+vbo+off+16U*j;
                            for(k=0;k<3U;k++)slot->v[k]=(int16_t)read_be16(rv+2U*k);
                            slot->v[3]=(int16_t)read_be16(rv+8U);
                            slot->v[4]=(int16_t)read_be16(rv+10U);
                            memcpy(slot->rgba,rv+12U,4U);
                            slot->valid=1U;slot->modified=0U;
                        }
                    }else if(cmd==2U) {
                        uint32_t idx=(w0&0xffffU)/2U;
                        if((w0>>16U&255U)!=0x14U || (w0&1U) ||
                           idx>=32U || !cache[idx].valid)goto done;
                        cache[idx].v[3]=(int16_t)(w1>>16U);
                        cache[idx].v[4]=(int16_t)w1;
                        cache[idx].modified=1U;
                    }else if(cmd==5U) {
                        uint32_t j;
                        for(j=0;j<3U;j++) {
                            uint32_t encoded=(w0>>(16U-8U*j))&255U,idx=encoded/2U,k;
                            struct IntroRspVertex *rv;
                            if(encoded&1U||idx>=32U||!cache[idx].valid)goto done;
                            if(mat!=v)continue;
                            if(vertex_count>=4548U)goto done;
                            rv=&cache[idx];
                            if(kind==0U) {
                                uint8_t *out_v=result+vertex_count*14U;
                                for(k=0;k<5U;k++) {
                                    int val=(int)rv->v[k];
                                    uint16_t bits;
                                    if(k>=3U && rv->modified)val*=2;
                                    if(val<INT16_MIN || val>INT16_MAX)goto done;
                                    bits=(uint16_t)val;
                                    memcpy(out_v+k*2U,&bits,2U);
                                }
                                memcpy(out_v+10U,rv->rgba,4U);
                            }
                            vertex_count++;
                        }
                    }else if(cmd==0xdfU) {
                        ended=1;
                    }else goto done;
                }
            }
            if(vertex_count-first>UINT16_MAX)goto done;
            if(kind==1U) {
                uint16_t count=(uint16_t)(vertex_count-first);
                memcpy(draw+2U,&count,2U);
            }
            draw_count++;
            free(mesh);mesh=NULL;
        }else goto done;
    }
    if(vertex_count!=4548U||draw_count!=86U)goto done;
    *out=result;*outlen=total;result=NULL;rc=0;
done:
    free(scene);free(mesh);free(result);
    return rc;
}

/* Original USA NTSC intro FX type6 animation986 references FX985 scene
 * 60 targets, 187 separate tracks and 6,722 sparse-mask polynomial keys.
 * Strictly reconstruct native TNTIntroKey[20B], TNTIntroTrackDef[8B],
 * TNTIntroTargetDef[8B] with original source order. Never embeds original
 * N64 animation coefficients/target identities/vertex payload bytes. */
int tntv_fx_extract_intro_animation(TNTVRom *rom,uint32_t kind,
                                    uint8_t **out,uint32_t *outlen) {
    uint8_t *anim=NULL,*scene=NULL,*result=NULL;
    uint32_t an=0,sn=0,acnt,alist,meshes,nmesh,commands,ncmd;
    uint32_t ti,ntrack=0,nkey=0,total,rc=(uint32_t)-1;
    if(!rom||!out||!outlen||kind>2U)return -1;
    *out=NULL;*outlen=0;
    if(fx_read(rom,986U,TNTV_FX_TYPE_ANIM,&anim,&an)!=0)return -2;
    if(fx_read(rom,985U,TNTV_FX_TYPE_SCENE,&scene,&sn)!=0)goto done;
    if(an<20U || sn<60U || read_be32(anim)!=985U)goto done;
    acnt=read_be32(anim+4U);
    alist=read_be32(anim+8U);
    if(acnt!=60U || !scene_range(an,alist,acnt,36U))goto done;
    nmesh=read_be32(scene+8U);
    meshes=read_be32(scene+44U);
    ncmd=read_be32(scene+20U);
    commands=read_be32(scene+56U);
    if(nmesh!=45U || !scene_range(sn,meshes,nmesh,4U) ||
       !scene_range(sn,commands,ncmd,2U))goto done;
    total=kind==0U?6722U*20U:kind==1U?187U*8U:60U*8U;
    result=(uint8_t*)calloc(total,1U);
    if(!result){rc=(uint32_t)-3;goto done;}
    for(ti=0;ti<acnt;ti++) {
        uint32_t e=alist+ti*36U;
        uint32_t asset=read_be32(anim+e),type=read_be32(anim+e+12U);
        uint32_t target=read_be32(anim+e+16U),bits=read_be32(anim+e+20U);
        uint32_t tc=read_be32(anim+e+24U),tro=read_be32(anim+e+28U);
        uint32_t firsttrack=ntrack,j;
        if(tc>187U || tc>UINT8_MAX ||
           ntrack>187U-tc || !scene_range(an,tro,tc,20U))goto done;
        if(kind==2U) {
            uint32_t classification=255U,index=255U;
            uint8_t *dst=result+ti*8U;
            if(type==1U && target<71U) {
                classification=0U;index=target;
            }else if(type==0U && asset==985U && target==68U) {
                classification=1U;index=0U;
            }else if(type==0U && asset==981U && target==4U) {
                classification=2U;index=0U;
            }else if(type==0U && asset==1406U && target==4U) {
                classification=3U;index=1U;
            }else if(type==2U) {
                uint32_t c,curmesh=0U,di=0U;
                for(c=0;c<ncmd;c++) {
                    uint32_t code=read_be16(scene+commands+2U*c);
                    uint32_t op=code>>8U,value=code&255U;
                    if(op==4U) {
                        if(value>=nmesh)goto done;
                        curmesh=value;
                    }else if(op==5U) {
                        if(read_be32(scene+meshes+4U*curmesh)==asset && value==target) {
                            classification=4U;index=di;
                        }
                        di++;
                    }
                }
                if(di!=86U)goto done;
            }
            if(classification>4U || index>UINT8_MAX ||
               bits>UINT16_MAX || firsttrack>UINT16_MAX)goto done;
            dst[0]=(uint8_t)classification;dst[1]=(uint8_t)index;
            {
                uint16_t mask=(uint16_t)bits,ft=(uint16_t)firsttrack;
                memcpy(dst+2U,&mask,2U);
                memcpy(dst+4U,&ft,2U);
            }
            dst[6U]=(uint8_t)tc;dst[7U]=0U;
        }
        for(j=0;j<tc;j++) {
            uint32_t tr=tro+j*20U,mode=read_be32(anim+tr);
            uint32_t end=read_be16(anim+tr+4U),kpo=read_be32(anim+tr+12U);
            uint32_t kstart=nkey,loop=0;
            int found_end=0;
            if(mode!=0U || end>4096U)goto done;
            while(!found_end) {
                uint32_t tick,mask,co,k,c=0;
                if(nkey>=6722U || loop++>6722U || !scene_range(an,kpo,1U,8U))goto done;
                tick=read_be16(anim+kpo);
                mask=read_be16(anim+kpo+2U);
                co=read_be32(anim+kpo+4U);
                if(tick>4096U || mask>15U)goto done;
                if(!scene_range(an,co,(uint32_t)__builtin_popcount(mask),4U))goto done;
                if(kind==0U) {
                    uint8_t *dst=result+nkey*20U;
                    uint16_t t16=(uint16_t)tick,m16=(uint16_t)mask;
                    memcpy(dst,&t16,2U);memcpy(dst+2U,&m16,2U);
                    for(k=0;k<4U;k++) {
                        if(mask&(1U<<k)) {
                            scene_float(dst+4U+4U*k,anim+co+4U*c,1);
                            c++;
                        }
                    }
                }
                if(tick==end)found_end=1;
                nkey++;
                kpo+=8U;
            }
            if(kind==1U) {
                uint8_t *dst=result+ntrack*8U;
                uint32_t first=kstart;
                uint16_t count=(uint16_t)(nkey-kstart),last=(uint16_t)end;
                if(nkey-kstart>UINT16_MAX)goto done;
                memcpy(dst,&first,4U);
                memcpy(dst+4U,&count,2U);
                memcpy(dst+6U,&last,2U);
            }
            ntrack++;
        }
    }
    if(ntrack!=187U || nkey!=6722U)goto done;
    *out=result;*outlen=total;result=NULL;rc=0;
done:
    free(anim);free(scene);free(result);
    return (int)rc;
}
