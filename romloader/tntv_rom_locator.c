#include "tntv_rom_locator.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if defined(TNTV_BRIDGE_HOST_TEST) || defined(TNTV_ROM_LOCATOR_HOST_TEST)
#include <dirent.h>
#include <sys/stat.h>
#else
#include <psp2/io/dirent.h>
#include <psp2/io/stat.h>
#endif

static const char *const candidates[] = {
    "baserom.us.z64",
    "baserom.z64",
    "baserom.v64",
    "baserom.n64",
    "New Tetris, The (USA).z64",
    "The New Tetris (USA).z64"
};

/* Avoid accepting arbitrary files as ROMs; validation of a chosen file
 * always happens in tntv_rom_open(_sampled). */
static int is_rom_filename(const char *name) {
    const char *dot = strrchr(name, '.');
    if (!dot || strlen(dot) != 4) return 0;
    return dot[1] && dot[2] && dot[3] &&
        ((tolower((unsigned char)dot[1]) == 'z' &&
          tolower((unsigned char)dot[2]) == '6' &&
          tolower((unsigned char)dot[3]) == '4') ||
         (tolower((unsigned char)dot[1]) == 'v' &&
          tolower((unsigned char)dot[2]) == '6' &&
          tolower((unsigned char)dot[3]) == '4') ||
         (tolower((unsigned char)dot[1]) == 'n' &&
          tolower((unsigned char)dot[2]) == '6' &&
          tolower((unsigned char)dot[3]) == '4'));
}
static int candidate_path(const char *dir,const char *name,char *out,size_t cap) {
    int n;
    FILE *file;
    if (!dir || !name || !out || !cap || strchr(name, '/') ||
        strchr(name, '\\')) return 0;
    n=snprintf(out,cap,"%s/%s",dir,name);
    if (n < 0 || (size_t)n >= cap) return 0;
#if defined(TNTV_BRIDGE_HOST_TEST) || defined(TNTV_ROM_LOCATOR_HOST_TEST)
    {
        struct stat statbuf;
        if(stat(out,&statbuf)!=0 || !S_ISREG(statbuf.st_mode)) return 0;
    }
#else
    {
        SceIoStat statbuf;
        if(sceIoGetstat(out,&statbuf)<0 || !SCE_S_ISREG(statbuf.st_mode)) return 0;
    }
#endif
    file=fopen(out,"rb");
    if (!file) return 0;
    fclose(file);
    return 1;
}

int tntv_rom_find_in_directory(const char *directory,char *out,size_t capacity) {
    size_t i;
    char best[256]={0};
    if (!directory || !out || capacity < 2) return 0;
    out[0]=0;
    for(i=0;i<sizeof(candidates)/sizeof(candidates[0]);i++)
        if(candidate_path(directory,candidates[i],out,capacity)) return 1;

    /* Unknown title-based filename? Search this directory only. In case
     * several images are present, choose lexical order deterministically,
     * rather than whatever filesystem enumeration happens to return. */
#if defined(TNTV_BRIDGE_HOST_TEST) || defined(TNTV_ROM_LOCATOR_HOST_TEST)
    {
        DIR *dir=opendir(directory);
        struct dirent *entry;
        if(!dir) return 0;
        while((entry=readdir(dir))!=NULL) {
            if(is_rom_filename(entry->d_name) &&
               (!best[0] || strcmp(entry->d_name,best)<0)) {
                char path[512];
                if(candidate_path(directory,entry->d_name,path,sizeof(path))) {
                    size_t length=strlen(entry->d_name);
                    if(length < sizeof(best))
                        memcpy(best,entry->d_name,length+1);
                }
            }
        }
        closedir(dir);
    }
#else
    {
        SceUID dir=sceIoDopen(directory);
        SceIoDirent entry;
        int result;
        if(dir<0) return 0;
        memset(&entry,0,sizeof(entry));
        while((result=sceIoDread(dir,&entry))>0) {
            if(is_rom_filename(entry.d_name) &&
               (!best[0] || strcmp(entry.d_name,best)<0)) {
                char path[512];
                if(candidate_path(directory,entry.d_name,path,sizeof(path))) {
                    size_t length=strlen(entry.d_name);
                    if(length<sizeof(best))
                        memcpy(best,entry.d_name,length+1);
                }
            }
            memset(&entry,0,sizeof(entry));
        }
        sceIoDclose(dir);
    }
#endif
    if(best[0] && candidate_path(directory,best,out,capacity)) return 1;
    out[0]=0;
    return 0;
}
