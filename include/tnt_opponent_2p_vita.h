#ifndef TNT_OPPONENT_2P_VITA_H
#define TNT_OPPONENT_2P_VITA_H
#include "vita_compat.h"
#define TNT_OPPONENT_2P_CULTURE_COUNT 7
typedef struct { const u8 *packbits; u32 packbits_size; const u16 *palette; } TNTOpponent2PBackground;
typedef struct { u16 width,height; const u8 *ci8; const u16 *palette; } TNTOpponent2PTimePanel;
extern const TNTOpponent2PBackground g_tnt_opponent_2p_backgrounds[TNT_OPPONENT_2P_CULTURE_COUNT];
extern const TNTOpponent2PTimePanel g_tnt_opponent_2p_time_panels[TNT_OPPONENT_2P_CULTURE_COUNT];
#endif
