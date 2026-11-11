/* Original game IMG50 texture data restored only from player's verified USA ROM.
 * Separate from the 278-entry V6 cache packer: this historical encoder keeps
 * doubled pixels together at packet boundaries. Changing the cache packer
 * would invalidate all other byte-verified local extraction recipes.
 */
#include "tntv_img_reader.h"
#include <stdlib.h>
#include <string.h>
#define TNTV_4P_PACKED_BYTES 113458U
#define TNTV_IMG_4P_PIXELS 120000U
int tntv_img_restore_4p_packbits(TNTVRom *rom,uint8_t *destination,uint32_t outcap) {
    uint8_t *original=NULL, *encoded=NULL;
    uint32_t n=0,i=0,k=0;
    int rc=-1;
    if(!rom || !destination || outcap!=TNTV_4P_PACKED_BYTES)return -1;
    if(tntv_rom_extract(rom,TNTV_ROM_IMG_DIR,50U,&original,&n)!=0)return -2;
    if(n!=TNTV_IMG_4P_PIXELS+8U ||
       original[0]!=1U || original[1]!=0x90U ||
       original[2]!=1U || original[3]!=0x2cU ||
       original[4]!=0 || original[5]!=3U ||
       original[6]!=0 || original[7]!=0)goto finish;
    encoded=(uint8_t*)malloc(TNTV_IMG_4P_PIXELS*2U+8U);
    if(!encoded){rc=-3;goto finish;}
    while(i<TNTV_IMG_4P_PIXELS) {
        const uint8_t *pixels=original+8U;
        uint32_t j=i+1U,begin;
        while(j<TNTV_IMG_4P_PIXELS && pixels[j]==pixels[i] && j-i<128U)j++;
        if(j-i>=3U) {
            encoded[k++]=(uint8_t)(257U-(j-i));
            encoded[k++]=pixels[i];
            i=j;
            continue;
        }
        begin=i++;
        while(i<TNTV_IMG_4P_PIXELS && i-begin<128U) {
            /* Exact historical packet boundary: don't split a pair of
             * matching pixels between the last and next literal packet. */
            if(i-begin==127U && i+1U<TNTV_IMG_4P_PIXELS &&
               pixels[i]==pixels[i+1U])break;
            j=i+1U;
            while(j<TNTV_IMG_4P_PIXELS && pixels[j]==pixels[i] &&
                  j-i<128U)j++;
            if(j-i>=3U)break;
            i++;
        }
        encoded[k++]=(uint8_t)(i-begin-1U);
        memcpy(encoded+k,pixels+begin,i-begin);
        k+=i-begin;
    }
    if(k!=outcap){rc=-4;goto finish;}
    memcpy(destination,encoded,k);
    rc=0;
finish:
    free(encoded);
    free(original);
    return rc;
}


/* Original IMG/ANIM sprite source reader. No original animation content embedded. */
int tntv_img_restore_anim_ci8(TNTVRom *rom,const uint16_t *frame_ids,
                              const uint16_t *palette_ids,uint32_t frames,
                              uint32_t width,uint32_t height,
                              uint8_t *ci8,uint16_t *rgba16) {
    uint32_t i,pixels;
    if(!rom || !frame_ids || !palette_ids || !ci8 || !rgba16 ||
       !frames || frames>32U || !width || !height || width>256U ||
       height>256U || width>UINT32_MAX/height)return -1;
    pixels=width*height;
    for(i=0;i<frames;i++) {
        uint8_t *asset=NULL;
        uint32_t n=0,j;
        if(tntv_rom_extract(rom,TNTV_ROM_ANIM_DIR,frame_ids[i],&asset,&n)!=0)return -2;
        if(n!=8U+pixels || asset[0]!=(uint8_t)(width>>8) ||
           asset[1]!=(uint8_t)width ||
           asset[2]!=(uint8_t)(height>>8) ||
           asset[3]!=(uint8_t)height ||
           asset[4]!=0 || asset[5]!=3 ||
           asset[6]!=0 || asset[7]!=0) {
            free(asset);return -3;
        }
        memcpy(ci8+i*pixels,asset+8,pixels);
        free(asset);asset=NULL;
        if(tntv_rom_extract(rom,TNTV_ROM_ANIM_DIR,palette_ids[i],&asset,&n)!=0)return -4;
        if(n!=512U){free(asset);return -5;}
        for(j=0;j<256U;j++)
            rgba16[i*256U+j]=(uint16_t)(((uint16_t)asset[2U*j]<<8)|asset[2U*j+1U]);
        free(asset);
    }
    return 0;
}

/* Font advances are computed from the first row of each ROM glyph.
 * Scan only inside that row; an all-marker row uses the ordinary fallback. */
int tntv_img_font_widths(TNTVRom *rom,uint32_t image_id,uint8_t widths[46]) {
    uint8_t *image=NULL, derived[46];
    uint32_t n=0,w,h,stride,i;
    if(!rom || !widths || (image_id!=7U && image_id!=8U && image_id!=9U))return -1;
    if(tntv_rom_extract(rom,TNTV_ROM_IMG_DIR,image_id,&image,&n)!=0)return -2;
    if(n<8U)goto invalid;
    w=((uint32_t)image[0]<<8)|image[1];
    h=((uint32_t)image[2]<<8)|image[3];
    if(w!=(image_id==7U?19U:(image_id==8U?16U:13U)) ||
       h!=(image_id==7U?1024U:(image_id==8U?644U:552U)) ||
       image[4]!=0 || image[5]!=2 || image[6]!=0 || image[7]!=0 ||
       n!=8U+w*h)goto invalid;
    stride=w*(h/46U);
    for(i=0;i<46U;i++) {
        int x=(int)w-1;
        const uint8_t *row=image+8U+i*stride;
        while(x>=0 && (row[x]&0xF0U)==0xF0U)--x;
        derived[i]=(uint8_t)(x+1<2?w-1U:(uint32_t)(x+1));
    }
    memcpy(widths,derived,sizeof(derived));
    free(image);return 0;
invalid:
    free(image);return -3;
}

/* Original 3P/4P culture backgrounds from the player's verified ROM.
 * Source IDs select resources, not embedded original pixels or palettes.
 * The original six-source PackBits path uses full 128-byte literal packets;
 * IMG50/Mayan has a separate historical packet-boundary special case.
 */
int tntv_img_restore_4p_culture(TNTVRom *rom, uint32_t image_id, uint32_t palette_id,
                                uint8_t *destination, uint32_t capacity,
                                uint16_t palette[256]) {
    uint8_t *raw=NULL,*pal=NULL,*encoded=NULL;
    uint16_t decoded_palette[256];
    uint32_t n=0,pal_size=0,i=0,k=0,j,begin;
    const uint8_t *pixels;
    int rc=-1;
    if(!rom || !destination || !palette || image_id<52U || image_id>62U ||
       (image_id&1U) || palette_id!=image_id+1U ||
       capacity==0U || capacity>240008U)return -1;
    if(tntv_rom_extract(rom,TNTV_ROM_IMG_DIR,image_id,&raw,&n)!=0)return -2;
    if(n!=120008U || raw[0]!=1U || raw[1]!=0x90U ||
       raw[2]!=1U || raw[3]!=0x2cU || raw[4]!=0U ||
       raw[5]!=3U || raw[6]!=0U || raw[7]!=0U)goto finish;
    encoded=(uint8_t*)malloc(240008U);
    if(!encoded){rc=-3;goto finish;}
    pixels=raw+8U;
    while(i<120000U) {
        j=i+1U;
        while(j<120000U && pixels[j]==pixels[i] && j-i<128U)j++;
        if(j-i>=3U) {
            encoded[k++]=(uint8_t)(257U-(j-i));
            encoded[k++]=pixels[i];i=j;continue;
        }
        begin=i++;
        while(i<120000U && i-begin<128U) {
            j=i+1U;
            while(j<120000U && pixels[j]==pixels[i] && j-i<128U)j++;
            if(j-i>=3U)break;
            i++;
        }
        encoded[k++]=(uint8_t)(i-begin-1U);
        memcpy(encoded+k,pixels+begin,i-begin);
        k+=i-begin;
    }
    if(k!=capacity){rc=-4;goto finish;}
    if(tntv_rom_extract(rom,TNTV_ROM_IMG_DIR,palette_id,&pal,&pal_size)!=0){
        rc=-5;goto finish;
    }
    if(pal_size!=512U){rc=-6;goto finish;}
    for(j=0;j<256U;j++)
        decoded_palette[j]=(uint16_t)(((uint16_t)pal[2U*j]<<8)|pal[2U*j+1U]);
    /* Failure-atomic outputs: write only once both resources are validated. */
    memcpy(destination,encoded,k);
    memcpy(palette,decoded_palette,sizeof(decoded_palette));
    rc=0;
finish:
    free(encoded);free(raw);free(pal);return rc;
}

/* Original credits scroller text is read from verified user-ROM .rodata.
 * No original credits letters, names, slogans or textual payload embedded.
 * Source USA1.0 credits begin at normalized ROM offset 0xA4420; 1893
 * printable/newline bytes + a trailing NUL. The UI displays 181 rows;
 * one final empty split row after the last newline is not displayed.
 */
int tntv_img_restore_credits_text(TNTVRom *rom, char *destination,
                                  uint32_t capacity, const char **lines,
                                  uint32_t line_count) {
    char stage[1894];
    uint16_t offsets[181];
    uint32_t i,nrows=0,newlines=0;
    int new_row=1;
    if(!rom || !destination || !lines || capacity!=sizeof(stage) ||
       line_count!=181U)return -1;
    if(tntv_rom_read(rom,0xA4420U,stage,sizeof(stage))!=TNTV_ROM_OK)return -2;
    if(stage[1893]!='\0')return -3;
    for(i=0;i<1893U;i++) {
        unsigned char ch=(unsigned char)stage[i];
        if(ch==0 || (ch!='\n' && (ch<32U || ch>126U)))return -4;
        if(new_row) {
            if(nrows>=line_count)return -5;
            offsets[nrows++]=(uint16_t)i;
            new_row=0;
        }
        if(ch=='\n') {
            stage[i]='\0';
            newlines++;
            new_row=1;
        }
    }
    if(nrows!=line_count || newlines!=181U || !new_row)return -6;
    /* Failure-atomic for the visible rows and text buffer. */
    memcpy(destination,stage,sizeof(stage));
    for(i=0;i<line_count;i++)
        lines[i]=destination+offsets[i];
    return 0;
}

/* Test198Q: remove original authored Wonder narratives / CPU guest names
 * from compiled game. Only numeric byte offsets; source ROM hash validated.
 * No original copyrighted strings are present in this implementation.
 */
#include "tntv_ui_text_offsets.inc"

static int tntv_ui_read_one(TNTVRom *rom,uint32_t at,
                            char *out,size_t capacity) {
    char scratch[32];
    size_t i;
    if(!rom || !out || at==0U || capacity<2U ||
       capacity>sizeof(scratch))return -1;
    if(at>=TNTV_ROM_SIZE || (uint64_t)at+capacity>TNTV_ROM_SIZE)return -2;
    if(tntv_rom_read(rom,at,scratch,capacity)!=TNTV_ROM_OK)return -3;
    for(i=0;i<capacity;i++) {
        unsigned char c=(unsigned char)scratch[i];
        if(c==0U)break;
        if(c<32U || c>126U)return -4;
    }
    if(i==0U || i==capacity)return -5;
    memcpy(out,scratch,i+1U);
    return 0;
}
int tntv_img_restore_ui_strings(TNTVRom *rom,
        char story_storage[7][18][32], const char *stories[7][18],
        char guest_storage[8][9][16], const char *guests[8][9]) {
    char stage_story[7][18][32];
    char stage_guest[8][9][16];
    unsigned culture,row;
    if(!rom || !story_storage || !stories || !guest_storage || !guests)
        return -1;
    memset(stage_story,0,sizeof(stage_story));
    memset(stage_guest,0,sizeof(stage_guest));
    for(culture=0;culture<7U;culture++) {
        for(row=0;row<18U;row++) {
            uint32_t at=s_tntv_wonder_story_offsets[culture][row];
            if(!at)continue; /* genuine empty story line */
            if(tntv_ui_read_one(rom,at,stage_story[culture][row],32U)!=0)
                return -2;
        }
    }
    for(culture=0;culture<8U;culture++) {
        for(row=0;row<9U;row++) {
            uint32_t at=s_tntv_guest_name_offsets[culture][row];
            if(tntv_ui_read_one(rom,at,stage_guest[culture][row],16U)!=0)
                return -3;
        }
    }
    /* Commit only after all original text verified — no partially written
     * pixels/strings/pointers can be exposed following a failed ROM read. */
    memcpy(story_storage,stage_story,sizeof(stage_story));
    memcpy(guest_storage,stage_guest,sizeof(stage_guest));
    for(culture=0;culture<7U;culture++)
        for(row=0;row<18U;row++)
            stories[culture][row]=story_storage[culture][row];
    for(culture=0;culture<8U;culture++)
        for(row=0;row<9U;row++)
            guests[culture][row]=guest_storage[culture][row];
    return 0;
}
