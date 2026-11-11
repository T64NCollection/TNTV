#include "vita_compat.h"
#include "tnt_game_types.h"

static BoardPieces_Min g_boardPieces_fallback_storage;
BoardPieces_Min *g_boardPieces_ptr = &g_boardPieces_fallback_storage;

Cell *BoardP_playfield_xy_to_logic_cell_struct_8006e5e0(s32 x, s32 y) {
    register BoardPieces_Min *boardPieces_ptr = g_boardPieces_ptr;

    if ((x < 0) || (x > 9) || (y > 19)) {
        return &boardPieces_ptr->cell_201;
    }

    if (y < 0) {
        return &boardPieces_ptr->cell_200;
    }

    return g_boardPieces_ptr->cell_ptr_arr[x + (y * 10)];
}

void TNT_BoardPiecesMin_Init(void) {
    BoardPieces_Min *boardPieces = g_boardPieces_ptr;
    int i;

    if (boardPieces == NULL)
        return;

    /* BoardPieces now lives inside the active Tetris Board, matching the
     * original per-player layout. Its heap-owned cell/id arrays are released
     * explicitly by TNT_BoardPiecesMin_Deinit before the Tetris is cleared. */
    memset(boardPieces, 0, sizeof(*boardPieces));
    boardPieces->num_cells = 200;
    boardPieces->updQ.maxlen = 600;
    boardPieces->updQ.curlen = 0;
    Ids_Init(&boardPieces->pieceIds, 201);

    /* Original BoardPieces_Init reserves piece ID 0. */
    FUN_80050800_oneliner(&boardPieces->pieceIds, 0);

    boardPieces->cell_list =
        (Cell *)malloc(sizeof(Cell) * 200);

    for (i = 0; i < 200; i++) {
        Cell *cell = &boardPieces->cell_list[i];

        memset(cell, 0, sizeof(*cell));
        cell->properties = 2;
        cell->piece_type = EMPTY_CELL;
        cell->piece_id = 0;
        cell->square_id = 0xFF;
        cell->next = cell;
        Cube_NewCellInfo(&cell->cubeInfo);

        boardPieces->cell_ptr_arr[i] = cell;
    }

    memset(&boardPieces->cell_200, 0, sizeof(boardPieces->cell_200));
    boardPieces->cell_200.properties = 3;
    boardPieces->cell_200.piece_type = EMPTY_CELL;
    boardPieces->cell_200.piece_id = 0;
    boardPieces->cell_200.square_id = 0xFE;
    boardPieces->cell_200.next = &boardPieces->cell_200;
    Cube_NewCellInfo(&boardPieces->cell_200.cubeInfo);

    memset(&boardPieces->cell_201, 0, sizeof(boardPieces->cell_201));
    boardPieces->cell_201.properties = 1;
    boardPieces->cell_201.piece_type = 8;
    boardPieces->cell_201.piece_id = 0;
    boardPieces->cell_201.square_id = 0xFE;
    boardPieces->cell_201.next = &boardPieces->cell_201;
    Cube_NewCellInfo(&boardPieces->cell_201.cubeInfo);
}

void TNT_BoardPiecesMin_Deinit(void) {
    BoardPieces_Min *boardPieces = g_boardPieces_ptr;

    if (boardPieces == NULL)
        return;
    if (boardPieces->cell_list != NULL) {
        free(boardPieces->cell_list);
        boardPieces->cell_list = NULL;
    }
    if (boardPieces->pieceIds.list != NULL) {
        Ids_Deinit(&boardPieces->pieceIds);
        boardPieces->pieceIds.list = NULL;
    }
    boardPieces->num_cells = 0;
    boardPieces->updQ.curlen = 0;
}

u8 BoardP_IsPositionEmpty(s32 x, s32 y) {
    Cell *cell_ptr = BoardP_playfield_xy_to_logic_cell_struct_8006e5e0(x, y);

    if (cell_ptr->piece_type == EMPTY_CELL) {
        return TRUE;
    }

    return FALSE;
}

u8 BoardP_IsPointEmpty(s32 point_x, s32 point_y) {
    s32 pos_x = point_x >> 8;
    s32 pos_y = point_y >> 8;

    if (!BoardP_IsPositionEmpty(pos_x, pos_y)) {
        return FALSE;
    }

    if ((point_x & 0xFF) &&
        !BoardP_IsPositionEmpty(pos_x + 1, pos_y)) {
        return FALSE;
    }

    if (point_y & 0xFF) {
        if (!BoardP_IsPositionEmpty(pos_x, pos_y + 1)) {
            return FALSE;
        }

        if ((point_x & 0xFF) &&
            !BoardP_IsPositionEmpty(pos_x + 1, pos_y + 1)) {
            return FALSE;
        }
    }

    return TRUE;
}

Color *BoardP_8006d3c0_fourliner(Cell *cell_ptr) {
    Square *square_ptr;

    if (!Multisquare_isArg0_lessthan_44(cell_ptr->square_id)) {
        return g_pieceDef_ptr_arr[cell_ptr->piece_type]->color_ptr;
    }

    square_ptr = &g_squares_ptr->arr[cell_ptr->square_id];
    return Multisquare_pGetColor(square_ptr);
}

u8 BoardP_8006d9d8_calls_pf_xy_to_logic_cell_loops(
    Cell *first_cell_ptr, s32 delta_x, s32 delta_y) {
    Cell *cell_ptr;
    Cell *adjacent_cell_ptr;
    s32 x;
    s32 y;
    u8 piece_id = first_cell_ptr->piece_id;

    cell_ptr = first_cell_ptr;

    do {
        x = cell_ptr->cubeInfo.pos.x;
        y = cell_ptr->cubeInfo.pos.y;

        adjacent_cell_ptr =
            BoardP_playfield_xy_to_logic_cell_struct_8006e5e0(
                x + delta_x,
                y + delta_y
            );

        if ((adjacent_cell_ptr->piece_type != EMPTY_CELL) &&
            (adjacent_cell_ptr->piece_id != piece_id)) {
            return FALSE;
        }

        cell_ptr = cell_ptr->next;
    } while (cell_ptr != first_cell_ptr);

    return TRUE;
}

void BoardP_8006d450_fourteenliner_pf_logic_cell_loops(
    Cell *arg0, Cell *arg1) {
    register Cell *cell_ptr;
    register u8 temp_s1 = arg0->piece_id;

    if (temp_s1 == arg1->piece_id) {
        return;
    }

    cell_ptr = arg1;
    do {
        FUN_80050840_fiveliner(&g_boardPieces_ptr->pieceIds,
                               cell_ptr->piece_id);

        cell_ptr->piece_id = temp_s1;

        FUN_80050800_oneliner(&g_boardPieces_ptr->pieceIds,
                              temp_s1);

        cell_ptr = cell_ptr->next;
    } while (cell_ptr != arg1);

    cell_ptr = arg1->next;
    arg1->next = arg0->next;
    arg0->next = cell_ptr;
}

void BoardP_8006de40_nineliner(
    s32 x, s32 y, u8 piece_type) {
    register Cell *cell_ptr;

    if ((x < 0) || (x > 9) || (y < 0) || (y > 19)) {
        return;
    }

    cell_ptr =
        g_boardPieces_ptr->cell_ptr_arr[x + (y * 10)];

    cell_ptr->piece_type = piece_type;

    cell_ptr->piece_id =
        can_run_out_of_ids(&g_boardPieces_ptr->pieceIds);

    FUN_80050800_oneliner(
        &g_boardPieces_ptr->pieceIds,
        cell_ptr->piece_id);

    cell_ptr->properties &= ~2;

    /* Restore the original per-cell Cube allocation and transform state. */
    Cube_Alloc(
        &cell_ptr->cubeInfo,
        x,
        y,
        g_pieceDef_ptr_arr[piece_type]->color_ptr
    );
}

void BoardP_8006df38_twentyfiveliner_3_loops(
    UnkStruct_4 *arg0) {
    register Cell *cell_ptr;
    register Position *minoLockedPos_ptr;
    register u8 numMinos = arg0->numMinos;
    register s32 i;
    register Cell *first_cell_ptr;

    if (numMinos == 0) {
        return;
    }

    for (i = numMinos,
         minoLockedPos_ptr = arg0->minoLockedPos_ptr;
         i > 0;
         i--, minoLockedPos_ptr++) {

        if ((minoLockedPos_ptr->x < 0) ||
            (minoLockedPos_ptr->x > 9) ||
            (minoLockedPos_ptr->y < 0) ||
            (minoLockedPos_ptr->y > 19)) {
            return;
        }
    }

    for (i = numMinos,
         minoLockedPos_ptr = arg0->minoLockedPos_ptr;
         i > 0;
         i--, minoLockedPos_ptr++) {

        BoardP_8006de40_nineliner(
            minoLockedPos_ptr->x,
            minoLockedPos_ptr->y,
            arg0->pieceType);
    }

    if (numMinos > 1) {
        minoLockedPos_ptr = arg0->minoLockedPos_ptr;

        first_cell_ptr =
            g_boardPieces_ptr->cell_ptr_arr[
                minoLockedPos_ptr->x +
                10 * minoLockedPos_ptr->y
            ];

        for (i = numMinos - 1, minoLockedPos_ptr++;
             i > 0;
             i--, minoLockedPos_ptr++) {

            cell_ptr =
                g_boardPieces_ptr->cell_ptr_arr[
                    minoLockedPos_ptr->x +
                    10 * minoLockedPos_ptr->y
                ];

            BoardP_8006d450_fourteenliner_pf_logic_cell_loops(
                first_cell_ptr,
                cell_ptr);
        }

        /* Canonical BoardP_8006d910 plays one of SFX_GAME_5..7 before
         * refreshing the connected piece's surrounding CubeTile targets. */
        Audio2_Play_SFX(
            &D_801235B0,
            &g_gameSfxBank,
            func_800A35EC(SFX_GAME_5, SFX_GAME_7));

        cell_ptr = first_cell_ptr;
        do {
            BoardP_8006e64c_calls_UpdateQueue_AddEntry(
                cell_ptr->cubeInfo.pos.x, cell_ptr->cubeInfo.pos.y);
            cell_ptr = cell_ptr->next;
        } while (cell_ptr != first_cell_ptr);
    }
}


static void BoardP_8006d998_sixliner_loops(Cell *first_cell_ptr) {
    Cell *cell_ptr;

    if (!(first_cell_ptr->properties & 3)) {
        cell_ptr = first_cell_ptr;

        do {
            cell_ptr->properties |= 1;
            cell_ptr = cell_ptr->next;
        } while (cell_ptr != first_cell_ptr);
    }
}

static void TNT_BoardPiecesMin_DetachCell(Cell *cell_ptr) {
    Cell *prev;

    if (cell_ptr->next == cell_ptr) {
        return;
    }

    FUN_80050840_fiveliner(
        &g_boardPieces_ptr->pieceIds,
        cell_ptr->piece_id
    );

    cell_ptr->piece_id =
        can_run_out_of_ids(&g_boardPieces_ptr->pieceIds);

    FUN_80050800_oneliner(
        &g_boardPieces_ptr->pieceIds,
        cell_ptr->piece_id
    );

    prev = cell_ptr;
    do {
        prev = prev->next;
    } while (prev->next != cell_ptr);

    prev->next = cell_ptr->next;
    cell_ptr->next = cell_ptr;
}

void BoardP_8006d5bc_sixliner_calls_UpdateQueue_AddEntry(Cell *cell_ptr) {
    Square *square_ptr;

    if (Multisquare_isArg0_lessthan_44(cell_ptr->square_id)) {
        square_ptr = &g_squares_ptr->arr[cell_ptr->square_id];

        Multisquare_8006a410_fourliner(
            square_ptr,
            &cell_ptr->cubeInfo.cell_id
        );

        cell_ptr->square_id = 0xFF;
    }

    BoardP_8006d998_sixliner_loops(cell_ptr);
    TNT_BoardPiecesMin_DetachCell(cell_ptr);

    BoardP_8006e64c_calls_UpdateQueue_AddEntry(
        cell_ptr->cubeInfo.pos.x, cell_ptr->cubeInfo.pos.y);
}

u8 BoardP_8006dd64_check_for_nonempty_cell_downwards(s32 y) {
    s32 x;

    for (x = 0; x < 10; x++) {
        if (g_boardPieces_ptr->cell_ptr_arr[(y * 10) + x]->piece_type
            != EMPTY_CELL) {
            return TRUE;
        }
    }

    return FALSE;
}

u8 BoardP_8006ddd0_check_for_nonempty_cell_upwards(s32 y) {
    Cell **cell_ptr_ptr;
    s32 i;

    if (y <= 0)
        return FALSE;

    cell_ptr_ptr = &g_boardPieces_ptr->cell_ptr_arr[y * 10 - 1];
    i = y * 10;
    do {
        if ((*cell_ptr_ptr)->piece_type != EMPTY_CELL)
            return TRUE;
        cell_ptr_ptr--;
    } while (--i != 0);

    return FALSE;
}

s32 BoardP_8006dcd0_eightliner_loops_upto_19_times_increment(void) {
    s32 i = 0;

    do {
        if (BoardP_8006dd64_check_for_nonempty_cell_downwards(i)) {
            return i;
        }
    } while (++i < 19);

    return 19;
}

s32 BoardP_8006dd1c_eightliner_loops_upto_19_times_decrement(void) {
    s32 i = 19;

    do {
        if (BoardP_8006dd64_check_for_nonempty_cell_downwards(i)) {
            return i;
        }
    } while (--i > 0);

    return 0;
}

void BoardPieces_SwapCubes(
    s32 src_x, s32 src_y, s32 dst_x, s32 dst_y) {
    Cell **src_ptr;
    Cell **dst_ptr;
    Cell *tmp;

    if ((src_x < 0) || (src_x > 9) ||
        (src_y < 0) || (src_y > 19)) {
        /* Original N64 debug message omitted; Vita diagnostic stub has no effect. */
        return;
    }

    if ((dst_x < 0) || (dst_x > 9) ||
        (dst_y < 0) || (dst_y > 19)) {
        /* Original N64 debug message omitted; Vita diagnostic stub has no effect. */
        return;
    }

    src_ptr =
        &g_boardPieces_ptr->cell_ptr_arr[src_x + (src_y * 10)];
    dst_ptr =
        &g_boardPieces_ptr->cell_ptr_arr[dst_x + (dst_y * 10)];

    tmp = *src_ptr;
    *src_ptr = *dst_ptr;
    *dst_ptr = tmp;

    Cube_8006e8f8_sets_struct_0x14_to_0x24_to_mostly_zeros(
        &(*src_ptr)->cubeInfo, src_x, src_y);
    Cube_8006e8f8_sets_struct_0x14_to_0x24_to_mostly_zeros(
        &(*dst_ptr)->cubeInfo, dst_x, dst_y);
}

void BoardP_8006d674_tenliner_alloc_fallingcubes(Cell *cell_ptr) {
    Cube **falling_slot;

    if (cell_ptr->piece_type == EMPTY_CELL) {
        return;
    }

    BoardP_8006d5bc_sixliner_calls_UpdateQueue_AddEntry(cell_ptr);

    FUN_80050840_fiveliner(
        &g_boardPieces_ptr->pieceIds,
        cell_ptr->piece_id
    );

    cell_ptr->properties = 2;
    cell_ptr->piece_type = EMPTY_CELL;
    cell_ptr->piece_id = 0;
    cell_ptr->square_id = 0xFF;
    cell_ptr->next = cell_ptr;

    falling_slot = FallingCubes_Alloc();
    if (falling_slot != NULL) {
        *falling_slot = cell_ptr->cubeInfo.cube_ptr;
    } else if (cell_ptr->cubeInfo.cube_ptr != NULL) {
        MobileCubes_Unalloc(cell_ptr->cubeInfo.cube_ptr);
    }

    cell_ptr->cubeInfo.cube_ptr = NULL;
}

void BoardP_8006e288_adds_8_entries_and_swaps_cubes(Cell *cell_ptr) {
    s32 x = cell_ptr->cubeInfo.pos.x;
    s32 y = cell_ptr->cubeInfo.pos.y;

    BoardP_8006d674_tenliner_alloc_fallingcubes(cell_ptr);

    BoardP_8006e64c_calls_UpdateQueue_AddEntry(x - 1, y - 1);
    BoardP_8006e64c_calls_UpdateQueue_AddEntry(x - 1, y);
    BoardP_8006e64c_calls_UpdateQueue_AddEntry(x - 1, y + 1);
    BoardP_8006e64c_calls_UpdateQueue_AddEntry(x, y - 1);
    BoardP_8006e64c_calls_UpdateQueue_AddEntry(x, y + 1);
    BoardP_8006e64c_calls_UpdateQueue_AddEntry(x + 1, y - 1);
    BoardP_8006e64c_calls_UpdateQueue_AddEntry(x + 1, y);
    BoardP_8006e64c_calls_UpdateQueue_AddEntry(x + 1, y + 1);

    for (; y > 0; y--) {
        BoardPieces_SwapCubes(x, y, x, y - 1);
    }
}

void BoardPieces_RemoveLine(s32 dropy) {
    s32 x;
    s32 y;

    if ((dropy < 0) || (dropy > 19)) {
        return;
    }

    for (x = 0; x < 10; x++) {
        BoardP_8006d674_tenliner_alloc_fallingcubes(
            g_boardPieces_ptr->cell_ptr_arr[x + (dropy * 10)]
        );
    }

    for (y = dropy; y > 0; y--) {
        for (x = 0; x < 10; x++) {
            BoardPieces_SwapCubes(x, y, x, y - 1);
        }
    }
}

static void TNT_BoardPiecesMin_SetSurroundings(Cell *cell_ptr) {
    u8 piece_id;
    s32 x;
    s32 y;
    u8 surround_bitfield = 0;

    if (cell_ptr == NULL || cell_ptr->cubeInfo.cube_ptr == NULL ||
        cell_ptr->piece_type == EMPTY_CELL) {
        return;
    }

    x = cell_ptr->cubeInfo.pos.x;
    y = cell_ptr->cubeInfo.pos.y;
    piece_id = cell_ptr->piece_id;

    if (BoardP_playfield_xy_to_logic_cell_struct_8006e5e0(x - 1, y - 1)->piece_id == piece_id) surround_bitfield |= 0x80;
    if (BoardP_playfield_xy_to_logic_cell_struct_8006e5e0(x,     y - 1)->piece_id == piece_id) surround_bitfield |= 0x01;
    if (BoardP_playfield_xy_to_logic_cell_struct_8006e5e0(x + 1, y - 1)->piece_id == piece_id) surround_bitfield |= 0x10;
    if (BoardP_playfield_xy_to_logic_cell_struct_8006e5e0(x - 1, y    )->piece_id == piece_id) surround_bitfield |= 0x08;
    if (BoardP_playfield_xy_to_logic_cell_struct_8006e5e0(x + 1, y    )->piece_id == piece_id) surround_bitfield |= 0x02;
    if (BoardP_playfield_xy_to_logic_cell_struct_8006e5e0(x - 1, y + 1)->piece_id == piece_id) surround_bitfield |= 0x40;
    if (BoardP_playfield_xy_to_logic_cell_struct_8006e5e0(x,     y + 1)->piece_id == piece_id) surround_bitfield |= 0x04;
    if (BoardP_playfield_xy_to_logic_cell_struct_8006e5e0(x + 1, y + 1)->piece_id == piece_id) surround_bitfield |= 0x20;

    Minos_80070818_oneliner_sets_arg0_0x130_to_arg1(
        &cell_ptr->cubeInfo.cube_ptr->mino,
        g_cubeTiles_ptr->unkDC00[surround_bitfield]);
}

void BoardP_8006e64c_calls_UpdateQueue_AddEntry(s32 x, s32 y) {
    Cell *cell_ptr;
    TypedQueue *queue = &g_boardPieces_ptr->updQ;

    if (x < 0 || x > 9 || y < 0 || y > 19 || queue->curlen >= queue->maxlen) {
        return;
    }

    cell_ptr = g_boardPieces_ptr->cell_ptr_arr[x + y * 10];
    if (cell_ptr->piece_type == EMPTY_CELL) {
        return;
    }

    queue->entries[queue->curlen].entry = cell_ptr;
    queue->entries[queue->curlen].type = 1;
    queue->curlen++;
}

void BoardPieces_Update(BoardPieces_Min *boardPieces_ptr, s32 arg1) {
    TypedQueue *queue = &boardPieces_ptr->updQ;
    u16 i;
    (void)arg1;

    for (i = 0; i < queue->curlen; i++) {
        if (queue->entries[i].type == 1) {
            TNT_BoardPiecesMin_SetSurroundings(
                (Cell *)queue->entries[i].entry);
        }
    }
    queue->curlen = 0;
}

u8 TNT_BoardPiecesMin_GetPieceType(s32 x, s32 y) {
    Cell *cell_ptr;

    if ((x < 0) || (x > 9) || (y < 0) || (y > 19)) {
        return EMPTY_CELL;
    }

    cell_ptr = g_boardPieces_ptr->cell_ptr_arr[x + (y * 10)];

    if (cell_ptr == NULL) {
        return EMPTY_CELL;
    }

    return cell_ptr->piece_type;
}
