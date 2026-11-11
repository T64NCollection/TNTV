#include "vita_compat.h"
#include "tnt_game_types.h"

u8 FUN_8004191c_sixliner_loop_shift_right(u8 arg0, u8 arg1) {
  register s8 i;

  for (i = arg1; i >= 0; i--) {
    if (D_800E1F30->unk0[arg0 + (i * 10)].unk0 != EMPTY_CELL) {
      return FALSE;
    }
  }

  return TRUE;
}

u8 FUN_8004199c_twentyliner_loop(u8 arg0, u8 *arg1, u8 arg2, u8 arg3) {
  register u8 var_s0;
  register s8 var_s1;
  register s8 temp_s2;
  register s8 temp_s3;
  register s8 temp_s4;
  UnkStruct_105 sp40;

  temp_s2 = g_currentPiece_ptr->logicalPos.x;
  temp_s3 = g_currentPiece_ptr->logicalPos.y;
  temp_s4 = g_currentPiece_ptr->logicalRotCkwState;

  var_s0 = FALSE;
  for (var_s1 = *arg1; var_s1 >= 0; var_s1--) {
    FUN_005580_8003f760_twoliner_calls_fun(&sp40, arg0, var_s1, arg2, 0);
    var_s0 = FUN_800416cc_fourliner(arg0, var_s1, arg2) && FUN_005580_800400fc_largefun_more_currpiece_stuff(&sp40, arg3);
    if (var_s0) {
      *arg1 = var_s1;
      break;
    }
  }

  g_currentPiece_ptr->logicalPos.x = temp_s2;
  g_currentPiece_ptr->logicalPos.y = temp_s3;
  g_currentPiece_ptr->logicalRotCkwState = temp_s4;

  return var_s0;
}

u8 FUN_80041ab4_tenliner_loop(u8 arg0, u8 arg1) {
  register u8 var_a2;

  if (arg1 == 20) {
    return TRUE;
  }

  for (var_a2 = arg0; var_a2 < (arg0 + 4); var_a2++) {
    if (D_800E1F30->unk0[var_a2 + (arg1 * 10)].unk0 == EMPTY_CELL) {
      return FALSE;
    }
  }

  return TRUE;
}

void FUN_80041da0_threeliner(UnkStruct_108 *arg0, UnkStruct_108 *arg1) {
  arg0->unk0 = arg1->unk0;
  arg0->unk1 = arg1->unk1;
  arg0->unk2 = arg1->unk2;
}
