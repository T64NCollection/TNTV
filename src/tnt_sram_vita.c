#include "tnt_sram_vita.h"

#include <stddef.h>
#include <string.h>

#ifdef __vita__
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#endif

#define SRAM_OFF_BITPATTERN      0x0000
#define SRAM_OFF_PLAYERS         0x0004
#define SRAM_OFF_WONDER_ODD      0x0F04
#define SRAM_OFF_WONDER_EVEN     0x0F08
#define SRAM_OFF_GLOBAL_MARATHON 0x0F0C
#define SRAM_OFF_GLOBAL_ULTRA    0x0F84
#define SRAM_OFF_GLOBAL_SPRINT   0x0FFC
#define SRAM_OFF_TNTV_FLAGS      0x18E0
#define SRAM_OFF_MUSIC_LEVEL     0x18E4
#define TNTV_SRAM_FLAG_UNLOCK_ALL 0x01U
#define SRAM_OFF_SFX_LEVEL       0x18E8
#define SRAM_OFF_SONG            0x18EC
#define SRAM_OFF_MUSIC_MODE      0x18F0
#define SRAM_OFF_GAME_ID         0x18F4
#define SRAM_OFF_VERSION         0x18F8
#define SRAM_OFF_CHECKSUM        0x18FC
#define SRAM_COPY_COUNT          3

#ifdef __vita__
#define TNT_VITA_SRAM_DIR  "ux0:data/TNTV"
#define TNT_VITA_SRAM_PATH "ux0:data/TNTV/tntv_sram.bin"
#endif

static uint8_t g_tnt_sram_image[TNT_VITA_SRAM_SIZE];

/* Exact user-ROM defaults. Populated at verified ROM boot; the unmodified
 * SRAM format and three-copy persistent save remain unchanged. */
static char s_tntv_default_score_names[3][5][9];
static uint8_t s_tntv_rank_thresholds[30];
void TNT_Vita_SramSetRankThresholds(const uint8_t values[30]){
    if(values)memcpy(s_tntv_rank_thresholds,values,sizeof(s_tntv_rank_thresholds));
}


void TNT_Vita_SramSetDefaultNames(const char names[3][5][9]) {
    if(!names)return;
    memcpy(s_tntv_default_score_names,names,sizeof(s_tntv_default_score_names));
}


static uint32_t read_be32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static void write_be16(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v >> 8);
    p[1] = (uint8_t)v;
}

static void write_be32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

static uint32_t sram_checksum(const uint8_t *data, uint32_t nbytes) {
    uint32_t chksum = 0;
    uint32_t i;
    for (i = 0; i < nbytes; i += 4) {
        chksum += data[i + 0] ^ 0x10U;
        chksum -= data[i + 2];
        chksum += data[i + 1] ^ 0x20U;
        chksum -= (uint32_t)data[i + 3] << 1;
    }
    return chksum;
}

uint32_t TNT_Vita_SramChecksumForTest(const uint8_t *data, uint32_t nbytes) {
    return sram_checksum(data, nbytes);
}

static uint16_t player_checksum12(const TNTVitaStoredPlayer *player) {
    uint8_t data[12];
    uint16_t chksum = 0;
    uint32_t i;
    memcpy(data, player->name, 9);
    data[9] = player->salt[0];
    data[10] = player->salt[1];
    data[11] = player->pack;
    for (i = 0; i < 11; i += 4) {
        chksum = (uint16_t)(chksum + (data[i + 0] ^ 0x10U));
        chksum = (uint16_t)(chksum - data[i + 2]);
        chksum = (uint16_t)(chksum + (data[i + 1] ^ 0x20U));
        chksum = (uint16_t)(chksum + ((uint16_t)data[i + 3] << 1));
    }
    return chksum;
}

typedef struct {
    uint8_t *data;
    uint32_t bitpos;
} BitWriter;

typedef struct {
    const uint8_t *data;
    uint32_t bitpos;
} BitReader;

static void bits_write(BitWriter *w, uint32_t value, uint32_t nbits) {
    uint32_t i;
    for (i = 0; i < nbits; i++) {
        uint32_t shift = nbits - 1U - i;
        uint32_t bit = (value >> shift) & 1U;
        uint32_t byte_index = w->bitpos >> 3;
        uint32_t bit_index = 7U - (w->bitpos & 7U);
        if (bit)
            w->data[byte_index] |= (uint8_t)(1U << bit_index);
        w->bitpos++;
    }
}

static uint32_t bits_read(BitReader *r, uint32_t nbits) {
    uint32_t value = 0;
    uint32_t i;
    for (i = 0; i < nbits; i++) {
        uint32_t byte_index = r->bitpos >> 3;
        uint32_t bit_index = 7U - (r->bitpos & 7U);
        value = (value << 1) | ((r->data[byte_index] >> bit_index) & 1U);
        r->bitpos++;
    }
    return value;
}

int TNT_Vita_PlayerEncodeForTest(
    const TNTVitaStoredPlayer *src,
    uint8_t out[TNT_VITA_SRAM_PLAYER_SIZE]
) {
    TNTVitaStoredPlayer player;
    BitWriter w;
    int i;
    int group;

    if (src == NULL || out == NULL)
        return -1;

    player = *src;
    player.checksum = player_checksum12(&player);
    memset(out, 0, TNT_VITA_SRAM_PLAYER_SIZE);
    w.data = out;
    w.bitpos = 0;

    for (i = 0; i < 8; i++) {
        uint8_t c = (uint8_t)player.name[i];
        bits_write(&w, c != 0 ? (uint8_t)(c - 32U) : 0U, 6);
    }
    bits_write(&w, player.salt[0], 8);
    bits_write(&w, player.salt[1], 8);
    bits_write(&w, player.pack, 5);

    for (group = 0; group < 3; group++)
        for (i = 0; i < 5; i++) bits_write(&w, player.marathon_scores[group][i], 17);
    for (group = 0; group < 3; group++)
        for (i = 0; i < 5; i++) bits_write(&w, player.ultra_scores[group][i], 17);
    for (group = 0; group < 3; group++)
        for (i = 0; i < 5; i++) bits_write(&w, player.sprint_scores[group][i], 17);

    bits_write(&w, player.time_in_seconds, 10);
    bits_write(&w, player.lines, 14);
    bits_write(&w, (uint32_t)player.unkC4, 20);
    bits_write(&w, (uint32_t)player.unkC8, 30);
    bits_write(&w, (uint32_t)player.unkCC, 30);
    bits_write(&w, player.unkD0, 1);
    bits_write(&w, player.checksum, 16);

    return w.bitpos == 955U ? 0 : -1;
}

int TNT_Vita_PlayerDecodeForTest(
    const uint8_t in[TNT_VITA_SRAM_PLAYER_SIZE],
    TNTVitaStoredPlayer *player
) {
    BitReader r;
    int i;
    int group;

    if (in == NULL || player == NULL)
        return -1;

    memset(player, 0, sizeof(*player));
    r.data = in;
    r.bitpos = 0;

    for (i = 0; i < 8; i++)
        player->name[i] = (char)((bits_read(&r, 6) & 0x3FU) + 32U);
    player->name[8] = '\0';
    player->salt[0] = (uint8_t)bits_read(&r, 8);
    player->salt[1] = (uint8_t)bits_read(&r, 8);
    player->pack = (uint8_t)(bits_read(&r, 5) & 0x1FU);

    for (group = 0; group < 3; group++)
        for (i = 0; i < 5; i++) player->marathon_scores[group][i] = bits_read(&r, 17) & 0x1FFFFU;
    for (group = 0; group < 3; group++)
        for (i = 0; i < 5; i++) player->ultra_scores[group][i] = bits_read(&r, 17) & 0x1FFFFU;
    for (group = 0; group < 3; group++)
        for (i = 0; i < 5; i++) player->sprint_scores[group][i] = bits_read(&r, 17) & 0x1FFFFU;

    player->time_in_seconds = (uint16_t)(bits_read(&r, 10) & 0x3FFU);
    /* Preserve original decoder bug: reads 14 bits, then masks to 12. */
    player->lines = (uint16_t)(bits_read(&r, 14) & 0x0FFFU);
    player->unkC4 = (int32_t)(bits_read(&r, 20) & 0xFFFFFU);
    player->unkC8 = (int32_t)(bits_read(&r, 30) & 0x3FFFFFFFU);
    player->unkCC = (int32_t)(bits_read(&r, 30) & 0x3FFFFFFFU);
    player->unkD0 = (uint8_t)(bits_read(&r, 1) & 1U);
    player->checksum = (uint16_t)(bits_read(&r, 16) & 0xFFFFU);
    /* func_8007B430 always clears this runtime dirty flag after decode. */
    player->unkD0 = 0;

    return r.bitpos == 955U ? 0 : -1;
}

static void init_global_scores(
    uint32_t base,
    const char *const names[5],
    const uint32_t lines[5],
    const uint32_t times[5]
) {
    int i;
    for (i = 0; i < 5; i++) {
        uint8_t *entry = &g_tnt_sram_image[base + (uint32_t)i * 0x18U];
        size_t n = strlen(names[i]);
        if (n > 8) n = 8;
        memcpy(entry, names[i], n);
        entry[n] = 0;
        entry[0x0A] = 0;
        entry[0x0B] = 0;
        write_be32(entry + 0x0C, lines[i]);
        write_be32(entry + 0x10, times[i]);
        write_be16(entry + 0x14, 0);
        write_be16(entry + 0x16, 0);
    }
}

void TNT_Vita_SramInitFresh(uint32_t initial_game_id) {
    const char *const marathon_names[5] = {
        s_tntv_default_score_names[0][0],
        s_tntv_default_score_names[0][1],
        s_tntv_default_score_names[0][2],
        s_tntv_default_score_names[0][3],
        s_tntv_default_score_names[0][4],
    };
    const char *const ultra_names[5] = {
        s_tntv_default_score_names[1][0],
        s_tntv_default_score_names[1][1],
        s_tntv_default_score_names[1][2],
        s_tntv_default_score_names[1][3],
        s_tntv_default_score_names[1][4],
    };
    const char *const sprint_names[5] = {
        s_tntv_default_score_names[2][0],
        s_tntv_default_score_names[2][1],
        s_tntv_default_score_names[2][2],
        s_tntv_default_score_names[2][3],
        s_tntv_default_score_names[2][4],
    };
    static const uint32_t marathon_lines[5] = {500, 400, 300, 200, 100};
    static const uint32_t ultra_lines[5] = {150, 150, 150, 150, 150};
    static const uint32_t sprint_lines[5] = {70, 60, 40, 30, 20};
    static const uint32_t marathon_times[5] = {500, 400, 300, 200, 100};
    static const uint32_t ultra_times[5] = {120, 180, 240, 300, 360};
    static const uint32_t sprint_times[5] = {180, 180, 180, 180, 180};

    memset(g_tnt_sram_image, 0, sizeof(g_tnt_sram_image));
    write_be32(g_tnt_sram_image + SRAM_OFF_BITPATTERN, 0);
    write_be32(g_tnt_sram_image + SRAM_OFF_WONDER_ODD, 0);
    write_be32(g_tnt_sram_image + SRAM_OFF_WONDER_EVEN, 0);
    init_global_scores(SRAM_OFF_GLOBAL_MARATHON, marathon_names, marathon_lines, marathon_times);
    init_global_scores(SRAM_OFF_GLOBAL_ULTRA, ultra_names, ultra_lines, ultra_times);
    init_global_scores(SRAM_OFF_GLOBAL_SPRINT, sprint_names, sprint_lines, sprint_times);
    write_be32(g_tnt_sram_image + SRAM_OFF_MUSIC_LEVEL, 2340U * 10U);
    write_be32(g_tnt_sram_image + SRAM_OFF_SFX_LEVEL, 0x7000U);
    write_be32(g_tnt_sram_image + SRAM_OFF_SONG, 0);
    write_be32(g_tnt_sram_image + SRAM_OFF_MUSIC_MODE, 0);
    write_be32(g_tnt_sram_image + SRAM_OFF_GAME_ID, initial_game_id);
    memcpy(g_tnt_sram_image + SRAM_OFF_VERSION, "0.0b", 4);
    write_be32(
        g_tnt_sram_image + SRAM_OFF_CHECKSUM,
        sram_checksum(g_tnt_sram_image, SRAM_OFF_CHECKSUM));
}

static int sram_image_valid(const uint8_t *image) {
    if (memcmp(image + SRAM_OFF_VERSION, "0.0b", 4) != 0)
        return 0;
    return sram_checksum(image, SRAM_OFF_CHECKSUM) ==
           read_be32(image + SRAM_OFF_CHECKSUM);
}

int TNT_Vita_SramValidateForTest(void) {
    return sram_image_valid(g_tnt_sram_image);
}

uint32_t TNT_Vita_SramGetGameId(void) {
    return read_be32(g_tnt_sram_image + SRAM_OFF_GAME_ID);
}

void TNT_Vita_SramSetGameId(uint32_t game_id) {
    write_be32(g_tnt_sram_image + SRAM_OFF_GAME_ID, game_id);
}

uint32_t TNT_Vita_SramGetTotalWonderLines(void) {
    return read_be32(g_tnt_sram_image + SRAM_OFF_WONDER_ODD) |
           read_be32(g_tnt_sram_image + SRAM_OFF_WONDER_EVEN);
}

void TNT_Vita_SramSetTotalWonderLines(uint32_t total_wonder_lines) {
    /* Original set_total_wonder_lines stores alternating bits in two words. */
    write_be32(
        g_tnt_sram_image + SRAM_OFF_WONDER_ODD,
        total_wonder_lines & 0xAAAAAAAAU);
    write_be32(
        g_tnt_sram_image + SRAM_OFF_WONDER_EVEN,
        total_wonder_lines & 0x55555555U);
}

void TNT_Vita_SramAddWonderLines(uint32_t lines) {
    TNT_Vita_SramSetTotalWonderLines(
        TNT_Vita_SramGetTotalWonderLines() + lines);
}

uint32_t TNT_Vita_SramGetBitpattern(void) {
    return read_be32(g_tnt_sram_image + SRAM_OFF_BITPATTERN);
}

uint8_t TNT_Vita_SramGetMusicLevelStep(void) {
    uint32_t step = read_be32(g_tnt_sram_image + SRAM_OFF_MUSIC_LEVEL) / 2340U;
    return (uint8_t)(step > 13U ? 13U : step);
}

void TNT_Vita_SramSetMusicLevelStep(uint8_t step) {
    if (step > 13U) step = 13U;
    write_be32(g_tnt_sram_image + SRAM_OFF_MUSIC_LEVEL, (uint32_t)step * 2340U);
}

uint8_t TNT_Vita_SramGetSfxLevelStep(void) {
    uint32_t step = read_be32(g_tnt_sram_image + SRAM_OFF_SFX_LEVEL) / 2340U;
    return (uint8_t)(step > 13U ? 13U : step);
}

void TNT_Vita_SramSetSfxLevelStep(uint8_t step) {
    if (step > 13U) step = 13U;
    write_be32(g_tnt_sram_image + SRAM_OFF_SFX_LEVEL, (uint32_t)step * 2340U);
}

uint8_t TNT_Vita_SramGetMusicMode(void) {
    uint32_t mode = read_be32(g_tnt_sram_image + SRAM_OFF_MUSIC_MODE);
    return (uint8_t)(mode < 3U ? mode : 0U);
}

void TNT_Vita_SramSetMusicMode(uint8_t mode) {
    write_be32(g_tnt_sram_image + SRAM_OFF_MUSIC_MODE, mode < 3U ? mode : 0U);
}

uint8_t TNT_Vita_SramGetSong(void) {
    uint32_t song = read_be32(g_tnt_sram_image + SRAM_OFF_SONG);
    return (uint8_t)(song < 16U ? song : 0U);
}

void TNT_Vita_SramSetSong(uint8_t song) {
    write_be32(g_tnt_sram_image + SRAM_OFF_SONG, song < 16U ? song : 0U);
}

uint8_t TNT_Vita_SramGetUnlockAll(void) {
    return (uint8_t)((g_tnt_sram_image[SRAM_OFF_TNTV_FLAGS] &
                      TNTV_SRAM_FLAG_UNLOCK_ALL) != 0);
}

void TNT_Vita_SramSetUnlockAll(uint8_t enabled) {
    if (enabled)
        g_tnt_sram_image[SRAM_OFF_TNTV_FLAGS] |= TNTV_SRAM_FLAG_UNLOCK_ALL;
    else
        g_tnt_sram_image[SRAM_OFF_TNTV_FLAGS] &=
            (uint8_t)~TNTV_SRAM_FLAG_UNLOCK_ALL;
}

int TNT_Vita_SramGetPlayer(int slot, TNTVitaStoredPlayer *player) {
    uint32_t bitpattern;
    if (slot < 0 || slot >= TNT_VITA_SRAM_MAX_PLAYERS || player == NULL)
        return -1;
    bitpattern = TNT_Vita_SramGetBitpattern();
    if ((bitpattern & (1U << (uint32_t)slot)) == 0)
        return 0;
    if (TNT_Vita_PlayerDecodeForTest(
            g_tnt_sram_image + SRAM_OFF_PLAYERS + slot * TNT_VITA_SRAM_PLAYER_SIZE,
            player) != 0)
        return -1;
    return 1;
}

static uint8_t tnt_sram_rank_for(uint16_t time_in_seconds, uint16_t lines) {

    int i;
    int lines_per_minute;
    if (time_in_seconds != 0 && (time_in_seconds / 60U) != 0U) {
        lines_per_minute = (int)((float)lines / ((float)time_in_seconds / 60.0f));
        for (i = 0; i < 30; i++) {
            if (lines_per_minute < s_tntv_rank_thresholds[i])
                return (uint8_t)i;
        }
        return 29;
    }
    return 0;
}

uint8_t TNT_Vita_SramRankFor(uint16_t time_in_seconds, uint16_t lines) {
    return tnt_sram_rank_for(time_in_seconds, lines);
}

static uint32_t tnt_sram_global_base(uint8_t game_type) {
    switch (game_type) {
    case 0: return SRAM_OFF_GLOBAL_MARATHON;
    case 1: return SRAM_OFF_GLOBAL_SPRINT;
    case 2: return SRAM_OFF_GLOBAL_ULTRA;
    default: return UINT32_MAX;
    }
}

static void tnt_sram_read_global_at(uint32_t base, int index, TNTVitaGlobalScore *score) {
    const uint8_t *entry = g_tnt_sram_image + base + (uint32_t)index * 0x18U;
    memset(score, 0, sizeof(*score));
    memcpy(score->name, entry, 8);
    score->name[8] = '\0';
    score->salt[0] = entry[0x0A];
    score->salt[1] = entry[0x0B];
    score->lines = read_be32(entry + 0x0C);
    score->time_in_seconds = read_be32(entry + 0x10);
    score->rank_time_in_seconds = (uint16_t)(((uint16_t)entry[0x14] << 8) | entry[0x15]);
    score->rank_lines = (uint16_t)(((uint16_t)entry[0x16] << 8) | entry[0x17]);
}

static void tnt_sram_write_global_at(uint32_t base, int index, const TNTVitaGlobalScore *score) {
    uint8_t *entry = g_tnt_sram_image + base + (uint32_t)index * 0x18U;
    memset(entry, 0, 0x18U);
    memcpy(entry, score->name, 8);
    entry[0x0A] = score->salt[0];
    entry[0x0B] = score->salt[1];
    write_be32(entry + 0x0C, score->lines);
    write_be32(entry + 0x10, score->time_in_seconds);
    write_be16(entry + 0x14, score->rank_time_in_seconds);
    write_be16(entry + 0x16, score->rank_lines);
}

int TNT_Vita_SramGetGlobalScore(uint8_t game_type, int index, TNTVitaGlobalScore *score) {
    uint32_t base = tnt_sram_global_base(game_type);
    if (score == NULL || index < 0 || index >= 5 || base == UINT32_MAX)
        return -1;
    tnt_sram_read_global_at(base, index, score);
    return 0;
}

int TNT_Vita_SramUpdatePlayer(int slot, const TNTVitaStoredPlayer *player) {
    uint32_t bitpattern;
    uint8_t encoded[TNT_VITA_SRAM_PLAYER_SIZE];
    if (slot < 0 || slot >= TNT_VITA_SRAM_MAX_PLAYERS || player == NULL)
        return -1;
    bitpattern = TNT_Vita_SramGetBitpattern();
    if ((bitpattern & (1U << (uint32_t)slot)) == 0)
        return -1;
    if (TNT_Vita_PlayerEncodeForTest(player, encoded) != 0)
        return -1;
    memcpy(g_tnt_sram_image + SRAM_OFF_PLAYERS + slot * TNT_VITA_SRAM_PLAYER_SIZE,
           encoded, sizeof(encoded));
    return 0;
}

int TNT_Vita_SramDeletePlayer(int slot) {
    uint32_t bitpattern;
    if (slot < 0 || slot >= TNT_VITA_SRAM_MAX_PLAYERS)
        return -1;
    bitpattern = TNT_Vita_SramGetBitpattern();
    if ((bitpattern & (1U << (uint32_t)slot)) == 0)
        return -1;
    memset(g_tnt_sram_image + SRAM_OFF_PLAYERS +
               slot * TNT_VITA_SRAM_PLAYER_SIZE,
           0, TNT_VITA_SRAM_PLAYER_SIZE);
    bitpattern &= ~(1U << (uint32_t)slot);
    write_be32(g_tnt_sram_image + SRAM_OFF_BITPATTERN, bitpattern);
    return 0;
}

static void tnt_sram_shift_player_scores(uint32_t scores[3][5], int at) {
    int i;
    for (i = 4; i > at; i--) {
        scores[0][i] = scores[0][i - 1];
        scores[1][i] = scores[1][i - 1];
        scores[2][i] = scores[2][i - 1];
    }
}

static void tnt_sram_shift_global_scores(uint32_t base, int at) {
    int i;
    TNTVitaGlobalScore score;
    for (i = 4; i > at; i--) {
        tnt_sram_read_global_at(base, i - 1, &score);
        tnt_sram_write_global_at(base, i, &score);
    }
}

static void tnt_sram_make_global_score(
    TNTVitaGlobalScore *score, const TNTVitaStoredPlayer *player,
    uint32_t total_lines, uint32_t elapsed_seconds
) {
    memset(score, 0, sizeof(*score));
    memcpy(score->name, player->name, 8);
    score->name[8] = '\0';
    score->salt[0] = player->salt[0];
    score->salt[1] = player->salt[1];
    score->lines = total_lines;
    score->time_in_seconds = elapsed_seconds;
    score->rank_time_in_seconds = player->time_in_seconds;
    score->rank_lines = player->lines;
}

int TNT_Vita_SramRecordSinglePlayerResult(
    int slot, uint8_t game_type, uint32_t total_lines,
    uint32_t physical_lines, uint32_t elapsed_seconds
) {
    TNTVitaStoredPlayer player;
    TNTVitaGlobalScore global;
    uint32_t bonus_lines;
    uint32_t base;
    uint32_t cumulative;
    int i;

    if (TNT_Vita_SramGetPlayer(slot, &player) != 1)
        return -1;
    if (physical_lines > total_lines)
        physical_lines = total_lines;
    bonus_lines = total_lines - physical_lines;

    /* FUN_SRAM_8007875c_check_gameover_conditions updates these cumulative
     * rank fields before storing a new leaderboard entry. */
    player.time_in_seconds = (uint16_t)(player.time_in_seconds + elapsed_seconds);
    player.lines = (uint16_t)(player.lines + total_lines);
    if (player.time_in_seconds >= 900U) {
        player.lines = (uint16_t)(player.lines / 3U);
        player.time_in_seconds = (uint16_t)(player.time_in_seconds / 3U);
    }

    cumulative = (uint32_t)player.unkC8 + physical_lines;
    player.unkC8 = (int32_t)(cumulative < 0x40000000U ? cumulative : 0x40000000U);
    cumulative = (uint32_t)player.unkCC + bonus_lines;
    player.unkCC = (int32_t)(cumulative < 0x40000000U ? cumulative : 0x40000000U);

    if (game_type == 0) { /* MARATHON: descending line total. */
        for (i = 0; i < 5; i++) {
            if (player.marathon_scores[0][i] + player.marathon_scores[1][i] < total_lines) {
                tnt_sram_shift_player_scores(player.marathon_scores, i);
                player.marathon_scores[0][i] = physical_lines;
                player.marathon_scores[1][i] = bonus_lines;
                player.marathon_scores[2][i] = elapsed_seconds;
                break;
            }
        }
        base = SRAM_OFF_GLOBAL_MARATHON;
        for (i = 0; i < 5; i++) {
            tnt_sram_read_global_at(base, i, &global);
            if (global.lines < total_lines) {
                tnt_sram_shift_global_scores(base, i);
                tnt_sram_make_global_score(&global, &player, total_lines, elapsed_seconds);
                tnt_sram_write_global_at(base, i, &global);
                break;
            }
        }
    } else if (game_type == 1) { /* SPRINT records only the 180-second result. */
        if (elapsed_seconds >= 179U)
            elapsed_seconds = 180U;
        if (elapsed_seconds >= 180U) {
            for (i = 0; i < 5; i++) {
                if (player.sprint_scores[0][i] + player.sprint_scores[1][i] < total_lines) {
                    tnt_sram_shift_player_scores(player.sprint_scores, i);
                    player.sprint_scores[0][i] = physical_lines;
                    player.sprint_scores[1][i] = bonus_lines;
                    player.sprint_scores[2][i] = elapsed_seconds;
                    break;
                }
            }
            base = SRAM_OFF_GLOBAL_SPRINT;
            for (i = 0; i < 5; i++) {
                tnt_sram_read_global_at(base, i, &global);
                if (global.lines < total_lines) {
                    tnt_sram_shift_global_scores(base, i);
                    tnt_sram_make_global_score(&global, &player, total_lines, elapsed_seconds);
                    tnt_sram_write_global_at(base, i, &global);
                    break;
                }
            }
        }
    } else if (game_type == 2) { /* ULTRA records successful 150-line games. */
        if (total_lines >= 150U) {
            for (i = 0; i < 5; i++) {
                if (player.ultra_scores[2][i] > elapsed_seconds || player.ultra_scores[2][i] == 0U) {
                    tnt_sram_shift_player_scores(player.ultra_scores, i);
                    player.ultra_scores[0][i] = physical_lines;
                    player.ultra_scores[1][i] = bonus_lines;
                    player.ultra_scores[2][i] = elapsed_seconds;
                    break;
                }
            }

            /* Preserve the original Ultra global-table quirk exactly: the
             * comparison controls only the shift, while the write/break occur
             * on the first loop iteration regardless. */
            base = SRAM_OFF_GLOBAL_ULTRA;
            tnt_sram_read_global_at(base, 0, &global);
            if (global.time_in_seconds > elapsed_seconds)
                tnt_sram_shift_global_scores(base, 0);
            tnt_sram_make_global_score(&global, &player, total_lines, elapsed_seconds);
            tnt_sram_write_global_at(base, 0, &global);
        }
    } else {
        return -1;
    }

    return TNT_Vita_SramUpdatePlayer(slot, &player);
}

int TNT_Vita_SramCreatePlayer(const TNTVitaStoredPlayer *player, int *slot_out) {
    uint32_t bitpattern;
    int slot;
    uint8_t encoded[TNT_VITA_SRAM_PLAYER_SIZE];

    if (player == NULL)
        return -1;
    bitpattern = TNT_Vita_SramGetBitpattern();
    for (slot = 0; slot < TNT_VITA_SRAM_MAX_PLAYERS; slot++) {
        if ((bitpattern & (1U << (uint32_t)slot)) == 0)
            break;
    }
    if (slot == TNT_VITA_SRAM_MAX_PLAYERS)
        return -1;
    if (TNT_Vita_PlayerEncodeForTest(player, encoded) != 0)
        return -1;
    memcpy(
        g_tnt_sram_image + SRAM_OFF_PLAYERS + slot * TNT_VITA_SRAM_PLAYER_SIZE,
        encoded, sizeof(encoded));
    bitpattern |= 1U << (uint32_t)slot;
    write_be32(g_tnt_sram_image + SRAM_OFF_BITPATTERN, bitpattern);
    if (slot_out != NULL)
        *slot_out = slot;
    return 0;
}

int TNT_Vita_SramSave(void) {
    uint32_t checksum = sram_checksum(g_tnt_sram_image, SRAM_OFF_CHECKSUM);
    write_be32(g_tnt_sram_image + SRAM_OFF_CHECKSUM, checksum);
#ifdef __vita__
    {
        SceUID fd;
        int copy;
        int wrote;
        (void)sceIoMkdir(TNT_VITA_SRAM_DIR, 0777);
        fd = sceIoOpen(
            TNT_VITA_SRAM_PATH,
            SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC,
            0666);
        if (fd < 0)
            return -1;
        for (copy = 0; copy < SRAM_COPY_COUNT; copy++) {
            wrote = sceIoWrite(fd, g_tnt_sram_image, TNT_VITA_SRAM_SIZE);
            if (wrote != TNT_VITA_SRAM_SIZE) {
                sceIoClose(fd);
                return -1;
            }
        }
        sceIoClose(fd);
    }
#endif
    return 0;
}

int TNT_Vita_SramLoadOrInit(uint32_t initial_game_id) {
#ifdef __vita__
    uint8_t copies[SRAM_COPY_COUNT][TNT_VITA_SRAM_SIZE];
    SceUID fd;
    int got;
    int copy;
    fd = sceIoOpen(TNT_VITA_SRAM_PATH, SCE_O_RDONLY, 0);
    if (fd >= 0) {
        got = sceIoRead(fd, copies, sizeof(copies));
        sceIoClose(fd);
        if (got == (int)sizeof(copies)) {
            for (copy = 0; copy < SRAM_COPY_COUNT; copy++) {
                if (sram_image_valid(copies[copy])) {
                    memcpy(g_tnt_sram_image, copies[copy], TNT_VITA_SRAM_SIZE);
                    return 1;
                }
            }
        }
    }
#endif
    TNT_Vita_SramInitFresh(initial_game_id);
    (void)TNT_Vita_SramSave();
    return 0;
}
