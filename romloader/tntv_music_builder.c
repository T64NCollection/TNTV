/* ROM->TNTV TNMP music reconstruction for Vita. Author-written implementation.
 *
 * Original N64 audio data is sourced ONLY from the user's validated ROM.
 * Event streams are H2O DCM's dictionary-compressed data, NOT standard MIDI.
 * Metadata IDs/offsets refer to resource indices in the user's own ROM.
 *
 * Sound events, samples and resulting 10.6MB pack are not part of this file.
 * The exact output checksum identifies the previously hardware-approved
 * audio archive; it is not a substitute for ROM version/hash validation.
 */
#include "tntv_music_builder.h"
#include "tntv_rom_reader.h"
#include "third_party/crypto-algorithms/sha256.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define SONGS 16U
#define DCM_DIR 0x5B9AB0U
#define SAMPLE_DIR 0x7C1960U
#define MAX_SAMPLES 64U
#define MAX_EVENTS 4000000U
#define EXPECTED_PACK_BYTES 10589568U

static const uint8_t s_music_sha256[32]={
    0x54,0x92,0x06,0xf0,0x5f,0x7a,0xe5,0x6f,
    0xc0,0x94,0x73,0x4c,0x58,0x22,0x52,0x11,
    0x02,0xc6,0x3d,0x02,0xac,0xa0,0x0e,0x52,
    0xd8,0xe5,0x07,0x4c,0xe4,0x90,0xce,0x31
};

typedef struct {
    uint32_t length;
    uint32_t loop_begin;
    uint32_t loop_end;
    uint16_t flags;
    uint16_t sample_id;
} MusicSample;

static uint16_t le16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0]|((uint16_t)p[1]<<8));
}
static uint32_t le32(const uint8_t *p) {
    return (uint32_t)p[0]|(uint32_t)p[1]<<8|
           (uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
static uint32_t be32(const uint8_t *p) {
    return (uint32_t)p[0]<<24|(uint32_t)p[1]<<16|
           (uint32_t)p[2]<<8|p[3];
}
static void put_le32(uint8_t *p,uint32_t x){
    p[0]=(uint8_t)x;
    p[1]=(uint8_t)(x>>8);
    p[2]=(uint8_t)(x>>16);
    p[3]=(uint8_t)(x>>24);
}
static int write_all(FILE *f,const void *p,size_t len){
    return fwrite(p,1,len,f)==len?0:-1;
}
static int put_zeros(FILE *f,size_t len) {
    static const uint8_t zero[4]={0,0,0,0};
    if(len>3)return -1;
    return len ? write_all(f,zero,len) : 0;
}
static uint32_t align4(uint32_t value) {return (value+3U)&~3U;}

/* Reverse H2O DCM's 4096B dictionary mode, processing one output byte at a
 * time to support overlapping back-references. This code is independent of
 * the original game's source and has no fixed game data in it. */
static int unpack_events(const uint8_t *src,uint32_t srclen,
                         uint8_t **out,uint32_t *outlen) {
    uint8_t dict[4096];
    uint8_t *dst=NULL;
    uint32_t i=4,used=0,dpos=0,wanted,k;
    if(!src || !out || !outlen || srclen<5)return -1;
    *out=NULL;
    *outlen=0;
    wanted=be32(src);
    if(wanted==0 || wanted>MAX_EVENTS)return -2;
    dst=(uint8_t*)malloc(wanted);
    if(!dst)return -3;
    for(k=0;k<4096;k++){
        dict[k]=(uint8_t)(k<256?k:k<512?511-k:
                          k<1536?(k-512)/4:k&255U);
    }
    while(used<wanted){
        uint8_t tag;
        uint32_t count,j;
        if(i>=srclen)goto malformed;
        tag=src[i++];
        if(tag<16){
            count=(uint32_t)tag+1;
            if(count>srclen-i || count>wanted-used)goto malformed;
            for(j=0;j<count;j++){
                const uint8_t val=src[i++];
                dst[used++]=val;
                dict[dpos]=val;
                dpos=(dpos+1)&4095U;
            }
        } else {
            uint32_t from;
            if(i>=srclen)goto malformed;
            count=((uint32_t)tag>>4)+2;
            from=((uint32_t)(tag&15)<<8)|src[i++];
            if(count>wanted-used)goto malformed;
            for(j=0;j<count;j++){
                const uint8_t val=dict[from];
                from=(from+1)&4095U;
                dst[used++]=val;
                dict[dpos]=val;
                dpos=(dpos+1)&4095U;
            }
        }
    }
    if(i!=srclen)goto malformed;
    *out=dst;
    *outlen=wanted;
    return 0;
malformed:
    free(dst);
    return -4;
}

/* Entire output is hashed in streaming chunks; a truncated/altered pack
 * cannot pass the accepted SHA256 check. */
int tntv_music_validate(const char *path){
    FILE *f;
    long size;
    SHA256_CTX sha;
    uint8_t chunk[16384],digest[32];
    uint32_t done=0;
    if(!path)return -1;
    f=fopen(path,"rb");
    if(!f)return -2;
    if(fseek(f,0,SEEK_END)!=0)goto bad;
    size=ftell(f);
    if(size!=(long)EXPECTED_PACK_BYTES)goto bad;
    if(fseek(f,0,SEEK_SET)!=0)goto bad;
    sha256_init(&sha);
    while(done<EXPECTED_PACK_BYTES){
        const size_t needed=EXPECTED_PACK_BYTES-done>sizeof(chunk)?
                           sizeof(chunk):EXPECTED_PACK_BYTES-done;
        if(fread(chunk,1,needed,f)!=needed)goto bad;
        sha256_update(&sha,chunk,needed);
        done+=(uint32_t)needed;
    }
    sha256_final(&sha,digest);
    fclose(f);
    return memcmp(digest,s_music_sha256,32)==0?0:-4;
bad:
    fclose(f);
    return -3;
}

static int write_one_song(TNTVRom *rom,FILE *f,uint8_t index,
                          uint32_t *length_out) {
    uint8_t *dcm=NULL,*events=NULL,*sample_bytes=NULL;
    uint32_t dcm_len=0,eventlen=0,sample_len=0;
    uint8_t header[24]={0};
    uint8_t record[24];
    MusicSample samples[MAX_SAMPLES];
    uint32_t count,channels,streamsize,streamoff,descoff=24U,i,at;
    long blob_start,end;
    int result=-1;
    if(!rom || !f || !length_out)return -1;
    if(tntv_rom_extract(rom,DCM_DIR,index,&dcm,&dcm_len)!=0)return -2;
    if(dcm_len<14 || memcmp(dcm,"DCM1",4)!=0)goto fail;
    channels=dcm[4];
    count=dcm[5];
    if(channels!=16 || count<1 || count>MAX_SAMPLES)goto fail;
    streamsize=le32(dcm+6);
    if(streamsize<5 || 14U+16U*count>dcm_len ||
       streamsize>dcm_len-(14U+16U*count))goto fail;
    for(i=0;i<count;i++){
        const uint8_t *p=dcm+14+16*i;
        samples[i].length=le32(p);
        samples[i].loop_begin=le32(p+4);
        samples[i].loop_end=le32(p+8);
        samples[i].flags=le16(p+12);
        samples[i].sample_id=le16(p+14);
    }
    if(unpack_events(dcm+14+16*count,streamsize,&events,&eventlen)!=0)goto fail;
    blob_start=ftell(f);
    if(blob_start<0)goto fail;
    streamoff=24+24*count;
    memcpy(header,"TSNG",4);
    header[4]=index;
    header[5]=(uint8_t)channels;
    header[6]=(uint8_t)count;
    put_le32(header+8,eventlen);
    put_le32(header+12,streamoff);
    put_le32(header+16,descoff);
    /* blob_size header+20 filled in after all local sample writes */
    if(write_all(f,header,sizeof(header))!=0)goto fail;
    memset(record,0,sizeof(record));
    for(i=0;i<count;i++){
        if(write_all(f,record,sizeof(record))!=0)goto fail;
    }
    if(write_all(f,events,eventlen)!=0)goto fail;
    at=streamoff+eventlen;
    free(events);events=NULL;
    free(dcm);dcm=NULL;

    for(i=0;i<count;i++){
        MusicSample *s=&samples[i];
        uint32_t start=align4(at);
        long return_pos;
        uint32_t need=s->length*((s->flags&1U)?2U:1U);
        if(at+3U<at || start>UINT32_MAX-need)goto fail;
        if(put_zeros(f,start-at)!=0)goto fail;
        if(tntv_rom_extract(rom,SAMPLE_DIR,s->sample_id,
                            &sample_bytes,&sample_len)!=0)goto fail;
        if(sample_len!=need)goto fail;
        if(write_all(f,sample_bytes,sample_len)!=0)goto fail;
        free(sample_bytes);sample_bytes=NULL;
        at=start+sample_len;
        return_pos=ftell(f);
        if(return_pos<0)goto fail;
        put_le32(record+0,s->length);
        put_le32(record+4,s->loop_begin);
        put_le32(record+8,s->loop_end);
        put_le32(record+12,((uint32_t)s->sample_id<<16)|s->flags);
        put_le32(record+16,start);
        put_le32(record+20,sample_len);
        if(fseek(f,blob_start+descoff+24*i,SEEK_SET)!=0 ||
           write_all(f,record,sizeof(record))!=0 ||
           fseek(f,return_pos,SEEK_SET)!=0)goto fail;
    }
    end=ftell(f);
    if(end<0 || at>UINT32_MAX-3U)goto fail;
    if(put_zeros(f,align4(at)-at)!=0)goto fail;
    at=align4(at);
    end=ftell(f);
    if(end<0 || (uint32_t)(end-blob_start)!=at)goto fail;
    put_le32(header+20,at);
    if(fseek(f,blob_start,SEEK_SET)!=0 ||
       write_all(f,header,sizeof(header))!=0 ||
       fseek(f,end,SEEK_SET)!=0)goto fail;
    *length_out=at;
    result=0;
fail:
    free(events);
    free(dcm);
    free(sample_bytes);
    return result;
}

/* Build by song with only a few hundred KB of memory, not the whole 10MB pack.
 * On success the generated PRIVATE file is byte-identical to the approved
 * original game soundtrack pack. */
int tntv_music_build_from_verified_rom(TNTVRom *verified_rom,const char *destination){
    FILE *f=NULL;
    char *tmp=NULL;
    uint8_t header[16]={0},entries[128]={0};
    uint32_t offset=144, song,bytes;
    long len;
    int result=-1;
    if(!verified_rom || !verified_rom->file || !destination)return -1;
    len=(long)strlen(destination);
    if(len<=0 || len>1000)return -2;
    f=fopen(destination,"rb");
    if(f){fclose(f);return -3;}
    tmp=(char*)malloc((size_t)len+5);
    if(!tmp){result=-5;goto done;}
    memcpy(tmp,destination,(size_t)len);
    memcpy(tmp+len,".tmp",5);
    f=fopen(tmp,"wb");
    if(!f){result=-6;goto done;}
    memcpy(header,"TNMP",4);
    put_le32(header+4,1);
    put_le32(header+8,SONGS);
    put_le32(header+12,SONGS);
    if(write_all(f,header,16)!=0 ||
       write_all(f,entries,sizeof(entries))!=0){
        result=-7;goto done;
    }
    for(song=0;song<SONGS;song++){
        long current=ftell(f);
        if(current!=(long)offset){result=-8;goto done;}
        if(write_one_song(verified_rom,f,(uint8_t)song,&bytes)!=0){
            result=-9;goto done;
        }
        put_le32(entries+8*song,offset);
        put_le32(entries+8*song+4,bytes);
        if(bytes>UINT32_MAX-offset){result=-10;goto done;}
        offset+=bytes;
    }
    if(offset!=EXPECTED_PACK_BYTES){result=-11;goto done;}
    if(fseek(f,16,SEEK_SET)!=0 ||
       write_all(f,entries,sizeof(entries))!=0 ||
       fflush(f)!=0 || ferror(f)){result=-12;goto done;}
    if(fclose(f)!=0){f=NULL;result=-13;goto done;}
    f=NULL;
    if(tntv_music_validate(tmp)!=0){result=-14;goto done;}
    if(rename(tmp,destination)!=0){result=-15;goto done;}
    result=0;
done:
    if(f)fclose(f);
    if(result!=0 && tmp)remove(tmp);
    free(tmp);
    return result;
}

/* Standalone builder API retains full-ROM verification for independent use. */
int tntv_music_build_from_rom(const char *rom_path,const char *destination){
    TNTVRom rom;
    int result;
    if(!rom_path || tntv_rom_open(&rom,rom_path)!=0)return -4;
    result=tntv_music_build_from_verified_rom(&rom,destination);
    tntv_rom_close(&rom);
    return result;
}

/* Read sound metadata from verified player-ROM DCM descriptors.
 * IDs select resources; no original sound values or sample maps are embedded. */
int tntv_sfx_metadata(TNTVRom *rom,const uint16_t *dcm_ids,uint32_t dcm_count,
                     const uint16_t *sample_ids,uint32_t sample_count,
                     uint32_t slots,uint32_t *stream_lengths,uint32_t *lengths,
                     uint32_t *begins,uint32_t *ends,uint8_t *flags,int8_t *maps) {
    uint32_t sl[64],ln[64]={0},lb[64]={0},le[64]={0},i,j,k;
    uint8_t fl[64]={0},seen[64]={0};int8_t map[64*8];
    uint8_t *dcm=NULL,*sample=NULL;uint32_t size=0,sample_size=0;
    int rc=-1;
    if(!rom||!dcm_ids||!sample_ids||!stream_lengths||!lengths||
       !dcm_count||dcm_count>64U||!sample_count||sample_count>64U||
       !slots||slots>8U)return -1;
    for(i=0;i<sample_count;i++)for(j=0;j<i;j++)
        if(sample_ids[i]==sample_ids[j])return -1;
    memset(map,-1,sizeof(map));
    for(i=0;i<dcm_count;i++) {
        uint32_t count,off;
        if(tntv_rom_extract(rom,DCM_DIR,dcm_ids[i],&dcm,&size)!=0)goto done;
        if(size<14U||memcmp(dcm,"DCM1",4)||dcm[4]!=2U)goto done;
        count=dcm[5];off=14U+16U*count;
        if(!count||count>slots||off>size)goto done;
        sl[i]=le32(dcm+6);
        if(!sl[i]||sl[i]!=size-off)goto done;
        for(j=0;j<count;j++) {
            const uint8_t *p=dcm+14U+j*16U;
            uint32_t length=le32(p),begin=le32(p+4),end=le32(p+8);
            uint16_t flag=le16(p+12),id=le16(p+14);
            for(k=0;k<sample_count && sample_ids[k]!=id;k++){}
            if(k==sample_count||!length||flag>255U||begin>end||end>length)goto done;
            if(seen[k] && (ln[k]!=length||lb[k]!=begin||le[k]!=end||fl[k]!=flag))goto done;
            ln[k]=length;lb[k]=begin;le[k]=end;fl[k]=(uint8_t)flag;seen[k]=1;
            map[i*slots+j]=(int8_t)k;
        }
        free(dcm);dcm=NULL;
    }
    for(k=0;k<sample_count;k++) {
        if(!seen[k])goto done;
        if(tntv_rom_extract(rom,SAMPLE_DIR,sample_ids[k],&sample,&sample_size)!=0)goto done;
        if(sample_size!=ln[k])goto done;
        free(sample);sample=NULL;
    }
    memcpy(stream_lengths,sl,dcm_count*sizeof(uint32_t));
    memcpy(lengths,ln,sample_count*sizeof(uint32_t));
    if(begins)memcpy(begins,lb,sample_count*sizeof(uint32_t));
    if(ends)memcpy(ends,le,sample_count*sizeof(uint32_t));
    if(flags)memcpy(flags,fl,sample_count);
    if(maps)memcpy(maps,map,dcm_count*slots);
    rc=0;
done:
    free(dcm);free(sample);return rc;
}
