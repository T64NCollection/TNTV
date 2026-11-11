#include "vita_compat.h"
#include "tnt_game_types.h"
#include "tntv_rom_bridge.h"
#include <string.h>

Tetris *g_tetris_ptr;

static void Tetris_CheckButtons(void);
static void Tetris_SetMarathonHandicap(Tetris *, GameVars *);
static void Tetris_SetSprintHandicap(Tetris *, GameVars *);
static void Tetris_SetUltraHandicap(Tetris *, GameVars *);
static void Tetris_SetGameHandicap(Tetris *, GameVars *);

_Static_assert(sizeof(BoardPieces_Min) <= sizeof(((Board *)0)->pad33A8),
    "embedded BoardPieces_Min exceeds reserved Board storage");

void TNT_Vita_TetrisSetGlobalPointers(Tetris *tetris_ptr, u8 playerNum) {
  if (tetris_ptr == NULL || playerNum >= 4) {
    return;
  }

  g_currentplayer = playerNum;
  g_PV_ptr = &g_PV_arr[playerNum];
  g_tetris_ptr = tetris_ptr;

  g_frameAct_ptr = &tetris_ptr->frameAct;
  g_minos_ptr = &tetris_ptr->minos;
  g_lineScan_ptr = &tetris_ptr->lineScan;
  g_squares_ptr = &tetris_ptr->squares;
  D_8011FBC0 = &tetris_ptr->unk768;
  g_gameStats_ptr = &tetris_ptr->gameStats;
  g_garbage_ptr = (Garbage *)tetris_ptr->garbage_ptr;

  g_mobileCubes_ptr = &tetris_ptr->board.mobileCubes;
  g_fallingCubes_ptr = &tetris_ptr->board.fallingCubes;
  g_next_cell_id_ptr = &tetris_ptr->board.next_cell_id;
  g_boardPieces_ptr = (BoardPieces_Min *)(void *)tetris_ptr->board.pad33A8;
  g_currentPiece_ptr = &tetris_ptr->board.currentPiece;
  g_nextPieces_ptr = &tetris_ptr->board.nextPieces;
}

static void Tetris_CheckButtons(void) {
  register Tetris *tetris_ptr = g_tetris_ptr;
  register UnkStruct_1 *temp_s1;
  register u8 currentplayer;
  register CurrentPiece *currentPiece_ptr;

  if (tetris_ptr->unk4 > 0) {
    tetris_ptr->unk4--;
  } else {
    temp_s1 = &g_PV_ptr->unk28;
    currentplayer = g_currentplayer;
    currentPiece_ptr = &tetris_ptr->board.currentPiece;
    if (currentPiece_ptr->state == 1) {
      if (temp_s1->unk8C & 0x20) {           // L_TRIG / CONT_L
        PieceHold_Swap(&tetris_ptr->pieceHold, currentPiece_ptr);
        Audio2_Play_SFX(&D_801235B0, &g_gameSfxBank, SFX_GAME_1);
      }
      if (temp_s1->unk84 & 0x200) {          // L_JPAD / CONT_LEFT
        currentPiece_ptr->possibleMoves |= 0x1;
      }
      if (temp_s1->unk84 & 0x100) {          // R_JPAD / CONT_RIGHT
        currentPiece_ptr->possibleMoves |= 0x2;
      }
      if (temp_s1->unk84 & 0x8000) {         // A_BUTTON / CONT_A
        currentPiece_ptr->possibleMoves |= 0x8;
      }
      if (temp_s1->unk84 & 0x4000) {         // B_BUTTON / CONT_B
        currentPiece_ptr->possibleMoves |= 0x10;
      }
      if (!(temp_s1->unk88 & 0x200)) {       // L_JPAD / CONT_LEFT
        currentPiece_ptr->possibleMoves &= ~0x1;
      }
      if (!(temp_s1->unk88 & 0x100)) {       // R_JPAD / CONT_RIGHT
        currentPiece_ptr->possibleMoves &= ~0x2;
      }
      if (!(temp_s1->unk88 & 0x8000)) {      // A_BUTTON / CONT_A
        currentPiece_ptr->possibleMoves &= ~0x8;
      }
      if (!(temp_s1->unk88 & 0x4000)) {      // B_BUTTON / CONT_B
        currentPiece_ptr->possibleMoves &= ~0x10;
      }
      if (temp_s1->unk8C & 0x400) {          // D_JPAD / CONT_DOWN
        currentPiece_ptr->fallVelocity = 0x100;
        Audio2_Play_SFX(&D_801235B0, &g_gameSfxBank, SFX_GAME_14);
      }
      if (!(temp_s1->unk88 & 0x400)) {       // D_JPAD / CONT_DOWN
        currentPiece_ptr->fallVelocity = currentPiece_ptr->fallVelocityCopy;
      }
      if (temp_s1->unk8C & 0x800) {          // U_JPAD / CONT_UP
        CurrentPiece_800676ac_fourliner_looper(currentPiece_ptr);
        Audio2_Play_SFX(&D_801235B0, &g_gameSfxBank, SFX_GAME_14);
      }
    }
    if ((g_playercount >= 3) && (g_landfill_ptr->type == LANDFILLTYPE_DIRECTED)) {
      if (temp_s1->unk8C & 0x2) {            // L_CBUTTONS / CONT_C
        func_80072508(currentplayer);
      }
      /* Vita Triangle is C-up and Square is C-left. Reuse C-up as
       * right-target ONLY when 3P DIRECTED; retail C-right stays valid. */
      if (temp_s1->unk8C & (CONT_F | CONT_E)) { // Triangle/C-right
        func_8007257C(currentplayer);
      }
    }
  }
}

void Tetris_FUN_80054220_fourliner(Tetris *tetris_ptr, u8 arg1) {
  tetris_ptr->unk3 = arg1;
  if (!arg1) {
    tetris_ptr->unk4 = 1;
  }
}

u8 Tetris_Update(Tetris *tetris_ptr) {
  register s32 i;

  tetris_ptr->unk1 = 0;
  if (tetris_ptr->unk0 == 0) {
    tetris_ptr->unk1 = 1;
    return 1;
  }
  if (tetris_ptr->unk2 && (D_800CFEE8 != 12)) {
    Tetris_CheckButtons();
  }
  FrameAct_ScheduleEvents(&tetris_ptr->frameAct, D_801109F4);
  debug_print2(TNTV_ROM_OriginalText(5U), TNTV_ROM_OriginalText(6U));
  if (g_frameAct_ptr->unk1 == 1) {
    tetris_ptr->unk0 = 0;
  }
  if (tetris_ptr->unk2 && !tetris_ptr->unk3) {
    PieceHold_Update(&tetris_ptr->pieceHold, D_801109F4);
    debug_print2(TNTV_ROM_OriginalText(5U), TNTV_ROM_OriginalText(7U));
    Board_Update(&tetris_ptr->board, D_801109F4);
    debug_print2(TNTV_ROM_OriginalText(5U), TNTV_ROM_OriginalText(8U));
    GameStats_Update(&tetris_ptr->gameStats, D_801109F4);
    debug_print2(TNTV_ROM_OriginalText(5U), TNTV_ROM_OriginalText(9U));
    for (i = D_801109F4; i > 0; i--) {
      FallingCubes_Update();
      debug_print2(TNTV_ROM_OriginalText(5U), TNTV_ROM_OriginalText(10U));
      MobileCubes_Update();
      debug_print2(TNTV_ROM_OriginalText(5U), TNTV_ROM_OriginalText(11U));
    }
    Multisquares_UpdateGlows(&tetris_ptr->squares, D_801109F4);
    debug_print2(TNTV_ROM_OriginalText(5U), TNTV_ROM_OriginalText(12U));
  }
  if (tetris_ptr->unk3) {
    tetris_ptr->unk1 = 2;
  }
  if (tetris_ptr->frameAct.unk3) {
    tetris_ptr->unk1 = 1;
  }
  return tetris_ptr->unk1;
}

void Tetris_Render(Tetris *tetris_ptr) {
  GameCamera_Render();
  debug_print2("AFTER", TNTV_ROM_OriginalText(13U));
  MobileCubes_Render();
  debug_print2("AFTER", TNTV_ROM_OriginalText(14U));
  CurrentPiece_Render(&tetris_ptr->board.currentPiece);
  debug_print2("AFTER", TNTV_ROM_OriginalText(15U));
  PieceHold_Render(&tetris_ptr->pieceHold);
  debug_print2("AFTER", TNTV_ROM_OriginalText(16U));
  NextPieces_Render();
  debug_print2("AFTER", TNTV_ROM_OriginalText(17U));
  GameStats_Render(&tetris_ptr->gameStats);
  debug_print2("AFTER", TNTV_ROM_OriginalText(18U));
  Multisquares_RenderGlows(&tetris_ptr->squares);
  debug_print2("AFTER", TNTV_ROM_OriginalText(19U));
  FrameAct_Render(&tetris_ptr->frameAct);
  debug_print2("AFTER", TNTV_ROM_OriginalText(20U));
}

static void Tetris_SetMarathonHandicap(Tetris *tetris_ptr, GameVars *gameVars_ptr) {
  register s32 lowestScanableLine;
  register s32 y;
  register s32 x;

  lowestScanableLine = 19 - D_800CFF00[gameVars_ptr->handicap];
  if (lowestScanableLine != 19) {
    LineScan_80069f10_threeliner_lowestscanableline(lowestScanableLine);
    for (y = lowestScanableLine + 1; y < 20; y++) {
      for (x = 0; x < 10; x++) {
        BoardP_8006de40_nineliner(x, y, 6);
      }
    }
    FrameAct_80053F40_oneliner(&tetris_ptr->unk6840, 0);
  }
}

static void Tetris_SetSprintHandicap(Tetris *tetris_ptr, GameVars *gameVars_ptr) {
  register s32 lowestScanableLine;
  register s32 y;
  register s32 x;

  lowestScanableLine = 19 - D_800CFF00[gameVars_ptr->handicap];
  if (lowestScanableLine != 19) {
    LineScan_80069f10_threeliner_lowestscanableline(lowestScanableLine);
    for (y = lowestScanableLine + 1; y < 20; y++) {
      for (x = 0; x < 10; x++) {
        BoardP_8006de40_nineliner(x, y, 6);
      }
    }
    FrameAct_80053F40_oneliner(&tetris_ptr->unk6840, 0);
  }
}

static void Tetris_SetUltraHandicap(Tetris *tetris_ptr, GameVars *gameVars_ptr) {
  tetris_ptr->ultraGoal = gameVars_ptr->handicap * 10 + 150;
  FrameAct_80053F40_oneliner(&tetris_ptr->unk6840, 0);
}

static void Tetris_SetGameHandicap(Tetris *tetris_ptr, GameVars *gameVars_ptr) {
  switch (gameVars_ptr->gameType) {
  case GAMETYPE_MARATHON:
    Tetris_SetMarathonHandicap(tetris_ptr, gameVars_ptr);
    break;
  case GAMETYPE_SPRINT:
    Tetris_SetSprintHandicap(tetris_ptr, gameVars_ptr);
    break;
  case GAMETYPE_ULTRA:
    Tetris_SetUltraHandicap(tetris_ptr, gameVars_ptr);
    break;
  }
}

void Tetris_Init(Tetris *tetris_ptr, GameVars *gameVars_ptr) {
  u32 seed = gameVars_ptr->seed;
  register u8 currentplayer;
  register u8 temp_s1;
  Point boardlocation;

  tetris_ptr->unk0 = 1;
  tetris_ptr->unk2 = TRUE;
  tetris_ptr->unk3 = FALSE;
  tetris_ptr->unk4 = 1;
  Bag63_Init(&tetris_ptr->bag63, &seed);
  Minos_Init(&tetris_ptr->minos, gameVars_ptr->unk4);
  PFGFX_Playfield_Init(gameVars_ptr->screen);
  Board_Init(&tetris_ptr->board, &tetris_ptr->bag63, &tetris_ptr->minos);
  PieceHold_Init(&tetris_ptr->pieceHold, &seed, 10, 20, gameVars_ptr->screen);
  Multisquare_8006ada4_Init();
  LineScan_Init();
  Tetris_SetGameHandicap(tetris_ptr, gameVars_ptr);
  GameStats_Init(&tetris_ptr->gameStats);
  PFGFX_Init(&boardlocation, &tetris_ptr->pfGfx);
  BoardInfo_Init(&tetris_ptr->boardInfo, boardlocation, g_minos_ptr->unk2 >> 8);
  FrameAct_Init(&tetris_ptr->frameAct, &tetris_ptr->pieceHold, &tetris_ptr->unk6840, &tetris_ptr->boardInfo);
  tetris_ptr->garbage_ptr = Landfill_Garbage();
  g_garbage_ptr = tetris_ptr->garbage_ptr;
  Garbage_Init(tetris_ptr->garbage_ptr, &seed);
  switch (g_landfill_ptr->type) {
  case LANDFILLTYPE_NONE:
    Garbage_80072e2c_set_arg0p_to_arg1(g_garbage_ptr, FALSE);
    break;
  case LANDFILLTYPE_DIRECTED:
    Garbage_80072e2c_set_arg0p_to_arg1(g_garbage_ptr, TRUE);
    currentplayer = g_currentplayer;
    Landfill_800723a0_calls_garbage_fun(currentplayer, currentplayer, FALSE);
    Landfill_8007240c_fiveliner(currentplayer, (currentplayer + 1) % g_playercount);
    if (g_playercount == 2) {
      if (g_currentplayer == 0) {
        Landfill_8007240c_fiveliner(0, 1);
      } else {
        Landfill_8007240c_fiveliner(1, 0);
      }
    }
    break;
  case LANDFILLTYPE_HOTPOTATO:
    Garbage_80072e2c_set_arg0p_to_arg1(g_garbage_ptr, TRUE);
    temp_s1 = g_landfill_ptr->unk30.unk0;
    Landfill_8007240c_fiveliner(g_currentplayer, temp_s1);
    break;
  default:
    debug_print_reason_routine_linenum("bad type", "tetris.c", 504);
    break;
  }
  tetris_ptr->gameStats.elapsedTime = 0;
  // sets initial state (to 1) for FrameAct_ScheduleEvents
  FrameAct_set_state(1);
}

void Tetris_Deinit(Tetris *tetris_ptr) {
  FrameAct_Deinit_doesnothing(&tetris_ptr->unk6840);
  FrameAct_Deinit(&tetris_ptr->frameAct);
  PieceHold_Deinit(&tetris_ptr->pieceHold);
  Board_Deinit(&tetris_ptr->board);
  Minos_Deinit_doesnothing(&tetris_ptr->minos);
  Garbage_Deinit(tetris_ptr->garbage_ptr);
  Multisquare_Deinit_doesnothing();
  Bag63_Deinit(&tetris_ptr->bag63);
  GameStats_Deinit(&tetris_ptr->gameStats);
}
