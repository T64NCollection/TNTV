#include "tntv_cache_reader.h"
#include <limits.h>
#include <string.h>

static uint32_t read_le32(const uint8_t *p) {
    return (uint32_t)p[0] | (uint32_t)p[1]<<8 |
           (uint32_t)p[2]<<16 | (uint32_t)p[3]<<24;
}
/* IEEE CRC32, same polynomial/output as the original bit-at-a-time
 * decoder. Table initialization is done once on the single startup thread.
 * The old loop performed eight branch/shift rounds per input byte, twice
 * during every warm boot's duplicate validation/binding passes. */
static uint32_t crc32_bytes(const uint8_t *p,size_t n) {
    static uint32_t table[256];
    static int initialized=0;
    uint32_t crc=~(uint32_t)0;
    size_t i;
    if(!initialized) {
        unsigned x,j;
        for(x=0;x<256;x++) {
            uint32_t c=x;
            for(j=0;j<8;j++)
                c=(c>>1)^(0xedb88320U & -(c&1U));
            table[x]=c;
        }
        initialized=1;
    }
    for(i=0;i<n;i++)
        crc=table[(crc^p[i])&255U]^(crc>>8);
    return ~crc;
}

void tntv_cache_close(TNTVCache *c) {
    if (!c) return;
    if (c->file) fclose(c->file);
    memset(c,0,sizeof(*c));
}
int tntv_cache_open(TNTVCache *c, const char *path, const uint8_t md5[16]) {
    uint8_t header[40], rec[80];
    uint32_t n,base,begin,i,last;
    long flen;
    if(!c || !path || !md5) return -1;
    memset(c,0,sizeof(*c));
    c->file=fopen(path,"rb");
    if(!c->file) return -2;
    if(fseek(c->file,0,SEEK_END)!=0) goto fail;
    flen=ftell(c->file);
    if(flen<40 || (unsigned long)flen>UINT32_MAX) goto fail;
    c->file_size=(uint32_t)flen;
    if(fseek(c->file,0,SEEK_SET)!=0 || fread(header,1,40,c->file)!=40)goto fail;
    if(memcmp(header,"TNTVPAK1",8)!=0 || read_le32(header+8)!=1U ||
       memcmp(header+24,md5,16)!=0) goto fail;
    n=read_le32(header+12);
    base=read_le32(header+16);
    begin=read_le32(header+20);
    if(n==0 || n>TNTV_CACHE_MAX_ENTRIES || base!=40 ||
       (uint64_t)base+(uint64_t)n*80!=(uint64_t)begin ||
       begin>c->file_size) goto fail;
    c->count=n;
    last=begin;
    for(i=0;i<n;i++) {
        TNTVCacheEntry *e=&c->entries[i];
        uint32_t j;
        if(fread(rec,1,80,c->file)!=80)goto fail;
        if(!memchr(rec,0,64))goto fail;
        memcpy(e->name,rec,64);
        e->offset=read_le32(rec+64);
        e->length=read_le32(rec+68);
        e->crc32=read_le32(rec+72);
        if(read_le32(rec+76)!=0 ||
           e->offset<last ||
           (uint64_t)e->offset+(uint64_t)e->length>c->file_size) goto fail;
        last=e->offset+e->length;
        for(j=0;j<i;j++)
            if(strcmp(e->name,c->entries[j].name)==0)goto fail;
    }
    return 0;
fail:
    tntv_cache_close(c);
    return -3;
}
const TNTVCacheEntry *tntv_cache_find(const TNTVCache *c,const char *name) {
    uint32_t i;
    if(!c || !c->file || !name) return NULL;
    for(i=0;i<c->count;i++)
        if(strcmp(c->entries[i].name,name)==0)return &c->entries[i];
    return NULL;
}
int tntv_cache_read(const TNTVCache *c, const char *name,
                    void *dst,size_t cap) {
    const TNTVCacheEntry *e=tntv_cache_find(c,name);
    if(!e || !dst || cap<e->length || e->offset>0x7fffffffU)return -1;
    if(fseek(c->file,(long)e->offset,SEEK_SET)!=0)return -2;
    if(fread(dst,1,e->length,c->file)!=e->length)return -3;
    if(crc32_bytes((const uint8_t*)dst,e->length)!=e->crc32)return -4;
    return 0;
}
