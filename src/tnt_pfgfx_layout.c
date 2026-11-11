#include "vita_compat.h"
#include "tnt_game_types.h"
#include <string.h>

typedef struct {
    Point unk0;
    Point unk4;
} VitaPFGFXPosPair;

static VitaPFGFXPosPair D_800D01A0[8];

static VitaPFGFXPosPair D_800D01E0[8][2];

/* The exact 64+128 native table bytes occur in the verified USA 1.0 ROM.
 * Decode once during ROM boot rather than ship these original tables. */
void TNTV_Vita_SetPFGFXTables(const unsigned char bytes[192]) {
    if(!bytes)return;
    memcpy(D_800D01A0,bytes,64);
    memcpy(D_800D01E0,bytes+64,128);
}


/*
 * Minimal port of the original one-player PFGFX_Playfield_Init branch.
 * This restores the original Minos coordinate system and NextPieces layout
 * while the N64 PFGFX matrix/background renderer remains deferred.
 */
void PFGFX_VitaInit1PLayout(u8 screen) {
    UnkStruct_17 sp28;
    Point sp1C[3];

    /* Original PFGFX_Sets_x58_x59_Checks_NumPlayers_CurrPlayer(), 1P. */
    const s16 playfield_x = 0x14C;
    const s16 playfield_y = 0x90;

    Minos_80070c40_twoliner_set_OR_1(0xB00);
    Minos_80070c70_threeliner_set_OR_8(0x500, 0xA00);
    Minos_80070cb8_threeliner_set_OR_4(playfield_x, playfield_y);
    Minos_80070a34_twentyliner();

    sp1C[0] = D_800D01A0[screen].unk0;
    sp1C[2] = D_800D01A0[screen].unk4;
    sp1C[1].x = (sp1C[0].x + sp1C[2].x) >> 1;
    sp1C[1].y = (sp1C[0].y + sp1C[2].y) >> 1;

    Minos_8007104c_fiveliner_nuts(
        &sp28.unk0[0], sp1C[0].x << 2, sp1C[0].y << 2);
    Minos_8007104c_fiveliner_nuts(
        &sp28.unk0[1], sp1C[1].x << 2, sp1C[1].y << 2);
    Minos_8007104c_fiveliner_nuts(
        &sp28.unk0[2], sp1C[2].x << 2, sp1C[2].y << 2);
    Minos_8007104c_fiveliner_nuts(
        &sp28.unk0[3], sp1C[2].x << 2, sp1C[2].y << 2);

    sp28.unk10 = 0xFF;
    sp28.unk11 = 0xFF;
    sp28.unk12 = 0xC0;
    sp28.unk14 = 0xC0;

    NextPieces_80068b7c_largeliner_sets_lots_of_struct_elems(&sp28);

    g_gameStats_ptr->linesInfo.x = 295;
    g_gameStats_ptr->linesInfo.y = 183;
}

/* Exact source PFGFX_SetTextDisplayPos_2p branch used by One Player
 * OPPONENT.  This is intentionally dormant until the two-Tetris game loop is
 * enabled; keeping it here lets that future path reuse source transforms. */
void PFGFX_VitaInit2PLayout(u8 screen) {
    UnkStruct_17 sp38;
    Point sp2C[3];
    u8 player = g_currentplayer > 1 ? 1 : g_currentplayer;
    const s16 playfield_x[2] = {0x00E4, 0x03DC};
    const s16 playfield_y = 0x00AC;

    if (screen > 7) screen = 7;
    Minos_80070c40_twoliner_set_OR_1(0xA00);
    Minos_80070c70_threeliner_set_OR_8(0x500, 0xA00);
    Minos_80070cb8_threeliner_set_OR_4(playfield_x[player], playfield_y);
    Minos_80070a34_twentyliner();

    sp2C[0] = D_800D01E0[screen][player].unk0;
    sp2C[2] = D_800D01E0[screen][player].unk4;
    sp2C[1].x = (sp2C[0].x + sp2C[2].x) >> 1;
    sp2C[1].y = (sp2C[0].y + sp2C[2].y) >> 1;

    Minos_8007104c_fiveliner_nuts(&sp38.unk0[0], sp2C[0].x << 2, sp2C[0].y << 2);
    Minos_8007104c_fiveliner_nuts(&sp38.unk0[1], sp2C[1].x << 2, sp2C[1].y << 2);
    Minos_8007104c_fiveliner_nuts(&sp38.unk0[2], sp2C[2].x << 2, sp2C[2].y << 2);
    Minos_8007104c_fiveliner_nuts(&sp38.unk0[3], sp2C[2].x << 2, sp2C[2].y << 2);
    sp38.unk10 = 0xFF;
    sp38.unk11 = 0xFF;
    sp38.unk12 = 0xC0;
    sp38.unk14 = 0xC0;
    NextPieces_80068b7c_largeliner_sets_lots_of_struct_elems(&sp38);

    g_gameStats_ptr->linesInfo.x = 169;
    g_gameStats_ptr->linesInfo.y = player == 0 ? 186 : 217;
}

/* Exact source PFGFX_SetTextDisplayPos_3p branch.  Three- and four-player
 * local games share the compact 8-unit playfield scale; preview pieces are
 * placed in the small common strip while PieceHold itself is offscreen. */
void PFGFX_VitaInit3PLayout(u8 screen) {
    UnkStruct_17 sp28;
    static const s16 playfield_x[3] = {0x0088, 0x01D8, 0x0328};
    u8 player = g_currentplayer > 2 ? 2 : g_currentplayer;
    (void)screen;

    Minos_80070c40_twoliner_set_OR_1(0x800);
    Minos_80070c70_threeliner_set_OR_8(0x500, 0xA00);
    Minos_80070cb8_threeliner_set_OR_4(playfield_x[player], 0x0110);
    Minos_80070a34_twentyliner();

    sp28.unk0[0].x = 0x100; sp28.unk0[0].y = -0x500;
    sp28.unk0[1].x = 0x480; sp28.unk0[1].y = -0x500;
    sp28.unk0[2].x = 0x800; sp28.unk0[2].y = -0x500;
    sp28.unk0[3].x = 0x800; sp28.unk0[3].y = -0x500;
    sp28.unk10 = 0x80;
    sp28.unk11 = 0xFF;
    sp28.unk12 = 0x80;
    sp28.unk14 = 0xD8;
    NextPieces_80068b7c_largeliner_sets_lots_of_struct_elems(&sp28);

    g_gameStats_ptr->linesInfo.x = player == 0 ? 39 : (player == 1 ? 123 : 207);
    g_gameStats_ptr->linesInfo.y = 248;
}

void PFGFX_VitaInit4PLayout(u8 screen) {
    UnkStruct_17 sp28;
    static const s16 playfield_x[4] = {0x0088, 0x01D8, 0x0328, 0x0478};
    static const s16 line_x[4] = {39, 123, 207, 292};
    u8 player = g_currentplayer > 3 ? 3 : g_currentplayer;
    (void)screen;

    Minos_80070c40_twoliner_set_OR_1(0x800);
    Minos_80070c70_threeliner_set_OR_8(0x500, 0xA00);
    Minos_80070cb8_threeliner_set_OR_4(playfield_x[player], 0x0110);
    Minos_80070a34_twentyliner();

    sp28.unk0[0].x = 0x100; sp28.unk0[0].y = -0x500;
    sp28.unk0[1].x = 0x480; sp28.unk0[1].y = -0x500;
    sp28.unk0[2].x = 0x800; sp28.unk0[2].y = -0x500;
    sp28.unk0[3].x = 0x800; sp28.unk0[3].y = -0x500;
    sp28.unk10 = 0xC0;
    sp28.unk11 = 0xFF;
    sp28.unk12 = 0x80;
    sp28.unk14 = 0xD8;
    NextPieces_80068b7c_largeliner_sets_lots_of_struct_elems(&sp28);

    g_gameStats_ptr->linesInfo.x = line_x[player];
    g_gameStats_ptr->linesInfo.y = 248;
}
