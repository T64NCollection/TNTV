/* User-owned retail ROM offsets and lengths, not original score text. */
typedef struct {uint32_t offset;uint8_t length;} TNTVScoreTextSlice;
static const TNTVScoreTextSlice s_tntv_default_score_offsets[3][5] = {
    {{0x0a4038U,8U},{0x0a4040U,5U},{0x0a407cU,6U},{0x0a406cU,8U},{0x0a4100U,5U}},
    {{0x0a4054U,7U},{0x0a3fe8U,6U},{0x0a4048U,9U},{0x0a4124U,9U},{0x0a4130U,8U}},
    {{0x0a4138U,6U},{0x0a4140U,6U},{0x0a4148U,8U},{0x0a4150U,7U},{0x0a4158U,8U}},
};
