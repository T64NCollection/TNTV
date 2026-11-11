#include "tntv_rom_reader.h"
#include "third_party/crypto-algorithms/sha256.h"
#include "third_party/lzokay/lzokay-c/lzokay-c.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static const uint8_t s_sha256[32]={
    0xef,0x64,0x73,0x7d,0x6b,0x7b,0x66,0x8b,
    0xc9,0xd4,0xc8,0xa0,0x34,0x9b,0x70,0xe9,
    0x34,0xbb,0x69,0x2f,0x27,0x2b,0xc8,0xd3,
    0xd4,0x83,0x12,0x99,0x80,0x0b,0xbc,0x39
};
static const uint8_t s_md5[16]={
    0x7a,0x28,0x17,0x9b,0x00,0x73,0x4c,0x9a,
    0xa0,0xf0,0x60,0x9f,0xaf,0xaa,0xfd,0x5f
};
const uint8_t *tntv_rom_expected_sha256(void) { return s_sha256; }
const uint8_t *tntv_rom_expected_md5(void) { return s_md5; }

static uint32_t be32(const uint8_t *p) {
    return (uint32_t)p[0]<<24 | (uint32_t)p[1]<<16 |
           (uint32_t)p[2]<<8 | (uint32_t)p[3];
}
static uint16_t be16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0]<<8 | p[1]);
}
void tntv_rom_close(TNTVRom *r) {
    if(!r)return;
    if(r->file)fclose(r->file);
    memset(r,0,sizeof(*r));
}

int tntv_rom_read(TNTVRom *r, uint32_t off,void *out,size_t length) {
    uint8_t *dest=(uint8_t*)out;
    size_t done=0;
    if(!r || !r->file || (!out && length))return TNTV_ROM_ERR_PARAM;
    if((uint64_t)off+length>r->length)return TNTV_ROM_ERR_BOUND;
    while(done<length) {
        const uint32_t here=off+(uint32_t)done;
        const uint32_t align=here & ~3U;
        const size_t prefix=(size_t)(here-align);
        const size_t n=length-done>8192?8192:length-done;
        const size_t readlen=(prefix+n+3)&~(size_t)3U;
        /* If the exact read extends past the end, still reject: ROM length is 4B aligned. */
        uint8_t raw[8196];
        size_t k;
        if((uint64_t)align+readlen>r->length)return TNTV_ROM_ERR_BOUND;
        if(fseek(r->file,(long)align,SEEK_SET)!=0 ||
           fread(raw,1,readlen,r->file)!=readlen)return TNTV_ROM_ERR_IO;
        if(!r->swap_mask) {
            memcpy(dest+done,raw+prefix,n);
        } else {
            for(k=0;k<n;k++)
                dest[done+k]=raw[(prefix+k)^r->swap_mask];
        }
        done+=n;
    }
    return TNTV_ROM_OK;
}

/* Initialize only file format and exact size. Both cold and warm paths
 * require the user's actual ROM file in a supported byte order. */
static int tntv_rom_open_header(TNTVRom *r,const char *path) {
    uint8_t header[4];
    long size;
    int rc=TNTV_ROM_ERR_IO;
    if(!r || !path)return TNTV_ROM_ERR_PARAM;
    memset(r,0,sizeof(*r));
    r->file=fopen(path,"rb");
    if(!r->file)return TNTV_ROM_ERR_OPEN;
    if(fseek(r->file,0,SEEK_END)!=0)goto fail;
    size=ftell(r->file);
    if(size!=(long)TNTV_ROM_SIZE){rc=TNTV_ROM_ERR_SIZE;goto fail;}
    r->length=TNTV_ROM_SIZE;
    if(fseek(r->file,0,SEEK_SET)!=0 || fread(header,1,4,r->file)!=4)goto fail;
    if(!memcmp(header,"\x80\x37\x12\x40",4))r->swap_mask=0;
    else if(!memcmp(header,"\x37\x80\x40\x12",4))r->swap_mask=1;
    else if(!memcmp(header,"\x40\x12\x37\x80",4))r->swap_mask=3;
    else {rc=TNTV_ROM_ERR_FORMAT;goto fail;}
    return TNTV_ROM_OK;
fail:
    tntv_rom_close(r);
    return rc;
}

/* Full strict revision verification is mandatory before reconstructing any
 * original-ROM-derived asset. Reuses an already opened stream when upgrading
 * from a sampled warm check so cold-start builds still hash only once. */
int tntv_rom_verify_full(TNTVRom *r) {
    uint8_t hash[32],*buf=NULL;
    SHA256_CTX sha;
    uint32_t done=0;
    int rc=TNTV_ROM_ERR_IO;
    if(!r || !r->file || r->length!=TNTV_ROM_SIZE)return TNTV_ROM_ERR_PARAM;
    if(r->fully_verified)return TNTV_ROM_OK;
    buf=(uint8_t*)malloc(65536);
    if(!buf)return TNTV_ROM_ERR_MEMORY;
    sha256_init(&sha);
    if(fseek(r->file,0,SEEK_SET)!=0)goto fail;
    while(done<TNTV_ROM_SIZE){
        uint32_t i;
        size_t n=TNTV_ROM_SIZE-done>65536U?65536U:TNTV_ROM_SIZE-done;
        if(fread(buf,1,n,r->file)!=n)goto fail;
        if(r->swap_mask){
            for(i=0;i<(uint32_t)n;i+=4){
                uint8_t a=buf[i],b=buf[i+1],c=buf[i+2],d=buf[i+3];
                if(r->swap_mask==1){
                    buf[i]=b;buf[i+1]=a;buf[i+2]=d;buf[i+3]=c;
                } else {
                    buf[i]=d;buf[i+1]=c;buf[i+2]=b;buf[i+3]=a;
                }
            }
        }
        sha256_update(&sha,buf,n);
        done+=(uint32_t)n;
    }
    sha256_final(&sha,hash);
    if(memcmp(hash,s_sha256,32)!=0){rc=TNTV_ROM_ERR_REVISION;goto fail;}
    r->fully_verified=1;
    free(buf);
    return TNTV_ROM_OK;
fail:
    free(buf);
    return rc;
}

int tntv_rom_open(TNTVRom *r,const char *path) {
    int rc=tntv_rom_open_header(r,path);
    if(rc!=TNTV_ROM_OK)return rc;
    rc=tntv_rom_verify_full(r);
    if(rc!=TNTV_ROM_OK)tntv_rom_close(r);
    return rc;
}

/* 15 distributed 4K byte samples across all 12MiB. Hash only normalized
 * bytes; .z64/.v64/.n64 produce the same fingerprint. These public SHA bytes
 * are not original game assets. This is an accidental-change detector, not
 * a cryptographic guarantee that unsampled ROM data is unchanged. */
int tntv_rom_open_sampled(TNTVRom *r,const char *path) {
    static const uint32_t offsets[15]={
        0x000000U,0x010000U,0x040000U,0x100000U,0x200000U,
        0x300000U,0x400000U,0x500000U,0x600000U,0x700000U,
        0x800000U,0x900000U,0xa00000U,0xb00000U,0xbff000U
    };
    static const uint8_t expected[32]={
        0x0f,0x01,0xc2,0xe1,0x87,0xdc,0x99,0x60,
        0xaa,0xbd,0x2b,0x8a,0x14,0x81,0x3e,0x39,
        0x79,0x23,0xf6,0x68,0x92,0x28,0x78,0x8c,
        0x7d,0xa3,0xe5,0xe0,0x0e,0xa2,0x55,0xad
    };
    SHA256_CTX sha;
    uint8_t block[4096],digest[32];
    size_t i;
    int rc=tntv_rom_open_header(r,path);
    if(rc!=TNTV_ROM_OK)return rc;
    sha256_init(&sha);
    for(i=0;i<sizeof(offsets)/sizeof(offsets[0]);i++){
        rc=tntv_rom_read(r,offsets[i],block,sizeof(block));
        if(rc!=TNTV_ROM_OK)goto fail;
        sha256_update(&sha,block,sizeof(block));
    }
    sha256_final(&sha,digest);
    if(memcmp(digest,expected,sizeof(expected))!=0){
        rc=TNTV_ROM_ERR_REVISION;
        goto fail;
    }
    r->fully_verified=0;
    return TNTV_ROM_OK;
fail:
    tntv_rom_close(r);
    return rc;
}

int tntv_rom_extract(TNTVRom *r,uint32_t base,uint32_t idx,
                     uint8_t **out,uint32_t *outlen) {
    uint8_t countbuf[2],offsets[8],header[8];
    uint32_t start,end,size,raw_size;
    uint16_t count;
    uint8_t *dst=NULL,*src=NULL;
    int rc;
    size_t got;
    if(!r || !r->file || !out || !outlen)return TNTV_ROM_ERR_PARAM;
    *out=NULL;
    *outlen=0;
    if((uint64_t)base+2>r->length)return TNTV_ROM_ERR_BOUND;
    rc=tntv_rom_read(r,base,countbuf,2);
    if(rc!=0)return rc;
    count=be16(countbuf);
    if(count<2 || idx>=(uint32_t)count-1)return TNTV_ROM_ERR_BOUND;
    if((uint64_t)base+2+(uint64_t)(idx+2)*4>r->length)return TNTV_ROM_ERR_BOUND;
    rc=tntv_rom_read(r,base+2+4*idx,offsets,8);
    if(rc!=0)return rc;
    start=be32(offsets);end=be32(offsets+4);
    if(start>=end || (uint64_t)base+end>r->length || end-start<8)
        return TNTV_ROM_ERR_BOUND;
    rc=tntv_rom_read(r,base+start,header,8);
    if(rc!=0)return rc;
    size=be32(header+4);
    raw_size=end-start-8;
    if(size==0 || size>TNTV_ROM_MAX_RESOURCE)return TNTV_ROM_ERR_BOUND;
    dst=(uint8_t*)malloc(size);
    if(!dst)return TNTV_ROM_ERR_MEMORY;
    if(!memcmp(header,"H2ON",4)){
        if(raw_size<size){rc=TNTV_ROM_ERR_BOUND;goto fail;}
        rc=tntv_rom_read(r,base+start+8,dst,size);
        if(rc!=0)goto fail;
    } else if(!memcmp(header,"H2OS",4)) {
        if(raw_size==0){rc=TNTV_ROM_ERR_BOUND;goto fail;}
        src=(uint8_t*)malloc(raw_size);
        if(!src){rc=TNTV_ROM_ERR_MEMORY;goto fail;}
        rc=tntv_rom_read(r,base+start+8,src,raw_size);
        if(rc!=0)goto fail;
        got=size;
        /* liblzo2 accepts trailing H2OS alignment padding: lzokay reports
         * InputNotConsumed=1 for the same condition. Require exact output. */
        {
            lzokay_EResult status=lzokay_decompress(src,raw_size,dst,&got);
            if((status!=EResult_Success && status!=EResult_InputNotConsumed)
               || got!=size) {
                rc=TNTV_ROM_ERR_DECODE;goto fail;
            }
        }
    } else {rc=TNTV_ROM_ERR_FORMAT;goto fail;}
    free(src);
    *out=dst;
    *outlen=size;
    return TNTV_ROM_OK;
fail:
    free(src);
    free(dst);
    return rc;
}
