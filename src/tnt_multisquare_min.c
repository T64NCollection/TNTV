#include "vita_compat.h"
#include "tnt_game_types.h"

Squares *g_squares_ptr;
s32 g_tnt_vita_last_square_probe = 0;

void Multisquare_8006ada4_Init(void) {
    Squares *squares_ptr = g_squares_ptr;
    Square *square_ptr;
    u32 i;

    squares_ptr->cnt = 0;

    square_ptr = squares_ptr->arr;
    for (i = 0; i < 44; i++, square_ptr++) {
        square_ptr->id = i;
        square_ptr->type = SQUARETYPE_NONE;
        square_ptr->unk3 = FALSE;
    }
}


static s32 Multisquare_8006aba4_loops_44_times_1(void) {
    Squares *squares_ptr = g_squares_ptr;
    Square *square_ptr;
    u32 i;
    s32 count = 0;

    square_ptr = squares_ptr->arr;

    for (i = 0; i < 44; i++, square_ptr++) {
        if (square_ptr->unk3) {
            count++;
        }
    }

    return count;
}

static void Multisquare_8006abf4_loops_44_times_2(void) {
    Squares *squares_ptr = g_squares_ptr;
    Square *square_ptr;
    u32 i;

    square_ptr = squares_ptr->arr;

    for (i = 0; i < 44; i++, square_ptr++) {
        square_ptr->unk3 = FALSE;
    }
}

s32 Multisquare_8006ac2c_loops_10_times(s32 row, s32 type) {
    Cell **cell_ptr_ptr;
    Cell *cell_ptr;
    Square *square_ptr;
    s32 col;
    u8 square_id;

    Multisquare_8006abf4_loops_44_times_2();

    cell_ptr_ptr = &g_boardPieces_ptr->cell_ptr_arr[row * 10];

    for (col = 0; col < 10; col++) {
        cell_ptr = *cell_ptr_ptr;
        square_id = cell_ptr->square_id;

        if (Multisquare_isArg0_lessthan_44(square_id)) {
            square_ptr = &g_squares_ptr->arr[square_id];

            if (square_ptr->type == type) {
                square_ptr->unk3 = TRUE;
            }
        }

        cell_ptr_ptr++;
    }

    return Multisquare_8006aba4_loops_44_times_1();
}


static Square *Multisquare_8006a050_extralarge_manyloops_interesting(
    s32 x,
    s32 y,
    u8 square_type
) {
    Square *square_ptr;
    s32 i;
    s32 col;
    s32 row;
    u8 square_id;
    Color *target_color_ptr;
    Point glow_location;
    PointAngle glow_source;
    Cell *base_cell;
    Cell *cell_ptr;
    Cell **cell_ptr_ptr;

    for (
        i = 0, square_ptr = g_squares_ptr->arr;
        i < 44 && square_ptr->type != SQUARETYPE_NONE;
        i++, square_ptr++
    ) {
    }

    if (i == 44) {
        return NULL;
    }

    square_ptr->type = square_type;
    square_id = square_ptr->id;
    target_color_ptr = (square_type == SQUARETYPE_MONO)
        ? &g_color_gold
        : &g_color_silver;

    /* Original multisquare glow anchor: convert the square's upper-left
     * board point through the current one-player Minos transform.
     */
    setPointAngle(glow_source, (x << 8) - 0x80, (y << 8) - 0x80, 0);
    Minos_80071100_fiveliner_nuts2(&glow_location, &glow_source);
    MultisquareGlow_Init(
        &square_ptr->glow, glow_location, (u8)(g_minos_ptr->unk2 >> 8));

    for (row = y; row < y + 4; row++) {
        for (col = x; col < x + 4; col++) {
            cell_ptr =
                g_boardPieces_ptr->cell_ptr_arr[col + (row * 10)];

            cell_ptr->square_id = square_id;

            /* Original multisquare creation morphs every Cube Mino to the
             * square color over 16 frames while its CubeTile blend advances.
             */
            if (cell_ptr->cubeInfo.cube_ptr != NULL) {
                Mino *mino_ptr = &cell_ptr->cubeInfo.cube_ptr->mino;
                mino_ptr->unk22 = (0xFF - mino_ptr->unk20) / 16;
                Minos_80070860_fortyliner(
                    mino_ptr, target_color_ptr, 16);
            }
        }
    }

    cell_ptr_ptr =
        &g_boardPieces_ptr->cell_ptr_arr[x + (y * 10)];

    base_cell = *cell_ptr_ptr;

    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            BoardP_8006d450_fourteenliner_pf_logic_cell_loops(
                base_cell,
                *cell_ptr_ptr
            );
            BoardP_8006e64c_calls_UpdateQueue_AddEntry(
                (*cell_ptr_ptr)->cubeInfo.pos.x,
                (*cell_ptr_ptr)->cubeInfo.pos.y
            );

            cell_ptr_ptr++;
        }

        cell_ptr_ptr += 6;
    }

    square_ptr->unk1 = 16;
    g_squares_ptr->cnt++;

    if (square_type == SQUARETYPE_MULTI) {
        Audio2_Play_SFX(&D_801235B0, &g_gameSfxBank, SFX_GAME_3);
    } else if (square_type == SQUARETYPE_MONO) {
        Audio2_Play_SFX(&D_801235B0, &g_gameSfxBank, SFX_GAME_3);
        Audio2_Play_SFX(&D_801235B0, &g_gameSfxBank, SFX_GAME_4);
    }

    return square_ptr;
}


void Multisquare_8006a410_fourliner(
    Square *square_ptr,
    u8 *cell_id_ptr
) {
    (void)cell_id_ptr;

    if (square_ptr->type == SQUARETYPE_NONE) {
        return;
    }

    square_ptr->unk1--;

    if (square_ptr->unk1 == 0) {
        square_ptr->type = SQUARETYPE_NONE;

        if (g_squares_ptr->cnt > 0) {
            g_squares_ptr->cnt--;
        }
    }
}

static s32 Multisquares_Test4x4byType(s32 x, s32 y, u8 piece_type) {
    s32 i;
    Cell **cell_ptr_ptr;
    Cell *cell_ptr;
    u8 is_monosquare;
    u8 piece_ids[4];
    u8 *piece_id_ptr;
    u8 piece_id;

    if (x < 0 || y < 0 || x >= 7 || y >= 17) {
        return SQUARETYPE_NONE;
    }

    piece_id_ptr = piece_ids;
    for (i = 4; i > 0; i--) {
        *piece_id_ptr++ = 0;
    }

    is_monosquare = TRUE;
    cell_ptr_ptr = &g_boardPieces_ptr->cell_ptr_arr[x + (y * 10)];

    for (y = 4; y > 0; y--) {
        for (x = 4; x > 0; x--) {
            cell_ptr = *cell_ptr_ptr;

            if (cell_ptr->properties & 3) {
                return SQUARETYPE_NONE;
            }

            if (cell_ptr->square_id != 0xFF) {
                return SQUARETYPE_NONE;
            }

            if (cell_ptr->piece_type != piece_type) {
                is_monosquare = FALSE;
            }

            piece_id = cell_ptr->piece_id;

            for (i = 0;;) {
                if (piece_ids[i] == 0) {
                    piece_ids[i] = piece_id;
                    break;
                }

                if (piece_ids[i] == piece_id) {
                    break;
                }

                if (++i == 4) {
                    return SQUARETYPE_NONE;
                }
            }

            cell_ptr_ptr++;
        }

        cell_ptr_ptr += 6;
    }

    if (is_monosquare) {
        return SQUARETYPE_MONO;
    }

    return SQUARETYPE_MULTI;
}

static void Multisquare_8006a6f0_threeliner_sets_arg0_stuff(
    Squares *squares_ptr
) {
    squares_ptr->delay = 45;
    squares_ptr->unk0 = 2;
    squares_ptr->unk1 = 0;
}


static s32 TNT_Vita_Probe4x4At(s32 x, s32 y, u8 piece_type) {
    u8 piece_ids[16];
    s32 unique_count = 0;
    s32 row;
    s32 col;
    s32 mono = TRUE;

    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            Cell *cell_ptr =
                g_boardPieces_ptr->cell_ptr_arr[
                    (x + col) + ((y + row) * 10)
                ];
            s32 i;
            s32 found = FALSE;

            if (cell_ptr->piece_type == EMPTY_CELL) {
                return 0;
            }

            if (cell_ptr->properties & 3) {
                return 1;
            }

            if (cell_ptr->square_id != 0xFF) {
                return 2;
            }

            if (cell_ptr->piece_type != piece_type) {
                mono = FALSE;
            }

            for (i = 0; i < unique_count; i++) {
                if (piece_ids[i] == cell_ptr->piece_id) {
                    found = TRUE;
                    break;
                }
            }

            if (!found) {
                piece_ids[unique_count++] = cell_ptr->piece_id;

                if (unique_count > 4) {
                    return 3;
                }
            }
        }
    }

    return mono ? 5 : 4;
}

static void Multisquare_8006a740_thirtyfiveliner(Squares *squares_ptr) {
    BoundingBox bbox;
    s16 x_start;
    s16 y_start;
    s16 x_end;
    s16 y_end;
    s32 x;
    s32 y;
    u8 square_type = SQUARETYPE_NONE;

    CurrentPiece_80067c1c_fifteenliner_loops3times(
        g_currentPiece_ptr,
        &bbox
    );

    x_start = bbox.x_max - 3;
    y_start = bbox.y_max - 3;
    x_end = bbox.x_min;
    y_end = bbox.y_min;

    if (x_start < 0) x_start = 0;
    if (x_end > 6) x_end = 6;
    if (y_start < 0) y_start = 0;
    if (y_end > 16) y_end = 16;

    g_tnt_vita_last_square_probe = 0;

    for (y = y_start; y <= y_end; y++) {
        for (x = x_start; x <= x_end; x++) {
            s32 probe =
                TNT_Vita_Probe4x4At(
                    x,
                    y,
                    g_currentPiece_ptr->pieceType
                );

            if (probe > g_tnt_vita_last_square_probe) {
                g_tnt_vita_last_square_probe = probe;
            }
        }
    }

    for (y = y_start; y <= y_end; y++) {
        for (x = x_start; x <= x_end; x++) {
            square_type =
                Multisquares_Test4x4byType(
                    x,
                    y,
                    g_currentPiece_ptr->pieceType
                );

            if (square_type == SQUARETYPE_MONO) {
                break;
            }
        }

        if (square_type == SQUARETYPE_MONO) {
            break;
        }
    }

    if (square_type == SQUARETYPE_MONO) {
        Multisquare_8006a050_extralarge_manyloops_interesting(
            x,
            y,
            SQUARETYPE_MONO
        );
        Multisquare_8006a6f0_threeliner_sets_arg0_stuff(squares_ptr);
        return;
    }

    for (y = y_start; y <= y_end; y++) {
        for (x = x_start; x <= x_end; x++) {
            square_type =
                Multisquares_Test4x4byType(
                    x,
                    y,
                    g_currentPiece_ptr->pieceType
                );

            if (square_type == SQUARETYPE_MULTI) {
                break;
            }
        }

        if (square_type == SQUARETYPE_MULTI) {
            break;
        }
    }

    if (square_type == SQUARETYPE_MULTI) {
        Multisquare_8006a050_extralarge_manyloops_interesting(
            x,
            y,
            SQUARETYPE_MULTI
        );
        Multisquare_8006a6f0_threeliner_sets_arg0_stuff(squares_ptr);
        return;
    }

    squares_ptr->unk1 = 1;
}


static s32 Multisquare_8006a6bc_oneliner_calls_fun(Cell *cell_ptr) {
    return Multisquares_Test4x4byType(
        cell_ptr->cubeInfo.pos.x,
        cell_ptr->cubeInfo.pos.y,
        cell_ptr->piece_type
    );
}

static void Multisquare_8006a9f4_twentyliner_loops_t7_t17(
    Squares *squares_ptr
) {
    Cell *cell_ptr;
    s32 col;
    s32 row;

    for (row = 0; row < 17; row++) {
        for (col = 0; col < 7; col++) {
            cell_ptr =
                g_boardPieces_ptr->cell_ptr_arr[col + (row * 10)];

            if (
                cell_ptr->piece_type != EMPTY_CELL &&
                cell_ptr->square_id == 0xFF &&
                Multisquare_8006a6bc_oneliner_calls_fun(cell_ptr) ==
                    SQUARETYPE_MONO
            ) {
                Multisquare_8006a050_extralarge_manyloops_interesting(
                    col,
                    row,
                    SQUARETYPE_MONO
                );
            }
        }
    }

    for (row = 0; row < 17; row++) {
        for (col = 0; col < 7; col++) {
            cell_ptr =
                g_boardPieces_ptr->cell_ptr_arr[col + (row * 10)];

            if (
                cell_ptr->piece_type != EMPTY_CELL &&
                cell_ptr->square_id == 0xFF &&
                Multisquare_8006a6bc_oneliner_calls_fun(cell_ptr) ==
                    SQUARETYPE_MULTI
            ) {
                Multisquare_8006a050_extralarge_manyloops_interesting(
                    col,
                    row,
                    SQUARETYPE_MULTI
                );
            }
        }
    }

    squares_ptr->unk1 = 3;
}

u8 Multisquare_8006ad0c_checks_1_2_3_4(void) {
    Squares *squares_ptr = g_squares_ptr;

    switch (squares_ptr->unk0) {
    case 1:
        Multisquare_8006a740_thirtyfiveliner(squares_ptr);
        break;

    case 2:
        if (squares_ptr->delay == 0) {
            squares_ptr->unk0 = 0;
            squares_ptr->unk1 = 2;
        } else {
            squares_ptr->unk1 = 0;
            squares_ptr->delay--;
        }
        break;

    case 3:
        Multisquare_8006a9f4_twentyliner_loops_t7_t17(
            squares_ptr
        );
        break;

    default:
        squares_ptr->unk1 = 0;
        break;
    }

    return squares_ptr->unk1;
}

s32 TNT_Vita_CountFilled4x4(void) {
    s32 x;
    s32 y;
    s32 row;
    s32 col;
    s32 count = 0;

    for (y = 0; y < 17; y++) {
        for (x = 0; x < 7; x++) {
            s32 full = 1;

            for (row = 0; row < 4 && full; row++) {
                for (col = 0; col < 4; col++) {
                    Cell *cell_ptr =
                        g_boardPieces_ptr->cell_ptr_arr[
                            (x + col) + ((y + row) * 10)
                        ];

                    if (cell_ptr->piece_type == EMPTY_CELL) {
                        full = 0;
                        break;
                    }
                }
            }

            if (full) {
                count++;
            }
        }
    }

    return count;
}

s32 TNT_Vita_Filled4x4Status(void) {
    s32 x;
    s32 y;
    s32 row;
    s32 col;

    for (y = 0; y < 17; y++) {
        for (x = 0; x < 7; x++) {
            u8 piece_ids[16];
            s32 unique_count = 0;
            s32 full = 1;
            s32 mono = 1;
            u8 first_type = EMPTY_CELL;

            for (row = 0; row < 4 && full; row++) {
                for (col = 0; col < 4; col++) {
                    Cell *cell_ptr =
                        g_boardPieces_ptr->cell_ptr_arr[
                            (x + col) + ((y + row) * 10)
                        ];
                    s32 i;
                    s32 found = 0;

                    if (cell_ptr->piece_type == EMPTY_CELL) {
                        full = 0;
                        break;
                    }

                    if (first_type == EMPTY_CELL) {
                        first_type = cell_ptr->piece_type;
                    } else if (cell_ptr->piece_type != first_type) {
                        mono = 0;
                    }

                    if (cell_ptr->properties & 3) {
                        return 1;
                    }

                    if (cell_ptr->square_id != 0xFF) {
                        return 2;
                    }

                    for (i = 0; i < unique_count; i++) {
                        if (piece_ids[i] == cell_ptr->piece_id) {
                            found = 1;
                            break;
                        }
                    }

                    if (!found) {
                        piece_ids[unique_count++] = cell_ptr->piece_id;

                        if (unique_count > 4) {
                            return 3;
                        }
                    }
                }
            }

            if (full) {
                return mono ? 5 : 4;
            }
        }
    }

    return 0;
}
