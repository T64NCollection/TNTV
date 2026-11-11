#ifndef TNT_SRAM_VITA_H
#define TNT_SRAM_VITA_H

#include <stdint.h>

#define TNT_VITA_SRAM_SIZE 0x1900
#define TNT_VITA_SRAM_PLAYER_SIZE 120
#define TNT_VITA_SRAM_MAX_PLAYERS 32

typedef struct {
    char name[9];
    uint8_t salt[2];
    uint8_t pack;
    uint32_t marathon_scores[3][5];
    uint32_t ultra_scores[3][5];
    uint32_t sprint_scores[3][5];
    uint16_t time_in_seconds;
    uint16_t lines;
    int32_t unkC4;
    int32_t unkC8;
    int32_t unkCC;
    uint8_t unkD0;
    uint16_t checksum;
} TNTVitaStoredPlayer;

typedef struct {
    char name[9];
    uint8_t salt[2];
    uint32_t lines;
    uint32_t time_in_seconds;
    uint16_t rank_time_in_seconds;
    uint16_t rank_lines;
} TNTVitaGlobalScore;

/* Original default high-score text is set at ROM boot, before any fresh save. */
void TNT_Vita_SramSetDefaultNames(const char names[3][5][9]);
void TNT_Vita_SramSetRankThresholds(const uint8_t values[30]);
void TNT_Vita_SramInitFresh(uint32_t initial_game_id);
int TNT_Vita_SramLoadOrInit(uint32_t initial_game_id);
int TNT_Vita_SramSave(void);
uint32_t TNT_Vita_SramGetGameId(void);
void TNT_Vita_SramSetGameId(uint32_t game_id);
uint32_t TNT_Vita_SramGetTotalWonderLines(void);
void TNT_Vita_SramSetTotalWonderLines(uint32_t total_wonder_lines);
void TNT_Vita_SramAddWonderLines(uint32_t lines);
uint32_t TNT_Vita_SramGetBitpattern(void);
uint8_t TNT_Vita_SramGetMusicLevelStep(void);
void TNT_Vita_SramSetMusicLevelStep(uint8_t step);
uint8_t TNT_Vita_SramGetSfxLevelStep(void);
void TNT_Vita_SramSetSfxLevelStep(uint8_t step);
uint8_t TNT_Vita_SramGetMusicMode(void);
void TNT_Vita_SramSetMusicMode(uint8_t mode);
uint8_t TNT_Vita_SramGetSong(void);
void TNT_Vita_SramSetSong(uint8_t song);
uint8_t TNT_Vita_SramGetUnlockAll(void);
void TNT_Vita_SramSetUnlockAll(uint8_t enabled);
int TNT_Vita_SramGetPlayer(int slot, TNTVitaStoredPlayer *player);
int TNT_Vita_SramCreatePlayer(const TNTVitaStoredPlayer *player, int *slot_out);
int TNT_Vita_SramUpdatePlayer(int slot, const TNTVitaStoredPlayer *player);
int TNT_Vita_SramDeletePlayer(int slot);
int TNT_Vita_SramGetGlobalScore(uint8_t game_type, int index, TNTVitaGlobalScore *score);
uint8_t TNT_Vita_SramRankFor(uint16_t time_in_seconds, uint16_t lines);
int TNT_Vita_SramRecordSinglePlayerResult(
    int slot, uint8_t game_type, uint32_t total_lines,
    uint32_t physical_lines, uint32_t elapsed_seconds);
uint32_t TNT_Vita_SramChecksumForTest(const uint8_t *data, uint32_t nbytes);
int TNT_Vita_SramValidateForTest(void);
int TNT_Vita_PlayerEncodeForTest(const TNTVitaStoredPlayer *player, uint8_t out[TNT_VITA_SRAM_PLAYER_SIZE]);
int TNT_Vita_PlayerDecodeForTest(const uint8_t in[TNT_VITA_SRAM_PLAYER_SIZE], TNTVitaStoredPlayer *player);

#endif
