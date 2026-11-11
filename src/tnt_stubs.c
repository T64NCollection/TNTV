#include "vita_compat.h"

s32 D_800CFED4 = 0;

void debug_print_reason_routine(const char *reason, const char *routine) {
    (void)reason;
    (void)routine;
}

#include "tnt_game_types.h"

/* Temporary dependencies for exercising original Tetris_CheckButtons(). */
u8 g_playercount = 1;
u8 g_currentplayer = 0;
u8 D_800CFF50 = 1;

Landfill *g_landfill_ptr = NULL;
Garbage *g_garbage_ptr = NULL;

void debug_print_reason_routine_linenum(const char *reason, const char *routine, s32 line) {
    (void)reason;
    (void)routine;
    (void)line;
}

/* Deferred systems used by LineScan. */


/*
 * FrameAct compatibility for the current single-player Vita bring-up.
 *
 * These match the original branches when:
 * - garbage is disabled
 * - no AI player is active
 */

/*
 * Original game-mode/frame timing globals.
 *
 * D_800CFEE8 starts at 15 in the original executable.
 * Vita currently advances gameplay exactly one 60 Hz tick per update.
 */
u8 D_800CFEE8 = 15;
u32 D_801109F4 = 1;

/* Original Board_Update only services the BoardPieces visual update queue. */
void Board_Update(Board *board_ptr, s32 arg1) {
    (void)board_ptr;
    BoardPieces_Update(g_boardPieces_ptr, arg1);
}


void debug_print2(const char *arg0, const char *arg1) {
    (void)arg0;
    (void)arg1;
}
