/* Test126R: Private first TNTV native game -> user's ROM/cache integration.
 * NOT a releasable asset-free build; most original game assets still embed.
 * Does not read/write existing TNTV SRAM and does not overwrite game VPK.
 */
#include "tntv_rom_bridge.h"
#include "tntv_rom_reader.h"
#include "tntv_rom_locator.h"
#include "tntv_img_reader.h"
#include "tntv_cache_reader.h"
#include "tntv_cache_builder.h"
#include "tntv_music_builder.h"
#include "third_party/crypto-algorithms/sha256.h"

typedef struct {
    const char *name;
    void *destination;
    size_t length;
    int element_u16_be;
} TNTVBridgeBinding;
#include "tntv_bridge_bindings_v6.inc"

#ifdef TNTV_BRIDGE_HOST_TEST
#include <sys/stat.h>
#define sceIoMkdir mkdir
#else
#include <psp2/io/stat.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "tntv_original_ui_offsets.inc"
#ifndef TNTV_BRIDGE_HOST_TEST
#include "tnt_sram_vita.h"
#include "tntv_score_default_offsets.inc"
extern void TNTV_Vita_SetPFGFXTables(const unsigned char bytes[192]);
extern void TNTV_Vita_SetOriginalPieceColors(const unsigned char bytes[72]);
extern void TNTV_Vita_SetAIOriginalTables(const unsigned char bytes[32]);
#endif


/* Test213D: loaded from owner's normalized ROM after revision gate.
 * All original authored bytes remain in the BSS until user supplies ROM.
 * Do not add these to the persistent ROM-derived cache; this keeps
 * already-approved audio/art/scene cache checksums and formats stable. */
static char s_tntv_original_ui_text[TNTV_UI_LITERAL_COUNT][64];
unsigned char g_tnt_font_c_map[64];
unsigned char g_tnt_font_c_char_widths[46];
char g_tnt_name_editor_chars[45];

const char *TNTV_ROM_OriginalText(unsigned id) {
    return id<TNTV_UI_LITERAL_COUNT?s_tntv_original_ui_text[id]:"";
}


#ifndef TNTV_BRIDGE_HOST_TEST
/* These original N64 records must exist before game/SRAM initialization.
 * Their old static initializer bytes were contiguous owner-ROM data in ELF.
 * Populate private BSS from the already-open revision-checked user's ROM. */
static int tntv_rom_restore_additional_originals(TNTVRom *rom) {
    unsigned char positions[192],palette[72],ai[32],rank_thresholds[30];
    char defaults[3][5][9];
    unsigned row,col;
    memset(defaults,0,sizeof(defaults));
    if(tntv_rom_read(rom,0x96421U,positions,sizeof(positions))!=TNTV_ROM_OK)
        return -1;
    if(tntv_rom_read(rom,0x96211U,palette,sizeof(palette))!=TNTV_ROM_OK)
        return -2;
    if(tntv_rom_read(rom,0x8c050U,ai,sizeof(ai))!=TNTV_ROM_OK)
        return -3;
    if(tntv_rom_read(rom,0x9907cU,rank_thresholds,sizeof(rank_thresholds))!=TNTV_ROM_OK)
        return -7;
    for(row=0;row<3;row++)
        for(col=0;col<5;col++){
            const TNTVScoreTextSlice *sl=&s_tntv_default_score_offsets[row][col];
            if(sl->length==0 || sl->length>9)return -4;
            if(tntv_rom_read(rom,sl->offset,defaults[row][col],sl->length)!=TNTV_ROM_OK)
                return -5;
            if(defaults[row][col][sl->length-1]!=0)return -6;
        }
    TNTV_Vita_SetPFGFXTables(positions);
    TNTV_Vita_SetOriginalPieceColors(palette);
    TNTV_Vita_SetAIOriginalTables(ai);
    TNT_Vita_SramSetDefaultNames(defaults);
    TNT_Vita_SramSetRankThresholds(rank_thresholds);
    return 0;
}
#endif

static int tntv_rom_restore_original_ui(TNTVRom *rom) {
    unsigned i;
    unsigned char map[64],widths[46];
    char name_chars[45];
    char text[TNTV_UI_LITERAL_COUNT][64];
    if(!rom)return -1;
    if(tntv_rom_read(rom,0x98f70U,map,sizeof(map))!=TNTV_ROM_OK)
        return -2;
    if(tntv_rom_read(rom,0x9a020U,name_chars,sizeof(name_chars))!=TNTV_ROM_OK)
        return -3;
    if(tntv_img_font_widths(rom,9U,widths)!=0)
        return -4;
    memset(text,0,sizeof(text));
    for(i=0;i<TNTV_UI_LITERAL_COUNT;i++){
        const TNTVOriginalTextSlice *entry=&s_tntv_original_text_offsets[i];
        if(entry->length==0U || entry->length>=64U)
            return -5;
        if(tntv_rom_read(rom,entry->offset,text[i],entry->length)!=TNTV_ROM_OK)
            return -6;
        text[i][entry->length]=0;
    }
    memcpy(g_tnt_font_c_map,map,sizeof(map));
    memcpy(g_tnt_font_c_char_widths,widths,sizeof(widths));
    memcpy(g_tnt_name_editor_chars,name_chars,sizeof(name_chars));
    memcpy(s_tntv_original_ui_text,text,sizeof(text));
    return 0;
}


#ifdef TNTV_BRIDGE_HOST_TEST
#ifndef TNTV_BRIDGE_HOST_DATA_DIR
#error Must define isolated host test data directory
#endif
#define TNTV_DATA_DIR TNTV_BRIDGE_HOST_DATA_DIR
#else
#define TNTV_DATA_DIR "ux0:data/TNTV"
#endif
#define TNTV_BRIDGE_CACHE TNTV_DATA_DIR "/tntv_rom_bridge_v6.pak"
#define TNTV_BRIDGE_LOG TNTV_DATA_DIR "/tntv_rom_bridge_status.txt"
#define TNTV_SCENE_FILE TNTV_DATA_DIR "/tntv_scene_cache_v1.bin"
#define TNTV_MUSIC_FILE TNTV_DATA_DIR "/tntv_music_from_rom_v1.bin"


#ifdef TNTV_BRIDGE_HOST_TEST
#include <time.h>
static uint64_t boot_us(void) { struct timespec t;timespec_get(&t,TIME_UTC);return (uint64_t)t.tv_sec*1000000U+t.tv_nsec/1000U; }
#else
#include <psp2/kernel/threadmgr.h>
static uint64_t boot_us(void) { return sceKernelGetSystemTimeWide(); }
#endif
static uint64_t s_boot_start,s_boot_previous;
static char s_boot_timings[32768];
static size_t s_boot_timing_used;
static unsigned s_scene_progress_steps;
static void save_boot_timings(void) {
    FILE *f=fopen(TNTV_DATA_DIR "/tntv_boot_timings.txt","w");
    if(f){fwrite(s_boot_timings,1,s_boot_timing_used,f);fclose(f);}
}
static void (*s_progress)(const char *);
void TNTV_ROM_SetProgress(void (*callback)(const char *)) { s_progress=callback; }
static void write_status(const char *s) {
    /* Cold scene extraction used to present and sync 198 individual scene
     * checkpoints, each with a mandatory VBlank wait and status-file write.
     * Sample every 8th step (also the first) to keep meaningful progress
     * without inserting several seconds of avoidable presentation delay.
     * All errors and major phases are still reported immediately. */
    if(strstr(s," SCENE STEP ")!=NULL &&
       (++s_scene_progress_steps!=1U) &&
       (s_scene_progress_steps%8U)!=0U)return;
    uint64_t now=boot_us();
    size_t remain=sizeof(s_boot_timings)-s_boot_timing_used;
    if(remain>1) {
        int n=snprintf(s_boot_timings+s_boot_timing_used,remain,
            "%llu ms total, %llu ms previous phase: %s\n",
            (unsigned long long)((now-s_boot_start)/1000U),
            (unsigned long long)((now-s_boot_previous)/1000U),s);
        if(n>0)s_boot_timing_used+=(size_t)n<remain?(size_t)n:remain-1;
    }
    s_boot_previous=now;
    if(s_progress)s_progress(s);
    FILE *f=fopen(TNTV_BRIDGE_LOG,"w");
    if(f){fputs(s,f);fputc('\n',f);fclose(f);}
}
/* Check every byte of the local 241-entry cache, including the directory
 * and any alignment padding. Per-asset CRC32 alone cannot catch changes
 * to unused directory entries or header padding. This SHA is a checksum
 * of our approved SOURCE RECIPE output, not embedded original game bytes.
 */
static int verify_cache_file_hash(void) {
    static const uint8_t expected[32]={
        0xb8,0xe1,0xdb,0x76,0x68,0x1b,0xb7,0xff,
        0x5c,0x67,0x97,0xdd,0x11,0xe2,0xb5,0xad,
        0x61,0xe3,0x37,0x2c,0x5b,0xca,0xdf,0x78,
        0x2f,0x8a,0x03,0x40,0x33,0x30,0xef,0x54
    };
    uint8_t buf[16384],digest[32];
    FILE *f=fopen(TNTV_BRIDGE_CACHE,"rb");
    long n;
    SHA256_CTX ctx;
    size_t got;
    if(!f)return -1;
    if(fseek(f,0,SEEK_END)!=0){fclose(f);return -1;}
    n=ftell(f);
    if(n!=5876868L){fclose(f);return -1;}
    if(fseek(f,0,SEEK_SET)!=0){fclose(f);return -1;}
    sha256_init(&ctx);
    while((got=fread(buf,1,sizeof(buf),f))>0)
        sha256_update(&ctx,buf,got);
    if(ferror(f)){fclose(f);return -1;}
    fclose(f);
    sha256_final(&ctx,digest);
    return memcmp(digest,expected,sizeof(digest))==0?0:-1;
}
/* A full-file expected SHA-256 covers header, directory, padding and every
 * asset byte. Reading every asset for CRC here duplicated the checksum pass
 * AND the actual array binding load below. Cache reads still verify CRC32
 * before accepting each bound array. Any changed byte fails the SHA here
 * and triggers the existing ROM-derived archive rebuild. */
static int verify_all(TNTVCache *cache) {
    if(cache->count!=278)return -1;
    return verify_cache_file_hash();
}
/* The quick ROM fingerprint is ONLY eligible when every private original-
 * ROM-derived cache is present at the expected length. A corrupt cache with
 * correct length is discovered by its own SHA validation and forces upgrade
 * to the complete ROM hash *before* rebuilding. No persistent trust flag. */
static int exists_with_size(const char *path,long required) {
    FILE *f=fopen(path,"rb");
    long n;
    if(!f)return 0;
    n=(fseek(f,0,SEEK_END)==0)?ftell(f):-1;
    fclose(f);
    return n==required;
}
static int can_use_sampled_rom(void) {
    return exists_with_size(TNTV_SCENE_FILE,3371917L) &&
           exists_with_size(TNTV_BRIDGE_CACHE,5876868L) &&
           exists_with_size(TNTV_MUSIC_FILE,10589568L);
}
static int require_verified_rom(TNTVRom *rom,const char *reason) {
    if(rom->fully_verified)return TNTV_ROM_OK;
    write_status(reason);
    if(tntv_rom_verify_full(rom)!=TNTV_ROM_OK) {
        write_status("208D FULL ROM HASH FAILED: supported revision required");
        return TNTV_ROM_ERR_REVISION;
    }
    return TNTV_ROM_OK;
}
/* Palette arrays are native uint16_t. The ROM cache stores each N64 RGBA16
 * numeric pixel in big-endian byte order, unlike ARM's native little-endian
 * memory layout. Load each item by numeric value to preserve exact colors.
 */
static int load_bound_array(TNTVCache *cache,
                            const TNTVBridgeBinding *binding) {
    const TNTVCacheEntry *entry=tntv_cache_find(cache,binding->name);
    uint8_t *raw;
    uint16_t *dst16;
    size_t i;
    int rc;
    if(!entry || entry->length!=binding->length)return -1;
    if(!binding->element_u16_be)
        return tntv_cache_read(cache,binding->name,binding->destination,
                               binding->length);
    if(binding->length%2!=0)return -2;
    raw=(uint8_t*)malloc(binding->length);
    if(!raw)return -3;
    rc=tntv_cache_read(cache,binding->name,raw,binding->length);
    if(rc==0) {
        dst16=(uint16_t*)binding->destination;
        for(i=0;i<binding->length/2;i++)
            dst16[i]=(uint16_t)(((uint16_t)raw[2*i]<<8)|raw[2*i+1]);
    }
    free(raw);
    return rc;
}
#include "tntv_scene_bindings.inc"
#include "tntv_scene_cache.inc"

int TNTV_ROM_BridgeBoot(void) {
    TNTVRom rom;
    /* ~39KB cache directory is not a transient call-frame object. The
     * ROM bridge runs only on the main thread at startup, so BSS is safe.
     * This eliminates nested 78KB main +39KB cache stack pressure. */
    static TNTVCache cache;
    const char *path;
    char rom_path[512];
    int rc;
    s_boot_start=s_boot_previous=boot_us();s_boot_timing_used=0;s_scene_progress_steps=0;
    sceIoMkdir(TNTV_DATA_DIR,0777);
    path=tntv_rom_find_in_directory(TNTV_DATA_DIR,rom_path,sizeof(rom_path))
        ? rom_path : NULL;
    if(path)write_status("208D ROM FOUND: verifying supported USA 1.0 revision");
    if(!path){
        write_status("TEST126R ROM MISSING: supply USA 1.0 in ux0:data/TNTV");
        return -1;
    }
    /* Every new install or missing asset cache requires a FULL canonical
     * ROM SHA-256. Only warmed, complete caches permit the sampled quick
     * check; the full hash remains required on any cache regeneration. */
    {
        int sampled=can_use_sampled_rom();
        rc=sampled ? tntv_rom_open_sampled(&rom,path)
                   : tntv_rom_open(&rom,path);
        if(rc!=TNTV_ROM_OK){
            write_status("TEST126R ROM INVALID: require original USA 1.0 ROM");
            return -2;
        }
        write_status(sampled
            ? "208D FAST ROM FINGERPRINT OK: checking saved native scenes"
            : "208D FULL ROM SHA256 OK: first-time assets or repair");
    }
    rc=load_cached_scene_tables(&rom);
    if(rc!=0){ write_status("TEST195Q: original Wonder scene tables failed");
        tntv_rom_close(&rom); return -11; }
    /* Must also run on warm-cache boots; ROM is already open and sampled
     * revision-checked. UI text and glyph metrics are read fresh, never
     * shipped or stored in the original art/music cache. */
    rc=tntv_rom_restore_original_ui(&rom);
    if(rc!=0){
        write_status("213D: original ROM UI metadata read failed");
        tntv_rom_close(&rom); return -15;
    }

#ifndef TNTV_BRIDGE_HOST_TEST
    rc=tntv_rom_restore_additional_originals(&rom);
    if(rc!=0){
        write_status("215D: ROM gameplay tables / SRAM defaults read failed");
        tntv_rom_close(&rom); return -16;
    }
#endif
    write_status("208D SCENES VERIFIED: validating or building cached artwork");
    rc=tntv_cache_open(&cache,TNTV_BRIDGE_CACHE,tntv_rom_expected_md5());
    if(rc==0){
        rc=verify_all(&cache);
        tntv_cache_close(&cache);
    }
    if(rc!=0) {
        if(require_verified_rom(&rom,
           "208D FULL ROM HASH REQUIRED: rebuilding artwork")!=TNTV_ROM_OK) {
            tntv_rom_close(&rom);
            return -2;
        }
        /* Delete only test126R's own cache. Never delete save data. */
        remove(TNTV_BRIDGE_CACHE);
        write_status("TEST126R ROM VERIFIED: generating local native data");
        if(tntv_cache_build_from_verified_rom(&rom,TNTV_BRIDGE_CACHE)!=0){
            write_status("TEST126R CACHE ERROR: extraction failed");
            tntv_rom_close(&rom);
            return -3;
        }
        rc=tntv_cache_open(&cache,TNTV_BRIDGE_CACHE,tntv_rom_expected_md5());
        if(rc!=0){
            write_status("TEST126R CACHE ERROR: open failed after generation");
            tntv_rom_close(&rom);
            return -4;
        }
        rc=verify_all(&cache);
        tntv_cache_close(&cache);
        if(rc!=0){
            remove(TNTV_BRIDGE_CACHE);
            write_status("TEST126R CACHE ERROR: asset CRC mismatch");
            tntv_rom_close(&rom);
            return -5;
        }
    }
    write_status("208D CACHED ART VERIFIED: binding 278 cache structures");
    if(tntv_cache_open(&cache,TNTV_BRIDGE_CACHE,
                       tntv_rom_expected_md5())!=0){
        write_status("TEST126R CACHE ERROR: final index failed");
        tntv_rom_close(&rom);
        return -6;
    }
    rc=0;
    {
        size_t i;
        for(i=0;i<sizeof(g_tntv_bridge_bindings)/sizeof(g_tntv_bridge_bindings[0]);i++) {
            if(load_bound_array(&cache,&g_tntv_bridge_bindings[i])!=0){
                rc=-7;
                break;
            }
        }
    }
    tntv_cache_close(&cache);
    if(rc!=0){
        write_status("TEST126R ASSET ERROR: startup images could not load");
        tntv_rom_close(&rom);
        return -7;
    }
    write_status("208D ART BOUND: validating or generating local 16-song cache");
    /* Test128R: require the complete, source-exact original soundtrack from
     * the user's ROM before game audio init. Never ship the original pack.
     * Only this dedicated local audio cache may be deleted on corruption;
     * preserve all SRAM save data and other application files.
     */
    {
        static const char *song_pack=TNTV_MUSIC_FILE;
        if(tntv_music_validate(song_pack)!=0) {
            if(require_verified_rom(&rom,
                "208D FULL ROM HASH REQUIRED: rebuilding soundtrack")!=TNTV_ROM_OK) {
                tntv_rom_close(&rom);
                return -2;
            }
            remove(song_pack);
            write_status("TEST128R: ROM verified; preparing original game music.");
            /* Builder SHA-verifies the private pack before its atomic rename.
             * An immediate second 10.6 MiB read is unnecessary on first boot;
             * subsequent launches still verify the saved file fully. */
            if(tntv_music_build_from_verified_rom(&rom,song_pack)!=0) {
                remove(song_pack);
                write_status("TEST128R: failed to generate verified song cache.");
                tntv_rom_close(&rom);
                return -10;
            }
        }
    }
    write_status("208D PASS: ROM scenes artwork music ready");
    save_boot_timings();
    tntv_rom_close(&rom);
    return 0;
}
