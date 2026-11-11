/* Offline + Vita-compatible original-ROM asset pack generator (partial).
 * Depends on our tntv_rom_reader and on permissively licensed SHA256/lzokay.
 * The generated archive is COPYRIGHTED GAME DATA (never publicly distribute).
 *
 * This builds the current 241 parity-verified native asset arrays. It is not
 * complete enough to play TNTV. Test125A source/gameplay remains separate.
 */
#include "tntv_cache_builder.h"
#include "tntv_rom_reader.h"
#include "tntv_fx_reader.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TNTV_CACHE_MAGIC "TNTVPAK1"
#define TNTV_SPEC_MAX_IDS 12U
#define TNTV_CACHE_MAGIC_HEADER_SIZE 40U
#define TNTV_CACHE_RECORD_SIZE 80U
#define TNTV_MAX_PACK_BYTES (32U*1024U*1024U)

typedef enum {
    TNTV_ASSET_CI8=1, TNTV_ASSET_PAL=2, TNTV_ASSET_PACK=3,
    TNTV_ASSET_IA8=4, TNTV_ASSET_RGBA16=5, TNTV_ASSET_RAW=6,
    TNTV_ASSET_DCM_STREAM=7, TNTV_ASSET_FX_ANIM=8, TNTV_ASSET_FX_WONDER_TEXELS=9
} TNTVAssetType;
typedef struct {
    const char *name;
    uint32_t dir;
    uint32_t id_count;
    uint32_t ids[TNTV_SPEC_MAX_IDS];
    uint32_t w;
    uint32_t h;
    TNTVAssetType type;
} TNTVAssetSpec;
#include "asset_specs_v6.inc"

typedef struct {
    const TNTVAssetSpec *spec;
    uint8_t *data;
    uint32_t length;
} TNTVProducedAsset;
#define TNTV_ASSET_COUNT (sizeof(g_specs)/sizeof(g_specs[0]))

static void put_le32(uint8_t *p,uint32_t value) {
    p[0]=(uint8_t)value;
    p[1]=(uint8_t)(value>>8);
    p[2]=(uint8_t)(value>>16);
    p[3]=(uint8_t)(value>>24);
}
static uint16_t be16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0]<<8 | p[1]);
}
/* Match the reader's IEEE CRC32 with a one-time table on the single
 * startup thread: cold builds used to run eight bit shifts per asset byte. */
static uint32_t crc32_bytes(const uint8_t *p,uint32_t n) {
    static uint32_t table[256];
    static int ready=0;
    uint32_t c=~(uint32_t)0,i;
    if(!ready) {
        unsigned x,j;
        for(x=0;x<256;x++) {
            uint32_t v=x;
            for(j=0;j<8;j++)v=(v>>1)^(0xedb88320U & -(v&1U));
            table[x]=v;
        }
        ready=1;
    }
    for(i=0;i<n;i++)c=table[(c^p[i])&255U]^(c>>8);
    return ~c;
}
static int write_exact(FILE *f,const void *ptr,size_t len) {
    return fwrite(ptr,1,len,f)==len?0:-1;
}

/* Exact original image PackBits run/literal threshold and segment boundaries.
 * This matches the previous host-side image encoder, including runs >=3 and
 * the 128-byte maximum; byte-for-byte identical output is tested. */
static uint8_t *encode_packbits(const uint8_t *input,
                                uint32_t n,uint32_t *encoded_len) {
    uint8_t *out=NULL;
    uint32_t i=0,j,k=0,cap;
    if(!input || n==0 || n>1U<<23 || n>UINT32_MAX/2-8)return NULL;
    cap=n*2+8;
    out=(uint8_t*)malloc(cap);
    if(!out)return NULL;
    while(i<n) {
        j=i+1;
        while(j<n && input[j]==input[i] && j-i<128)j++;
        if(j-i>=3){
            out[k++]=(uint8_t)(257-(j-i));
            out[k++]=input[i];
            i=j;
            continue;
        }
        {
            const uint32_t start=i;
            i++;
            while(i<n && i-start<128) {
                j=i+1;
                while(j<n && input[j]==input[i] && j-i<128)j++;
                if(j-i>=3)break;
                i++;
            }
            out[k++]=(uint8_t)(i-start-1);
            memcpy(out+k,input+start,i-start);
            k+=i-start;
        }
    }
    if(k>cap){free(out);return NULL;}
    *encoded_len=k;
    return out;
}

static int make_one(TNTVRom *rom,const TNTVAssetSpec *spec,
                    TNTVProducedAsset *asset) {
    uint8_t *pieces[TNTV_SPEC_MAX_IDS]={0};
    uint32_t lengths[TNTV_SPEC_MAX_IDS]={0};
    uint8_t *result=NULL;
    uint32_t all=0, i, target;
    if(!spec || !asset || spec->id_count==0 || spec->id_count>TNTV_SPEC_MAX_IDS)
        return -1;
    for(i=0;i<spec->id_count;i++){
        uint8_t *resource=NULL;
        uint32_t resource_size=0;
        int rc;
        if(spec->type==TNTV_ASSET_FX_ANIM) {
            /* Original FX type6 keyframes use fixed 16B archive records. */
            rc=tntv_fx_extract_animation(rom,spec->ids[i],
                                         &resource,&resource_size);
        } else if(spec->type==TNTV_ASSET_FX_WONDER_TEXELS) {
            /* Original FX type5 scene -> ordered type1 RGBA16 texture pixels.
             * Output remains BE16; game bridge restores native u16. */
            rc=tntv_fx_extract_wonder_texels(rom,spec->ids[i],
                                             &resource,&resource_size);
        } else {
            rc=tntv_rom_extract(rom,spec->dir,spec->ids[i],
                                &resource,&resource_size);
        }
        if(rc!=0){goto fail;}
        if(spec->type==TNTV_ASSET_PAL || spec->type==TNTV_ASSET_RAW ||
           spec->type==TNTV_ASSET_FX_ANIM ||
           spec->type==TNTV_ASSET_FX_WONDER_TEXELS) {
            if(spec->type==TNTV_ASSET_PAL && resource_size!=512){free(resource);goto fail;}
            pieces[i]=resource;
            lengths[i]=resource_size;
        } else if(spec->type==TNTV_ASSET_DCM_STREAM) {
            uint32_t n,off,length;
            if(resource_size<14 || memcmp(resource,"DCM1",4)!=0 ||
               resource[4]!=2 || resource[5]>8) {
                free(resource);goto fail;
            }
            n=resource[5];
            off=14U+n*16U;
            length=(uint32_t)resource[6] |
                   (uint32_t)resource[7]<<8 |
                   (uint32_t)resource[8]<<16 |
                   (uint32_t)resource[9]<<24;
            if(off>resource_size || length>resource_size-off ||
               off+length!=resource_size || length==0) {
                free(resource);goto fail;
            }
            pieces[i]=(uint8_t*)malloc(length);
            if(!pieces[i]){free(resource);goto fail;}
            memcpy(pieces[i],resource+off,length);
            lengths[i]=length;
            free(resource);
        } else {
            uint16_t rw,rh,fmt;
            uint32_t actual_h,byte_depth,expected_format;
            if(resource_size<8){free(resource);goto fail;}
            rw=be16(resource);rh=be16(resource+2);fmt=be16(resource+4);
            actual_h=spec->h?spec->h:rh;
            expected_format=(spec->type==TNTV_ASSET_IA8)?2U:
                            (spec->type==TNTV_ASSET_RGBA16)?0U:3U;
            byte_depth=(spec->type==TNTV_ASSET_RGBA16)?2U:1U;
            if(fmt!=expected_format || rw!=spec->w || rh<actual_h ||
               /* Only the Egyptian original 400x301 image is clipped. */
               (rh!=actual_h &&
                !(spec->dir==TNTV_ROM_IMG_DIR && spec->ids[i]==15 &&
                  rh==301 && actual_h==300)) ||
               actual_h>4096U ||
               (uint64_t)rw*actual_h*byte_depth>resource_size-8) {
                free(resource);goto fail;
            }
            target=rw*actual_h*byte_depth;
            pieces[i]=(uint8_t*)malloc(target);
            if(!pieces[i]){free(resource);goto fail;}
            memcpy(pieces[i],resource+8,target);
            lengths[i]=target;
            free(resource);
        }
        if(lengths[i]>TNTV_MAX_PACK_BYTES-all)goto fail;
        all+=lengths[i];
    }
    result=(uint8_t*)malloc(all);
    if(!result)goto fail;
    {
        uint32_t pos=0;
        for(i=0;i<spec->id_count;i++){
            memcpy(result+pos,pieces[i],lengths[i]);
            pos+=lengths[i];
        }
    }
    if(spec->type==TNTV_ASSET_PACK){
        uint32_t packed_len=0;
        uint8_t *packed=encode_packbits(result,all,&packed_len);
        if(!packed){goto fail;}
        free(result);
        result=packed;
        all=packed_len;
    }
    for(i=0;i<spec->id_count;i++)free(pieces[i]);
    asset->spec=spec;
    asset->data=result;
    asset->length=all;
    return 0;
fail:
    for(i=0;i<spec->id_count;i++)free(pieces[i]);
    free(result);
    return -2;
}

static int write_cache_atomic(const char *path,
                              const TNTVProducedAsset *assets,size_t count) {
    FILE *f=NULL;
    char *temp=NULL;
    uint8_t header[TNTV_CACHE_MAGIC_HEADER_SIZE]={0};
    uint8_t rec[TNTV_CACHE_RECORD_SIZE]={0};
    uint32_t dataoffset,offset,i;
    size_t len;
    int status=-1;
    if(!path || !assets || count==0 || count>512)return -1;
    if(count>(UINT32_MAX-40U)/80U)return -1;
    len=strlen(path);
    if(len>1000)return -1;
    temp=(char*)malloc(len+5);
    if(!temp)return -1;
    memcpy(temp,path,len);
    memcpy(temp+len,".tmp",5);
    /* No silent overwrites of user-owned game caches. */
    f=fopen(path,"rb");
    if(f){fclose(f);f=NULL;goto finish;}
    f=fopen(temp,"wb");
    if(!f)goto finish;

    dataoffset=40U+(uint32_t)count*80U;
    memcpy(header,TNTV_CACHE_MAGIC,8);
    put_le32(header+8,1);
    put_le32(header+12,(uint32_t)count);
    put_le32(header+16,40);
    put_le32(header+20,dataoffset);
    memcpy(header+24,tntv_rom_expected_md5(),16);
    if(write_exact(f,header,sizeof(header))!=0)goto finish;
    offset=dataoffset;
    for(i=0;i<(uint32_t)count;i++){
        size_t n;
        const TNTVProducedAsset *a=&assets[i];
        memset(rec,0,sizeof(rec));
        n=strlen(a->spec->name);
        if(n>=64 || a->length>TNTV_MAX_PACK_BYTES || !a->data)goto finish;
        memcpy(rec,a->spec->name,n);
        put_le32(rec+64,offset);
        put_le32(rec+68,a->length);
        put_le32(rec+72,crc32_bytes(a->data,a->length));
        if(write_exact(f,rec,sizeof(rec))!=0)goto finish;
        if(UINT32_MAX-offset<a->length+3U)goto finish;
        offset+=(a->length+3U)&~3U;
    }
    for(i=0;i<(uint32_t)count;i++){
        const TNTVProducedAsset *a=&assets[i];
        static const uint8_t zeros[4]={0,0,0,0};
        size_t pad=(4U-a->length%4U)%4U;
        if(write_exact(f,a->data,a->length)!=0)goto finish;
        if(pad && write_exact(f,zeros,pad)!=0)goto finish;
    }
    if(fflush(f)!=0 || ferror(f))goto finish;
    if(fclose(f)!=0){f=NULL;goto finish;}
    f=NULL;
    /* The cache path's directory must already exist. */
    if(rename(temp,path)!=0)goto finish;
    status=0;
finish:
    if(f)fclose(f);
    if(status!=0 && temp)remove(temp);
    free(temp);
    return status;
}

int tntv_cache_build_from_verified_rom(TNTVRom *verified_rom, const char *cache_path) {
    TNTVProducedAsset *assets=NULL;
    size_t count=TNTV_ASSET_COUNT,i;
    int status=-1;
    if(!verified_rom || !verified_rom->file || !cache_path)return -2;
    if(count!=278)return -1; /* lock private proof manifest version */
    assets=(TNTVProducedAsset*)calloc(count,sizeof(*assets));
    if(!assets)goto done;
    for(i=0;i<count;i++){
        if(i && strcmp(g_specs[i-1].name,g_specs[i].name)>=0)goto done;
        if(make_one(verified_rom,&g_specs[i],&assets[i])!=0)goto done;
    }
    if(write_cache_atomic(cache_path,assets,count)!=0)goto done;
    status=0;
done:
    if(assets){
        for(i=0;i<count;i++)free(assets[i].data);
    }
    free(assets);
    return status;
}

/* Preserve standalone utility API, which still validates its own ROM. */
int tntv_cache_build_from_rom(const char *rom_path, const char *cache_path) {
    TNTVRom rom;
    int result;
    if(tntv_rom_open(&rom,rom_path)!=0)return -2;
    result=tntv_cache_build_from_verified_rom(&rom,cache_path);
    tntv_rom_close(&rom);
    return result;
}
