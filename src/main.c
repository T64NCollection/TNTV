#include <psp2/types.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/display.h>
#include <psp2/ctrl.h>
#include "vita_input.h"
#include "tnt_game_types.h"
#include "tnt_gamefinish_vita.h"
#include "tnt_pause_vita.h"
#include "tnt_mayan_large_fire.h"
#include "tnt_wonder_vita.h"
#include "tnt_wonder_milestone_vita.h"
#include "tnt_wonder_milestone_gxm.h"
#include "tnt_wonder_complete_vita.h"
#include "tnt_greek_gameplay_vita.h"
#include "tnt_egyptian_gameplay_vita.h"
#include "tnt_celtic_gameplay_vita.h"
#include "tnt_african_gameplay_vita.h"
#include "tnt_japanese_gameplay_vita.h"
#include "tnt_russian_gameplay_vita.h"
#include "tnt_finale_gameplay_vita.h"
#include "tnt_opponent_2p_vita.h"
#include "tnt_multiplayer_menu_data.h"
#include "tnt_multiplayer_4p_data.h"
#include "tnt_haluci_vita.h"
#include "tnt_wonder_hall_vita.h"
#include "tnt_sram_vita.h"
#include "tnt_audio_vita.h"
#include "tnt_intro_vita.h"
#include "tntv_rom_bridge.h"
#include <stdio.h>

/* Test203D HARDWARE BOOT FIX. The original main reserves ~78KB stack;
 * the ROM bridge used to reserve an additional ~39KB per call, then call
 * nested ROM / IMG readers. Do not rely on Vita's implicit default stack.
 * The Vita ELF module builder reads this exported setup symbol.
 */
__attribute__((used)) const SceSize sceUserMainThreadStackSize=0x100000U;

/* One short status breadcrumb retained in the owner's private data folder.
 * This is independent of the original ROM and original game's SRAM/saves.
 * No expensive per-frame logging. Last completed step survives a crash.
 */
static void tntv_startup_trace(const char *stage) {
    FILE *trace=fopen("ux0:data/TNTV/tntv_boot_trace.txt","w");
    if(trace){fputs(stage,trace);fputc('\n',trace);fclose(trace);}
}

#define TNT_INTRO_HANDOFF_FADE_TICKS 6U

extern PlayerVars g_PV_arr[4];
extern PlayerVars *g_PV_ptr;

void FUN_80053538_fiveliner(void);
void FUN_80053500_fiveliner(void);
void PV_set_controller_no(s16 cont_no);
void TNT_BoardPiecesMin_Init(void);
void TNT_BoardPiecesMin_Deinit(void);
u8 TNT_BoardPiecesMin_GetPieceType(s32 x, s32 y);
void BoardPieces_RemoveLine(s32 dropy);
void TNT_Vita_CurrentPieceGetMinoPosition(CurrentPiece *currentPiece_ptr, u8 mino_idx, Position *pos_ptr);
void CurrentPiece_80068090_oneliner_sets_0x538_to_1(CurrentPiece *currentPiece_ptr);
void TNT_Vita_MobilePieceGetMinoPointScaled(
    Piece *piece_ptr, u8 mino_idx, u16 mino_scale, Point *pnt_ptr);
#include <psp2/gxm.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
extern u8 g_tnt_font_c_ia8[7176];
extern u8 g_tnt_mayan_moon_ia8[10206];
extern u8 g_tnt_mayan_playfield_ia8[20808];
extern u8 g_tnt_mayan_background_ci8[120000];
extern u16 g_tnt_mayan_background_rgba16[256];
extern u8 g_tnt_mayan_fire_ci8[5][30];
extern u16 g_tnt_mayan_fire_rgba16[5][256];
extern u8 g_tnt_title_ci8[120000];
extern u16 g_tnt_title_rgba16[256];
extern u8 g_tnt_developer_ci8[120000];
extern u16 g_tnt_developer_rgba16[256];
extern u8 g_tnt_main_menu_background_ci8[120000];
extern u16 g_tnt_main_menu_background_rgba16[256];
extern u8 g_tnt_main_menu_shadow_ia8[67308];
extern u8 g_tnt_main_menu_panel_ci8[65520];
extern u16 g_tnt_main_menu_panel_rgba16[256];
extern u16 g_tnt_main_menu_a_accept_rgba16[1065];
extern u16 g_tnt_onep_a_accept_b_back_rgba16[1980];
extern u8 g_tnt_onep_selection_box_ia8[4620];
extern u8 g_tnt_onep_cursor_ia8[676];
extern u8 g_tnt_onep_shadow_ia8[99540];
extern u8 g_tnt_onep_panel_ci8[99647];
extern u16 g_tnt_onep_panel_rgba16[256];
extern u8 g_tnt_options_panel_ci8[66456];
extern u16 g_tnt_options_panel_rgba16[256];
extern u8 g_tnt_options_shadow_ia8[70720];
extern u8 g_tnt_data_panel_ci8[100584];
extern u16 g_tnt_data_panel_rgba16[256];
extern u8 g_tnt_data_shadow_ia8[110760];
extern u8 g_tnt_modal_rectangle_ia8[30000];
extern u8 g_tnt_audio_panel_ci8[103490];
extern u16 g_tnt_audio_panel_rgba16[256];
extern u8 g_tnt_audio_shadow_ia8[107640];
extern u16 g_tnt_audio_slider_wood_rgba16[3950];
extern u16 g_tnt_audio_slider_feather_rgba16[160];
extern u8 g_tnt_tally_panel_ci8[99898];
extern u8 g_tnt_scores_panel_ci8[105465];
extern u16 g_tnt_scores_panel_rgba16[256];
extern u8 g_tnt_scores_shadow_ia8[110714];
extern u8 g_tnt_scores_gamecart_ia8[832];
extern u8 g_tnt_font_b_ia8[10304];
extern u8 g_tnt_font_b_char_widths[46];
extern u8 g_tnt_credits_background_ci8[30000];
extern u8 g_tnt_credits_background_rgb[768];
extern u8 g_tnt_credits_height_i8[30000];
extern u8 g_tnt_credits_spotlight_ci8[16384];
extern u8 g_tnt_credits_spotlight_rgb[768];
extern u8 g_tnt_credits_font_is_b[192];
extern s16 g_tnt_credits_sintable[1024];
extern const char *g_tnt_credits_lines[181];
extern const u16 g_tnt_credits_line_count;
extern u16 g_tnt_tally_panel_rgba16[256];
extern u8 g_tnt_tally_shadow_ia8[101400];
extern u16 g_tnt_tally_trophy_rgba16[28*30];
_Static_assert(sizeof(Piece) == 0x4F8, "Piece size mismatch");
_Static_assert(sizeof(CubeTile) == 0x80, "CubeTile size mismatch");
_Static_assert(sizeof(Cube) == 0x160, "Cube size mismatch");
_Static_assert(sizeof(MobileCubes) == 0x0C, "MobileCubes size mismatch");
_Static_assert(sizeof(FallingCubes) == 0x08, "FallingCubes size mismatch");
_Static_assert(sizeof(CubeInfo) == 0x08, "CubeInfo size mismatch");
_Static_assert(sizeof(NextPieces) == 0x2920, "NextPieces size mismatch");
_Static_assert(sizeof(Board) == 0x6018, "Board size mismatch");
_Static_assert(sizeof(LineEffect) == 0x30, "LineEffect size mismatch");
_Static_assert(sizeof(LineScan) == 0x3F8, "LineScan size mismatch");
_Static_assert(sizeof(SquareGlow) == 0x0E, "SquareGlow size mismatch");
_Static_assert(sizeof(Square) == 0x12, "Square size mismatch");
_Static_assert(sizeof(Squares) == 0x31C, "Squares size mismatch");
#include <string.h>

#define ALIGN(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 544
#define DISPLAY_STRIDE 1024
#define DISPLAY_BUFFER_COUNT 2
#define DISPLAY_MAX_PENDING_SWAPS 1

/* Fixed native presentation geometry used throughout the current 1P/front-end
 * renderer.  Keeping it at file scope lets the final presentation pass apply
 * the same centered 4:3 surround to every state, including attract-mode paths
 * that previously bypassed the board-specific side masks. */
#define TNT_PRESENT_BOARD_X 267
#define TNT_PRESENT_BOARD_Y 65
#define TNT_PRESENT_CELL 20

#define COLOR(r,g,b) \
    ((uint32_t)(0xFF000000 | ((b) << 16) | ((g) << 8) | (r)))

typedef struct {
    void *data;
    SceGxmSyncObject *sync;
    SceGxmColorSurface surface;
    SceUID uid;
} DisplayBuffer;

typedef struct {
    u8 active;
    u8 selected;
    u8 entrance_ticks;
    u8 leaving;
    u16 idle_ticks;
    u16 pad;
    f32 pulse;
    f32 pulse_rate;
    f32 fade;
} TNTVitaMainMenu;

typedef struct {
    u8 active;
    u8 selected;
    u8 entrance_ticks;
    u8 leaving;
    u8 leave_action;
    u8 pad[3];
    f32 pulse;
    f32 pulse_rate;
    f32 fade;
} TNTVitaOptionsMenu;

typedef struct {
    u8 active;
    u8 selected;       /* 0=TRANSFER NAME, 1=DUMP LINES, 2=DELETE NAME */
    u8 entrance_ticks;
    u8 leaving;
    u8 modal_no_pak;
    u8 name_selecting;
    u8 name_choice;    /* 0=CHOOSE, 1..N=internal SRAM player */
    u8 confirm_delete;
    u8 delete_success;
    u8 profile_count;
    u8 profile_slots[TNT_VITA_SRAM_MAX_PLAYERS];
    char profile_names[TNT_VITA_SRAM_MAX_PLAYERS][9];
    f32 pulse;
    f32 pulse_rate;
    f32 fade;
} TNTVitaDataMenu;

typedef struct {
    u8 active;
    u8 focus;          /* 0=category, 1=GLOBAL/profile selector */
    u8 category;       /* GENERAL, MARATHON, SPRINT, ULTRA */
    u8 entrance_ticks;
    u8 leaving;
    u8 name_selecting;
    u8 player_choice;  /* 0=GLOBAL, 1..N=saved profile */
    u8 saved_choice;
    u8 profile_count;
    u8 profile_slots[TNT_VITA_SRAM_MAX_PLAYERS];
    char profile_names[TNT_VITA_SRAM_MAX_PLAYERS][9];
    f32 pulse;
    f32 pulse_rate;
    f32 fade;
} TNTVitaScoresMenu;

typedef struct {
    u8 active;
    u8 pad;
    u16 angle_a;
    u16 angle_b;
    u16 angle_c;
    u16 pad2;
    u32 scroll_q16;   /* source logical pixels, 16.16 fixed point */
    u32 total_height;
    u64 start_us;
} TNTVitaCredits;

typedef struct {
    u8 active;
    u8 selected;
    u8 entrance_ticks;
    u8 leaving;
    u8 music_level;
    u8 sfx_level;
    u8 music_mode;
    u8 song;
    f32 pulse;
    f32 pulse_rate;
    f32 fade;
} TNTVitaAudioMenu;

typedef struct {
    u8 active;
    u8 selected;
    u8 entrance_ticks;
    u8 leaving;
    u8 opponent;
    u8 game_type;
    u8 stage;           /* TNTV4ALL-only deterministic gameplay screen 0..7 */
    u8 name_entry;       /* 0=GUEST, 1=NEW NAME, 2=created profile */
    u8 name_claimed;     /* original TextList high-nibble player-0 claim */
    u8 name_editing;
    u8 editor_char;      /* D_800D57A4.unk5: 0..44 */
    u8 editor_pos;       /* D_800D57A4.unk4: 0..8 */
    u8 profile_count;
    u8 edit_salt[2];
    char edit_name[9];
    char profile_names[16][9];
    u8 profile_pack[16];
    u8 profile_slot[16];
    u8 profile_salt[16][2];
    f32 pulse;
    f32 pulse_rate;
    f32 fade;
} TNTVitaOnePlayerMenu;

typedef struct {
    u8 active;
    u8 selected;       /* P1 focus: 0=NAME, 1=GAME, 2=GARBAGE, 3=START */
    u8 entrance_ticks;
    u8 leaving;
    u8 game_type;      /* MARATHON / SPRINT / ULTRA */
    u8 garbage_type;   /* current faithful subset: NONE or DIRECTED */
    u8 pad_count;
    u8 profile_count;
    u8 slot_choice[4];     /* 0=GUEST, 1..N=saved SRAM profile */
    u8 slot_confirmed[4];  /* source TextList controller-claim equivalent */
    u8 profile_slot[16];
    char profile_names[16][9];
    f32 pulse;
    f32 pulse_rate;
    f32 fade;
} TNTVitaMultiMenu;

typedef struct {
    u8 active;
    u8 leaving;
    u8 exit_delay;
    u8 rank;             /* row 0 displayed one-based source rank, 1..30 */
    u8 player_count;     /* 1 for solo, 2 for One Player OPPONENT */
    u8 winner_mask;      /* bit 0=row 0, bit 1=row 1 */
    u8 rank2;            /* GUEST starts at rank 1 in the source */
    u8 pad0;
    u16 reveal_ticks;
    u16 idle_ticks;
    u32 lines;
    u32 elapsed_seconds;
    u32 lines2;
    u32 elapsed_seconds2;
    u32 award_lines;     /* source GameResults.unk38 */
    f32 fade;
    char player_name[9];
    char player_name2[9];
    /* Source tally.c renders a row for every participant, spaced 25px. */
    u8 rank3;
    u32 lines3;
    u32 elapsed_seconds3;
    char player_name3[9];
    /* Test92: four-player retail tally; fourth row at source y165. */
    u8 rank4;
    u32 lines4;
    u32 elapsed_seconds4;
    char player_name4[9];
} TNTVitaTally;

typedef struct {
    u8 active;
    u8 state;
    u8 entrance_ticks;
    u8 pad;
    u16 anim_tick;
} TNTVitaWonderViewer;

typedef struct {
    u8 active;
    s8 milestone_index;
    u16 elapsed_ticks;
    u16 block_rotation_x[TNT_WONDER_MILESTONE_MAX_BLOCKS];
} TNTVitaWonderMilestone;

typedef struct {
    u8 active;
    u8 phase;
    u8 wonder;
    u8 finale_prompt;   /* Russian threshold-crossing flow: A = FINALE once */
    u32 phase_ticks;
    u32 story_ticks;
    u32 final_ticks;
} TNTVitaWonderComplete;

typedef struct {
    u8 active;
    u8 wonder;          /* 0..6 maps source finale indices 49..55 */
    u16 pad;
    u32 anim_ticks;
} TNTVitaWonderFinale;

typedef struct {
    u8 active;
    u8 state;
    u8 entrance_ticks;
    u8 current_screen;
    u8 next_screen;
    u8 pad;
    u16 anim_tick;
} TNTVitaWonderHall;

enum {
    TNT_MAIN_MENU_ACTION_NONE = 0,
    TNT_MAIN_MENU_ACTION_ENTER_ONE_PLAYER = 1,
    TNT_MAIN_MENU_ACTION_ENTER_WONDERS = 2,
    TNT_MAIN_MENU_ACTION_ENTER_OPTIONS = 3,
    TNT_MAIN_MENU_ACTION_ENTER_ATTRACT = 4,
    TNT_MAIN_MENU_ACTION_ENTER_MULTI_PLAYER = 5
};

enum {
    TNT_OPTIONS_ACTION_NONE = 0,
    TNT_OPTIONS_ACTION_SCORES = 1,
    TNT_OPTIONS_ACTION_AUDIO = 2,
    TNT_OPTIONS_ACTION_CREDITS = 3,
    TNT_OPTIONS_ACTION_DATA = 4,
    TNT_OPTIONS_ACTION_BACK = 5
};

enum {
    TNT_DATA_ACTION_NONE = 0,
    TNT_DATA_ACTION_BACK = 1
};

enum {
    TNT_AUDIO_ACTION_NONE = 0,
    TNT_AUDIO_ACTION_BACK = 1
};

enum {
    TNT_SCORES_ACTION_NONE = 0,
    TNT_SCORES_ACTION_BACK = 1
};

enum {
    TNT_WONDER_ORIGIN_NONE = 0,
    TNT_WONDER_ORIGIN_ROOT = 1,
    TNT_WONDER_ORIGIN_TALLY = 2,
    TNT_WONDER_ORIGIN_PAUSE = 3
};

enum {
    TNT_ONEP_ACTION_NONE = 0,
    TNT_ONEP_ACTION_START_GAME = 1,
    TNT_ONEP_ACTION_BACK = 2
};

enum {
    TNT_MULTI_ACTION_NONE = 0,
    TNT_MULTI_ACTION_START_GAME = 1,
    TNT_MULTI_ACTION_BACK = 2
};

enum {
    TNT_TALLY_ACTION_NONE = 0,
    TNT_TALLY_ACTION_RETURN_ONE_PLAYER = 1
};

enum {
    TNT_MILESTONE_ACTION_NONE = 0,
    TNT_MILESTONE_ACTION_HALL = 1,
    TNT_MILESTONE_ACTION_EXIT = 2,
    TNT_MILESTONE_ACTION_COMPLETE = 3
};

enum {
    TNT_COMPLETE_ACTION_NONE = 0,
    TNT_COMPLETE_ACTION_HALL = 1,
    TNT_COMPLETE_ACTION_EXIT = 2,
    TNT_COMPLETE_ACTION_FINALE = 3
};

enum {
    TNT_FINALE_ACTION_NONE = 0,
    TNT_FINALE_ACTION_HALL = 1,
    TNT_FINALE_ACTION_EXIT = 2
};

enum {
    TNT_HALL_ACTION_NONE = 0,
    TNT_HALL_ACTION_ACCEPT = 1,
    TNT_HALL_ACTION_BACK = 2
};

static DisplayBuffer buffers[DISPLAY_BUFFER_COUNT];
static unsigned int frontBuffer = 0;
static unsigned int backBuffer = 0;

/* Cached static 1P scene. Building the ROM-derived Mayan layers is relatively
 * expensive in the software framebuffer, so compose them once and memcpy the
 * finished image each frame instead of resampling 400x300 artwork at 60 Hz.
 */
static uint32_t g_tnt_static_scene[DISPLAY_STRIDE * DISPLAY_HEIGHT];
static u32 g_tnt117_background_generation;

/* 3P match: source-background persists across the two framebuffer flips.
 * On entering a new match or returning from an external display owner,
 * initialize both complete buffers before using small rect restores. */
static u8 g_tnt_3p_scene_buffer_ready[DISPLAY_BUFFER_COUNT];
/* Opponent-only static-backdrop persistence, independently tracked for both
 * scanout buffers. Local multiplayer deliberately keeps its approved path. */
static u8 g_tnt_opp60_scene_buffer_ready[DISPLAY_BUFFER_COUNT];
/* Test219D: per-buffer dirty 32x16 pixel tile maps; the actual software
 * draw footprints replace the fixed region assumptions of Test218D. */
#define TNT_OPP60_TILE_X 32
#define TNT_OPP60_TILE_Y 16
#define TNT_OPP60_TILE_ROWS (DISPLAY_HEIGHT / TNT_OPP60_TILE_Y)
static u32 g_tnt_opp60_dirty[DISPLAY_BUFFER_COUNT][TNT_OPP60_TILE_ROWS];
static u8 g_tnt_opp60_dirty_full[DISPLAY_BUFFER_COUNT];
static u8 g_tnt_opp60_tracking;
/* Opponent's two 10px N64 cells map to 200/11 Vita pixels, NOT 18.
 * Independent phases preserve the fractional 400x300 scene offset
 * lost when the image origin is first projected to integer pixels. */
static u8 g_tnt_opp60_exact_board;
static u8 g_tnt_opp60_phase_x;
static u8 g_tnt_opp60_phase_y;
/* Test222D private profiler: 2P boards' locked, preview and other
 * source rasterizer contributions. Only sampled with FPS HUD enabled. */
static u32 g_tnt_opp60_locked_us[2];
static u32 g_tnt_opp60_preview_us[2];
static u32 g_tnt_opp60_other_us[2];

/* Profiler-only visibility for whether latest 3P frame used full restore. */
static u8 g_tnt_3p_restore_full_last;
static uint32_t g_tnt_gameplay_base_scene[DISPLAY_STRIDE * DISPLAY_HEIGHT];

/* Root-menu artwork is also entirely static except for the four Font A
 * labels. Keep both the background-only frame and the fully composited static
 * frame in BSS so the live menu never rescales its large ROM images. */
static uint32_t g_tnt_menu_background_scene[DISPLAY_STRIDE * DISPLAY_HEIGHT];
static uint32_t g_tnt_menu_static_scene[DISPLAY_STRIDE * DISPLAY_HEIGHT];
static uint32_t g_tnt_onep_static_scene[DISPLAY_STRIDE * DISPLAY_HEIGHT];
/* Shared full-screen frontend scratch cache.  Menu states are mutually
 * exclusive, so ONE PLAYER, OPTIONS/DATA/AUDIO/SCORES, root, and tally can
 * reuse this one 1024x544 buffer instead of reserving another ~2.2 MiB per
 * page.  Each owner rebuilds after another state has claimed the cache. */
static uint32_t g_tnt_onep_settled_scene[DISPLAY_STRIDE * DISPLAY_HEIGHT];
static u32 g_tnt_onep_settled_key;
static u8 g_tnt_onep_settled_valid;
static u8 g_tnt_onep_cache_kind; /* 0=invalid, 1=ordinary, 2=NEW NAME editor */
enum {
    TNT_FRONTEND_CACHE_NONE = 0,
    TNT_FRONTEND_CACHE_ONEP = 1,
    TNT_FRONTEND_CACHE_MAIN = 2,
    TNT_FRONTEND_CACHE_OPTIONS = 3,
    TNT_FRONTEND_CACHE_DATA = 4,
    TNT_FRONTEND_CACHE_AUDIO = 5,
    TNT_FRONTEND_CACHE_SCORES = 6,
    TNT_FRONTEND_CACHE_TALLY = 7,
    TNT_FRONTEND_CACHE_MULTI = 8
};
static u8 g_tnt_frontend_cache_owner;
static u8 g_tnt_frontend_cache_kind;
static u32 g_tnt_frontend_cache_key;
static u8 g_tnt_multi_settled_valid;
static u8 g_tnt_multi_cache_game_type;
static u8 g_tnt_multi_cache_garbage_type;
static u8 g_tnt_multi_cache_ready;
static u8 g_tnt_multi_cache_slot_choice[4];
static u8 g_tnt_multi_cache_slot_confirmed[4];
static u8 g_tnt_multi_cache_pad_count;

/* MULTI name-box pulse cache.  The retail-style GUI pulse advances in fixed
 * 0.05 steps between 0.60 and 1.00, so there are only nine visible u8 levels.
 * Preblend each colored IA8 outline against the static MULTI panel once, then
 * copy the resulting native rectangle during live rendering.  This avoids
 * resampling/alpha-blending four 154x30 source masks every frame in 4P setup. */
#define TNT_MULTI_SLOT_PULSE_STATES 9
#define TNT_MULTI_SLOT_CACHE_W 281
#define TNT_MULTI_SLOT_CACHE_H 56
#define TNT_MULTI_SLOT_CACHE_PIXELS (TNT_MULTI_SLOT_CACHE_W * TNT_MULTI_SLOT_CACHE_H)
static uint32_t *g_tnt_multi_slot_pulse_cache;
static s16 g_tnt_multi_slot_cache_dx[4];
static s16 g_tnt_multi_slot_cache_dy[4];
static u16 g_tnt_multi_slot_cache_w[4];
static u16 g_tnt_multi_slot_cache_h[4];
static u8 g_tnt_multi_slot_pulse_valid[4][TNT_MULTI_SLOT_PULSE_STATES];
static u8 g_tnt_multi_slot_pulse_build_cursor;
#define TNT_MULTI_SLOT_CACHE_BYTES ((size_t)4 * TNT_MULTI_SLOT_PULSE_STATES * TNT_MULTI_SLOT_CACHE_PIXELS * sizeof(uint32_t))
#define TNT_MULTI_SLOT_CACHE_PTR(slot_, state_) \
    (g_tnt_multi_slot_pulse_cache + \
     (((size_t)(slot_) * TNT_MULTI_SLOT_PULSE_STATES + (size_t)(state_)) * TNT_MULTI_SLOT_CACHE_PIXELS))

static uint32_t g_tnt_tally_static_scene[DISPLAY_STRIDE * DISPLAY_HEIGHT];
static uint32_t g_tnt_wonder_static_scene[DISPLAY_STRIDE * DISPLAY_HEIGHT];

static u32 g_tnt_60hz_last_us;
static u32 g_tnt_60hz_fraction;
static u32 g_tnt_finish_60hz_last_us;
static u32 g_tnt_finish_60hz_fraction;
static u32 g_tnt_frontend_rng_seed;
static u8 g_tnt_frontend_rng_seeded;
static u8 g_tnt_haluci_requested;
static u8 g_tnt_milestone_gxm_ready;
static u8 g_tnt_gameplay_gxm_ready;
static u8 g_tnt_multi_available;
static u8 g_tnt_multi_pad_count;
static s8 g_tnt_multi_ports[4] = {-1, -1, -1, -1};
static s8 g_tnt_multi_types[4] = {-1, -1, -1, -1};
static u8 g_tnt_local_2p_fast_render;
/* TEST114A: retain the exact TEST113A RAM alpha kernel through solo
 * NEXT, HOLD and CURRENT/GHOST. Restore scope before multisquare glows. */
static u8 g_tnt113_next_scratch_active;

/* TEST107A isolated: 1P locked cells are exactly 20x20 Vita pixels.
 * Cache the full 20x20 appearance of each settled non-metallic Cube,
 * keyed to the original MobileCubes pool index, source Color, and ALL
 * 64 texture bytes. This is visually identical to 64 8x8 source-texel
 * rectangles at cell_w=cell_h=20; inverse sample mapping uses the
 * same floor boundaries. Morphing/metallic, transient non-pool cubes,
 * or allocation failure fall back to the ORIGINAL renderer.
 * Allocate the ~368 KiB cache on 1P game init (not while drawing).
 * Free/reinitialize via normal process lifetime; cache is explicitly
 * invalidated at every fresh 1P game and can never leak between matches.
 * No source gameplay updates, GPU ordering, or audio are changed. */
#define TNT107_1P_CUBE_POOL 220
#define TNT107_1P_TILE 20
typedef struct {
    const Cube *cube_ptr;
    Color color;
    u8 texels[64];
    u8 valid;
    u8 reserved[3];
    u32 tile[TNT107_1P_TILE*TNT107_1P_TILE];
} TNT107OnePCubeTile;
static TNT107OnePCubeTile *g_tnt107_cache;
static u8 g_tnt107_locked_active;
static u32 g_tnt107_hits, g_tnt107_misses, g_tnt107_fallbacks;
static u32 g_tnt123_dynamic_rendered; /* Diagnostic-only Mayan morph tile count. */

static void tnt107_onep_cache_reset(void) {
    if (!g_tnt107_cache)
        g_tnt107_cache=(TNT107OnePCubeTile *)calloc(
            TNT107_1P_CUBE_POOL,sizeof(TNT107OnePCubeTile));
    else
        memset(g_tnt107_cache,0,
            TNT107_1P_CUBE_POOL*sizeof(TNT107OnePCubeTile));
}

/* Test73: exact compact 3P source-space scaling; never affects 1P or 2P. */
static u8 g_tnt_compact_playfield_active;


#define TNT_2P_CUBE_CACHE_PLAYERS 2
#define TNT_2P_CUBE_CACHE_CUBES   220
#define TNT_2P_CUBE_CACHE_CELL    18

typedef struct {
    u8 valid;
    u8 pixel_shape; /* 0: original 18x18 cache; 1: original 8x8 shading */
    Color color;
    u32 tile[TNT_2P_CUBE_CACHE_CELL * TNT_2P_CUBE_CACHE_CELL];
} TNT2PCubeTileCache;

static TNT2PCubeTileCache
    g_tnt_2p_cube_tile_cache[TNT_2P_CUBE_CACHE_PLAYERS][TNT_2P_CUBE_CACHE_CUBES];

static void tnt_vita_2p_cube_tile_cache_reset(void) {
    memset(g_tnt_2p_cube_tile_cache, 0, sizeof(g_tnt_2p_cube_tile_cache));
}

/* Compact multiplayer has 8px N64 cells projected through 20/11; each
 * settled pixel-space mino is 14 or 15px wide/high, not the 18px 2P tile.
 * Keep the 2P cache intact. Allocate this 3P cache only while 3P is active:
 * placing ~690 KiB in static BSS would exceed Vita's velf segment boundary. */
#define TNT_3P_CUBE_CACHE_PLAYERS 3
#define TNT_4P_CUBE_CACHE_PLAYERS 4
#define TNT_3P_CUBE_CACHE_CUBES   220
#define TNT_3P_CUBE_CACHE_CELL    16

typedef struct {
    const Cube *cube_ptr;
    Color color;
    u8 valid;
    u8 cell_w;
    u8 cell_h;
    u8 reserved;
    u32 tile[TNT_3P_CUBE_CACHE_CELL * TNT_3P_CUBE_CACHE_CELL];
} TNT3PCubeTileCache;

static TNT3PCubeTileCache *g_tnt_3p_cube_tile_cache;
static u8 g_tnt_compact_cube_cache_capacity;
/* Test91: isolated four-board timings. The approved 3P profiler unchanged. */
static u8 g_tnt_4p_prof_active;
static u32 g_tnt_4p_prof_us[9]; /* U C B0 B1 B2 B3 X V T */
/* Test94P: distinguish HP overlay from P1 rendering, and preserve
 * the actual worst four-live frame after a match ends. The old
 * test93 overlay combined HP drawing with B0 and only showed
 * one frame, which was misleading beside the running FPS window. */
static u32 g_tnt_4p_hp_frame_us;
static u32 g_tnt_4p_hp_peak_us[9];
static u32 g_tnt_4p_hp_peak_h_us;
static u32 g_tnt_4p_hp_peak_total;
static u32 g_tnt_4p_hp_fallbacks;
static u8 g_tnt_4p_hp_cache_mask;
static u32 g_tnt_4p_hp_avg_total;
static u32 g_tnt_4p_hp_avg_h;
static u64 g_tnt_4p_hp_window_total;
static u64 g_tnt_4p_hp_window_h;
static u64 g_tnt_4p_hp_window_start;
static u32 g_tnt_4p_hp_window_frames;


/* Test76: diagnostic-only 3P frame timings, displayed only with the existing
 * developer FPS counter. No gameplay or presentation changes. All values
 * are half-second moving averages in microseconds. */
enum { TNT3P_PROF_UPDATE, TNT3P_PROF_COPY, TNT3P_PROF_LOCK0,
       TNT3P_PROF_LOCK1, TNT3P_PROF_LOCK2, TNT3P_PROF_REST0,
       TNT3P_PROF_REST1, TNT3P_PROF_REST2, TNT3P_PROF_EXTRA,
       TNT3P_PROF_VBLANK, TNT3P_PROF_TOTAL, TNT3P_PROF_HOT,
       TNT3P_PROF_COUNT };
static u8 g_tnt_3p_prof_active;
static u64 g_tnt_3p_prof_start_us;
static u64 g_tnt_3p_prof_sum[TNT3P_PROF_COUNT];
static u32 g_tnt_3p_prof_avg[TNT3P_PROF_COUNT];
static u32 g_tnt_3p_prof_frames;
static u32 g_tnt_3p_prof_tile_hits_frame, g_tnt_3p_prof_tile_misses_frame;
static u64 g_tnt_3p_prof_hits_sum, g_tnt_3p_prof_misses_sum;
static u32 g_tnt_3p_prof_hits_avg, g_tnt_3p_prof_misses_avg;
static u32 g_tnt_3p_prof_locked_frame[3];

/* Test89E: retain the slowest 3P Ultra+HP *three-live-player* frame for
 * post-topout screenshots. This supplements, not replaces, the legacy
 * 500ms average. Nothing is captured while paused, nor during a finished
 * board's sweep or the VBlank-only stall of the results screen. */
static u32 g_tnt_3p_ultra_peak_us[TNT3P_PROF_COUNT];
static u32 g_tnt_3p_ultra_peak_total;
static u8 g_tnt_3p_ultra_peak_show;

static void __attribute__((optimize("Os"))) tnt_vita_3p_prof_sample(
    const u32 sample[TNT3P_PROF_COUNT]
) {
    u64 now = sceKernelGetProcessTimeWide();
    int i;
    if (!g_tnt_3p_prof_start_us) g_tnt_3p_prof_start_us = now;
    for (i = 0; i < TNT3P_PROF_COUNT; i++)
        g_tnt_3p_prof_sum[i] += sample[i];
    g_tnt_3p_prof_hits_sum += g_tnt_3p_prof_tile_hits_frame;
    g_tnt_3p_prof_misses_sum += g_tnt_3p_prof_tile_misses_frame;
    g_tnt_3p_prof_frames++;
    if (now - g_tnt_3p_prof_start_us >= 500000U && g_tnt_3p_prof_frames) {
        for (i = 0; i < TNT3P_PROF_COUNT; i++) {
            g_tnt_3p_prof_avg[i] =
                (u32)(g_tnt_3p_prof_sum[i] / g_tnt_3p_prof_frames);
            g_tnt_3p_prof_sum[i] = 0;
        }
        g_tnt_3p_prof_hits_avg =
            (u32)(g_tnt_3p_prof_hits_sum / g_tnt_3p_prof_frames);
        g_tnt_3p_prof_misses_avg =
            (u32)(g_tnt_3p_prof_misses_sum / g_tnt_3p_prof_frames);
        g_tnt_3p_prof_hits_sum = g_tnt_3p_prof_misses_sum = 0;
        g_tnt_3p_prof_frames = 0;
        g_tnt_3p_prof_start_us = now;
    }
}

static void __attribute__((optimize("Os"))) tnt_vita_3p_cube_tile_cache_init(void) {
    g_tnt_compact_cube_cache_capacity = TNT_3P_CUBE_CACHE_PLAYERS;
    if (g_tnt_3p_cube_tile_cache == NULL) {
        g_tnt_3p_cube_tile_cache = (TNT3PCubeTileCache *)malloc(
            (size_t)TNT_3P_CUBE_CACHE_PLAYERS * TNT_3P_CUBE_CACHE_CUBES *
            sizeof(TNT3PCubeTileCache));
    }
    if (g_tnt_3p_cube_tile_cache != NULL) {
        memset(g_tnt_3p_cube_tile_cache, 0,
            (size_t)TNT_3P_CUBE_CACHE_PLAYERS * TNT_3P_CUBE_CACHE_CUBES *
            sizeof(TNT3PCubeTileCache));
    }
}

static void __attribute__((optimize("Os"))) tnt_vita_3p_cube_tile_cache_term(void) {
    free(g_tnt_3p_cube_tile_cache);
    g_tnt_3p_cube_tile_cache = NULL;
    g_tnt_compact_cube_cache_capacity = 0;
}
/* Four-player adds one 220-cube cache allocated only during 4P.
 * The approved 3P match still allocates exactly three pools. */
static void __attribute__((optimize("Os")))
tnt_vita_4p_cube_tile_cache_init(void) {
    tnt_vita_3p_cube_tile_cache_term();
    g_tnt_3p_cube_tile_cache = malloc(
        (size_t)TNT_4P_CUBE_CACHE_PLAYERS * TNT_3P_CUBE_CACHE_CUBES *
        sizeof(TNT3PCubeTileCache));
    if (g_tnt_3p_cube_tile_cache != NULL) {
        g_tnt_compact_cube_cache_capacity = TNT_4P_CUBE_CACHE_PLAYERS;
        memset(g_tnt_3p_cube_tile_cache,0,
            (size_t)TNT_4P_CUBE_CACHE_PLAYERS * TNT_3P_CUBE_CACHE_CUBES *
            sizeof(TNT3PCubeTileCache));
    }
}

#define TNT_2P_MOVING_TEX_CACHE_ENTRIES 128
typedef struct {
    u8 valid;
    const Mino *mino_ptr;
    const PieceDefinition *piece_def_ptr;
    Point physical_pos;
    Point physical_rot_origin;
    Point minos_origin;
    u16 physical_rot;
    u16 mino_scale;
} TNT2PMovingTexCache;

static TNT2PMovingTexCache
    g_tnt_2p_moving_tex_cache[TNT_2P_MOVING_TEX_CACHE_ENTRIES];

static void tnt_vita_2p_moving_tex_cache_reset(void) {
    memset(g_tnt_2p_moving_tex_cache, 0, sizeof(g_tnt_2p_moving_tex_cache));
}

static TNT2PMovingTexCache *tnt_vita_2p_moving_tex_cache_slot(
    const Mino *mino_ptr
) {
    uintptr_t key = (uintptr_t)mino_ptr;
    return &g_tnt_2p_moving_tex_cache[
        (key >> 4) & (TNT_2P_MOVING_TEX_CACHE_ENTRIES - 1)];
}

static u8 tnt_vita_2p_moving_tex_cache_matches(
    const Mino *mino_ptr, const Piece *piece_ptr, u16 mino_scale
) {
    TNT2PMovingTexCache *c;
    if (!g_tnt_local_2p_fast_render || (mino_ptr->unk1 & 1) || mino_ptr->unk20 != 0)
        return FALSE;
    c = tnt_vita_2p_moving_tex_cache_slot(mino_ptr);
    return (u8)(c->valid && c->mino_ptr == mino_ptr &&
        c->piece_def_ptr == piece_ptr->pieceDef_ptr &&
        c->physical_pos.x == piece_ptr->physicalPos.x &&
        c->physical_pos.y == piece_ptr->physicalPos.y &&
        c->physical_rot_origin.x == piece_ptr->physicalRotOrigin.x &&
        c->physical_rot_origin.y == piece_ptr->physicalRotOrigin.y &&
        c->physical_rot == piece_ptr->physicalCkwRotValue &&
        c->mino_scale == mino_scale &&
        c->minos_origin.x == g_minos_ptr->unkE.x &&
        c->minos_origin.y == g_minos_ptr->unkE.y);
}

static void tnt_vita_2p_moving_tex_cache_record(
    const Mino *mino_ptr, const Piece *piece_ptr, u16 mino_scale
) {
    TNT2PMovingTexCache *c = tnt_vita_2p_moving_tex_cache_slot(mino_ptr);
    if (mino_ptr->unk20 != 0) {
        c->valid = FALSE;
        return;
    }
    c->valid = TRUE;
    c->mino_ptr = mino_ptr;
    c->piece_def_ptr = piece_ptr->pieceDef_ptr;
    c->physical_pos = piece_ptr->physicalPos;
    c->physical_rot_origin = piece_ptr->physicalRotOrigin;
    c->physical_rot = piece_ptr->physicalCkwRotValue;
    c->mino_scale = mino_scale;
    c->minos_origin = g_minos_ptr->unkE;
}
static f32 g_tnt_mayan_skybox_angle;
static f32 g_tnt_greek_skybox_angle;
static u32 g_tnt_greek_rain_ticks;
static u8 g_tnt_greek_fire_frame;
static u8 g_tnt_greek_fire_hold;
static u8 g_tnt_egyptian_coal_frame;
static u8 g_tnt_egyptian_coal_hold;
static u8 g_tnt_egyptian_candle_a_frame;
static u8 g_tnt_egyptian_candle_a_hold;
static u8 g_tnt_egyptian_candle_b_frame;
static u8 g_tnt_egyptian_candle_b_hold;
static u32 g_tnt_celtic_sky_ticks;
static u32 g_tnt_celtic_leaf_ticks;
static u8 g_tnt_celtic_lamp_frame;
static u8 g_tnt_celtic_lamp_hold;
static u32 g_tnt_african_sky_ticks;
static u8 g_tnt_african_smoke_frame;
static u8 g_tnt_african_smoke_hold;
static f32 g_tnt_japanese_skybox_angle;
static u32 g_tnt_japanese_effect_ticks;
static u32 g_tnt_japanese_butterfly_ticks;
static u32 g_tnt_russian_sky_ticks;
static u32 g_tnt_russian_snow_ticks;
static u8 g_tnt_russian_candle_frame[TNT_RUSSIAN_CANDLE_INSTANCE_COUNT];
static u8 g_tnt_russian_candle_hold[TNT_RUSSIAN_CANDLE_INSTANCE_COUNT];
typedef struct {
    u8 intra;
    u16 inter;
} TNTVitaFinaleSparseState;
static TNTVitaFinaleSparseState g_tnt_finale_boiler_state[TNT_FINALE_BOILER_INSTANCE_COUNT];
static TNTVitaFinaleSparseState g_tnt_finale_bulb_state[TNT_FINALE_BULB_INSTANCE_COUNT];
static u8 g_tnt_finale_gauge_frame;
static u8 g_tnt_finale_gauge_hold;
static u8 g_tnt_active_gameplay_screen;

typedef struct {
    s8 y;
    s8 x;
} TNTVitaCreditsGradient;

static TNTVitaCreditsGradient g_tnt_credits_gradients[200 * 150];
static u32 g_tnt_credits_blend_rgba[256 * 256];
static u32 g_tnt_credits_frame[200 * 150];
/* Compose Credits in the original 400x300 GUI domain. GXM then performs the
 * non-integer 20/11 enlargement, avoiding expensive CPU work per Vita pixel. */
static u32 g_tnt_credits_native_base[400 * 300];
static u32 g_tnt_credits_native_frame[400 * 300];
static u8 g_tnt_credits_effect_ready;

/* N64 osGetTime() counts at 46.875 MHz: OS_CYCLES_TO_NSEC(c) is c*64/3.
 * Vita process time is in microseconds, so convert it back to the same raw
 * cycle domain before applying the original Game_Init seed conversion. */
static u32 tnt_vita_n64_time_cycles32(void) {
    u64 usec = sceKernelGetProcessTimeWide();
    return (u32)((usec * 3000ULL) / 64ULL);
}

static u32 tnt_vita_cycles_to_nsec32(u32 cycles) {
    return (u32)(((u64)cycles * 64ULL) / 3ULL);
}

static u32 tnt_vita_game_seed_from_id(void) {
    return tnt_vita_cycles_to_nsec32(TNT_Vita_SramGetGameId());
}

static void tnt_vita_save_to_sram(void) {
    /* Original save_to_sram() always replaces game_id with raw osGetTime()
     * immediately before checksumming/writing all three SRAM copies. */
    TNT_Vita_SramSetGameId(tnt_vita_n64_time_cycles32());
    (void)TNT_Vita_SramSave();
}

/* D_800D2E20: exact culture-specific generated player/CPU names. Source uses
 * the first seven entries on Wonder screens 0..6, and all nine on FINALE/7.
 * Player.name is eight characters, so longer source strings are truncated. */
/* Source-exact CPU opponent name texts decoded from the verified player ROM. */
char g_tnt_guest_name_storage[8][9][16];
const char *g_tnt_guest_names[8][9];

/* D_800D3DA0: exact 3x15 NEW NAME editor table. Font B maps '[' and
 * '\\' to the original BACKSPACE and OK glyphs respectively. */


/* D_800CF844 / D_800CF864 / D_800CF928: original Wonder names,
 * seven construction milestones per Wonder, and cumulative completion totals. */
static const char *const g_tnt_wonder_names[8] = {
    "MAYAN", "GREEK", "EGYPTIAN", "CELTIC",
    "AFRICAN", "JAPANESE", "RUSSIAN", "FINALE"
};
static const u32 g_tnt_wonder_milestones[49] = {
       357,    714,   1071,   1428,   1785,   2142,   2500,
      3214,   3928,   4642,   5356,   6070,   6784,   7500,
      9285,  11070,  12855,  14640,  16425,  18210,  20000,
     24285,  28570,  32855,  37140,  41425,  45710,  50000,
     57142,  64284,  71426,  78568,  85710,  92852, 100000,
    121428, 142856, 164284, 185712, 207140, 228568, 250000,
    285714, 321428, 357142, 392856, 428570, 464284, 500000
};
static const u32 g_tnt_wonder_totals[7] = {
    2500, 7500, 20000, 50000, 100000, 250000, 500000
};

static u32 tnt_vita_frontend_rng_next(void) {
    g_tnt_frontend_rng_seed =
        g_tnt_frontend_rng_seed * 10001U + 100001U;
    return g_tnt_frontend_rng_seed;
}

static void tnt_vita_frontend_rng_init_once(void) {
    int i;
    if (g_tnt_frontend_rng_seeded)
        return;
    g_tnt_frontend_rng_seed = (u32)sceKernelGetProcessTimeWide();
    g_tnt_frontend_rng_seeded = TRUE;
    /* Root-menu initialization consumes three random float draws before the
     * one-player GUEST callback asks for its first integer draw. */
    for (i = 0; i < 3; i++)
        (void)tnt_vita_frontend_rng_next();
}

static void tnt_vita_choose_guest_name(
    char out[9], u8 screen, u16 *used_mask
) {
    u8 culture = screen < 8U ? screen : 7U;
    u32 count = culture < 7U ? 7U : 9U;
    u32 idx = 0;
    int retries = 10;

    do {
        idx = (tnt_vita_frontend_rng_next() % 256U) % count;
        if (used_mask == NULL || ((*used_mask & (1U << idx)) == 0))
            break;
    } while (retries-- > 0);

    if (used_mask != NULL)
        *used_mask |= (u16)(1U << idx);
    snprintf(out, 9, "%.8s", g_tnt_guest_names[culture][idx]);
}

/* Convert Vita monotonic microseconds to the original 60 Hz retrace delta.
 * Frontend and gameplay are mutually exclusive, so one resettable timing
 * baseline serves both the GUI fade and outer-Game timing domains. */
static u32 tnt_vita_60hz_delta(u8 running) {
    u32 now_us = (u32)sceKernelGetProcessTimeWide();
    u32 ticks = 0;

    if (g_tnt_60hz_last_us == 0) {
        g_tnt_60hz_last_us = now_us;
        g_tnt_60hz_fraction = 0;
        return running ? 1 : 0;
    }
    if (running) {
        u64 scaled = g_tnt_60hz_fraction +
            (u64)(now_us - g_tnt_60hz_last_us) * 60U;
        ticks = (u32)(scaled / 1000000U);
        g_tnt_60hz_fraction = (u32)(scaled % 1000000U);
    }
    g_tnt_60hz_last_us = now_us;
    return ticks;
}

/* GameFinish is authored in scheduler jiffies, not rendered frames.  Keep
 * normal gameplay on its hardware-verified single-tick cadence, but once the
 * finish state is active, recover the original frametime_delta() behavior so
 * a frame spanning multiple Vita VBlanks advances the finish animation by the
 * corresponding number of 60 Hz retraces instead of visibly slowing down. */
static u32 tnt_vita_gamefinish_60hz_delta(u8 running) {
    u32 now_us = (u32)sceKernelGetProcessTimeWide();
    u32 ticks = 0;

    if (!running) {
        g_tnt_finish_60hz_last_us = 0;
        g_tnt_finish_60hz_fraction = 0;
        return 1;
    }
    if (g_tnt_finish_60hz_last_us == 0) {
        g_tnt_finish_60hz_last_us = now_us;
        g_tnt_finish_60hz_fraction = 0;
        return 1;
    }
    {
        u64 scaled = g_tnt_finish_60hz_fraction +
            (u64)(now_us - g_tnt_finish_60hz_last_us) * 60U;
        ticks = (u32)(scaled / 1000000U);
        g_tnt_finish_60hz_fraction = (u32)(scaled % 1000000U);
    }
    g_tnt_finish_60hz_last_us = now_us;
    return ticks != 0 ? ticks : 1;
}

static void *cdram_alloc(unsigned int size, SceUID *uid) {
    void *mem = NULL;
    unsigned int alignedSize = ALIGN(size, 256 * 1024);

    *uid = sceKernelAllocMemBlock(
        "display_buffer",
        SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
        alignedSize,
        NULL
    );

    if (*uid < 0)
        return NULL;

    sceKernelGetMemBlockBase(*uid, &mem);

    sceGxmMapMemory(
        mem,
        alignedSize,
        SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE
    );

    return mem;
}

static void display_callback(const void *callbackData) {
    const void *address = *(const void **)callbackData;

    SceDisplayFrameBuf framebuf;
    memset(&framebuf, 0, sizeof(framebuf));

    framebuf.size = sizeof(framebuf);
    framebuf.base = (void *)address;
    framebuf.pitch = DISPLAY_STRIDE;
    framebuf.pixelformat = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8;
    framebuf.width = DISPLAY_WIDTH;
    framebuf.height = DISPLAY_HEIGHT;

    sceDisplaySetFrameBuf(&framebuf, SCE_DISPLAY_SETBUF_NEXTFRAME);
}

static int graphics_init(void) {
    SceGxmInitializeParams params;
    memset(&params, 0, sizeof(params));

    params.flags = 0;
    params.displayQueueMaxPendingCount = DISPLAY_MAX_PENDING_SWAPS;
    params.displayQueueCallback = display_callback;
    params.displayQueueCallbackDataSize = sizeof(void *);
    params.parameterBufferSize = SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE;

    if (sceGxmInitialize(&params) < 0)
        return -1;

    for (int i = 0; i < DISPLAY_BUFFER_COUNT; i++) {
        buffers[i].data = cdram_alloc(
            4 * DISPLAY_STRIDE * DISPLAY_HEIGHT,
            &buffers[i].uid
        );

        if (!buffers[i].data)
            return -1;

        sceGxmColorSurfaceInit(
            &buffers[i].surface,
            SCE_GXM_COLOR_FORMAT_A8B8G8R8,
            SCE_GXM_COLOR_SURFACE_LINEAR,
            SCE_GXM_COLOR_SURFACE_SCALE_NONE,
            SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
            DISPLAY_WIDTH,
            DISPLAY_HEIGHT,
            DISPLAY_STRIDE,
            buffers[i].data
        );

        sceGxmSyncObjectCreate(&buffers[i].sync);
    }

    return 0;
}

static void clear_screen(void *buffer, uint32_t color) {
    uint32_t *pixels = (uint32_t *)buffer;

    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            pixels[y * DISPLAY_STRIDE + x] = color;
        }
    }
}

/* Record all software pixels touched during CPU Opponent play on this
 * buffer. Old marks are used ONLY when that same buffer is reused. */
static void tnt_vita_opp60_mark(
    void *buffer, int x0, int y0, int x1, int y1
) {
    u32 bits;
    int row, first, last;
    if (!g_tnt_opp60_tracking || buffer != buffers[backBuffer].data)
        return;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > DISPLAY_WIDTH) x1 = DISPLAY_WIDTH;
    if (y1 > DISPLAY_HEIGHT) y1 = DISPLAY_HEIGHT;
    if (x1 <= x0 || y1 <= y0) return;
    first = x0 / TNT_OPP60_TILE_X;
    last = (x1 - 1) / TNT_OPP60_TILE_X;
    bits = ((1U << (last - first + 1)) - 1U) << first;
    for (row = y0 / TNT_OPP60_TILE_Y;
         row <= (y1 - 1) / TNT_OPP60_TILE_Y; row++)
        g_tnt_opp60_dirty[backBuffer][row] |= bits;
}

static void draw_rect(
    void *buffer,
    int x,
    int y,
    int w,
    int h,
    uint32_t color
) {
    uint32_t *pixels = (uint32_t *)buffer;
    tnt_vita_opp60_mark(buffer, x, y, x + w, y + h);

    for (int py = y; py < y + h; py++) {
        if (py < 0 || py >= DISPLAY_HEIGHT)
            continue;

        for (int px = x; px < x + w; px++) {
            if (px < 0 || px >= DISPLAY_WIDTH)
                continue;

            pixels[py * DISPLAY_STRIDE + px] = color;
        }
    }
}


static uint8_t tnt_color_channel_to_u8(s16 value) {
    int v = value >> 4;

    /*
     * Minos_Mino_Render always feeds the RDP 12-bit Color channels as
     * channel >> 4.  Do not switch interpretation when a morph crosses
     * below 0x100; doing so creates a false bright/color flash near black.
     */
    if (v < 0)
        v = 0;
    if (v > 0xFF)
        v = 0xFF;

    return (uint8_t)v;
}

static uint32_t tnt_alpha_blend_rgb(
    uint32_t dst, u8 src_r, u8 src_g, u8 src_b, u8 src_a);
static u8 tnt_mino_alpha(const Mino *mino_ptr, u8 piece_alpha);

static void draw_rect_alpha(
    void *buffer, int x, int y, int w, int h,
    u8 r, u8 g, u8 b, u8 alpha) {
    uint32_t *pixels = (uint32_t *)buffer;
    int py;

    if (alpha == 0)
        return;
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > DISPLAY_WIDTH) w = DISPLAY_WIDTH - x;
    if (y + h > DISPLAY_HEIGHT) h = DISPLAY_HEIGHT - y;
    if (w <= 0 || h <= 0)
        return;
    tnt_vita_opp60_mark(buffer, x, y, x + w, y + h);

    for (py = y; py < y + h; py++) {
        int px;
        for (px = x; px < x + w; px++) {
            uint32_t *pixel = &pixels[py * DISPLAY_STRIDE + px];
            *pixel = tnt_alpha_blend_rgb(*pixel, r, g, b, alpha);
        }
    }
}

/* Return TRUE only if this compact tile was fully rendered. Stable cubes
 * are shaded and expanded ONCE, then just row-copied each frame. Silver/gold
 * morphs still run through the original per-frame metallic renderer. */
static u8 __attribute__((optimize("Os"))) tnt_vita_3p_cube_tile_draw(
    void *buffer, int x, int y, int w, int h, Cube *cube_ptr,
    u8 dirty_before, u8 alpha
) {
    TNT3PCubeTileCache *c;
    Mino *m = &cube_ptr->mino;
    ptrdiff_t cube_idx;
    int i, ix, iy;
    int dr,dg,db,er,eg,eb;
    uint32_t shaded[64];
    uint32_t *pixels = (uint32_t *)buffer;

    if (g_tnt_3p_cube_tile_cache == NULL ||
        !g_tnt_compact_playfield_active ||
        g_currentplayer >= g_tnt_compact_cube_cache_capacity ||
        g_mobileCubes_ptr == NULL || g_mobileCubes_ptr->cube_list == NULL ||
        w <= 0 || h <= 0 || w > TNT_3P_CUBE_CACHE_CELL ||
        h > TNT_3P_CUBE_CACHE_CELL || m->unk20 != 0)
        return FALSE;

    /* As in the proven 2P tile cache, MobileCubes owns a 220-cube pool.
     * Reject free-floating/nonpool pointers without touching cache slots. */
    if (cube_ptr < g_mobileCubes_ptr->cube_list ||
        cube_ptr >= g_mobileCubes_ptr->cube_list + g_mobileCubes_ptr->max_allowed)
        return FALSE;
    cube_idx = cube_ptr - g_mobileCubes_ptr->cube_list;
    if (cube_idx < 0 || cube_idx >= TNT_3P_CUBE_CACHE_CUBES)
        return FALSE;

    c = &g_tnt_3p_cube_tile_cache[
        (size_t)g_currentplayer * TNT_3P_CUBE_CACHE_CUBES + (size_t)cube_idx];
    if (!c->valid || c->cube_ptr != cube_ptr || dirty_before ||
        c->cell_w != w || c->cell_h != h ||
        memcmp(&c->color, &m->unk4, sizeof(Color)) != 0) {
        if (g_tnt_3p_prof_active) g_tnt_3p_prof_tile_misses_frame++;
        dr = tnt_color_channel_to_u8(m->unk4.diffuse_red);
        dg = tnt_color_channel_to_u8(m->unk4.diffuse_green);
        db = tnt_color_channel_to_u8(m->unk4.diffuse_blue);
        er = tnt_color_channel_to_u8(m->unk4.specular_red);
        eg = tnt_color_channel_to_u8(m->unk4.specular_green);
        eb = tnt_color_channel_to_u8(m->unk4.specular_blue);
        for (i = 0; i < 64; ++i) {
            int texel = m->unkB0[i];
            int r = dr + (((er - dr) * texel) >> 8);
            int g = dg + (((eg - dg) * texel) >> 8);
            int b = db + (((eb - db) * texel) >> 8);
            if (r < 0) r = 0; else if (r > 255) r = 255;
            if (g < 0) g = 0; else if (g > 255) g = 255;
            if (b < 0) b = 0; else if (b > 255) b = 255;
            shaded[i] = COLOR(r,g,b);
        }
        for (iy = 0; iy < h; ++iy) {
            /* Inverse of old per-source texel boundaries floor(ty*h/8).
             * This exactly reproduces the 14/15-pixel nearest-texel layout. */
            int source_y = (((iy + 1) * 8) - 1) / h;
            for (ix = 0; ix < w; ++ix) {
                int source_x = (((ix + 1) * 8) - 1) / w;
                c->tile[iy * TNT_3P_CUBE_CACHE_CELL + ix] =
                    shaded[source_y * 8 + source_x];
            }
        }
        c->cube_ptr = cube_ptr;
        c->color = m->unk4;
        c->cell_w = (u8)w;
        c->cell_h = (u8)h;
        c->valid = TRUE;
    } else if (g_tnt_3p_prof_active) {
        g_tnt_3p_prof_tile_hits_frame++;
    }

    for (iy = 0; iy < h; ++iy) {
        int sy = y + iy;
        int sx0 = 0;
        int copy_w = w;
        uint32_t *dst;
        const uint32_t *src;
        if (sy < 0 || sy >= DISPLAY_HEIGHT) continue;
        if (x < 0) { sx0 = -x; copy_w -= sx0; }
        if (x + w > DISPLAY_WIDTH) copy_w = DISPLAY_WIDTH - x - sx0;
        if (copy_w <= 0) continue;
        dst = pixels + sy * DISPLAY_STRIDE + x + sx0;
        src = c->tile + iy * TNT_3P_CUBE_CACHE_CELL + sx0;
        if (alpha == 0xFF) {
            memcpy(dst, src, (size_t)copy_w * sizeof(uint32_t));
        } else {
            for (ix = 0; ix < copy_w; ++ix) {
                uint32_t color = src[ix];
                dst[ix] = tnt_alpha_blend_rgb(dst[ix],
                    (u8)(color & 0xFFU),
                    (u8)((color >> 8) & 0xFFU),
                    (u8)((color >> 16) & 0xFFU), alpha);
            }
        }
    }
    return TRUE;
}

/* TEST107A exact 20x20 tile cache. Original texture update has already
 * run before entry, so comparisons use the LIVE final 8x8 spotlight data. */
static u8 tnt107_onep_cube_draw(
    void *buffer,int x,int y,int w,int h,
    Cube *cube_ptr,u8 alpha,u8 dirty_before,int blend
) {
    static const u8 src_texel[20]={
        0,0,1,1,1,2,2,3,3,3,4,4,5,5,5,6,6,7,7,7
    };
    TNT107OnePCubeTile *c;
    Mino *m=&cube_ptr->mino;
    ptrdiff_t idx;
    u32 *pixels=(u32 *)buffer;
    int i,xx,yy,dr,dg,db,er,eg,eb;
    u32 shaded[64];
    if (!g_tnt107_locked_active || w!=20 || h!=20 ||
        blend!=0 || g_tnt107_cache==NULL ||
        g_mobileCubes_ptr==NULL ||
        g_mobileCubes_ptr->cube_list==NULL ||
        cube_ptr<g_mobileCubes_ptr->cube_list ||
        cube_ptr>=g_mobileCubes_ptr->cube_list+g_mobileCubes_ptr->max_allowed) {
        g_tnt107_fallbacks++;
        return FALSE;
    }
    idx=cube_ptr-g_mobileCubes_ptr->cube_list;
    if (idx<0 || idx>=TNT107_1P_CUBE_POOL) {
        g_tnt107_fallbacks++;
        return FALSE;
    }
    c=&g_tnt107_cache[idx];
    if (!c->valid || c->cube_ptr!=cube_ptr || dirty_before ||
        memcmp(&c->color,&m->unk4,sizeof(Color))!=0 ||
        memcmp(c->texels,m->unkB0,64)!=0) {
        g_tnt107_misses++;
        dr=tnt_color_channel_to_u8(m->unk4.diffuse_red);
        dg=tnt_color_channel_to_u8(m->unk4.diffuse_green);
        db=tnt_color_channel_to_u8(m->unk4.diffuse_blue);
        er=tnt_color_channel_to_u8(m->unk4.specular_red);
        eg=tnt_color_channel_to_u8(m->unk4.specular_green);
        eb=tnt_color_channel_to_u8(m->unk4.specular_blue);
        for(i=0;i<64;i++) {
            const int texel=m->unkB0[i];
            int r=dr+(((er-dr)*texel)>>8);
            int g=dg+(((eg-dg)*texel)>>8);
            int b=db+(((eb-db)*texel)>>8);
            if(r<0)r=0;else if(r>255)r=255;
            if(g<0)g=0;else if(g>255)g=255;
            if(b<0)b=0;else if(b>255)b=255;
            shaded[i]=COLOR(r,g,b);
        }
        for(yy=0;yy<20;yy++) {
            const int src_row=(int)src_texel[yy]*8;
            for(xx=0;xx<20;xx++)
                c->tile[yy*20+xx]=shaded[src_row+src_texel[xx]];
        }
        c->cube_ptr=cube_ptr;
        c->color=m->unk4;
        memcpy(c->texels,m->unkB0,64);
        c->valid=TRUE;
    } else {
        g_tnt107_hits++;
    }
    for(yy=0;yy<20;yy++) {
        const int py=y+yy;
        int sx=0,cw=20;
        u32 *dst;
        const u32 *src;
        if(py<0 || py>=DISPLAY_HEIGHT)continue;
        if(x<0){sx=-x;cw-=sx;}
        if(x+20>DISPLAY_WIDTH)cw=DISPLAY_WIDTH-x-sx;
        if(cw<=0)continue;
        dst=pixels+py*DISPLAY_STRIDE+x+sx;
        src=c->tile+yy*20+sx;
        if(alpha==0xFF)
            memcpy(dst,src,(size_t)cw*sizeof(u32));
        else
            for(xx=0;xx<cw;xx++){
                const u32 color=src[xx];
                dst[xx]=tnt_alpha_blend_rgb(dst[xx],
                    (u8)(color&0xFFU),(u8)((color>>8)&0xFFU),
                    (u8)((color>>16)&0xFFU),alpha);
            }
    }
    return TRUE;
}

/* TEST123A: isolate 1P settled, full-size OPAQUE morph cubes only.
 * Preserve the exact original 8x8 spotlight/color interpolation and
 * source 20x20 integer cell expansion, but replace 64 clipped draw_rect
 * calls with one contiguous pixel-expansion traversal. Other alpha, sizes,
 * idle/settled, opponent, attract, and multiplayer code is unchanged.
 * This is experimental and must be hardware tested before 1.0 inclusion. */
static void tnt123_onep_opaque_morph_draw(
    void *buffer,int x,int y,const Mino *m,int blend,
    int prim_r,int prim_g,int prim_b,int env_r,int env_g,int env_b
) {
    static const u8 source_texel[20]={
        0,0,1,1,1,2,2,3,3,3,4,4,5,5,5,6,6,7,7,7
    };
    u32 shaded[64];
    u32 *dst=(u32*)buffer;
    int i,xx,yy;
    for(i=0;i<64;i++){
        int t;
        int r,g,b;
        if(blend==0xff)t=m->unkF0[i];
        else t=m->unkB0[i]+(((int)m->unkF0[i]-(int)m->unkB0[i])*blend>>8);
        r=prim_r+(((env_r-prim_r)*t)>>8);
        g=prim_g+(((env_g-prim_g)*t)>>8);
        b=prim_b+(((env_b-prim_b)*t)>>8);
        if(r<0)r=0;else if(r>255)r=255;
        if(g<0)g=0;else if(g>255)g=255;
        if(b<0)b=0;else if(b>255)b=255;
        shaded[i]=COLOR(r,g,b);
    }
    for(yy=0;yy<20;yy++){
        const int py=y+yy;
        const int srcrow=(int)source_texel[yy]*8;
        int xfirst=0,xlast=20;
        u32 *row;
        if(py<0||py>=DISPLAY_HEIGHT)continue;
        if(x<0)xfirst=-x;
        if(x+20>DISPLAY_WIDTH)xlast=DISPLAY_WIDTH-x;
        if(xfirst>=xlast)continue;
        row=dst+py*DISPLAY_STRIDE;
        for(xx=xfirst;xx<xlast;xx++)
            row[x+xx]=shaded[srcrow+source_texel[xx]];
    }
    g_tnt123_dynamic_rendered++;
}

static void draw_cube_mino_textured(
    void *buffer,
    int x,
    int y,
    int cell_w,
    int cell_h,
    Cube *cube_ptr
) {
    Mino *mino_ptr;
    int prim_r;
    int prim_g;
    int prim_b;
    int env_r;
    int env_g;
    int env_b;
    int blend;
    u8 render_alpha;
    int texture_dirty_before_update;
    int ty;
    int tx;

    if (cube_ptr == NULL) {
        return;
    }

    mino_ptr = &cube_ptr->mino;

    /* Pause fades MobileCubes to alpha zero.  The old renderer still rebuilt
     * spotlight textures and shaded all 64 texels before draw_rect_alpha()
     * discarded every pixel.  Visibility is already fully determined here,
     * so skip render-only work while the cube is exactly transparent. */
    render_alpha = tnt_mino_alpha(
        mino_ptr,
        g_mobileCubes_ptr != NULL ? g_mobileCubes_ptr->alpha : 0xFF
    );
    if (render_alpha == 0)
        return;
    tnt_vita_opp60_mark(buffer, x, y, x + cell_w, y + cell_h);

    texture_dirty_before_update = (mino_ptr->unk1 & 1) != 0;
    TNT_Vita_MinoTextures_Update(mino_ptr, cube_ptr);

    prim_r = tnt_color_channel_to_u8(mino_ptr->unk4.diffuse_red);
    prim_g = tnt_color_channel_to_u8(mino_ptr->unk4.diffuse_green);
    prim_b = tnt_color_channel_to_u8(mino_ptr->unk4.diffuse_blue);
    env_r = tnt_color_channel_to_u8(mino_ptr->unk4.specular_red);
    env_g = tnt_color_channel_to_u8(mino_ptr->unk4.specular_green);
    env_b = tnt_color_channel_to_u8(mino_ptr->unk4.specular_blue);
    blend = mino_ptr->unk20;
    if (blend < 0) blend = 0;
    if (blend > 0xFF) blend = 0xFF;

    /* TEST123A: dynamic metallic/morph tiles were known 107A fallbacks.
     * Restrict to opaque20x20, retaining legacy alpha blending and all
     * other sizes/modes and exact 107A settled-cache behavior. */
    if(g_tnt107_locked_active && g_tnt_active_gameplay_screen==0 &&
       cell_w==20 && cell_h==20 &&
       blend!=0 && render_alpha==0xff){
        tnt123_onep_opaque_morph_draw(buffer,x,y,mino_ptr,blend,
            prim_r,prim_g,prim_b,env_r,env_g,env_b);
        return;
    }

    /* TEST107A: 1P 20x20 exact settled tiles, fall back for any
     * metallic/animated texture. Reset on each new one-player match. */
    if (g_tnt107_locked_active &&
        tnt107_onep_cube_draw(buffer,x,y,cell_w,cell_h,cube_ptr,
            render_alpha,(u8)texture_dirty_before_update,blend))
        return;

    /* Compact 3P locked cube cache: no work at all on warm settled tiles. */
    if (g_tnt_local_2p_fast_render && blend == 0 &&
        tnt_vita_3p_cube_tile_draw(buffer, x, y, cell_w, cell_h,
            cube_ptr, (u8)texture_dirty_before_update, render_alpha))
        return;

    /* Local 2P settled cubes are axis-aligned. The source 10-pixel cell is
     * presented as exactly 18 Vita pixels with the current 20/11 scale.
     * Stable, non-metallic locked cubes are visually identical frame-to-frame,
     * so cache their fully shaded/expanded 18x18 tile by player + MobileCube
     * pool index. Dirty texture state or any diffuse/specular color change
     * invalidates immediately. Metallic cubes bypass the cache because their
     * spotlight field intentionally animates every frame. */
    if (g_tnt_local_2p_fast_render && cell_w == TNT_2P_CUBE_CACHE_CELL && cell_h == TNT_2P_CUBE_CACHE_CELL) {
        static const u8 texel_for_pixel[TNT_2P_CUBE_CACHE_CELL] = {
            0,0,1,1,2,2,3,3,3,4,4,5,5,6,6,7,7,7
        };
        TNT2PCubeTileCache *cache = NULL;
        int cube_idx = -1;
        int cache_hit = FALSE;
        uint32_t *pixels = (uint32_t *)buffer;

        if (g_currentplayer < TNT_2P_CUBE_CACHE_PLAYERS &&
            g_mobileCubes_ptr != NULL && g_mobileCubes_ptr->cube_list != NULL &&
            cube_ptr >= g_mobileCubes_ptr->cube_list &&
            cube_ptr < g_mobileCubes_ptr->cube_list + g_mobileCubes_ptr->max_allowed) {
            cube_idx = (int)(cube_ptr - g_mobileCubes_ptr->cube_list);
            if (cube_idx >= 0 && cube_idx < TNT_2P_CUBE_CACHE_CUBES)
                cache = &g_tnt_2p_cube_tile_cache[g_currentplayer][cube_idx];
        }

        if (blend == 0 && cache != NULL && cache->valid &&
            cache->pixel_shape == 0 &&
            !texture_dirty_before_update &&
            memcmp(&cache->color, &mino_ptr->unk4, sizeof(Color)) == 0) {
            cache_hit = TRUE;
        }

        if (blend == 0 && cache != NULL && !cache_hit) {
            uint32_t shaded[64];
            int idx;
            for (idx = 0; idx < 64; idx++) {
                int texel = mino_ptr->unkB0[idx];
                int out_r = prim_r + (((env_r - prim_r) * texel) >> 8);
                int out_g = prim_g + (((env_g - prim_g) * texel) >> 8);
                int out_b = prim_b + (((env_b - prim_b) * texel) >> 8);
                if (out_r < 0) out_r = 0;
                if (out_r > 255) out_r = 255;
                if (out_g < 0) out_g = 0;
                if (out_g > 255) out_g = 255;
                if (out_b < 0) out_b = 0;
                if (out_b > 255) out_b = 255;
                shaded[idx] = COLOR(out_r, out_g, out_b);
            }
            for (ty = 0; ty < TNT_2P_CUBE_CACHE_CELL; ty++) {
                int sy = texel_for_pixel[ty] << 3;
                for (tx = 0; tx < TNT_2P_CUBE_CACHE_CELL; tx++)
                    cache->tile[ty * TNT_2P_CUBE_CACHE_CELL + tx] =
                        shaded[sy + texel_for_pixel[tx]];
            }
            cache->color = mino_ptr->unk4;
            cache->pixel_shape = 0;
            cache->valid = TRUE;
            cache_hit = TRUE;
        }

        if (cache_hit && cache != NULL) {
            for (ty = 0; ty < TNT_2P_CUBE_CACHE_CELL; ty++) {
                int py = y + ty;
                int sx0 = 0;
                int copy_w = TNT_2P_CUBE_CACHE_CELL;
                uint32_t *row;
                const uint32_t *srcrow;
                if (py < 0 || py >= DISPLAY_HEIGHT) continue;
                if (x < 0) { sx0 = -x; copy_w -= sx0; }
                if (x + TNT_2P_CUBE_CACHE_CELL > DISPLAY_WIDTH)
                    copy_w = DISPLAY_WIDTH - x - sx0;
                if (copy_w <= 0) continue;
                row = pixels + py * DISPLAY_STRIDE + x + sx0;
                srcrow = cache->tile + ty * TNT_2P_CUBE_CACHE_CELL + sx0;
                if (render_alpha == 0xFF) {
                    memcpy(row, srcrow, (size_t)copy_w * sizeof(uint32_t));
                } else {
                    for (tx = 0; tx < copy_w; tx++) {
                        uint32_t color = srcrow[tx];
                        row[tx] = tnt_alpha_blend_rgb(
                            row[tx], (u8)(color & 0xFFU),
                            (u8)((color >> 8) & 0xFFU),
                            (u8)((color >> 16) & 0xFFU), render_alpha);
                    }
                }
            }
            return;
        }

        /* Dynamic metallic/color-morph case: shade exactly as before, but map
         * each 8x8 source texel across the exact 18-pixel destination spans. */
        {
            uint32_t shaded[64];
            int idx;
            for (idx = 0; idx < 64; idx++) {
                int texel;
                int out_r, out_g, out_b;
                if (blend == 0) texel = mino_ptr->unkB0[idx];
                else if (blend == 0xFF) texel = mino_ptr->unkF0[idx];
                else texel = mino_ptr->unkB0[idx] +
                    (((int)mino_ptr->unkF0[idx] - (int)mino_ptr->unkB0[idx]) * blend >> 8);
                out_r = prim_r + (((env_r - prim_r) * texel) >> 8);
                out_g = prim_g + (((env_g - prim_g) * texel) >> 8);
                out_b = prim_b + (((env_b - prim_b) * texel) >> 8);
                if (out_r < 0) out_r = 0;
                if (out_r > 255) out_r = 255;
                if (out_g < 0) out_g = 0;
                if (out_g > 255) out_g = 255;
                if (out_b < 0) out_b = 0;
                if (out_b > 255) out_b = 255;
                shaded[idx] = COLOR(out_r, out_g, out_b);
            }
            for (ty = 0; ty < TNT_2P_CUBE_CACHE_CELL; ty++) {
                int py = y + ty;
                int sy;
                uint32_t *row;
                if (py < 0 || py >= DISPLAY_HEIGHT) continue;
                sy = texel_for_pixel[ty] << 3;
                row = pixels + py * DISPLAY_STRIDE;
                for (tx = 0; tx < TNT_2P_CUBE_CACHE_CELL; tx++) {
                    int px = x + tx;
                    uint32_t color;
                    if (px < 0 || px >= DISPLAY_WIDTH) continue;
                    color = shaded[sy + texel_for_pixel[tx]];
                    if (render_alpha == 0xFF) row[px] = color;
                    else row[px] = tnt_alpha_blend_rgb(
                        row[px], (u8)(color & 0xFFU),
                        (u8)((color >> 8) & 0xFFU),
                        (u8)((color >> 16) & 0xFFU), render_alpha);
                }
            }
            return;
        }
    }

    /* Exact Opponent source-space projection can produce 18x19,
     * 19x18 or 19x19 settled blocks. The legacy 18x18 expanded-tile
     * cache rejects them and falls back to 64 draw_rect calls PER CUBE,
     * which costs precious frame time on medium stacks. Keep the
     * already-existing per-cube 18x18 storage allocation, but use its
     * first 64 colors as a shaded 8x8 source tile on these rare shapes.
     * Only pixel expansion varies; original eight-texel source and
     * per-cube color/dirty invalidation remain identical. Local MULTI
     * never enters this path. */
    if (g_tnt_opp60_exact_board &&
        g_tnt_local_2p_fast_render &&
        cell_w >= 18 && cell_w <= 19 &&
        cell_h >= 18 && cell_h <= 19 &&
        (cell_w == 19 || cell_h == 19) &&
        blend == 0 && render_alpha == 0xFF) {
        TNT2PCubeTileCache *cache = NULL;
        uint32_t local_shaded[64];
        const uint32_t *shaded = local_shaded;
        uint32_t *pixels = (uint32_t *)buffer;
        if (g_currentplayer < TNT_2P_CUBE_CACHE_PLAYERS &&
            g_mobileCubes_ptr && g_mobileCubes_ptr->cube_list &&
            cube_ptr >= g_mobileCubes_ptr->cube_list &&
            cube_ptr < g_mobileCubes_ptr->cube_list +
                       g_mobileCubes_ptr->max_allowed) {
            int idx = (int)(cube_ptr - g_mobileCubes_ptr->cube_list);
            if (idx >= 0 && idx < TNT_2P_CUBE_CACHE_CUBES)
                cache = &g_tnt_2p_cube_tile_cache[g_currentplayer][idx];
        }
        if (cache && cache->valid && cache->pixel_shape == 1 &&
            !texture_dirty_before_update &&
            memcmp(&cache->color, &mino_ptr->unk4, sizeof(Color)) == 0) {
            shaded = cache->tile;
        } else {
            int i;
            for (i = 0; i < 64; i++) {
                int texel = mino_ptr->unkB0[i];
                int red = prim_r + (((env_r - prim_r) * texel) >> 8);
                int green = prim_g + (((env_g - prim_g) * texel) >> 8);
                int blue = prim_b + (((env_b - prim_b) * texel) >> 8);
                if (red < 0) red = 0; else if (red > 255) red = 255;
                if (green < 0) green = 0; else if (green > 255) green = 255;
                if (blue < 0) blue = 0; else if (blue > 255) blue = 255;
                local_shaded[i] = COLOR(red, green, blue);
            }
            if (cache) {
                memcpy(cache->tile, local_shaded, sizeof(local_shaded));
                cache->color = mino_ptr->unk4;
                cache->pixel_shape = 1;
                cache->valid = TRUE;
                shaded = cache->tile;
            }
        }
        for (ty = 0; ty < cell_h; ty++) {
            int py = y + ty;
            int tex_y = (((ty + 1) * 8 + cell_h - 1) / cell_h) - 1;
            uint32_t *destrow;
            if (py < 0 || py >= DISPLAY_HEIGHT) continue;
            destrow = pixels + (size_t)py * DISPLAY_STRIDE;
            for (tx = 0; tx < cell_w; tx++) {
                int px = x + tx;
                int tex_x = (((tx + 1) * 8 + cell_w - 1) / cell_w) - 1;
                if (px >= 0 && px < DISPLAY_WIDTH)
                    destrow[px] = shaded[tex_y * 8 + tex_x];
            }
        }
        return;
    }

    for (ty = 0; ty < 8; ty++) {
        int py0 = y + (ty * cell_h) / 8;
        int py1 = y + ((ty + 1) * cell_h) / 8;

        for (tx = 0; tx < 8; tx++) {
            int idx = ty * 8 + tx;
            int texel;
            int out_r;
            int out_g;
            int out_b;
            int px0 = x + (tx * cell_w) / 8;
            int px1 = x + ((tx + 1) * cell_w) / 8;

            if (blend == 0) {
                texel = mino_ptr->unkB0[idx];
            } else if (blend == 0xFF) {
                texel = mino_ptr->unkF0[idx];
            } else {
                texel = mino_ptr->unkB0[idx] +
                    (((int)mino_ptr->unkF0[idx] -
                      (int)mino_ptr->unkB0[idx]) * blend >> 8);
            }

            out_r = prim_r + (((env_r - prim_r) * texel) >> 8);
            out_g = prim_g + (((env_g - prim_g) * texel) >> 8);
            out_b = prim_b + (((env_b - prim_b) * texel) >> 8);

            if (out_r < 0) out_r = 0;
            if (out_r > 255) out_r = 255;
            if (out_g < 0) out_g = 0;
            if (out_g > 255) out_g = 255;
            if (out_b < 0) out_b = 0;
            if (out_b > 255) out_b = 255;

            if (render_alpha == 0xFF) {
                draw_rect(
                    buffer,
                    px0,
                    py0,
                    px1 - px0,
                    py1 - py0,
                    COLOR(out_r, out_g, out_b)
                );
            } else {
                draw_rect_alpha(
                    buffer,
                    px0,
                    py0,
                    px1 - px0,
                    py1 - py0,
                    (u8)out_r,
                    (u8)out_g,
                    (u8)out_b,
                    render_alpha
                );
            }
        }
    }
}

static u8 tnt_mino_alpha(const Mino *mino_ptr, u8 piece_alpha) {
    if (mino_ptr->unk1C == 0xFF)
        return piece_alpha;
    if (piece_alpha == 0xFF)
        return mino_ptr->unk1C;
    return (mino_ptr->unk1C * piece_alpha) / 0x100U;
}

/*
 * Convert an original 8.8 fixed-point edge to a Vita pixel using floor
 * division.  C truncates negative division toward zero, which can make two
 * neighboring moving minos choose different pixel boundaries for one frame.
 */
static int tnt_fixed_edge_to_pixel(s32 value, int origin, int cell) {
    s64 scaled = (s64)value *
        (g_tnt_compact_playfield_active ? (8 * TNT_PRESENT_CELL) : cell);
    s64 divisor = (s64)0x100 *
        (g_tnt_compact_playfield_active ? 11 : 1);
    s64 pixels;

    if (scaled >= 0)
        pixels = scaled / divisor;
    else
        pixels = -(((-scaled) + divisor - 1) / divisor);

    return origin + (int)pixels;
}

/* Test220D: exact shared 400x300 background basis for Opponent pieces.
 * x,y output from the existing 2P projected origins. Match background's
 * floor((source_position - 83/36) * 20/11) including fractional remainder.
 * The old integer 18px/cell places row20 FOUR pixels above its border. */
static int tnt_vita_opp60_edge(
    s32 value, int origin, int cell, u8 is_y
) {
    s64 scaled, div;
    if (!g_tnt_opp60_exact_board)
        return tnt_fixed_edge_to_pixel(value, origin, cell);
    scaled = (s64)value * 10 * TNT_PRESENT_CELL +
        (s64)(is_y ? g_tnt_opp60_phase_y :
                     g_tnt_opp60_phase_x) * 256;
    div = (s64)256 * 11;
    return origin + (int)(scaled >= 0
        ? (scaled / div) : -(((-scaled) + div - 1) / div));
}

/* Multiplayer holds are authored in playfield-local Minos coordinates.
 * Project them through each player's source BoardInfo location / unitSize
 * and the exact 400x300 presentation transform, instead of a rounded local
 * cell, to register with the source art in both 2P and compact 3P/4P. */
static u8 g_tnt_2p_hold_source_space;
static int g_tnt_2p_hold_screen_board_x;
static int g_tnt_2p_hold_screen_board_y;
static int g_tnt_2p_hold_screen_cell;
static int g_tnt_2p_hold_playfield_source_x;
static int g_tnt_2p_hold_playfield_source_y;
static int g_tnt_hold_source_unit_size;

/* Preserve the full precision of the N64 hold anchor in 3/4P. This avoids
 * falling back to the slow affine rasterizer merely because hold artwork is
 * projected in absolute source space rather than the rounded board origin. */
static int tnt_vita_compact_hold_edge_pixel(s32 edge, u8 is_y) {
    s64 source_fixed = (s64)(is_y ? g_tnt_2p_hold_playfield_source_y - 36
                                    : g_tnt_2p_hold_playfield_source_x - 83) * 256 +
        (s64)edge * g_tnt_hold_source_unit_size;
    s64 scaled = source_fixed * g_tnt_2p_hold_screen_cell;
    s64 den = (s64)256 * 11;
    s64 pix = scaled >= 0 ? scaled / den : -((-scaled + den - 1) / den);
    return (is_y ? g_tnt_2p_hold_screen_board_y : g_tnt_2p_hold_screen_board_x) +
        (int)pix;
}


/*
 * Draw the same logical mino square used by the original falling-piece
 * transform.  Mapping both fixed-point edges (rather than rounding a position
 * and a width separately) keeps shared edges identical while a preview piece
 * moves and scales.
 */
static void draw_piece_mino_textured_alpha(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    Point center,
    u16 mino_scale,
    Piece *piece_ptr,
    u8 mino_idx,
    u8 alpha
) {
    Mino *mino_ptr = &piece_ptr->mino_arr[mino_idx];
    uint32_t *pixels = (uint32_t *)buffer;
    s32 residual = piece_ptr->physicalCkwRotValue & 0x3FFF;
    s32 sin_val = FUN_026900_800608f4_oneliner_shift6_1((u16)residual);
    s32 cos_val = FUN_026900_80060914_oneliner_shift6_2((u16)residual);
    s32 half = mino_scale >> 1;
    s32 offsets[4][2] = {
        { -half, -half },
        {  half, -half },
        { -half,  half },
        {  half,  half }
    };
    double qx[4];
    double qy[4];
    double ax;
    double ay;
    double bx;
    double by;
    double det;
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    int prim_r;
    int prim_g;
    int prim_b;
    int env_r;
    int env_g;
    int env_b;
    int blend;
    int i;
    int x;
    int y;

    /* The pause/global-alpha path can make preview/hold/current/ghost minos
     * exactly invisible.  Avoid texture regeneration and the affine double-
     * precision raster loop when no destination pixel can contribute. */
    if (alpha == 0)
        return;

    /* N64 MobilePiece_Render marks every moving mino dirty because its RDP
     * texture setup is rebuilt per draw. In the Vita software renderer that
     * became an expensive 8x8 spotlight resample even when a preview/hold
     * piece is completely stationary. Reuse the already-generated fixed
     * spotlight texture when every input that affects it is unchanged.
     * Current/falling pieces naturally miss as they move/rotate; metallic
     * minos deliberately bypass reuse because their metal field animates. */
    if (!tnt_vita_2p_moving_tex_cache_matches(
            mino_ptr, piece_ptr, mino_scale)) {
        mino_ptr->unk1 |= 1;
        TNT_Vita_MinoTextures_UpdatePiece(
            mino_ptr, piece_ptr, mino_idx, mino_scale);
        if (g_tnt_local_2p_fast_render)
            tnt_vita_2p_moving_tex_cache_record(
                mino_ptr, piece_ptr, mino_scale);
    }

    prim_r = tnt_color_channel_to_u8(mino_ptr->unk4.diffuse_red);
    prim_g = tnt_color_channel_to_u8(mino_ptr->unk4.diffuse_green);
    prim_b = tnt_color_channel_to_u8(mino_ptr->unk4.diffuse_blue);
    env_r = tnt_color_channel_to_u8(mino_ptr->unk4.specular_red);
    env_g = tnt_color_channel_to_u8(mino_ptr->unk4.specular_green);
    env_b = tnt_color_channel_to_u8(mino_ptr->unk4.specular_blue);
    blend = mino_ptr->unk20;
    if (blend < 0) blend = 0;
    if (blend > 0xFF) blend = 0xFF;

    /*
     * The original Minos transform maps physical coordinate 0 to the center
     * of board cell 0; BoardInfo.location is the top-left EDGE of that cell.
     * Convert that convention here once for every MobilePiece user by adding
     * half a physical cell before mapping into the Vita board rectangle.
     * This keeps current/ghost unchanged while also placing hold/preview at
     * their exact original screen anchors.
     *
     * Minos_8006f4b4... then rotates the four corners of each moving Mino by
     * physicalCkwRotValue & 0x3FFF after the coarse quarter-turn has already
     * positioned the Mino center.
     */
    center.x += 0x80;
    center.y += 0x80;

    /* Steady-state moving minos are axis-aligned. Avoid the general affine
     * per-pixel double-precision divide path in that overwhelmingly common
     * case, while preserving the same 8x8 source texture and nearest-texel
     * mapping. Rotating frames still use the original affine rasterizer below. */
    if (g_tnt_local_2p_fast_render && residual == 0 &&
        (!g_tnt_2p_hold_source_space || g_tnt_compact_playfield_active)) {
        s32 span = half << 1;
        s32 fx0 = center.x - half;
        s32 fy0 = center.y - half;
        s32 fx1 = center.x + half;
        s32 fy1 = center.y + half;
        u8 source_hold = g_tnt_2p_hold_source_space;
        int x0 = (source_hold ? tnt_vita_compact_hold_edge_pixel(fx0, FALSE)
                              : tnt_vita_opp60_edge(fx0, board_x, cell, FALSE)) - 1;
        int y0 = (source_hold ? tnt_vita_compact_hold_edge_pixel(fy0, TRUE)
                              : tnt_vita_opp60_edge(fy0, board_y, cell, TRUE)) - 1;
        int x1 = (source_hold ? tnt_vita_compact_hold_edge_pixel(fx1, FALSE)
                              : tnt_vita_opp60_edge(fx1, board_x, cell, FALSE)) + 1;
        int y1 = (source_hold ? tnt_vita_compact_hold_edge_pixel(fy1, TRUE)
                              : tnt_vita_opp60_edge(fy1, board_y, cell, TRUE)) + 1;
        s64 scale_num = source_hold
            ? (s64)g_tnt_hold_source_unit_size * g_tnt_2p_hold_screen_cell
            : (g_tnt_compact_playfield_active ? (8 * TNT_PRESENT_CELL) :
                (g_tnt_opp60_exact_board ? (10 * TNT_PRESENT_CELL) : cell));
        s64 scale_den = (source_hold || g_tnt_compact_playfield_active ||
                         g_tnt_opp60_exact_board) ? 11 : 1;
        s64 denom = (s64)2 * span * scale_num;
        int w, h;
        s8 x_tex[64], y_tex[64];
        uint32_t shaded[64];
        int si;
        if (x0 < 0) x0 = 0;
        if (y0 < 0) y0 = 0;
        if (x1 >= DISPLAY_WIDTH) x1 = DISPLAY_WIDTH - 1;
        if (y1 >= DISPLAY_HEIGHT) y1 = DISPLAY_HEIGHT - 1;
        w = x1 - x0 + 1;
        h = y1 - y0 + 1;
        if (span > 0 && denom > 0 && w > 0 && h > 0 && w <= 64 && h <= 64) {
            tnt_vita_opp60_mark(buffer, x0, y0, x1 + 1, y1 + 1);
            for (x = 0; x < w; x++) {
                s32 dx = x0 + x;
                s64 num;
                if (source_hold) {
                    s64 anchor = (s64)(g_tnt_2p_hold_playfield_source_x - 83) * 256 +
                        (s64)fx0 * g_tnt_hold_source_unit_size;
                    num = ((s64)(2 * (dx - g_tnt_2p_hold_screen_board_x) + 1) * 256 * 11) -
                        (s64)2 * anchor * g_tnt_2p_hold_screen_cell;
                } else {
                    num = ((s64)(2 * (dx - board_x) + 1) * 256 * scale_den) -
                        (s64)2 * fx0 * scale_num -
                        (g_tnt_opp60_exact_board ?
                            (s64)2 * g_tnt_opp60_phase_x * 256 : 0);
                }
                if (num >= 0 && num < denom) {
                    if (g_tnt_opp60_exact_board &&
                        denom <= 0x0FFFFFFFLL)
                        x_tex[x] = (s8)(((u32)num * 8U) / (u32)denom);
                    else
                        x_tex[x] = (s8)((num * 8) / denom);
                } else {
                    x_tex[x] = (s8)-1;
                }
            }
            for (y = 0; y < h; y++) {
                s32 dy = y0 + y;
                s64 num;
                if (source_hold) {
                    s64 anchor = (s64)(g_tnt_2p_hold_playfield_source_y - 36) * 256 +
                        (s64)fy0 * g_tnt_hold_source_unit_size;
                    num = ((s64)(2 * (dy - g_tnt_2p_hold_screen_board_y) + 1) * 256 * 11) -
                        (s64)2 * anchor * g_tnt_2p_hold_screen_cell;
                } else {
                    num = ((s64)(2 * (dy - board_y) + 1) * 256 * scale_den) -
                        (s64)2 * fy0 * scale_num -
                        (g_tnt_opp60_exact_board ?
                            (s64)2 * g_tnt_opp60_phase_y * 256 : 0);
                }
                if (num >= 0 && num < denom) {
                    if (g_tnt_opp60_exact_board &&
                        denom <= 0x0FFFFFFFLL)
                        y_tex[y] = (s8)(((u32)num * 8U) / (u32)denom);
                    else
                        y_tex[y] = (s8)((num * 8) / denom);
                } else {
                    y_tex[y] = (s8)-1;
                }
            }

            /* Shade the 64 source texels once. The old fast path repeated
             * exactly this math for every expanded destination pixel. */
            for (si = 0; si < 64; si++) {
                int texel, out_r, out_g, out_b;
                if (blend == 0) texel = mino_ptr->unkB0[si];
                else if (blend == 0xFF) texel = mino_ptr->unkF0[si];
                else texel = mino_ptr->unkB0[si] +
                    (((int)mino_ptr->unkF0[si] - (int)mino_ptr->unkB0[si]) * blend >> 8);
                out_r = prim_r + (((env_r - prim_r) * texel) >> 8);
                out_g = prim_g + (((env_g - prim_g) * texel) >> 8);
                out_b = prim_b + (((env_b - prim_b) * texel) >> 8);
                if (out_r < 0) out_r = 0;
                if (out_r > 255) out_r = 255;
                if (out_g < 0) out_g = 0;
                if (out_g > 255) out_g = 255;
                if (out_b < 0) out_b = 0;
                if (out_b > 255) out_b = 255;
                shaded[si] = COLOR(out_r, out_g, out_b);
            }

            for (y = 0; y < h; y++) {
                int ty = y_tex[y];
                uint32_t row_scratch[64];
                uint32_t *frame_row = pixels + (y0 + y) * DISPLAY_STRIDE + x0;
                uint32_t *draw_row = frame_row;
                u8 use_scratch = g_tnt113_next_scratch_active && alpha != 0xFF;
                if (ty < 0) continue;
                /* Bulk reads avoid uncached CDRAM loads for every blended
                 * pixel. Preserve untouched edge pixels and original alpha
                 * arithmetic, ordering, texture sampling and opaque path. */
                if (use_scratch) {
                    memcpy(row_scratch, frame_row, (size_t)w * sizeof(uint32_t));
                    draw_row = row_scratch;
                }
                for (x = 0; x < w; x++) {
                    int tx = x_tex[x];
                    int idx;
                    uint32_t color;
                    if (tx < 0) continue;
                    idx = ty * 8 + tx;
                    color = shaded[idx];
                    if (alpha == 0xFF) {
                        draw_row[x] = color;
                    } else {
                        uint32_t *dstp = &draw_row[x];
                        uint32_t dst = *dstp;
                        u32 inv = 255U - alpha;
                        u32 dr = dst & 0xFFU;
                        u32 dg = (dst >> 8) & 0xFFU;
                        u32 db = (dst >> 16) & 0xFFU;
                        u32 sr = color & 0xFFU;
                        u32 sg = (color >> 8) & 0xFFU;
                        u32 sb = (color >> 16) & 0xFFU;
                        /* Numerators are <=65025. For that domain this is
                         * exact floor(x/255), avoiding three integer divides. */
                        u32 rr_num = sr * alpha + dr * inv;
                        u32 gg_num = sg * alpha + dg * inv;
                        u32 bb_num = sb * alpha + db * inv;
                        u32 rr = (rr_num + 1U + (rr_num >> 8)) >> 8;
                        u32 gg = (gg_num + 1U + (gg_num >> 8)) >> 8;
                        u32 bb = (bb_num + 1U + (bb_num >> 8)) >> 8;
                        *dstp = COLOR(rr, gg, bb);
                    }
                }
                if (use_scratch)
                    memcpy(frame_row, row_scratch, (size_t)w * sizeof(uint32_t));
            }
            return;
        }
    }

    for (i = 0; i < 4; i++) {
        s32 ox = offsets[i][0];
        s32 oy = offsets[i][1];
        s32 rx = ((ox * cos_val) - (oy * sin_val)) >> 14;
        s32 ry = ((ox * sin_val) + (oy * cos_val)) >> 14;
        s32 fx = center.x + rx;
        s32 fy = center.y + ry;

        if (g_tnt_2p_hold_source_space) {
            /* fx/fy are 8.8 units of the source 10px 2P cell. Convert to
             * 8.8 source-screen pixels, then use the same 83,36 / 11px
             * presentation basis as the exact 400x300 multiplayer image. */
            s64 source_fx =
                (s64)g_tnt_2p_hold_playfield_source_x * 0x100 +
                (s64)fx * g_tnt_hold_source_unit_size;
            s64 source_fy =
                (s64)g_tnt_2p_hold_playfield_source_y * 0x100 +
                (s64)fy * g_tnt_hold_source_unit_size;
            qx[i] = g_tnt_2p_hold_screen_board_x +
                ((double)(source_fx - (s64)83 * 0x100) *
                 g_tnt_2p_hold_screen_cell) / (11.0 * 256.0);
            qy[i] = g_tnt_2p_hold_screen_board_y +
                ((double)(source_fy - (s64)36 * 0x100) *
                 g_tnt_2p_hold_screen_cell) / (11.0 * 256.0);
        } else {
            const double pixels_per_cell = g_tnt_compact_playfield_active
                ? ((double)(8 * TNT_PRESENT_CELL) / 11.0) : (double)cell;
            if (g_tnt_opp60_exact_board) {
                qx[i] = board_x + (double)g_tnt_opp60_phase_x / 11.0 +
                    ((double)fx * (10 * TNT_PRESENT_CELL)) / (11.0 * 256.0);
                qy[i] = board_y + (double)g_tnt_opp60_phase_y / 11.0 +
                    ((double)fy * (10 * TNT_PRESENT_CELL)) / (11.0 * 256.0);
            } else {
                qx[i] = board_x + ((double)fx * pixels_per_cell) / 256.0;
                qy[i] = board_y + ((double)fy * pixels_per_cell) / 256.0;
            }
        }
    }

    /* q0->q1 and q0->q2 are the two affine texture axes. */
    ax = qx[1] - qx[0];
    ay = qy[1] - qy[0];
    bx = qx[2] - qx[0];
    by = qy[2] - qy[0];
    det = ax * by - ay * bx;
    if (det > -0.0001 && det < 0.0001)
        return;

    min_x = max_x = (int)qx[0];
    min_y = max_y = (int)qy[0];
    for (i = 1; i < 4; i++) {
        int ix = (int)qx[i];
        int iy = (int)qy[i];
        if (ix < min_x) min_x = ix;
        if (ix > max_x) max_x = ix;
        if (iy < min_y) min_y = iy;
        if (iy > max_y) max_y = iy;
    }
    min_x -= 1;
    min_y -= 1;
    max_x += 1;
    max_y += 1;

    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= DISPLAY_WIDTH) max_x = DISPLAY_WIDTH - 1;
    if (max_y >= DISPLAY_HEIGHT) max_y = DISPLAY_HEIGHT - 1;
    tnt_vita_opp60_mark(buffer, min_x, min_y, max_x + 1, max_y + 1);

    for (y = min_y; y <= max_y; y++) {
        for (x = min_x; x <= max_x; x++) {
            double px = (x + 0.5) - qx[0];
            double py = (y + 0.5) - qy[0];
            double u = (px * by - py * bx) / det;
            double v = (ax * py - ay * px) / det;
            int tx;
            int ty;
            int idx;
            int texel;
            int out_r;
            int out_g;
            int out_b;
            uint32_t color;

            if (u < 0.0 || u >= 1.0 || v < 0.0 || v >= 1.0)
                continue;

            tx = (int)(u * 8.0);
            ty = (int)(v * 8.0);
            if (tx < 0) tx = 0;
            if (tx > 7) tx = 7;
            if (ty < 0) ty = 0;
            if (ty > 7) ty = 7;
            idx = ty * 8 + tx;

            if (blend == 0) {
                texel = mino_ptr->unkB0[idx];
            } else if (blend == 0xFF) {
                texel = mino_ptr->unkF0[idx];
            } else {
                texel = mino_ptr->unkB0[idx] +
                    (((int)mino_ptr->unkF0[idx] -
                      (int)mino_ptr->unkB0[idx]) * blend >> 8);
            }

            out_r = prim_r + (((env_r - prim_r) * texel) >> 8);
            out_g = prim_g + (((env_g - prim_g) * texel) >> 8);
            out_b = prim_b + (((env_b - prim_b) * texel) >> 8);
            if (out_r < 0) out_r = 0;
            if (out_r > 255) out_r = 255;
            if (out_g < 0) out_g = 0;
            if (out_g > 255) out_g = 255;
            if (out_b < 0) out_b = 0;
            if (out_b > 255) out_b = 255;
            color = COLOR(out_r, out_g, out_b);

            if (alpha == 0xFF) {
                pixels[y * DISPLAY_STRIDE + x] = color;
            } else if (alpha != 0) {
                uint32_t dst = pixels[y * DISPLAY_STRIDE + x];
                int dst_r = dst & 0xFF;
                int dst_g = (dst >> 8) & 0xFF;
                int dst_b = (dst >> 16) & 0xFF;
                int src_r = color & 0xFF;
                int src_g = (color >> 8) & 0xFF;
                int src_b = (color >> 16) & 0xFF;
                int out_a_r = (src_r * alpha + dst_r * (255 - alpha)) / 255;
                int out_a_g = (src_g * alpha + dst_g * (255 - alpha)) / 255;
                int out_a_b = (src_b * alpha + dst_b * (255 - alpha)) / 255;
                pixels[y * DISPLAY_STRIDE + x] =
                    COLOR(out_a_r, out_a_g, out_a_b);
            }
        }
    }
}


static uint32_t tnt_alpha_blend_rgb(
    uint32_t dst,
    u8 src_r,
    u8 src_g,
    u8 src_b,
    u8 src_a
) {
    u32 inv = 255U - src_a;
    u32 dr = dst & 0xFFU;
    u32 dg = (dst >> 8) & 0xFFU;
    u32 db = (dst >> 16) & 0xFFU;
    u32 r = (src_r * src_a + dr * inv + 127U) / 255U;
    u32 g = (src_g * src_a + dg * inv + 127U) / 255U;
    u32 b = (src_b * src_a + db * inv + 127U) / 255U;
    return COLOR(r, g, b);
}

static int tnt_floor_div_s64(s64 n, s64 d) {
    if (n >= 0)
        return (int)(n / d);
    return -(int)(((-n) + d - 1) / d);
}

static void tnt_vita_restore_native_rect_from_scene(
    void *buffer,
    const uint32_t *source,
    int board_x,
    int board_y,
    int cell,
    int native_x0,
    int native_y0,
    int native_x1,
    int native_y1
) {
    uint32_t *dst = (uint32_t *)buffer;
    int dx0 = board_x + tnt_floor_div_s64((s64)(native_x0 - 83) * cell, 11);
    int dy0 = board_y + tnt_floor_div_s64((s64)(native_y0 - 36) * cell, 11);
    int dx1 = board_x + tnt_floor_div_s64((s64)(native_x1 - 83) * cell + 10, 11);
    int dy1 = board_y + tnt_floor_div_s64((s64)(native_y1 - 36) * cell + 10, 11);
    int y;

    if (dx0 < 0) dx0 = 0;
    if (dy0 < 0) dy0 = 0;
    if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
    if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;
    if (dx0 >= dx1 || dy0 >= dy1)
        return;

    for (y = dy0; y < dy1; y++) {
        memcpy(dst + y * DISPLAY_STRIDE + dx0,
               source + y * DISPLAY_STRIDE + dx0,
               (size_t)(dx1 - dx0) * sizeof(uint32_t));
    }
}

static int tnt_vita_gameplay_side_mask_width(u8 gameplay_screen) {
    /* Every restored gameplay environment uses the same conservative CRT
     * presentation crop.  Egyptian (screen 2) used to fall through here,
     * which is why some attract/single-player boards could appear without the
     * side framing even though the surrounding boards had it. */
    return gameplay_screen <= 7 ? 12 : 0;
}

static void tnt_vita_draw_bezel_line_clipped(
    void *buffer, int x0, int y0, int x1, int y1,
    int clip_x0, int clip_x1, uint32_t color
) {
    uint32_t *pixels = (uint32_t *)buffer;
    int dx = x1 > x0 ? x1 - x0 : x0 - x1;
    int sx = x0 < x1 ? 1 : -1;
    int dy_abs = y1 > y0 ? y1 - y0 : y0 - y1;
    int dy = -dy_abs;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    for (;;) {
        if (x0 >= clip_x0 && x0 < clip_x1 &&
            y0 >= 0 && y0 < DISPLAY_HEIGHT) {
            pixels[y0 * DISPLAY_STRIDE + x0] = color;
        }
        if (x0 == x1 && y0 == y1)
            break;
        {
            int e2 = err << 1;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }
}

static void tnt_vita_draw_bezel_grid_panel(
    void *buffer, int x0, int x1, u8 left_side
) {
    /* The side surround borrows the late-80s/early-90s perspective-grid
     * language without borrowing its bright arcade palette.  All tones stay
     * close to black so TNT's board art remains the visual focus. */
    static const s16 horizon_offsets[] = {
        0, 8, 18, 32, 50, 72, 98, 128, 162, 200, 242, 272
    };
    const uint32_t base = COLOR(5, 7, 9);
    const uint32_t grid_minor = COLOR(12, 17, 22);
    const uint32_t grid_major = COLOR(17, 23, 29);
    const uint32_t horizon = COLOR(19, 26, 33);
    const int mid_y = DISPLAY_HEIGHT / 2;
    const int vanish_x = left_side ? x1 + 96 : x0 - 96;
    const int outer_x = left_side ? x0 : x1 - 1;
    int i;

    if (x0 < 0) x0 = 0;
    if (x1 > DISPLAY_WIDTH) x1 = DISPLAY_WIDTH;
    if (x0 >= x1) return;

    draw_rect(buffer, x0, 0, x1 - x0, DISPLAY_HEIGHT, base);

    /* Cross-lines tighten toward the horizon, then open up toward the bezel
     * edges.  The spacing, rather than brightness, carries the perspective. */
    for (i = 0; i < (int)(sizeof(horizon_offsets) / sizeof(horizon_offsets[0])); i++) {
        int off = horizon_offsets[i];
        uint32_t c = (i == 0) ? horizon : ((i % 3) == 0 ? grid_major : grid_minor);
        int y0 = mid_y - off;
        int y1 = mid_y + off;
        if (y0 >= 0) draw_rect(buffer, x0, y0, x1 - x0, 1, c);
        if (off != 0 && y1 < DISPLAY_HEIGHT)
            draw_rect(buffer, x0, y1, x1 - x0, 1, c);
    }

    /* Depth rays converge at a point hidden just beneath the game image.
     * Keeping the vanishing point off-panel makes the visible lines feel like
     * a cropped tunnel instead of a bright starburst. */
    for (i = 0; i <= 8; i++) {
        int end_y = (i * (DISPLAY_HEIGHT - 1)) / 8;
        uint32_t c = (i == 0 || i == 4 || i == 8)
            ? grid_major : grid_minor;
        tnt_vita_draw_bezel_line_clipped(
            buffer, outer_x, end_y, vanish_x, mid_y, x0, x1, c);
    }
}

static void tnt_vita_draw_bezel_between(
    void *buffer, int left_end, int right_start) {
    const uint32_t outer = COLOR(3, 4, 5);
    const uint32_t bevel0 = COLOR(12, 16, 20);
    const uint32_t bevel1 = COLOR(20, 26, 32);
    const uint32_t bevel2 = COLOR(30, 38, 46);

    if (left_end < 0) left_end = 0;
    if (left_end > DISPLAY_WIDTH) left_end = DISPLAY_WIDTH;
    if (right_start < 0) right_start = 0;
    if (right_start > DISPLAY_WIDTH) right_start = DISPLAY_WIDTH;

    tnt_vita_draw_bezel_grid_panel(buffer, 0, left_end, TRUE);
    tnt_vita_draw_bezel_grid_panel(
        buffer, right_start, DISPLAY_WIDTH, FALSE);

    /* Dark outer edge + restrained three-pixel inner rail.  The rail is
     * intentionally brighter than the grid, but still neutral/cool and well
     * below the game's saturated artwork. */
    draw_rect(buffer, 0, 0, 1, DISPLAY_HEIGHT, outer);
    draw_rect(buffer, DISPLAY_WIDTH - 1, 0, 1, DISPLAY_HEIGHT, outer);

    if (left_end >= 3) {
        draw_rect(buffer, left_end - 3, 0, 1, DISPLAY_HEIGHT, bevel0);
        draw_rect(buffer, left_end - 2, 0, 1, DISPLAY_HEIGHT, bevel1);
        draw_rect(buffer, left_end - 1, 0, 1, DISPLAY_HEIGHT, bevel2);
    }
    if (right_start + 2 < DISPLAY_WIDTH) {
        draw_rect(buffer, right_start, 0, 1, DISPLAY_HEIGHT, bevel2);
        draw_rect(buffer, right_start + 1, 0, 1, DISPLAY_HEIGHT, bevel1);
        draw_rect(buffer, right_start + 2, 0, 1, DISPLAY_HEIGHT, bevel0);
    }
}

/* Intro-only bezel base. This intentionally matches the normal cybergrid
 * panels and physical outer-edge treatment while omitting only the three-pixel
 * vertical inner rails. The sand cinematic's narrow shim occupies that
 * boundary during intro playback, so a separate rail there reads as a stray
 * divider. All non-intro states continue using tnt_vita_draw_bezel_between(). */
static void tnt_vita_draw_bezel_between_no_inner_rail(
    void *buffer, int left_end, int right_start) {
    const uint32_t outer = COLOR(3, 4, 5);

    if (left_end < 0) left_end = 0;
    if (left_end > DISPLAY_WIDTH) left_end = DISPLAY_WIDTH;
    if (right_start < 0) right_start = 0;
    if (right_start > DISPLAY_WIDTH) right_start = DISPLAY_WIDTH;

    tnt_vita_draw_bezel_grid_panel(buffer, 0, left_end, TRUE);
    tnt_vita_draw_bezel_grid_panel(
        buffer, right_start, DISPLAY_WIDTH, FALSE);

    /* Preserve only the physical display-edge strokes. */
    draw_rect(buffer, 0, 0, 1, DISPLAY_HEIGHT, outer);
    draw_rect(buffer, DISPLAY_WIDTH - 1, 0, 1, DISPLAY_HEIGHT, outer);
}

/* Intro-only cybergrid shim slice. The normal frontend pillarbox and its
 * three-pixel inner rail remain at their canonical positions. This draws only
 * a narrow continuation of the existing grid pattern into the otherwise-black
 * intro spacer, so the rails never jump when the cinematic begins. */
static void tnt_vita_draw_intro_cybergrid_shim(
    void *buffer, int left_end, int right_start, int width
) {
    static const s16 horizon_offsets[] = {
        0, 8, 18, 32, 50, 72, 98, 128, 162, 200, 242, 272
    };
    const uint32_t base = COLOR(5, 7, 9);
    const uint32_t grid_minor = COLOR(12, 17, 22);
    const uint32_t grid_major = COLOR(17, 23, 29);
    const uint32_t horizon = COLOR(19, 26, 33);
    const int mid_y = DISPLAY_HEIGHT / 2;
    int i;

    if (width <= 0) return;

    {
        int x0 = left_end;
        int x1 = left_end + width;
        int vanish_x = left_end + 96;
        int outer_x = 0;
        if (x0 < 0) x0 = 0;
        if (x1 > DISPLAY_WIDTH) x1 = DISPLAY_WIDTH;
        if (x0 < x1) {
            draw_rect(buffer, x0, 0, x1 - x0, DISPLAY_HEIGHT, base);
            for (i = 0; i < (int)(sizeof(horizon_offsets) / sizeof(horizon_offsets[0])); i++) {
                int off = horizon_offsets[i];
                uint32_t c = (i == 0) ? horizon : ((i % 3) == 0 ? grid_major : grid_minor);
                int y0 = mid_y - off;
                int y1 = mid_y + off;
                if (y0 >= 0) draw_rect(buffer, x0, y0, x1 - x0, 1, c);
                if (off != 0 && y1 < DISPLAY_HEIGHT)
                    draw_rect(buffer, x0, y1, x1 - x0, 1, c);
            }
            for (i = 0; i <= 8; i++) {
                int end_y = (i * (DISPLAY_HEIGHT - 1)) / 8;
                uint32_t c = (i == 0 || i == 4 || i == 8) ? grid_major : grid_minor;
                tnt_vita_draw_bezel_line_clipped(
                    buffer, outer_x, end_y, vanish_x, mid_y, x0, x1, c);
            }
        }
    }

    {
        int x0 = right_start - width;
        int x1 = right_start;
        int vanish_x = right_start - 96;
        int outer_x = DISPLAY_WIDTH - 1;
        if (x0 < 0) x0 = 0;
        if (x1 > DISPLAY_WIDTH) x1 = DISPLAY_WIDTH;
        if (x0 < x1) {
            draw_rect(buffer, x0, 0, x1 - x0, DISPLAY_HEIGHT, base);
            for (i = 0; i < (int)(sizeof(horizon_offsets) / sizeof(horizon_offsets[0])); i++) {
                int off = horizon_offsets[i];
                uint32_t c = (i == 0) ? horizon : ((i % 3) == 0 ? grid_major : grid_minor);
                int y0 = mid_y - off;
                int y1 = mid_y + off;
                if (y0 >= 0) draw_rect(buffer, x0, y0, x1 - x0, 1, c);
                if (off != 0 && y1 < DISPLAY_HEIGHT)
                    draw_rect(buffer, x0, y1, x1 - x0, 1, c);
            }
            for (i = 0; i <= 8; i++) {
                int end_y = (i * (DISPLAY_HEIGHT - 1)) / 8;
                uint32_t c = (i == 0 || i == 4 || i == 8) ? grid_major : grid_minor;
                tnt_vita_draw_bezel_line_clipped(
                    buffer, outer_x, end_y, vanish_x, mid_y, x0, x1, c);
            }
        }
    }
}

static void tnt_vita_draw_frontend_pillarbox(
    void *buffer, int board_x, int cell) {
    int left_end = board_x + tnt_floor_div_s64((s64)(0 - 83) * cell + 10, 11);
    int right_start = board_x + tnt_floor_div_s64((s64)(400 - 83) * cell, 11);

    tnt_vita_draw_bezel_between(buffer, left_end, right_start);
}

/* Tetris-in-the-Sand uses a fixed intro-only cybergrid shim. The source
 * cinematic itself remains untouched; these pixels belong to the surround. */
#define TNT_INTRO_CYBERGRID_SHIM_PX 10

/* Tiny physical-pixel debug font used only in the bezel.  Each glyph is five
 * rows of three bits, scaled 2x, so the counter never touches the N64 image. */
static const u8 g_tnt_debug_font3x5[28][5] = {
    {7,5,5,5,7}, /* 0 */ {2,6,2,2,7}, /* 1 */
    {7,1,7,4,7}, /* 2 */ {7,1,7,1,7}, /* 3 */
    {5,5,7,1,1}, /* 4 */ {7,4,7,1,7}, /* 5 */
    {7,4,7,5,7}, /* 6 */ {7,1,1,1,1}, /* 7 */
    {7,5,7,5,7}, /* 8 */ {7,5,7,1,7}, /* 9 */
    {0,0,0,0,2}, /* . */ {6,4,6,4,4}, /* F */
    {6,5,6,4,4}, /* P */ {3,4,2,1,6}, /* S */
    {0,0,7,0,0}, /* - */ {0,0,0,0,0}, /* space */
    {6,5,6,5,6}, /* B */ {7,4,4,4,7}, /* C */
    {7,4,5,5,7}, /* G */ {6,5,6,5,5}, /* R */
    {5,5,5,7,5}, /* W */ {5,5,5,5,7}, /* U */
    {7,5,5,5,7}, /* O */ {5,5,7,5,5}, /* H */
    {2,5,7,5,5}, /* A */ {5,5,2,5,5}, /* X */
    {5,5,5,5,2}, /* V */ {5,7,7,5,5}  /* M */
};

static int tnt_vita_debug_glyph_index(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c == '.') return 10;
    if (c == 'F') return 11;
    if (c == 'P') return 12;
    if (c == 'S') return 13;
    if (c == '-') return 14;
    if (c == 'B') return 16;
    if (c == 'C') return 17;
    if (c == 'G') return 18;
    if (c == 'R') return 19;
    if (c == 'W') return 20;
    if (c == 'U') return 21;
    if (c == 'O') return 22;
    if (c == 'H') return 23;
    if (c == 'A') return 24;
    if (c == 'X') return 25;
    if (c == 'V') return 26;
    if (c == 'M') return 27;
    return 15;
}

static void tnt_vita_draw_debug_text(void *buffer, int x, int y, const char *text) {
    const int scale = 2;
    const uint32_t color = COLOR(172, 176, 181);
    int pen = x;

    while (*text != '\0') {
        int gi = tnt_vita_debug_glyph_index(*text++);
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 3; col++) {
                if (g_tnt_debug_font3x5[gi][row] & (1U << (2 - col)))
                    draw_rect(buffer, pen + col * scale, y + row * scale,
                              scale, scale, color);
            }
        }
        pen += 4 * scale;
    }
}

/* Test196Q — independent ROM setup screen; original 5x7 diagnostics font.
 * No game ROM data, copyrighted art, fonts, sound, or original UI code.
 * Runs only if verification fails, before game state/saves/audio init.
 */
static const u8 tntv_setup_glyph[40][7] = {
    {14,17,17,31,17,17,17}, /* A */
    {30,17,17,30,17,17,30}, /* B */
    {14,17,16,16,16,17,14}, /* C */
    {30,17,17,17,17,17,30}, /* D */
    {31,16,16,30,16,16,31}, /* E */
    {31,16,16,30,16,16,16}, /* F */
    {14,17,16,23,17,17,14}, /* G */
    {17,17,17,31,17,17,17}, /* H */
    {31,4,4,4,4,4,31}, /* I */
    {7,2,2,2,18,18,12}, /* J */
    {17,18,20,24,20,18,17}, /* K */
    {16,16,16,16,16,16,31}, /* L */
    {17,27,21,21,17,17,17}, /* M */
    {17,25,21,19,17,17,17}, /* N */
    {14,17,17,17,17,17,14}, /* O */
    {30,17,17,30,16,16,16}, /* P */
    {14,17,17,17,21,18,13}, /* Q */
    {30,17,17,30,20,18,17}, /* R */
    {15,16,16,14,1,1,30}, /* S */
    {31,4,4,4,4,4,4}, /* T */
    {17,17,17,17,17,17,14}, /* U */
    {17,17,17,17,17,10,4}, /* V */
    {17,17,17,21,21,21,10}, /* W */
    {17,17,10,4,10,17,17}, /* X */
    {17,17,10,4,4,4,4}, /* Y */
    {31,1,2,4,8,16,31}, /* Z */
    {14,17,19,21,25,17,14}, /* 0 */
    {4,12,4,4,4,4,14}, /* 1 */
    {14,17,1,2,4,8,31}, /* 2 */
    {30,1,1,14,1,1,30}, /* 3 */
    {2,6,10,18,31,2,2}, /* 4 */
    {31,16,16,30,1,1,30}, /* 5 */
    {14,16,16,30,17,17,14}, /* 6 */
    {31,1,2,4,8,8,8}, /* 7 */
    {14,17,17,14,17,17,14}, /* 8 */
    {14,17,17,15,1,1,14}, /* 9 */
    {0,0,0,0,0,6,6}, /* . */
    {0,0,0,31,0,0,0}, /* - */
    {0,4,4,0,4,4,0}, /* : */
    {1,1,2,4,8,16,16} /* / */
};
static int tntv_setup_glyph_index(char c) {
    if(c>='A' && c<='Z')return c-'A';
    if(c>='0' && c<='9')return 26+c-'0';
    if(c=='.')return 36;
    if(c=='-')return 37;
    if(c==':')return 38;
    if(c=='/')return 39;
    return -1;
}
static void tntv_setup_line(void *buffer, int y, const char *line, u32 color) {
    int length=0,scale=3;
    while(line[length])length++;
    /* 5 lit columns plus one inter-character space, always screen centered. */
    int x=(DISPLAY_WIDTH-length*6*scale)/2;
    if(x<0)x=0;
    for(int i=0;i<length;i++,x+=6*scale) {
        int idx=tntv_setup_glyph_index(line[i]);
        if(idx<0)continue;
        for(int r=0;r<7;r++)
            for(int c=0;c<5;c++)
                if(tntv_setup_glyph[idx][r]&(1U<<(4-c)))
                    draw_rect(buffer,x+c*scale,y+r*scale,scale,scale,color);
    }
}
/* Reuse the exact approved muted cybergrid line work and palette for the
 * full-screen bootstrap view. It is generated procedurally, so no original
 * ROM assets or startup caches are required to display the loading UI. */
static void tntv_boot_cybergrid(void *buffer) {
    const int center=DISPLAY_WIDTH/2;
    tnt_vita_draw_bezel_grid_panel(buffer,0,center,TRUE);
    tnt_vita_draw_bezel_grid_panel(buffer,center,DISPLAY_WIDTH,FALSE);
}
/* Original, asset-independent loading UI; uses the setup display path. */
static unsigned tntv_loading_frame;
static void tntv_rom_loading_progress(const char *stage) {
    void *buffer=buffers[tntv_loading_frame++ % 2].data;
    void *present=buffer;
    char label[40];
    if(!buffer)return;
    tntv_boot_cybergrid(buffer);
    draw_rect(buffer,110,76,740,3,COLOR(40,150,170));
    tntv_setup_line(buffer,101,"TNTV LOADING",COLOR(178,230,234));
    tntv_setup_line(buffer,180,"PREPARING YOUR ROM",COLOR(222,222,224));
    if(strncmp(stage,"205D SCENE STEP ",16)==0) {
        snprintf(label,sizeof(label),"SCENE STEP %.3s",stage+16);
    } else if(strstr(stage,"SCENE CACHE HIT")) strcpy(label,"LOADING SAVED SCENES");
    else if(strstr(stage,"SAVING SCENES")) strcpy(label,"SAVING SCENES FOR NEXT TIME");
    else if(strstr(stage,"FAST ROM FINGERPRINT")) strcpy(label,"ROM QUICK CHECK OK");
    else if(strstr(stage,"FULL ROM HASH")) strcpy(label,"VERIFYING ENTIRE ROM");
    else if(strstr(stage,"FULL ROM SHA256")) strcpy(label,"ROM FULL CHECK OK");
    else if(strstr(stage,"FOUND")) strcpy(label,"CHECKING ROM");
    else if(strstr(stage,"SCENES VERIFIED")) strcpy(label,"CHECKING ART CACHE");
    else if(strstr(stage,"generating local native")) strcpy(label,"BUILDING ART CACHE");
    else if(strstr(stage,"CACHED ART VERIFIED")) strcpy(label,"LOADING ART CACHE");
    else if(strstr(stage,"ART BOUND") || strstr(stage,"game music")) strcpy(label,"PREPARING MUSIC");
    else if(strstr(stage,"PASS")) strcpy(label,"STARTING GAME");
    else strcpy(label,"PREPARING SCENES");
    tntv_setup_line(buffer,250,label,COLOR(178,230,234));
    tntv_setup_line(buffer,350,"FIRST SETUP MAY TAKE LONGER",COLOR(222,222,224));
    display_callback(&present);
    sceDisplayWaitVblankStart();
}

static int tntv_rom_setup_screen(int error_code, int *graphics_ready) {
    SceCtrlData ctrl;
    u32 old_buttons=0;
    void *buffer;
    void *present;
    if(!graphics_ready)return 0;
    if(!*graphics_ready) {
        if(graphics_init()<0)return 0;
        *graphics_ready=1;
    }
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    buffer=buffers[0].data;
    if(!buffer)return 0;
    tntv_boot_cybergrid(buffer);
    draw_rect(buffer,110,76,740,3,COLOR(40,150,170));
    tntv_setup_line(buffer,101,"TNTV ROM SETUP",COLOR(178,230,234));
    tntv_setup_line(buffer,150,error_code==-1?"ROM FILE MISSING":
        error_code==-2?"ROM VALIDATION FAILED":"ROM DATA SETUP ERROR",
        COLOR(245,184,143));
    tntv_setup_line(buffer,208,"ORIGINAL USA 1.0 ROM REQUIRED",COLOR(222,222,224));
    tntv_setup_line(buffer,246,"COPY YOUR ROM USING VITASHELL",COLOR(222,222,224));
    tntv_setup_line(buffer,285,"UX0:DATA/TNTV/",COLOR(178,230,234));
    tntv_setup_line(buffer,320,"NAME IT BASEROM.Z64",COLOR(222,222,224));
    tntv_setup_line(buffer,353,"OR BASEROM.V64 OR BASEROM.N64",COLOR(222,222,224));
    tntv_setup_line(buffer,399,"PRESS X TO RETRY",COLOR(178,230,234));
    tntv_setup_line(buffer,433,"PRESS O TO EXIT",COLOR(180,181,187));
    draw_rect(buffer,110,478,740,2,COLOR(40,150,170));
    /* An X press used to launch TNTV may still be physically held. Latch
     * current buttons before first poll so the user must release and press
     * again to retry, instead of triggering an invisible retry loop. */
    memset(&ctrl,0,sizeof(ctrl));
    if(sceCtrlPeekBufferPositive(0,&ctrl,1)>0)
        old_buttons=ctrl.buttons;
    present=buffer;
    /* Direct display before the normal game's display queue is active. */
    display_callback(&present);
    sceDisplayWaitVblankStart();
    for(;;) {
        u32 buttons=0;
        memset(&ctrl,0,sizeof(ctrl));
        if(sceCtrlPeekBufferPositive(0,&ctrl,1)>0)
            buttons=ctrl.buttons;
        if((buttons&SCE_CTRL_CROSS) && !(old_buttons&SCE_CTRL_CROSS))
            return 1;
        if((buttons&SCE_CTRL_CIRCLE) && !(old_buttons&SCE_CTRL_CIRCLE))
            return 0;
        old_buttons=buttons;
        (void)sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DEFAULT);
        sceDisplayWaitVblankStart();
    }
}

/* TEST103A, EXTERNAL ISOLATED DEVELOPMENT WORKSPACE ONLY:
 * one-player timing profiler. Production test102 sources do not include it.
 * GPU's finish wait is a SUBSET of background (B), not a separate slice.
 * Presentation (P) includes bezel, FPS HUD, queue and VBlank waiting. */
enum {
    TNT103A_UPDATE, TNT103A_BACKGROUND, TNT103A_GPUWAIT,
    TNT103A_LOCKED, TNT103A_EXTRA, TNT103A_PRESENT, TNT103A_TOTAL,
    TNT103A_COUNT
};
u8 g_tnt_test103a_measure_gxm;
u32 g_tnt_test103a_gpu_wait_us;
static u8 g_tnt103a_1p_active;
/* TEST110A: isolated Mayan 1P persistent cybergrid in double-buffered
 * CDRAM. The GXM Mayan draw clips to its 400x300 source viewport,
 * leaving almost all of the side panels untouched. Full redraw of the
 * static panels need only occur once per display buffer on entering
 * gameplay or a geometry change. A narrow strip under the GXM clip and
 * the top-left diagnostics area are restored every frame. Everything
 * else in the original bezel remains physically resident in that
 * framebuffer. Disable entirely outside 1P GXM Mayan gameplay. */
static u8 g_tnt110_mayan_bezel_eligible;
static u8 g_tnt110_panel_valid[DISPLAY_BUFFER_COUNT];
static int g_tnt110_panel_left=-1,g_tnt110_panel_right=-1;
static u32 *g_tnt110_panel_master;
static u32 g_tnt110_warm_count,g_tnt110_rebuild_count;
static u32 g_tnt112_written_pixel_count;
static u32 g_tnt110_border_avg_us,g_tnt110_border_sum_us,g_tnt110_border_samples;
static u64 g_tnt110_border_last_window;

static u64 g_tnt103a_window_start;
static u64 g_tnt103a_sums[TNT103A_COUNT];
static u32 g_tnt103a_avg[TNT103A_COUNT];
static u32 g_tnt103a_count;
static u32 g_tnt103a_peak_total;
static u32 g_tnt103a_last_peak_total;

static void tnt103a_sample(const u32 sample[TNT103A_COUNT]) {
    u64 now = sceKernelGetProcessTimeWide();
    if (g_tnt103a_window_start == 0)
        g_tnt103a_window_start = now;
    for (int i = 0; i < TNT103A_COUNT; ++i)
        g_tnt103a_sums[i] += sample[i];
    if (sample[TNT103A_TOTAL] > g_tnt103a_peak_total)
        g_tnt103a_peak_total = sample[TNT103A_TOTAL];
    ++g_tnt103a_count;
    if (now - g_tnt103a_window_start >= 500000U) {
        for (int i = 0; i < TNT103A_COUNT; ++i) {
            g_tnt103a_avg[i] = (u32)(
                g_tnt103a_sums[i] / g_tnt103a_count);
            g_tnt103a_sums[i] = 0;
        }
        g_tnt103a_last_peak_total = g_tnt103a_peak_total;
        g_tnt103a_peak_total = 0;
        g_tnt103a_count = 0;
        g_tnt103a_window_start = now;
    }
}

/* TEST103B fine-sliced one-player profiling, all experimental.
 * S Mayan sky/GXM; I name+small fire; F large fire;
 * H HUD; N next queue; K hold; C current+ghost; Q glows+finish;
 * R remaining 1P overlays, pause and crop state.
 * This is purely observational, no rendering path changes. */
enum {
    TNT103B_SKY, TNT103B_MINOR, TNT103B_FIRE,
    TNT103B_HUD, TNT103B_NEXT, TNT103B_HOLD,
    TNT103B_CURRENT, TNT103B_GLOW, TNT103B_OTHER,
    TNT103B_COUNT
};
static u32 g_tnt103b_frame[TNT103B_COUNT];
static u64 g_tnt103b_sum[TNT103B_COUNT];
static u32 g_tnt103b_avg[TNT103B_COUNT];
static u64 g_tnt103b_start;
static u32 g_tnt103b_count;

static void tnt103b_mark(int index, u64 *stamp) {
    u64 now = sceKernelGetProcessTimeWide();
    g_tnt103b_frame[index] += (u32)(now - *stamp);
    *stamp = now;
}
static void tnt103b_sample(void) {
    u64 now = sceKernelGetProcessTimeWide();
    if (g_tnt103b_start == 0) g_tnt103b_start = now;
    for (int i = 0; i < TNT103B_COUNT; ++i)
        g_tnt103b_sum[i] += g_tnt103b_frame[i];
    ++g_tnt103b_count;
    if (now - g_tnt103b_start >= 500000U) {
        for (int i = 0; i < TNT103B_COUNT; ++i) {
            g_tnt103b_avg[i] = (u32)(
                g_tnt103b_sum[i] / g_tnt103b_count);
            g_tnt103b_sum[i] = 0;
        }
        g_tnt103b_count = 0;
        g_tnt103b_start = now;
    }
}
static u8 g_tnt_fps_counter_enabled;
static u8 g_tnt_fps_combo_down;
static u8 g_tnt_presentation_crop;
static u8 g_tnt_intro_shim_active;
/* Boot-only presentation flag: suppress the normal inner vertical bezel rails
 * from the very first black/developer frame until the first sand intro hands
 * off to the title. It is never re-enabled, so normal post-intro frontend
 * screens keep the approved rail treatment. */
static u8 g_tnt_boot_no_inner_rail_active = TRUE;
static u64 g_tnt_fps_window_start_us;
static u32 g_tnt_fps_window_frames;
static u32 g_tnt_fps_x10 = 600;
static u8 g_tnt_wonder_profile_active;
static u32 g_tnt_wonder_clear_us, g_tnt_wonder_frame_us;

/* Temporary two-board profiler. Half-second averages in microseconds.
 * Active only with the developer FPS counter in OPPONENT or local MULTI.
 * AMax preserves the worst single AI-call spike; it remains zero in MULTI. */
static u8 g_tnt_ai_profile_active;
static char g_tnt_2p_prof_mode = '-';
static u64 g_tnt_ai_prof_window_start_us;
static u64 g_tnt_ai_prof_sum_h, g_tnt_ai_prof_sum_a, g_tnt_ai_prof_sum_c;
static u64 g_tnt_ai_prof_sum_b, g_tnt_ai_prof_sum_p0, g_tnt_ai_prof_sum_p1;
static u64 g_tnt_ai_prof_sum_x, g_tnt_ai_prof_sum_v;
static u32 g_tnt_ai_prof_count, g_tnt_ai_prof_max_a_window;
static u32 g_tnt_ai_prof_h_us, g_tnt_ai_prof_a_us, g_tnt_ai_prof_c_us;
static u32 g_tnt_ai_prof_b_us, g_tnt_ai_prof_p0_us, g_tnt_ai_prof_p1_us;
static u32 g_tnt_ai_prof_x_us, g_tnt_ai_prof_v_us, g_tnt_ai_prof_amax_us;

static void tnt_vita_ai_prof_reset(void) {
    g_tnt_ai_prof_window_start_us = 0;
    g_tnt_ai_prof_sum_h = g_tnt_ai_prof_sum_a = g_tnt_ai_prof_sum_c = 0;
    g_tnt_ai_prof_sum_b = g_tnt_ai_prof_sum_p0 = g_tnt_ai_prof_sum_p1 = 0;
    g_tnt_ai_prof_sum_x = g_tnt_ai_prof_sum_v = 0;
    g_tnt_ai_prof_count = 0;
    g_tnt_ai_prof_max_a_window = 0;
    g_tnt_ai_prof_h_us = g_tnt_ai_prof_a_us = g_tnt_ai_prof_c_us = 0;
    g_tnt_ai_prof_b_us = g_tnt_ai_prof_p0_us = g_tnt_ai_prof_p1_us = 0;
    g_tnt_ai_prof_x_us = g_tnt_ai_prof_v_us = g_tnt_ai_prof_amax_us = 0;
}

static void tnt_vita_ai_prof_sample(
    u32 h, u32 a, u32 c, u32 b, u32 p0, u32 p1, u32 x, u32 v
) {
    u64 now = sceKernelGetProcessTimeWide();
    if (g_tnt_ai_prof_window_start_us == 0)
        g_tnt_ai_prof_window_start_us = now;
    g_tnt_ai_prof_sum_h += h; g_tnt_ai_prof_sum_a += a; g_tnt_ai_prof_sum_c += c;
    g_tnt_ai_prof_sum_b += b; g_tnt_ai_prof_sum_p0 += p0; g_tnt_ai_prof_sum_p1 += p1;
    g_tnt_ai_prof_sum_x += x; g_tnt_ai_prof_sum_v += v;
    g_tnt_ai_prof_count++;
    if (a > g_tnt_ai_prof_max_a_window) g_tnt_ai_prof_max_a_window = a;
    if (now - g_tnt_ai_prof_window_start_us >= 500000U && g_tnt_ai_prof_count) {
        u32 n = g_tnt_ai_prof_count;
        g_tnt_ai_prof_h_us=(u32)(g_tnt_ai_prof_sum_h/n);
        g_tnt_ai_prof_a_us=(u32)(g_tnt_ai_prof_sum_a/n);
        g_tnt_ai_prof_c_us=(u32)(g_tnt_ai_prof_sum_c/n);
        g_tnt_ai_prof_b_us=(u32)(g_tnt_ai_prof_sum_b/n);
        g_tnt_ai_prof_p0_us=(u32)(g_tnt_ai_prof_sum_p0/n);
        g_tnt_ai_prof_p1_us=(u32)(g_tnt_ai_prof_sum_p1/n);
        g_tnt_ai_prof_x_us=(u32)(g_tnt_ai_prof_sum_x/n);
        g_tnt_ai_prof_v_us=(u32)(g_tnt_ai_prof_sum_v/n);
        g_tnt_ai_prof_amax_us=g_tnt_ai_prof_max_a_window;
        g_tnt_ai_prof_window_start_us=now;
        g_tnt_ai_prof_sum_h=g_tnt_ai_prof_sum_a=g_tnt_ai_prof_sum_c=0;
        g_tnt_ai_prof_sum_b=g_tnt_ai_prof_sum_p0=g_tnt_ai_prof_sum_p1=0;
        g_tnt_ai_prof_sum_x=g_tnt_ai_prof_sum_v=0;
        g_tnt_ai_prof_count=0; g_tnt_ai_prof_max_a_window=0;
    }
}

static s16 tnt_vita_credits_sins(u16 angle);

static void tnt_vita_draw_alpha_rect(
    void *buffer, int x, int y, int w, int h, u8 r, u8 g, u8 b, u8 a
) {
    u32 *pixels=(u32 *)buffer;
    int py,px;
    if (a==0) return;
    for (py=y;py<y+h;py++) {
        if (py<0 || py>=DISPLAY_HEIGHT) continue;
        for (px=x;px<x+w;px++) {
            u32 d; u8 dr,dg,db; u32 ia;
            if (px<0 || px>=DISPLAY_WIDTH) continue;
            d=pixels[py*DISPLAY_STRIDE+px]; dr=(u8)d; dg=(u8)(d>>8); db=(u8)(d>>16); ia=255U-a;
            pixels[py*DISPLAY_STRIDE+px]=COLOR(
                (u8)(((u32)r*a+(u32)dr*ia+127U)/255U),
                (u8)(((u32)g*a+(u32)dg*ia+127U)/255U),
                (u8)(((u32)b*a+(u32)db*ia+127U)/255U));
        }
    }
}

static int tnt_vita_source_x(int sx,int board_x,int cell) {
    return board_x + tnt_floor_div_s64((s64)(sx-83)*cell,11);
}
static int tnt_vita_source_y(int sy,int board_y,int cell) {
    return board_y + tnt_floor_div_s64((s64)(sy-36)*cell,11);
}

static void tnt_vita_hotpotato_render(
    void *buffer, Landfill *landfill, int board_x, int board_y, int cell
) {
    int sx,x0,x1,y0,y1,tx0,tx1,ty0,ty1;
    u8 alpha;
    if (!landfill || landfill->type != LANDFILLTYPE_HOTPOTATO || g_playercount != 2) return;
    HotPotato_Update(&landfill->unk30);
    sx = landfill->unk30.unk0 == 0 ? 57 : 247;
    /* Source: alpha=(sins(angle)>>9)+160, angle += 0x800 per render. */
    alpha=(u8)(((s32)tnt_vita_credits_sins(landfill->unk30.unk12)>>9)+160);
    landfill->unk30.unk8=alpha; landfill->unk30.unk12=(u16)(landfill->unk30.unk12+0x800U);
    x0=tnt_vita_source_x(sx,board_x,cell); x1=tnt_vita_source_x(sx+100,board_x,cell);
    y0=tnt_vita_source_y(43,board_y,cell); y1=tnt_vita_source_y(246,board_y,cell);
    tx0=tnt_vita_source_x(sx-3,board_x,cell); tx1=tnt_vita_source_x(sx,board_x,cell);
    ty0=tnt_vita_source_y(243,board_y,cell); ty1=tnt_vita_source_y(246,board_y,cell);
    if (tx1<=tx0) tx1=tx0+1;
    if (ty1<=ty0) ty1=ty0+1;
    tnt_vita_draw_alpha_rect(buffer,tx0,y0,tx1-tx0,y1-y0,255,0,0,alpha);
    tx0=tnt_vita_source_x(sx+100,board_x,cell); tx1=tnt_vita_source_x(sx+103,board_x,cell);
    if (tx1<=tx0) tx1=tx0+1;
    tnt_vita_draw_alpha_rect(buffer,tx0,y0,tx1-tx0,y1-y0,255,0,0,alpha);
    tnt_vita_draw_alpha_rect(buffer,x0,ty0,x1-x0,ty1-ty0,255,0,0,alpha);
}

/* Test83: the translucent Hot Potato region consists solely of a fixed
 * red tint over the ALREADY-COMPOSED STATIC N64 background. In test82 the
 * ordinary per-pixel alpha rectangle cost ~30ms every frame on PSTV, causing
 * gameplay to fall from 30 to 15 FPS. Preblend the three potential holder
 * backgrounds ONCE at match entry; then copy just the selected region on each
 * frame. This is byte-identical to the original per-pixel blend and renders
 * BEFORE all minos, as the N64 does. Pulsing rails stay live animated. */
typedef struct {
    u32 *pixels;
    int x, y, w, h;
} TNT3PHotPotatoTint;
static TNT3PHotPotatoTint g_tnt_3p_hp_tint[4];

static void __attribute__((optimize("Os")))
tnt_vita_3p_hp_tint_free(void) {
    int i;
    for (i = 0; i < 4; ++i) {
        free(g_tnt_3p_hp_tint[i].pixels);
        memset(&g_tnt_3p_hp_tint[i], 0,
            sizeof(g_tnt_3p_hp_tint[i]));
    }
}

static void __attribute__((optimize("Os")))
tnt_vita_3p_hp_tint_prepare(int board_x, int board_y, int cell) {
    static const s16 source_x[4] = {34, 118, 202, 286};
    int p, row, x;
    const int count = g_playercount == 4 ? 4 : 3;
    const int y0 = tnt_vita_source_y(68, board_y, cell);
    const int y1 = tnt_vita_source_y(228, board_y, cell);
    const u32 red_alpha = 32U;
    const u32 inv_alpha = 255U - red_alpha;
    tnt_vita_3p_hp_tint_free();
    if (g_playercount == 4)
        g_tnt_4p_hp_cache_mask = 0;
    for (p = 0; p < count; ++p) {
        TNT3PHotPotatoTint *entry = &g_tnt_3p_hp_tint[p];
        int x0 = tnt_vita_source_x(source_x[p], board_x, cell);
        int x1 = tnt_vita_source_x(source_x[p] + 80, board_x, cell);
        int w = x1 - x0, h = y1 - y0;
        if (w <= 0 || h <= 0 || x0 < 0 || y0 < 0 ||
            x1 > DISPLAY_WIDTH || y1 > DISPLAY_HEIGHT)
            continue;
        entry->pixels = malloc((size_t)w * (size_t)h * sizeof(u32));
        if (entry->pixels == NULL)
            continue; /* Safe fallback to original blend if memory is low. */
        if (g_playercount == 4)
            g_tnt_4p_hp_cache_mask |= (u8)(1U << p);
        entry->x = x0;
        entry->y = y0;
        entry->w = w;
        entry->h = h;
        for (row = 0; row < h; ++row) {
            const u32 *src = g_tnt_static_scene +
                (size_t)(y0 + row) * DISPLAY_STRIDE + x0;
            u32 *dst = entry->pixels + (size_t)row * w;
            for (x = 0; x < w; ++x) {
                u32 pixel = src[x];
                u32 red = ((pixel & 255U) * inv_alpha +
                    255U * red_alpha + 127U) / 255U;
                u32 green = (((pixel >> 8) & 255U) * inv_alpha +
                    127U) / 255U;
                u32 blue = (((pixel >> 16) & 255U) * inv_alpha +
                    127U) / 255U;
                dst[x] = COLOR(red, green, blue);
            }
        }
    }
}

static u8 __attribute__((optimize("Os")))
tnt_vita_3p_hp_tint_copy(void *buffer, u8 player,
    int x, int y, int w, int h) {
    const TNT3PHotPotatoTint *entry;
    u32 *dest = buffer;
    int row;
    if (player >= 4) return FALSE;
    entry = &g_tnt_3p_hp_tint[player];
    if (entry->pixels == NULL || entry->x != x || entry->y != y ||
        entry->w != w || entry->h != h)
        return FALSE;
    for (row = 0; row < h; ++row) {
        memcpy(dest + (size_t)(y + row) * DISPLAY_STRIDE + x,
            entry->pixels + (size_t)row * w,
            (size_t)w * sizeof(u32));
    }
    return TRUE;
}

/* Test84: the two narrow pulsating 3P Hot Potato rails have only 32
 * possible red alpha values (phase advances exactly 0x800 every frame).
 * Precompute exact rounded /255 channel results for each 8-bit input.
 * This avoids per-pixel three-channel blending arithmetic on every render.
 * LUT uses ~16 KiB and preserves the original alpha/position/pulse. */
static u8 g_tnt_3p_hp_pulse_red[32][256];
static u8 g_tnt_3p_hp_pulse_other[32][256];
static u8 g_tnt_3p_hp_pulse_alpha[32];
static u8 g_tnt_3p_hp_pulse_ready;

static void __attribute__((optimize("Os")))
tnt_vita_3p_hp_pulse_prepare(void) {
    unsigned phase, v;
    if (g_tnt_3p_hp_pulse_ready) return;
    for (phase = 0; phase < 32; ++phase) {
        u8 a = (u8)(((s32)tnt_vita_credits_sins(
            (u16)(phase << 11)) >> 9) + 160);
        u32 inv = 255U - a;
        g_tnt_3p_hp_pulse_alpha[phase] = a;
        for (v = 0; v < 256; ++v) {
            g_tnt_3p_hp_pulse_red[phase][v] =
                (u8)((255U * a + v * inv + 127U) / 255U);
            g_tnt_3p_hp_pulse_other[phase][v] =
                (u8)((v * inv + 127U) / 255U);
        }
    }
    g_tnt_3p_hp_pulse_ready = TRUE;
}

static void __attribute__((optimize("Os")))
tnt_vita_3p_hp_pulse_rail(
    void *buffer, int x, int y, int w, int h, u8 a, u8 phase
) {
    u32 *pixels = (u32 *)buffer;
    const u8 *red, *other;
    int py, px;
    if (a == 0) return;
    if (!g_tnt_3p_hp_pulse_ready ||
        g_tnt_3p_hp_pulse_alpha[phase] != a ||
        w <= 0 || h <= 0 || x < 0 || y < 0 ||
        x + w > DISPLAY_WIDTH || y + h > DISPLAY_HEIGHT) {
        /* Includes frozen initial frames with uninitialized pulse alpha.
         * Preserve original exact clipped draw for any exceptional case. */
        tnt_vita_draw_alpha_rect(buffer, x, y, w, h, 255, 0, 0, a);
        return;
    }
    red = g_tnt_3p_hp_pulse_red[phase];
    other = g_tnt_3p_hp_pulse_other[phase];
    for (py = y; py < y + h; ++py) {
        u32 *line = pixels + (size_t)py * DISPLAY_STRIDE + x;
        for (px = 0; px < w; ++px) {
            u32 d = line[px];
            line[px] = COLOR(red[d & 255U],
                other[(d >> 8) & 255U],
                other[(d >> 16) & 255U]);
        }
    }
}

/* Source hotpotato.c func_80074024: identical optimized compact
 * 8-unit wells. Test93 extends the validated 3P x34/118/202
 * tint/rails to the fourth x286 well; 3P behavior is unchanged.
 * Tint and rails render before all pieces like the N64 original. */
static void __attribute__((optimize("Os"))) tnt_vita_3p_hotpotato_render(
    void *buffer, Landfill *landfill, int board_x, int board_y, int cell,
    u8 frozen, u8 body_already_restored
) {
    static const s16 source_x[4] = {34, 118, 202, 286};
    s32 alpha;
    int x, y, bottom, right, stripe_left, stripe_right;
    u8 player, pulse_phase;
    if (landfill == NULL || landfill->type != LANDFILLTYPE_HOTPOTATO ||
        (g_playercount != 3 && g_playercount != 4) ||
        landfill->list == NULL ||
        landfill->garbage_idx < g_playercount)
        return;

    /* N64 HotPotato_Render advances recipient once after a qualifying
     * holder line clear or top-out. Frozen pause/viewer frames must not
     * process that pending handoff until gameplay resumes. */
    /* 4P fused restoration must know the current recipient BEFORE
     * selecting the background row source. It executes the original
     * update just before restore; 3P still updates at this exact point. */
    if (!frozen && g_playercount == 3)
        HotPotato_Update(&landfill->unk30);
    player = landfill->unk30.unk0;
    if (player >= g_playercount) return;
    if (!frozen) {
        alpha = ((s32)tnt_vita_credits_sins(landfill->unk30.unk12) >> 9) + 160;
        landfill->unk30.unk8 = (u8)alpha;
        landfill->unk30.unk12 = (u16)(landfill->unk30.unk12 + 0x800U);
    }
    alpha = landfill->unk30.unk8;
    pulse_phase = (u8)(((u16)(landfill->unk30.unk12 - 0x800U) >> 11) & 31U);
    x = tnt_vita_source_x(source_x[player], board_x, cell);
    right = tnt_vita_source_x(source_x[player] + 80, board_x, cell);
    stripe_left = tnt_vita_source_x(source_x[player] - 3, board_x, cell);
    stripe_right = tnt_vita_source_x(source_x[player] + 83, board_x, cell);
    y = tnt_vita_source_y(68, board_y, cell);
    bottom = tnt_vita_source_y(228, board_y, cell);

    /* Pixel-identical red body is copied from the static preblend. The
     * fallback preserves correctness if a cache allocation ever fails. */
    if (!body_already_restored &&
        !tnt_vita_3p_hp_tint_copy(
            buffer, player, x, y, right - x, bottom - y)) {
        if (g_playercount == 4)
            g_tnt_4p_hp_fallbacks++;
        tnt_vita_draw_alpha_rect(buffer, x, y,
            right - x, bottom - y, 255, 0, 0, landfill->unk30.unk10);
    }
    /* Retail pulse uses sin(angle)>>9 + 160 on two 3px vertical rails.
     * Its y228 bottom line is below the 160px board, hence the +3px. */
    tnt_vita_3p_hp_pulse_rail(buffer, stripe_left, y,
        x - stripe_left, tnt_vita_source_y(231, board_y, cell) - y,
        (u8)alpha, pulse_phase);
    tnt_vita_3p_hp_pulse_rail(buffer, right, y,
        stripe_right - right, tnt_vita_source_y(231, board_y, cell) - y,
        (u8)alpha, pulse_phase);
}

/* RELEASE CANDIDATE TEST124A: public FPS toggle is independent of dev
 * profilers. Retain original half-second rolling window and L+R+SELECT
 * chord, but do not draw internal frame slices, controller ports, garbage
 * stats, Mayan/multiplayer probes, or proprietary diagnostics in public. */
static void tnt_vita_update_and_draw_fps(void *buffer) {
    u64 now_us = sceKernelGetProcessTimeWide();

    if (g_tnt_fps_window_start_us == 0)
        g_tnt_fps_window_start_us = now_us;
    g_tnt_fps_window_frames++;

    if (now_us - g_tnt_fps_window_start_us >= 500000U) {
        u64 elapsed = now_us - g_tnt_fps_window_start_us;
        g_tnt_fps_x10 = (u32)(
            ((u64)g_tnt_fps_window_frames * 10000000U + elapsed / 2U) / elapsed);
        g_tnt_fps_window_start_us = now_us;
        g_tnt_fps_window_frames = 0;
    }

    if (g_tnt_fps_counter_enabled) {
        char text[96];
        snprintf(text, sizeof(text), "%u.%u FPS",
                 (unsigned)(g_tnt_fps_x10 / 10U),
                 (unsigned)(g_tnt_fps_x10 % 10U));
        tnt_vita_draw_debug_text(buffer, 14, 14, text);
        /* PRIVATE Opponent60 test instrumentation, microseconds per frame.
         * Enable only with L+R+SELECT. No game logic or clock changes.
         * H=human update, A=AI, C=CPU update, B=scene/HUD,
         * P0/P1=board draw, X=overlay, V=present/VBlank. */
        if (g_tnt_ai_profile_active && g_tnt_2p_prof_mode == 79) {
            /* Five narrow lines: the old combined debug strings ran
             * outside the left pillarbox and across the 2P backdrop. */
            snprintf(text, sizeof(text), "H%u A%u",
                     (unsigned)g_tnt_ai_prof_h_us,
                     (unsigned)g_tnt_ai_prof_a_us);
            tnt_vita_draw_debug_text(buffer, 14, 31, text);
            snprintf(text, sizeof(text), "C%u B%u",
                     (unsigned)g_tnt_ai_prof_c_us,
                     (unsigned)g_tnt_ai_prof_b_us);
            tnt_vita_draw_debug_text(buffer, 14, 48, text);
            snprintf(text, sizeof(text), "P0%u P1%u",
                     (unsigned)g_tnt_ai_prof_p0_us,
                     (unsigned)g_tnt_ai_prof_p1_us);
            tnt_vita_draw_debug_text(buffer, 14, 65, text);
            snprintf(text, sizeof(text), "X%u V%u",
                     (unsigned)g_tnt_ai_prof_x_us,
                     (unsigned)g_tnt_ai_prof_v_us);
            tnt_vita_draw_debug_text(buffer, 14, 82, text);
            snprintf(text, sizeof(text), "M%u",
                     (unsigned)g_tnt_ai_prof_amax_us);
            tnt_vita_draw_debug_text(buffer, 14, 99, text);
            snprintf(text, sizeof(text), "L%u/%u",
                     (unsigned)g_tnt_opp60_locked_us[0],
                     (unsigned)g_tnt_opp60_locked_us[1]);
            tnt_vita_draw_debug_text(buffer, 14, 116, text);
            snprintf(text, sizeof(text), "N%u/%u",
                     (unsigned)g_tnt_opp60_preview_us[0],
                     (unsigned)g_tnt_opp60_preview_us[1]);
            tnt_vita_draw_debug_text(buffer, 14, 133, text);
            snprintf(text, sizeof(text), "R%u/%u",
                     (unsigned)g_tnt_opp60_other_us[0],
                     (unsigned)g_tnt_opp60_other_us[1]);
            tnt_vita_draw_debug_text(buffer, 14, 150, text);
        }
    }
}

static void tnt_vita_draw_original_ia8_sprite(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    int src_x,
    int src_y,
    int width,
    int height,
    const u8 *pixels,
    u8 global_alpha
) {
    uint32_t *dst = (uint32_t *)buffer;
    int dx0 = board_x + tnt_floor_div_s64((s64)(src_x - 83) * cell, 11);
    int dy0 = board_y + tnt_floor_div_s64((s64)(src_y - 36) * cell, 11);
    int dx1 = board_x + tnt_floor_div_s64((s64)(src_x + width - 83) * cell + 10, 11);
    int dy1 = board_y + tnt_floor_div_s64((s64)(src_y + height - 36) * cell + 10, 11);
    int y;

    if (dx0 < 0) dx0 = 0;
    if (dy0 < 0) dy0 = 0;
    if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
    if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;

    for (y = dy0; y < dy1; y++) {
        int orig_y = 36 + tnt_floor_div_s64((s64)(y - board_y) * 11, cell);
        int sy = orig_y - src_y;
        int x;

        if (sy < 0 || sy >= height)
            continue;

        for (x = dx0; x < dx1; x++) {
            int orig_x = 83 + tnt_floor_div_s64((s64)(x - board_x) * 11, cell);
            int sx = orig_x - src_x;
            u8 ia;
            u8 intensity;
            u8 alpha;

            if (sx < 0 || sx >= width)
                continue;

            ia = pixels[sy * width + sx];
            intensity = (u8)((ia >> 4) * 17U);
            alpha = (u8)((ia & 0x0FU) * 17U);
            if (global_alpha != 0xFF)
                alpha = (u8)(((u32)alpha * global_alpha + 127U) / 255U);

            if (alpha != 0) {
                uint32_t *pixel = &dst[y * DISPLAY_STRIDE + x];
                *pixel = tnt_alpha_blend_rgb(
                    *pixel, intensity, intensity, intensity, alpha);
            }
        }
    }
}

static void tnt_vita_draw_original_ia8_sprite_tinted(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    int src_x,
    int src_y,
    int width,
    int height,
    const u8 *pixels,
    u8 tint_r,
    u8 tint_g,
    u8 tint_b,
    u8 global_alpha
) {
    uint32_t *dst = (uint32_t *)buffer;
    int dx0 = board_x + tnt_floor_div_s64((s64)(src_x - 83) * cell, 11);
    int dy0 = board_y + tnt_floor_div_s64((s64)(src_y - 36) * cell, 11);
    int dx1 = board_x + tnt_floor_div_s64((s64)(src_x + width - 83) * cell + 10, 11);
    int dy1 = board_y + tnt_floor_div_s64((s64)(src_y + height - 36) * cell + 10, 11);
    int y;
    int sx_map[DISPLAY_WIDTH];
    int sy_map[DISPLAY_HEIGHT];
    u8 r_lut[16];
    u8 g_lut[16];
    u8 b_lut[16];
    u8 a_lut[16];
    int x;
    int level;

    if (dx0 < 0) dx0 = 0;
    if (dy0 < 0) dy0 = 0;
    if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
    if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;

    /* IA8 exposes only sixteen intensity and alpha levels.  Build the exact
     * tinted/alpha results once per sprite draw instead of repeating the same
     * multiply-and-divide work for every destination pixel.  In particular,
     * the ONE PLAYER selection-box mask is ~73% transparent; a_lut[0] lets
     * those pixels exit before doing any color work or destination blend. */
    for (level = 0; level < 16; level++) {
        u8 v = (u8)(level * 17U);
        r_lut[level] = (u8)(((u32)v * tint_r + 127U) / 255U);
        g_lut[level] = (u8)(((u32)v * tint_g + 127U) / 255U);
        b_lut[level] = (u8)(((u32)v * tint_b + 127U) / 255U);
        a_lut[level] = global_alpha == 0xFF
            ? v
            : (u8)(((u32)v * global_alpha + 127U) / 255U);
    }

    /* Source mappings depend only on one destination axis. Cache the exact
     * floor-divided values once per draw. */
    for (x = dx0; x < dx1; x++) {
        int orig_x = 83 +
            tnt_floor_div_s64((s64)(x - board_x) * 11, cell);
        sx_map[x] = orig_x - src_x;
    }
    for (y = dy0; y < dy1; y++) {
        int orig_y = 36 +
            tnt_floor_div_s64((s64)(y - board_y) * 11, cell);
        sy_map[y] = orig_y - src_y;
    }

    for (y = dy0; y < dy1; y++) {
        int sy = sy_map[y];

        if (sy < 0 || sy >= height)
            continue;

        for (x = dx0; x < dx1; x++) {
            int sx = sx_map[x];
            u8 ia;
            u8 alpha;

            if (sx < 0 || sx >= width)
                continue;

            ia = pixels[sy * width + sx];
            alpha = a_lut[ia & 0x0FU];
            if (alpha != 0) {
                uint32_t *pixel = &dst[y * DISPLAY_STRIDE + x];
                uint32_t d = *pixel;
                u8 level_i = ia >> 4;
                u32 inv = 255U - alpha;
                u32 rn = (u32)r_lut[level_i] * alpha + (d & 0xFFU) * inv;
                u32 gn = (u32)g_lut[level_i] * alpha + ((d >> 8) & 0xFFU) * inv;
                u32 bn = (u32)b_lut[level_i] * alpha + ((d >> 16) & 0xFFU) * inv;
                /* Exact round((src*a + dst*(255-a))/255) without integer
                 * division.  For this 0..65025 domain, floor(m/255) with
                 * m=num+127 is exactly (m + 1 + (m>>8)) >> 8. */
                u32 rm = rn + 127U, gm = gn + 127U, bm = bn + 127U;
                u32 rr = (rm + 1U + (rm >> 8)) >> 8;
                u32 gg = (gm + 1U + (gm >> 8)) >> 8;
                u32 bb = (bm + 1U + (bm >> 8)) >> 8;
                *pixel = COLOR(rr, gg, bb);
            }
        }
    }
}

/* Test86: exact original USA 1.0 IMG_INDICATOR #80, 5x11 IA8.
 * Retail aimedat.c uses four 7px-spaced markers per source location at
 * x61/145/230/314, y267, with 0x80-gray disabled targets and
 * R/G/B/Y highlighting the selected receiver. */
static const u8 g_tnt_directed_indicator_ia8[5 * 11] = {
    0x6F,0x9F,0x9F,0x9F,0x6F,0x9F,0xCF,0xCF,0xCF,0x9F,0x9F,0xCF,0xFF,0xCF,0x9F,0x9F,0xCF,0xFF,0xCF,0x9F,
    0x9F,0xCF,0xFF,0xCF,0x9F,0x9F,0xCF,0xFF,0xCF,0x9F,0x9F,0xCF,0xFF,0xCF,0x9F,0x9F,0xCF,0xFF,0xCF,0x9F,
    0x9F,0xCF,0xFF,0xCF,0x9F,0x9F,0xCF,0xCF,0xCF,0x9F,0x6F,0x9F,0x9F,0x9F,0x6F,
};

typedef struct {
    u32 *tile;
    int x, y, w, h;
    u8 enabled, disabled_mask, target, valid;
} TNT3PDirectedTargetTile;
static TNT3PDirectedTargetTile g_tnt_3p_directed_tiles[4];
static u8 g_tnt_3p_directed_scratch_ready;

static void __attribute__((optimize("Os")))
tnt_vita_3p_directed_free(void) {
    int i;
    for (i = 0; i < 4; i++) {
        free(g_tnt_3p_directed_tiles[i].tile);
        memset(&g_tnt_3p_directed_tiles[i], 0,
            sizeof(g_tnt_3p_directed_tiles[i]));
    }
    g_tnt_3p_directed_scratch_ready = FALSE;
}

static void __attribute__((optimize("Os")))
tnt_vita_3p_directed_marker(
    void *buffer, int board_x, int board_y, int cell,
    int player, int target, u8 grey
) {
    static const s16 marker_x[4] = {61,145,230,314};
    static const u8 rgb[4][3] = {
        {255,0,0}, {0,255,0}, {0,0,255}, {255,255,0}
    };
    if (player < 0 || player >= 4 || target < 0 || target >= 4) return;
    tnt_vita_draw_original_ia8_sprite_tinted(
        buffer, board_x, board_y, cell,
        marker_x[player] + 7 * target, 267, 5, 11,
        g_tnt_directed_indicator_ia8,
        grey ? 128 : rgb[target][0],
        grey ? 128 : rgb[target][1],
        grey ? 128 : rgb[target][2], 255);
}

/* Background bars are constant in retail AimedAt_Render, not animated.
 * Test95 expands the exact hardware-validated 3P original N64 layout
 * to four participants (source 4th x286, target indicators source x314).
 * Bake N64 alpha0x80 80x2 source y228 colored strips once per player.
 * In 3P, bake dormant P4's disabled indicators as before.
 * g_tnt_gameplay_base_scene is the dormant gameplay-scene scratch while
 * the three-player match is active, never the TALLY static image. */
static void __attribute__((optimize("Os")))
tnt_vita_3p_directed_prepare(
    Landfill *landfill, int board_x, int board_y, int cell
) {
    static const s16 source_x[4] = {34,118,202,286};
    static const s16 marker_x[4] = {61,145,230,314};
    static const u8 rgb[4][3] = {
        {255,0,0},{0,255,0},{0,0,255},{255,255,0}
    };
    const int count = g_playercount == 4 ? 4 : 3;
    int p, j;
    tnt_vita_3p_directed_free();
    if (landfill == NULL || landfill->type != LANDFILLTYPE_DIRECTED ||
        landfill->unk8.list == NULL || landfill->unk8.numIndicators < 4)
        return;
    for (p = 0; p < count; p++) {
        int x0 = tnt_vita_source_x(source_x[p], board_x, cell);
        int x1 = tnt_vita_source_x(source_x[p] + 80, board_x, cell);
        int y0 = tnt_vita_source_y(228, board_y, cell);
        int y1 = tnt_vita_source_y(230, board_y, cell);
        tnt_vita_draw_alpha_rect(g_tnt_static_scene,
            x0, y0, x1-x0, y1-y0,
            rgb[p][0], rgb[p][1], rgb[p][2], 128);
    }
    if (count == 3) {
        for (j = 0; j < 4; j++) {
            /* 3-player N64 mode disables every target on dormant P4. */
            if (!landfill->unk8.list[3].unk1[j])
                tnt_vita_3p_directed_marker(g_tnt_static_scene,
                    board_x, board_y, cell, 3, j, TRUE);
        }
    }
    memcpy(g_tnt_gameplay_base_scene, g_tnt_static_scene,
        sizeof(g_tnt_gameplay_base_scene));
    g_tnt_3p_directed_scratch_ready = TRUE;
    for (p = 0; p < count; p++) {
        TNT3PDirectedTargetTile *entry = &g_tnt_3p_directed_tiles[p];
        int x0 = tnt_vita_source_x(marker_x[p], board_x, cell) - 1;
        int x1 = tnt_vita_source_x(marker_x[p] + 26, board_x, cell) + 2;
        int y0 = tnt_vita_source_y(267, board_y, cell) - 1;
        int y1 = tnt_vita_source_y(278, board_y, cell) + 2;
        if (x0 < 0 || y0 < 0 || x1 > DISPLAY_WIDTH ||
            y1 > DISPLAY_HEIGHT || x0 >= x1 || y0 >= y1)
            continue;
        entry->x=x0; entry->y=y0;
        entry->w=x1-x0; entry->h=y1-y0;
        entry->tile = malloc(
            (size_t)entry->w * (size_t)entry->h * sizeof(u32));
        /* Null tile -> correct but slow direct-draw fallback. */
    }
}

/* Rebuild tiny cached indicator patches only when a player changes
 * targets or a top-out changes the enabled/disabled target mask.
 * Test95 uses four tiny patches in 4P, three unchanged in 3P. */
static void __attribute__((optimize("Os")))
tnt_vita_3p_directed_render(
    void *buffer, Landfill *landfill,
    int board_x, int board_y, int cell
) {
    int p, j;
    UnkStruct_112 *aimed;
    if (landfill == NULL || landfill->type != LANDFILLTYPE_DIRECTED ||
        (g_playercount != 3 && g_playercount != 4)) return;
    aimed = &landfill->unk8;
    if (aimed->list == NULL || aimed->numIndicators < 4) return;
    for (p = 0; p < (int)g_playercount; ++p) {
        const UnkStruct_111 *ind = &aimed->list[p];
        TNT3PDirectedTargetTile *entry = &g_tnt_3p_directed_tiles[p];
        u8 mask = 0;
        for (j = 0; j < 4; ++j)
            if (!ind->unk1[j]) mask |= (u8)(1U << j);
        if (g_tnt_3p_directed_scratch_ready && entry->tile != NULL) {
            int row;
            if (!entry->valid || entry->disabled_mask != mask ||
                entry->enabled != ind->unk0 ||
                entry->target != ind->unkA) {
                for (row = 0; row < entry->h; ++row) {
                    size_t offset = (size_t)(entry->y + row) *
                        DISPLAY_STRIDE + entry->x;
                    memcpy(g_tnt_gameplay_base_scene + offset,
                        g_tnt_static_scene + offset,
                        (size_t)entry->w * sizeof(u32));
                }
                for (j = 0; j < 4; ++j)
                    if (!ind->unk1[j])
                        tnt_vita_3p_directed_marker(
                            g_tnt_gameplay_base_scene,
                            board_x, board_y, cell, p, j, TRUE);
                if (ind->unk0 && ind->unkA < 4)
                    tnt_vita_3p_directed_marker(
                        g_tnt_gameplay_base_scene,
                        board_x, board_y, cell, p, ind->unkA, FALSE);
                for (row = 0; row < entry->h; ++row)
                    memcpy(entry->tile + (size_t)row * entry->w,
                        g_tnt_gameplay_base_scene +
                        (size_t)(entry->y + row) * DISPLAY_STRIDE + entry->x,
                        (size_t)entry->w * sizeof(u32));
                entry->disabled_mask=mask;
                entry->target=ind->unkA;
                entry->enabled=ind->unk0;
                entry->valid=TRUE;
            }
            for (row = 0; row < entry->h; ++row)
                memcpy((u32 *)buffer +
                    (size_t)(entry->y + row) * DISPLAY_STRIDE + entry->x,
                    entry->tile + (size_t)row * entry->w,
                    (size_t)entry->w * sizeof(u32));
        } else {
            for (j = 0; j < 4; ++j)
                if (!ind->unk1[j])
                    tnt_vita_3p_directed_marker(
                        buffer, board_x, board_y, cell, p, j, TRUE);
            if (ind->unk0 && ind->unkA < 4)
                tnt_vita_3p_directed_marker(
                    buffer, board_x, board_y, cell, p, ind->unkA, FALSE);
        }
    }
}

static void tnt_vita_draw_original_ci8_sprite(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    int src_x,
    int src_y,
    int width,
    int height,
    const u8 *pixels,
    const u16 *palette
) {
    uint32_t *dst = (uint32_t *)buffer;
    int dx0 = board_x + tnt_floor_div_s64((s64)(src_x - 83) * cell, 11);
    int dy0 = board_y + tnt_floor_div_s64((s64)(src_y - 36) * cell, 11);
    int dx1 = board_x + tnt_floor_div_s64((s64)(src_x + width - 83) * cell + 10, 11);
    int dy1 = board_y + tnt_floor_div_s64((s64)(src_y + height - 36) * cell + 10, 11);
    int y;

    if (dx0 < 0) dx0 = 0;
    if (dy0 < 0) dy0 = 0;
    if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
    if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;

    for (y = dy0; y < dy1; y++) {
        int orig_y = 36 + tnt_floor_div_s64((s64)(y - board_y) * 11, cell);
        int sy = orig_y - src_y;
        int x;

        if (sy < 0 || sy >= height)
            continue;

        for (x = dx0; x < dx1; x++) {
            int orig_x = 83 + tnt_floor_div_s64((s64)(x - board_x) * 11, cell);
            int sx = orig_x - src_x;
            u16 rgba;
            u8 r5;
            u8 g5;
            u8 b5;

            if (sx < 0 || sx >= width)
                continue;

            rgba = palette[pixels[sy * width + sx]];
            if ((rgba & 1U) == 0)
                continue;

            r5 = (u8)((rgba >> 11) & 0x1F);
            g5 = (u8)((rgba >> 6) & 0x1F);
            b5 = (u8)((rgba >> 1) & 0x1F);
            dst[y * DISPLAY_STRIDE + x] = COLOR(
                (r5 << 3) | (r5 >> 2),
                (g5 << 3) | (g5 >> 2),
                (b5 << 3) | (b5 >> 2));
        }
    }
}

static void tnt_vita_draw_original_ci8_sprite_alpha(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    int src_x,
    int src_y,
    int width,
    int height,
    const u8 *pixels,
    const u16 *palette,
    u8 global_alpha
) {
    uint32_t *dst = (uint32_t *)buffer;
    int dx0 = board_x + tnt_floor_div_s64((s64)(src_x - 83) * cell, 11);
    int dy0 = board_y + tnt_floor_div_s64((s64)(src_y - 36) * cell, 11);
    int dx1 = board_x + tnt_floor_div_s64((s64)(src_x + width - 83) * cell + 10, 11);
    int dy1 = board_y + tnt_floor_div_s64((s64)(src_y + height - 36) * cell + 10, 11);
    int y;

    if (global_alpha == 0)
        return;
    if (dx0 < 0) dx0 = 0;
    if (dy0 < 0) dy0 = 0;
    if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
    if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;

    for (y = dy0; y < dy1; y++) {
        int orig_y = 36 + tnt_floor_div_s64((s64)(y - board_y) * 11, cell);
        int sy = orig_y - src_y;
        int x;

        if (sy < 0 || sy >= height)
            continue;

        for (x = dx0; x < dx1; x++) {
            int orig_x = 83 + tnt_floor_div_s64((s64)(x - board_x) * 11, cell);
            int sx = orig_x - src_x;
            u16 rgba;
            u8 r5;
            u8 g5;
            u8 b5;

            if (sx < 0 || sx >= width)
                continue;

            rgba = palette[pixels[sy * width + sx]];
            if ((rgba & 1U) == 0)
                continue;

            r5 = (u8)((rgba >> 11) & 0x1F);
            g5 = (u8)((rgba >> 6) & 0x1F);
            b5 = (u8)((rgba >> 1) & 0x1F);
            if (global_alpha == 0xFF) {
                dst[y * DISPLAY_STRIDE + x] = COLOR(
                    (r5 << 3) | (r5 >> 2),
                    (g5 << 3) | (g5 >> 2),
                    (b5 << 3) | (b5 >> 2));
            } else {
                uint32_t *pixel = &dst[y * DISPLAY_STRIDE + x];
                *pixel = tnt_alpha_blend_rgb(
                    *pixel,
                    (u8)((r5 << 3) | (r5 >> 2)),
                    (u8)((g5 << 3) | (g5 >> 2)),
                    (u8)((b5 << 3) | (b5 >> 2)),
                    global_alpha);
            }
        }
    }
}

static void tnt_vita_draw_original_rgba16_sprite_alpha(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    int src_x,
    int src_y,
    int width,
    int height,
    const u16 *pixels,
    u8 global_alpha
) {
    uint32_t *dst = (uint32_t *)buffer;
    int dx0 = board_x + tnt_floor_div_s64((s64)(src_x - 83) * cell, 11);
    int dy0 = board_y + tnt_floor_div_s64((s64)(src_y - 36) * cell, 11);
    int dx1 = board_x + tnt_floor_div_s64((s64)(src_x + width - 83) * cell + 10, 11);
    int dy1 = board_y + tnt_floor_div_s64((s64)(src_y + height - 36) * cell + 10, 11);
    int y;

    if (global_alpha == 0)
        return;
    if (dx0 < 0) dx0 = 0;
    if (dy0 < 0) dy0 = 0;
    if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
    if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;

    for (y = dy0; y < dy1; y++) {
        int orig_y = 36 + tnt_floor_div_s64((s64)(y - board_y) * 11, cell);
        int sy = orig_y - src_y;
        int x;
        if (sy < 0 || sy >= height)
            continue;

        for (x = dx0; x < dx1; x++) {
            int orig_x = 83 + tnt_floor_div_s64((s64)(x - board_x) * 11, cell);
            int sx = orig_x - src_x;
            u16 rgba;
            u8 r5, g5, b5;
            if (sx < 0 || sx >= width)
                continue;
            rgba = pixels[sy * width + sx];
            if ((rgba & 1U) == 0)
                continue;
            r5 = (u8)((rgba >> 11) & 0x1F);
            g5 = (u8)((rgba >> 6) & 0x1F);
            b5 = (u8)((rgba >> 1) & 0x1F);
            if (global_alpha == 0xFF) {
                dst[y * DISPLAY_STRIDE + x] = COLOR(
                    (r5 << 3) | (r5 >> 2),
                    (g5 << 3) | (g5 >> 2),
                    (b5 << 3) | (b5 >> 2));
            } else {
                uint32_t *pixel = &dst[y * DISPLAY_STRIDE + x];
                *pixel = tnt_alpha_blend_rgb(
                    *pixel,
                    (u8)((r5 << 3) | (r5 >> 2)),
                    (u8)((g5 << 3) | (g5 >> 2)),
                    (u8)((b5 << 3) | (b5 >> 2)),
                    global_alpha);
            }
        }
    }
}

typedef struct {
    s16 x;
    s16 y;
    u8 image_cnt;
    u8 intra_frame_cnt;
} VitaMayanFire;

/* Exact initial image phases/locations from D_800D1030..D_800D1474. */
static VitaMayanFire g_tnt_mayan_fires[8] = {
    { 297,  89, 0, 0 },
    { 301, 115, 2, 0 },
    { 306, 141, 1, 0 },
    { 328,  89, 4, 0 },
    { 333, 115, 3, 0 },
    { 338, 141, 0, 0 },
    { 278,  74, 0, 0 },
    { 331,  74, 2, 0 },
};

static void tnt_vita_reset_mayan_temple_fires(void) {
    static const VitaMayanFire initial[8] = {
        { 297,  89, 0, 0 }, { 301, 115, 2, 0 },
        { 306, 141, 1, 0 }, { 328,  89, 4, 0 },
        { 333, 115, 3, 0 }, { 338, 141, 0, 0 },
        { 278,  74, 0, 0 }, { 331,  74, 2, 0 },
    };
    memcpy(g_tnt_mayan_fires, initial, sizeof(initial));
}

/* TEST106A (isolated experiment): eight tiny Mayan flame sprites share
 * the SAME original 400x300 -> 960x544 sampling grid. Previously each
 * fire redivided the exact same screen x/y coordinates per destination
 * pixel and repeated RGBA16 palette lookups. Precompute the mapping only
 * when board origin or scale changes, and predecode each of five 5x6
 * RGBA16-indexed frames exactly once. This is a source-equivalent
 * specialized path; ALL other sprites still use the original renderer.
 * The N64 animation counter / draw order and absence of alpha blending
 * for translucent RGBA16 alpha=0 entries remain unchanged. */
static int s_tnt106a_orig_x[DISPLAY_WIDTH];
static int s_tnt106a_orig_y[DISPLAY_HEIGHT];
static u32 s_tnt106a_fire_rgb[5][30];
static u8 s_tnt106a_fire_opaque[5][30];
static int s_tnt106a_bx, s_tnt106a_by, s_tnt106a_cell;
static u8 s_tnt106a_map_ready, s_tnt106a_color_ready;

static void tnt106a_prepare_mayan_fire_cache(
    int board_x, int board_y, int cell
) {
    if (!s_tnt106a_color_ready) {
        for (int frame=0; frame<5; frame++) {
            for (int i=0; i<30; i++) {
                const u16 rgba = g_tnt_mayan_fire_rgba16[frame]
                                            [g_tnt_mayan_fire_ci8[frame][i]];
                const u32 r5=(rgba>>11)&31U;
                const u32 g5=(rgba>>6)&31U;
                const u32 b5=(rgba>>1)&31U;
                s_tnt106a_fire_opaque[frame][i] = (u8)(rgba&1U);
                s_tnt106a_fire_rgb[frame][i] = COLOR(
                    (r5<<3)|(r5>>2),(g5<<3)|(g5>>2),
                    (b5<<3)|(b5>>2));
            }
        }
        s_tnt106a_color_ready=TRUE;
    }
    if (s_tnt106a_map_ready && s_tnt106a_bx==board_x &&
        s_tnt106a_by==board_y && s_tnt106a_cell==cell) return;
    for (int x=0;x<DISPLAY_WIDTH;x++)
        s_tnt106a_orig_x[x]=83+tnt_floor_div_s64(
            (s64)(x-board_x)*11,cell);
    for (int y=0;y<DISPLAY_HEIGHT;y++)
        s_tnt106a_orig_y[y]=36+tnt_floor_div_s64(
            (s64)(y-board_y)*11,cell);
    s_tnt106a_bx=board_x;
    s_tnt106a_by=board_y;
    s_tnt106a_cell=cell;
    s_tnt106a_map_ready=TRUE;
}

static void tnt106a_draw_mayan_fire(
    void *buffer, int board_x,int board_y,int cell,
    const VitaMayanFire *fire
) {
    const int src_x=fire->x, src_y=fire->y;
    int dx0=board_x+tnt_floor_div_s64(
        (s64)(src_x-83)*cell,11);
    int dy0=board_y+tnt_floor_div_s64(
        (s64)(src_y-36)*cell,11);
    int dx1=board_x+tnt_floor_div_s64(
        (s64)(src_x+5-83)*cell+10,11);
    int dy1=board_y+tnt_floor_div_s64(
        (s64)(src_y+6-36)*cell+10,11);
    const int frame=fire->image_cnt;
    uint32_t *dst=(uint32_t *)buffer;
    if(dx0<0) dx0=0;
    if(dy0<0) dy0=0;
    if(dx1>DISPLAY_WIDTH) dx1=DISPLAY_WIDTH;
    if(dy1>DISPLAY_HEIGHT) dy1=DISPLAY_HEIGHT;
    for (int y=dy0;y<dy1;y++) {
        const int sy=s_tnt106a_orig_y[y]-src_y;
        if(sy<0 || sy>=6) continue;
        uint32_t *row=dst+y*DISPLAY_STRIDE;
        const int off=sy*5;
        for (int x=dx0;x<dx1;x++) {
            const int sx=s_tnt106a_orig_x[x]-src_x;
            if(sx<0 || sx>=5) continue;
            const int pixel=off+sx;
            if(s_tnt106a_fire_opaque[frame][pixel])
                row[x]=s_tnt106a_fire_rgb[frame][pixel];
        }
    }
}

static void tnt_vita_draw_mayan_temple_fires(
    void *buffer,
    int board_x,
    int board_y,
    int cell
) {
    int i;

    tnt106a_prepare_mayan_fire_cache(board_x,board_y,cell);
    for (i = 0; i < 8; i++) {
        VitaMayanFire *fire = &g_tnt_mayan_fires[i];

        /* Original func_800767C0 with num_intra_frames == 2: a new
         * frame is selected when the counter is zero, then held for the
         * next two render calls. Animation is render-driven on N64.
         */
        if (fire->intra_frame_cnt == 0) {
            fire->intra_frame_cnt = 2;
            fire->image_cnt = (u8)((fire->image_cnt + 1) % 5);
        } else {
            fire->intra_frame_cnt--;
        }

        /* TEST106A: source-matched pixels, accelerated sampling only. */
        tnt106a_draw_mayan_fire(buffer,board_x,board_y,cell,fire);
    }
}

static void tnt_vita_build_ci8_index_scene(
    uint32_t *index_scene,
    int board_x,
    int board_y,
    int cell,
    const u8 *pixels
) {
    int dx0 = board_x + tnt_floor_div_s64((s64)(0 - 83) * cell, 11);
    int dy0 = board_y + tnt_floor_div_s64((s64)(0 - 36) * cell, 11);
    int dx1 = board_x + tnt_floor_div_s64((s64)(400 - 83) * cell + 10, 11);
    int dy1 = board_y + tnt_floor_div_s64((s64)(300 - 36) * cell + 10, 11);
    int y;

    /* The title asset is a fully opaque CI8 image. Cache only the scaled
     * palette index for each Vita pixel. This cache occupies the same buffer
     * later rebuilt as the Mayan static scene, so it adds no persistent RAM. */
    memset(index_scene, 0, sizeof(g_tnt_static_scene));

    if (dx0 < 0) dx0 = 0;
    if (dy0 < 0) dy0 = 0;
    if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
    if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;

    for (y = dy0; y < dy1; y++) {
        int orig_y = 36 + tnt_floor_div_s64((s64)(y - board_y) * 11, cell);
        int x;

        for (x = dx0; x < dx1; x++) {
            int orig_x = 83 + tnt_floor_div_s64((s64)(x - board_x) * 11, cell);
            index_scene[y * DISPLAY_STRIDE + x] =
                pixels[orig_y * 400 + orig_x];
        }
    }
}

static void tnt_vita_render_ci8_scene(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    const u16 *palette,
    u8 overlay_r,
    u8 overlay_g,
    u8 overlay_b,
    u8 fade_alpha
) {
    uint32_t faded_palette[256];
    uint32_t *dst = (uint32_t *)buffer;
    int dx0 = board_x + tnt_floor_div_s64((s64)(0 - 83) * cell, 11);
    int dy0 = board_y + tnt_floor_div_s64((s64)(0 - 36) * cell, 11);
    int dx1 = board_x + tnt_floor_div_s64((s64)(400 - 83) * cell + 10, 11);
    int dy1 = board_y + tnt_floor_div_s64((s64)(300 - 36) * cell + 10, 11);
    int i;
    int y;

    if (dx0 < 0) dx0 = 0;
    if (dy0 < 0) dy0 = 0;
    if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
    if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;

    /* Original fade-from-white is a white translucent full-screen primitive
     * over the CI8 title. Folding that constant alpha into the 256 palette
     * entries produces the same result without a second full-screen CPU blend. */
    for (i = 0; i < 256; i++) {
        u16 rgba = palette[i];
        u32 r5 = (rgba >> 11) & 0x1FU;
        u32 g5 = (rgba >> 6) & 0x1FU;
        u32 b5 = (rgba >> 1) & 0x1FU;
        u32 r = (r5 << 3) | (r5 >> 2);
        u32 g = (g5 << 3) | (g5 >> 2);
        u32 b = (b5 << 3) | (b5 >> 2);

        r = (r * (255U - fade_alpha) + (u32)overlay_r * fade_alpha + 127U) / 255U;
        g = (g * (255U - fade_alpha) + (u32)overlay_g * fade_alpha + 127U) / 255U;
        b = (b * (255U - fade_alpha) + (u32)overlay_b * fade_alpha + 127U) / 255U;
        faded_palette[i] = COLOR(r, g, b);
    }

    /* The display buffers are black before the title begins and the mapped
     * 400x300 viewport is rectangular, so only overwrite that viewport. */
    for (y = dy0; y < dy1; y++) {
        uint32_t *dst_row = dst + y * DISPLAY_STRIDE;
        const uint32_t *idx_row = g_tnt_static_scene + y * DISPLAY_STRIDE;
        int x;

        for (x = dx0; x < dx1; x++) {
            dst_row[x] = faded_palette[idx_row[x] & 0xFFU];
        }
    }
}

/* Original 063BC0.c frontend entrance.  The 400x300 menu background is
 * divided into an 8x8 grid (50x37 source tiles) and those 64 cells are
 * grouped into 16 four-cell tetrominoes by D_800D3ED0.  func_8009FA2C
 * starts every group at Y=-900, staggers the authored delays below, and
 * advances the Y translation by +110 world units per 60-Hz update.  With
 * the source 59-degree projection/camera, one world Y unit is 0.373414...
 * logical 400x300 pixels.  The groups have no entrance rotation, so moving
 * cached background rectangles reproduces the source transform without a
 * separate 3D renderer. */
static const u8 g_tnt_menu_assembly_group[64] = {
    0x0,0x0,0x0,0x0, 0x1,0x1,0x1,0x1,
    0x2,0x2,0x2,0x2, 0x3,0x3,0x3,0x3,
    0x4,0x4,0x4,0x4, 0x5,0x5,0x5,0x5,
    0x6,0x6,0x6,0x6, 0x7,0x7,0x7,0x7,
    0x8,0x9,0x9,0xB, 0xC,0xF,0xE,0xE,
    0x8,0x9,0x9,0xB, 0xC,0xF,0xF,0xE,
    0x8,0xA,0xB,0xB, 0xC,0xD,0xF,0xE,
    0x8,0xA,0xA,0xA, 0xC,0xD,0xD,0xD
};

static const u8 g_tnt_menu_assembly_delay[16] = {
    17,20,13,16,21,19,17,14,3,11,1,8,5,2,11,10
};

/* Canonical func_8009FF08 plays SFX_MENU_0 when each falling background
 * tetromino reaches Y=0. Starting at -900 with +110/update takes nine
 * movement updates plus the landing update, so each authored delay lands
 * exactly 10 updates later. Advance the shared entrance counter and fire the
 * same impact sound on the same visual frame for every GUI screen that uses
 * the common falling-block assembly. */
static u8 tnt_vita_menu_assembly_advance(u8 *entrance_ticks) {
    /* The N64 frontend advances this 31-step animation at its effective
     * ~30-Hz presentation cadence.  TNTV originally advanced one step per
     * *rendered* Vita frame; once menu rendering was optimized toward 60 FPS,
     * the same authored fall finished nearly twice as fast in wall-clock time.
     *
     * Keep the menu renderer uncapped/smooth, but clock only this shared
     * falling-block state machine at 30 Hz.  The source delay table, +110 Y
     * velocity, landing frames and SFX are otherwise unchanged. */
    static u8 *owner;
    static u64 last_us;
    static u64 accum_us;
    const u64 step_us = 33333ULL;
    u64 now;

    if (entrance_ticks == NULL || *entrance_ticks == 0) {
        if (owner == entrance_ticks) {
            owner = NULL;
            last_us = 0;
            accum_us = 0;
        }
        return FALSE;
    }

    now = sceKernelGetProcessTimeWide();
    if (owner != entrance_ticks || *entrance_ticks == 31) {
        owner = entrance_ticks;
        last_us = now;
        /* Source performs the first update immediately on the first presented
         * assembly frame, then subsequent updates are one 30-Hz tick apart. */
        accum_us = step_us;
    } else {
        u64 elapsed = now - last_us;
        last_us = now;
        /* A debugger/OS interruption should not fast-forward the whole menu
         * entrance and collapse all landing SFX into one frame. */
        if (elapsed > 200000ULL)
            elapsed = 200000ULL;
        accum_us += elapsed;
    }

    while (accum_us >= step_us && *entrance_ticks != 0) {
        int frame;
        int group;
        accum_us -= step_us;
        (*entrance_ticks)--;
        frame = 31 - (int)(*entrance_ticks);

        for (group = 0; group < 16; group++) {
            if (frame == (int)g_tnt_menu_assembly_delay[group] + 10)
                Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_0);
        }
    }

    return TRUE;
}

static void tnt_vita_menu_render_assembly(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    u8 entrance_ticks
) {
    uint32_t *dst = (uint32_t *)buffer;
    int frame = 31 - (int)entrance_ticks; /* source-visible frames 1..30 */
    int row;

    if (frame < 1) frame = 1;
    if (frame > 30) frame = 30;
    clear_screen(buffer, COLOR(0, 0, 0));

    for (row = 0; row < 8; row++) {
        int col;
        for (col = 0; col < 8; col++) {
            int group = g_tnt_menu_assembly_group[row * 8 + col];
            int moves = frame - (int)g_tnt_menu_assembly_delay[group] - 1;
            int world_y;
            int native_dy;
            int native_x0 = col * 50;
            int native_x1 = native_x0 + 50;
            int native_y0 = row * 37;
            int native_y1 = native_y0 + 37;
            int sx0, sx1, sy0, sy1, dy0;
            int y;

            if (moves < 0) moves = 0;
            if (moves > 9) moves = 9;
            world_y = -900 + moves * 110;
            if (world_y > 0) world_y = 0;
            native_dy = (int)((f32)world_y * 0.373414229f - 0.5f);

            sx0 = board_x + tnt_floor_div_s64((s64)(native_x0 - 83) * cell, 11);
            sx1 = board_x + tnt_floor_div_s64((s64)(native_x1 - 83) * cell + 10, 11);
            sy0 = board_y + tnt_floor_div_s64((s64)(native_y0 - 36) * cell, 11);
            sy1 = board_y + tnt_floor_div_s64((s64)(native_y1 - 36) * cell + 10, 11);
            dy0 = board_y + tnt_floor_div_s64((s64)(native_y0 + native_dy - 36) * cell, 11);

            for (y = sy0; y < sy1; y++) {
                int out_y = dy0 + (y - sy0);
                int x0 = sx0;
                int x1 = sx1;
                if (y < 0 || y >= DISPLAY_HEIGHT || out_y < 0 || out_y >= DISPLAY_HEIGHT)
                    continue;
                if (x0 < 0) x0 = 0;
                if (x1 > DISPLAY_WIDTH) x1 = DISPLAY_WIDTH;
                if (x0 < x1) {
                    memcpy(dst + out_y * DISPLAY_STRIDE + x0,
                           g_tnt_menu_background_scene + y * DISPLAY_STRIDE + x0,
                           (size_t)(x1 - x0) * sizeof(uint32_t));
                }
            }
        }
    }
}

static void tnt_vita_main_menu_build_caches(
    int board_x,
    int board_y,
    int cell
) {
    clear_screen(g_tnt_menu_background_scene, COLOR(0, 0, 0));
    tnt_vita_draw_original_ci8_sprite(
        g_tnt_menu_background_scene, board_x, board_y, cell,
        0, 0, 400, 300,
        g_tnt_main_menu_background_ci8,
        g_tnt_main_menu_background_rgba16);

    memcpy(
        g_tnt_menu_static_scene,
        g_tnt_menu_background_scene,
        sizeof(g_tnt_menu_static_scene));

    /* Final root-menu static composition at D_800D3E2C == 1.0. The selected
     * pulse/text remain dynamic and are intentionally not baked here. */
    tnt_vita_draw_original_ia8_sprite(
        g_tnt_menu_static_scene, board_x, board_y, cell,
        0x1E, 0x31, 316, 213,
        g_tnt_main_menu_shadow_ia8, 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_menu_static_scene, board_x, board_y, cell,
        (400 - 312) / 2, 0x2A, 312, 210,
        g_tnt_main_menu_panel_ci8,
        g_tnt_main_menu_panel_rgba16,
        0xFF);
    tnt_vita_draw_original_rgba16_sprite_alpha(
        g_tnt_menu_static_scene, board_x, board_y, cell,
        (400 - 71) / 2, 0x100, 71, 15,
        g_tnt_main_menu_a_accept_rgba16,
        0xFF);
}

static void tnt_vita_main_menu_copy_cached_frame(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    const TNTVitaMainMenu *menu
) {
    uint32_t *dst = (uint32_t *)buffer;
    int step;

    if (menu->entrance_ticks != 0) {
        tnt_vita_menu_render_assembly(
            buffer, board_x, board_y, cell, menu->entrance_ticks);
        return;
    }

    if (menu->fade <= 0.0f) {
        memcpy(
            dst, g_tnt_menu_background_scene,
            sizeof(g_tnt_menu_background_scene));
        return;
    }

    if (menu->fade >= 1.0f) {
        memcpy(dst, g_tnt_menu_static_scene, sizeof(g_tnt_menu_static_scene));
        return;
    }

    memcpy(
        dst, g_tnt_menu_background_scene,
        sizeof(g_tnt_menu_background_scene));

    /* D_800D3E2C advances in exact ninths. Crossfade only the union of the
     * shadow/panel/A-accept bounds. This replaces hundreds of thousands of
     * per-frame coordinate divisions and palette conversions with a compact
     * cached RGB interpolation during the nine entrance frames. */
    step = (int)(menu->fade * 9.0f + 0.5f);
    if (step < 1) step = 1;
    if (step > 8) step = 8;

    {
        int dx0 = board_x + tnt_floor_div_s64((s64)(30 - 83) * cell, 11);
        int dy0 = board_y + tnt_floor_div_s64((s64)(42 - 36) * cell, 11);
        int dx1 = board_x + tnt_floor_div_s64((s64)(356 - 83) * cell + 10, 11);
        int dy1 = board_y + tnt_floor_div_s64((s64)(271 - 36) * cell + 10, 11);
        int y;

        if (dx0 < 0) dx0 = 0;
        if (dy0 < 0) dy0 = 0;
        if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
        if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;

        for (y = dy0; y < dy1; y++) {
            int x;
            uint32_t *dst_row = dst + y * DISPLAY_STRIDE;
            const uint32_t *bg_row =
                g_tnt_menu_background_scene + y * DISPLAY_STRIDE;
            const uint32_t *fg_row =
                g_tnt_menu_static_scene + y * DISPLAY_STRIDE;

            for (x = dx0; x < dx1; x++) {
                uint32_t bg = bg_row[x];
                uint32_t fg = fg_row[x];
                int br, bgc, bb, fr, fgc, fb;
                int r, g, b;

                if (bg == fg)
                    continue;

                br = (int)(bg & 0xFFU);
                bgc = (int)((bg >> 8) & 0xFFU);
                bb = (int)((bg >> 16) & 0xFFU);
                fr = (int)(fg & 0xFFU);
                fgc = (int)((fg >> 8) & 0xFFU);
                fb = (int)((fg >> 16) & 0xFFU);

                r = br + ((fr - br) * step) / 9;
                g = bgc + ((fgc - bgc) * step) / 9;
                b = bb + ((fb - bb) * step) / 9;
                dst_row[x] = COLOR((u8)r, (u8)g, (u8)b);
            }
        }
    }
}

static void tnt_vita_main_menu_enter(
    TNTVitaMainMenu *menu,
    PlayerVars *pv_ptr
) {
    tnt_vita_frontend_rng_init_once();
    menu->active = TRUE;
    menu->selected = 0;
    menu->leaving = FALSE;
    menu->idle_ticks = 0;
    /* Original func_8009FA2C entrance completes after the latest staggered
     * background-tetromino group reaches the menu plane: 31 60-Hz updates. */
    menu->entrance_ticks = 31;
    menu->pulse = 0.0f;
    menu->pulse_rate = 0.05f;
    menu->fade = 0.0f;
    g_tnt_60hz_last_us = 0;
    g_tnt_60hz_fraction = 0;

    /* Original root menu repeat timings: D-pad 32/10, A/B 16/16. */
    FUN_026C80_80060ad4_oneliner_calls_fun(&pv_ptr->unk28);
    FUN_026C80_80060b04_twelveliner_loops_32t(
        &pv_ptr->unk28, 0x0F00, 32, 10);
    FUN_026C80_80060b04_twelveliner_loops_32t(
        &pv_ptr->unk28, 0xC000, 16, 16);
    FUN_026C80_nineliner_loops_32t(&pv_ptr->unk28);
}

static void tnt_vita_restore_gameplay_repeat(PlayerVars *pv_ptr) {
    FUN_026C80_80060ad4_oneliner_calls_fun(&pv_ptr->unk28);
    FUN_026C80_80060b04_twelveliner_loops_32t(
        &pv_ptr->unk28, 0x0F00, 8, 4);
    FUN_026C80_80060b04_twelveliner_loops_32t(
        &pv_ptr->unk28, 0xC000, 16, 16);
    FUN_026C80_nineliner_loops_32t(&pv_ptr->unk28);
    pv_ptr->unk20 = pv_ptr->unk1C != NULL ? pv_ptr->unk1C->unk0 : 0;
    pv_ptr->unk24 = 0;
}

static void tnt_vita_refresh_multiplayer_ports(void) {
    int ports[4] = {-1, -1, -1, -1};
    int types[4] = {-1, -1, -1, -1};
    int count = vita_get_external_controller_info(ports, types, 4);
    u8 available = (u8)(vita_is_pstv() && count >= 2);
    u8 changed = (u8)(available != g_tnt_multi_available ||
        (u8)(count > 4 ? 4 : count) != g_tnt_multi_pad_count);
    int i;

    for (i = 0; i < 4; i++) {
        s8 value = i < count && i < 4 ? (s8)ports[i] : (s8)-1;
        s8 type = i < count && i < 4 ? (s8)types[i] : (s8)-1;
        if (g_tnt_multi_ports[i] != value || g_tnt_multi_types[i] != type)
            changed = TRUE;
        g_tnt_multi_ports[i] = value;
        g_tnt_multi_types[i] = type;
    }
    g_tnt_multi_pad_count = (u8)(count > 4 ? 4 : count);
    g_tnt_multi_available = available;

    /* Root-menu row alpha is cached. A hot-plug must rebuild it immediately
     * so MULTI PLAYER lights/dims without leaving the menu. */
    if (changed) {
        g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_NONE;
        g_tnt_multi_settled_valid = FALSE;
    }
}

static int tnt_vita_main_menu_update(
    TNTVitaMainMenu *menu,
    PlayerVars *pv_ptr,
    u8 multi_available
) {
    u32 retrace_delta = tnt_vita_60hz_delta(TRUE);
    if (retrace_delta == 0)
        retrace_delta = 1;

    menu->pulse += menu->pulse_rate;
    if (menu->pulse >= 1.0f) {
        menu->pulse = 1.0f;
        menu->pulse_rate *= -1.0f;
    }
    if (menu->pulse <= 0.6f) {
        menu->pulse = 0.6f;
        menu->pulse_rate *= -1.0f;
    }

    if (tnt_vita_menu_assembly_advance(&menu->entrance_ticks))
        return TNT_MAIN_MENU_ACTION_NONE;

    if (menu->leaving) {
        menu->fade -= (1.0f / 9.0f) * retrace_delta;
        if (menu->fade <= 0.0f) {
            menu->fade = 0.0f;
            if (menu->selected == 1 && multi_available)
                return TNT_MAIN_MENU_ACTION_ENTER_MULTI_PLAYER;
            if (menu->selected == 2)
                return TNT_MAIN_MENU_ACTION_ENTER_WONDERS;
            if (menu->selected == 3)
                return TNT_MAIN_MENU_ACTION_ENTER_OPTIONS;
            return TNT_MAIN_MENU_ACTION_ENTER_ONE_PLAYER;
        }
        return TNT_MAIN_MENU_ACTION_NONE;
    }

    if (menu->fade < 1.0f) {
        menu->fade += (1.0f / 9.0f) * retrace_delta;
        if (menu->fade > 1.0f)
            menu->fade = 1.0f;
    }

    if (!multi_available && menu->selected == 1)
        menu->selected = 0;

    if (pv_ptr->unk28.unk84 & CONT_UP) {
        menu->idle_ticks = 0;
        if (multi_available)
            menu->selected = (u8)((menu->selected + 3U) & 3U);
        else if (menu->selected == 0)
            menu->selected = 3;
        else if (menu->selected == 2)
            menu->selected = 0;
        else
            menu->selected = 2;
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        return TNT_MAIN_MENU_ACTION_NONE;
    }

    if (pv_ptr->unk28.unk84 & CONT_DOWN) {
        menu->idle_ticks = 0;
        if (multi_available)
            menu->selected = (u8)((menu->selected + 1U) & 3U);
        else if (menu->selected == 0)
            menu->selected = 2;
        else if (menu->selected == 2)
            menu->selected = 3;
        else
            menu->selected = 0;
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        return TNT_MAIN_MENU_ACTION_NONE;
    }

    /* The original root GUI increments D_800D3D2C once per frontend update
     * and requests attract mode at 1000. D-pad movement resets that counter.
     * The outer state then performs the canonical inclusive 0..10 RNG fork:
     * 0..7 gameplay demo, 8..10 full intro replay. */
    if (menu->fade >= 1.0f) {
        if (menu->idle_ticks + retrace_delta >= 1000U) {
            menu->idle_ticks = 0;
            return TNT_MAIN_MENU_ACTION_ENTER_ATTRACT;
        }
        menu->idle_ticks = (u16)(menu->idle_ticks + retrace_delta);
    }

    /* Original root menu uses A to enter GUI_MENU/FUNC items. Start is
     * deliberately ignored here. ONE PLAYER, WONDERS, and OPTIONS begin the
     * generic nine-tick fade before their next front-end state is installed. */
    if ((pv_ptr->unk24 & CONT_A) &&
        (menu->selected == 0 ||
         (menu->selected == 1 && multi_available) ||
         menu->selected == 2 || menu->selected == 3)) {
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
        menu->leaving = TRUE;
        return TNT_MAIN_MENU_ACTION_NONE;
    }

    return TNT_MAIN_MENU_ACTION_NONE;
}

static void tnt_vita_main_menu_render(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    const TNTVitaMainMenu *menu,
    u8 multi_available
) {
    const char *labels[4] = {
        "ONE!PLAYER", TNTV_ROM_OriginalText(26U), "WONDERS", "OPTIONS"
    };
    static const s32 native_y[4] = { 0x61, 0x77, 0x8D, 0xA3 };
    u8 global_alpha;
    int i;

    /* Once the root menu is fully settled, all four labels except the focused
     * pulse are static.  Compose the four unfocused rows once into the shared
     * frontend cache and redraw only the selected row thereafter. */
    if (menu->entrance_ticks == 0 && menu->fade >= 1.0f && !menu->leaving) {
        if (g_tnt_frontend_cache_owner != TNT_FRONTEND_CACHE_MAIN) {
            memcpy(g_tnt_onep_settled_scene, g_tnt_menu_static_scene,
                   sizeof(g_tnt_onep_settled_scene));
            for (i = 0; i < 4; i++) {
                int x = 200 - (TNT_Vita_FontA_TextWidth(labels[i]) / 2);
                u8 a = (i == 1 && !multi_available) ? 0x64 : 0xA0;
                TNT_Vita_FontA_DrawText(
                    g_tnt_onep_settled_scene,
                    DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                    board_x, board_y, cell, x, native_y[i], labels[i],
                    0xFF, 0xFF, 0xFF, a);
            }
            g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_MAIN;
        }

        memcpy(buffer, g_tnt_onep_settled_scene,
               sizeof(g_tnt_onep_settled_scene));
        if (menu->selected < 4) {
            int x = 200 -
                (TNT_Vita_FontA_TextWidth(labels[menu->selected]) / 2);
            u8 pulse = (u8)(menu->pulse * 255.0f);
            tnt_vita_restore_native_rect_from_scene(
                buffer, g_tnt_menu_static_scene,
                board_x, board_y, cell,
                40, native_y[menu->selected],
                360, native_y[menu->selected] + 22);
            TNT_Vita_FontA_DrawText(
                buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                board_x, board_y, cell,
                x, native_y[menu->selected], labels[menu->selected],
                0xFF, pulse, pulse, pulse);
        }
        return;
    }

    tnt_vita_main_menu_copy_cached_frame(
        buffer, board_x, board_y, cell, menu);

    if (menu->entrance_ticks != 0)
        return;

    global_alpha = (u8)(menu->fade * 255.0f);

    for (i = 0; i < 4; i++) {
        int x = 200 - (TNT_Vita_FontA_TextWidth(labels[i]) / 2);
        u8 r = 0xFF;
        u8 g = 0xFF;
        u8 b = 0xFF;
        u8 a = (i == 1 && !multi_available) ? 0x64 : 0xA0;

        if (i == menu->selected) {
            u8 pulse = (u8)(menu->pulse * 255.0f);
            a = pulse;
            g = pulse;
            b = pulse;
        }
        a = (u8)(((u32)a * global_alpha + 127U) / 255U);

        TNT_Vita_FontA_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, x, native_y[i], labels[i],
            r, g, b, a);
    }
}


static void tnt_vita_multi_load_saved_profiles(TNTVitaMultiMenu *menu) {
    int slot;
    menu->profile_count = 0;
    for (slot = 0; slot < TNT_VITA_SRAM_MAX_PLAYERS; slot++) {
        TNTVitaStoredPlayer player;
        u8 i;
        if (TNT_Vita_SramGetPlayer(slot, &player) != 1)
            continue;
        if (menu->profile_count >= 16)
            break;
        /* Same reverse insertion order as the retail frontend TextList. */
        for (i = menu->profile_count; i > 0; i--) {
            memcpy(menu->profile_names[i], menu->profile_names[i - 1], 9);
            menu->profile_slot[i] = menu->profile_slot[i - 1];
        }
        memcpy(menu->profile_names[0], player.name, 9);
        menu->profile_slot[0] = (u8)slot;
        menu->profile_count++;
    }
}

static const char *tnt_vita_multi_slot_name(
    const TNTVitaMultiMenu *menu, int slot
) {
    u8 choice;
    if (slot < 0 || slot >= 4)
        return "GUEST";
    choice = menu->slot_choice[slot];
    if (choice == 0)
        return "GUEST";
    if ((u8)(choice - 1U) < menu->profile_count)
        return menu->profile_names[choice - 1U];
    return "GUEST";
}

static u8 tnt_vita_multi_result_rank(
    const TNTVitaMultiMenu *menu, int slot, u32 match_lines, u32 match_seconds
) {
    u32 total_lines = match_lines;
    u32 total_seconds = match_seconds;
    if (menu != NULL && slot >= 0 && slot < 4) {
        u8 choice = menu->slot_choice[slot];
        if (choice != 0 && (u8)(choice - 1U) < menu->profile_count) {
            TNTVitaStoredPlayer player;
            int profile_slot = menu->profile_slot[choice - 1U];
            if (TNT_Vita_SramGetPlayer(profile_slot, &player) == 1) {
                total_lines += player.lines;
                total_seconds += player.time_in_seconds;
            }
        }
    }
    /* Retail FUN_SRAM_8007875c applies this lifetime-stat rescale before
     * tally.c asks for the post-match rank. Calculate it hypothetically here
     * without mutating TNTV's persistent single-player profile data. */
    if (total_seconds >= 900U) {
        total_lines /= 3U;
        total_seconds /= 3U;
    }
    if (total_lines > 0xFFFFU) total_lines = 0xFFFFU;
    if (total_seconds > 0xFFFFU) total_seconds = 0xFFFFU;
    return (u8)(TNT_Vita_SramRankFor(
        (u16)total_seconds, (u16)total_lines) + 1U);
}

static u8 tnt_vita_multi_choice_claimed(
    const TNTVitaMultiMenu *menu, int owner, u8 choice
) {
    int i;
    if (choice == 0)
        return FALSE; /* Retail GUEST (type 0xE) can be shared. */
    for (i = 0; i < 4; i++) {
        if (i == owner || i >= menu->pad_count)
            continue;
        if (menu->slot_choice[i] == choice)
            return TRUE;
    }
    return FALSE;
}

static void tnt_vita_multi_cycle_slot(
    TNTVitaMultiMenu *menu, int slot, int direction
) {
    u8 count = (u8)(1U + menu->profile_count);
    u8 choice = menu->slot_choice[slot];
    int attempts;
    if (count == 0)
        return;
    for (attempts = 0; attempts < count; attempts++) {
        if (direction > 0)
            choice = (u8)((choice + 1U) % count);
        else
            choice = (u8)((choice + count - 1U) % count);
        if (!tnt_vita_multi_choice_claimed(menu, slot, choice)) {
            menu->slot_choice[slot] = choice;
            menu->slot_confirmed[slot] = FALSE;
            return;
        }
    }
}

static u8 tnt_vita_multi_ready(const TNTVitaMultiMenu *menu) {
    return (u8)(g_tnt_multi_available && menu->pad_count >= 2 &&
        menu->slot_confirmed[0] && menu->slot_confirmed[1]);
}

static void tnt_vita_multi_enter(
    TNTVitaMultiMenu *menu, f32 pulse, f32 pulse_rate
) {
    memset(menu, 0, sizeof(*menu));
    tnt_vita_multi_load_saved_profiles(menu);
    menu->active = TRUE;
    /* Retail MULTI enters with controller 1 focused on its first textbox. */
    menu->selected = 0;
    menu->entrance_ticks = 31;
    menu->game_type = GAMETYPE_MARATHON;
    menu->garbage_type = LANDFILLTYPE_NONE;
    menu->pad_count = g_tnt_multi_pad_count;
    menu->pulse = pulse;
    menu->pulse_rate = pulse_rate;
    menu->fade = 0.0f;
    g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_NONE;
    g_tnt_multi_settled_valid = FALSE;
    memset(g_tnt_multi_slot_pulse_valid, 0, sizeof(g_tnt_multi_slot_pulse_valid));
    g_tnt_multi_slot_pulse_build_cursor = 0;
    if (g_tnt_multi_slot_pulse_cache == NULL) {
        g_tnt_multi_slot_pulse_cache = (uint32_t *)malloc(TNT_MULTI_SLOT_CACHE_BYTES);
        if (g_tnt_multi_slot_pulse_cache != NULL)
            memset(g_tnt_multi_slot_pulse_cache, 0, TNT_MULTI_SLOT_CACHE_BYTES);
    }
    /* Every physical controller-owned textbox uses the same frontend repeat
     * policy as retail. Test67 extends the already-approved P1/P2 setup to
     * P3/P4 without enabling 3P/4P gameplay yet. */
    for (int i = 0; i < 4; i++) {
        FUN_026C80_80060ad4_oneliner_calls_fun(&g_PV_arr[i].unk28);
        FUN_026C80_80060b04_twelveliner_loops_32t(
            &g_PV_arr[i].unk28, 0x0F00, 32, 10);
        FUN_026C80_80060b04_twelveliner_loops_32t(
            &g_PV_arr[i].unk28, 0xC000, 16, 16);
        FUN_026C80_nineliner_loops_32t(&g_PV_arr[i].unk28);
    }
    g_tnt_60hz_last_us = 0;
    g_tnt_60hz_fraction = 0;
}

static int tnt_vita_multi_update(
    TNTVitaMultiMenu *menu, PlayerVars *p1, PlayerVars *p2,
    PlayerVars *p3, PlayerVars *p4
) {
    PlayerVars *players[4] = {p1, p2, p3, p4};
    u32 retrace_delta = tnt_vita_60hz_delta(TRUE);
    u32 repeat1, pressed1;
    if (retrace_delta == 0) retrace_delta = 1;

    menu->pad_count = g_tnt_multi_pad_count;
    menu->pulse += menu->pulse_rate;
    if (menu->pulse >= 1.0f) { menu->pulse = 1.0f; menu->pulse_rate *= -1.0f; }
    if (menu->pulse <= 0.6f) { menu->pulse = 0.6f; menu->pulse_rate *= -1.0f; }

    if (tnt_vita_menu_assembly_advance(&menu->entrance_ticks))
        return TNT_MULTI_ACTION_NONE;

    /* A local match requires two live external pads. If one disappears while
     * this page is open, return cleanly to root and release its claim. */
    if (!g_tnt_multi_available && !menu->leaving) {
        menu->slot_confirmed[1] = FALSE;
        menu->leaving = TRUE;
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_5);
    }

    if (menu->leaving) {
        menu->fade -= (1.0f / 9.0f) * retrace_delta;
        if (menu->fade <= 0.0f) {
            menu->fade = 0.0f;
            return TNT_MULTI_ACTION_BACK;
        }
        return TNT_MULTI_ACTION_NONE;
    }

    if (menu->fade < 1.0f) {
        menu->fade += (1.0f / 9.0f) * retrace_delta;
        if (menu->fade > 1.0f) menu->fade = 1.0f;
    }

    repeat1 = p1->unk28.unk84;
    pressed1 = p1->unk24;

    /* Retail MULTI gives each non-primary controller ownership of its own
     * textbox. Test67 enables the same behavior for P2/P3/P4. GUEST remains
     * shareable; a saved profile can only be claimed by one slot at a time. */
    for (int slot = 1; slot < 4 && slot < (int)menu->pad_count; slot++) {
        PlayerVars *pv = players[slot];
        u32 repeat = pv->unk28.unk84;
        u32 pressed = pv->unk24;
        if (!menu->slot_confirmed[slot]) {
            if (repeat & (CONT_RIGHT | CONT_DOWN)) {
                tnt_vita_multi_cycle_slot(menu, slot, +1);
                Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            } else if (repeat & (CONT_LEFT | CONT_UP)) {
                tnt_vita_multi_cycle_slot(menu, slot, -1);
                Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            }
            if (pressed & CONT_A) {
                menu->slot_confirmed[slot] = TRUE;
                Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
            }
        } else if (pressed & CONT_B) {
            menu->slot_confirmed[slot] = FALSE;
            /* Test67 intentionally leaves START eligibility on the frozen
             * P1/P2 rule; P3/P4 are ownership diagnostics only in this build. */
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_5);
        }
    }

    /* Controller 1 begins in textbox #1. Its focused textbox consumes all
     * four D-pad directions; A confirms and advances to GAME. */
    if (menu->selected == 0) {
        if (repeat1 & (CONT_RIGHT | CONT_DOWN)) {
            tnt_vita_multi_cycle_slot(menu, 0, +1);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_MULTI_ACTION_NONE;
        }
        if (repeat1 & (CONT_LEFT | CONT_UP)) {
            tnt_vita_multi_cycle_slot(menu, 0, -1);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_MULTI_ACTION_NONE;
        }
        if (pressed1 & CONT_A) {
            menu->slot_confirmed[0] = TRUE;
            menu->selected = 1;
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
            return TNT_MULTI_ACTION_NONE;
        }
        if (pressed1 & CONT_B) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_5);
            menu->leaving = TRUE;
            return TNT_MULTI_ACTION_NONE;
        }
        return TNT_MULTI_ACTION_NONE;
    }

    /* P1 owns the ordinary setup rows after confirming its identity. */
    if (repeat1 & CONT_UP) {
        u8 ready = tnt_vita_multi_ready(menu);
        if (menu->selected == 1) menu->selected = ready ? 3 : 2;
        else menu->selected--;
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        return TNT_MULTI_ACTION_NONE;
    }
    if (repeat1 & CONT_DOWN) {
        u8 ready = tnt_vita_multi_ready(menu);
        if (menu->selected == 1) menu->selected = 2;
        else if (menu->selected == 2) menu->selected = ready ? 3 : 1;
        else menu->selected = 1;
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        return TNT_MULTI_ACTION_NONE;
    }
    if (pressed1 & CONT_B) {
        /* Retail B from the setup rows returns P1 to its claimed textbox first. */
        menu->slot_confirmed[0] = FALSE;
        menu->selected = 0;
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_5);
        return TNT_MULTI_ACTION_NONE;
    }

    if (menu->selected == 1) {
        if (pressed1 & CONT_LEFT) {
            menu->game_type = (u8)((menu->game_type + 2U) % 3U);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        } else if (pressed1 & CONT_RIGHT) {
            menu->game_type = (u8)((menu->game_type + 1U) % 3U);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        }
    } else if (menu->selected == 2) {
        if (pressed1 & CONT_LEFT) {
            menu->garbage_type = (u8)((menu->garbage_type + 2U) % 3U);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        } else if (pressed1 & CONT_RIGHT) {
            menu->garbage_type = (u8)((menu->garbage_type + 1U) % 3U);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        }
    } else if (menu->selected == 3 &&
               (pressed1 & (CONT_A | CONT_START))) {
        if (tnt_vita_multi_ready(menu)) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
            return TNT_MULTI_ACTION_START_GAME;
        } else {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_3);
        }
    }
    return TNT_MULTI_ACTION_NONE;
}

static const u8 g_tnt_multi_pulse_levels[TNT_MULTI_SLOT_PULSE_STATES] = {
    153, 165, 178, 191, 204, 216, 229, 242, 255
};

__attribute__((optimize("Os")))
static int tnt_vita_multi_pulse_cache_index(u8 pulse) {
    int best = 0;
    int best_delta = 256;
    int i;
    for (i = 0; i < TNT_MULTI_SLOT_PULSE_STATES; i++) {
        int d = (int)pulse - (int)g_tnt_multi_pulse_levels[i];
        if (d < 0) d = -d;
        if (d < best_delta) {
            best = i;
            best_delta = d;
        }
    }
    return best;
}

__attribute__((optimize("Os")))
static void tnt_vita_multi_slot_cache_bounds(
    int board_x, int board_y, int cell, int slot,
    int *dx0_out, int *dy0_out, int *dx1_out, int *dy1_out
) {
    static const s32 box_x[4] = {0x28,0x28,0xCC,0xCC};
    static const s32 box_y[4] = {0x4D,0x70,0x4D,0x70};
    int dx0 = board_x + tnt_floor_div_s64((s64)(box_x[slot] - 83) * cell, 11);
    int dy0 = board_y + tnt_floor_div_s64((s64)(box_y[slot] - 36) * cell, 11);
    int dx1 = board_x + tnt_floor_div_s64((s64)(box_x[slot] + 154 - 83) * cell + 10, 11);
    int dy1 = board_y + tnt_floor_div_s64((s64)(box_y[slot] + 30 - 36) * cell + 10, 11);
    if (dx0 < 0) dx0 = 0;
    if (dy0 < 0) dy0 = 0;
    if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
    if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;
    *dx0_out = dx0; *dy0_out = dy0; *dx1_out = dx1; *dy1_out = dy1;
}

/* Build one exact pulse/color state per rendered entrance frame.  MULTI's
 * falling-block entrance lasts long enough at 60-Hz presentation to warm all
 * 36 states before the settled page appears, avoiding a one-frame precompute
 * hitch.  The old software draw remains as a fallback for any missing state. */
__attribute__((optimize("Os")))
static void tnt_vita_multi_build_next_slot_pulse_cache(
    int board_x, int board_y, int cell
) {
    static const s32 box_x[4] = {0x28,0x28,0xCC,0xCC};
    static const s32 box_y[4] = {0x4D,0x70,0x4D,0x70};
    int cursor = (int)g_tnt_multi_slot_pulse_build_cursor;
    int slot, state, dx0, dy0, dx1, dy1, w, h, y;
    u8 pulse, r = 0, g = 0, b = 0;
    uint32_t *cache;

    if (g_tnt_multi_slot_pulse_cache == NULL ||
        cursor >= 4 * TNT_MULTI_SLOT_PULSE_STATES)
        return;
    slot = cursor / TNT_MULTI_SLOT_PULSE_STATES;
    state = cursor % TNT_MULTI_SLOT_PULSE_STATES;
    tnt_vita_multi_slot_cache_bounds(board_x, board_y, cell, slot,
        &dx0, &dy0, &dx1, &dy1);
    w = dx1 - dx0; h = dy1 - dy0;
    if (w <= 0 || h <= 0 || w > TNT_MULTI_SLOT_CACHE_W || h > TNT_MULTI_SLOT_CACHE_H) {
        g_tnt_multi_slot_pulse_build_cursor = (u8)(4 * TNT_MULTI_SLOT_PULSE_STATES);
        return;
    }
    g_tnt_multi_slot_cache_dx[slot] = (s16)dx0;
    g_tnt_multi_slot_cache_dy[slot] = (s16)dy0;
    g_tnt_multi_slot_cache_w[slot] = (u16)w;
    g_tnt_multi_slot_cache_h[slot] = (u16)h;

    /* Pulse-cache scratch must NEVER alias g_tnt_tally_static_scene. Test78
     * exposed name-select outlines stamped into TALLY because test70 had
     * reused the precomposed TALLY scene as temporary storage. Gameplay's
     * decode buffer is dormant throughout MULTI and is overwritten before
     * game play starts, so it is safe scratch space here. */
    tnt_vita_restore_native_rect_from_scene(
        g_tnt_gameplay_base_scene, g_tnt_wonder_static_scene,
        board_x, board_y, cell, box_x[slot], box_y[slot],
        box_x[slot] + 154, box_y[slot] + 30);
    pulse = g_tnt_multi_pulse_levels[state];
    if (slot == 0) r = pulse;
    else if (slot == 1) g = pulse;
    else if (slot == 2) b = pulse;
    else r = g = pulse;
    tnt_vita_draw_original_ia8_sprite_tinted(
        g_tnt_gameplay_base_scene, board_x, board_y, cell,
        box_x[slot], box_y[slot], 154, 30,
        g_tnt_onep_selection_box_ia8, r, g, b, pulse);

    cache = TNT_MULTI_SLOT_CACHE_PTR(slot, state);
    for (y = 0; y < h; y++) {
        memcpy(cache + y * TNT_MULTI_SLOT_CACHE_W,
               g_tnt_gameplay_base_scene + (dy0 + y) * DISPLAY_STRIDE + dx0,
               (size_t)w * sizeof(uint32_t));
    }
    g_tnt_multi_slot_pulse_valid[slot][state] = TRUE;
    g_tnt_multi_slot_pulse_build_cursor++;
}

static void tnt_vita_multi_build_static(
    int board_x, int board_y, int cell
) {
    /* Keep a panel-only backing image for tiny dynamic-region restores.  The
     * settled MULTI page is composed separately into g_tnt_onep_settled_scene.
     * This avoids rerasterizing the 400x255 panel whenever focus/value changes. */
    memcpy(g_tnt_wonder_static_scene, g_tnt_menu_background_scene,
           sizeof(g_tnt_wonder_static_scene));
    tnt_vita_draw_original_ia8_sprite(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        200 - (388 >> 1), 0x2E, 388, 252,
        g_tnt_multi_shadow_ia8, 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        0, 0x1E, 400, 255,
        g_tnt_multi_panel_ci8, g_tnt_multi_panel_rgba16, 0xFF);
    tnt_vita_draw_original_rgba16_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        200 - (132 >> 1), 0xFC, 132, 15,
        g_tnt_multi_accept_back_rgba16, 0xFF);
    memcpy(g_tnt_onep_settled_scene, g_tnt_wonder_static_scene,
           sizeof(g_tnt_onep_settled_scene));
    g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_MULTI;
}

static void tnt_vita_multi_copy_frame(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaMultiMenu *menu
) {
    uint32_t *dst = (uint32_t *)buffer;
    int step;
    if (menu->entrance_ticks != 0) {
        tnt_vita_menu_render_assembly(
            buffer, board_x, board_y, cell, menu->entrance_ticks);
        return;
    }
    if (g_tnt_frontend_cache_owner != TNT_FRONTEND_CACHE_MULTI)
        tnt_vita_multi_build_static(board_x, board_y, cell);
    if (menu->fade <= 0.0f) {
        memcpy(dst, g_tnt_menu_background_scene, sizeof(g_tnt_menu_background_scene));
        return;
    }
    if (menu->fade >= 1.0f) {
        memcpy(dst, g_tnt_onep_settled_scene, sizeof(g_tnt_onep_settled_scene));
        return;
    }
    memcpy(dst, g_tnt_menu_background_scene, sizeof(g_tnt_menu_background_scene));
    step = (int)(menu->fade * 9.0f + 0.5f);
    if (step < 1) step = 1;
    if (step > 8) step = 8;
    {
        int dx0 = board_x + tnt_floor_div_s64((s64)(0 - 83) * cell, 11);
        int dy0 = board_y + tnt_floor_div_s64((s64)(30 - 36) * cell, 11);
        int dx1 = board_x + tnt_floor_div_s64((s64)(400 - 83) * cell + 10, 11);
        int dy1 = board_y + tnt_floor_div_s64((s64)(298 - 36) * cell + 10, 11);
        int y;
        if (dx0 < 0) dx0 = 0;
        if (dy0 < 0) dy0 = 0;
        if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
        if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;
        for (y = dy0; y < dy1; y++) {
            int x; uint32_t *dr = dst + y * DISPLAY_STRIDE;
            const uint32_t *br = g_tnt_menu_background_scene + y * DISPLAY_STRIDE;
            const uint32_t *fr = g_tnt_onep_settled_scene + y * DISPLAY_STRIDE;
            for (x = dx0; x < dx1; x++) {
                uint32_t bg = br[x], fg = fr[x];
                int r0,g0,b0,r1,g1,b1;
                if (bg == fg) continue;
                r0=bg&255; g0=(bg>>8)&255; b0=(bg>>16)&255;
                r1=fg&255; g1=(fg>>8)&255; b1=(fg>>16)&255;
                dr[x]=COLOR((u8)(r0+((r1-r0)*step)/9),
                            (u8)(g0+((g1-g0)*step)/9),
                            (u8)(b0+((b1-b0)*step)/9));
            }
        }
    }
}

static void tnt_vita_multi_draw_slots(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaMultiMenu *menu, u8 global_alpha
) {
    static const s32 box_x[4] = {0x28,0x28,0xCC,0xCC};
    static const s32 box_y[4] = {0x4D,0x70,0x4D,0x70};
    static const s32 name_x[4] = {0x2D,0x2D,0xD1,0xD1};
    static const s32 name_y[4] = {0x55,0x78,0x55,0x78};
    int i;
    u8 pulse=(u8)(menu->pulse*255.0f);
    for (i=0;i<4;i++) {
        const char *name = tnt_vita_multi_slot_name(menu,i);
        if (i < (int)menu->pad_count) {
            u8 active = (u8)((i==0 && menu->selected==0) ||
                             (i>0 && !menu->slot_confirmed[i]));
            u8 level = active ? pulse : 0xA0;
            u8 alpha = (u8)(((u32)level*global_alpha+127U)/255U);
            u8 r=0,g=0,b=0;
            if (i==0) r=level;
            else if (i==1) g=level;
            else if (i==2) b=level;
            else r=g=level;
            tnt_vita_draw_original_ia8_sprite_tinted(
                buffer,board_x,board_y,cell,box_x[i],box_y[i],154,30,
                g_tnt_onep_selection_box_ia8,r,g,b,alpha);
            TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
                board_x,board_y,cell,name_x[i],name_y[i],name,
                0xFF,active?pulse:0xFF,active?pulse:0xFF,
                (u8)(((u32)(active?pulse:0xFF)*global_alpha+127U)/255U));
        } else {
            TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
                board_x,board_y,cell,name_x[i],name_y[i],"GUEST",
                0xFF,0xFF,0xFF,(u8)(((u32)0x50*global_alpha+127U)/255U));
        }
    }
}

static void tnt_vita_multi_render_uncached(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaMultiMenu *menu
) {
    static const char *const games[3] = {"MARATHON","SPRINT","ULTRA"};
    static const s32 game_x[3] = {0xE4,0x10C,0x112};
    const char *garbage = menu->garbage_type == LANDFILLTYPE_HOTPOTATO
        ? "HOT POTATO" : (menu->garbage_type == LANDFILLTYPE_DIRECTED
        ? "DIRECTED" : "NONE");
    s32 garbage_x = menu->garbage_type == LANDFILLTYPE_HOTPOTATO ? 0xD0 :
        (menu->garbage_type == LANDFILLTYPE_DIRECTED ? 0xF7 : 0x118);
    u8 global_alpha,pulse,base,sel;
    int start_x;

    tnt_vita_multi_copy_frame(buffer,board_x,board_y,cell,menu);
    if(menu->entrance_ticks!=0)return;
    global_alpha=(u8)(menu->fade*255.0f); pulse=(u8)(menu->pulse*255.0f);
    base=(u8)(((u32)0xA0*global_alpha+127U)/255U);
    sel=(u8)(((u32)pulse*global_alpha+127U)/255U);

    tnt_vita_multi_draw_slots(buffer,board_x,board_y,cell,menu,global_alpha);

    TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
        board_x,board_y,cell,0x3C,0xA0,TNTV_ROM_OriginalText(42U),0xFF,
        menu->selected==1?pulse:0xFF,menu->selected==1?pulse:0xFF,
        menu->selected==1?sel:base);
    TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
        board_x,board_y,cell,game_x[menu->game_type],0xA0,games[menu->game_type],
        0xFF,0xFF,0xFF,menu->selected==1?sel:base);
    TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
        board_x,board_y,cell,0x3C,0xB7,"GARBAGE:",0xFF,
        menu->selected==2?pulse:0xFF,menu->selected==2?pulse:0xFF,
        menu->selected==2?sel:base);
    TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
        board_x,board_y,cell,garbage_x,0xB7,garbage,
        0xFF,0xFF,0xFF,menu->selected==2?sel:base);
    if (tnt_vita_multi_ready(menu)) {
        start_x=200-(TNT_Vita_FontA_TextWidth("START")>>1);
        TNT_Vita_FontA_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
            board_x,board_y,cell,start_x,0xDB,"START",0xFF,
            menu->selected==3?pulse:0xFF,menu->selected==3?pulse:0xFF,
            menu->selected==3?sel:base);
    }
}

static void tnt_vita_multi_draw_game_row_base(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaMultiMenu *menu
) {
    static const char *const games[3] = {"MARATHON","SPRINT","ULTRA"};
    static const s32 game_x[3] = {0xE4,0x10C,0x112};
    TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
        board_x,board_y,cell,0x3C,0xA0,TNTV_ROM_OriginalText(42U),0xFF,0xFF,0xFF,0xA0);
    TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
        board_x,board_y,cell,game_x[menu->game_type],0xA0,games[menu->game_type],
        0xFF,0xFF,0xFF,0xA0);
}

static void tnt_vita_multi_draw_garbage_row_base(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaMultiMenu *menu
) {
    const char *garbage = menu->garbage_type == LANDFILLTYPE_HOTPOTATO
        ? "HOT POTATO" : (menu->garbage_type == LANDFILLTYPE_DIRECTED
        ? "DIRECTED" : "NONE");
    s32 garbage_x = menu->garbage_type == LANDFILLTYPE_HOTPOTATO ? 0xD0 :
        (menu->garbage_type == LANDFILLTYPE_DIRECTED ? 0xF7 : 0x118);
    TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
        board_x,board_y,cell,0x3C,0xB7,"GARBAGE:",0xFF,0xFF,0xFF,0xA0);
    TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
        board_x,board_y,cell,garbage_x,0xB7,garbage,0xFF,0xFF,0xFF,0xA0);
}

static void tnt_vita_multi_draw_start_row_base(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaMultiMenu *menu
) {
    if (tnt_vita_multi_ready(menu)) {
        int start_x = 200 - (TNT_Vita_FontA_TextWidth("START") >> 1);
        TNT_Vita_FontA_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
            board_x,board_y,cell,start_x,0xDB,"START",0xFF,0xFF,0xFF,0xA0);
    }
}

static void tnt_vita_multi_patch_slots_cache(
    int board_x, int board_y, int cell, const TNTVitaMultiMenu *menu
) {
    static const s32 box_x[4] = {0x28,0x28,0xCC,0xCC};
    static const s32 box_y[4] = {0x4D,0x70,0x4D,0x70};
    TNTVitaMultiMenu base = *menu;
    int i;
    for (i=0;i<4;i++)
        tnt_vita_restore_native_rect_from_scene(
            g_tnt_onep_settled_scene, g_tnt_wonder_static_scene,
            board_x,board_y,cell,box_x[i],box_y[i],box_x[i]+154,box_y[i]+30);
    /* Force every connected slot to its nonfocused appearance.  The live frame
     * restores/redraws only controller-owned slots that are actually pulsing. */
    base.selected = 0xFF;
    for (i=1;i<4;i++)
        base.slot_confirmed[i] = TRUE;
    tnt_vita_multi_draw_slots(
        g_tnt_onep_settled_scene,board_x,board_y,cell,&base,0xFF);
}

static void tnt_vita_multi_patch_game_cache(
    int board_x, int board_y, int cell, const TNTVitaMultiMenu *menu
) {
    tnt_vita_restore_native_rect_from_scene(
        g_tnt_onep_settled_scene,g_tnt_wonder_static_scene,
        board_x,board_y,cell,0x32,0x9A,0x17C,0xB2);
    tnt_vita_multi_draw_game_row_base(
        g_tnt_onep_settled_scene,board_x,board_y,cell,menu);
}

static void tnt_vita_multi_patch_garbage_cache(
    int board_x, int board_y, int cell, const TNTVitaMultiMenu *menu
) {
    tnt_vita_restore_native_rect_from_scene(
        g_tnt_onep_settled_scene,g_tnt_wonder_static_scene,
        board_x,board_y,cell,0x32,0xB1,0x17C,0xCA);
    tnt_vita_multi_draw_garbage_row_base(
        g_tnt_onep_settled_scene,board_x,board_y,cell,menu);
}

static void tnt_vita_multi_patch_start_cache(
    int board_x, int board_y, int cell, const TNTVitaMultiMenu *menu
) {
    tnt_vita_restore_native_rect_from_scene(
        g_tnt_onep_settled_scene,g_tnt_wonder_static_scene,
        board_x,board_y,cell,0x78,0xD5,0x118,0xF0);
    tnt_vita_multi_draw_start_row_base(
        g_tnt_onep_settled_scene,board_x,board_y,cell,menu);
}

static void tnt_vita_multi_record_cache_state(const TNTVitaMultiMenu *menu) {
    int i;
    g_tnt_multi_cache_game_type=menu->game_type;
    g_tnt_multi_cache_garbage_type=menu->garbage_type;
    g_tnt_multi_cache_ready=tnt_vita_multi_ready(menu);
    g_tnt_multi_cache_pad_count=menu->pad_count;
    for(i=0;i<4;i++) {
        g_tnt_multi_cache_slot_choice[i]=menu->slot_choice[i];
        g_tnt_multi_cache_slot_confirmed[i]=menu->slot_confirmed[i];
    }
}

static void tnt_vita_multi_compose_settled_cache(
    int board_x, int board_y, int cell, const TNTVitaMultiMenu *menu
) {
    tnt_vita_multi_build_static(board_x,board_y,cell);
    tnt_vita_multi_patch_slots_cache(board_x,board_y,cell,menu);
    tnt_vita_multi_patch_game_cache(board_x,board_y,cell,menu);
    tnt_vita_multi_patch_garbage_cache(board_x,board_y,cell,menu);
    tnt_vita_multi_patch_start_cache(board_x,board_y,cell,menu);
    tnt_vita_multi_record_cache_state(menu);
    g_tnt_multi_settled_valid=TRUE;
}

static void tnt_vita_multi_update_settled_cache(
    int board_x, int board_y, int cell, const TNTVitaMultiMenu *menu
) {
    int i;
    u8 slots_changed=(u8)(g_tnt_multi_cache_pad_count!=menu->pad_count);
    u8 ready=tnt_vita_multi_ready(menu);
    for(i=0;i<4;i++) {
        if(g_tnt_multi_cache_slot_choice[i]!=menu->slot_choice[i] ||
           g_tnt_multi_cache_slot_confirmed[i]!=menu->slot_confirmed[i])
            slots_changed=TRUE;
    }
    if(slots_changed) {
        tnt_vita_multi_patch_slots_cache(board_x,board_y,cell,menu);
        g_tnt_multi_cache_pad_count=menu->pad_count;
        for(i=0;i<4;i++) {
            g_tnt_multi_cache_slot_choice[i]=menu->slot_choice[i];
            g_tnt_multi_cache_slot_confirmed[i]=menu->slot_confirmed[i];
        }
    }
    if(g_tnt_multi_cache_game_type!=menu->game_type) {
        tnt_vita_multi_patch_game_cache(board_x,board_y,cell,menu);
        g_tnt_multi_cache_game_type=menu->game_type;
    }
    if(g_tnt_multi_cache_garbage_type!=menu->garbage_type) {
        tnt_vita_multi_patch_garbage_cache(board_x,board_y,cell,menu);
        g_tnt_multi_cache_garbage_type=menu->garbage_type;
    }
    if(g_tnt_multi_cache_ready!=ready) {
        tnt_vita_multi_patch_start_cache(board_x,board_y,cell,menu);
        g_tnt_multi_cache_ready=ready;
    }
}

static void tnt_vita_multi_draw_active_slots(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaMultiMenu *menu
) {
    static const s32 box_x[4]={0x28,0x28,0xCC,0xCC};
    static const s32 box_y[4]={0x4D,0x70,0x4D,0x70};
    static const s32 name_x[4]={0x2D,0x2D,0xD1,0xD1};
    static const s32 name_y[4]={0x55,0x78,0x55,0x78};
    u8 pulse=(u8)(menu->pulse*255.0f);
    int i;
    for(i=0;i<(int)menu->pad_count && i<4;i++) {
        u8 active=(u8)((i==0 && menu->selected==0) ||
                       (i>0 && !menu->slot_confirmed[i]));
        u8 r=0,g=0,b=0;
        if(!active) continue;
        {
            int state = tnt_vita_multi_pulse_cache_index(pulse);
            if (g_tnt_multi_slot_pulse_cache != NULL &&
                g_tnt_multi_slot_pulse_valid[i][state]) {
                uint32_t *dst = (uint32_t *)buffer;
                const uint32_t *src = TNT_MULTI_SLOT_CACHE_PTR(i, state);
                int dx = g_tnt_multi_slot_cache_dx[i];
                int dy = g_tnt_multi_slot_cache_dy[i];
                int w = g_tnt_multi_slot_cache_w[i];
                int h = g_tnt_multi_slot_cache_h[i];
                int yy;
                for (yy = 0; yy < h; yy++) {
                    memcpy(dst + (dy + yy) * DISPLAY_STRIDE + dx,
                           src + yy * TNT_MULTI_SLOT_CACHE_W,
                           (size_t)w * sizeof(uint32_t));
                }
            } else {
                tnt_vita_restore_native_rect_from_scene(
                    buffer,g_tnt_wonder_static_scene,board_x,board_y,cell,
                    box_x[i],box_y[i],box_x[i]+154,box_y[i]+30);
                if(i==0) r=pulse;
                else if(i==1) g=pulse;
                else if(i==2) b=pulse;
                else r=g=pulse;
                tnt_vita_draw_original_ia8_sprite_tinted(
                    buffer,board_x,board_y,cell,box_x[i],box_y[i],154,30,
                    g_tnt_onep_selection_box_ia8,r,g,b,pulse);
            }
        }
        TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
            board_x,board_y,cell,name_x[i],name_y[i],
            tnt_vita_multi_slot_name(menu,i),0xFF,pulse,pulse,pulse);
    }
}

static void tnt_vita_multi_draw_selected_row(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaMultiMenu *menu
) {
    static const char *const games[3]={"MARATHON","SPRINT","ULTRA"};
    static const s32 game_x[3]={0xE4,0x10C,0x112};
    const char *garbage=menu->garbage_type==LANDFILLTYPE_HOTPOTATO?"HOT POTATO":
        (menu->garbage_type==LANDFILLTYPE_DIRECTED?"DIRECTED":"NONE");
    s32 garbage_x=menu->garbage_type==LANDFILLTYPE_HOTPOTATO?0xD0:
        (menu->garbage_type==LANDFILLTYPE_DIRECTED?0xF7:0x118);
    u8 pulse=(u8)(menu->pulse*255.0f);
    int start_x=200-(TNT_Vita_FontA_TextWidth("START")>>1);
    if(menu->selected==1){
        tnt_vita_restore_native_rect_from_scene(buffer,g_tnt_wonder_static_scene,
            board_x,board_y,cell,0x32,0x9A,0x17C,0xB2);
        TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,board_x,board_y,cell,0x3C,0xA0,TNTV_ROM_OriginalText(42U),0xFF,pulse,pulse,pulse);
        TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,board_x,board_y,cell,game_x[menu->game_type],0xA0,games[menu->game_type],0xFF,0xFF,0xFF,pulse);
    }else if(menu->selected==2){
        tnt_vita_restore_native_rect_from_scene(buffer,g_tnt_wonder_static_scene,
            board_x,board_y,cell,0x32,0xB1,0x17C,0xCA);
        TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,board_x,board_y,cell,0x3C,0xB7,"GARBAGE:",0xFF,pulse,pulse,pulse);
        TNT_Vita_FontB_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,board_x,board_y,cell,garbage_x,0xB7,garbage,0xFF,0xFF,0xFF,pulse);
    }else if(menu->selected==3 && tnt_vita_multi_ready(menu)){
        tnt_vita_restore_native_rect_from_scene(buffer,g_tnt_wonder_static_scene,
            board_x,board_y,cell,0x78,0xD5,0x118,0xF0);
        TNT_Vita_FontA_DrawText(buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,board_x,board_y,cell,start_x,0xDB,"START",0xFF,pulse,pulse,pulse);
    }
}

static void tnt_vita_multi_render(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaMultiMenu *menu
) {
    /* Prepare the panel scratch image as soon as MULTI begins, then amortize
     * one exact name-box pulse state per rendered frame behind the entrance. */
    if (g_tnt_frontend_cache_owner != TNT_FRONTEND_CACHE_MULTI)
        tnt_vita_multi_build_static(board_x, board_y, cell);
    tnt_vita_multi_build_next_slot_pulse_cache(board_x, board_y, cell);

    if (menu->entrance_ticks != 0 || menu->fade < 1.0f) {
        tnt_vita_multi_render_uncached(buffer, board_x, board_y, cell, menu);
        return;
    }
    if(!g_tnt_multi_settled_valid ||
       g_tnt_frontend_cache_owner!=TNT_FRONTEND_CACHE_MULTI)
        tnt_vita_multi_compose_settled_cache(board_x,board_y,cell,menu);
    else
        tnt_vita_multi_update_settled_cache(board_x,board_y,cell,menu);
    memcpy(buffer,g_tnt_onep_settled_scene,sizeof(g_tnt_onep_settled_scene));
    tnt_vita_multi_draw_active_slots(buffer,board_x,board_y,cell,menu);
    tnt_vita_multi_draw_selected_row(buffer,board_x,board_y,cell,menu);
}

static void tnt_vita_frontend_copy_shared_cache(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    u8 entrance_ticks,
    f32 fade,
    int native_x0,
    int native_y0,
    int native_x1,
    int native_y1
) {
    uint32_t *dst = (uint32_t *)buffer;
    int step;

    if (entrance_ticks != 0) {
        tnt_vita_menu_render_assembly(
            buffer, board_x, board_y, cell, entrance_ticks);
        return;
    }

    if (fade <= 0.0f) {
        memcpy(dst, g_tnt_menu_background_scene,
               sizeof(g_tnt_menu_background_scene));
        return;
    }
    if (fade >= 1.0f) {
        memcpy(dst, g_tnt_wonder_static_scene,
               sizeof(g_tnt_wonder_static_scene));
        return;
    }

    memcpy(dst, g_tnt_menu_background_scene,
           sizeof(g_tnt_menu_background_scene));
    step = (int)(fade * 9.0f + 0.5f);
    if (step < 1) step = 1;
    if (step > 8) step = 8;

    {
        int dx0 = board_x + tnt_floor_div_s64((s64)(native_x0 - 83) * cell, 11);
        int dy0 = board_y + tnt_floor_div_s64((s64)(native_y0 - 36) * cell, 11);
        int dx1 = board_x + tnt_floor_div_s64((s64)(native_x1 - 83) * cell + 10, 11);
        int dy1 = board_y + tnt_floor_div_s64((s64)(native_y1 - 36) * cell + 10, 11);
        int y;
        if (dx0 < 0) dx0 = 0;
        if (dy0 < 0) dy0 = 0;
        if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
        if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;
        for (y = dy0; y < dy1; y++) {
            int x;
            uint32_t *dst_row = dst + y * DISPLAY_STRIDE;
            const uint32_t *bg_row = g_tnt_menu_background_scene + y * DISPLAY_STRIDE;
            const uint32_t *fg_row = g_tnt_wonder_static_scene + y * DISPLAY_STRIDE;
            for (x = dx0; x < dx1; x++) {
                uint32_t bg = bg_row[x];
                uint32_t fg = fg_row[x];
                int br, bgc, bb, fr, fgc, fb;
                if (bg == fg) continue;
                br = (int)(bg & 0xFFU);
                bgc = (int)((bg >> 8) & 0xFFU);
                bb = (int)((bg >> 16) & 0xFFU);
                fr = (int)(fg & 0xFFU);
                fgc = (int)((fg >> 8) & 0xFFU);
                fb = (int)((fg >> 16) & 0xFFU);
                dst_row[x] = COLOR(
                    (u8)(br + ((fr - br) * step) / 9),
                    (u8)(bgc + ((fgc - bgc) * step) / 9),
                    (u8)(bb + ((fb - bb) * step) / 9));
            }
        }
    }
}

static void tnt_vita_options_build_cache(
    int board_x, int board_y, int cell
) {
    memcpy(g_tnt_wonder_static_scene, g_tnt_menu_background_scene,
           sizeof(g_tnt_wonder_static_scene));
    tnt_vita_draw_original_ia8_sprite(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        40, 50, 320, 221, g_tnt_options_shadow_ia8, 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        44, 40, 312, 213,
        g_tnt_options_panel_ci8, g_tnt_options_panel_rgba16, 0xFF);
    tnt_vita_draw_original_rgba16_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        200 - (132 >> 1), 252, 132, 15,
        g_tnt_onep_a_accept_b_back_rgba16, 0xFF);
}

static void tnt_vita_options_enter(
    TNTVitaOptionsMenu *menu,
    PlayerVars *pv_ptr,
    int board_x, int board_y, int cell,
    f32 pulse, f32 pulse_rate,
    u8 selected
) {
    memset(menu, 0, sizeof(*menu));
    menu->active = TRUE;
    /* All four authored OPTIONS rows now have Vita-side destinations.
     * Child returns restore the row they came from. */
    menu->selected = selected < 4 ? selected : 0;
    menu->entrance_ticks = 31;
    menu->pulse = pulse;
    menu->pulse_rate = pulse_rate;
    menu->fade = 0.0f;
    tnt_vita_options_build_cache(board_x, board_y, cell);
    g_tnt_60hz_last_us = 0;
    g_tnt_60hz_fraction = 0;
    FUN_026C80_80060ad4_oneliner_calls_fun(&pv_ptr->unk28);
    FUN_026C80_80060b04_twelveliner_loops_32t(
        &pv_ptr->unk28, 0x0F00, 32, 10);
    FUN_026C80_80060b04_twelveliner_loops_32t(
        &pv_ptr->unk28, 0xC000, 16, 16);
    FUN_026C80_nineliner_loops_32t(&pv_ptr->unk28);
}

static int tnt_vita_options_update(
    TNTVitaOptionsMenu *menu, PlayerVars *pv_ptr
) {
    u32 delta = tnt_vita_60hz_delta(TRUE);
    if (delta == 0) delta = 1;
    menu->pulse += menu->pulse_rate;
    if (menu->pulse >= 1.0f) { menu->pulse = 1.0f; menu->pulse_rate *= -1.0f; }
    if (menu->pulse <= 0.6f) { menu->pulse = 0.6f; menu->pulse_rate *= -1.0f; }

    if (tnt_vita_menu_assembly_advance(&menu->entrance_ticks))
        return TNT_OPTIONS_ACTION_NONE;
    if (menu->leaving) {
        menu->fade -= (1.0f / 9.0f) * delta;
        if (menu->fade <= 0.0f) {
            menu->fade = 0.0f;
            if (menu->leave_action == TNT_OPTIONS_ACTION_SCORES)
                return TNT_OPTIONS_ACTION_SCORES;
            if (menu->leave_action == TNT_OPTIONS_ACTION_AUDIO)
                return TNT_OPTIONS_ACTION_AUDIO;
            if (menu->leave_action == TNT_OPTIONS_ACTION_CREDITS)
                return TNT_OPTIONS_ACTION_CREDITS;
            if (menu->leave_action == TNT_OPTIONS_ACTION_DATA)
                return TNT_OPTIONS_ACTION_DATA;
            return TNT_OPTIONS_ACTION_BACK;
        }
        return TNT_OPTIONS_ACTION_NONE;
    }
    if (menu->fade < 1.0f) {
        menu->fade += (1.0f / 9.0f) * delta;
        if (menu->fade > 1.0f) menu->fade = 1.0f;
    }

    if (pv_ptr->unk28.unk84 & CONT_DOWN) {
        menu->selected = (u8)((menu->selected + 1U) & 3U);
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        return TNT_OPTIONS_ACTION_NONE;
    }
    if (pv_ptr->unk28.unk84 & CONT_UP) {
        menu->selected = (u8)((menu->selected + 3U) & 3U);
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        return TNT_OPTIONS_ACTION_NONE;
    }
    if (pv_ptr->unk24 & CONT_B) {
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_5);
        menu->leaving = TRUE;
        menu->leave_action = TNT_OPTIONS_ACTION_BACK;
        return TNT_OPTIONS_ACTION_NONE;
    }
    if (pv_ptr->unk24 & CONT_A) {
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
        if (menu->selected == 0) {
            menu->leaving = TRUE;
            menu->leave_action = TNT_OPTIONS_ACTION_SCORES;
        } else if (menu->selected == 1) {
            menu->leaving = TRUE;
            menu->leave_action = TNT_OPTIONS_ACTION_DATA;
        } else if (menu->selected == 2) {
            menu->leaving = TRUE;
            menu->leave_action = TNT_OPTIONS_ACTION_AUDIO;
        } else if (menu->selected == 3) {
            menu->leaving = TRUE;
            menu->leave_action = TNT_OPTIONS_ACTION_CREDITS;
        }
    }
    return TNT_OPTIONS_ACTION_NONE;
}

static void tnt_vita_options_render(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaOptionsMenu *menu
) {
    const char *const labels[4] = {
        TNTV_ROM_OriginalText(48U), TNTV_ROM_OriginalText(49U), TNTV_ROM_OriginalText(50U), TNTV_ROM_OriginalText(51U)
    };
    static const s32 ys[4] = {103, 126, 149, 172};
    static const u8 base_alpha[4] = {0xA0, 0xFF, 0xFF, 0xFF};
    u8 global_alpha;
    int i;

    if (menu->entrance_ticks == 0 && menu->fade >= 1.0f && !menu->leaving) {
        if (g_tnt_frontend_cache_owner != TNT_FRONTEND_CACHE_OPTIONS) {
            memcpy(g_tnt_onep_settled_scene, g_tnt_wonder_static_scene,
                   sizeof(g_tnt_onep_settled_scene));
            for (i = 0; i < 4; i++) {
                int x = 200 - (TNT_Vita_FontA_TextWidth(labels[i]) / 2);
                TNT_Vita_FontA_DrawText(
                    g_tnt_onep_settled_scene,
                    DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                    board_x, board_y, cell, x, ys[i], labels[i],
                    0xFF, 0xFF, 0xFF, base_alpha[i]);
            }
            g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_OPTIONS;
        }
        memcpy(buffer, g_tnt_onep_settled_scene,
               sizeof(g_tnt_onep_settled_scene));
        if (menu->selected < 4) {
            int x = 200 -
                (TNT_Vita_FontA_TextWidth(labels[menu->selected]) / 2);
            u8 pulse = (u8)(menu->pulse * 255.0f);
            tnt_vita_restore_native_rect_from_scene(
                buffer, g_tnt_wonder_static_scene,
                board_x, board_y, cell,
                60, ys[menu->selected],
                340, ys[menu->selected] + 22);
            TNT_Vita_FontA_DrawText(
                buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                board_x, board_y, cell, x, ys[menu->selected],
                labels[menu->selected], 0xFF, pulse, pulse, pulse);
        }
        return;
    }

    tnt_vita_frontend_copy_shared_cache(
        buffer, board_x, board_y, cell,
        menu->entrance_ticks, menu->fade, 40, 40, 364, 267);
    if (menu->entrance_ticks != 0) return;
    global_alpha = (u8)(menu->fade * 255.0f);
    for (i = 0; i < 4; i++) {
        u8 g = 0xFF, b = 0xFF;
        u8 a = base_alpha[i];
        int x = 200 - (TNT_Vita_FontA_TextWidth(labels[i]) / 2);
        if (i == menu->selected) {
            u8 pulse = (u8)(menu->pulse * 255.0f);
            g = pulse; b = pulse; a = pulse;
        }
        a = (u8)(((u32)a * global_alpha + 127U) / 255U);
        TNT_Vita_FontA_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, x, ys[i], labels[i],
            0xFF, g, b, a);
    }
}

static void tnt_vita_data_build_cache(
    int board_x, int board_y, int cell
) {
    memcpy(g_tnt_wonder_static_scene, g_tnt_menu_background_scene,
           sizeof(g_tnt_wonder_static_scene));

    /* D_800D42B4 / D_800D4254: exact source DATA composition. */
    tnt_vita_draw_original_ia8_sprite(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        1, 17, 390, 284, g_tnt_data_shadow_ia8, 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        2, 24, 396, 254,
        g_tnt_data_panel_ci8, g_tnt_data_panel_rgba16, 0xFF);

    /* D_800D41F0's IMG_SELECTION_BOX is state-colored by the source GUI,
     * so it is drawn dynamically in tnt_vita_data_render rather than baked. */
    tnt_vita_draw_original_rgba16_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        200 - (132 >> 1), 265, 132, 15,
        g_tnt_onep_a_accept_b_back_rgba16, 0xFF);
}

static void tnt_vita_data_load_profiles(TNTVitaDataMenu *menu) {
    int slot;
    menu->profile_count = 0;
    for (slot = 0; slot < TNT_VITA_SRAM_MAX_PLAYERS; slot++) {
        TNTVitaStoredPlayer player;
        if (TNT_Vita_SramGetPlayer(slot, &player) != 1)
            continue;
        if (menu->profile_count >= TNT_VITA_SRAM_MAX_PLAYERS)
            break;
        menu->profile_slots[menu->profile_count] = (u8)slot;
        memcpy(menu->profile_names[menu->profile_count], player.name, 9);
        menu->profile_names[menu->profile_count][8] = '\0';
        menu->profile_count++;
    }
    if (menu->name_choice > menu->profile_count)
        menu->name_choice = 0;
}

static const char *tnt_vita_data_name_text(const TNTVitaDataMenu *menu) {
    if (menu->name_choice == 0)
        return "CHOOSE";
    if ((u8)(menu->name_choice - 1U) < menu->profile_count)
        return menu->profile_names[menu->name_choice - 1U];
    return "CHOOSE";
}

static void tnt_vita_data_enter(
    TNTVitaDataMenu *menu, PlayerVars *pv_ptr,
    int board_x, int board_y, int cell,
    f32 pulse, f32 pulse_rate
) {
    memset(menu, 0, sizeof(*menu));
    menu->active = TRUE;
    menu->selected = 0; /* Source no-Pak path starts at TRANSFER NAME. */
    menu->entrance_ticks = 31;
    menu->pulse = pulse;
    menu->pulse_rate = pulse_rate;
    menu->fade = 0.0f;
    tnt_vita_data_load_profiles(menu);
    tnt_vita_data_build_cache(board_x, board_y, cell);
    g_tnt_60hz_last_us = 0;
    g_tnt_60hz_fraction = 0;
    FUN_026C80_80060ad4_oneliner_calls_fun(&pv_ptr->unk28);
    FUN_026C80_80060b04_twelveliner_loops_32t(
        &pv_ptr->unk28, 0x0F00, 32, 10);
    FUN_026C80_80060b04_twelveliner_loops_32t(
        &pv_ptr->unk28, 0xC000, 16, 16);
    FUN_026C80_nineliner_loops_32t(&pv_ptr->unk28);
}

static int tnt_vita_data_update(
    TNTVitaDataMenu *menu, PlayerVars *pv_ptr
) {
    u32 delta = tnt_vita_60hz_delta(TRUE);
    u32 repeat = pv_ptr->unk28.unk84;
    if (delta == 0) delta = 1;

    menu->pulse += menu->pulse_rate;
    if (menu->pulse >= 1.0f) {
        menu->pulse = 1.0f;
        menu->pulse_rate *= -1.0f;
    }
    if (menu->pulse <= 0.6f) {
        menu->pulse = 0.6f;
        menu->pulse_rate *= -1.0f;
    }

    if (tnt_vita_menu_assembly_advance(&menu->entrance_ticks))
        return TNT_DATA_ACTION_NONE;
    if (menu->leaving) {
        menu->fade -= (1.0f / 9.0f) * delta;
        if (menu->fade <= 0.0f) {
            menu->fade = 0.0f;
            return TNT_DATA_ACTION_BACK;
        }
        return TNT_DATA_ACTION_NONE;
    }
    if (menu->fade < 1.0f) {
        menu->fade += (1.0f / 9.0f) * delta;
        if (menu->fade > 1.0f) menu->fade = 1.0f;
    }

    /* Original state 0xF0: no Controller Pak. */
    if (menu->modal_no_pak) {
        if (pv_ptr->unk24 & CONT_A) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_3);
            menu->modal_no_pak = FALSE;
        }
        return TNT_DATA_ACTION_NONE;
    }

    /* Original state 0xF1 after deleting an internal type-13 SRAM player. */
    if (menu->delete_success) {
        if (pv_ptr->unk24 & CONT_A) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
            menu->delete_success = FALSE;
            menu->name_selecting = FALSE;
            menu->name_choice = 0;
        }
        return TNT_DATA_ACTION_NONE;
    }

    /* Original state 0xFB from DELETE NAME: A accepts, B cancels. */
    if (menu->confirm_delete) {
        if (pv_ptr->unk24 & CONT_B) {
            menu->confirm_delete = FALSE;
            return TNT_DATA_ACTION_NONE;
        }
        if (pv_ptr->unk24 & CONT_A) {
            u8 index = (u8)(menu->name_choice - 1U);
            if (menu->name_choice != 0 && index < menu->profile_count) {
                int slot = menu->profile_slots[index];
                if (TNT_Vita_SramDeletePlayer(slot) == 0) {
                    (void)TNT_Vita_SramSave();
                    menu->confirm_delete = FALSE;
                    menu->delete_success = TRUE;
                    tnt_vita_data_load_profiles(menu);
                }
            }
        }
        return TNT_DATA_ACTION_NONE;
    }

    /* DELETE NAME activates the source D_800D41F0 textbox.  Internal SRAM
     * players are present in this list even with no Controller Pak.  The
     * original accepts all four directions as previous/next list controls. */
    if (menu->name_selecting) {
        u8 count = (u8)(menu->profile_count + 1U); /* CHOOSE + SRAM names */
        if ((repeat & CONT_DOWN) || (repeat & CONT_RIGHT)) {
            menu->name_choice = (u8)((menu->name_choice + 1U) % count);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_DATA_ACTION_NONE;
        }
        if ((repeat & CONT_UP) || (repeat & CONT_LEFT)) {
            menu->name_choice = (u8)((menu->name_choice + count - 1U) % count);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_DATA_ACTION_NONE;
        }
        if (pv_ptr->unk24 & CONT_B) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_5);
            menu->name_selecting = FALSE;
            menu->name_choice = 0;
            return TNT_DATA_ACTION_NONE;
        }
        if ((pv_ptr->unk24 & CONT_A) && menu->name_choice != 0) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
            menu->confirm_delete = TRUE;
        }
        return TNT_DATA_ACTION_NONE;
    }

    if (repeat & CONT_DOWN) {
        menu->selected = (u8)((menu->selected + 1U) % 3U);
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        return TNT_DATA_ACTION_NONE;
    }
    if (repeat & CONT_UP) {
        menu->selected = (u8)((menu->selected + 2U) % 3U);
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        return TNT_DATA_ACTION_NONE;
    }
    if (pv_ptr->unk24 & CONT_B) {
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_5);
        menu->leaving = TRUE;
        return TNT_DATA_ACTION_NONE;
    }
    if (pv_ptr->unk24 & CONT_A) {
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_2);
        if (menu->selected == 2) {
            /* func_800A08A0: DELETE NAME moves focus to the NAME textbox;
             * it does not require a Controller Pak for internal SRAM names. */
            tnt_vita_data_load_profiles(menu);
            menu->name_choice = 0;
            menu->name_selecting = TRUE;
        } else {
            /* TRANSFER NAME and DUMP LINES TO GAME PAK both hit source state
             * 0xF0 immediately when no Controller Pak/Transfer Pak is present. */
            menu->modal_no_pak = TRUE;
        }
    }
    return TNT_DATA_ACTION_NONE;
}

static void tnt_vita_data_render_modal(
    void *buffer, int board_x, int board_y, int cell, u8 alpha
) {
    int x;
    int y = 50;
    u8 yellow_r = 0xFF, yellow_g = 0xFF, yellow_b = 0x00;

    /* func_80091D60 / state 0xF0: IMG_RECTANGLE at (50,40), Font B at (60,50). */
    tnt_vita_draw_original_ia8_sprite(
        buffer, board_x, board_y, cell,
        50, 40, 300, 100, g_tnt_modal_rectangle_ia8, alpha);
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 60, y, TNTV_ROM_OriginalText(4U),
        0xFF, 0xFF, 0xFF, alpha);
    y += 15;
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 60, y, TNTV_ROM_OriginalText(25U),
        0xFF, 0xFF, 0xFF, alpha);
    y += 15;
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 60, y, TNTV_ROM_OriginalText(24U),
        0xFF, 0xFF, 0xFF, alpha);
    y += 25;

    x = 60;
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, y, "PRESS ",
        yellow_r, yellow_g, yellow_b, alpha);
    x += TNT_Vita_FontB_TextWidth("PRESS ");
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, y, "A ",
        0x5A, 0x50, 0xB9, alpha);
    x += TNT_Vita_FontB_TextWidth("A ");
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, y, TNTV_ROM_OriginalText(2U),
        yellow_r, yellow_g, yellow_b, alpha);
}

static void tnt_vita_data_render_delete_confirm(
    void *buffer, int board_x, int board_y, int cell, u8 alpha
) {
    int x;
    int y = 50;
    tnt_vita_draw_original_ia8_sprite(
        buffer, board_x, board_y, cell,
        50, 40, 300, 100, g_tnt_modal_rectangle_ia8, alpha);
    TNT_Vita_FontB_DrawText(buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 60, y, TNTV_ROM_OriginalText(21U),
        0xFF, 0xFF, 0xFF, alpha);
    y += 15;
    TNT_Vita_FontB_DrawText(buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 60, y, TNTV_ROM_OriginalText(22U),
        0xFF, 0xFF, 0xFF, alpha);
    y += 25;
    x = 60;
    TNT_Vita_FontB_DrawText(buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, y, "A ", 0x5A, 0x50, 0xB9, alpha);
    x += TNT_Vita_FontB_TextWidth("A ");
    TNT_Vita_FontB_DrawText(buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, y, "ACCEPT ", 0xFF, 0xFF, 0x00, alpha);
    x += TNT_Vita_FontB_TextWidth("ACCEPT ");
    TNT_Vita_FontB_DrawText(buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, y, "B ", 0x70, 0xC3, 0x74, alpha);
    x += TNT_Vita_FontB_TextWidth("B ");
    TNT_Vita_FontB_DrawText(buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, y, "CANCEL ", 0xFF, 0xFF, 0x00, alpha);
}

static void tnt_vita_data_render_delete_success(
    void *buffer, int board_x, int board_y, int cell, u8 alpha
) {
    int x;
    int y = 60;
    tnt_vita_draw_original_ia8_sprite(
        buffer, board_x, board_y, cell,
        50, 40, 300, 100, g_tnt_modal_rectangle_ia8, alpha);
    TNT_Vita_FontB_DrawText(buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 60, y, TNTV_ROM_OriginalText(23U),
        0xFF, 0xFF, 0xFF, alpha);
    y += 20;
    x = 60;
    TNT_Vita_FontB_DrawText(buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, y, "PRESS ", 0xFF, 0xFF, 0x00, alpha);
    x += TNT_Vita_FontB_TextWidth("PRESS ");
    TNT_Vita_FontB_DrawText(buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, y, "A ", 0x5A, 0x50, 0xB9, alpha);
    x += TNT_Vita_FontB_TextWidth("A ");
    TNT_Vita_FontB_DrawText(buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, y, TNTV_ROM_OriginalText(2U), 0xFF, 0xFF, 0x00, alpha);
}

static void tnt_vita_data_render_uncached(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaDataMenu *menu
) {
    const char *const labels[3] = {
        TNTV_ROM_OriginalText(27U), TNTV_ROM_OriginalText(28U), TNTV_ROM_OriginalText(29U)
    };
    static const s32 xs[3] = {97, 97, 97};
    static const s32 ys[3] = {92, 114, 162};
    u8 global_alpha;
    int i;

    tnt_vita_frontend_copy_shared_cache(
        buffer, board_x, board_y, cell,
        menu->entrance_ticks, menu->fade, 1, 17, 398, 301);
    if (menu->entrance_ticks != 0)
        return;

    global_alpha = (u8)(menu->fade * 255.0f);
    for (i = 0; i < 3; i++) {
        u8 c = 0xFF;
        u8 a = (u8)(((u32)0xA0 * global_alpha + 127U) / 255U);
        if (i == menu->selected && !menu->modal_no_pak &&
            !menu->name_selecting && !menu->confirm_delete &&
            !menu->delete_success) {
            c = (u8)(menu->pulse * 255.0f);
            a = (u8)(((u32)c * global_alpha + 127U) / 255U);
        }
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, xs[i], ys[i], labels[i],
            0xFF, c, c, a);
    }

    /* Source entry 8 is Font A; D_800D41F0 displays its CHOOSE TextList in B.
     * State 0xF8 keeps IMG_SELECTION_BOX visible in white while inactive;
     * active name selection tints the same IA8 mask with the red focus pulse. */
    {
        u8 name_c = 0xFF;
        u8 name_a = (u8)(((u32)0xA0 * global_alpha + 127U) / 255U);
        if (menu->name_selecting && !menu->confirm_delete && !menu->delete_success) {
            name_c = (u8)(menu->pulse * 255.0f);
            name_a = (u8)(((u32)name_c * global_alpha + 127U) / 255U);
        }
        tnt_vita_draw_original_ia8_sprite_tinted(
            buffer, board_x, board_y, cell,
            190, 204, 154, 30, g_tnt_onep_selection_box_ia8,
            0xFF, name_c, name_c, name_a);
        TNT_Vita_FontA_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 80, 205, TNTV_ROM_OriginalText(40U),
            0xFF, name_c, name_c, name_a);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 195, 212, tnt_vita_data_name_text(menu),
            0xFF, name_c, name_c, global_alpha);
    }

    if (menu->modal_no_pak)
        tnt_vita_data_render_modal(
            buffer, board_x, board_y, cell, global_alpha);
    else if (menu->confirm_delete)
        tnt_vita_data_render_delete_confirm(
            buffer, board_x, board_y, cell, global_alpha);
    else if (menu->delete_success)
        tnt_vita_data_render_delete_success(
            buffer, board_x, board_y, cell, global_alpha);
}

static void tnt_vita_data_render(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaDataMenu *menu
) {
    const char *const labels[3] = {
        TNTV_ROM_OriginalText(27U), TNTV_ROM_OriginalText(28U), TNTV_ROM_OriginalText(29U)
    };
    static const s32 xs[3] = {97, 97, 97};
    static const s32 ys[3] = {92, 114, 162};
    u8 kind;

    if (menu->entrance_ticks != 0 || menu->fade < 1.0f || menu->leaving) {
        tnt_vita_data_render_uncached(buffer, board_x, board_y, cell, menu);
        return;
    }

    kind = menu->modal_no_pak ? 2U :
        (menu->confirm_delete ? 3U : (menu->delete_success ? 4U : 1U));

    if (g_tnt_frontend_cache_owner != TNT_FRONTEND_CACHE_DATA ||
        g_tnt_frontend_cache_kind != kind) {
        TNTVitaDataMenu cached = *menu;
        cached.selected = 0xFF;
        cached.pulse = 1.0f;
        cached.fade = 1.0f;
        cached.entrance_ticks = 0;
        cached.leaving = FALSE;
        /* The ordinary base always carries CHOOSE underneath the live NAME
         * field.  Active name selection restores this strip before drawing
         * its current live value, so cycling profiles never rebuilds the page. */
        if (kind == 1U) {
            cached.name_selecting = FALSE;
            cached.name_choice = 0;
            cached.modal_no_pak = FALSE;
            cached.confirm_delete = FALSE;
            cached.delete_success = FALSE;
        } else {
            /* Modal pages have no pulsing controls; cache the exact complete
             * page once, including the dialog and current underlying name. */
            cached.name_selecting = FALSE;
        }
        tnt_vita_data_render_uncached(
            g_tnt_onep_settled_scene, board_x, board_y, cell, &cached);
        g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_DATA;
        g_tnt_frontend_cache_kind = kind;
    }

    memcpy(buffer, g_tnt_onep_settled_scene,
           sizeof(g_tnt_onep_settled_scene));

    /* Static dialog states need no live drawing at all. */
    if (kind != 1U)
        return;

    if (menu->name_selecting) {
        u8 pulse = (u8)(menu->pulse * 255.0f);
        tnt_vita_restore_native_rect_from_scene(
            buffer, g_tnt_wonder_static_scene,
            board_x, board_y, cell, 70, 198, 355, 240);
        tnt_vita_draw_original_ia8_sprite_tinted(
            buffer, board_x, board_y, cell,
            190, 204, 154, 30, g_tnt_onep_selection_box_ia8,
            0xFF, pulse, pulse, pulse);
        TNT_Vita_FontA_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 80, 205, TNTV_ROM_OriginalText(40U),
            0xFF, pulse, pulse, pulse);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 195, 212, tnt_vita_data_name_text(menu),
            0xFF, pulse, pulse, 0xFF);
        return;
    }

    if (menu->selected < 3) {
        u8 pulse = (u8)(menu->pulse * 255.0f);
        tnt_vita_restore_native_rect_from_scene(
            buffer, g_tnt_wonder_static_scene,
            board_x, board_y, cell,
            80, ys[menu->selected] - 2,
            360, ys[menu->selected] + 18);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell,
            xs[menu->selected], ys[menu->selected], labels[menu->selected],
            0xFF, pulse, pulse, pulse);
    }
}

static u8 tnt_vita_audio_max_song(void) {
    u32 total = TNT_Vita_SramGetTotalWonderLines();
    u8 current_wonder = 0;
    if (total >= 500000U) return 15;
    while (current_wonder < 6 && total >= g_tnt_wonder_totals[current_wonder])
        current_wonder++;
    return (u8)(current_wonder + 9);
}

static u8 tnt_vita_max_restored_gameplay_screen(void) {
    u32 total;
    /* TNTV4ALL is an access override only: expose every restored board without
     * changing the persisted Wonder-line total that normally gates this roll. */
    if (TNT_Vita_SramGetUnlockAll())
        return 7U;
    /* Canonical Game_Init rolls 0..D_800CF838. Match the restored subset:
     * each completed Wonder adds its corresponding gameplay environment. */
    total = TNT_Vita_SramGetTotalWonderLines();
    if (total >= 500000U)
        return 7U;
    if (total >= 250000U)
        return 6U;
    if (total >= 100000U)
        return 5U;
    if (total >= 50000U)
        return 4U;
    if (total >= 20000U)
        return 3U;
    if (total >= 7500U)
        return 2U;
    if (total >= 2500U)
        return 1U;
    return 0U;
}

static void tnt_vita_music_start_gameplay_screen(u8 screen) {
    u8 mode = TNT_Vita_SramGetMusicMode();
    u8 song;

    /* FUN_027BF0_check_music_settings_and_play. CHOOSE leaves the currently
     * selected song alone. AUTO uses the screen-specific song on 30 of 51
     * rolls, otherwise a song from 1..screen+9. RANDOM always uses that range.
     * Screen-specific AUTO selection follows the randomly selected unlocked
     * gameplay board. */
    if (mode == 1)
        return;
    if (screen > 7)
        screen = 7;
    if (mode == 0 && func_800A35EC(0, 50) < 30) {
        static const u8 screen_song[8] = {9, 10, 11, 12, 13, 14, 15, 15};
        song = screen_song[screen];
    } else {
        song = (u8)func_800A35EC(1, screen + 9);
    }

    if (TNT_Vita_MusicPlaySong(song) == 0) {
        /* Audio_80084e20(..., arg2=1) updates the in-memory SRAM song field.
         * It is persisted later by the same ordinary SRAM save paths. */
        TNT_Vita_SramSetSong(song);
    }
}

static void tnt_vita_audio_build_cache(
    int board_x, int board_y, int cell
) {
    memcpy(g_tnt_wonder_static_scene, g_tnt_menu_background_scene,
           sizeof(g_tnt_wonder_static_scene));
    tnt_vita_draw_original_ia8_sprite(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        0, 23, 390, 276, g_tnt_audio_shadow_ia8, 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        3, 15, 395, 262,
        g_tnt_audio_panel_ci8, g_tnt_audio_panel_rgba16, 0xFF);
    tnt_vita_draw_original_rgba16_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        184, 88, 158, 25, g_tnt_audio_slider_wood_rgba16, 0xFF);
    tnt_vita_draw_original_rgba16_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        184, 118, 158, 25, g_tnt_audio_slider_wood_rgba16, 0xFF);
    tnt_vita_draw_original_rgba16_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        200 - (132 >> 1), 267, 132, 15,
        g_tnt_onep_a_accept_b_back_rgba16, 0xFF);
}

static void tnt_vita_audio_enter(
    TNTVitaAudioMenu *menu,
    PlayerVars *pv_ptr,
    int board_x, int board_y, int cell,
    f32 pulse, f32 pulse_rate
) {
    u8 max_song = tnt_vita_audio_max_song();
    memset(menu, 0, sizeof(*menu));
    menu->active = TRUE;
    menu->selected = 0;
    menu->entrance_ticks = 31;
    menu->pulse = pulse;
    menu->pulse_rate = pulse_rate;
    menu->music_level = TNT_Vita_SramGetMusicLevelStep();
    menu->sfx_level = TNT_Vita_SramGetSfxLevelStep();
    menu->music_mode = TNT_Vita_SramGetMusicMode();
    menu->song = TNT_Vita_SramGetSong();
    if (menu->song > max_song) menu->song = 0;
    tnt_vita_audio_build_cache(board_x, board_y, cell);
    g_tnt_60hz_last_us = 0;
    g_tnt_60hz_fraction = 0;
    FUN_026C80_80060ad4_oneliner_calls_fun(&pv_ptr->unk28);
    FUN_026C80_80060b04_twelveliner_loops_32t(
        &pv_ptr->unk28, 0x0F00, 32, 10);
    FUN_026C80_80060b04_twelveliner_loops_32t(
        &pv_ptr->unk28, 0xC000, 16, 16);
    FUN_026C80_nineliner_loops_32t(&pv_ptr->unk28);
}

static int tnt_vita_audio_update(
    TNTVitaAudioMenu *menu, PlayerVars *pv_ptr
) {
    u32 delta = tnt_vita_60hz_delta(TRUE);
    u32 repeat = pv_ptr->unk28.unk84;
    u8 changed = FALSE;
    if (delta == 0) delta = 1;
    menu->pulse += menu->pulse_rate;
    if (menu->pulse >= 1.0f) { menu->pulse = 1.0f; menu->pulse_rate *= -1.0f; }
    if (menu->pulse <= 0.6f) { menu->pulse = 0.6f; menu->pulse_rate *= -1.0f; }

    if (tnt_vita_menu_assembly_advance(&menu->entrance_ticks))
        return TNT_AUDIO_ACTION_NONE;
    if (menu->leaving) {
        menu->fade -= (1.0f / 9.0f) * delta;
        if (menu->fade <= 0.0f) {
            menu->fade = 0.0f;
            return TNT_AUDIO_ACTION_BACK;
        }
        return TNT_AUDIO_ACTION_NONE;
    }
    if (menu->fade < 1.0f) {
        menu->fade += (1.0f / 9.0f) * delta;
        if (menu->fade > 1.0f) menu->fade = 1.0f;
    }

    if (repeat & CONT_UP) {
        menu->selected = (u8)((menu->selected + 3) % 4);
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        return TNT_AUDIO_ACTION_NONE;
    }
    if (repeat & CONT_DOWN) {
        menu->selected = (u8)((menu->selected + 1) % 4);
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        return TNT_AUDIO_ACTION_NONE;
    }
    if (pv_ptr->unk24 & CONT_B) {
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_5);
        menu->leaving = TRUE;
        return TNT_AUDIO_ACTION_NONE;
    }

    if (menu->selected == 0) {
        if ((repeat & CONT_LEFT) && menu->music_level > 0) {
            menu->music_level--; changed = TRUE;
        } else if ((repeat & CONT_RIGHT) && menu->music_level < 13) {
            menu->music_level++; changed = TRUE;
        }
        if (changed) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            TNT_Vita_SramSetMusicLevelStep(menu->music_level);
            tnt_vita_save_to_sram();
        }
    } else if (menu->selected == 1) {
        if ((repeat & CONT_LEFT) && menu->sfx_level > 0) {
            menu->sfx_level--; changed = TRUE;
        } else if ((repeat & CONT_RIGHT) && menu->sfx_level < 13) {
            menu->sfx_level++; changed = TRUE;
        }
        if (changed) {
            TNT_Vita_SramSetSfxLevelStep(menu->sfx_level);
            tnt_vita_save_to_sram();
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        }
    } else if (menu->selected == 2) {
        if (pv_ptr->unk24 & CONT_LEFT) {
            menu->music_mode = (u8)((menu->music_mode + 2) % 3); changed = TRUE;
        } else if (pv_ptr->unk24 & CONT_RIGHT) {
            menu->music_mode = (u8)((menu->music_mode + 1) % 3); changed = TRUE;
        }
        if (changed) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            TNT_Vita_SramSetMusicMode(menu->music_mode);
            tnt_vita_save_to_sram();
        }
    } else if (menu->selected == 3) {
        u8 max_song = tnt_vita_audio_max_song();
        if (repeat & CONT_LEFT) {
            menu->song = menu->song == 0 ? max_song : (u8)(menu->song - 1);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        } else if (repeat & CONT_RIGHT) {
            menu->song = menu->song >= max_song ? 0 : (u8)(menu->song + 1);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        }
        /* Original SONG textbox previews its circular list with the D-pad and
         * commits g_sram_ptr->song only when A is pressed. */
        if (pv_ptr->unk24 & CONT_A) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
            TNT_Vita_SramSetSong(menu->song);
            tnt_vita_save_to_sram();
            /* Original frontend sets D_800D3D88 here; the next frontend
             * update reloads g_sram_ptr->song and starts it immediately. */
            (void)TNT_Vita_MusicPlaySong(menu->song);
        }
    }
    return TNT_AUDIO_ACTION_NONE;
}

static void tnt_vita_audio_render_uncached(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaAudioMenu *menu
) {
    static const char *const modes[3] = {"AUTO", "CHOOSE", "RANDOM"};
    static const char *const songs[16] = {
        "TITLE", "MOROCCO", "DVIE", "POLYASIA", "FLOPPY", "PYRAMID",
        "GIALI", "THREAD6", "HALUCI", "MAYAN", "GREEK", "EGYPT",
        "CELTIC", "AFRICA", "JAPAN", "KALINKA"
    };
    static const int mark_y[14] = {
        93,94,91,92,91,92,91,92,91,92,91,91,92,93
    };
    const char *const labels[4] = {
        TNTV_ROM_OriginalText(30U), "SFX LEVEL", TNTV_ROM_OriginalText(31U), "SONG:"
    };
    static const int label_y[4] = {92,122,175,207};
    static const u8 base_alpha[4] = {0xFF,0xFF,0xA0,0xA0};
    u8 global_alpha;
    int i;

    tnt_vita_frontend_copy_shared_cache(
        buffer, board_x, board_y, cell,
        menu->entrance_ticks, menu->fade, 0, 15, 400, 299);
    if (menu->entrance_ticks != 0) return;
    global_alpha = (u8)(menu->fade * 255.0f);

    for (i = 0; i < 4; i++) {
        u8 g = 0xFF, b = 0xFF;
        u8 a = base_alpha[i];
        if (i == menu->selected) {
            u8 pulse = (u8)(menu->pulse * 255.0f);
            g = pulse; b = pulse; a = pulse;
        }
        a = (u8)(((u32)a * global_alpha + 127U) / 255U);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 60, label_y[i], labels[i],
            0xFF, g, b, a);
    }

    {
        u8 mode_a = menu->selected == 2
            ? (u8)(menu->pulse * (f32)global_alpha)
            : (u8)(((u32)0xA0 * global_alpha + 127U) / 255U);
        u8 song_a = menu->selected == 3
            ? (u8)(menu->pulse * (f32)global_alpha)
            : (u8)(((u32)0xA0 * global_alpha + 127U) / 255U);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 191, 175, modes[menu->music_mode],
            0xFF, 0xFF, 0xFF, mode_a);
        {
            u8 song_c = menu->selected == 3
                ? (u8)(menu->pulse * 255.0f) : 0xFF;
            u8 box_a = menu->selected == 3
                ? song_a
                : (u8)(((u32)0xA0 * global_alpha + 127U) / 255U);
            /* D_800D4CDC: IMG_SELECTION_BOX at (0xB9,0xC8), text +(5,8). */
            tnt_vita_draw_original_ia8_sprite_tinted(
                buffer, board_x, board_y, cell,
                0xB9, 0xC8, 154, 30, g_tnt_onep_selection_box_ia8,
                0xFF, song_c, song_c, box_a);
        }
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 190, 208, songs[menu->song],
            0xFF, 0xFF, 0xFF, song_a);
    }

    for (i = 0; i < 14; i++) {
        u8 alpha = (u8)(i <= menu->music_level ? global_alpha : 0);
        if (alpha != 0) {
            tnt_vita_draw_original_rgba16_sprite_alpha(
                buffer, board_x, board_y, cell,
                193 + 10 * i, mark_y[i], 10, 16,
                g_tnt_audio_slider_feather_rgba16, alpha);
        }
        alpha = (u8)(i <= menu->sfx_level ? global_alpha : 0);
        if (alpha != 0) {
            tnt_vita_draw_original_rgba16_sprite_alpha(
                buffer, board_x, board_y, cell,
                193 + 10 * i, mark_y[i] + 30, 10, 16,
                g_tnt_audio_slider_feather_rgba16, alpha);
        }
    }
}

static void tnt_vita_audio_render(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaAudioMenu *menu
) {
    static const char *const modes[3] = {"AUTO", "CHOOSE", "RANDOM"};
    static const char *const songs[16] = {
        "TITLE", "MOROCCO", "DVIE", "POLYASIA", "FLOPPY", "PYRAMID",
        "GIALI", "THREAD6", "HALUCI", "MAYAN", "GREEK", "EGYPT",
        "CELTIC", "AFRICA", "JAPAN", "KALINKA"
    };
    static const int mark_y[14] = {
        93,94,91,92,91,92,91,92,91,92,91,91,92,93
    };
    u32 key;
    u8 pulse;
    int i;

    if (menu->entrance_ticks != 0 || menu->fade < 1.0f || menu->leaving) {
        tnt_vita_audio_render_uncached(buffer, board_x, board_y, cell, menu);
        return;
    }

    key = (u32)menu->music_level |
          ((u32)menu->sfx_level << 4) |
          ((u32)menu->music_mode << 8) |
          ((u32)menu->song << 10);
    if (g_tnt_frontend_cache_owner != TNT_FRONTEND_CACHE_AUDIO ||
        g_tnt_frontend_cache_key != key) {
        TNTVitaAudioMenu cached = *menu;
        cached.selected = 0xFF;
        cached.pulse = 1.0f;
        cached.fade = 1.0f;
        cached.entrance_ticks = 0;
        cached.leaving = FALSE;
        tnt_vita_audio_render_uncached(
            g_tnt_onep_settled_scene, board_x, board_y, cell, &cached);
        g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_AUDIO;
        g_tnt_frontend_cache_key = key;
    }

    memcpy(buffer, g_tnt_onep_settled_scene,
           sizeof(g_tnt_onep_settled_scene));
    pulse = (u8)(menu->pulse * 255.0f);

    if (menu->selected == 0) {
        tnt_vita_restore_native_rect_from_scene(
            buffer, g_tnt_wonder_static_scene,
            board_x, board_y, cell, 50, 86, 350, 114);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 60, 92, TNTV_ROM_OriginalText(30U),
            0xFF, pulse, pulse, pulse);
        for (i = 0; i < 14; i++) {
            if (i <= menu->music_level) {
                tnt_vita_draw_original_rgba16_sprite_alpha(
                    buffer, board_x, board_y, cell,
                    193 + 10 * i, mark_y[i], 10, 16,
                    g_tnt_audio_slider_feather_rgba16, 0xFF);
            }
        }
        return;
    }
    if (menu->selected == 1) {
        tnt_vita_restore_native_rect_from_scene(
            buffer, g_tnt_wonder_static_scene,
            board_x, board_y, cell, 50, 116, 350, 144);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 60, 122, "SFX LEVEL",
            0xFF, pulse, pulse, pulse);
        for (i = 0; i < 14; i++) {
            if (i <= menu->sfx_level) {
                tnt_vita_draw_original_rgba16_sprite_alpha(
                    buffer, board_x, board_y, cell,
                    193 + 10 * i, mark_y[i] + 30, 10, 16,
                    g_tnt_audio_slider_feather_rgba16, 0xFF);
            }
        }
        return;
    }
    if (menu->selected == 2) {
        tnt_vita_restore_native_rect_from_scene(
            buffer, g_tnt_wonder_static_scene,
            board_x, board_y, cell, 50, 169, 345, 194);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 60, 175, TNTV_ROM_OriginalText(31U),
            0xFF, pulse, pulse, pulse);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 191, 175, modes[menu->music_mode],
            0xFF, 0xFF, 0xFF, pulse);
        return;
    }
    if (menu->selected == 3) {
        tnt_vita_restore_native_rect_from_scene(
            buffer, g_tnt_wonder_static_scene,
            board_x, board_y, cell, 50, 197, 350, 237);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 60, 207, "SONG:",
            0xFF, pulse, pulse, pulse);
        tnt_vita_draw_original_ia8_sprite_tinted(
            buffer, board_x, board_y, cell,
            0xB9, 0xC8, 154, 30, g_tnt_onep_selection_box_ia8,
            0xFF, pulse, pulse, pulse);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 190, 208, songs[menu->song],
            0xFF, 0xFF, 0xFF, pulse);
    }
}

/* TEST102: TNTV4ALL-only visual correction to SINGLE's framed panel.
 * The ROM's 397x251 CI8 artwork contains a 295px-wide one-pixel
 * rounded border at panel-local y104..175 (OPPONENT / GAME). The
 * original 395x252 IA8 shadow supplies its translucent dark interior.
 * A native TNTV4ALL STAGE row is y196 and historically sat outside the
 * authored two-row rectangle. Never change the original ROM assets.
 *
 * Create an alternate panel/shadow ONCE, retaining the original palette,
 * pixel-perfect top corners and both vertical borders, moving only the
 * bottom seven rounded-border rows down 28 native GUI pixels. Extend the
 * existing shadow alpha into STAGE rather than superimposing a different
 * black rectangle. The normal unlocked/off path consumes the original
 * immutable sprites, so the N64 SINGLE layout is byte-for-byte identical.
 * No per-frame work: this is used only in static scene composition. */
static u8 g_tnt_onep_stage_panel_ci8[397 * 251];
static u8 g_tnt_onep_stage_shadow_ia8[395 * 252];
static u8 g_tnt_onep_stage_assets_ready;
static u8 g_tnt_onep_cached_unlock_flag;

static void tnt_vita_onep_prepare_stage_frame(void) {
    int x, y;
    if (g_tnt_onep_stage_assets_ready) return;

    memcpy(g_tnt_onep_stage_panel_ci8, g_tnt_onep_panel_ci8,
           sizeof(g_tnt_onep_stage_panel_ci8));
    memcpy(g_tnt_onep_stage_shadow_ia8, g_tnt_onep_shadow_ia8,
           sizeof(g_tnt_onep_stage_shadow_ia8));

    /* The ROM palette's sole transparent entry is #251 (alpha bit 0).
     * Erase only the bottom border of the inner rounded rectangle;
     * the surrounding SINGLE wood rails and original typography art
     * are untouched. */
    for (y = 169; y <= 175; y++) {
        for (x = 49; x <= 343; x++) {
            int pos = y * 397 + x;
            if (g_tnt_onep_panel_rgba16[g_tnt_onep_stage_panel_ci8[pos]] & 1U)
                g_tnt_onep_stage_panel_ci8[pos] = 251;
        }
    }

    /* Continue the original two side borders at the exact same x using
     * an unmodified straight frame row, then paste the exact original
     * rounded lower seven rows at y197..203 (28 pixels lower). */
    for (y = 169; y <= 196; y++)
        memcpy(g_tnt_onep_stage_panel_ci8 + y * 397 + 49,
               g_tnt_onep_panel_ci8 + 145 * 397 + 49,
               295);
    for (y = 197; y <= 203; y++)
        memcpy(g_tnt_onep_stage_panel_ci8 + y * 397 + 49,
               g_tnt_onep_panel_ci8 + (y - 28) * 397 + 49,
               295);

    /* The IA8 source holds the actual dark translucent well behind the
     * outline. The original well's lower fade occurs around GUI y196..
     * 201, so move that fade 28 pixels while keeping the original
     * interior darkness and feathered borders intact. */
    for (y = 190; y < 230; y++) {
        int sample_global_y = y < 218 ? 184 : (y - 28);
        memcpy(g_tnt_onep_stage_shadow_ia8 + (y - 37) * 395 + 45,
               g_tnt_onep_shadow_ia8
                   + (sample_global_y - 37) * 395 + 45,
               302);
    }
    g_tnt_onep_stage_assets_ready = TRUE;
}

static void tnt_vita_onep_build_cache(
    int board_x,
    int board_y,
    int cell
) {
    u8 stage_extra = TNT_Vita_SramGetUnlockAll() ? TRUE : FALSE;
    if (stage_extra)
        tnt_vita_onep_prepare_stage_frame();
    g_tnt_onep_cached_unlock_flag = stage_extra;

    g_tnt_onep_settled_valid = FALSE;
    g_tnt_onep_cache_kind = 0;
    g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_NONE;
    memcpy(
        g_tnt_onep_static_scene,
        g_tnt_menu_background_scene,
        sizeof(g_tnt_onep_static_scene));

    /* Normal 2-row N64 panel is exactly unchanged. ONLY the persistent
     * TNTV4ALL unlock uses the custom 3-row variant and matching shadow. */
    tnt_vita_draw_original_ia8_sprite(
        g_tnt_onep_static_scene, board_x, board_y, cell,
        0, 0x25, 395, 252,
        stage_extra ? g_tnt_onep_stage_shadow_ia8 :
                      g_tnt_onep_shadow_ia8, 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_onep_static_scene, board_x, board_y, cell,
        200 - (397 >> 1), 0x14, 397, 251,
        stage_extra ? g_tnt_onep_stage_panel_ci8 :
                      g_tnt_onep_panel_ci8,
        g_tnt_onep_panel_rgba16,
        0xFF);
    tnt_vita_draw_original_rgba16_sprite_alpha(
        g_tnt_onep_static_scene, board_x, board_y, cell,
        200 - (132 >> 1), 0x104, 132, 15,
        g_tnt_onep_a_accept_b_back_rgba16,
        0xFF);
}

static void tnt_vita_onep_copy_cached_frame(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    const TNTVitaOnePlayerMenu *menu
) {
    uint32_t *dst = (uint32_t *)buffer;
    int step;

    if (menu->entrance_ticks != 0) {
        tnt_vita_menu_render_assembly(
            buffer, board_x, board_y, cell, menu->entrance_ticks);
        return;
    }

    if (menu->fade <= 0.0f) {
        memcpy(
            dst, g_tnt_menu_background_scene,
            sizeof(g_tnt_menu_background_scene));
        return;
    }

    if (menu->fade >= 1.0f) {
        memcpy(dst, g_tnt_onep_static_scene, sizeof(g_tnt_onep_static_scene));
        return;
    }

    memcpy(
        dst, g_tnt_menu_background_scene,
        sizeof(g_tnt_menu_background_scene));

    step = (int)(menu->fade * 9.0f + 0.5f);
    if (step < 1) step = 1;
    if (step > 8) step = 8;

    /* Union of SINGLE panel/shadow/prompt bounds. All source-image work was
     * done once at startup; these nine transition frames only interpolate
     * cached 32-bit pixels. */
    {
        int dx0 = board_x + tnt_floor_div_s64((s64)(0 - 83) * cell, 11);
        int dy0 = board_y + tnt_floor_div_s64((s64)(20 - 36) * cell, 11);
        int dx1 = board_x + tnt_floor_div_s64((s64)(399 - 83) * cell + 10, 11);
        int dy1 = board_y + tnt_floor_div_s64((s64)(289 - 36) * cell + 10, 11);
        int y;

        if (dx0 < 0) dx0 = 0;
        if (dy0 < 0) dy0 = 0;
        if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
        if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;

        for (y = dy0; y < dy1; y++) {
            int x;
            uint32_t *dst_row = dst + y * DISPLAY_STRIDE;
            const uint32_t *bg_row =
                g_tnt_menu_background_scene + y * DISPLAY_STRIDE;
            const uint32_t *fg_row =
                g_tnt_onep_static_scene + y * DISPLAY_STRIDE;

            for (x = dx0; x < dx1; x++) {
                uint32_t bg = bg_row[x];
                uint32_t fg = fg_row[x];
                int br, bgc, bb, fr, fgc, fb;
                int r, g, b;

                if (bg == fg)
                    continue;

                br = (int)(bg & 0xFFU);
                bgc = (int)((bg >> 8) & 0xFFU);
                bb = (int)((bg >> 16) & 0xFFU);
                fr = (int)(fg & 0xFFU);
                fgc = (int)((fg >> 8) & 0xFFU);
                fb = (int)((fg >> 16) & 0xFFU);

                r = br + ((fr - br) * step) / 9;
                g = bgc + ((fgc - bgc) * step) / 9;
                b = bb + ((fb - bb) * step) / 9;
                dst_row[x] = COLOR((u8)r, (u8)g, (u8)b);
            }
        }
    }
}

static u8 tnt_vita_onep_name_count(const TNTVitaOnePlayerMenu *menu) {
    return (u8)(2 + menu->profile_count);
}

static const char *tnt_vita_onep_name_text(const TNTVitaOnePlayerMenu *menu) {
    if (menu->name_entry == 0)
        return "GUEST";
    if (menu->name_entry == 1)
        return "NEW NAME";
    if ((u8)(menu->name_entry - 2) < menu->profile_count)
        return menu->profile_names[menu->name_entry - 2];
    return "GUEST";
}

static void tnt_vita_onep_load_saved_profiles(TNTVitaOnePlayerMenu *menu) {
    int slot;
    menu->profile_count = 0;
    for (slot = 0; slot < TNT_VITA_SRAM_MAX_PLAYERS; slot++) {
        TNTVitaStoredPlayer player;
        u8 i;
        if (TNT_Vita_SramGetPlayer(slot, &player) != 1)
            continue;
        if (menu->profile_count >= 16)
            break;
        /* Original frontend scans slots 0..31 but inserts each node directly
         * after the static list anchor, so later occupied slots appear first. */
        for (i = menu->profile_count; i > 0; i--) {
            memcpy(menu->profile_names[i], menu->profile_names[i - 1], 9);
            menu->profile_pack[i] = menu->profile_pack[i - 1];
            menu->profile_slot[i] = menu->profile_slot[i - 1];
            menu->profile_salt[i][0] = menu->profile_salt[i - 1][0];
            menu->profile_salt[i][1] = menu->profile_salt[i - 1][1];
        }
        memcpy(menu->profile_names[0], player.name, 9);
        menu->profile_pack[0] = 13;
        menu->profile_slot[0] = (u8)slot;
        menu->profile_salt[0][0] = player.salt[0];
        menu->profile_salt[0][1] = player.salt[1];
        menu->profile_count++;
    }
}

static void tnt_vita_onep_begin_name_editor(TNTVitaOnePlayerMenu *menu) {
    if (menu->profile_count >= 16)
        return;

    menu->name_editing = TRUE;
    g_tnt_onep_settled_valid = FALSE;
    g_tnt_onep_cache_kind = 0;
    g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_NONE;
    menu->editor_pos = 0;
    menu->editor_char = 0;
    memset(menu->edit_name, 0, sizeof(menu->edit_name));
    /* func_8007A738 creates the temporary type-11 node with text "A". */
    menu->edit_name[0] = 'A';
    /* func_8007A738 identity salts. Assignment truncates both to u8. */
    menu->edit_salt[0] = (u8)(
        tnt_vita_cycles_to_nsec32(tnt_vita_n64_time_cycles32()) >> 4);
    menu->edit_salt[1] = (u8)(tnt_vita_frontend_rng_next() % 256U);
    menu->name_claimed = FALSE;
}

static void tnt_vita_onep_cancel_name_editor(TNTVitaOnePlayerMenu *menu) {
    /* Original B-at-position-zero deletes the temporary type-11 node and
     * returns the textbox to the static GUEST node. */
    menu->name_editing = FALSE;
    g_tnt_onep_settled_valid = FALSE;
    g_tnt_onep_cache_kind = 0;
    g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_NONE;
    menu->name_entry = 0;
    menu->name_claimed = FALSE;
    menu->editor_pos = 0;
    menu->editor_char = 0;
    menu->edit_salt[0] = 0;
    menu->edit_salt[1] = 0;
    memset(menu->edit_name, 0, sizeof(menu->edit_name));
}

/* Retail NEW NAME secret codes from GUI_Textbox_Render/callback in 055500.c.
 * Recognized codes are consumed as commands: no profile is created and the
 * NAME field returns to GUEST.  HALUCI is handled separately because it enters
 * its own render state rather than modifying ordinary gameplay state. */
static u8 tnt_vita_onep_apply_name_secret(TNTVitaOnePlayerMenu *menu) {
    const char *name = menu->edit_name;
    u8 recognized = FALSE;
    u8 play_ack = FALSE;

    /* The source clears the AI turbo flag on every completed NEW NAME entry,
     * then re-enables it only for AI2EZ4U?. Thus creating any later normal
     * profile also cancels the CPU turbo cheat until it is entered again. */
    D_800E1F50 = FALSE;

    if (strcmp(name, "HALUCI") == 0) {
        /* Retail callback always consumes HALUCI, but only func_800A35EC(1,3)
         * == 2 enters frontend substate 4 / main state 13. This odd 1-in-3
         * gate is intentional source behavior. */
        recognized = TRUE;
        if (func_800A35EC(1, 3) == 2) {
            g_tnt_haluci_requested = TRUE;
            play_ack = TRUE;
        }
    } else if (strcmp(name, "2FAST4U") == 0) {
        g_pieceFallAcceleration = 0.0001f;
        recognized = TRUE;
        play_ack = TRUE;
    } else if (strcmp(name, "TNTV4ALL") == 0) {
        /* TNTV-native reversible content-access override. It never mutates
         * player stats or Wonder progression; only frontend access checks
         * consult this checksum-protected Vita save flag. */
        TNT_Vita_SramSetUnlockAll((u8)!TNT_Vita_SramGetUnlockAll());
        (void)TNT_Vita_SramSave();
        menu->stage = 0;
        recognized = TRUE;
        play_ack = TRUE;
    } else if (strcmp(name, "01DERS") == 0) {
        /* Retail 01DERS clears the global Wonder-line counter and immediately
         * saves; saved player/high-score records are left intact. */
        TNT_Vita_SramSetTotalWonderLines(0);
        tnt_vita_save_to_sram();
        recognized = TRUE;
        play_ack = TRUE;
    } else if (strcmp(name, "1N175R4M") == 0) {
        /* Retail load_from_sram(1): factory-initialize SRAM, including player
         * profiles, high scores, Wonder progress and audio settings. */
        TNT_Vita_SramInitFresh(tnt_vita_n64_time_cycles32());
        (void)TNT_Vita_SramSave();
        tnt_vita_onep_load_saved_profiles(menu);
        recognized = TRUE;
        play_ack = TRUE;
    } else if (strcmp(name, "AI2EZ4U?") == 0) {
        D_800E1F50 = TRUE;
        recognized = TRUE;
        play_ack = TRUE;
    }

    if (!recognized)
        return FALSE;

    /* HALUCI plays the acknowledgement only when its 1-in-3 gate succeeds;
     * the other retail name cheats always acknowledge. */
    if (play_ack) {
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_2);
    }
    tnt_vita_onep_cancel_name_editor(menu);
    return TRUE;
}

static void tnt_vita_onep_commit_name_editor(TNTVitaOnePlayerMenu *menu) {
    TNTVitaStoredPlayer player;
    int slot;
    u8 i;

    memset(&player, 0, sizeof(player));
    memcpy(player.name, menu->edit_name, 9);
    player.salt[0] = menu->edit_salt[0];
    player.salt[1] = menu->edit_salt[1];
    /* Fresh runtime Player.pack is zero; TextList type 13 is separate. */
    player.pack = 0;
    if (TNT_Vita_SramCreatePlayer(&player, &slot) != 0)
        return;

    /* func_8007A078 promotes type 11 -> 13 and immediately save_to_sram(). */
    tnt_vita_save_to_sram();

    /* func_80079F90 inserts the newly saved node immediately after NEW NAME. */
    for (i = menu->profile_count; i > 0; i--) {
        memcpy(menu->profile_names[i], menu->profile_names[i - 1], 9);
        menu->profile_pack[i] = menu->profile_pack[i - 1];
        menu->profile_slot[i] = menu->profile_slot[i - 1];
        menu->profile_salt[i][0] = menu->profile_salt[i - 1][0];
        menu->profile_salt[i][1] = menu->profile_salt[i - 1][1];
    }
    memcpy(menu->profile_names[0], menu->edit_name, 9);
    menu->profile_pack[0] = 13;
    menu->profile_slot[0] = (u8)slot;
    menu->profile_salt[0][0] = menu->edit_salt[0];
    menu->profile_salt[0][1] = menu->edit_salt[1];
    menu->profile_count++;
    menu->name_entry = 2;
    menu->name_claimed = TRUE;
    menu->name_editing = FALSE;
    g_tnt_onep_settled_valid = FALSE;
    g_tnt_onep_cache_kind = 0;
    g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_NONE;
    menu->selected = 1;
}

static void tnt_vita_onep_name_editor_update(
    TNTVitaOnePlayerMenu *menu,
    PlayerVars *pv_ptr
) {
    u32 repeat = pv_ptr->unk28.unk84;
    u32 pressed = pv_ptr->unk24;
    u8 moved = FALSE;

    if (repeat & CONT_RIGHT) {
        if (menu->editor_pos == 8) menu->editor_pos = 7;
        if (menu->editor_char == 14) menu->editor_char = 0;
        else if (menu->editor_char == 29) menu->editor_char = 15;
        else if (menu->editor_char == 44) menu->editor_char = 30;
        else menu->editor_char++;
        moved = TRUE;
    }
    if (repeat & CONT_LEFT) {
        if (menu->editor_pos == 8) menu->editor_pos = 7;
        if (menu->editor_char == 0) menu->editor_char = 14;
        else if (menu->editor_char == 15) menu->editor_char = 29;
        else if (menu->editor_char == 30) menu->editor_char = 44;
        else menu->editor_char--;
        moved = TRUE;
    }
    if (repeat & CONT_UP) {
        if (menu->editor_pos == 8) menu->editor_pos = 7;
        if (menu->editor_char < 15) menu->editor_char += 30;
        else menu->editor_char -= 15;
        moved = TRUE;
    }
    if (repeat & CONT_DOWN) {
        if (menu->editor_pos == 8) menu->editor_pos = 7;
        if (menu->editor_char + 15 >= 45) menu->editor_char -= 30;
        else menu->editor_char += 15;
        moved = TRUE;
    }
    if (moved) {
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        if (menu->editor_pos < 8)
            menu->edit_name[menu->editor_pos] =
                g_tnt_name_editor_chars[menu->editor_char];
    }

    if (pressed & CONT_START) {
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
        menu->editor_char = 44;
        if (menu->editor_pos < 8)
            menu->edit_name[menu->editor_pos] =
                g_tnt_name_editor_chars[menu->editor_char];
    }

    if (pressed & CONT_A) {
        if (menu->editor_char == 43) { /* BACKSPACE */
            if (menu->editor_pos > 0) {
                Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_3);
                menu->edit_name[menu->editor_pos] = 0;
                menu->editor_pos--;
            }
        } else if (menu->editor_char == 44) { /* OK */
            if (menu->editor_pos == 0) {
                Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_3);
                return;
            }
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_2);
            menu->edit_name[menu->editor_pos] = 0;
            if (tnt_vita_onep_apply_name_secret(menu))
                return;
            tnt_vita_onep_commit_name_editor(menu);
            return;
        } else if (menu->edit_name[0] != ' ') {
            if (menu->editor_pos < 8) {
                Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
                menu->editor_pos++;
            }
            if (menu->editor_pos == 8) {
                menu->editor_char = 44;
                return;
            }
        } else {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_3);
        }

        if (menu->name_editing && menu->editor_pos < 8)
            menu->edit_name[menu->editor_pos] =
                g_tnt_name_editor_chars[menu->editor_char];
    }

    if ((pressed & CONT_B) && menu->name_editing) {
        if (menu->editor_pos > 0) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_3);
            menu->edit_name[menu->editor_pos] = 0;
            menu->editor_pos--;
            menu->edit_name[menu->editor_pos] =
                g_tnt_name_editor_chars[menu->editor_char];
        } else {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_5);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_2);
            tnt_vita_onep_cancel_name_editor(menu);
        }
    }
}

static void tnt_vita_onep_enter(
    TNTVitaOnePlayerMenu *menu,
    f32 pulse,
    f32 pulse_rate
) {
    tnt_vita_onep_load_saved_profiles(menu);
    menu->active = TRUE;
    menu->selected = 0;
    menu->entrance_ticks = 31;
    menu->leaving = FALSE;
    menu->opponent = 0;
    menu->game_type = 0;
    if (!TNT_Vita_SramGetUnlockAll())
        menu->stage = 0;
    menu->name_entry = 0;
    menu->name_claimed = FALSE;
    menu->name_editing = FALSE;
    menu->editor_char = 0;
    menu->editor_pos = 0;
    menu->edit_salt[0] = 0;
    menu->edit_salt[1] = 0;
    memset(menu->edit_name, 0, sizeof(menu->edit_name));
    menu->pulse = pulse;
    menu->pulse_rate = pulse_rate;
    menu->fade = 0.0f;
    g_tnt_60hz_last_us = 0;
    g_tnt_60hz_fraction = 0;
}

static int tnt_vita_onep_update(
    TNTVitaOnePlayerMenu *menu,
    PlayerVars *pv_ptr
) {
    u32 retrace_delta = tnt_vita_60hz_delta(TRUE);
    u32 repeat;
    u8 count;

    if (retrace_delta == 0)
        retrace_delta = 1;

    menu->pulse += menu->pulse_rate;
    if (menu->pulse >= 1.0f) {
        menu->pulse = 1.0f;
        menu->pulse_rate *= -1.0f;
    }
    if (menu->pulse <= 0.6f) {
        menu->pulse = 0.6f;
        menu->pulse_rate *= -1.0f;
    }

    if (tnt_vita_menu_assembly_advance(&menu->entrance_ticks))
        return TNT_ONEP_ACTION_NONE;

    if (menu->leaving) {
        menu->fade -= (1.0f / 9.0f) * retrace_delta;
        if (menu->fade <= 0.0f) {
            menu->fade = 0.0f;
            return TNT_ONEP_ACTION_BACK;
        }
        return TNT_ONEP_ACTION_NONE;
    }

    if (menu->fade < 1.0f) {
        menu->fade += (1.0f / 9.0f) * retrace_delta;
        if (menu->fade > 1.0f)
            menu->fade = 1.0f;
    }

    if (menu->name_editing) {
        tnt_vita_onep_name_editor_update(menu, pv_ptr);
        return TNT_ONEP_ACTION_NONE;
    }

    repeat = pv_ptr->unk28.unk84;

    /* The focused NAME textbox consumes all four D-pad directions to walk its
     * circular TextList. A confirms the identity and advances focus. */
    if (menu->selected == 0) {
        count = tnt_vita_onep_name_count(menu);
        if (repeat & (CONT_RIGHT | CONT_DOWN)) {
            menu->name_entry = (u8)((menu->name_entry + 1) % count);
            menu->name_claimed = FALSE;
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_ONEP_ACTION_NONE;
        }
        if (repeat & (CONT_LEFT | CONT_UP)) {
            menu->name_entry = (u8)((menu->name_entry + count - 1) % count);
            menu->name_claimed = FALSE;
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_ONEP_ACTION_NONE;
        }
        if (pv_ptr->unk24 & CONT_A) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
            if (menu->name_entry == 1) {
                tnt_vita_onep_begin_name_editor(menu);
            } else {
                menu->name_claimed = TRUE;
                menu->selected = 1;
            }
            return TNT_ONEP_ACTION_NONE;
        }
        if (pv_ptr->unk24 & CONT_B) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_5);
            menu->leaving = TRUE;
            return TNT_ONEP_ACTION_NONE;
        }
        return TNT_ONEP_ACTION_NONE;
    }

    {
        u8 row_count = TNT_Vita_SramGetUnlockAll() ? 5U : 4U;
        if (menu->selected >= row_count)
            menu->selected = (u8)(row_count - 1U);
        if (repeat & CONT_UP) {
            menu->selected = (u8)((menu->selected + row_count - 1U) % row_count);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_ONEP_ACTION_NONE;
        }
        if (repeat & CONT_DOWN) {
            menu->selected = (u8)((menu->selected + 1U) % row_count);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_ONEP_ACTION_NONE;
        }
    }

    if (pv_ptr->unk24 & CONT_B) {
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_5);
        menu->leaving = TRUE;
        return TNT_ONEP_ACTION_NONE;
    }

    if (menu->selected == 1) {
        if (pv_ptr->unk24 & CONT_LEFT) {
            menu->opponent = (u8)((menu->opponent + 5) % 6);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        }
        if (pv_ptr->unk24 & CONT_RIGHT) {
            menu->opponent = (u8)((menu->opponent + 1) % 6);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        }
    } else if (menu->selected == 2) {
        if (pv_ptr->unk24 & CONT_LEFT) {
            menu->game_type = (u8)((menu->game_type + 2) % 3);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        }
        if (pv_ptr->unk24 & CONT_RIGHT) {
            menu->game_type = (u8)((menu->game_type + 1) % 3);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        }
    } else if (TNT_Vita_SramGetUnlockAll() && menu->selected == 3) {
        if (pv_ptr->unk24 & CONT_LEFT) {
            menu->stage = (u8)((menu->stage + 7U) % 8U);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        }
        if (pv_ptr->unk24 & CONT_RIGHT) {
            menu->stage = (u8)((menu->stage + 1U) % 8U);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
        }
    } else if (menu->selected == (TNT_Vita_SramGetUnlockAll() ? 4U : 3U) &&
               (pv_ptr->unk24 & (CONT_A | CONT_START))) {
        if (menu->name_claimed && menu->name_entry != 1) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
            return TNT_ONEP_ACTION_START_GAME;
        }
    }

    return TNT_ONEP_ACTION_NONE;
}

static void tnt_vita_onep_render_name_editor(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    const TNTVitaOnePlayerMenu *menu,
    u8 global_alpha,
    u8 pulse
) {
    char glyph[2] = { 0, 0 };
    int i;
    int x;

    /* Active D_800D57A4 keeps IMG_SELECTION_BOX around the textbox.  The
     * source GUI modulates the IA8 mask by the current focus color: red stays
     * at 255 while green/blue pulse, producing the characteristic red/pink
     * selector border also visible in the retail ROM. */
    tnt_vita_draw_original_ia8_sprite_tinted(
        buffer, board_x, board_y, cell,
        0xB0, 0x50, 154, 30,
        g_tnt_onep_selection_box_ia8,
        0xFF, pulse, pulse,
        (u8)(((u32)pulse * global_alpha + 127U) / 255U));

    /* Temporary TextList text begins at textbox + (5,8). The insertion slot
     * is pulsed; committed characters remain solid white. */
    x = 0xB0 + 5;
    for (i = 0; i < 8 && menu->edit_name[i] != 0; i++) {
        u8 a;
        u8 c;
        glyph[0] = menu->edit_name[i];
        if (i == menu->editor_pos) {
            c = pulse;
            a = (u8)(((u32)pulse * global_alpha + 127U) / 255U);
        } else {
            c = 0xFF;
            a = global_alpha;
        }
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, x, 0x50 + 8, glyph,
            c, c, c, a);
        x += TNT_Vita_FontB_TextWidth(glyph);
    }

    /* D_800D3DA0 character grid: 15 columns x 3 rows. */
    for (i = 0; i < 45; i++) {
        int col = i % 15;
        int row = i / 15;
        int gx;
        glyph[0] = g_tnt_name_editor_chars[i];
        gx = (63 + col * 18) - (TNT_Vita_FontB_TextWidth(glyph) / 2) + 9;
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, gx, 132 + row * 23, glyph,
            0xFF, 0xFF, 0xFF, global_alpha);
    }

    /* IMG_CURSOR (153), exact source coordinates from func_80091D60. */
    tnt_vita_draw_original_ia8_sprite(
        buffer, board_x, board_y, cell,
        59 + (menu->editor_char % 15) * 18,
        126 + (menu->editor_char / 15) * 23,
        26, 26, g_tnt_onep_cursor_ia8, global_alpha);
}

static void tnt_vita_onep_build_editor_cache(
    int board_x,
    int board_y,
    int cell,
    const TNTVitaOnePlayerMenu *menu
) {
    char glyph[2] = { 0, 0 };
    int i;
    int start_x;
    s32 start_y = TNT_Vita_SramGetUnlockAll() ? 0xE2 : 0xCA;

    /* NEW NAME owns the NAME field and hides OPPONENT/GAME/STAGE.  Everything
     * else on this settled editor page is static: the source panel, all 45
     * keyboard glyphs, and the unselected START label.  Compose those once
     * into the existing ONE PLAYER cache; the ordinary settled page and the
     * editor are mutually exclusive, so no extra full-screen buffer is needed. */
    memcpy(g_tnt_onep_settled_scene, g_tnt_onep_static_scene,
           sizeof(g_tnt_onep_settled_scene));

    for (i = 0; i < 45; i++) {
        int col = i % 15;
        int row = i / 15;
        int gx;
        glyph[0] = g_tnt_name_editor_chars[i];
        gx = (63 + col * 18) - (TNT_Vita_FontB_TextWidth(glyph) / 2) + 9;
        TNT_Vita_FontB_DrawText(
            g_tnt_onep_settled_scene,
            DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, gx, 132 + row * 23, glyph,
            0xFF, 0xFF, 0xFF, 0xFF);
    }

    start_x = 200 - (TNT_Vita_FontA_TextWidth("START") >> 1);
    TNT_Vita_FontA_DrawText(
        g_tnt_onep_settled_scene,
        DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, start_x, start_y, "START",
        0xFF, 0xFF, 0xFF, 0xA0);

    g_tnt_onep_settled_valid = TRUE;
    g_tnt_onep_cache_kind = 2;
    g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_ONEP;
    (void)menu;
}

static void tnt_vita_onep_render_name_editor_dynamic(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    const TNTVitaOnePlayerMenu *menu,
    u8 pulse
) {
    char glyph[2] = { 0, 0 };
    int i;
    int x;

    /* NAME label remains live because it pulses while the editor owns focus. */
    TNT_Vita_FontA_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 0x3C, 0x51, TNTV_ROM_OriginalText(40U),
        0xFF, 0xFF, 0xFF, pulse);

    tnt_vita_draw_original_ia8_sprite_tinted(
        buffer, board_x, board_y, cell,
        0xB0, 0x50, 154, 30,
        g_tnt_onep_selection_box_ia8,
        0xFF, pulse, pulse, pulse);

    x = 0xB0 + 5;
    for (i = 0; i < 8 && menu->edit_name[i] != 0; i++) {
        u8 c = i == menu->editor_pos ? pulse : 0xFF;
        glyph[0] = menu->edit_name[i];
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, x, 0x50 + 8, glyph,
            c, c, c, c);
        x += TNT_Vita_FontB_TextWidth(glyph);
    }

    tnt_vita_draw_original_ia8_sprite(
        buffer, board_x, board_y, cell,
        59 + (menu->editor_char % 15) * 18,
        126 + (menu->editor_char / 15) * 23,
        26, 26, g_tnt_onep_cursor_ia8, 0xFF);
}

static void tnt_vita_onep_render_uncached(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    const TNTVitaOnePlayerMenu *menu
) {
    const char *opponents[6] = {
        TNTV_ROM_OriginalText(43U), TNTV_ROM_OriginalText(44U), TNTV_ROM_OriginalText(45U), TNTV_ROM_OriginalText(46U), "VERY!HARD", TNTV_ROM_OriginalText(47U)
    };
    static const s32 opponent_x[6] = {
        0x12D, 0x122, 0x107, 0x11C, 0xE9, 0x10A
    };
    static const char *game_types[3] = {
        "MARATHON", "SPRINT", "ULTRA"
    };
    static const s32 game_x[3] = { 0xE8, 0x110, 0x114 };
    static const char *stage_names[8] = {
        "MAYAN", "GREEK", "EGYPTIAN", "CELTIC",
        "AFRICAN", "JAPANESE", "RUSSIAN", "FINALE"
    };
    u8 global_alpha;
    u8 pulse;
    u8 base_alpha;
    u8 selected_alpha;
    int start_x;

    tnt_vita_onep_copy_cached_frame(
        buffer, board_x, board_y, cell, menu);

    if (menu->entrance_ticks != 0)
        return;

    global_alpha = (u8)(menu->fade * 255.0f);
    pulse = (u8)(menu->pulse * 255.0f);
    base_alpha = (u8)(((u32)0xA0 * global_alpha + 127U) / 255U);
    selected_alpha = (u8)(((u32)pulse * global_alpha + 127U) / 255U);

    /* NAME is Font A; normal TextList value is Font B. */
    TNT_Vita_FontA_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 0x3C, 0x51, TNTV_ROM_OriginalText(40U),
        0xFF, 0xFF, 0xFF,
        menu->selected == 0 ? selected_alpha : base_alpha);

    if (menu->name_editing) {
        tnt_vita_onep_render_name_editor(
            buffer, board_x, board_y, cell, menu, global_alpha, pulse);
    } else {
        const char *name_text = tnt_vita_onep_name_text(menu);
        /* TNTV's simplified one-player state treats selected==0 as the
         * original GUI_Textbox active-name-selection state (unk5C != 0xFF).
         * In that state the N64 renderer draws IMG_SELECTION_BOX, tinted by
         * the same red/pink focus pulse as the selected text. */
        if (menu->selected == 0) {
            tnt_vita_draw_original_ia8_sprite_tinted(
                buffer, board_x, board_y, cell,
                0xB0, 0x50, 154, 30,
                g_tnt_onep_selection_box_ia8,
                0xFF, pulse, pulse, selected_alpha);
        }
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 0xB0 + 5, 0x50 + 8, name_text,
            0xFF,
            menu->selected == 0 ? pulse : 0xFF,
            menu->selected == 0 ? pulse : 0xFF,
            menu->selected == 0 ? selected_alpha : base_alpha);
    }

    /* OPPONENT and GAME are GUI_NULL while the NEW NAME editor owns focus. */
    if (!menu->name_editing) {
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 0x3A, 0x88, TNTV_ROM_OriginalText(41U),
        0xFF,
        menu->selected == 1 ? pulse : 0xFF,
        menu->selected == 1 ? pulse : 0xFF,
        menu->selected == 1 ? selected_alpha : base_alpha);
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell,
        opponent_x[menu->opponent], 0x88, opponents[menu->opponent],
        0xFF, 0xFF, 0xFF,
        menu->selected == 1 ? selected_alpha : base_alpha);

    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 0x3A, 0xA6, TNTV_ROM_OriginalText(42U),
        0xFF,
        menu->selected == 2 ? pulse : 0xFF,
        menu->selected == 2 ? pulse : 0xFF,
        menu->selected == 2 ? selected_alpha : base_alpha);
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell,
        game_x[menu->game_type], 0xA6, game_types[menu->game_type],
        0xFF, 0xFF, 0xFF,
        menu->selected == 2 ? selected_alpha : base_alpha);
    }

    if (!menu->name_editing && TNT_Vita_SramGetUnlockAll()) {
        int stage_x = 0x12D - TNT_Vita_FontB_TextWidth(stage_names[menu->stage]);
        if (stage_x < 0xB0) stage_x = 0xB0;
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 0x3A, 0xC4, "STAGE:",
            0xFF,
            menu->selected == 3 ? pulse : 0xFF,
            menu->selected == 3 ? pulse : 0xFF,
            menu->selected == 3 ? selected_alpha : base_alpha);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, stage_x, 0xC4, stage_names[menu->stage],
            0xFF, 0xFF, 0xFF,
            menu->selected == 3 ? selected_alpha : base_alpha);
    }

    start_x = 200 - (TNT_Vita_FontA_TextWidth("START") >> 1);
    {
        u8 start_row = TNT_Vita_SramGetUnlockAll() ? 4U : 3U;
        s32 start_y = TNT_Vita_SramGetUnlockAll() ? 0xE2 : 0xCA;
        TNT_Vita_FontA_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, start_x, start_y, "START",
            0xFF,
            menu->selected == start_row ? pulse : 0xFF,
            menu->selected == start_row ? pulse : 0xFF,
            menu->selected == start_row ? selected_alpha : base_alpha);
    }
}

static u32 tnt_vita_onep_settled_cache_key(const TNTVitaOnePlayerMenu *menu) {
    const char *name = tnt_vita_onep_name_text(menu);
    u32 h = 2166136261U;
    while (*name != '\0') {
        h ^= (u8)*name++;
        h *= 16777619U;
    }
    h ^= menu->opponent; h *= 16777619U;
    h ^= menu->game_type; h *= 16777619U;
    h ^= menu->stage; h *= 16777619U;
    h ^= TNT_Vita_SramGetUnlockAll() ? 1U : 0U;
    return h;
}

static void tnt_vita_onep_restore_native_strip(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    int native_y0,
    int native_y1
) {
    uint32_t *dst = (uint32_t *)buffer;
    int dy0 = board_y + tnt_floor_div_s64((s64)(native_y0 - 36) * cell, 11);
    int dy1 = board_y + tnt_floor_div_s64((s64)(native_y1 - 36) * cell + 10, 11);
    int y;
    if (dy0 < 0) dy0 = 0;
    if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;
    for (y = dy0; y < dy1; y++) {
        memcpy(dst + y * DISPLAY_STRIDE,
               g_tnt_onep_static_scene + y * DISPLAY_STRIDE,
               DISPLAY_WIDTH * sizeof(uint32_t));
    }
}

static void tnt_vita_onep_render_selected_row(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    const TNTVitaOnePlayerMenu *menu
) {
    const char *const opponents[6] = {
        TNTV_ROM_OriginalText(43U), TNTV_ROM_OriginalText(44U), TNTV_ROM_OriginalText(45U), TNTV_ROM_OriginalText(46U), "VERY!HARD", TNTV_ROM_OriginalText(47U)
    };
    static const s32 opponent_x[6] = {
        0x12D, 0x122, 0x107, 0x11C, 0xE9, 0x10A
    };
    static const char *const game_types[3] = {
        "MARATHON", "SPRINT", "ULTRA"
    };
    static const s32 game_x[3] = { 0xE8, 0x110, 0x114 };
    static const char *const stage_names[8] = {
        "MAYAN", "GREEK", "EGYPTIAN", "CELTIC",
        "AFRICAN", "JAPANESE", "RUSSIAN", "FINALE"
    };
    u8 pulse = (u8)(menu->pulse * 255.0f);

    if (menu->selected == 0) {
        const char *name_text = tnt_vita_onep_name_text(menu);
        tnt_vita_onep_restore_native_strip(buffer, board_x, board_y, cell, 76, 114);
        TNT_Vita_FontA_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 0x3C, 0x51, TNTV_ROM_OriginalText(40U),
            0xFF, 0xFF, 0xFF, pulse);
        tnt_vita_draw_original_ia8_sprite_tinted(
            buffer, board_x, board_y, cell,
            0xB0, 0x50, 154, 30,
            g_tnt_onep_selection_box_ia8,
            0xFF, pulse, pulse, pulse);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 0xB0 + 5, 0x50 + 8, name_text,
            0xFF, pulse, pulse, pulse);
        return;
    }

    if (menu->selected == 1) {
        tnt_vita_onep_restore_native_strip(buffer, board_x, board_y, cell, 132, 154);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 0x3A, 0x88, TNTV_ROM_OriginalText(41U),
            0xFF, pulse, pulse, pulse);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell,
            opponent_x[menu->opponent], 0x88, opponents[menu->opponent],
            0xFF, 0xFF, 0xFF, pulse);
        return;
    }

    if (menu->selected == 2) {
        tnt_vita_onep_restore_native_strip(buffer, board_x, board_y, cell, 162, 184);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 0x3A, 0xA6, TNTV_ROM_OriginalText(42U),
            0xFF, pulse, pulse, pulse);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell,
            game_x[menu->game_type], 0xA6, game_types[menu->game_type],
            0xFF, 0xFF, 0xFF, pulse);
        return;
    }

    if (TNT_Vita_SramGetUnlockAll() && menu->selected == 3) {
        int stage_x = 0x12D - TNT_Vita_FontB_TextWidth(stage_names[menu->stage]);
        if (stage_x < 0xB0) stage_x = 0xB0;
        tnt_vita_onep_restore_native_strip(buffer, board_x, board_y, cell, 192, 214);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 0x3A, 0xC4, "STAGE:",
            0xFF, pulse, pulse, pulse);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, stage_x, 0xC4, stage_names[menu->stage],
            0xFF, 0xFF, 0xFF, pulse);
        return;
    }

    {
        u8 start_row = TNT_Vita_SramGetUnlockAll() ? 4U : 3U;
        if (menu->selected == start_row) {
            s32 start_y = TNT_Vita_SramGetUnlockAll() ? 0xE2 : 0xCA;
            int start_x = 200 - (TNT_Vita_FontA_TextWidth("START") >> 1);
            tnt_vita_onep_restore_native_strip(
                buffer, board_x, board_y, cell,
                TNT_Vita_SramGetUnlockAll() ? 222 : 198,
                TNT_Vita_SramGetUnlockAll() ? 252 : 228);
            TNT_Vita_FontA_DrawText(
                buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                board_x, board_y, cell, start_x, start_y, "START",
                0xFF, pulse, pulse, pulse);
        }
    }
}

static void tnt_vita_onep_render(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    const TNTVitaOnePlayerMenu *menu
) {
    u32 key;

    /* The cheat can be turned ON or OFF from the NEW NAME editor while
     * SINGLE is already open. Rebuild only the static scene on a flag
     * transition; its cached selected-row images are invalidated too.
     * This also handles toggling without restarting the Vita. */
    if (g_tnt_onep_cached_unlock_flag !=
        (u8)(TNT_Vita_SramGetUnlockAll() ? TRUE : FALSE))
        tnt_vita_onep_build_cache(board_x, board_y, cell);

    /* Preserve the original full renderer during the falling-block entrance,
     * nine-step fades, and NEW NAME editor.  The settled ordinary page is the
     * expensive steady state: all unselected rows are visually static there. */
    if (menu->entrance_ticks != 0 || menu->fade < 1.0f) {
        tnt_vita_onep_render_uncached(buffer, board_x, board_y, cell, menu);
        return;
    }

    if (menu->name_editing) {
        u8 pulse = (u8)(menu->pulse * 255.0f);
        if (!g_tnt_onep_settled_valid ||
            g_tnt_frontend_cache_owner != TNT_FRONTEND_CACHE_ONEP ||
            g_tnt_onep_cache_kind != 2)
            tnt_vita_onep_build_editor_cache(board_x, board_y, cell, menu);
        memcpy(buffer, g_tnt_onep_settled_scene,
               sizeof(g_tnt_onep_settled_scene));
        tnt_vita_onep_render_name_editor_dynamic(
            buffer, board_x, board_y, cell, menu, pulse);
        return;
    }

    key = tnt_vita_onep_settled_cache_key(menu);
    if (!g_tnt_onep_settled_valid ||
        g_tnt_frontend_cache_owner != TNT_FRONTEND_CACHE_ONEP ||
        g_tnt_onep_cache_kind != 1 || key != g_tnt_onep_settled_key) {
        TNTVitaOnePlayerMenu cached = *menu;
        /* Render every row in its ordinary unselected state once.  Selection
         * and pulse are then overlaid live below, so focus changes do not
         * force the expensive page-wide font composition to run again. */
        cached.selected = 0xFF;
        cached.pulse = 1.0f;
        cached.fade = 1.0f;
        cached.name_editing = FALSE;
        tnt_vita_onep_render_uncached(
            g_tnt_onep_settled_scene, board_x, board_y, cell, &cached);
        g_tnt_onep_settled_key = key;
        g_tnt_onep_settled_valid = TRUE;
        g_tnt_onep_cache_kind = 1;
        g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_ONEP;
    }

    memcpy(buffer, g_tnt_onep_settled_scene,
           sizeof(g_tnt_onep_settled_scene));
    tnt_vita_onep_render_selected_row(
        buffer, board_x, board_y, cell, menu);
}

static void tnt_vita_draw_mayan_moon_layer(
    void *buffer, int board_x, int board_y, int cell
) {
    tnt_vita_draw_original_ia8_sprite(
        buffer, board_x, board_y, cell,
        210, 20, 126, 81, g_tnt_mayan_moon_ia8, 0xFF);
}

static void tnt_vita_draw_mayan_foreground_layers(
    void *buffer, int board_x, int board_y, int cell
) {
    tnt_vita_draw_original_ia8_sprite(
        buffer, board_x, board_y, cell,
        87, 30, 102, 204, g_tnt_mayan_playfield_ia8, 0x64);
    tnt_vita_draw_original_ci8_sprite(
        buffer, board_x, board_y, cell,
        0, 0, 400, 300,
        g_tnt_mayan_background_ci8,
        g_tnt_mayan_background_rgba16);
}

static void tnt_vita_draw_mayan_static_scene(
    void *buffer, int board_x, int board_y, int cell
) {
    /* Fallback composition when the live Borg skybox GXM path is unavailable.
     * The source live path inserts asset 201 between these two helpers. */
    tnt_vita_draw_mayan_moon_layer(buffer, board_x, board_y, cell);
    tnt_vita_draw_mayan_foreground_layers(buffer, board_x, board_y, cell);
}

static void tnt_vita_greek_gameplay_reset(void) {
    g_tnt_greek_skybox_angle = 0.0f;
    g_tnt_greek_rain_ticks = 0;
    g_tnt_greek_fire_frame = 0;
    g_tnt_greek_fire_hold = 0;
}

static void tnt_vita_draw_greek_background_layer(
    void *buffer, int board_x, int board_y, int cell
) {
    tnt_vita_draw_original_ci8_sprite(
        buffer, board_x, board_y, cell, 0, 0, 400, 300,
        g_tnt_greek_background_ci8, g_tnt_greek_background_rgba16);
}

static u8 tnt_vita_advance_greek_fire_pot(void) {
    /* D_800D2890: num_intra_frames=1. func_800767C0 advances before
     * drawing, so frame 1 is the first visible frame and each frame then
     * persists for two rendered background updates. */
    if (g_tnt_greek_fire_hold == 0) {
        g_tnt_greek_fire_hold = 1;
        g_tnt_greek_fire_frame = (u8)((g_tnt_greek_fire_frame + 1U) % 14U);
    } else {
        g_tnt_greek_fire_hold--;
    }
    return g_tnt_greek_fire_frame;
}

static void tnt_vita_draw_greek_fire_pot_frame(
    void *buffer, int board_x, int board_y, int cell, u8 frame
) {
    tnt_vita_draw_original_ci8_sprite_alpha(
        buffer, board_x, board_y, cell, 271, 15, 103, 71,
        g_tnt_greek_fire_ci8[frame % 14U],
        g_tnt_greek_fire_rgba16[frame % 14U], 0x7F);
}

static void tnt_vita_egyptian_gameplay_reset(void) {
    g_tnt_egyptian_coal_frame = 0;
    g_tnt_egyptian_coal_hold = 0;
    /* D_800D29C8 starts candle A at image_cnt=2; the first source update
     * advances it to frame 3 before drawing. Candle B starts at frame 0. */
    g_tnt_egyptian_candle_a_frame = 2;
    g_tnt_egyptian_candle_a_hold = 0;
    g_tnt_egyptian_candle_b_frame = 0;
    g_tnt_egyptian_candle_b_hold = 0;
}

static void tnt_vita_celtic_gameplay_reset(void) {
    g_tnt_celtic_sky_ticks = 0;
    g_tnt_celtic_leaf_ticks = 0;
    g_tnt_celtic_lamp_frame = 0;
    g_tnt_celtic_lamp_hold = 0;
}

static void tnt_vita_african_gameplay_reset(void) {
    g_tnt_african_sky_ticks = 0;
    g_tnt_african_smoke_frame = 0;
    g_tnt_african_smoke_hold = 0;
}

static void tnt_vita_japanese_gameplay_reset(void) {
    g_tnt_japanese_skybox_angle = 0.0f;
    g_tnt_japanese_effect_ticks = 0;
    g_tnt_japanese_butterfly_ticks = 0;
}

static void tnt_vita_russian_gameplay_reset(void) {
    u8 i;
    g_tnt_russian_sky_ticks = 0;
    g_tnt_russian_snow_ticks = 0;
    TNT_Vita_RussianGameplayGxmReset();
    for (i = 0; i < TNT_RUSSIAN_CANDLE_INSTANCE_COUNT; i++) {
        g_tnt_russian_candle_frame[i] = g_tnt_russian_candle_start[i];
        g_tnt_russian_candle_hold[i] = 0;
    }
}

static void tnt_vita_finale_gameplay_reset(void) {
    memset(g_tnt_finale_boiler_state, 0, sizeof(g_tnt_finale_boiler_state));
    memset(g_tnt_finale_bulb_state, 0, sizeof(g_tnt_finale_bulb_state));
    g_tnt_finale_gauge_frame = 0;
    g_tnt_finale_gauge_hold = 0;
}

/* Exact single-image branch of AnimParams::func_800767C0.  The Industrial
 * boiler/bulbs use one shared sprite each, with independently authored
 * intra/inter counters to create sparse, staggered flashes. */
static u8 tnt_vita_finale_sparse_step(
    TNTVitaFinaleSparseState *state, u8 num_intra, u8 num_inter
) {
    if ((u16)num_inter < state->inter) {
        if (state->inter > (u16)(num_inter + 1U)) {
            state->inter = 0;
            return FALSE;
        }
        if (num_intra != 0) {
            if (state->intra == 0) {
                state->intra = num_intra;
                state->inter++;
            } else {
                state->intra--;
            }
        } else {
            state->inter++;
        }
        return TRUE;
    }
    state->inter++;
    return FALSE;
}

static u8 tnt_vita_advance_sprite_frame(
    u8 *frame, u8 *hold, u8 frame_count, u8 num_intra_frames
) {
    if (*hold == 0) {
        *hold = num_intra_frames;
        *frame = (u8)((*frame + 1U) % frame_count);
    } else {
        (*hold)--;
    }
    return *frame;
}

static u8 tnt_vita_advance_sprite_frame_ticks(
    u8 *frame, u8 *hold, u8 frame_count, u8 num_intra_frames, u32 ticks
) {
    /* AnimParams::func_800767C0 advances once per N64 60 Hz presentation
     * update.  African GXM can occasionally span two Vita retraces, so replay
     * the authored state machine for every elapsed retrace instead of slowing
     * the smoke to the rendered-frame rate. */
    if (ticks == 0)
        ticks = 1;
    while (ticks--)
        (void)tnt_vita_advance_sprite_frame(
            frame, hold, frame_count, num_intra_frames);
    return *frame;
}

static void tnt_vita_decode_packbits(
    const u8 *src, u32 src_size, u8 *dst, u32 dst_size
) {
    u32 si = 0, di = 0;
    while (si < src_size && di < dst_size) {
        u8 ctl = src[si++];
        if (ctl <= 127U) {
            u32 count = (u32)ctl + 1U;
            while (count-- && si < src_size && di < dst_size)
                dst[di++] = src[si++];
        } else if (ctl != 128U && si < src_size) {
            u32 count = 257U - (u32)ctl;
            u8 value = src[si++];
            while (count-- && di < dst_size)
                dst[di++] = value;
        }
    }
}

static void tnt_vita_draw_celtic_background_layer(
    void *buffer, int board_x, int board_y, int cell
) {
    u8 *decoded = (u8 *)g_tnt_gameplay_base_scene;
    tnt_vita_decode_packbits(
        g_tnt_celtic_background_packbits,
        g_tnt_celtic_background_packbits_size, decoded, 400U * 300U);
    tnt_vita_draw_original_ci8_sprite(
        buffer, board_x, board_y, cell, 0, 0, 400, 300,
        decoded, g_tnt_celtic_background_rgba16);
}

static void tnt_vita_draw_celtic_lamp_frame(
    void *buffer, int board_x, int board_y, int cell, u8 frame
) {
    tnt_vita_draw_original_ci8_sprite(
        buffer, board_x, board_y, cell, 212, 176, 61, 60,
        g_tnt_celtic_lamp_ci8[frame % 4U],
        g_tnt_celtic_lamp_rgba16[frame % 4U]);
}

static void tnt_vita_draw_african_background_layer(
    void *buffer, int board_x, int board_y, int cell
) {
    u8 *decoded = (u8 *)g_tnt_gameplay_base_scene;
    tnt_vita_decode_packbits(
        g_tnt_african_background_packbits,
        g_tnt_african_background_packbits_size, decoded, 400U * 300U);
    tnt_vita_draw_original_ci8_sprite(
        buffer, board_x, board_y, cell, 0, 0, 400, 300,
        decoded, g_tnt_african_background_rgba16);
}

static void tnt_vita_draw_japanese_background_layer(
    void *buffer, int board_x, int board_y, int cell
) {
    u8 *decoded = (u8 *)g_tnt_gameplay_base_scene;
    tnt_vita_decode_packbits(
        g_tnt_japanese_background_packbits,
        g_tnt_japanese_background_packbits_size, decoded, 400U * 300U);
    tnt_vita_draw_original_ci8_sprite(
        buffer, board_x, board_y, cell, 0, 0, 400, 300,
        decoded, g_tnt_japanese_background_rgba16);
}

static void tnt_vita_draw_russian_background_layer(
    void *buffer, int board_x, int board_y, int cell
) {
    u8 *decoded = (u8 *)g_tnt_gameplay_base_scene;
    tnt_vita_decode_packbits(
        g_tnt_russian_background_packbits,
        g_tnt_russian_background_packbits_size, decoded, 400U * 300U);
    tnt_vita_draw_original_ci8_sprite(
        buffer, board_x, board_y, cell, 0, 0, 400, 300,
        decoded, g_tnt_russian_background_rgba16);
}

static void tnt_vita_draw_russian_candles(
    void *buffer, int board_x, int board_y, int cell
) {
    u8 i;
    for (i = 0; i < TNT_RUSSIAN_CANDLE_INSTANCE_COUNT; i++) {
        u8 frame = (u8)(g_tnt_russian_candle_frame[i] % TNT_RUSSIAN_CANDLE_FRAME_COUNT);
        tnt_vita_draw_original_ci8_sprite_alpha(
            buffer, board_x, board_y, cell,
            g_tnt_russian_candle_x[i], g_tnt_russian_candle_y[i], 2, 6,
            g_tnt_russian_candle_ci8[frame], g_tnt_russian_candle_rgba16[frame], 0xFF);
    }
}

static void tnt_vita_draw_finale_background_layer(
    void *buffer, int board_x, int board_y, int cell
) {
    u8 *decoded = (u8 *)g_tnt_gameplay_base_scene;
    tnt_vita_decode_packbits(
        g_tnt_finale_background_packbits,
        g_tnt_finale_background_packbits_size, decoded, 400U * 300U);
    tnt_vita_draw_original_ci8_sprite(
        buffer, board_x, board_y, cell, 0, 0, 400, 300,
        decoded, g_tnt_finale_background_rgba16);
}

static void tnt_vita_draw_finale_effects(
    void *buffer, int board_x, int board_y, int cell, u32 delta
) {
    u8 boiler_visible[TNT_FINALE_BOILER_INSTANCE_COUNT] = {0};
    u8 bulb_visible[TNT_FINALE_BULB_INSTANCE_COUNT] = {0};
    u8 i;
    u32 tick;
    if (delta == 0) delta = 1;
    for (tick = 0; tick < delta; tick++) {
        for (i = 0; i < TNT_FINALE_BOILER_INSTANCE_COUNT; i++)
            boiler_visible[i] = tnt_vita_finale_sparse_step(
                &g_tnt_finale_boiler_state[i],
                g_tnt_finale_boiler_intra[i], g_tnt_finale_boiler_inter[i]);
        for (i = 0; i < TNT_FINALE_BULB_INSTANCE_COUNT; i++)
            bulb_visible[i] = tnt_vita_finale_sparse_step(
                &g_tnt_finale_bulb_state[i],
                g_tnt_finale_bulb_intra[i], g_tnt_finale_bulb_inter[i]);
    }
    /* All three source boiler AnimParams instances share the exact same
     * sprite and coordinates. With opaque CI8 pixels, drawing it repeatedly
     * during overlapping flashes is pixel-for-pixel redundant. Preserve each
     * independent timing state above, but composite the shared image once if
     * any instance is currently visible. */
    {
        u8 boiler_any = FALSE;
        for (i = 0; i < TNT_FINALE_BOILER_INSTANCE_COUNT; i++)
            boiler_any = (u8)(boiler_any || boiler_visible[i]);
        if (boiler_any)
            tnt_vita_restore_native_rect_from_scene(
                buffer, g_tnt_gameplay_base_scene,
                board_x, board_y, cell, 262, 67, 383, 186);
    }
    for (i = 0; i < TNT_FINALE_BULB_INSTANCE_COUNT; i++) {
        if (bulb_visible[i])
            tnt_vita_draw_original_ci8_sprite_alpha(
                buffer, board_x, board_y, cell, 12, g_tnt_finale_bulb_y[i], 21, 21,
                g_tnt_finale_bulb_ci8, g_tnt_finale_bulb_rgba16, 0xFF);
    }
    (void)tnt_vita_advance_sprite_frame_ticks(
        &g_tnt_finale_gauge_frame, &g_tnt_finale_gauge_hold,
        TNT_FINALE_GAUGE_FRAME_COUNT, 2, delta);
    tnt_vita_draw_original_ci8_sprite_alpha(
        buffer, board_x, board_y, cell, 38, 101, 10, 9,
        g_tnt_finale_gauge_ci8[g_tnt_finale_gauge_frame],
        g_tnt_finale_gauge_rgba16[g_tnt_finale_gauge_frame], 0xFF);
}

static void tnt_vita_draw_african_smoke_frame(
    void *buffer, int board_x, int board_y, int cell, u8 frame
) {
    tnt_vita_draw_original_ci8_sprite_alpha(
        buffer, board_x, board_y, cell, 256, 13, 131, 106,
        g_tnt_african_smoke_ci8[frame % TNT_AFRICAN_SMOKE_FRAME_COUNT],
        g_tnt_african_smoke_rgba16[frame % TNT_AFRICAN_SMOKE_FRAME_COUNT],
        0xFF);
}

static void tnt_vita_draw_egyptian_background_layer(
    void *buffer, int board_x, int board_y, int cell
) {
    /* Reuse the otherwise-idle gameplay base cache as a temporary 120 KB
     * CI8 decode buffer. This keeps the ROM-exact art out of the crowded
     * Vita ELF read-only segment without adding another large BSS allocation. */
    u8 *decoded = (u8 *)g_tnt_gameplay_base_scene;
    tnt_vita_decode_packbits(
        g_tnt_egyptian_background_packbits,
        g_tnt_egyptian_background_packbits_size, decoded, 400U * 300U);
    tnt_vita_draw_original_ci8_sprite(
        buffer, board_x, board_y, cell, 0, 0, 400, 300,
        decoded, g_tnt_egyptian_background_rgba16);
}

static void tnt_vita_draw_egyptian_animated_props(
    void *buffer, int board_x, int board_y, int cell
) {
    u8 coal = tnt_vita_advance_sprite_frame(
        &g_tnt_egyptian_coal_frame, &g_tnt_egyptian_coal_hold, 12, 2);
    u8 candle_a = tnt_vita_advance_sprite_frame(
        &g_tnt_egyptian_candle_a_frame, &g_tnt_egyptian_candle_a_hold, 4, 3);
    u8 candle_b = tnt_vita_advance_sprite_frame(
        &g_tnt_egyptian_candle_b_frame, &g_tnt_egyptian_candle_b_hold, 4, 3);

    tnt_vita_draw_original_ci8_sprite(
        buffer, board_x, board_y, cell, 225, 230, 50, 26,
        g_tnt_egyptian_coal_ci8[coal], g_tnt_egyptian_coal_rgba16[coal]);
    tnt_vita_draw_original_ci8_sprite(
        buffer, board_x, board_y, cell, 51, 252, 22, 19,
        g_tnt_egyptian_candle_a_ci8[candle_a],
        g_tnt_egyptian_candle_a_rgba16[candle_a]);
    tnt_vita_draw_original_ci8_sprite(
        buffer, board_x, board_y, cell, 215, 256, 22, 19,
        g_tnt_egyptian_candle_b_ci8[candle_b],
        g_tnt_egyptian_candle_b_rgba16[candle_b]);
}

static void tnt_vita_prepare_gameplay_background(
    u8 screen, int board_x, int board_y, int cell
) {
    ++g_tnt117_background_generation;
    clear_screen(g_tnt_gameplay_base_scene, COLOR(20, 20, 40));
    clear_screen(g_tnt_static_scene, COLOR(20, 20, 40));
    if (screen == 1) {
        /* Greek's live order is skybox -> 400x300 foreground. The base cache
         * intentionally remains clear so GXM can insert scene 1399 first. */
        tnt_vita_draw_greek_background_layer(
            g_tnt_static_scene, board_x, board_y, cell);
        tnt_vita_greek_gameplay_reset();
    } else if (screen == 2) {
        /* Egyptian gameplay has no Borg skybox/weather pass: one static
         * 400x300 background plus coal/candle sprite animations. */
        tnt_vita_draw_egyptian_background_layer(
            g_tnt_static_scene, board_x, board_y, cell);
        tnt_vita_egyptian_gameplay_reset();
    } else if (screen == 3) {
        /* Celtic live order is sky 487 -> foreground -> name/lamp -> leaves
         * 1335. Keep a foreground-only fallback cache for GXM failure. */
        tnt_vita_draw_celtic_background_layer(
            g_tnt_static_scene, board_x, board_y, cell);
        tnt_vita_celtic_gameplay_reset();
        clear_screen(g_tnt_gameplay_base_scene, COLOR(20, 20, 40));
    } else if (screen == 4) {
        /* African live order: animated sky 567 -> foreground -> name ->
         * D_800D2C38 volcano smoke. */
        tnt_vita_draw_african_background_layer(
            g_tnt_static_scene, board_x, board_y, cell);
        tnt_vita_african_gameplay_reset();
        clear_screen(g_tnt_gameplay_base_scene, COLOR(20, 20, 40));
    } else if (screen == 5) {
        /* Japanese live order: scene 770 sky -> foreground -> name -> water
         * and sparkles -> fresh-depth butterfly scene 1360/animation 1361. */
        tnt_vita_draw_japanese_background_layer(
            g_tnt_static_scene, board_x, board_y, cell);
        tnt_vita_japanese_gameplay_reset();
        clear_screen(g_tnt_gameplay_base_scene, COLOR(20, 20, 40));
    } else if (screen == 6) {
        /* Russian live order: animated sky scene 901/902 -> keyed foreground
         * -> name -> thirteen candle flames -> fresh-depth snow 1370/1371. */
        tnt_vita_draw_russian_background_layer(
            g_tnt_static_scene, board_x, board_y, cell);
        tnt_vita_russian_gameplay_reset();
        clear_screen(g_tnt_gameplay_base_scene, COLOR(20, 20, 40));
    } else if (screen == 7) {
        /* Screen 7 is the post-500,000-line FINALE / bonus Industrial board:
         * static IMG 125/126 plus boiler, bulb and gauge AnimParams sprites.
         * The large boiler image is invariant and always appears at the same
         * coordinates, so cache a background+boiler version once. Live flash
         * frames restore only that rectangle instead of rerasterizing ~48K
         * scaled destination pixels. */
        tnt_vita_draw_finale_background_layer(
            g_tnt_static_scene, board_x, board_y, cell);
        memcpy(g_tnt_gameplay_base_scene, g_tnt_static_scene,
               sizeof(g_tnt_gameplay_base_scene));
        tnt_vita_draw_original_ci8_sprite_alpha(
            g_tnt_gameplay_base_scene, board_x, board_y, cell,
            262, 67, 121, 119,
            g_tnt_finale_boiler_ci8, g_tnt_finale_boiler_rgba16, 0xFF);
        tnt_vita_finale_gameplay_reset();
    } else {
        tnt_vita_draw_mayan_moon_layer(
            g_tnt_gameplay_base_scene, board_x, board_y, cell);
        tnt_vita_draw_mayan_static_scene(
            g_tnt_static_scene, board_x, board_y, cell);
    }
}



/* Exact widths produced by Font_Init46Char for IMG_FONT_C. */


static int tnt_vita_font_c_text_width_native(const char *text) {
    int width = 0;
    while (*text != '\0') {
        unsigned c = (unsigned char)*text++;
        if (c >= 32 && c < 96)
            width += g_tnt_font_c_char_widths[g_tnt_font_c_map[c - 32]];
    }
    return width;
}

static void tnt_vita_draw_font_c_text(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    int native_x,
    int native_y,
    const char *text,
    u8 red,
    u8 green,
    u8 blue,
    u8 global_alpha
) {
    uint32_t *dst = (uint32_t *)buffer;
    int pen_x = native_x;
    u8 r_lut[16], g_lut[16], b_lut[16], a_lut[16];
    int dy0_map[12], dy1_map[12];
    int level, gy;

    if (g_tnt_opp60_tracking && text && global_alpha != 0) {
        int width = tnt_vita_font_c_text_width_native(text);
        int x0 = board_x + tnt_floor_div_s64(
            (s64)(native_x - 83) * cell, 11) - 1;
        int x1 = board_x + tnt_floor_div_s64(
            (s64)(native_x + width - 83) * cell + 10, 11) + 1;
        int y0 = board_y + tnt_floor_div_s64(
            (s64)(native_y - 36) * cell, 11) - 1;
        int y1 = board_y + tnt_floor_div_s64(
            (s64)(native_y + 12 - 36) * cell + 10, 11) + 1;
        tnt_vita_opp60_mark(buffer, x0, y0, x1, y1);
    }

    /* Font C is the long-form Wonder story font.  IA8 has only sixteen
     * intensity/alpha levels, and every glyph in one line shares the same 12
     * destination Y rows.  Cache both here so scrolling story text does not
     * repeat thousands of integer /255 operations and signed 64-bit divisions
     * every frame.  This is the same exact-value optimization already used by
     * the Font A/B renderer. */
    for (level = 0; level < 16; level++) {
        u8 v = (u8)(level * 17U);
        r_lut[level] = (u8)(((u32)red * v + 127U) / 255U);
        g_lut[level] = (u8)(((u32)green * v + 127U) / 255U);
        b_lut[level] = (u8)(((u32)blue * v + 127U) / 255U);
        a_lut[level] = (u8)(((u32)v * global_alpha + 127U) / 255U);
    }
    for (gy = 0; gy < 12; gy++) {
        int native_py = native_y + gy;
        int dy0 = board_y + tnt_floor_div_s64(
            (s64)(native_py - 36) * cell, 11);
        int dy1 = board_y + tnt_floor_div_s64(
            (s64)(native_py + 1 - 36) * cell + 10, 11);
        if (dy0 < 0) dy0 = 0;
        if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;
        dy0_map[gy] = dy0;
        dy1_map[gy] = dy1;
    }

    while (*text != '\0') {
        unsigned c = (unsigned char)*text++;
        u8 glyph;
        int glyph_w;
        int dx0_map[13], dx1_map[13];
        int gx;

        if (c < 32 || c >= 96)
            continue;
        glyph = g_tnt_font_c_map[c - 32];
        glyph_w = g_tnt_font_c_char_widths[glyph];

        /* Destination X bounds depend only on the glyph column, not its row. */
        for (gx = 0; gx < glyph_w; gx++) {
            int native_px = pen_x + gx;
            int dx0 = board_x + tnt_floor_div_s64(
                (s64)(native_px - 83) * cell, 11);
            int dx1 = board_x + tnt_floor_div_s64(
                (s64)(native_px + 1 - 83) * cell + 10, 11);
            if (dx0 < 0) dx0 = 0;
            if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
            dx0_map[gx] = dx0;
            dx1_map[gx] = dx1;
        }

        for (gy = 0; gy < 12; gy++) {
            int dy0 = dy0_map[gy], dy1 = dy1_map[gy];
            if (dy1 <= 0 || dy0 >= DISPLAY_HEIGHT || dy0 >= dy1)
                continue;
            for (gx = 0; gx < glyph_w; gx++) {
                u8 ia = g_tnt_font_c_ia8[
                    (int)glyph * (13 * 12) + gy * 13 + gx];
                u8 alpha = a_lut[ia & 0x0F];
                int dx0 = dx0_map[gx], dx1 = dx1_map[gx];
                int y;
                if (alpha == 0 || dx1 <= 0 || dx0 >= DISPLAY_WIDTH || dx0 >= dx1)
                    continue;
                for (y = dy0; y < dy1; y++) {
                    int x;
                    for (x = dx0; x < dx1; x++) {
                        uint32_t *pixel = &dst[y * DISPLAY_STRIDE + x];
                        *pixel = tnt_alpha_blend_rgb(
                            *pixel,
                            r_lut[ia >> 4], g_lut[ia >> 4], b_lut[ia >> 4],
                            alpha);
                    }
                }
            }
        }

        pen_x += glyph_w;
    }
}


/* TEST115A: render the unchanged Mayan name glyphs against ordinary RAM.
 * Keep full stride so the existing font painter needs only a Y translation.
 * All overlapping alpha writes happen in their original order. */
static uint32_t s_tnt115_name_scratch[DISPLAY_STRIDE * 32];
static void tnt115_draw_mayan_name(
    void *buffer, int board_x, int board_y, int cell, const char *text
) {
    const int width = tnt_vita_font_c_text_width_native(text);
    int x0, x1, y0, y1, y;
    if (cell != TNT_PRESENT_CELL || board_x != TNT_PRESENT_BOARD_X ||
        board_y != TNT_PRESENT_BOARD_Y || width <= 0 || width > 104)
        goto fallback;
    x0 = board_x + tnt_floor_div_s64((s64)(290 - 83) * cell, 11);
    x1 = board_x + tnt_floor_div_s64((s64)(290 + width - 83) * cell + 10, 11);
    y0 = board_y + tnt_floor_div_s64((s64)(202 - 36) * cell, 11);
    y1 = board_y + tnt_floor_div_s64((s64)(202 + 12 - 36) * cell + 10, 11);
    if (x0 < 0 || x1 > DISPLAY_WIDTH || x1 <= x0 ||
        y0 < 0 || y1 > DISPLAY_HEIGHT || y1 <= y0 || y1 - y0 > 32)
        goto fallback;
    for (y = y0; y < y1; y++)
        memcpy(s_tnt115_name_scratch + (y-y0)*DISPLAY_STRIDE + x0,
               (uint32_t *)buffer + y*DISPLAY_STRIDE + x0,
               (size_t)(x1-x0)*sizeof(uint32_t));
    tnt_vita_draw_font_c_text(s_tnt115_name_scratch,
        board_x, board_y-y0, cell, 290, 202, text, 255, 255, 255, 255);
    for (y = y0; y < y1; y++)
        memcpy((uint32_t *)buffer + y*DISPLAY_STRIDE + x0,
               s_tnt115_name_scratch + (y-y0)*DISPLAY_STRIDE + x0,
               (size_t)(x1-x0)*sizeof(uint32_t));
    return;
fallback:
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        290, 202, text, 255, 255, 255, 255);
}

static s16 tnt_vita_credits_sins(u16 angle) {
    u16 a = (u16)(angle >> 4);
    s16 value;
    if (a & 0x400U)
        value = g_tnt_credits_sintable[0x3FFU - (a & 0x3FFU)];
    else
        value = g_tnt_credits_sintable[a & 0x3FFU];
    return (a & 0x800U) ? (s16)-value : value;
}

static s32 tnt_vita_credits_highmul(s32 a, s32 b) {
    return (s32)(((s64)a * (s64)b) >> 32);
}

static void tnt_vita_credits_prepare_effect(void) {
    int y;
    int spot;
    if (g_tnt_credits_effect_ready)
        return;

    /* Credits_Init loads IMG_MAIN_BACKGROUND into its 400x300 GUI buffer
     * before the 200x150 Haluci effect and EndScroller text are drawn. */
    for (y = 0; y < 300; y++) {
        int x;
        for (x = 0; x < 400; x++) {
            u16 rgba = g_tnt_main_menu_background_rgba16[
                g_tnt_main_menu_background_ci8[y * 400 + x]];
            u8 r5 = (u8)((rgba >> 11) & 0x1F);
            u8 g5 = (u8)((rgba >> 6) & 0x1F);
            u8 b5 = (u8)((rgba >> 1) & 0x1F);
            g_tnt_credits_native_base[y * 400 + x] = COLOR(
                (r5 << 3) | (r5 >> 2),
                (g5 << 3) | (g5 >> 2),
                (b5 << 3) | (b5 >> 2));
        }
    }

    for (y = 0; y < 150; y++) {
        int x;
        for (x = 0; x < 200; x++) {
            int i = y * 200 + x;
            if (y < 149 && x < 199) {
                g_tnt_credits_gradients[i].y = (s8)(
                    (s32)g_tnt_credits_height_i8[i] -
                    (s32)g_tnt_credits_height_i8[i + 200]);
                g_tnt_credits_gradients[i].x = (s8)(
                    (s32)g_tnt_credits_height_i8[i] -
                    (s32)g_tnt_credits_height_i8[i + 1]);
            } else {
                g_tnt_credits_gradients[i].y = 0;
                g_tnt_credits_gradients[i].x = 0;
            }
        }
    }

    /* func_80075480: preblend every background palette entry with every
     * spotlight palette entry, then quantize through the N64 RGBA5551 path. */
    for (spot = 0; spot < 256; spot++) {
        int base;
        u32 sr = g_tnt_credits_spotlight_rgb[spot * 3 + 0];
        u32 sg = g_tnt_credits_spotlight_rgb[spot * 3 + 1];
        u32 sb = g_tnt_credits_spotlight_rgb[spot * 3 + 2];
        for (base = 0; base < 256; base++) {
            u32 br = g_tnt_credits_background_rgb[base * 3 + 0];
            u32 bg = g_tnt_credits_background_rgb[base * 3 + 1];
            u32 bb = g_tnt_credits_background_rgb[base * 3 + 2];
            u32 r = sr * (255U - br) / 255U + br;
            u32 g = sg * (255U - bg) / 255U + bg;
            u32 b = sb * (255U - bb) / 255U + bb;
            u8 r5 = (u8)(r >> 3);
            u8 g5 = (u8)(g >> 3);
            u8 b5 = (u8)(b >> 3);
            g_tnt_credits_blend_rgba[(spot << 8) | base] = COLOR(
                (r5 << 3) | (r5 >> 2),
                (g5 << 3) | (g5 >> 2),
                (b5 << 3) | (b5 >> 2));
        }
    }
    g_tnt_credits_effect_ready = TRUE;
}

static void tnt_vita_credits_compose_background(const TNTVitaCredits *credits) {
    s32 a = (s32)tnt_vita_credits_sins(credits->angle_a) << 7;
    s32 b = (s32)tnt_vita_credits_sins((u16)(credits->angle_b + 0x4000U)) << 7;
    s32 c = ((s32)tnt_vita_credits_sins(credits->angle_c) << 7) + 0xF00000;
    s32 lx0, ly0, rx0, ry0, lx1, ly1, rx1, ry1;
    const s32 hstep = (0x10000 / 150) << 16;
    const s32 wstep = (0x10000 / 200) << 16;
    s32 ldx, ldy, rdx, rdy;
    int y;

    a += b >> 1;
    lx0 = a; ly0 = b; rx0 = a + c; ry0 = b;
    lx1 = a; ly1 = b + c; rx1 = (a + c) >> 1; ry1 = b + (c / 2);
    ldx = tnt_vita_credits_highmul(lx1 - lx0, hstep);
    ldy = tnt_vita_credits_highmul(ly1 - ly0, hstep);
    rdx = tnt_vita_credits_highmul(rx1 - rx0, hstep);
    rdy = tnt_vita_credits_highmul(ry1 - ry0, hstep);

    for (y = 0; y < 150; y++) {
        s32 dx = tnt_vita_credits_highmul(rx0 - lx0, wstep);
        s32 dy = tnt_vita_credits_highmul(ry0 - ly0, wstep);
        s32 tx = lx0, ty = ly0;
        int x;
        for (x = 0; x < 200; x++) {
            int i = y * 200 + x;
            s32 sx = (tx >> 16) + g_tnt_credits_gradients[i].x;
            s32 sy = (ty >> 16) + g_tnt_credits_gradients[i].y;
            u8 spot;
            u8 base = g_tnt_credits_background_ci8[i];
            if ((sx >> 8) != 0 || (sy >> 8) != 0)
                spot = g_tnt_credits_spotlight_ci8[0];
            else
                spot = g_tnt_credits_spotlight_ci8[
                    ((u32)(sx >> 1) << 7) | (u32)(sy >> 1)];
            g_tnt_credits_frame[i] =
                g_tnt_credits_blend_rgba[((u32)spot << 8) | base];
            tx += dx; ty += dy;
        }
        lx0 += ldx; ly0 += ldy; rx0 += rdx; ry0 += rdy;
    }
}

static void tnt_vita_credits_draw_native_line(
    int native_x, s32 native_y_q16, const char *text, int font_b
) {
    int pen_x = native_x;
    const u8 *image = font_b ? g_tnt_font_b_ia8 : g_tnt_font_c_ia8;
    const u8 *widths = font_b ? g_tnt_font_b_char_widths : g_tnt_font_c_char_widths;
    int glyph_stride = font_b ? 16 : 13;
    int glyph_height = font_b ? 14 : 12;
    int base_y = tnt_floor_div_s64((s64)native_y_q16, 65536);
    u32 frac = (u32)((s64)native_y_q16 - ((s64)base_y << 16));

    while (*text != '\0') {
        unsigned c = (unsigned char)*text++;
        u8 glyph;
        int glyph_w;
        int gy;
        if (c >= 'a' && c <= 'z') c -= 32;
        if (c < 32 || c >= 96) continue;
        glyph = g_tnt_font_c_map[c - 32];
        glyph_w = widths[glyph];
        for (gy = 0; gy < glyph_height; gy++) {
            int py0 = base_y + gy;
            int py1 = py0 + 1;
            int gx;
            for (gx = 0; gx < glyph_w; gx++) {
                int px = pen_x + gx;
                u8 ia;
                u8 intensity;
                u8 tex_alpha;
                u8 a0, a1;
                if (px < 0 || px >= 400) continue;
                ia = image[(int)glyph * glyph_stride * glyph_height +
                           gy * glyph_stride + gx];
                tex_alpha = (u8)((ia & 0x0FU) * 17U);
                if (tex_alpha == 0) continue;
                intensity = (u8)(((ia >> 4) & 0x0FU) * 17U);
                a0 = (u8)(((u32)tex_alpha * (65536U - frac) + 32768U) >> 16);
                a1 = (u8)(((u32)tex_alpha * frac + 32768U) >> 16);
                if (a0 && py0 >= 0 && py0 < 300) {
                    u32 *pixel = &g_tnt_credits_native_frame[py0 * 400 + px];
                    *pixel = tnt_alpha_blend_rgb(
                        *pixel, intensity, intensity, intensity, a0);
                }
                if (a1 && py1 >= 0 && py1 < 300) {
                    u32 *pixel = &g_tnt_credits_native_frame[py1 * 400 + px];
                    *pixel = tnt_alpha_blend_rgb(
                        *pixel, intensity, intensity, intensity, a1);
                }
            }
        }
        pen_x += glyph_w;
    }
}

static void tnt_vita_credits_build_native_frame(const TNTVitaCredits *credits) {
    s32 y_q16 = (300 << 16) - (s32)credits->scroll_q16;
    u16 i;
    int row;

    memcpy(g_tnt_credits_native_frame, g_tnt_credits_native_base,
           sizeof(g_tnt_credits_native_frame));
    tnt_vita_credits_compose_background(credits);
    for (row = 0; row < 150; row++) {
        memcpy(&g_tnt_credits_native_frame[(70 + row) * 400 + 103],
               &g_tnt_credits_frame[row * 200], 200 * sizeof(u32));
    }

    for (i = 0; i < g_tnt_credits_line_count; i++) {
        const char *line = g_tnt_credits_lines[i];
        int is_b = g_tnt_credits_font_is_b[i] != 0;
        int h = is_b ? 16 : 14;
        if (y_q16 < (300 << 16) && y_q16 + (h << 16) >= 0) {
            int width = is_b ? TNT_Vita_FontB_TextWidth(line)
                             : tnt_vita_font_c_text_width_native(line);
            tnt_vita_credits_draw_native_line(
                (400 - width) >> 1, y_q16, line, is_b);
        }
        y_q16 += h << 16;
    }
}

static void tnt_vita_credits_enter(TNTVitaCredits *credits) {
    u32 total = 0;
    u16 i;
    memset(credits, 0, sizeof(*credits));
    credits->active = TRUE;
    for (i = 0; i < g_tnt_credits_line_count; i++)
        total += g_tnt_credits_font_is_b[i] ? 16U : 14U;
    credits->total_height = total;
    credits->start_us = sceKernelGetProcessTimeWide();
    credits->scroll_q16 = 0;
    tnt_vita_credits_prepare_effect();
}

static int tnt_vita_credits_update(
    TNTVitaCredits *credits, PlayerVars *pv_ptr
) {
    u64 now_us = sceKernelGetProcessTimeWide();
    u64 elapsed_us = now_us - credits->start_us;
    u64 scroll_q16 = (elapsed_us * 60ULL * 65536ULL) / 1000000ULL;
    u32 whole_ticks;

    if (pv_ptr->unk24 & CONT_A)
        return 1;
    if (scroll_q16 > 0xFFFFFFFFULL)
        scroll_q16 = 0xFFFFFFFFULL;
    credits->scroll_q16 = (u32)scroll_q16;
    whole_ticks = credits->scroll_q16 >> 16;
    credits->angle_a = (u16)(whole_ticks * 258U);
    credits->angle_b = (u16)(whole_ticks * 106U);
    credits->angle_c = (u16)(whole_ticks * 34U);
    return credits->scroll_q16 >
        ((credits->total_height + 300U) << 16);
}

static void tnt_vita_credits_render(
    void *buffer, SceGxmColorSurface *surface, SceGxmSyncObject *sync,
    int board_x, int board_y, int cell, const TNTVitaCredits *credits
) {
    int rendered = -1;
    tnt_vita_credits_build_native_frame(credits);

    /* Only the pillarbox area needs a CPU clear. A single memset is still far
     * cheaper than the old per-output-pixel text/filtering path. */
    memset(buffer, 0, DISPLAY_STRIDE * DISPLAY_HEIGHT * sizeof(u32));
    if (g_tnt_milestone_gxm_ready) {
        rendered = TNT_Vita_CreditsGxmRender(
            surface, sync, board_x, board_y, cell,
            g_tnt_credits_native_frame);
    }

    /* Safety fallback if the GXM helper is unavailable: point-scale the native
     * frame. This is not the intended path but keeps Credits functional. */
    if (rendered < 0) {
        u32 *dst = (u32 *)buffer;
        int y;
        for (y = 0; y < DISPLAY_HEIGHT; y++) {
            int sy = 36 + tnt_floor_div_s64((s64)(y - board_y) * 11, cell);
            int x;
            if (sy < 0 || sy >= 300) continue;
            for (x = 0; x < DISPLAY_WIDTH; x++) {
                int sx = 83 + tnt_floor_div_s64((s64)(x - board_x) * 11, cell);
                if (sx >= 0 && sx < 400)
                    dst[y * DISPLAY_STRIDE + x] =
                        g_tnt_credits_native_frame[sy * 400 + sx];
            }
        }
    }
}

static void draw_original_font_c_line_count(
    void *buffer, int board_x, int board_y, int cell, u32 value
) {
    char text[16];
    snprintf(text, sizeof(text), "%u", (unsigned)value);
    tnt_vita_draw_font_c_text(
        buffer, board_x, board_y, cell,
        295, 183, text, 0xFF, 0xFF, 0xFF, 0xFF);
}

static void tnt_vita_format_original_time(u32 ticks, char *text, size_t size) {
    u32 time = ticks / 60U;

    /* Exact useful range/format branches from displayTimeFormatted_XY_RGBA. */
    if (time >= 36000U)
        time = 35999U;

    if (time < 600U) {
        snprintf(text, size, "%u:%u%u",
            (unsigned)(((time / 60U) % 60U) % 10U),
            (unsigned)((time % 60U) / 10U),
            (unsigned)((time % 60U) % 10U));
    } else if (time < 3601U) {
        snprintf(text, size, "%u%u:%u%u",
            (unsigned)(((time / 60U) % 60U) / 10U),
            (unsigned)(((time / 60U) % 60U) % 10U),
            (unsigned)((time % 60U) / 10U),
            (unsigned)((time % 60U) % 10U));
    } else {
        snprintf(text, size, "%u:%u%u:%u%u",
            (unsigned)(((time / 60U) / 60U) % 10U),
            (unsigned)(((time / 60U) % 60U) / 10U),
            (unsigned)(((time / 60U) % 60U) % 10U),
            (unsigned)((time % 60U) / 10U),
            (unsigned)((time % 60U) % 10U));
    }
}

static void tnt_vita_draw_original_time(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    int native_right_x,
    int native_y,
    u32 ticks
) {
    char text[20];
    int x;

    tnt_vita_format_original_time(ticks, text, sizeof(text));
    x = native_right_x - tnt_vita_font_c_text_width_native(text);
    tnt_vita_draw_font_c_text(
        buffer, board_x, board_y, cell,
        x, native_y, text, 0xFF, 0xFF, 0xFF, 0xFF);
}

/* Test89E Ultra+Hot Potato: Font C's goal never changes; the elapsed
 * display changes once per 60Hz second. Bake the exact goal sprite into the
 * static 3P scene ONCE, and composite the clock on a tiny background-backed
 * tile only when its visible second changes. This replaces 4-5 expensive
 * scaled alpha-glyph draws PER FRAME with ~10k-pixel framebuffer memcpy.
 * The 3P NONE/Directed and every 1P/2P clock retain their original path. */
typedef struct {
    u32 *tile;
    int x,y,w,h;
    u32 drawn_second;
    u8 active, valid;
} TNT3PUltraHpClockCache;
static TNT3PUltraHpClockCache g_tnt_3p_ultra_hp_clock;

static void __attribute__((optimize("Os")))
tnt_vita_3p_ultra_hp_clock_free(void) {
    free(g_tnt_3p_ultra_hp_clock.tile);
    memset(&g_tnt_3p_ultra_hp_clock, 0,
        sizeof(g_tnt_3p_ultra_hp_clock));
}

static void __attribute__((optimize("Os")))
tnt_vita_3p_ultra_hp_clock_prepare(
    int board_x, int board_y, int cell, u32 goal
) {
    TNT3PUltraHpClockCache *clock = &g_tnt_3p_ultra_hp_clock;
    char goal_text[12];
    int x0,x1,y0,y1;
    tnt_vita_3p_ultra_hp_clock_free();

    /* Draw the goal at precisely the original Ultra HUD source coordinate. */
    snprintf(goal_text, sizeof(goal_text), "%u",
        (unsigned)goal);
    tnt_vita_draw_font_c_text(g_tnt_static_scene,
        board_x, board_y, cell, 188, 265, goal_text,
        0xFF, 0xFF, 0xFF, 0xFF);
    clock->active = TRUE;

    /* Supports all original formatted times through 9:59:59. The tile
     * includes the goal's bottom pixel, source-time glyph x~160..217
     * (right-aligned), and 12-pixel-tall source glyph at y278..289. */
    x0=tnt_vita_source_x(145, board_x, cell)-4;
    x1=tnt_vita_source_x(225, board_x, cell)+4;
    y0=tnt_vita_source_y(275, board_y, cell)-3;
    y1=tnt_vita_source_y(294, board_y, cell)+4;
    if (x0 < 0 || y0 < 0 || x1 > DISPLAY_WIDTH ||
        y1 > DISPLAY_HEIGHT || x0 >= x1 || y0 >= y1)
        return;
    clock->x=x0; clock->y=y0; clock->w=x1-x0; clock->h=y1-y0;
    clock->tile=malloc((size_t)clock->w * clock->h * sizeof(u32));
    /* Correct fallback remains enabled if an allocation fails. */
}

static void __attribute__((optimize("Os")))
tnt_vita_3p_ultra_hp_clock_render(
    void *dest, int board_x, int board_y, int cell, u32 elapsed_ticks
) {
    TNT3PUltraHpClockCache *clock = &g_tnt_3p_ultra_hp_clock;
    u32 second = elapsed_ticks / 60U;
    int row;
    if (clock->active && clock->tile != NULL) {
        if (!clock->valid || clock->drawn_second != second) {
            /* Scratch is dormant during 3P HP gameplay, never the static
             * tally cache. Composite from the exact finished static
             * image so cached pixels are indistinguishable from direct
             * Font C drawing. */
            for (row=0; row<clock->h; row++) {
                size_t offset=(size_t)(clock->y+row) *
                    DISPLAY_STRIDE + clock->x;
                memcpy(g_tnt_gameplay_base_scene+offset,
                    g_tnt_static_scene+offset,
                    (size_t)clock->w * sizeof(u32));
            }
            tnt_vita_draw_original_time(g_tnt_gameplay_base_scene,
                board_x, board_y, cell, 217, 278, elapsed_ticks);
            for (row=0; row<clock->h; row++)
                memcpy(clock->tile+(size_t)row*clock->w,
                    g_tnt_gameplay_base_scene+
                        (size_t)(clock->y+row)*DISPLAY_STRIDE+clock->x,
                    (size_t)clock->w * sizeof(u32));
            clock->drawn_second=second;
            clock->valid=TRUE;
        }
        for (row=0; row<clock->h; row++)
            memcpy((u32 *)dest+(size_t)(clock->y+row)*DISPLAY_STRIDE+clock->x,
                clock->tile+(size_t)row*clock->w,
                (size_t)clock->w * sizeof(u32));
    } else {
        tnt_vita_draw_original_time(dest, board_x, board_y,
            cell, 217, 278, elapsed_ticks);
    }
}

static void tnt_vita_draw_original_time_font_b(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    int native_right_x,
    int native_y,
    u32 ticks
) {
    char text[20];
    int x;

    tnt_vita_format_original_time(ticks, text, sizeof(text));
    x = native_right_x - TNT_Vita_FontB_TextWidth(text);
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, native_y,
        text, 0xFF, 0xFF, 0xFF, 0xFF);
}

/* Test89F: 3P Ultra + Hot Potato's same 3 line counts were drawn TWICE
 * at identical source positions: once in render_tetris_player and again
 * in 3p_core_frame. This preserves that *exact two-pass alpha appearance*
 * by baking both passes into a small per-player tile only when the value
 * changes. Tally/other modes keep their prior pixel paths unchanged. */
typedef struct {
    u32 *tile;
    int x,y,w,h;
    u32 line_count;
    u8 valid;
} TNT3PUltraHpLinesTile;
static TNT3PUltraHpLinesTile g_tnt_3p_ultra_hp_line_tiles[4];
static u8 g_tnt_3p_ultra_hp_line_tiles_active;

static void __attribute__((optimize("Os")))
tnt_vita_3p_ultra_hp_lines_free(void) {
    for (int i=0;i<4;i++) {
        free(g_tnt_3p_ultra_hp_line_tiles[i].tile);
        memset(&g_tnt_3p_ultra_hp_line_tiles[i],0,
            sizeof(g_tnt_3p_ultra_hp_line_tiles[i]));
    }
    g_tnt_3p_ultra_hp_line_tiles_active=FALSE;
}
static void __attribute__((optimize("Os")))
tnt_vita_3p_ultra_hp_lines_prepare(
    int board_x,int board_y,int cell
) {
    static const int text_x[3]={39,123,207};
    tnt_vita_3p_ultra_hp_lines_free();
    for (int i=0;i<3;i++) {
        TNT3PUltraHpLinesTile *t=&g_tnt_3p_ultra_hp_line_tiles[i];
        /* Each result needs <=3 decimal glyphs for goals up to 150.
         * source_x+36 is padded beyond any 3-digit Font C glyph. */
        int x0=tnt_vita_source_x(text_x[i]-3,board_x,cell)-2;
        int x1=tnt_vita_source_x(text_x[i]+36,board_x,cell)+3;
        int y0=tnt_vita_source_y(245,board_y,cell)-2;
        int y1=tnt_vita_source_y(263,board_y,cell)+3;
        if (x0<0 || x1>DISPLAY_WIDTH || y0<0 || y1>DISPLAY_HEIGHT ||
            x0>=x1 || y0>=y1) continue;
        t->x=x0;t->y=y0;t->w=x1-x0;t->h=y1-y0;
        t->tile=malloc((size_t)t->w*t->h*sizeof(u32));
        /* On allocation failure both historical text passes remain live. */
    }
    g_tnt_3p_ultra_hp_line_tiles_active=TRUE;
}
/* Four-player reuses the proven double-alpha Font-C counter cache and
 * allocates only one extra static-background-backed line-number tile. */
static void __attribute__((optimize("Os")))
tnt_vita_4p_lines_prepare(int board_x,int board_y,int cell) {
    TNT3PUltraHpLinesTile *t;
    int x0,x1,y0,y1;
    tnt_vita_3p_ultra_hp_lines_prepare(board_x,board_y,cell);
    t=&g_tnt_3p_ultra_hp_line_tiles[3];
    x0=tnt_vita_source_x(292-3,board_x,cell)-2;
    x1=tnt_vita_source_x(292+36,board_x,cell)+3;
    y0=tnt_vita_source_y(245,board_y,cell)-2;
    y1=tnt_vita_source_y(263,board_y,cell)+3;
    if (x0<0 || x1>DISPLAY_WIDTH || y0<0 ||
        y1>DISPLAY_HEIGHT || x0>=x1 || y0>=y1) return;
    t->x=x0;t->y=y0;t->w=x1-x0;t->h=y1-y0;
    t->tile=malloc((size_t)t->w*t->h*sizeof(u32));
}
static u8 __attribute__((optimize("Os")))
tnt_vita_3p_ultra_hp_line_tile_enabled(int i) {
    return (u8)(g_tnt_3p_ultra_hp_line_tiles_active &&
        i>=0 && i<4 && g_tnt_3p_ultra_hp_line_tiles[i].tile != NULL);
}
static void __attribute__((optimize("Os")))
tnt_vita_3p_ultra_hp_line_tile_render(
    void *buffer, int index, int board_x,int board_y,int cell,
    u32 line_count
) {
    static const int text_x[4]={39,123,207,292};
    TNT3PUltraHpLinesTile *t=&g_tnt_3p_ultra_hp_line_tiles[index];
    if (!t->valid || t->line_count != line_count) {
        char str[12];
        int row;
        snprintf(str,sizeof(str),"%u",(unsigned)line_count);
        for (row=0;row<t->h;row++) {
            size_t off=(size_t)(t->y+row)*DISPLAY_STRIDE+t->x;
            memcpy(g_tnt_gameplay_base_scene+off,
                g_tnt_static_scene+off,(size_t)t->w*sizeof(u32));
        }
        /* Two original calls, same text position and color, in order. */
        tnt_vita_draw_font_c_text(g_tnt_gameplay_base_scene,
            board_x,board_y,cell,text_x[index],248,
            str,0xFF,0xFF,0xFF,0xFF);
        tnt_vita_draw_font_c_text(g_tnt_gameplay_base_scene,
            board_x,board_y,cell,text_x[index],248,
            str,0xFF,0xFF,0xFF,0xFF);
        for (row=0;row<t->h;row++)
            memcpy(t->tile+(size_t)row*t->w,
                g_tnt_gameplay_base_scene+
                    (size_t)(t->y+row)*DISPLAY_STRIDE+t->x,
                (size_t)t->w*sizeof(u32));
        t->line_count=line_count;
        t->valid=TRUE;
    }
    for (int row=0;row<t->h;row++)
        memcpy((u32 *)buffer+(size_t)(t->y+row)*DISPLAY_STRIDE+t->x,
            t->tile+(size_t)row*t->w,(size_t)t->w*sizeof(u32));
}

static s8 tnt_vita_wonder_milestone_index(u32 total) {
    s8 i = -1;
    while (i < 48 && total >= g_tnt_wonder_milestones[i + 1])
        i++;
    return i;
}

static void tnt_vita_wonder_milestone_enter(
    TNTVitaWonderMilestone *milestone,
    s8 milestone_index
) {
    int i;

    memset(milestone, 0, sizeof(*milestone));
    milestone->active = TRUE;
    milestone->milestone_index = milestone_index;
    /* wonders1_800438a8_mediumliner_loop: construction objects beyond the
     * camera/light/base trio consume one inclusive 0..3 RNG draw and add
     * that many 90-degree turns to their wrapper X rotation. Draw exactly
     * once per authored falling block so later frontend RNG stays aligned. */
    if (milestone_index >= 0 && milestone_index <= 6) {
        for (i = 0; i < g_tnt_mayan_milestone_block_count[milestone_index]; i++)
            milestone->block_rotation_x[i] =
                (u16)((tnt_vita_frontend_rng_next() % 4U) * 90U);
    } else if (milestone_index >= 7 && milestone_index <= 13) {
        u8 greek_stage = (u8)(milestone_index - 7);
        for (i = 0; i < g_tnt_greek_milestone_block_count[greek_stage]; i++)
            milestone->block_rotation_x[i] =
                (u16)((tnt_vita_frontend_rng_next() % 4U) * 90U);
    } else if (milestone_index >= 14 && milestone_index <= 20) {
        u8 egyptian_stage = (u8)(milestone_index - 14);
        for (i = 0; i < g_tnt_egyptian_milestone_block_count[egyptian_stage]; i++)
            milestone->block_rotation_x[i] =
                (u16)((tnt_vita_frontend_rng_next() % 4U) * 90U);
    } else if (milestone_index >= 21 && milestone_index <= 27) {
        u8 celtic_stage = (u8)(milestone_index - 21);
        for (i = 0; i < g_tnt_celtic_milestone_block_count[celtic_stage]; i++)
            milestone->block_rotation_x[i] =
                (u16)((tnt_vita_frontend_rng_next() % 4U) * 90U);
    } else if (milestone_index >= 28 && milestone_index <= 34) {
        u8 african_stage = (u8)(milestone_index - 28);
        for (i = 0; i < g_tnt_african_milestone_block_count[african_stage]; i++)
            milestone->block_rotation_x[i] =
                (u16)((tnt_vita_frontend_rng_next() % 4U) * 90U);
    } else if (milestone_index >= 35 && milestone_index <= 41) {
        u8 japanese_stage = (u8)(milestone_index - 35);
        /* Source table milestones 35..41 are the scene-1244 dataset currently
         * stored under the provisional Celtic symbol names. */
        for (i = 0; i < g_tnt_celtic_milestone_block_count[japanese_stage]; i++)
            milestone->block_rotation_x[i] =
                (u16)((tnt_vita_frontend_rng_next() % 4U) * 90U);
    } else if (milestone_index >= 42 && milestone_index <= 48) {
        u8 russian_stage = (u8)(milestone_index - 42);
        /* Canonical Russian construction is scene 1292/1293..1299.  That
         * source dataset was first extracted under provisional African names;
         * reuse the validated authored block table without duplicating it. */
        for (i = 0; i < g_tnt_african_milestone_block_count[russian_stage]; i++)
            milestone->block_rotation_x[i] =
                (u16)((tnt_vita_frontend_rng_next() % 4U) * 90U);
    } else {
        milestone->active = FALSE;
    }

    g_tnt_60hz_last_us = 0;
    g_tnt_60hz_fraction = 0;
}

static int tnt_vita_wonder_milestone_update_render(
    TNTVitaWonderMilestone *milestone,
    PlayerVars *pv_ptr,
    void *buffer,
    int board_x,
    int board_y,
    int cell
) {
    u32 delta;
    u32 total;
    u32 remaining;
    char message[24];
    int x;

    /* Mayan 0..6, Greek 7..13, Egyptian 14..20, Celtic 21..27,
     * African 28..34, Japanese 35..41, Russian 42..48. */
    if (!milestone->active || milestone->milestone_index < 0 ||
        milestone->milestone_index > 48)
        return TNT_MILESTONE_ACTION_EXIT;

    delta = tnt_vita_60hz_delta(TRUE);
    if (delta == 0)
        delta = 1;
    if ((u32)milestone->elapsed_ticks + delta < 0xFFFFU)
        milestone->elapsed_ticks = (u16)(milestone->elapsed_ticks + delta);
    else
        milestone->elapsed_ticks = 0xFFFFU;

    clear_screen(buffer, COLOR(0, 0, 0));
    {
        u8 wonder = (u8)(milestone->milestone_index / 7);
        u8 local_stage = (u8)(milestone->milestone_index % 7);
        int render_result = 0;

        if (g_tnt_milestone_gxm_ready) {
            if (wonder == 0)
                render_result = TNT_Vita_WonderMilestoneGxmRenderMayan(
                    &buffers[backBuffer].surface, buffers[backBuffer].sync,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
            else if (wonder == 1)
                render_result = TNT_Vita_WonderMilestoneGxmRenderGreek(
                    &buffers[backBuffer].surface, buffers[backBuffer].sync,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
            else if (wonder == 2)
                render_result = TNT_Vita_WonderMilestoneGxmRenderEgyptian(
                    &buffers[backBuffer].surface, buffers[backBuffer].sync,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
            else if (wonder == 3)
                render_result = TNT_Vita_WonderMilestoneGxmRenderCeltic(
                    &buffers[backBuffer].surface, buffers[backBuffer].sync,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
            else if (wonder == 4)
                render_result = TNT_Vita_WonderMilestoneGxmRenderAfrican(
                    &buffers[backBuffer].surface, buffers[backBuffer].sync,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
            else if (wonder == 5)
                render_result = TNT_Vita_WonderMilestoneGxmRenderJapanese(
                    &buffers[backBuffer].surface, buffers[backBuffer].sync,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
            else
                render_result = TNT_Vita_WonderMilestoneGxmRenderRussian(
                    &buffers[backBuffer].surface, buffers[backBuffer].sync,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
            if (render_result < 0)
                g_tnt_milestone_gxm_ready = FALSE;
        }
        if (!g_tnt_milestone_gxm_ready) {
            if (wonder == 0)
                TNT_Vita_WonderMilestoneRenderMayan(
                    buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
            else if (wonder == 1)
                TNT_Vita_WonderMilestoneRenderGreek(
                    buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
            else if (wonder == 2)
                TNT_Vita_WonderMilestoneRenderEgyptian(
                    buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
            else if (wonder == 3)
                TNT_Vita_WonderMilestoneRenderCeltic(
                    buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
            else if (wonder == 4)
                TNT_Vita_WonderMilestoneRenderAfrican(
                    buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
            else if (wonder == 5)
                TNT_Vita_WonderMilestoneRenderJapanese(
                    buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
            else
                TNT_Vita_WonderMilestoneRenderRussian(
                    buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                    board_x, board_y, cell, local_stage,
                    milestone->elapsed_ticks, milestone->block_rotation_x);
        }
    }

    /* wonders1_hall_or_exit reports distance to the current Wonder's final
     * total, not distance to the next construction threshold. */
    total = TNT_Vita_SramGetTotalWonderLines();
    {
        u8 wonder = (u8)(milestone->milestone_index / 7);
        remaining = total < g_tnt_wonder_totals[wonder]
            ? g_tnt_wonder_totals[wonder] - total : 0;
    }
    if (remaining == 0)
        snprintf(message, sizeof(message), TNTV_ROM_OriginalText(0U));
    else if (remaining == 1)
        snprintf(message, sizeof(message), TNTV_ROM_OriginalText(1U));
    else
        snprintf(message, sizeof(message), "%u LINES NEEDED", (unsigned)remaining);

    x = 200 - (TNT_Vita_FontB_TextWidth(message) / 2);
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, 250, message,
        0xFF, 0xFF, 0xFF, 0xFF);

    /* Font_Init46Char gives Font B sheet width 16 and Font C width 13.
     * The source uses those fixed sheet widths here rather than measured text
     * widths, producing A@130, HALL@146, B@214, EXIT@230. */
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 130, 266, "A",
        0x7F, 0x7F, 0xFF, 0xFF);
    tnt_vita_draw_font_c_text(
        buffer, board_x, board_y, cell,
        146, 266, "HALL", 0xFF, 0xFF, 0xFF, 0xFF);
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 214, 266, "B",
        0x7F, 0xFF, 0x7F, 0xFF);
    tnt_vita_draw_font_c_text(
        buffer, board_x, board_y, cell,
        230, 266, "EXIT", 0xFF, 0xFF, 0xFF, 0xFF);

    if (milestone->milestone_index == 6 || milestone->milestone_index == 13 ||
        milestone->milestone_index == 20 || milestone->milestone_index == 27 ||
        milestone->milestone_index == 34 || milestone->milestone_index == 41 ||
        milestone->milestone_index == 48) {
        /* Source state 2 holds the final construction through tick 1490.
         * State 3 then performs a ten-tick fade to white before replacing the
         * construction objects with the completed-Wonder presentation. */
        if (milestone->elapsed_ticks >= 1490U) {
            int vx0 = board_x + tnt_floor_div_s64((s64)(0 - 83) * cell, 11);
            int vy0 = board_y + tnt_floor_div_s64((s64)(0 - 36) * cell, 11);
            int vx1 = board_x + tnt_floor_div_s64((s64)(400 - 83) * cell + 10, 11);
            int vy1 = board_y + tnt_floor_div_s64((s64)(300 - 36) * cell + 10, 11);
            u32 fade_tick = milestone->elapsed_ticks - 1490U;
            u8 alpha;

            if (fade_tick >= 10U)
                return TNT_MILESTONE_ACTION_COMPLETE;
            alpha = (u8)((fade_tick * 255U) / 10U);
            draw_rect_alpha(buffer, vx0, vy0, vx1 - vx0, vy1 - vy0,
                            255, 255, 255, alpha);
            return TNT_MILESTONE_ACTION_NONE;
        }
    }

    if (pv_ptr->unk24 & CONT_A)
        return TNT_MILESTONE_ACTION_HALL;
    if (pv_ptr->unk24 & CONT_B)
        return TNT_MILESTONE_ACTION_EXIT;
    return TNT_MILESTONE_ACTION_NONE;
}

/* All seven completed-Wonder narratives are player-ROM strings, not
 * compiled original copyrighted game writing. Indexed by Wonder 0..6;
 * preserving original 18-slot row capacity, row counts and blank spacing. */
char g_tnt_story_storage[7][18][32];
const char *g_tnt_complete_stories[7][18];

static void tnt_vita_wonder_complete_enter(
    TNTVitaWonderComplete *complete, u8 wonder, u8 final_only
) {
    memset(complete, 0, sizeof(*complete));
    complete->active = TRUE;
    complete->wonder = wonder;
    /* Source only offers A FINALE on the fresh Russian threshold-crossing
     * ceremony. Later visits to an already-completed Russian Wonder use HALL. */
    complete->finale_prompt = (u8)(wonder == 6U && !final_only &&
        TNT_Vita_SramGetTotalWonderLines() >= 500000U);
    /* A normally selected already-completed Wonder enters the source's
     * finished-Wonder viewer directly. The threshold-crossing path starts at
     * the 45-tick fade-from-white completed-Wonder presentation instead. */
    complete->phase = final_only ? 4 : 0;
    g_tnt_60hz_last_us = 0;
    g_tnt_60hz_fraction = 0;
}

static void tnt_vita_wonder_complete_story_data(
    u8 wonder, const char *const **story_out, u32 *line_count_out
) {
    const char *const *story = g_tnt_complete_stories[wonder<=6U?wonder:0U];
    u32 line_count = wonder == 6 ? 15U : (wonder == 5 ? 17U :
        (wonder == 2 ? 16U : ((wonder == 1 || wonder == 3 || wonder == 4) ? 14U : 18U)));
    if(story_out)*story_out=story;if(line_count_out)*line_count_out=line_count;
}

static void tnt_vita_wonder_complete_story(
    void *buffer, int board_x, int board_y, int cell, u8 wonder, u32 ticks
) {
    const char *const *story;u32 line_count;
    tnt_vita_wonder_complete_story_data(wonder,&story,&line_count);
    u32 scroll_px = ticks / 8U;
    u32 line_advance = scroll_px / 12U;
    int pixel_offset = (int)(scroll_px % 12U);
    int i;

    for (i = 0; i < 5; i++) {
        u32 index = ((line_count - 4U) + line_advance + (u32)i) % line_count;
        tnt_vita_draw_font_c_text(
            buffer, board_x, board_y, cell,
            80, 30 - pixel_offset + i * 12,
            story[index],
            0xFF, 0xFF, 0xFF, 0xFF);
    }
}

static void tnt_vita_wonder_complete_controls(
    void *buffer, int board_x, int board_y, int cell, const char *a_label
) {
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 130, 266, "A",
        0x7F, 0x7F, 0xFF, 0xFF);
    tnt_vita_draw_font_c_text(
        buffer, board_x, board_y, cell,
        146, 266, a_label, 0xFF, 0xFF, 0xFF, 0xFF);
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 214, 266, "B",
        0x7F, 0xFF, 0x7F, 0xFF);
    tnt_vita_draw_font_c_text(
        buffer, board_x, board_y, cell,
        230, 266, "EXIT", 0xFF, 0xFF, 0xFF, 0xFF);
}

static int tnt_vita_wonder_complete_update_render(
    TNTVitaWonderComplete *complete, PlayerVars *pv_ptr,
    void *buffer, int board_x, int board_y, int cell
) {
    u32 delta;
    int vx0, vy0, vx1, vy1;
    u8 overlay_alpha = 0;
    u8 overlay_white = FALSE;
    u8 story_visible = FALSE;
    u8 story_gpu_drawn = FALSE;
    u32 story_ticks = 0, story_line_count = 0;
    const char *const *story = NULL;

    if (!complete->active)
        return TNT_COMPLETE_ACTION_EXIT;

    delta = tnt_vita_60hz_delta(TRUE);
    if (delta == 0)
        delta = 1;

    /* Determine whether the source story overlay is visible before submitting
     * the GXM scene, so Font C can be composited by the GPU in that same scene. */
    tnt_vita_wonder_complete_story_data(complete->wonder,&story,&story_line_count);
    if(complete->phase<=2){story_visible=TRUE;story_ticks=complete->story_ticks;}
    else if(complete->phase==4){
        u32 story_start = complete->wonder == 6 ? 3999U :
            (complete->wonder == 5 ? 2399U :
            (complete->wonder == 4 ? 2999U :
            (complete->wonder == 3 ? 4395U :
            (complete->wonder == 2 ? 3439U :
            (complete->wonder == 1 ? 3999U : 2749U)))));
        if(complete->final_ticks>=story_start){story_visible=TRUE;story_ticks=complete->final_ticks-story_start;}
    }

    /* Full cinematic color/depth clear and, when visible, story text now run
     * in the same GXM scene. */
    g_tnt_wonder_clear_us=0;
    {
        u8 anim_mode = complete->phase <= 2
            ? TNT_WONDER_COMPLETE_ANIM_PRESENTATION
            : TNT_WONDER_COMPLETE_ANIM_FINAL;
        u32 anim_ticks = complete->phase == 4 ? complete->final_ticks : 0U;
        if (!g_tnt_milestone_gxm_ready ||
            TNT_Vita_WonderCompleteGxmRender(
                &buffers[backBuffer].surface, buffers[backBuffer].sync,
                board_x, board_y, cell, complete->wonder,
                anim_mode, anim_ticks,story,story_line_count,story_ticks,story_visible) < 0) {
            /* Keep the state machine usable even if GXM rejects the completion
             * scene; the software text path remains as an exact fallback. */
            clear_screen(buffer, COLOR(0, 0, 0));
        } else if(story_visible) {
            story_gpu_drawn=TRUE;
        }
    }

    vx0 = board_x + tnt_floor_div_s64((s64)(0 - 83) * cell, 11);
    vy0 = board_y + tnt_floor_div_s64((s64)(0 - 36) * cell, 11);
    vx1 = board_x + tnt_floor_div_s64((s64)(400 - 83) * cell + 10, 11);
    vy1 = board_y + tnt_floor_div_s64((s64)(300 - 36) * cell + 10, 11);

    if (complete->phase <= 2) {
        const char *title = TNTV_ROM_OriginalText(0U);
        int title_x = 200 - TNT_Vita_FontB_TextWidth(title) / 2;
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, title_x, 250, title,
            0xFF, 0xFF, 0xFF, 0xFF);
        if(!story_gpu_drawn)
            tnt_vita_wonder_complete_story(
                buffer, board_x, board_y, cell,
                complete->wonder, complete->story_ticks);
    } else if (complete->phase == 4) {
        /* Story text begins at each source final animation's authored terminal.
         * The GXM path owns it when available; retain software as fallback. */
        if(story_visible && !story_gpu_drawn)
            tnt_vita_wonder_complete_story(
                buffer, board_x, board_y, cell, complete->wonder, story_ticks);
        tnt_vita_wonder_complete_controls(
            buffer, board_x, board_y, cell,
            complete->finale_prompt ? "FINALE" : "HALL");
    }

    switch (complete->phase) {
    case 0: { /* completed scene, accelerated fade from white */
        /* The N64 state machine requests a 45-retrace fade-from-white here.
         * On Vita that reveal reads noticeably slower than the reference
         * transition in motion, so retain the source 10-tick fade TO white
         * above and shorten only this reveal to 15 ticks. */
        const u32 white_reveal_ticks = 15U;
        complete->phase_ticks += delta;
        overlay_white = TRUE;
        if (complete->phase_ticks >= white_reveal_ticks) {
            complete->phase = 1;
            complete->phase_ticks = 0;
            complete->story_ticks = 0;
            overlay_alpha = 0;
        } else {
            overlay_alpha = (u8)(((white_reveal_ticks - complete->phase_ticks) * 255U) /
                                 white_reveal_ticks);
        }
        break;
    }
    case 1: {
        u32 story_duration = complete->wonder == 5 ? 1440U :
            (complete->wonder == 2 ? 1344U :
            ((complete->wonder == 1 || complete->wonder == 3 || complete->wonder == 4) ? 1152U : 1536U));
        /* Source duration is (story_lines - 2) * 12 * 8 ticks. */
        complete->story_ticks += delta;
        if (complete->story_ticks >= story_duration) {
            complete->story_ticks = story_duration;
            complete->phase = 2;
            complete->phase_ticks = 0;
        }
        break;
    }
    case 2: /* fade completed scene/story to black over 90 ticks */
        complete->phase_ticks += delta;
        if (complete->phase_ticks >= 90U) {
            complete->phase = 3;
            complete->phase_ticks = 0;
            overlay_alpha = 255;
        } else {
            overlay_alpha = (u8)((complete->phase_ticks * 255U) / 90U);
        }
        break;
    case 3: /* finished-Wonder viewer, fade from black over 90 ticks */
        complete->phase_ticks += delta;
        if (complete->phase_ticks >= 90U) {
            complete->phase = 4;
            complete->phase_ticks = 0;
            complete->final_ticks = 0;
            overlay_alpha = 0;
        } else {
            overlay_alpha = (u8)(((90U - complete->phase_ticks) * 255U) / 90U);
        }
        break;
    case 4:
        if (complete->final_ticks + delta < 0xFFFFFFFFU)
            complete->final_ticks += delta;
        if (pv_ptr->unk24 & CONT_A)
            return complete->finale_prompt
                ? TNT_COMPLETE_ACTION_FINALE : TNT_COMPLETE_ACTION_HALL;
        if (pv_ptr->unk24 & CONT_B)
            return TNT_COMPLETE_ACTION_EXIT;
        break;
    default:
        complete->phase = 4;
        break;
    }

    if (overlay_alpha != 0) {
        u8 c = overlay_white ? 255 : 0;
        draw_rect_alpha(buffer, vx0, vy0, vx1 - vx0, vy1 - vy0,
                        c, c, c, overlay_alpha);
    }
    return TNT_COMPLETE_ACTION_NONE;
}

static void tnt_vita_wonder_finale_enter(TNTVitaWonderFinale *finale) {
    memset(finale, 0, sizeof(*finale));
    finale->active = TRUE;
    finale->wonder = 0;
    finale->anim_ticks = 0;
    g_tnt_60hz_last_us = 0;
    g_tnt_60hz_fraction = 0;
}

static int tnt_vita_wonder_finale_update_render(
    TNTVitaWonderFinale *finale, PlayerVars *pv_ptr,
    void *buffer, int board_x, int board_y, int cell
) {
    static const u16 duration[7] = {
        1999, 1901, 2417, 1919, 1879, 2319, 2359
    };
    u32 delta;
    const char *name;
    int x;

    if (!finale->active)
        return TNT_FINALE_ACTION_EXIT;
    if (finale->wonder > 6U)
        finale->wonder = 0;

    delta = tnt_vita_60hz_delta(TRUE);
    if (delta == 0) delta = 1;

    /* Full cinematic color/depth clear now runs in the GXM scene. */
    g_tnt_wonder_clear_us=0;
    if (!g_tnt_milestone_gxm_ready ||
        TNT_Vita_WonderCompleteGxmRender(
            &buffers[backBuffer].surface, buffers[backBuffer].sync,
            board_x, board_y, cell, finale->wonder,
            TNT_WONDER_COMPLETE_ANIM_FINALE, finale->anim_ticks,
            NULL,0U,0U,FALSE) < 0) {
        clear_screen(buffer, COLOR(0, 0, 0));
    }

    /* wonders1_800455bc: source writes the currently cycling Wonder name at
     * y=250 and keeps A HALL / B EXIT live throughout the FINALE loop. */
    name = g_tnt_wonder_names[finale->wonder];
    x = 200 - (TNT_Vita_FontB_TextWidth(name) / 2);
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, 250, name,
        0xFF, 0xFF, 0xFF, 0xFF);
    tnt_vita_wonder_complete_controls(
        buffer, board_x, board_y, cell, "HALL");

    if (pv_ptr->unk24 & CONT_A)
        return TNT_FINALE_ACTION_HALL;
    if (pv_ptr->unk24 & CONT_B)
        return TNT_FINALE_ACTION_EXIT;

    if (finale->anim_ticks + delta >= duration[finale->wonder]) {
        finale->anim_ticks = 0;
        finale->wonder = (u8)((finale->wonder + 1U) % 7U);
    } else {
        finale->anim_ticks += delta;
    }
    return TNT_FINALE_ACTION_NONE;
}

static u8 g_tnt210_hall_gpu_bg_ready;
static void tnt_vita_wonder_hall_build_cache(
    u8 screen,
    int board_x,
    int board_y,
    int cell
) {
    u8 *const pie_img[8] = {
        g_tnt_wonder_pie_a_ci8, g_tnt_wonder_pie_b_ci8,
        g_tnt_wonder_pie_c_ci8, g_tnt_wonder_pie_d_ci8,
        g_tnt_wonder_pie_e_ci8, g_tnt_wonder_pie_f_ci8,
        g_tnt_wonder_pie_g_ci8, g_tnt_wonder_pie_h_ci8
    };
    u16 *const pie_pal[8] = {
        g_tnt_wonder_pie_a_rgba16, g_tnt_wonder_pie_b_rgba16,
        g_tnt_wonder_pie_c_rgba16, g_tnt_wonder_pie_d_rgba16,
        g_tnt_wonder_pie_e_rgba16, g_tnt_wonder_pie_f_rgba16,
        g_tnt_wonder_pie_g_rgba16, g_tnt_wonder_pie_h_rgba16
    };
    u32 total = TNT_Vita_SramGetTotalWonderLines();
    s8 reached = tnt_vita_wonder_milestone_index(total);
    u8 current_wonder = 0;
    u8 stage = 0;
    u32 remaining;
    s8 display_milestone;
    char progress[12];
    int x;

    while (current_wonder < 7 && total >= g_tnt_wonder_totals[current_wonder])
        current_wonder++;

    if (TNT_Vita_SramGetUnlockAll() ||
        (screen == 7U && total >= 500000U)) {
        /* Override, or retail fully-complete FINALE card: pie H / zero lines. */
        stage = 7;
        remaining = 0;
        display_milestone = screen < 7 ? (s8)(screen * 7 + 6) : 48;
    } else {
        /* wonders3_display_wonders_hall_text: a non-current screen is evaluated at
         * its seventh construction milestone. The current in-progress screen uses
         * D_800CF830 directly. */
        if (screen != current_wonder ||
            reached < 0 || (u8)(reached / 7) != screen)
            display_milestone = (s8)(screen * 7 + 6);
        else
            display_milestone = reached;

        /* Retail Hall normally cycles only 0..6, so screen 7 exists here only
         * through TNTV4ALL. Keep this bounds-safe for future Finale routing. */
        remaining = screen < 7 && total < g_tnt_wonder_totals[screen]
            ? g_tnt_wonder_totals[screen] - total : 0;
        if (reached >= display_milestone)
            stage = (u8)((display_milestone - (s8)(screen * 7)) + 1);
        if (stage > 7)
            stage = 7;
    }

    snprintf(progress, sizeof(progress), "%u", (unsigned)remaining);
    memcpy(g_tnt_wonder_static_scene, g_tnt_menu_background_scene,
           sizeof(g_tnt_wonder_static_scene));

    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        3, 28, 397, 254,
        g_tnt_wonder_overlay_ci8, g_tnt_wonder_overlay_rgba16, 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        35, 150, 70, 109,
        pie_img[stage], pie_pal[stage], 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        55, 65, 50, 34,
        g_tnt_wonder_arrow_left_ci8, g_tnt_wonder_arrow_left_rgba16, 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        300, 65, 49, 33,
        g_tnt_wonder_arrow_right_ci8, g_tnt_wonder_arrow_right_rgba16, 0xFF);

    x = 70 - (TNT_Vita_FontB_TextWidth(progress) / 2);
    TNT_Vita_FontB_DrawText(
        g_tnt_wonder_static_scene,
        DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, 180, progress,
        0xFF, 0xFF, 0xFF, 0xFF);
    x = 200 - (TNT_Vita_FontB_TextWidth(g_tnt_wonder_names[screen]) / 2);
    TNT_Vita_FontB_DrawText(
        g_tnt_wonder_static_scene,
        DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, 80, g_tnt_wonder_names[screen],
        0xFF, 0xFF, 0xFF, 0xFF);

    TNT_Vita_FontB_DrawText(
        g_tnt_wonder_static_scene,
        DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, 130, 270, "A",
        0x7F, 0x7F, 0xFF, 0xFF);
    tnt_vita_draw_font_c_text(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        146, 271, "ACCEPT", 0xFF, 0xFF, 0xFF, 0xFF);
    x = 130 + 32 + tnt_vita_font_c_text_width_native("ACCEPT");
    TNT_Vita_FontB_DrawText(
        g_tnt_wonder_static_scene,
        DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, 270, "B",
        0x7F, 0xFF, 0x7F, 0xFF);
    tnt_vita_draw_font_c_text(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        x + 16, 271, "BACK", 0xFF, 0xFF, 0xFF, 0xFF);
    /* Stage the unchanged completed Hall page once per selection. Reusing
     * it on the GPU avoids 2.2 MiB/frame of CPU screen restoration. */
    g_tnt210_hall_gpu_bg_ready=(u8)(g_tnt_milestone_gxm_ready &&
        TNT_Vita_WonderHallGxmSetBackground(g_tnt_wonder_static_scene)==0);
}

/* Test209D hardware Hall-only timing evidence. Keep the FPS counter normal,
 * but log a compact 60-frame window per selected model so actual Vita time
 * (render CPU including GPU finish, GPU finish wait subset, VBlank/present)
 * can be compared without a stream of per-frame disk I/O.
 * NO changes to original save, ROM, Hall animation timing or gameplay. */
static struct {
    u8 current;
    u32 samples;
    u64 start_us;
    u64 render_us;
    u64 present_us;
    u64 gpu_wait_us;
    u64 geometry_us;
    u64 begin_us;
    u64 submit_us;
    u64 finish_us;
} g_tnt209_hall_profile={.current=255};

static void tnt209_hall_profile_note(
    u8 screen,u8 state,u64 start_us,u64 render_end,u64 end_us,u32 gpu_wait_us
) {
    FILE *f;
    if(state!=0U)return;
    if(g_tnt209_hall_profile.current!=screen){
        g_tnt209_hall_profile.current=screen;
        g_tnt209_hall_profile.samples=0;
        g_tnt209_hall_profile.render_us=0;
        g_tnt209_hall_profile.present_us=0;
        g_tnt209_hall_profile.gpu_wait_us=0;
        g_tnt209_hall_profile.geometry_us=0;
        g_tnt209_hall_profile.begin_us=0;
        g_tnt209_hall_profile.submit_us=0;
        g_tnt209_hall_profile.finish_us=0;
        g_tnt209_hall_profile.start_us=start_us;
    }
    g_tnt209_hall_profile.samples++;
    g_tnt209_hall_profile.render_us+=render_end-start_us;
    g_tnt209_hall_profile.present_us+=end_us-render_end;
    g_tnt209_hall_profile.gpu_wait_us+=gpu_wait_us;
    if(screen<7U){
        g_tnt209_hall_profile.geometry_us+=g_tnt_wonder_profile_us[0];
        g_tnt209_hall_profile.begin_us+=g_tnt_wonder_profile_us[1];
        g_tnt209_hall_profile.submit_us+=g_tnt_wonder_profile_us[2];
        g_tnt209_hall_profile.finish_us+=g_tnt_wonder_profile_us[3];
    }
    if(g_tnt209_hall_profile.samples<60)return;
    f=fopen("ux0:data/TNTV/tntv_hall212_timing.txt","a");
    if(f){
        const u64 wall=end_us-g_tnt209_hall_profile.start_us;
        fprintf(f,"WONDER %u: %u frames in %llu ms = %.2f FPS, "
            "avg_render=%llu us, avg_present=%llu us, avg_gpu_finish_wait=%llu us, "
            "avg_3d_geometry=%llu us, avg_gxm_begin=%llu us, "
            "avg_gxm_submit=%llu us, avg_gxm_finish=%llu us\n",
            (unsigned)screen,(unsigned)g_tnt209_hall_profile.samples,
            (unsigned long long)(wall/1000U),
            wall?((double)g_tnt209_hall_profile.samples*1000000.0/(double)wall):0.0,
            (unsigned long long)(g_tnt209_hall_profile.render_us/g_tnt209_hall_profile.samples),
            (unsigned long long)(g_tnt209_hall_profile.present_us/g_tnt209_hall_profile.samples),
            (unsigned long long)(g_tnt209_hall_profile.gpu_wait_us/g_tnt209_hall_profile.samples),
            (unsigned long long)(g_tnt209_hall_profile.geometry_us/g_tnt209_hall_profile.samples),
            (unsigned long long)(g_tnt209_hall_profile.begin_us/g_tnt209_hall_profile.samples),
            (unsigned long long)(g_tnt209_hall_profile.submit_us/g_tnt209_hall_profile.samples),
            (unsigned long long)(g_tnt209_hall_profile.finish_us/g_tnt209_hall_profile.samples));
        fclose(f);
    }
    g_tnt209_hall_profile.samples=0;
    g_tnt209_hall_profile.render_us=0;
    g_tnt209_hall_profile.present_us=0;
    g_tnt209_hall_profile.gpu_wait_us=0;
    g_tnt209_hall_profile.geometry_us=0;
    g_tnt209_hall_profile.begin_us=0;
    g_tnt209_hall_profile.submit_us=0;
    g_tnt209_hall_profile.finish_us=0;
    g_tnt209_hall_profile.start_us=end_us;
}

static void tnt_vita_wonder_hall_enter(
    TNTVitaWonderHall *hall,
    u8 screen,
    int board_x,
    int board_y,
    int cell
) {
    memset(hall, 0, sizeof(*hall));
    hall->active = TRUE;
    hall->state = 1;
    hall->entrance_ticks = 31;
    hall->current_screen = screen;
    hall->next_screen = screen;
    tnt_vita_wonder_hall_build_cache(screen, board_x, board_y, cell);
    {
        FILE *f=fopen("ux0:data/TNTV/tntv_hall212_timing.txt","w");
        if(f){fputs("Test212D all 3 Hall types GPU: 0 may be current pile, 1..6 locked marks with cheat off; model7 FINALE. "
                    "GPU wait is a subset of render.\n",f);fclose(f);}
        g_tnt209_hall_profile.current=255;
        g_tnt209_hall_profile.samples=0;
    }
    g_tnt_60hz_last_us = 0;
    g_tnt_60hz_fraction = 0;
}

static int tnt_vita_wonder_hall_update_render(
    TNTVitaWonderHall *hall,
    PlayerVars *pv_ptr,
    void *buffer,
    int board_x,
    int board_y,
    int cell
) {
    u32 delta;
    u32 total = TNT_Vita_SramGetTotalWonderLines();
    u8 current_wonder = 0;
    s8 reached = tnt_vita_wonder_milestone_index(total);
    u8 selectable;

    while (current_wonder < 7 && total >= g_tnt_wonder_totals[current_wonder])
        current_wonder++;
    selectable = TNT_Vita_SramGetUnlockAll()
        ? (u8)(hall->current_screen <= 7U)
        : (u8)((hall->current_screen == 7U && total >= 500000U) ||
            hall->current_screen < current_wonder ||
            (hall->current_screen == current_wonder && reached >= 0 &&
             (u8)(reached / 7) == hall->current_screen));

    if (hall->state == 1) {
        (void)tnt_vita_60hz_delta(FALSE);

        /* Original wonders3_wonder_viewer() calls func_8009FA2C() and stays
         * in entrance state 1 until all 16 authored background tetrominoes
         * have landed.  TNTV already had the matching 31-frame state timer
         * but accidentally copied the fully settled background throughout
         * it.  Reuse the same restored assembly/SFX path as the other GUI
         * pages so WONDERS visibly assembles exactly where the source does. */
        if (tnt_vita_menu_assembly_advance(&hall->entrance_ticks)) {
            tnt_vita_menu_render_assembly(
                buffer, board_x, board_y, cell, hall->entrance_ticks);
            return TNT_HALL_ACTION_NONE;
        }

        hall->state = 2;
        memcpy(buffer, g_tnt_menu_background_scene,
               sizeof(g_tnt_menu_background_scene));
        return TNT_HALL_ACTION_NONE;
    }
    if (hall->state == 2) {
        (void)tnt_vita_60hz_delta(FALSE);
        hall->state = 3;
        memcpy(buffer, g_tnt_menu_background_scene,
               sizeof(g_tnt_menu_background_scene));
        return TNT_HALL_ACTION_NONE;
    }
    if (hall->state == 3) {
        (void)tnt_vita_60hz_delta(FALSE);
        hall->state = 4;
        memcpy(buffer, g_tnt_wonder_static_scene,
               sizeof(g_tnt_wonder_static_scene));
        return TNT_HALL_ACTION_NONE;
    }
    if (hall->state == 5) {
        hall->current_screen = hall->next_screen;
        hall->anim_tick = 0;
        tnt_vita_wonder_hall_build_cache(
            hall->current_screen, board_x, board_y, cell);
        hall->state = 0;
    } else if (hall->state == 0) {
        if (selectable && (pv_ptr->unk24 & CONT_A)) {
            memcpy(buffer, g_tnt_wonder_static_scene,
                   sizeof(g_tnt_wonder_static_scene));
            return TNT_HALL_ACTION_ACCEPT;
        }
        if (pv_ptr->unk24 & CONT_B) {
            memcpy(buffer, g_tnt_wonder_static_scene,
                   sizeof(g_tnt_wonder_static_scene));
            return TNT_HALL_ACTION_BACK;
        }
        if (pv_ptr->unk24 & CONT_LEFT) {
            u8 hall_max = (TNT_Vita_SramGetUnlockAll() || total >= 500000U) ? 7U : 6U;
            hall->next_screen = hall->current_screen != 0
                ? (u8)(hall->current_screen - 1) : hall_max;
            hall->state = 5;
            memcpy(buffer, g_tnt_wonder_static_scene,
                   sizeof(g_tnt_wonder_static_scene));
            return TNT_HALL_ACTION_NONE;
        }
        if (pv_ptr->unk24 & CONT_RIGHT) {
            u8 hall_max = (TNT_Vita_SramGetUnlockAll() || total >= 500000U) ? 7U : 6U;
            hall->next_screen = hall->current_screen < hall_max
                ? (u8)(hall->current_screen + 1) : 0;
            hall->state = 5;
            memcpy(buffer, g_tnt_wonder_static_scene,
                   sizeof(g_tnt_wonder_static_scene));
            return TNT_HALL_ACTION_NONE;
        }
    }

    /* State 4 initializes the selected Borg object and renders it immediately;
     * state 5 likewise installs the next model before its first full frame. */
    delta = tnt_vita_60hz_delta(TRUE);
    if (delta == 0)
        delta = 1;
    hall->anim_tick = (u16)(hall->anim_tick + delta);
    hall->state = 0;
    /* Test212D: all Hall cards now restore their identical, already
     * composited static background inside the *same* GPU scene as the
     * selected 3D model, including current-Wonder pile and locked mark.
     * CPU copy remains a fallback if texture/GXM initialization fails. */
    if(!g_tnt_milestone_gxm_ready || !g_tnt210_hall_gpu_bg_ready)
        memcpy(buffer,g_tnt_wonder_static_scene,
            sizeof(g_tnt_wonder_static_scene));
    if (TNT_Vita_SramGetUnlockAll() && hall->current_screen <= 6) {
        /* TNTV4ALL shows the finished Hall presentation for every restored
         * Wonder independent of the real save's current Wonder. */
        if (g_tnt_milestone_gxm_ready &&
            TNT_Vita_WonderCompleteGxmRender(
                &buffers[backBuffer].surface, buffers[backBuffer].sync,
                board_x, board_y, cell, hall->current_screen,
                TNT_WONDER_COMPLETE_ANIM_HALL, hall->anim_tick,
                NULL,0U,0U,FALSE)<0)
            memcpy(buffer,g_tnt_wonder_static_scene,
                   sizeof(g_tnt_wonder_static_scene));
    } else if ((TNT_Vita_SramGetUnlockAll() || total >= 500000U) &&
               hall->current_screen == 7) {
        /* Test209D: scene1425/animation1426 now uses the same GXM context
         * as the other seven Hall cards. Keep exact CPU software fallback. */
        if (!g_tnt_milestone_gxm_ready ||
            TNT_Vita_WonderFinaleHallGxmRender(
                &buffers[backBuffer].surface, buffers[backBuffer].sync,
                board_x, board_y, cell, hall->anim_tick) < 0) {
            memcpy(buffer,g_tnt_wonder_static_scene,
                   sizeof(g_tnt_wonder_static_scene));
            TNT_Vita_WonderRenderFinaleHall(
                buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                board_x, board_y, cell, hall->anim_tick);
        }
    } else if (hall->current_screen < current_wonder && hall->current_screen <= 6) {
        /* Restored completed Wonders use their real Borg scenes in the Hall
         * rather than the future-Wonder golden question mark. */
        if (g_tnt_milestone_gxm_ready &&
            TNT_Vita_WonderCompleteGxmRender(
                &buffers[backBuffer].surface, buffers[backBuffer].sync,
                board_x, board_y, cell, hall->current_screen,
                TNT_WONDER_COMPLETE_ANIM_HALL, hall->anim_tick,
                NULL,0U,0U,FALSE)<0)
            memcpy(buffer,g_tnt_wonder_static_scene,
                   sizeof(g_tnt_wonder_static_scene));
    } else if (hall->current_screen == current_wonder) {
        /* D_800CFB30[8] (resource 1048) is the generic current-Wonder
         * progress model. Pause -> PROGRESS already uses the exact same
         * resource through GXM; reuse that approved path here as well instead
         * of retaining a second 568-triangle CPU rasterizer in the Hall. */
        if (!g_tnt_milestone_gxm_ready ||
            TNT_Vita_WonderProgressHallGxmRender(
                &buffers[backBuffer].surface, buffers[backBuffer].sync,
                board_x, board_y, cell,
                (u16)(hall->anim_tick & 511U)) < 0) {
            memcpy(buffer,g_tnt_wonder_static_scene,
                   sizeof(g_tnt_wonder_static_scene));
            TNT_Vita_WonderRenderMayan(
                buffer,
                DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                board_x, board_y, cell,
                (u16)(hall->anim_tick & 511U));
        }
    } else {
        /* Original scene1071 GPU question-mark. Source interpolation
         * and 60Hz animation ticks are unchanged. Software-only fallback
         * still works on a GPU texture/scene allocation failure. */
        if(!g_tnt_milestone_gxm_ready ||
           TNT_Vita_WonderLockedHallGxmRender(
               &buffers[backBuffer].surface, buffers[backBuffer].sync,
               board_x, board_y, cell,
               (u16)(hall->anim_tick % 511U))<0){
            memcpy(buffer,g_tnt_wonder_static_scene,
                   sizeof(g_tnt_wonder_static_scene));
            TNT_Vita_WonderRenderLocked(
                buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
                board_x,board_y,cell,(u16)(hall->anim_tick % 511U));
        }
    }
    return TNT_HALL_ACTION_NONE;
}

static void tnt_vita_wonder_build_cache(
    int board_x,
    int board_y,
    int cell
) {
    u8 *const pie_img[8] = {
        g_tnt_wonder_pie_a_ci8, g_tnt_wonder_pie_b_ci8,
        g_tnt_wonder_pie_c_ci8, g_tnt_wonder_pie_d_ci8,
        g_tnt_wonder_pie_e_ci8, g_tnt_wonder_pie_f_ci8,
        g_tnt_wonder_pie_g_ci8, g_tnt_wonder_pie_h_ci8
    };
    u16 *const pie_pal[8] = {
        g_tnt_wonder_pie_a_rgba16, g_tnt_wonder_pie_b_rgba16,
        g_tnt_wonder_pie_c_rgba16, g_tnt_wonder_pie_d_rgba16,
        g_tnt_wonder_pie_e_rgba16, g_tnt_wonder_pie_f_rgba16,
        g_tnt_wonder_pie_g_rgba16, g_tnt_wonder_pie_h_rgba16
    };
    u32 total = TNT_Vita_SramGetTotalWonderLines();
    u32 remaining = 0;
    u8 wonder = 0;
    u8 stage = 0;
    const char *name;
    const char *accept = "ACCEPT";
    char progress[12];
    int x;
    int i;

    /* wonders2_80045e50_sets_num_won_compl_q: current Wonder is selected from
     * cumulative totals. Pause PROGRESS always opens that current screen. */
    while (wonder < 7 && total >= g_tnt_wonder_totals[wonder])
        wonder++;

    if (wonder < 7) {
        remaining = g_tnt_wonder_totals[wonder] - total;
        /* wonders3_display_wonders_hall_text chooses A..G from the number of
         * reached construction milestones in the current incomplete Wonder. */
        for (i = 0; i < 7; i++) {
            if (total >= g_tnt_wonder_milestones[wonder * 7 + i])
                stage = (u8)(i + 1);
            else
                break;
        }
        if (stage > 7)
            stage = 7;
    } else {
        /* Completed Finale uses pie H and a different scene asset (1426).
         * Keep its 2D source state correct; that far-end 3D asset remains a
         * later Wonders-viewer slice. */
        wonder = 7;
        stage = 7;
        remaining = 0;
    }

    name = g_tnt_wonder_names[wonder];
    snprintf(progress, sizeof(progress), "%u", (unsigned)remaining);

    memcpy(
        g_tnt_wonder_static_scene,
        g_tnt_menu_background_scene,
        sizeof(g_tnt_wonder_static_scene));

    /* wonders3_init_tuples / wonders3_display_wonders_hall_text. */
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        3, 28, 397, 254,
        g_tnt_wonder_overlay_ci8, g_tnt_wonder_overlay_rgba16, 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        35, 150, 70, 109,
        pie_img[stage], pie_pal[stage], 0xFF);

    x = 70 - (TNT_Vita_FontB_TextWidth(progress) / 2);
    TNT_Vita_FontB_DrawText(
        g_tnt_wonder_static_scene,
        DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, 180, progress,
        0xFF, 0xFF, 0xFF, 0xFF);

    x = 200 - (TNT_Vita_FontB_TextWidth(name) / 2);
    TNT_Vita_FontB_DrawText(
        g_tnt_wonder_static_scene,
        DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, 80, name,
        0xFF, 0xFF, 0xFF, 0xFF);

    /* arg1 == FALSE still reserves the absent "A ACCEPT" width before the
     * source draws B/BACK. Font B's native sheet width is 16. */
    x = 130 + (16 * 2) + tnt_vita_font_c_text_width_native(accept);
    TNT_Vita_FontB_DrawText(
        g_tnt_wonder_static_scene,
        DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell, x, 270, "B",
        0x7F, 0xFF, 0x7F, 0xFF);
    x += 16;
    tnt_vita_draw_font_c_text(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        x, 271, "BACK", 0xFF, 0xFF, 0xFF, 0xFF);
}

static void tnt_vita_wonder_enter(
    TNTVitaWonderViewer *viewer,
    int board_x,
    int board_y,
    int cell
) {
    /* PROGRESS is a snapshot of persisted Wonder progress at viewer entry. */
    tnt_vita_wonder_build_cache(board_x, board_y, cell);
    memset(viewer, 0, sizeof(*viewer));
    viewer->active = TRUE;
    viewer->state = 1;
    viewer->entrance_ticks = 31;
    g_tnt_60hz_last_us = 0;
    g_tnt_60hz_fraction = 0;
}

static u8 tnt_vita_wonder_update_render(
    TNTVitaWonderViewer *viewer,
    PlayerVars *pv_ptr,
    void *buffer,
    int board_x,
    int board_y,
    int cell
) {
    u32 delta;

    if (viewer->state == 1) {
        /* TEST101: source wonders3_wonder_viewer() calls
         * func_8009FA2C(&D_801290D0), starting the SAME 16-group
         * falling-tetromino IMG_MAIN_BACKGROUND assembly used by the
         * frontend and already restored in our Wonder Hall.
         *
         * Test100 and earlier accidentally held the completely settled
         * backdrop for the 31 entrance ticks. Render the authored
         * falling pieces instead, with identical 30 Hz clock and per-block
         * landing SFX; the GXM Wonder model / viewer controls remain as
         * they were. Works for both solo and 2/3/4P Pause -> PROGRESS. */
        if (tnt_vita_menu_assembly_advance(&viewer->entrance_ticks)) {
            tnt_vita_menu_render_assembly(
                buffer, board_x, board_y, cell, viewer->entrance_ticks);
            return FALSE;
        }
        viewer->state = 2;
        memcpy(buffer, g_tnt_menu_background_scene,
               sizeof(g_tnt_menu_background_scene));
        return FALSE;
    }

    if (viewer->state == 2) {
        (void)tnt_vita_60hz_delta(FALSE);
        viewer->state = 3;
        memcpy(buffer, g_tnt_menu_background_scene,
               sizeof(g_tnt_menu_background_scene));
        return FALSE;
    }

    if (viewer->state == 3) {
        (void)tnt_vita_60hz_delta(FALSE);
        viewer->state = 4;
        memcpy(buffer, g_tnt_wonder_static_scene,
               sizeof(g_tnt_wonder_static_scene));
        return FALSE;
    }

    if (viewer->state == 0 && (pv_ptr->unk24 & CONT_B)) {
        /* Source state 0 changes to state 7 on this B press. Its renderer then
         * draws only the 2D Wonder page for that frame; teardown/resume follows
         * on the next outer-state iteration. */
        (void)tnt_vita_60hz_delta(FALSE);
        viewer->state = 7;
        memcpy(buffer, g_tnt_wonder_static_scene,
               sizeof(g_tnt_wonder_static_scene));
        return TRUE;
    }

    /* Source state 4 initializes animation 1048 and falls into a full render
     * on the same frame. Animation reset is immediately followed by the current
     * frametime delta, so the first normal frame advances one retrace. */
    delta = tnt_vita_60hz_delta(TRUE);
    if (delta == 0)
        delta = 1;
    viewer->anim_tick = (u16)((viewer->anim_tick + delta) & 511U);
    viewer->state = 0;

    memcpy(buffer, g_tnt_wonder_static_scene,
           sizeof(g_tnt_wonder_static_scene));
    if (TNT_Vita_SramGetTotalWonderLines() >= 500000U) {
        /* Source cur_screen >= 7 uses D_800CFB30[9]: scene 1425 / anim 1426. */
        TNT_Vita_WonderRenderFinaleHall(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, viewer->anim_tick);
    } else {
        /* Resource 1048 used to run through the old 400x300 software triangle
         * rasterizer here, which made Pause -> PROGRESS quantize to ~12 FPS.
         * Its exact 568 triangles + 20 textures are already resident in the
         * shared Wonder GXM subsystem; render the same rotating model there and
         * retain the software path only as a recovery fallback. */
        if (!g_tnt_milestone_gxm_ready ||
            TNT_Vita_WonderProgressGxmRender(
                &buffers[backBuffer].surface, buffers[backBuffer].sync,
                board_x, board_y, cell, viewer->anim_tick) < 0) {
            TNT_Vita_WonderRenderMayan(
                buffer,
                DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                board_x, board_y, cell,
                viewer->anim_tick);
        }
    }
    return FALSE;
}

static void tnt_vita_scores_load_profiles(TNTVitaScoresMenu *menu) {
    int slot;
    menu->profile_count = 0;
    for (slot = 0; slot < TNT_VITA_SRAM_MAX_PLAYERS; slot++) {
        TNTVitaStoredPlayer player;
        u8 i;
        if (TNT_Vita_SramGetPlayer(slot, &player) != 1)
            continue;
        /* The original GUI inserts each decoded SRAM player immediately after
         * the GLOBAL list anchor, so later occupied slots appear first. */
        for (i = menu->profile_count; i > 0; i--) {
            memcpy(menu->profile_names[i], menu->profile_names[i - 1], 9);
            menu->profile_slots[i] = menu->profile_slots[i - 1];
        }
        memcpy(menu->profile_names[0], player.name, 9);
        menu->profile_slots[0] = (u8)slot;
        if (menu->profile_count < TNT_VITA_SRAM_MAX_PLAYERS)
            menu->profile_count++;
    }
}

static const char *tnt_vita_scores_choice_text(const TNTVitaScoresMenu *menu) {
    if (menu->player_choice == 0)
        return "GLOBAL";
    if ((u8)(menu->player_choice - 1) < menu->profile_count)
        return menu->profile_names[menu->player_choice - 1];
    return "GLOBAL";
}

static int tnt_vita_scores_selected_player(
    const TNTVitaScoresMenu *menu, TNTVitaStoredPlayer *player
) {
    u8 index;
    if (menu->player_choice == 0 || player == NULL)
        return 0;
    index = (u8)(menu->player_choice - 1);
    if (index >= menu->profile_count)
        return 0;
    return TNT_Vita_SramGetPlayer(menu->profile_slots[index], player) == 1;
}

static void tnt_vita_scores_build_cache(
    int board_x, int board_y, int cell
) {
    memcpy(g_tnt_wonder_static_scene, g_tnt_menu_background_scene,
           sizeof(g_tnt_wonder_static_scene));
    /* D_800D5180: SINGLE_SHADOW_C at (0,20), centered SCORES panel y=16. */
    tnt_vita_draw_original_ia8_sprite(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        0, 20, 394, 281, g_tnt_scores_shadow_ia8, 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        3, 16, 395, 267,
        g_tnt_scores_panel_ci8, g_tnt_scores_panel_rgba16, 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        50, 63, 50, 34,
        g_tnt_wonder_arrow_left_ci8, g_tnt_wonder_arrow_left_rgba16, 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        300, 63, 49, 33,
        g_tnt_wonder_arrow_right_ci8, g_tnt_wonder_arrow_right_rgba16, 0xFF);
    tnt_vita_draw_original_rgba16_sprite_alpha(
        g_tnt_wonder_static_scene, board_x, board_y, cell,
        200 - (132 >> 1), 269, 132, 15,
        g_tnt_onep_a_accept_b_back_rgba16, 0xFF);
}

static void tnt_vita_scores_enter(
    TNTVitaScoresMenu *menu,
    PlayerVars *pv_ptr,
    int board_x, int board_y, int cell,
    f32 pulse, f32 pulse_rate
) {
    memset(menu, 0, sizeof(*menu));
    menu->active = TRUE;
    menu->focus = 0;
    menu->category = 0;
    menu->entrance_ticks = 31;
    menu->pulse = pulse;
    menu->pulse_rate = pulse_rate;
    menu->fade = 0.0f;
    tnt_vita_scores_load_profiles(menu);
    tnt_vita_scores_build_cache(board_x, board_y, cell);
    g_tnt_60hz_last_us = 0;
    g_tnt_60hz_fraction = 0;
    FUN_026C80_80060ad4_oneliner_calls_fun(&pv_ptr->unk28);
    FUN_026C80_80060b04_twelveliner_loops_32t(
        &pv_ptr->unk28, 0x0F00, 32, 10);
    FUN_026C80_80060b04_twelveliner_loops_32t(
        &pv_ptr->unk28, 0xC000, 16, 16);
    FUN_026C80_nineliner_loops_32t(&pv_ptr->unk28);
}

static int tnt_vita_scores_update(
    TNTVitaScoresMenu *menu, PlayerVars *pv_ptr
) {
    u32 delta = tnt_vita_60hz_delta(TRUE);
    u32 repeat = pv_ptr->unk28.unk84;
    u8 count = (u8)(menu->profile_count + 1U);
    if (delta == 0) delta = 1;

    menu->pulse += menu->pulse_rate;
    if (menu->pulse >= 1.0f) {
        menu->pulse = 1.0f;
        menu->pulse_rate *= -1.0f;
    }
    if (menu->pulse <= 0.6f) {
        menu->pulse = 0.6f;
        menu->pulse_rate *= -1.0f;
    }

    if (tnt_vita_menu_assembly_advance(&menu->entrance_ticks))
        return TNT_SCORES_ACTION_NONE;
    if (menu->leaving) {
        menu->fade -= (1.0f / 9.0f) * delta;
        if (menu->fade <= 0.0f) {
            menu->fade = 0.0f;
            return TNT_SCORES_ACTION_BACK;
        }
        return TNT_SCORES_ACTION_NONE;
    }
    if (menu->fade < 1.0f) {
        menu->fade += (1.0f / 9.0f) * delta;
        if (menu->fade > 1.0f) menu->fade = 1.0f;
    }

    if (menu->name_selecting) {
        /* Original Textbox circular-list walk accepts both vertical D-pad and
         * matching left/right directions, though the on-screen instructions
         * mention UP/DOWN only. */
        if (repeat & (CONT_DOWN | CONT_RIGHT)) {
            menu->player_choice = (u8)((menu->player_choice + 1U) % count);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_SCORES_ACTION_NONE;
        }
        if (repeat & (CONT_UP | CONT_LEFT)) {
            menu->player_choice = (u8)((menu->player_choice + count - 1U) % count);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_SCORES_ACTION_NONE;
        }
        if (pv_ptr->unk24 & CONT_A) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
            menu->saved_choice = menu->player_choice;
            menu->name_selecting = FALSE;
            return TNT_SCORES_ACTION_NONE;
        }
        if (pv_ptr->unk24 & CONT_B) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_5);
            menu->player_choice = menu->saved_choice;
            menu->name_selecting = FALSE;
            return TNT_SCORES_ACTION_NONE;
        }
        return TNT_SCORES_ACTION_NONE;
    }

    if (pv_ptr->unk24 & CONT_B) {
        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_5);
        menu->leaving = TRUE;
        return TNT_SCORES_ACTION_NONE;
    }

    if (menu->focus == 0) {
        if (pv_ptr->unk24 & CONT_LEFT) {
            menu->category = (u8)((menu->category + 3U) % 4U);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_SCORES_ACTION_NONE;
        }
        if (pv_ptr->unk24 & CONT_RIGHT) {
            menu->category = (u8)((menu->category + 1U) % 4U);
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_SCORES_ACTION_NONE;
        }
        if (repeat & (CONT_UP | CONT_DOWN)) {
            menu->focus = 1;
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_SCORES_ACTION_NONE;
        }
    } else {
        if (repeat & (CONT_UP | CONT_DOWN)) {
            menu->focus = 0;
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_1);
            return TNT_SCORES_ACTION_NONE;
        }
        if (pv_ptr->unk24 & CONT_A) {
            Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_4);
            menu->saved_choice = menu->player_choice;
            menu->name_selecting = TRUE;
            return TNT_SCORES_ACTION_NONE;
        }
    }
    return TNT_SCORES_ACTION_NONE;
}

static void tnt_vita_scores_draw_time(
    void *buffer, int board_x, int board_y, int cell,
    int native_right_x, int native_y, u32 seconds, u8 alpha
) {
    char text[20];
    int x;
    tnt_vita_format_original_time(seconds * 60U, text, sizeof(text));
    x = native_right_x - tnt_vita_font_c_text_width_native(text);
    tnt_vita_draw_font_c_text(
        buffer, board_x, board_y, cell, x, native_y, text,
        0xFF, 0xFF, 0xFF, alpha);
}

static void tnt_vita_scores_draw_instructions(
    void *buffer, int board_x, int board_y, int cell, u8 alpha
) {
    int x;
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        60, 152, "PRESS ", 0xFF, 0xFF, 0xFF, alpha);
    x = 60 + tnt_vita_font_c_text_width_native("PRESS ");
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        x, 152, "A ", 0x5A, 0x50, 0xB9, alpha);
    x += tnt_vita_font_c_text_width_native("A ");
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        x, 152, TNTV_ROM_OriginalText(32U), 0xFF, 0xFF, 0xFF, alpha);
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        60, 167, TNTV_ROM_OriginalText(33U), 0xFF, 0xFF, 0xFF, alpha);
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        60, 182, TNTV_ROM_OriginalText(34U), 0xFF, 0xFF, 0xFF, alpha);
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        60, 197, TNTV_ROM_OriginalText(35U), 0xFF, 0xFF, 0xFF, alpha);
    x = 60 + tnt_vita_font_c_text_width_native(TNTV_ROM_OriginalText(35U));
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        x, 197, "A ", 0x5A, 0x50, 0xB9, alpha);
    x += tnt_vita_font_c_text_width_native("A ");
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        x, 197, TNTV_ROM_OriginalText(36U), 0xFF, 0xFF, 0xFF, alpha);
}

static void tnt_vita_scores_draw_general(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaScoresMenu *menu, u8 alpha
) {
    char text[80];
    TNTVitaStoredPlayer player;
    int y = 150;
    if (menu->player_choice == 0) {
        snprintf(text, sizeof(text), "TOTAL WONDER LINES: %u",
            (unsigned)TNT_Vita_SramGetTotalWonderLines());
        tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
            56, y, text, 0xFF, 0xFF, 0xFF, alpha);
        return;
    }
    if (!tnt_vita_scores_selected_player(menu, &player))
        return;

    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        56, y, player.name, 0xFF, 0xFF, 0xFF, alpha);
    snprintf(text, sizeof(text), "%u",
        (unsigned)TNT_Vita_SramRankFor(player.time_in_seconds, player.lines) + 1U);
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        136, y, text, 0xFF, 0xFF, 0xFF, alpha);
    y += 19;
    snprintf(text, sizeof(text), TNTV_ROM_OriginalText(37U), (int)player.unkC4);
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        56, y, text, 0xFF, 0xFF, 0xFF, alpha);
    y += 15;
    snprintf(text, sizeof(text), TNTV_ROM_OriginalText(38U), (int)player.unkC8);
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        56, y, text, 0xFF, 0xFF, 0xFF, alpha);
    y += 15;
    snprintf(text, sizeof(text), TNTV_ROM_OriginalText(39U), (int)player.unkCC);
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        56, y, text, 0xFF, 0xFF, 0xFF, alpha);
    y += 15;
    snprintf(text, sizeof(text), "TOTAL LINES: %u",
        (unsigned)((u32)player.unkC8 + (u32)player.unkCC));
    tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
        56, y, text, 0xFF, 0xFF, 0xFF, alpha);
}

static void tnt_vita_scores_draw_mode(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaScoresMenu *menu, u8 alpha
) {
    char text[32];
    int row;
    int y = 152;
    u8 game_type = (u8)(menu->category - 1U);
    int rank_x = game_type == GAMETYPE_ULTRA ? 146 : 136;
    int lines_x = game_type == GAMETYPE_ULTRA ? 216 : 206;
    int time_right_x = game_type == GAMETYPE_ULTRA ? 336 : 326;

    if (menu->player_choice == 0) {
        for (row = 0; row < 5; row++) {
            TNTVitaGlobalScore score;
            if (TNT_Vita_SramGetGlobalScore(game_type, row, &score) != 0)
                continue;
            if (score.lines == 0 || score.time_in_seconds == 0)
                continue;
            tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
                56, y, score.name, 0xFF, 0xFF, 0xFF, alpha);
            snprintf(text, sizeof(text), "%u",
                (unsigned)TNT_Vita_SramRankFor(
                    score.rank_time_in_seconds, score.rank_lines) + 1U);
            tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
                rank_x, y, text, 0xFF, 0xFF, 0xFF, alpha);
            snprintf(text, sizeof(text), "%u", (unsigned)score.lines);
            tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
                lines_x, y, text, 0xFF, 0xFF, 0xFF, alpha);
            tnt_vita_scores_draw_time(buffer, board_x, board_y, cell,
                time_right_x, y, score.time_in_seconds, alpha);
            y += 18;
        }
        return;
    }

    {
        TNTVitaStoredPlayer player;
        uint32_t (*scores)[5];
        if (!tnt_vita_scores_selected_player(menu, &player))
            return;
        if (game_type == GAMETYPE_MARATHON)
            scores = player.marathon_scores;
        else if (game_type == GAMETYPE_SPRINT)
            scores = player.sprint_scores;
        else
            scores = player.ultra_scores;
        for (row = 0; row < 5; row++) {
            u32 lines = scores[0][row] + scores[1][row];
            u32 seconds = scores[2][row];
            if (lines == 0 || seconds == 0)
                continue;
            tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
                56, y, player.name, 0xFF, 0xFF, 0xFF, alpha);
            snprintf(text, sizeof(text), "%u",
                (unsigned)TNT_Vita_SramRankFor((u16)seconds, (u16)lines) + 1U);
            tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
                rank_x, y, text, 0xFF, 0xFF, 0xFF, alpha);
            snprintf(text, sizeof(text), "%u", (unsigned)lines);
            tnt_vita_draw_font_c_text(buffer, board_x, board_y, cell,
                lines_x, y, text, 0xFF, 0xFF, 0xFF, alpha);
            tnt_vita_scores_draw_time(buffer, board_x, board_y, cell,
                time_right_x, y, seconds, alpha);
            y += 18;
        }
    }
}

static void tnt_vita_scores_render_uncached(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaScoresMenu *menu,
    u8 suppress_pulse
) {
    static const char *const categories[4] = {
        "GENERAL", "MARATHON", "SPRINT", "ULTRA"
    };
    static const int category_x[4] = {144, 132, 157, 159};
    u8 global_alpha;
    u8 category_alpha;
    u8 selector_alpha;
    u8 cg = 0xFF, cb = 0xFF;
    u8 sg = 0xFF, sb = 0xFF;

    tnt_vita_frontend_copy_shared_cache(
        buffer, board_x, board_y, cell,
        menu->entrance_ticks, menu->fade, 0, 16, 400, 301);
    if (menu->entrance_ticks != 0)
        return;
    global_alpha = (u8)(menu->fade * 255.0f);

    category_alpha = (u8)(((u32)0xA0 * global_alpha + 127U) / 255U);
    selector_alpha = category_alpha;
    if (!suppress_pulse && menu->focus == 0) {
        u8 pulse = (u8)(menu->pulse * 255.0f);
        cg = pulse;
        cb = pulse;
        category_alpha = (u8)(((u32)pulse * global_alpha + 127U) / 255U);
    } else if (!suppress_pulse) {
        u8 pulse = (u8)(menu->pulse * 255.0f);
        sg = pulse;
        sb = pulse;
        selector_alpha = (u8)(((u32)pulse * global_alpha + 127U) / 255U);
    }

    TNT_Vita_FontA_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell,
        category_x[menu->category], 66, categories[menu->category],
        0xFF, cg, cb, category_alpha);

    /* D_800D5014: IMG_SELECTION_BOX at (123,96), text at +(5,8).
     * State 0xFC keeps it white while inactive; focus uses the GUI red pulse. */
    tnt_vita_draw_original_ia8_sprite_tinted(
        buffer, board_x, board_y, cell,
        123, 96, 154, 30, g_tnt_onep_selection_box_ia8,
        0xFF, sg, sb, selector_alpha);
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell,
        128, 104, tnt_vita_scores_choice_text(menu),
        0xFF, sg, sb, selector_alpha);

    if (menu->player_choice != 0) {
        TNTVitaStoredPlayer player;
        if (tnt_vita_scores_selected_player(menu, &player)) {
            char rank_text[4];
            u8 rank = (u8)(TNT_Vita_SramRankFor(
                player.time_in_seconds, player.lines) + 1U);
            tnt_vita_draw_original_ia8_sprite(
                buffer, board_x, board_y, cell,
                251, 99, 32, 26, g_tnt_scores_gamecart_ia8, global_alpha);
            snprintf(rank_text, sizeof(rank_text), "%u", (unsigned)rank);
            TNT_Vita_FontB_DrawText(
                buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                board_x, board_y, cell,
                rank >= 10 ? 256 : 260, 104, rank_text,
                0xFF, 0xFF, 0x00, global_alpha);
        }
    }

    /* GUI_WINDOW callbacks are invoked only for the focused row. */
    if (menu->focus == 0) {
        if (menu->category == 0)
            tnt_vita_scores_draw_general(
                buffer, board_x, board_y, cell, menu, global_alpha);
        else
            tnt_vita_scores_draw_mode(
                buffer, board_x, board_y, cell, menu, global_alpha);
    } else {
        tnt_vita_scores_draw_instructions(
            buffer, board_x, board_y, cell, global_alpha);
    }
}

static void tnt_vita_scores_render(
    void *buffer, int board_x, int board_y, int cell,
    const TNTVitaScoresMenu *menu
) {
    static const char *const categories[4] = {
        "GENERAL", "MARATHON", "SPRINT", "ULTRA"
    };
    static const int category_x[4] = {144, 132, 157, 159};
    u32 key;
    u8 pulse;

    if (menu->entrance_ticks != 0 || menu->fade < 1.0f || menu->leaving) {
        tnt_vita_scores_render_uncached(
            buffer, board_x, board_y, cell, menu, FALSE);
        return;
    }

    key = (u32)menu->category |
          ((u32)menu->player_choice << 3) |
          ((u32)menu->focus << 10);
    if (g_tnt_frontend_cache_owner != TNT_FRONTEND_CACHE_SCORES ||
        g_tnt_frontend_cache_key != key) {
        TNTVitaScoresMenu cached = *menu;
        cached.pulse = 1.0f;
        cached.fade = 1.0f;
        cached.entrance_ticks = 0;
        cached.leaving = FALSE;
        tnt_vita_scores_render_uncached(
            g_tnt_onep_settled_scene, board_x, board_y, cell,
            &cached, TRUE);
        g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_SCORES;
        g_tnt_frontend_cache_key = key;
    }

    memcpy(buffer, g_tnt_onep_settled_scene,
           sizeof(g_tnt_onep_settled_scene));
    pulse = (u8)(menu->pulse * 255.0f);

    if (menu->focus == 0) {
        tnt_vita_restore_native_rect_from_scene(
            buffer, g_tnt_wonder_static_scene,
            board_x, board_y, cell, 105, 60, 295, 96);
        TNT_Vita_FontA_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell,
            category_x[menu->category], 66, categories[menu->category],
            0xFF, pulse, pulse, pulse);
        return;
    }

    /* Restore the complete name-selector region, including the optional
     * game-cart/rank badge which overlaps the right edge of the selection
     * box, then redraw the focused selector exactly once. */
    tnt_vita_restore_native_rect_from_scene(
        buffer, g_tnt_wonder_static_scene,
        board_x, board_y, cell, 118, 94, 290, 132);
    tnt_vita_draw_original_ia8_sprite_tinted(
        buffer, board_x, board_y, cell,
        123, 96, 154, 30, g_tnt_onep_selection_box_ia8,
        0xFF, pulse, pulse, pulse);
    TNT_Vita_FontB_DrawText(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        board_x, board_y, cell,
        128, 104, tnt_vita_scores_choice_text(menu),
        0xFF, pulse, pulse, pulse);

    if (menu->player_choice != 0) {
        TNTVitaStoredPlayer player;
        if (tnt_vita_scores_selected_player(menu, &player)) {
            char rank_text[4];
            u8 rank = (u8)(TNT_Vita_SramRankFor(
                player.time_in_seconds, player.lines) + 1U);
            tnt_vita_draw_original_ia8_sprite(
                buffer, board_x, board_y, cell,
                251, 99, 32, 26, g_tnt_scores_gamecart_ia8, 0xFF);
            snprintf(rank_text, sizeof(rank_text), "%u", (unsigned)rank);
            TNT_Vita_FontB_DrawText(
                buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                board_x, board_y, cell,
                rank >= 10 ? 256 : 260, 104, rank_text,
                0xFF, 0xFF, 0x00, 0xFF);
        }
    }
}

static void tnt_vita_tally_build_cache(
    int board_x,
    int board_y,
    int cell
) {
    memcpy(
        g_tnt_tally_static_scene,
        g_tnt_menu_background_scene,
        sizeof(g_tnt_tally_static_scene));

    /* D_800D6010/D_800D5FE8: TALLY_SHADOW at (0,40), centered TALLY
     * panel at y=20, and centered A_ACCEPT at y=262. */
    tnt_vita_draw_original_ia8_sprite(
        g_tnt_tally_static_scene, board_x, board_y, cell,
        0, 40, 390, 260, g_tnt_tally_shadow_ia8, 0xFF);
    tnt_vita_draw_original_ci8_sprite_alpha(
        g_tnt_tally_static_scene, board_x, board_y, cell,
        (400 - 398) / 2, 20, 398, 251,
        g_tnt_tally_panel_ci8, g_tnt_tally_panel_rgba16, 0xFF);
    tnt_vita_draw_original_rgba16_sprite_alpha(
        g_tnt_tally_static_scene, board_x, board_y, cell,
        (400 - 71) / 2, 262, 71, 15,
        g_tnt_main_menu_a_accept_rgba16, 0xFF);
}

static void tnt_vita_tally_enter(
    TNTVitaTally *tally,
    int board_x,
    int board_y,
    int cell,
    const char *player_name,
    u8 rank,
    u32 lines,
    u32 elapsed_seconds
) {
    memset(tally, 0, sizeof(*tally));
    tally->active = TRUE;
    tally->exit_delay = 2;
    tally->rank = rank != 0 ? rank : 1;
    tally->player_count = 1;
    tally->winner_mask = 1U;
    tally->rank2 = 1;
    tally->lines = lines;
    tally->elapsed_seconds = elapsed_seconds;
    tally->award_lines = lines;
    snprintf(tally->player_name, sizeof(tally->player_name), "%s", player_name);
    tally->fade = 0.0f;
    g_tnt_60hz_last_us = 0;
    g_tnt_60hz_fraction = 0;
}

static void tnt_vita_tally_enter_opponent(
    TNTVitaTally *tally,
    int board_x,
    int board_y,
    int cell,
    const char *human_name,
    const char *opponent_name,
    u8 human_rank,
    u8 opponent_display_rank,
    u32 human_lines,
    u32 human_elapsed_seconds,
    u32 guest_lines,
    u32 guest_elapsed_seconds,
    u8 winner_mask,
    u32 award_lines
) {
    tnt_vita_tally_enter(
        tally, board_x, board_y, cell, human_name, human_rank,
        human_lines, human_elapsed_seconds);
    tally->player_count = 2;
    tally->winner_mask = winner_mask;
    tally->rank2 = opponent_display_rank != 0 ? opponent_display_rank : 1;
    tally->lines2 = guest_lines;
    tally->elapsed_seconds2 = guest_elapsed_seconds;
    tally->award_lines = award_lines;
    snprintf(tally->player_name2, sizeof(tally->player_name2), "%s", opponent_name);
}

/* Test78: real N64 three-player tally. Source GameResults stores one row
 * per player; no multiplayer stats are written to persistent SRAM in TNTV. */
static void __attribute__((optimize("Os"))) tnt_vita_tally_enter_three(
    TNTVitaTally *tally, int board_x, int board_y, int cell,
    const char *p1_name, const char *p2_name, const char *p3_name,
    const u8 ranks[3], const u32 lines[3], const u32 seconds[3],
    u8 winner_mask, u32 award_lines
) {
    tnt_vita_tally_enter_opponent(tally, board_x, board_y, cell,
        p1_name, p2_name, ranks[0], ranks[1],
        lines[0], seconds[0], lines[1], seconds[1],
        winner_mask, award_lines);
    tally->player_count = 3;
    tally->rank3 = ranks[2] != 0 ? ranks[2] : 1;
    tally->lines3 = lines[2];
    tally->elapsed_seconds3 = seconds[2];
    snprintf(tally->player_name3, sizeof(tally->player_name3),
        "%s", p3_name);
}

/* Test92: one extra retail tally row; preserve existing 1P, 2P and 3P
 * entry points and exact 25-source-pixel row spacing. */
static void __attribute__((optimize("Os"))) tnt_vita_tally_enter_four(
    TNTVitaTally *tally,int board_x,int board_y,int cell,
    const char *const names[4],const u8 ranks[4],
    const u32 lines[4],const u32 seconds[4],
    u8 winner_mask,u32 award_lines
) {
    tnt_vita_tally_enter_three(
        tally,board_x,board_y,cell,
        names[0],names[1],names[2],
        ranks,lines,seconds,winner_mask,award_lines);
    tally->player_count=4;
    tally->rank4=ranks[3]?ranks[3]:1;
    tally->lines4=lines[3];
    tally->elapsed_seconds4=seconds[3];
    snprintf(tally->player_name4,sizeof(tally->player_name4),
        "%s",names[3]);
}

static int tnt_vita_tally_update(
    TNTVitaTally *tally,
    PlayerVars *pv_ptr
) {
    u8 a_pressed = (pv_ptr->unk24 & CONT_A) != 0;
    u32 retrace_delta = tnt_vita_60hz_delta(TRUE);

    if (retrace_delta == 0)
        retrace_delta = 1;
    if (tally->fade < 1.0f) {
        tally->fade += (1.0f / 9.0f) * retrace_delta;
        if (tally->fade > 1.0f)
            tally->fade = 1.0f;
    }

    /* Original tally A-button behavior advances to the next 30-frame reveal
     * boundary; only a later A press, after the counter is already >120,
     * exits the results screen. */
    if (!tally->leaving && tally->reveal_ticks < 120 && a_pressed) {
        if (tally->reveal_ticks < 30)
            tally->reveal_ticks = 30;
        else if (tally->reveal_ticks < 60)
            tally->reveal_ticks = 60;
        else if (tally->reveal_ticks < 90)
            tally->reveal_ticks = 90;
        else
            tally->reveal_ticks = 120;
    }

    if (!tally->leaving) {
        if (tally->idle_ticks < 3000)
            tally->idle_ticks++;
        if (tally->idle_ticks >= 3000 ||
            (a_pressed && tally->reveal_ticks > 120)) {
            tally->idle_ticks = 3000;
            tally->leaving = TRUE;
        }
    }

    if (tally->leaving) {
        if (tally->exit_delay == 0)
            return TNT_TALLY_ACTION_RETURN_ONE_PLAYER;
        tally->exit_delay--;
    }

    if (tally->reveal_ticks < 0xFFFF)
        tally->reveal_ticks++;
    return TNT_TALLY_ACTION_NONE;
}

static void tnt_vita_tally_copy_cached_frame(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    const TNTVitaTally *tally
) {
    uint32_t *dst = (uint32_t *)buffer;
    int step;

    if (tally->fade <= 0.0f) {
        memcpy(dst, g_tnt_menu_background_scene,
               sizeof(g_tnt_menu_background_scene));
        return;
    }
    if (tally->fade >= 1.0f) {
        memcpy(dst, g_tnt_tally_static_scene, sizeof(g_tnt_tally_static_scene));
        return;
    }

    memcpy(dst, g_tnt_menu_background_scene,
           sizeof(g_tnt_menu_background_scene));
    step = (int)(tally->fade * 9.0f + 0.5f);
    if (step < 1) step = 1;
    if (step > 8) step = 8;

    {
        int dx0 = board_x + tnt_floor_div_s64((s64)(0 - 83) * cell, 11);
        int dy0 = board_y + tnt_floor_div_s64((s64)(20 - 36) * cell, 11);
        int dx1 = board_x + tnt_floor_div_s64((s64)(400 - 83) * cell + 10, 11);
        int dy1 = board_y + tnt_floor_div_s64((s64)(300 - 36) * cell + 10, 11);
        int y;

        if (dx0 < 0) dx0 = 0;
        if (dy0 < 0) dy0 = 0;
        if (dx1 > DISPLAY_WIDTH) dx1 = DISPLAY_WIDTH;
        if (dy1 > DISPLAY_HEIGHT) dy1 = DISPLAY_HEIGHT;

        for (y = dy0; y < dy1; y++) {
            int x;
            uint32_t *dst_row = dst + y * DISPLAY_STRIDE;
            const uint32_t *bg_row =
                g_tnt_menu_background_scene + y * DISPLAY_STRIDE;
            const uint32_t *fg_row =
                g_tnt_tally_static_scene + y * DISPLAY_STRIDE;

            for (x = dx0; x < dx1; x++) {
                uint32_t bg = bg_row[x];
                uint32_t fg = fg_row[x];
                int br, bgc, bb, fr, fgc, fb;
                int r, g, b;
                if (bg == fg)
                    continue;
                br = (int)(bg & 0xFFU);
                bgc = (int)((bg >> 8) & 0xFFU);
                bb = (int)((bg >> 16) & 0xFFU);
                fr = (int)(fg & 0xFFU);
                fgc = (int)((fg >> 8) & 0xFFU);
                fb = (int)((fg >> 16) & 0xFFU);
                r = br + ((fr - br) * step) / 9;
                g = bgc + ((fgc - bgc) * step) / 9;
                b = bb + ((fb - bb) * step) / 9;
                dst_row[x] = COLOR((u8)r, (u8)g, (u8)b);
            }
        }
    }
}

static void tnt_vita_tally_render_uncached(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    const TNTVitaTally *tally
) {
    char text[60];
    int x;
    u8 rows = tally->player_count >= 4 ? 4 :
              (tally->player_count >= 3 ? 3 :
               (tally->player_count >= 2 ? 2 : 1));
    int i;

    tnt_vita_tally_copy_cached_frame(
        buffer, board_x, board_y, cell, tally);

    /* Source tally rows begin at native y=90 and are spaced by 25 pixels.
     * Font B's baseline is y+4 for name/rank and y+3 for score/time. */
    for (i = 0; i < rows; i++) {
        const char *name = i == 0 ? tally->player_name :
            (i == 1 ? tally->player_name2 :
             (i == 2 ? tally->player_name3 : tally->player_name4));
        u8 rank = i == 0 ? tally->rank :
            (i == 1 ? tally->rank2 :
             (i == 2 ? tally->rank3 : tally->rank4));
        u32 lines = i == 0 ? tally->lines :
            (i == 1 ? tally->lines2 :
             (i == 2 ? tally->lines3 : tally->lines4));
        u32 seconds = i == 0 ? tally->elapsed_seconds :
            (i == 1 ? tally->elapsed_seconds2 :
             (i == 2 ? tally->elapsed_seconds3 : tally->elapsed_seconds4));
        int row_y = 90 + (25 * i);

        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, 46, row_y + 4, name,
            0xFF, 0xFF, 0xFF, 0xFF);

        snprintf(text, sizeof(text), "%u", (unsigned)(rank ? rank : 1));
        x = 166 - (TNT_Vita_FontB_TextWidth(text) / 2);
        TNT_Vita_FontB_DrawText(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, x, row_y + 4, text,
            0xFF, 0xFF, 0xFF, 0xFF);

        if (tally->reveal_ticks > 30) {
            snprintf(text, sizeof(text), "%u", (unsigned)lines);
            x = 240 - (TNT_Vita_FontB_TextWidth(text) / 2);
            TNT_Vita_FontB_DrawText(
                buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                board_x, board_y, cell, x, row_y + 3, text,
                0xFF, 0xFF, 0xFF, 0xFF);
            tnt_vita_draw_original_time_font_b(
                buffer, board_x, board_y, cell,
                333, row_y + 3, seconds * 60U);
        }

        if (rows > 1 && tally->reveal_ticks > 60 &&
            (tally->winner_mask & (1U << i))) {
            /* Retail tally.c calls IMG_INVISIBLE_TROPHY here. The exact 28x30
             * RGBA16 asset has A=0 on every texel, and original-N64 capture
             * confirms no visible trophy appears. Keep the source draw call
             * semantics rather than inventing a visible winner icon. */
            tnt_vita_draw_original_rgba16_sprite_alpha(
                buffer, board_x, board_y, cell,
                12, row_y, 28, 30, g_tnt_tally_trophy_rgba16, 0xFF);
        }
    }

    if (tally->reveal_ticks > 120 && tally->award_lines != 0) {
        u8 winners = 0;
        for (i = 0; i < rows; i++)
            if (tally->winner_mask & (1U << i)) winners++;

        if (winners == 0)
            return;

        if (rows > 1 && winners > 1) {
            snprintf(text, sizeof(text),
                tally->award_lines == 1 ? "WINNERS GET %u LINE" : "WINNERS GET %u LINES",
                (unsigned)tally->award_lines);
            TNT_Vita_FontB_DrawText(
                buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                board_x, board_y, cell, 62, 198, text,
                0xFF, 0xFF, 0xFF, 0xFF);
        } else {
            const char *winner_name = tally->player_name;
            if (rows >= 4 && (tally->winner_mask & 8U))
                winner_name = tally->player_name4;
            else if (rows >= 3 && (tally->winner_mask & 4U))
                winner_name = tally->player_name3;
            else if (rows > 1 && (tally->winner_mask & 2U))
                winner_name = tally->player_name2;
            snprintf(text, sizeof(text),
                tally->award_lines == 1 ? "%s WINS %u LINE" : "%s WINS %u LINES",
                winner_name, (unsigned)tally->award_lines);
            TNT_Vita_FontB_DrawText(
                buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                board_x, board_y, cell, 62, 195, text,
                0xFF, 0xFF, 0xFF, 0xFF);
        }
    }
}

static u32 tnt_vita_tally_cache_key(const TNTVitaTally *tally) {
    u32 h = 2166136261U;
    const char *p;
    u32 stage = tally->reveal_ticks > 120 ? 3U :
        (tally->reveal_ticks > 60 ? 2U : (tally->reveal_ticks > 30 ? 1U : 0U));
    for (p = tally->player_name; *p != '\0'; p++) { h ^= (u8)*p; h *= 16777619U; }
    for (p = tally->player_name2; *p != '\0'; p++) { h ^= (u8)*p; h *= 16777619U; }
    if (tally->player_count >= 3) {
        for (p = tally->player_name3; *p != '\0'; p++) {
            h ^= (u8)*p; h *= 16777619U;
        }
        h ^= tally->rank3; h *= 16777619U;
        h ^= tally->lines3; h *= 16777619U;
        h ^= tally->elapsed_seconds3; h *= 16777619U;
    }
    if (tally->player_count >= 4) {
        for (p = tally->player_name4; *p != '\0'; p++) {
            h ^= (u8)*p; h *= 16777619U;
        }
        h ^= tally->rank4; h *= 16777619U;
        h ^= tally->lines4; h *= 16777619U;
        h ^= tally->elapsed_seconds4; h *= 16777619U;
    }
    h ^= stage; h *= 16777619U;
    h ^= tally->player_count; h *= 16777619U;
    h ^= tally->winner_mask; h *= 16777619U;
    h ^= tally->rank; h *= 16777619U;
    h ^= tally->rank2; h *= 16777619U;
    h ^= tally->lines; h *= 16777619U;
    h ^= tally->lines2; h *= 16777619U;
    h ^= tally->elapsed_seconds; h *= 16777619U;
    h ^= tally->elapsed_seconds2; h *= 16777619U;
    h ^= tally->award_lines; h *= 16777619U;
    return h;
}

static void tnt_vita_tally_render(
    void *buffer,
    int board_x,
    int board_y,
    int cell,
    const TNTVitaTally *tally
) {
    u32 key;

    if (tally->fade < 1.0f) {
        tnt_vita_tally_render_uncached(
            buffer, board_x, board_y, cell, tally);
        return;
    }

    key = tnt_vita_tally_cache_key(tally);
    if (g_tnt_frontend_cache_owner != TNT_FRONTEND_CACHE_TALLY ||
        g_tnt_frontend_cache_key != key) {
        TNTVitaTally cached = *tally;
        cached.fade = 1.0f;
        tnt_vita_tally_render_uncached(
            g_tnt_onep_settled_scene, board_x, board_y, cell, &cached);
        g_tnt_frontend_cache_owner = TNT_FRONTEND_CACHE_TALLY;
        g_tnt_frontend_cache_key = key;
    }

    memcpy(buffer, g_tnt_onep_settled_scene,
           sizeof(g_tnt_onep_settled_scene));
}

static void tnt_vita_prepare_opponent_background(
    u8 screen, int board_x, int board_y, int cell
) {
    /* Background is recomposed on stage/match entry: invalidate both
     * display buffers before permitting partial restoration. */
    memset(g_tnt_opp60_scene_buffer_ready, 0,
           sizeof(g_tnt_opp60_scene_buffer_ready));
    memset(g_tnt_opp60_dirty, 0, sizeof(g_tnt_opp60_dirty));
    memset(g_tnt_opp60_dirty_full, 0, sizeof(g_tnt_opp60_dirty_full));
    g_tnt_opp60_tracking = FALSE;
    u8 culture = screen < TNT_OPPONENT_2P_CULTURE_COUNT
        ? screen : (TNT_OPPONENT_2P_CULTURE_COUNT - 1);
    const TNTOpponent2PBackground *bg =
        &g_tnt_opponent_2p_backgrounds[culture];
    u8 *decoded = (u8 *)g_tnt_gameplay_base_scene;

    clear_screen(g_tnt_static_scene, COLOR(0, 0, 0));
    tnt_vita_decode_packbits(bg->packbits, bg->packbits_size,
        decoded, 400U * 300U);
    tnt_vita_draw_original_ci8_sprite(
        g_tnt_static_scene, board_x, board_y, cell,
        0, 0, 400, 300, decoded, bg->palette);
}

/* Source FUN_027BF0_Init maps both 3P and 4P to the same culturally
 * indexed 400x300 N64 artwork, D_800D0E30[2*screen]. Stage 7 maps
 * to Russian. The prior TNTV implementation always forced Mayan.
 * Decode once per game into scratch before drawing into static scene;
 * the existing high-performance partial restore remains unchanged. */
#ifndef TNTV_MP_CULTURE_TEST_CYCLE
#define TNTV_MP_CULTURE_TEST_CYCLE 0
#endif
#if TNTV_MP_CULTURE_TEST_CYCLE != 0 && TNTV_MP_CULTURE_TEST_CYCLE != 1
#error "TNTV_MP_CULTURE_TEST_CYCLE must be 0 or 1"
#endif

static void __attribute__((optimize("Os"))) tnt_vita_prepare_3p_culture_background(
    u8 screen, int board_x, int board_y, int cell
) {
    u8 culture = screen < TNT_4P_CULTURE_COUNT
        ? screen : (TNT_4P_CULTURE_COUNT - 1U);
    const TNT4PCultureBackground *bg = &g_tnt_4p_culture_backgrounds[culture];
    u8 *decoded = (u8 *)g_tnt_gameplay_base_scene;
    tnt_vita_decode_packbits(
        bg->packbits, bg->packbits_size, decoded, 400U * 300U);
    clear_screen(g_tnt_static_scene, COLOR(0, 0, 0));
    tnt_vita_draw_original_ci8_sprite(
        g_tnt_static_scene, board_x, board_y, cell,
        0, 0, 400, 300, decoded, bg->palette);
}

static void __attribute__((optimize("Os"))) tnt_vita_compose_3p_names(
    int board_x, int board_y, int cell,
    const char *p1_name, const char *p2_name, const char *p3_name
) {
    /* Exact retail 3P Font C positions/colors from FUN_027BF0 render path. */
    tnt_vita_draw_font_c_text(
        g_tnt_static_scene, board_x, board_y, cell,
        39, 234, p1_name, 0xFF, 0x00, 0x00, 0xFF);
    tnt_vita_draw_font_c_text(
        g_tnt_static_scene, board_x, board_y, cell,
        123, 234, p2_name, 0x00, 0xFF, 0x00, 0xFF);
    tnt_vita_draw_font_c_text(
        g_tnt_static_scene, board_x, board_y, cell,
        207, 234, p3_name, 0x00, 0x00, 0xFF, 0xFF);
}

static void tnt_vita_compose_opponent_static_hud(
    void *buffer, int board_x, int board_y, int cell,
    u8 screen, u8 game_type,
    const char *human_name, const char *opponent_name
) {
    u8 culture = screen < TNT_OPPONENT_2P_CULTURE_COUNT
        ? screen : (TNT_OPPONENT_2P_CULTURE_COUNT - 1);

    /* Names and the time-panel frame never change during a match. Compose
     * them once into the already-cached 2P background instead of rerasterizing
     * software Font C every frame. This is source-identical presentation; only
     * the time digits remain dynamic. */
    tnt_vita_draw_font_c_text(
        buffer, board_x, board_y, cell,
        169, 200, human_name, 0xFF, 0, 0, 0xFF);
    tnt_vita_draw_font_c_text(
        buffer, board_x, board_y, cell,
        169, 231, opponent_name, 0, 0xFF, 0, 0xFF);

    if (game_type == GAMETYPE_SPRINT || game_type == GAMETYPE_ULTRA) {
        const TNTOpponent2PTimePanel *panel =
            &g_tnt_opponent_2p_time_panels[culture];
        tnt_vita_draw_original_ci8_sprite_alpha(
            buffer, board_x, board_y, cell,
            170, 246, panel->width, panel->height,
            panel->ci8, panel->palette, 0xFF);
        if (game_type == GAMETYPE_ULTRA)
            tnt_vita_draw_font_c_text(
                buffer, board_x, board_y, cell,
                190, 279, "150", 0xFF, 0xFF, 0xFF, 0xFF);
    }
}

static void tnt_vita_render_opponent_hud_dynamic(
    void *buffer, int board_x, int board_y, int cell,
    u8 game_type, u32 elapsed_ticks
) {
    if (game_type == GAMETYPE_SPRINT) {
        u32 remaining = elapsed_ticks < 10800U
            ? 10800U - elapsed_ticks : 0U;
        tnt_vita_draw_original_time(
            buffer, board_x, board_y, cell, 220, 261, remaining);
    } else if (game_type == GAMETYPE_ULTRA) {
        tnt_vita_draw_original_time(
            buffer, board_x, board_y, cell, 220, 261, elapsed_ticks);
    }
}

static void tnt_vita_render_tetris_player(
    void *buffer,
    Tetris *tetris_ptr,
    u8 player_index,
    int playfield_x,
    int playfield_y,
    int playfield_cell,
    int screen_board_x,
    int screen_board_y,
    int screen_cell
) {
    u64 locked_start = g_tnt_3p_prof_active
        ? sceKernelGetProcessTimeWide() : 0;
    u8 prof222 = (u8)(g_tnt_fps_counter_enabled &&
                       g_tnt_opp60_tracking && player_index < 2);
    u64 t222 = prof222 ? sceKernelGetProcessTimeWide() : 0;
    TNT_Vita_TetrisSetGlobalPointers(tetris_ptr, player_index);
    g_tnt_compact_playfield_active = (u8)(g_playercount >= 3);
    g_tnt_opp60_exact_board =
        (u8)(g_tnt_opp60_tracking && g_playercount == 2);
    if (g_tnt_opp60_exact_board) {
        int phase_x = (tetris_ptr->boardInfo.location.x - 83) *
                      TNT_PRESENT_CELL;
        int phase_y = (tetris_ptr->boardInfo.location.y - 36) *
                      TNT_PRESENT_CELL;
        g_tnt_opp60_phase_x = (u8)((phase_x % 11 + 11) % 11);
        g_tnt_opp60_phase_y = (u8)((phase_y % 11 + 11) % 11);
    }

    for (int by = 0; by < 20; by++) {
        for (int bx = 0; bx < 10; bx++) {
            Cell *board_cell =
                g_boardPieces_ptr->cell_ptr_arr[bx + (by * 10)];
            if (board_cell->piece_type != EMPTY_CELL &&
                !TNT_Vita_LineEffectCellHidden(
                    &tetris_ptr->lineScan, board_cell)) {
                Color *locked_tnt_color = BoardP_8006d3c0_fourliner(board_cell);
                s32 locked_x_fixed = bx << 8;
                s32 locked_y_fixed = by << 8;

                if (board_cell->cubeInfo.cube_ptr != NULL) {
                    locked_tnt_color = &board_cell->cubeInfo.cube_ptr->mino.unk4;
                    locked_x_fixed = board_cell->cubeInfo.cube_ptr->unk2.unk6.p.x;
                    locked_y_fixed = board_cell->cubeInfo.cube_ptr->unk2.unk6.p.y;
                    int px0 = tnt_vita_opp60_edge(locked_x_fixed, playfield_x, playfield_cell, FALSE);
                    int py0 = tnt_vita_opp60_edge(locked_y_fixed, playfield_y, playfield_cell, TRUE);
                    int cw = tnt_vita_opp60_edge(locked_x_fixed + 0x100, playfield_x, playfield_cell, FALSE) - px0;
                    int ch = tnt_vita_opp60_edge(locked_y_fixed + 0x100, playfield_y, playfield_cell, TRUE) - py0;
                    draw_cube_mino_textured(
                        buffer, px0, py0, cw, ch,
                        board_cell->cubeInfo.cube_ptr);
                } else {
                    int px0 = tnt_vita_opp60_edge(locked_x_fixed, playfield_x, playfield_cell, FALSE);
                    int py0 = tnt_vita_opp60_edge(locked_y_fixed, playfield_y, playfield_cell, TRUE);
                    draw_rect(
                        buffer, px0, py0,
                        tnt_vita_opp60_edge(locked_x_fixed + 0x100, playfield_x, playfield_cell, FALSE) - px0,
                        tnt_vita_opp60_edge(locked_y_fixed + 0x100, playfield_y, playfield_cell, TRUE) - py0,
                        COLOR(
                            tnt_color_channel_to_u8(locked_tnt_color->diffuse_red),
                            tnt_color_channel_to_u8(locked_tnt_color->diffuse_green),
                            tnt_color_channel_to_u8(locked_tnt_color->diffuse_blue)));
                }
            }
        }
    }

    if (g_fallingCubes_ptr != NULL &&
        g_fallingCubes_ptr->cube_ptr_list != NULL) {
        for (int fi = 0; fi < g_fallingCubes_ptr->max_allowed; fi++) {
            Cube *falling_cube = g_fallingCubes_ptr->cube_ptr_list[fi];
            if (falling_cube != NULL) {
                s32 fx = falling_cube->unk2.unk6.p.x;
                s32 fy = falling_cube->unk2.unk6.p.y;
                int px0 = tnt_vita_opp60_edge(fx, playfield_x, playfield_cell, FALSE);
                int py0 = tnt_vita_opp60_edge(fy, playfield_y, playfield_cell, TRUE);
                draw_cube_mino_textured(
                    buffer, px0, py0,
                    tnt_vita_opp60_edge(fx + 0x100, playfield_x, playfield_cell, FALSE) - px0,
                    tnt_vita_opp60_edge(fy + 0x100, playfield_y, playfield_cell, TRUE) - py0,
                    falling_cube);
            }
        }
    }

    if (g_tnt_3p_prof_active && player_index < 3)
        g_tnt_3p_prof_locked_frame[player_index] =
            (u32)(sceKernelGetProcessTimeWide() - locked_start);

    if (prof222) {
        g_tnt_opp60_locked_us[player_index] =
            (u32)(sceKernelGetProcessTimeWide()-t222);
        t222 = sceKernelGetProcessTimeWide();
    }
    if (!tnt_vita_3p_ultra_hp_line_tile_enabled(player_index)) {
        char line_text[16];
        snprintf(line_text, sizeof(line_text), "%u",
            (unsigned)getLineCount(&tetris_ptr->gameStats.linesInfo));
        tnt_vita_draw_font_c_text(
            buffer, screen_board_x, screen_board_y, screen_cell,
            tetris_ptr->gameStats.linesInfo.x,
            tetris_ptr->gameStats.linesInfo.y,
            line_text, 0xFF, 0xFF, 0xFF, 0xFF);
    }

    /* This row-blend scratch path is already used in v1.0 Marathon.
     * Enable it for Opponent preview minos ONLY; this replaces repeated
     * uncached per-pixel CDRAM read/writes with one row read and write. */
    u8 saved_next_scratch = g_tnt113_next_scratch_active;
    if (g_tnt_opp60_exact_board) g_tnt113_next_scratch_active = TRUE;
    for (int ni = 0; ni < 4; ni++) {
        NextPiece *next_piece = &tetris_ptr->board.nextPieces.buf[ni];
        NextPieceShadow *shadow = &next_piece->shadow;
        Piece *shadow_piece = &shadow->piece;
        Piece *piece = &next_piece->piece;
        int shadow_alpha;
        int next_alpha;

        shadow_piece->physicalPos = shadow->physicalPos;
        shadow_piece->physicalPos.x += 0x80;
        shadow_piece->physicalPos.y += 0x80;
        if (tetris_ptr->board.nextPieces.alpha == 0xFF)
            shadow_alpha = shadow->alpha_sl4b >> 4;
        else
            shadow_alpha =
                (shadow->alpha_sl4b * tetris_ptr->board.nextPieces.alpha) / 4096U;
        if (shadow_alpha < 0) shadow_alpha = 0;
        if (shadow_alpha > 0xFF) shadow_alpha = 0xFF;

        for (int mi = 0; mi < 4; mi++) {
            Point mp;
            TNT_Vita_MobilePieceGetMinoPointScaled(
                shadow_piece, mi, shadow->mino_scale, &mp);
            draw_piece_mino_textured_alpha(
                buffer, playfield_x, playfield_y, playfield_cell,
                mp, shadow->mino_scale, shadow_piece, (u8)mi,
                tnt_mino_alpha(
                    &shadow_piece->mino_arr[mi], (u8)shadow_alpha));
        }

        piece->physicalPos = next_piece->physicalPos;
        piece->physicalRotOrigin = next_piece->physicalRotOrigin;
        if (tetris_ptr->board.nextPieces.alpha == 0xFF)
            next_alpha = next_piece->alpha_sl4b >> 4;
        else
            next_alpha =
                (next_piece->alpha_sl4b * tetris_ptr->board.nextPieces.alpha) / 4096U;
        if (next_alpha < 0) next_alpha = 0;
        if (next_alpha > 0xFF) next_alpha = 0xFF;

        for (int mi = 0; mi < 4; mi++) {
            Point mp;
            TNT_Vita_MobilePieceGetMinoPointScaled(
                piece, mi, next_piece->mino_scale, &mp);
            draw_piece_mino_textured_alpha(
                buffer, playfield_x, playfield_y, playfield_cell,
                mp, next_piece->mino_scale, piece, (u8)mi,
                tnt_mino_alpha(&piece->mino_arr[mi], (u8)next_alpha));
        }
    }

    g_tnt113_next_scratch_active = saved_next_scratch;
    if (prof222) {
        g_tnt_opp60_preview_us[player_index] =
            (u32)(sceKernelGetProcessTimeWide()-t222);
        t222 = sceKernelGetProcessTimeWide();
    }

    if (g_playercount >= 2) {
        /* Retail 3P/4P hold coordinates (0x800, -0x240) are FIXED-POINT
         * PLAYFIELD-LOCAL, not offscreen. Project every MP hold against its
         * actual source BoardInfo dimensions; this aligns the compact 3P
         * hold well without any per-player pixel nudges. */
        g_tnt_2p_hold_source_space = TRUE;
        /* Test100 photo calibration, EXACT original culture artwork.
         * The unchanged 3P test73 and all 2P placements keep their
         * previous source-projection correction. In test99 the newly
         * restored 4P artwork exposes slight visual well/held-mino
         * differences in five cultures: user IMG_3529..IMG_3533.
         *
         * Values are Vita display pixels (not N64 source px), applied
         * to the projected hold piece ONLY (including swap shadow).
         * Mayan and Celtic match the established test99 placement.
         * Never move board, preview, locked minos, game logic or GUI.
         *
         * Culture:    P1  P2  P3  P4
         * Mayan       -2   0   0   0 (unchanged)
         * Egyptian     0  -1  -1  -1 (3529: P1 left, others right)
         * Japanese    -2  -2  -2  -2 (3530: all in framed holders)
         * Celtic      -2   0   0   0 (unchanged)
         * African     -2  -2   0  -2 (3531: P2/P4 right)
         * Greek        0   0   0   0 (3532: P1 left)
         * Russian     -2  -2   0   0 (3533: P2 right)
         * The values are deliberately conservative trial positions,
         * not claimed to be N64's authored sprite calibration. */
        static const s8 mp4_hold_x_px[7][4] = {
            {-2, 0, 0, 0},
            { 0,-1,-1,-1},
            {-2,-2,-2,-2},
            {-2, 0, 0, 0},
            {-2,-2, 0,-2},
            { 0, 0, 0, 0},
            {-2,-2, 0, 0}
        };
        if (g_playercount == 4 && player_index < 4) {
            u8 culture = g_tnt_active_gameplay_screen < 7
                ? g_tnt_active_gameplay_screen : 6;
            g_tnt_2p_hold_screen_board_x =
                screen_board_x + mp4_hold_x_px[culture][player_index];
        } else {
            /* Hardware-approved test73 P1-only 3P compact correction. */
            g_tnt_2p_hold_screen_board_x = screen_board_x -
                ((g_playercount == 3 && player_index == 0) ? 2 : 0);
        }
        g_tnt_2p_hold_screen_board_y = screen_board_y;
        g_tnt_2p_hold_screen_cell = screen_cell;
        g_tnt_2p_hold_playfield_source_x = tetris_ptr->boardInfo.location.x;
        g_tnt_2p_hold_playfield_source_y = tetris_ptr->boardInfo.location.y;
        g_tnt_hold_source_unit_size = tetris_ptr->boardInfo.unitSize;

        for (int hi = 0; hi < 2; hi++) {
            PieceHoldPiece *hold_piece = &tetris_ptr->pieceHold.buf[hi];
            if (hold_piece->state != 0) {
                PieceHoldPieceShadow *hold_shadow = &hold_piece->shadow;
                Piece *piece = &hold_piece->piece;
                if (hold_shadow->unk0 != 0) {
                    Piece *shadow_piece = &hold_shadow->piece;
                    int shadow_alpha = hold_shadow->alpha == 0xFF
                        ? hold_shadow->unk2 >> 4
                        : (hold_shadow->unk2 * hold_shadow->alpha) >> 12;
                    if (shadow_alpha < 0) shadow_alpha = 0;
                    if (shadow_alpha > 0xFF) shadow_alpha = 0xFF;
                    for (int mi = 0; mi < 4; mi++) {
                        Point mp;
                        TNT_Vita_MobilePieceGetMinoPointScaled(
                            shadow_piece, mi, hold_shadow->mino_scale, &mp);
                        draw_piece_mino_textured_alpha(
                            buffer, playfield_x, playfield_y, playfield_cell,
                            mp, hold_shadow->mino_scale, shadow_piece, (u8)mi,
                            tnt_mino_alpha(
                                &shadow_piece->mino_arr[mi], (u8)shadow_alpha));
                    }
                }

                piece->physicalRotOrigin = hold_piece->physicalRotOrigin;
                piece->physicalPos = hold_piece->physicalPos;
                piece->physicalCkwRotValue = hold_piece->physicalCkwRotValue;
                for (int mi = 0; mi < 4; mi++) {
                    Point mp;
                    TNT_Vita_MobilePieceGetMinoPointScaled(
                        piece, mi, hold_piece->mino_scale, &mp);
                    draw_piece_mino_textured_alpha(
                        buffer, playfield_x, playfield_y, playfield_cell,
                        mp, hold_piece->mino_scale, piece, (u8)mi,
                        tnt_mino_alpha(&piece->mino_arr[mi], piece->alpha));
                }
            }
        }
        g_tnt_2p_hold_source_space = FALSE;
    }

    if (tetris_ptr->board.currentPiece.state == 1) {
        CurrentPiece *current_piece = &tetris_ptr->board.currentPiece;
        GhostPiece *ghost = &current_piece->ghostPiece;
        Piece *ghost_piece = &ghost->piece;
        Piece *piece = &current_piece->piece;
        ghost_piece->alpha = (current_piece->alpha * 0x60) >> 8;

        if (ghost->unk0 &&
            MobilePiece_800651c8_sevenliner_loops_4_times_v2_retbool(
                ghost_piece)) {
            for (int mi = 0; mi < 4; mi++) {
                Point mp;
                u8 alpha = tnt_mino_alpha(
                    &ghost_piece->mino_arr[mi], ghost_piece->alpha);
                TNT_Vita_MobilePieceGetMinoPointScaled(
                    ghost_piece, mi, 0x100, &mp);
                draw_piece_mino_textured_alpha(
                    buffer, playfield_x, playfield_y, playfield_cell,
                    mp, 0x100, ghost_piece, (u8)mi, alpha);
            }
        }

        piece->alpha = current_piece->alpha;
        for (int mi = 0; mi < 4; mi++) {
            Point mp;
            u8 alpha = tnt_mino_alpha(&piece->mino_arr[mi], piece->alpha);
            TNT_Vita_MobilePieceGetMinoPointScaled(piece, mi, 0x100, &mp);
            draw_piece_mino_textured_alpha(
                buffer, playfield_x, playfield_y, playfield_cell,
                mp, 0x100, piece, (u8)mi, alpha);
        }
    }

    TNT_Vita_RenderMultisquareGlows(
        buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
        playfield_x, playfield_y, playfield_cell, &tetris_ptr->squares);
    /* GameFinish image_loc is already in full 400x300 source coordinates.
     * Use the screen presentation transform here, not the local playfield
     * origin. P1 happened to look plausible with the old local transform; P2
     * was shifted far right/off-screen. The source renders one finish object
     * for every Tetris player through the common screen coordinate system. */
    TNT_Vita_GameFinish_Render(
        buffer, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_STRIDE,
        screen_board_x, screen_board_y, screen_cell,
        &tetris_ptr->frameAct.unk10);
    if (prof222)
        g_tnt_opp60_other_us[player_index] =
            (u32)(sceKernelGetProcessTimeWide()-t222);
    g_tnt_opp60_exact_board = FALSE;
    g_tnt_compact_playfield_active = FALSE;
}


static void tnt_vita_init_tetris_player(
    Tetris *tetris_ptr,
    u32 bag_seed,
    u8 screen,
    u8 player_index,
    u8 player_count,
    Landfill *landfill
) {
    u32 local_seed = bag_seed;

    memset(tetris_ptr, 0, sizeof(*tetris_ptr));
    tetris_ptr->unk0 = 1;
    tetris_ptr->unk2 = TRUE;
    tetris_ptr->unk3 = FALSE;
    tetris_ptr->unk4 = 1;

    if (player_count == 0) player_count = 1;
    if (player_count > 4) player_count = 4;
    if (player_index >= player_count) player_index = 0;

    /* Game_Init sets the shared player count before each Tetris_Init, while
     * Game_SetGlobalPointers selects the Tetris/PlayerVars being initialized. */
    g_playercount = player_count;
    tetris_ptr->minos.scale = 0x100;
    TNT_Vita_TetrisSetGlobalPointers(tetris_ptr, player_index);

    MobileCubes_Init(220, g_minos_ptr);
    FallingCubes_Init(200);
    TNT_BoardPiecesMin_Init();

    LineScan_Init();
    Multisquare_8006ada4_Init();

    GameStats_Init(&tetris_ptr->gameStats);
    Bag63_Init(&tetris_ptr->bag63, &local_seed);

    if (player_count == 2)
        PFGFX_VitaInit2PLayout(screen);
    else if (player_count == 3)
        PFGFX_VitaInit3PLayout(screen);
    else if (player_count == 4)
        PFGFX_VitaInit4PLayout(screen);
    else
        PFGFX_VitaInit1PLayout(screen);
    NextPieces_Init(&tetris_ptr->bag63);

    CurrentPiece_80068090_oneliner_sets_0x538_to_1(
        &tetris_ptr->board.currentPiece);
    tetris_ptr->board.currentPiece.fallVelocityCopy = 3;
    tetris_ptr->board.currentPiece.state = 0;
    PieceHold_Init(
        &tetris_ptr->pieceHold, &local_seed, 10, 20, screen);

    if (player_count == 2) {
        tetris_ptr->boardInfo.location.x = player_index == 0 ? 57 : 247;
        tetris_ptr->boardInfo.location.y = 43;
        tetris_ptr->boardInfo.unitSize = 10;
    } else if (player_count >= 3) {
        static const s16 small_x[4] = {34, 118, 202, 286};
        tetris_ptr->boardInfo.location.x = small_x[player_index];
        tetris_ptr->boardInfo.location.y = 68;
        tetris_ptr->boardInfo.unitSize = 8;
    } else {
        tetris_ptr->boardInfo.location.x = 83;
        tetris_ptr->boardInfo.location.y = 36;
        tetris_ptr->boardInfo.unitSize = 11;
    }
    FrameAct_Init(
        &tetris_ptr->frameAct,
        &tetris_ptr->pieceHold,
        &tetris_ptr->unk6840,
        &tetris_ptr->boardInfo);

    /* Tetris_Init allocates one Garbage object per player from the shared
     * Landfill. Normal solo TNTV passes NULL and retains the approved
     * garbage-disabled path; OPPONENT will pass a DIRECTED Landfill. */
    if (landfill != NULL) {
        u8 currentplayer = player_index;
        g_landfill_ptr = landfill;
        tetris_ptr->garbage_ptr = Landfill_Garbage();
        g_garbage_ptr = (Garbage *)tetris_ptr->garbage_ptr;
        Garbage_Init(g_garbage_ptr, &local_seed);
        if (landfill->type == LANDFILLTYPE_DIRECTED) {
            Garbage_80072e2c_set_arg0p_to_arg1(g_garbage_ptr, TRUE);
            Landfill_800723a0_calls_garbage_fun(
                currentplayer, currentplayer, FALSE);
            Landfill_8007240c_fiveliner(
                currentplayer, (u8)((currentplayer + 1U) % player_count));
            if (player_count == 2)
                Landfill_8007240c_fiveliner(
                    currentplayer, (u8)(currentplayer ^ 1U));
        } else if (landfill->type == LANDFILLTYPE_HOTPOTATO) {
            Garbage_80072e2c_set_arg0p_to_arg1(g_garbage_ptr, TRUE);
            Landfill_8007240c_fiveliner(
                currentplayer, landfill->unk30.unk0);
        } else {
            Garbage_80072e2c_set_arg0p_to_arg1(g_garbage_ptr, FALSE);
        }
    } else {
        tetris_ptr->garbage_ptr = NULL;
        g_garbage_ptr = NULL;
    }

    FrameAct_set_state(1);
    TNT_Vita_SetGameplayAlpha(tetris_ptr, 0);
}

static void tnt_vita_init_tetris_state(
    Tetris *tetris_ptr, u32 bag_seed, u8 screen
) {
    /* TEST107A: allocate/reset before rendering begins, not on a frame. */
    tnt107_onep_cache_reset();
    tnt_vita_init_tetris_player(
        tetris_ptr, bag_seed, screen, 0, 1, NULL);
}

static void tnt_vita_deinit_tetris_player_state(
    Tetris *tetris_ptr, u8 player_index
) {
    if (tetris_ptr == NULL)
        return;

    TNT_Vita_TetrisSetGlobalPointers(tetris_ptr, player_index);
    PieceHold_Deinit(&tetris_ptr->pieceHold);
    TNT_BoardPiecesMin_Deinit();
    FallingCubes_Deinit();
    MobileCubes_Deinit();
    Garbage_Deinit((Garbage *)tetris_ptr->garbage_ptr);
    GameStats_Deinit(&tetris_ptr->gameStats);
    tetris_ptr->garbage_ptr = NULL;
}

static void tnt_vita_reset_gameplay_state(
    Tetris *tetris_ptr,
    TNTMayanLargeFire *mayan_large_fire,
    u8 screen
) {
    /* Explicitly select player 0 before tearing down heap-backed board state.
     * This is behavior-neutral in solo play and prevents a previous player-1
     * OPPONENT context from freeing the wrong board on the next reset. */
    TNT_Vita_TetrisSetGlobalPointers(tetris_ptr, 0);
    TNT_BoardPiecesMin_Deinit();
    FallingCubes_Deinit();
    MobileCubes_Deinit();

    TNT_Vita_MetalField_Init();
    TNT_Vita_MayanLargeFire_Init(mayan_large_fire);
    tnt_vita_reset_mayan_temple_fires();
    tnt_vita_init_tetris_state(tetris_ptr, tnt_vita_game_seed_from_id(), screen);
}

static void tnt_vita_init_opponent_match(
    Tetris *human,
    Tetris *cpu,
    Landfill *landfill,
    u32 seed,
    u8 screen,
    u8 game_type,
    u8 ai_rank
) {
    /* OPPONENT uses the same two-board 18px renderer as local multiplayer.
     * Reset the proven 2P render caches when a fresh AI match starts so no
     * texture/tile state can carry across modes or matches. */
    tnt_vita_2p_cube_tile_cache_reset();
    tnt_vita_2p_moving_tex_cache_reset();
    tnt_vita_ai_prof_reset();
    memset(cpu, 0, sizeof(*cpu));
    memset(landfill, 0, sizeof(*landfill));

    g_playercount = 2;
    landfill->type = LANDFILLTYPE_DIRECTED;
    Landfill_Init(landfill);
    landfill->type = LANDFILLTYPE_DIRECTED;

    tnt_vita_init_tetris_player(human, seed, screen, 0, 2, landfill);
    tnt_vita_init_tetris_player(cpu, seed, screen, 1, 2, landfill);
    if (game_type == GAMETYPE_ULTRA) {
        human->ultraGoal = 150;
        cpu->ultraGoal = 150;
    }

    /* Game_Init allocates the shared square-analysis scratch after both
     * players exist, then the frontend creates the selected AI on player 1. */
    FUN_80041260_twoliner();
    aiplayer_gameinit_related(2, game_type, ai_rank);
}

static void tnt_vita_init_multiplayer_match(
    Tetris *p1,
    Tetris *p2,
    Landfill *landfill,
    u32 seed,
    u8 screen,
    u8 game_type,
    u8 garbage_type
) {
    tnt_vita_2p_cube_tile_cache_reset();
    tnt_vita_2p_moving_tex_cache_reset();
    memset(p2, 0, sizeof(*p2));
    memset(landfill, 0, sizeof(*landfill));
    g_playercount = 2;
    landfill->type = garbage_type;
    Landfill_Init(landfill);
    landfill->type = garbage_type;

    tnt_vita_init_tetris_player(p1, seed, screen, 0, 2, landfill);
    tnt_vita_init_tetris_player(p2, seed, screen, 1, 2, landfill);
    if (game_type == GAMETYPE_ULTRA) {
        p1->ultraGoal = 150;
        p2->ultraGoal = 150;
    }
    /* Same shared square-analysis scratch that Game_Init creates after all
     * player Tetris objects exist. No AI object is allocated for local play. */
    FUN_80041260_twoliner();
}

/* Retail 3P Sprint is 180 seconds (=10800 60Hz jiffies). Dedicated
 * test87D (45s) / test88D (90s) CMake builds override this ONLY for
 * 3P Sprint. Normal release builds always retain 180 seconds. */
#ifndef TNTV_3P_SPRINT_SECONDS
#define TNTV_3P_SPRINT_SECONDS 180U
#endif
#if TNTV_3P_SPRINT_SECONDS < 15 || TNTV_3P_SPRINT_SECONDS > 180
#error "TNTV_3P_SPRINT_SECONDS must be within 15..180"
#endif
#define TNTV_3P_SPRINT_TICKS (TNTV_3P_SPRINT_SECONDS * 60U)

/* Retail 3P Ultra: each participant's starting handicap is zero, goal 150.
 * A separate test-only build can set this to 3 via CMAKE_C_FLAGS; the
 * normal / release source defaults to 150 and contains no unlock cheat. */
#ifndef TNTV_3P_ULTRA_GOAL
#define TNTV_3P_ULTRA_GOAL 150U
#endif
#if TNTV_3P_ULTRA_GOAL < 1 || TNTV_3P_ULTRA_GOAL > 150
#error "TNTV_3P_ULTRA_GOAL override must be within 1..150"
#endif
/* Isolated test-only option: force P1 initial Hot Potato holder to make
 * hands-on transfer testing deterministic. Retail defaults to random. */
#ifndef TNTV_3P_HP_FORCE_P1
#define TNTV_3P_HP_FORCE_P1 0
#endif
#if TNTV_3P_HP_FORCE_P1 != 0 && TNTV_3P_HP_FORCE_P1 != 1
#error "TNTV_3P_HP_FORCE_P1 must be 0 (retail) or 1 (test)"
#endif

static void __attribute__((optimize("Os"))) tnt_vita_init_multiplayer_3p_match(
    Tetris *p1, Tetris *p2, Tetris *p3,
    Landfill *landfill, u32 seed, u8 screen, u8 game_type,
    u8 garbage_type
) {
    /* All modes share source-authored compact board geometry. Test82 enables
     * original 3P Hot Potato on Marathon only, with NONE unchanged. */
    g_tnt_local_2p_fast_render = FALSE;
    tnt_vita_3p_hp_tint_free();
    tnt_vita_3p_directed_free();
    tnt_vita_3p_ultra_hp_clock_free();
    tnt_vita_3p_ultra_hp_lines_free();
    memset(g_tnt_3p_ultra_peak_us, 0, sizeof(g_tnt_3p_ultra_peak_us));
    g_tnt_3p_ultra_peak_total=0;
    g_tnt_3p_ultra_peak_show=FALSE;
    memset(g_tnt_3p_scene_buffer_ready, 0,
        sizeof(g_tnt_3p_scene_buffer_ready));
    tnt_vita_2p_moving_tex_cache_reset();
    tnt_vita_3p_cube_tile_cache_init();
    memset(p2, 0, sizeof(*p2));
    memset(p3, 0, sizeof(*p3));
    memset(landfill, 0, sizeof(*landfill));
    g_playercount = 3;
    landfill->type = garbage_type;
    Landfill_Init(landfill);
    landfill->type = garbage_type;
#if TNTV_3P_HP_FORCE_P1
    if (garbage_type == LANDFILLTYPE_HOTPOTATO)
        landfill->unk30.unk0 = 0;
#endif
    tnt_vita_init_tetris_player(p1, seed, screen, 0, 3, landfill);
    tnt_vita_init_tetris_player(p2, seed, screen, 1, 3, landfill);
    tnt_vita_init_tetris_player(p3, seed, screen, 2, 3, landfill);
    if (game_type == GAMETYPE_ULTRA) {
        /* Original Tetris_SetUltraHandicap: 150 + handicap*10. The 3P
         * frontend currently has zero handicap for all participants. */
        p1->ultraGoal = TNTV_3P_ULTRA_GOAL;
        p2->ultraGoal = TNTV_3P_ULTRA_GOAL;
        p3->ultraGoal = TNTV_3P_ULTRA_GOAL;
    }
    FUN_80041260_twoliner();
}

static void __attribute__((optimize("Os"))) tnt_vita_deinit_multiplayer_3p_match(
    Tetris *p1, Tetris *p2, Tetris *p3, Landfill *landfill
) {
    FUN_8004129c_fourliner();
    tnt_vita_deinit_tetris_player_state(p1, 0);
    tnt_vita_deinit_tetris_player_state(p2, 1);
    tnt_vita_deinit_tetris_player_state(p3, 2);
    Landfill_Deinit(landfill);
    tnt_vita_3p_cube_tile_cache_term();
    tnt_vita_3p_hp_tint_free();
    tnt_vita_3p_directed_free();
    tnt_vita_3p_ultra_hp_clock_free();
    tnt_vita_3p_ultra_hp_lines_free();
    g_playercount = 1;
}

static void tnt_vita_deinit_multiplayer_match(
    Tetris *p1, Tetris *p2, Landfill *landfill
) {
    FUN_8004129c_fourliner();
    tnt_vita_deinit_tetris_player_state(p1, 0);
    tnt_vita_deinit_tetris_player_state(p2, 1);
    Landfill_Deinit(landfill);
    g_playercount = 1;
}

static void tnt_vita_deinit_opponent_match(
    Tetris *human, Tetris *cpu, Landfill *landfill
) {
    aiplayer_80042b3c_calls_heap_unalloc();
    FUN_8004129c_fourliner();
    tnt_vita_deinit_tetris_player_state(human, 0);
    tnt_vita_deinit_tetris_player_state(cpu, 1);
    Landfill_Deinit(landfill);
    g_playercount = 1;
}

/* TEST112A: test110A's narrow restoration corrupted the bezel; test111A
 * repaired all its pixels but a per-pixel CDRAM READ cost ~76ms/frame
 * (10FPS). Restore BOTH complete original-art side panels directly by
 * fast RAM->CDRAM memcpy. No framebuffer READS, no clip assumptions,
 * no changes to 109A fast fire, 107A settled-cube cache, or VBlank. */
static void tnt112_copy_panel_span(
    u32 *dst,const u32 *src,int x0,int x1
) {
    if(x1<=x0)return;
    memcpy(dst+x0,src+x0,(size_t)(x1-x0)*sizeof(u32));
    g_tnt112_written_pixel_count+=(u32)(x1-x0);
}

static u8 tnt110_restore_persistent_mayan_bezel(
    void *buffer,int left,int right,u32 buffer_index
) {
    const size_t full=(size_t)DISPLAY_STRIDE*DISPLAY_HEIGHT*sizeof(u32);
    if(buffer_index>=DISPLAY_BUFFER_COUNT ||
       left<=0||left>=DISPLAY_WIDTH||
       right<=left||right>=DISPLAY_WIDTH)
        return FALSE;
    if(!g_tnt110_panel_master)
        g_tnt110_panel_master=(u32*)malloc(full);
    if(!g_tnt110_panel_master)return FALSE;
    if(g_tnt110_panel_left!=left || g_tnt110_panel_right!=right){
        tnt_vita_draw_bezel_between(g_tnt110_panel_master,left,right);
        g_tnt110_panel_left=left;
        g_tnt110_panel_right=right;
        memset(g_tnt110_panel_valid,0,sizeof(g_tnt110_panel_valid));
    }
    if(!g_tnt110_panel_valid[buffer_index]){
        /* First frame on each framebuffer or after returning from a menu:
         * draw ORIGINAL complete art, then leave outer pixels resident. */
        tnt_vita_draw_bezel_between(buffer,left,right);
        g_tnt112_written_pixel_count=0;
        g_tnt110_panel_valid[buffer_index]=TRUE;
        g_tnt110_rebuild_count++;
        return TRUE;
    }
    /* Unconditional source-exact full-side panel memory copies.
     * No CDRAM read or conditional write and no GXM-only clip guess. */
    g_tnt112_written_pixel_count=0;
    const u32 *src=g_tnt110_panel_master;
    u32 *dst=(u32*)buffer;
    for(int y=0;y<DISPLAY_HEIGHT;y++){
        const size_t off=(size_t)y*DISPLAY_STRIDE;
        tnt112_copy_panel_span(dst+off,src+off,0,left);
        tnt112_copy_panel_span(dst+off,src+off,right,DISPLAY_WIDTH);
    }
    g_tnt110_warm_count++;
    return TRUE;
}
static void tnt110_record_border_cost(u32 micros){
    const u64 now=sceKernelGetProcessTimeWide();
    if(!g_tnt110_border_last_window)
        g_tnt110_border_last_window=now;
    g_tnt110_border_sum_us+=micros;
    g_tnt110_border_samples++;
    if(now-g_tnt110_border_last_window>=500000U &&
       g_tnt110_border_samples){
        g_tnt110_border_avg_us=
            g_tnt110_border_sum_us/g_tnt110_border_samples;
        g_tnt110_border_sum_us=0;
        g_tnt110_border_samples=0;
        g_tnt110_border_last_window=now;
    }
}

static void swap_buffers(void) {
    /* Final presentation pass.  This is intentionally below every frontend,
     * cinematic and gameplay renderer so no attract-mode or special-state path
     * can accidentally omit the surround.  Gameplay selects the established
     * 12-source-pixel CRT crop; other states use the full centered 4:3 field. */
    if(!g_tnt110_mayan_bezel_eligible)
        memset(g_tnt110_panel_valid,0,sizeof(g_tnt110_panel_valid));
    if (g_tnt_presentation_crop != 0) {
        int left_end = TNT_PRESENT_BOARD_X + tnt_floor_div_s64(
            (s64)(g_tnt_presentation_crop - 83) * TNT_PRESENT_CELL + 10, 11);
        int right_start = TNT_PRESENT_BOARD_X + tnt_floor_div_s64(
            (s64)(400 - g_tnt_presentation_crop - 83) * TNT_PRESENT_CELL, 11);
        u8 used_cache=FALSE;
        if(g_tnt110_mayan_bezel_eligible){
            const u64 stamp=sceKernelGetProcessTimeWide();
            used_cache=tnt110_restore_persistent_mayan_bezel(
                buffers[backBuffer].data,left_end,right_start,
                (u32)backBuffer);
            tnt110_record_border_cost((u32)(
                sceKernelGetProcessTimeWide()-stamp));
        }
        if(!used_cache)
            tnt_vita_draw_bezel_between(
                buffers[backBuffer].data,left_end,right_start);
    } else if (g_tnt_intro_shim_active) {
        int left_end = TNT_PRESENT_BOARD_X + tnt_floor_div_s64(
            (s64)(0 - 83) * TNT_PRESENT_CELL + 10, 11);
        int right_start = TNT_PRESENT_BOARD_X + tnt_floor_div_s64(
            (s64)(400 - 83) * TNT_PRESENT_CELL, 11);
        /* Intro-only surround: keep the approved cybergrid panel geometry at
         * its canonical positions, but suppress the normal three-pixel inner
         * vertical rails. The fixed shim then bridges the sand cinematic's
         * narrow spacer without leaving a stray divider line. */
        tnt_vita_draw_bezel_between_no_inner_rail(
            buffers[backBuffer].data, left_end, right_start);
        tnt_vita_draw_intro_cybergrid_shim(
            buffers[backBuffer].data, left_end, right_start,
            TNT_INTRO_CYBERGRID_SHIM_PX);
    } else if (g_tnt_boot_no_inner_rail_active) {
        int left_end = TNT_PRESENT_BOARD_X + tnt_floor_div_s64(
            (s64)(0 - 83) * TNT_PRESENT_CELL + 10, 11);
        int right_start = TNT_PRESENT_BOARD_X + tnt_floor_div_s64(
            (s64)(400 - 83) * TNT_PRESENT_CELL, 11);
        /* First-boot black/developer presentation only. Use the exact same
         * rail-less surround + fixed 10-pixel shim as the sand intro so the
         * apparent content width does not step inward when the cinematic begins.
         * This intentionally trims only the extreme vertical edges of the boot
         * black/developer pages; all post-intro frontend states stay unchanged. */
        tnt_vita_draw_bezel_between_no_inner_rail(
            buffers[backBuffer].data, left_end, right_start);
        tnt_vita_draw_intro_cybergrid_shim(
            buffers[backBuffer].data, left_end, right_start,
            TNT_INTRO_CYBERGRID_SHIM_PX);
    } else {
        tnt_vita_draw_frontend_pillarbox(
            buffers[backBuffer].data, TNT_PRESENT_BOARD_X, TNT_PRESENT_CELL);
    }
    tnt_vita_update_and_draw_fps(buffers[backBuffer].data);
    g_tnt_wonder_profile_active=FALSE;

    sceGxmPadHeartbeat(
        &buffers[backBuffer].surface,
        buffers[backBuffer].sync
    );

    sceGxmDisplayQueueAddEntry(
        buffers[frontBuffer].sync,
        buffers[backBuffer].sync,
        &buffers[backBuffer].data
    );

    /*
     * Queue the new framebuffer, rotate ownership, then wait for the single
     * VBlank that hands scanout to it before the next loop may touch the old
     * front buffer.  VitaSDK's normal double-buffer path does not drain the
     * whole display queue here: doing both sceGxmDisplayQueueFinish() and a
     * VBlank wait serialized us across two refresh opportunities and imposed
     * an artificial 30 FPS ceiling (with heavier frames quantizing to 15).
     *
     * Keep the VBlank wait itself.  That is the protection against the old
     * CPU-writing-the-live-scanout-buffer flicker/partial-transparency bug.
     */
    frontBuffer = backBuffer;
    backBuffer = (backBuffer + 1) % DISPLAY_BUFFER_COUNT;
    sceDisplayWaitVblankStart();
}

static void graphics_term(void) {
    sceGxmDisplayQueueFinish();
    TNT_Vita_WonderMilestoneGxmTerm();

    for (int i = 0; i < DISPLAY_BUFFER_COUNT; i++) {
        sceGxmUnmapMemory(buffers[i].data);
        sceGxmSyncObjectDestroy(buffers[i].sync);
        sceKernelFreeMemBlock(buffers[i].uid);
    }

    sceGxmTerminate();
}

/* Restore an exact rectangle of the static N64 multi-player artwork.
 * Padding on the outer source bounds deliberately covers interpolating
 * previews, held minos, ghost/multisquare effects, plus the three counters.
 * Source = same precomposed unmodified screen that test76 copied in full. */
static void __attribute__((optimize("Os"))) tnt_vita_3p_scene_rect(
    void *buffer, int x0, int y0, int x1, int y1
) {
    uint32_t *dest = (uint32_t *)buffer;
    int y;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > DISPLAY_WIDTH) x1 = DISPLAY_WIDTH;
    if (y1 > DISPLAY_HEIGHT) y1 = DISPLAY_HEIGHT;
    if (x1 <= x0 || y1 <= y0) return;
    for (y = y0; y < y1; ++y) {
        memcpy(dest + (size_t)y * DISPLAY_STRIDE + x0,
               g_tnt_static_scene + (size_t)y * DISPLAY_STRIDE + x0,
               (size_t)(x1 - x0) * sizeof(uint32_t));
    }
}

/* Test219D: source-coordinate regions left trails outside the wells.
 * This version restores the exact previous draw footprint for each
 * 960x544 buffer. Runs of adjacent 32x16 tiles copy in one span per row. */
static void tnt_vita_opp60_restore_scene(void *buffer, u8 force_full) {
    u32 row;
    u32 *dst = (u32 *)buffer;
    if (force_full || !g_tnt_opp60_scene_buffer_ready[backBuffer] ||
        g_tnt_opp60_dirty_full[backBuffer]) {
        memcpy(buffer, g_tnt_static_scene, sizeof(g_tnt_static_scene));
        g_tnt_opp60_scene_buffer_ready[backBuffer] = TRUE;
    } else {
        for (row = 0; row < TNT_OPP60_TILE_ROWS; row++) {
            u32 bits = g_tnt_opp60_dirty[backBuffer][row];
            while (bits) {
                int left = __builtin_ctz(bits);
                int right = left;
                int y0 = (int)row * TNT_OPP60_TILE_Y;
                int y;
                while (right < DISPLAY_WIDTH / TNT_OPP60_TILE_X &&
                       (bits & (1U << right)))
                    right++;
                u32 mask = ((1U << (right - left)) - 1U) << left;
                int px = left * TNT_OPP60_TILE_X;
                int w = (right - left) * TNT_OPP60_TILE_X;
                for (y = y0; y < y0 + TNT_OPP60_TILE_Y; y++)
                    memcpy(dst + (size_t)y * DISPLAY_STRIDE + px,
                           g_tnt_static_scene + (size_t)y * DISPLAY_STRIDE + px,
                           (size_t)w * sizeof(u32));
                bits &= ~mask;
            }
        }
    }
    memset(g_tnt_opp60_dirty[backBuffer], 0,
           sizeof(g_tnt_opp60_dirty[backBuffer]));
    g_tnt_opp60_dirty_full[backBuffer] = FALSE;
}

static void __attribute__((optimize("Os"))) tnt_vita_3p_restore_scene(
    void *buffer, int board_x, int board_y, int cell, u8 full
) {
    if (full || !g_tnt_3p_scene_buffer_ready[backBuffer]) {
        memcpy(buffer, g_tnt_static_scene, sizeof(g_tnt_static_scene));
        g_tnt_3p_scene_buffer_ready[backBuffer] = TRUE;
        return;
    }

    /* The upper strip spans the source preview line, all three hold wells,
     * and the developer FPS overlay. X is full native 400px culture artwork. */
    tnt_vita_3p_scene_rect(
        buffer,
        tnt_vita_source_x(0, board_x, cell) - 3,
        tnt_vita_source_y(0, board_y, cell) - 3,
        tnt_vita_source_x(400, board_x, cell) + 3,
        tnt_vita_source_y(78, board_y, cell) + 3);

    /* Three compact wells (34/118/202), line counts and finish animations.
     * The unused fourth well is wholly static. Outside both restored bands,
     * no gameplay pixel is ever modified (pillarbox redraw is independent). */
    tnt_vita_3p_scene_rect(
        buffer,
        tnt_vita_source_x(15, board_x, cell) - 6,
        tnt_vita_source_y(55, board_y, cell) - 6,
        tnt_vita_source_x(313, board_x, cell) + 6,
        tnt_vita_source_y(282, board_y, cell) + 6);
}

static u8 __attribute__((optimize("Os"))) tnt_vita_3p_core_frame(
    Tetris *p1, Tetris *p2, Tetris *p3, Landfill *landfill,
    TNTVitaPauseMenu *pause_menu, u16 *end_delay, u8 *winner_mask,
    u32 *elapsed_ticks, u8 *pause_owner, u8 game_type,
    int board_x, int board_y, int cell
) {
    Tetris *players[3] = {p1, p2, p3};
    static const s16 source_x[3] = {34, 118, 202};
    static const s16 line_x[3] = {39, 123, 207};
    void *buffer = buffers[backBuffer].data;
    int pf_cell = (8 * cell + 5) / 11;
    int pf_y = board_y + tnt_floor_div_s64((s64)(68 - 36) * cell, 11);
    u8 active_count = 0;
    u8 pause_was_active = TNT_Vita_PauseIsActive(pause_menu);
    s32 pause_action;
    int i;
    const u8 prof = g_tnt_3p_prof_active;
    const u8 all_live_at_start = (u8)(
        p1->unk2 && p2->unk2 && p3->unk2 && !pause_was_active);
    const u8 peak_track = (u8)(
        prof && landfill &&
        (landfill->type == LANDFILLTYPE_HOTPOTATO ||
         landfill->type == LANDFILLTYPE_DIRECTED) &&
        game_type == GAMETYPE_ULTRA);
    u64 stamp = prof ? sceKernelGetProcessTimeWide() : 0;
    u64 start_stamp = stamp;
    u32 times[TNT3P_PROF_COUNT] = {0};
    if (prof) {
        g_tnt_3p_prof_tile_hits_frame = 0;
        g_tnt_3p_prof_tile_misses_frame = 0;
    }

    D_801109F4 = tnt_vita_60hz_delta(TRUE);
    if (D_801109F4 == 0) D_801109F4 = 1;

    /* Retail accepts START from any still-playing participant, and keeps
     * the whole match frozen while one player's popup is on screen. */
    if (!pause_was_active) {
        for (i = 0; i < 3; i++) {
            if (players[i]->unk2 && !players[i]->frameAct.unkC &&
                (g_PV_arr[i].unk28.unk8C & CONT_START)) {
                int j;
                *pause_owner = (u8)i;
                TNT_Vita_TetrisSetGlobalPointers(players[i], (u8)i);
                TNT_Vita_PauseStart(pause_menu, players[i]);
                for (j = 0; j < 3; j++) {
                    if (j != i)
                        Tetris_FUN_80054220_fourliner(players[j], TRUE);
                }
                pause_was_active = TRUE;
                break;
            }
        }
    }
    TNT_Vita_TetrisSetGlobalPointers(players[*pause_owner], *pause_owner);
    pause_action = TNT_Vita_PauseUpdate(
        pause_menu, players[*pause_owner], &g_PV_arr[*pause_owner]);
    for (i = 0; i < 3; i++)
        TNT_Vita_SetGameplayAlpha(players[i], (u8)pause_menu->fade.alpha);
    if (pause_action == TNT_VITA_PAUSE_ACTION_CONTINUE) {
        for (i = 0; i < 3; i++) {
            if (i != *pause_owner) {
                Tetris_FUN_80054220_fourliner(players[i], FALSE);
                FUN_800535B8_fiveliner(&g_PV_arr[i]);
                g_PV_arr[i].unk20 = 0;
            }
        }
    }

    /* Game logic and timer must not advance beneath the popup. */
    if (!pause_was_active) {
    if (p1->unk2 || p2->unk2 || p3->unk2)
        *elapsed_ticks += D_801109F4;
    TNT_Vita_MetalField_Update(D_801109F4);

    for (i = 0; i < 3; i++) {
        TNT_Vita_TetrisSetGlobalPointers(players[i], (u8)i);
        (void)Tetris_Update(players[i]);
    }

    /* Retail Game_QueryGameOver first marks every newly topped board LOSE,
     * then awards WIN only after the whole pass, preserving simultaneous
     * top-out ordering. */
    for (i = 0; i < 3; i++) {
        Tetris *tp = players[i];
        if (tp->unk2 && tp->frameAct.unk3 && !tp->frameAct.unkC) {
            TNT_Vita_TetrisSetGlobalPointers(tp, (u8)i);
            FrameAct_calls_gamefinish_something2(
                &tp->frameAct, GAMEFINISHTYPE_LOSE);
            tp->unk2 = FALSE;
            func_800726A4(landfill, (u8)i);
        }
    }

    for (i = 0; i < 3; i++)
        active_count += players[i]->unk2 ? 1U : 0U;

    if (active_count == 1) {
        for (i = 0; i < 3; i++) {
            Tetris *tp = players[i];
            if (tp->unk2) {
                TNT_Vita_TetrisSetGlobalPointers(tp, (u8)i);
                FrameAct_calls_gamefinish_something2(
                    &tp->frameAct, GAMEFINISHTYPE_WIN);
                tp->unk2 = FALSE;
                func_800726A4(landfill, (u8)i);
                *winner_mask |= (u8)(1U << i);
                break;
            }
        }
    } else if (active_count > 1 && game_type == GAMETYPE_SPRINT &&
               *elapsed_ticks >= TNTV_3P_SPRINT_TICKS) {
        /* Retail game.c: isGoalCompleted() checks shared 3:00 Sprint limit,
         * AFTER processing every newly topped board. Game_isGameWinner()
         * then awards WIN to each surviving player tied at the maximum
         * line count. Previously topped players never win this comparison. */
        u32 max_lines = 0;
        *elapsed_ticks = TNTV_3P_SPRINT_TICKS;
        for (i = 0; i < 3; i++) {
            if (players[i]->unk2) {
                u32 lines = getLineCount(&players[i]->gameStats.linesInfo);
                if (lines > max_lines) max_lines = lines;
            }
        }
        for (i = 0; i < 3; i++) {
            Tetris *tp = players[i];
            if (tp->unk2) {
                u32 lines = getLineCount(&tp->gameStats.linesInfo);
                u8 wins = (u8)(lines == max_lines);
                TNT_Vita_TetrisSetGlobalPointers(tp, (u8)i);
                FrameAct_calls_gamefinish_something2(
                    &tp->frameAct,
                    wins ? GAMEFINISHTYPE_WIN : GAMEFINISHTYPE_LOSE);
                tp->unk2 = FALSE;
                func_800726A4(landfill, (u8)i);
                if (wins) *winner_mask |= (u8)(1U << i);
            }
        }
    } else if (active_count > 1 && game_type == GAMETYPE_ULTRA) {
        /* Ultra goal fires as soon as ANY participant reaches their line
         * target. The original isGoalCompleted() scans all 3 participants;
         * Game_isGameWinner() excludes prior topouts when finding min time
         * and max lines and accepts only those with that minimum elapsed
         * time AND (goal met OR top surviving line count). */
        u8 goal_reached = FALSE;
        u32 max_lines = 0;
        u32 min_time = 0xFFFFFFFFU;
        for (i = 0; i < 3; i++) {
            if (getLineCount(&players[i]->gameStats.linesInfo) >=
                players[i]->ultraGoal)
                goal_reached = TRUE;
        }
        if (goal_reached) {
            for (i = 0; i < 3; i++) {
                Tetris *tp = players[i];
                if (tp->unk2) {
                    u32 lines = getLineCount(&tp->gameStats.linesInfo);
                    if (lines > max_lines) max_lines = lines;
                    if (tp->gameStats.elapsedTime < min_time)
                        min_time = tp->gameStats.elapsedTime;
                }
            }
            for (i = 0; i < 3; i++) {
                Tetris *tp = players[i];
                if (tp->unk2) {
                    u32 lines = getLineCount(&tp->gameStats.linesInfo);
                    u8 wins = (u8)(
                        (lines >= tp->ultraGoal || lines >= max_lines) &&
                        tp->gameStats.elapsedTime == min_time);
                    TNT_Vita_TetrisSetGlobalPointers(tp, (u8)i);
                    FrameAct_calls_gamefinish_something2(
                        &tp->frameAct,
                        wins ? GAMEFINISHTYPE_WIN : GAMEFINISHTYPE_LOSE);
                    tp->unk2 = FALSE;
                    func_800726A4(landfill, (u8)i);
                    if (wins) *winner_mask |= (u8)(1U << i);
                }
            }
        }
    }

    if (peak_track && (!p1->unk2 || !p2->unk2 || !p3->unk2))
        g_tnt_3p_ultra_peak_show=TRUE;

    if (!p1->unk2 && !p2->unk2 && !p3->unk2) {
        if (*end_delay > D_801109F4) {
            *end_delay -= (u16)D_801109F4;
        } else {
            *end_delay = 0;
            if ((g_PV_arr[0].unk28.unk8C & (CONT_A | CONT_START)) ||
                (g_PV_arr[1].unk28.unk8C & (CONT_A | CONT_START)) ||
                (g_PV_arr[2].unk28.unk8C & (CONT_A | CONT_START))) {
                for (i = 0; i < 3; i++)
                    gamefinish_80052D48_twoliner(&players[i]->frameAct.unk10);
            }
        }
    }

    } /* !pause_was_active */

    if (prof) {
        u64 now = sceKernelGetProcessTimeWide();
        times[TNT3P_PROF_UPDATE] = (u32)(now - stamp);
        stamp = now;
    }
    {
        /* Test85: a 3P GameFinish sprite is constrained to source-space
         * playfield rectangles already in our test77 active-band restore:
         * 3P board centers x74/158/242, y148. Largest sprite (70x40)
         * lies in x39..277, and its cos*scale displacement is <=60 native
         * source px from nominal y128..148; hence y68..228. The restored
         * source x15..313, y55..282 region safely contains all WIN/LOSE/
         * GAME OVER animation frames, even during moving/sweeping states.
         * No per-frame 557056-pixel full copy is needed solely for finish.
         * The real pause popup may draw outside both active bands and STILL
         * forces full copies as before, including both-buffer invalidation. */
        u8 full = (u8)(pause_was_active ||
            TNT_Vita_PauseIsActive(pause_menu));
        if (full) {
            memset(g_tnt_3p_scene_buffer_ready, 0,
                sizeof(g_tnt_3p_scene_buffer_ready));
        }
        g_tnt_3p_restore_full_last = (u8)(
            full || !g_tnt_3p_scene_buffer_ready[backBuffer]);
        tnt_vita_3p_restore_scene(buffer, board_x, board_y, cell, full);
        if (!full && game_type == GAMETYPE_ULTRA &&
            (!g_tnt_3p_ultra_hp_clock.active ||
             g_tnt_3p_ultra_hp_clock.tile == NULL)) {
            /* Legacy 3P Ultra time can extend below the normal active-band
             * bottom edge. The new HP tile overwrites its exact whole
             * clock rectangle instead and requires no extra clear. */
            tnt_vita_3p_scene_rect(buffer,
                tnt_vita_source_x(182, board_x, cell) - 4,
                tnt_vita_source_y(279, board_y, cell) - 2,
                tnt_vita_source_x(260, board_x, cell) + 4,
                tnt_vita_source_y(299, board_y, cell) + 2);
        }
    }
    if (prof) {
        u64 now = sceKernelGetProcessTimeWide();
        times[TNT3P_PROF_COPY] = (u32)(now - stamp);
        stamp = now;
    }
    if (landfill != NULL && landfill->type == LANDFILLTYPE_HOTPOTATO) {
        tnt_vita_3p_hotpotato_render(
            buffer, landfill, board_x, board_y, cell,
            (u8)(pause_was_active || TNT_Vita_PauseIsActive(pause_menu)),
            FALSE);
        if (prof) {
            u64 now = sceKernelGetProcessTimeWide();
            times[TNT3P_PROF_HOT] = (u32)(now - stamp);
            stamp = now; /* Exclude HP cost from first board B. */
        }
    }
    if (landfill != NULL && landfill->type == LANDFILLTYPE_DIRECTED &&
        game_type != GAMETYPE_ULTRA) {
        tnt_vita_3p_directed_render(
            buffer, landfill, board_x, board_y, cell);
        if (prof) {
            u64 now = sceKernelGetProcessTimeWide();
            times[TNT3P_PROF_HOT] = (u32)(now - stamp);
            stamp = now; /* Standalone DIRECTED HUD cost; B0 unchanged. */
        }
    }
    /* Reuse the exact-value moving-mino cache / axis-aligned rasterizer that
     * solved 2P preview performance.  This also handles the restored held
     * pieces. The 2P 18px locked-cube tile cache stays gated by cell size. */
    g_tnt_local_2p_fast_render = TRUE;
    for (i = 0; i < 3; i++) {
        /* Align the logical compact board to its source framing without
         * nudging individual minos independently (moving/locked/glow stay
         * in the same coordinate system). Test73 hardware: 1px left drift. */
        int pf_x = board_x +
            tnt_floor_div_s64((s64)(source_x[i] - 83) * cell, 11) + 1;
        char lines[12];
        tnt_vita_render_tetris_player(
            buffer, players[i], (u8)i, pf_x, pf_y, pf_cell,
            board_x, board_y, cell);
        if (prof) {
            u64 now = sceKernelGetProcessTimeWide();
            u32 total = (u32)(now - stamp);
            u32 locked = g_tnt_3p_prof_locked_frame[i];
            times[TNT3P_PROF_LOCK0 + i] = locked;
            times[TNT3P_PROF_REST0 + i] = total > locked ? total - locked : 0;
            stamp = now;
        }
        {
            u32 value=getLineCount(&players[i]->gameStats.linesInfo);
            if (tnt_vita_3p_ultra_hp_line_tile_enabled(i)) {
                tnt_vita_3p_ultra_hp_line_tile_render(
                    buffer,i,board_x,board_y,cell,value);
            } else {
                snprintf(lines,sizeof(lines),"%u",(unsigned)value);
                tnt_vita_draw_font_c_text(
                    buffer,board_x,board_y,cell,line_x[i],248,
                    lines,0xFF,0xFF,0xFF,0xFF);
            }
        }
        if (prof) {
            u64 now = sceKernelGetProcessTimeWide();
            times[TNT3P_PROF_REST0 + i] += (u32)(now - stamp);
            stamp = now;
        }
    }
    g_tnt_local_2p_fast_render = FALSE;
    if (game_type == GAMETYPE_SPRINT) {
        /* Original FUN_027BF0_Render: 3P timer at (216,266) Font C, counts
         * down 180 seconds in shared 60Hz jiffies. The test77 active-region
         * background restore already covers this source rectangle. */
        u32 remaining = *elapsed_ticks < TNTV_3P_SPRINT_TICKS
            ? (TNTV_3P_SPRINT_TICKS - *elapsed_ticks) : 0U;
        tnt_vita_draw_original_time(
            buffer, board_x, board_y, cell, 216, 266, remaining);
    } else if (game_type == GAMETYPE_ULTRA) {
        /* Retail Ultra source: goal at (188,265), elapsed time at
         * (217,278). Only 3P Ultra + Hot Potato has its goal baked
         * into the static scene and its clock tile updated on second
         * changes; NONE and future Directed stay on the old path. */
        if (g_tnt_3p_ultra_hp_clock.active) {
            tnt_vita_3p_ultra_hp_clock_render(
                buffer, board_x, board_y, cell, *elapsed_ticks);
        } else {
            char goal_text[12];
            snprintf(goal_text, sizeof(goal_text), "%u",
                (unsigned)TNTV_3P_ULTRA_GOAL);
            tnt_vita_draw_font_c_text(
                buffer, board_x, board_y, cell,
                188, 265, goal_text, 0xFF, 0xFF, 0xFF, 0xFF);
            tnt_vita_draw_original_time(
                buffer, board_x, board_y, cell, 217, 278, *elapsed_ticks);
        }
    }
    if (landfill != NULL && landfill->type == LANDFILLTYPE_DIRECTED &&
        game_type == GAMETYPE_ULTRA) {
        /* Original target icons occupy source x145..256 y267..278;
         * the cached Ultra timer tile extends through y275..294.
         * Render markers AFTER that tile to prevent its background
         * restore from erasing the bottom of P2's active indicator.
         * All non-Ultra Directed modes keep their approved draw order. */
        tnt_vita_3p_directed_render(
            buffer, landfill, board_x, board_y, cell);
        if (prof) {
            u64 now=sceKernelGetProcessTimeWide();
            times[TNT3P_PROF_HOT]=(u32)(now-stamp);
            stamp=now;
        }
    }
    if (!TNT_Vita_PauseIsActive(pause_menu) ||
        !g_tnt_milestone_gxm_ready ||
        TNT_Vita_PauseGxmRender(
            &buffers[backBuffer].surface, buffers[backBuffer].sync,
            board_x, board_y, cell,
            pause_menu->selected, pause_menu->pulse) < 0) {
        TNT_Vita_PauseRender(
            buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            board_x, board_y, cell, pause_menu);
    }
    /* Test90: both Ultra garbage variants reuse the test89F
     * redundant-bezel elimination. swap_buffers always applies the
     * final identical opaque cybergrid to this same buffer; the earlier
     * extra full rendering pass is unnecessary. Marathon/Sprint paths
     * stay as hardware approved. */
    if (!(game_type == GAMETYPE_ULTRA && landfill != NULL &&
          (landfill->type == LANDFILLTYPE_HOTPOTATO ||
           landfill->type == LANDFILLTYPE_DIRECTED)))
        tnt_vita_draw_frontend_pillarbox(buffer, board_x, cell);
    g_tnt_presentation_crop = (u8)tnt_vita_gameplay_side_mask_width(0);
    if (prof) {
        u64 now = sceKernelGetProcessTimeWide();
        times[TNT3P_PROF_EXTRA] = (u32)(now - stamp);
        stamp = now;
    }
    swap_buffers();
    if (prof) {
        u64 now = sceKernelGetProcessTimeWide();
        times[TNT3P_PROF_VBLANK] = (u32)(now - stamp);
        times[TNT3P_PROF_TOTAL] = (u32)(now - start_stamp);
        if (peak_track && all_live_at_start &&
            times[TNT3P_PROF_TOTAL] > g_tnt_3p_ultra_peak_total) {
            memcpy(g_tnt_3p_ultra_peak_us, times, sizeof(times));
            g_tnt_3p_ultra_peak_total = times[TNT3P_PROF_TOTAL];
        }
        tnt_vita_3p_prof_sample(times);
    }

    /* 0=continue, 1=match finished, 2=pause EXIT, 3=pause PROGRESS. */
    if (pause_action == TNT_VITA_PAUSE_ACTION_EXIT) return 2;
    if (pause_action == TNT_VITA_PAUSE_ACTION_PROGRESS) return 3;
    return (u8)(p1->frameAct.unkC && p2->frameAct.unkC && p3->frameAct.unkC &&
                p1->unk0 == 0 && p2->unk0 == 0 && p3->unk0 == 0);
}


/* TEST91 STAGE 1: Four-player Marathon/NONE via the exact source 4P
 * Mayan graphic and compact 4P board layouts. It intentionally has its
 * own frame loop so that the hardware-approved 3P core is untouched. */
#ifndef TNTV_4P_DEV_TWO_PADS
#define TNTV_4P_DEV_TWO_PADS 0
#endif
#if TNTV_4P_DEV_TWO_PADS != 0 && TNTV_4P_DEV_TWO_PADS != 1
#error "TNTV_4P_DEV_TWO_PADS must be 0 (retail) or 1 (test)"
#endif


/* TEST96: four-player Sprint has the same retail three-minute limit and
 * 60 Hz clock as the original 3P Sprint. A separate 45-second build
 * override is diagnostic ONLY. The ordinary default is always 180s. */
#ifndef TNTV_4P_SPRINT_SECONDS
#define TNTV_4P_SPRINT_SECONDS 180U
#endif
#if TNTV_4P_SPRINT_SECONDS < 15 || TNTV_4P_SPRINT_SECONDS > 180
#error "TNTV_4P_SPRINT_SECONDS must be 15..180"
#endif
#define TNTV_4P_SPRINT_TICKS (TNTV_4P_SPRINT_SECONDS * 60U)

/* Test98: original Ultra ends when one of four players achieves
 * 150 lines, not after a clock limit. This short 3-line diagnostic
 * override is compiled only into the two-controller test build. */
#ifndef TNTV_4P_ULTRA_GOAL
#define TNTV_4P_ULTRA_GOAL 150U
#endif
#if TNTV_4P_ULTRA_GOAL < 1 || TNTV_4P_ULTRA_GOAL > 150
#error "TNTV_4P_ULTRA_GOAL must be in the original 1..150 range"
#endif

/* Original Font-C countdown is expensive if rerasterized continuously.
 * For 4P Sprint with any original garbage type cache the exact timer
 * rectangle (right-aligned
 * at native x216, y266, matching the 3P compact-board timer), rebuilding
 * only when the remaining second changes. Each frame just memcpy rows.
 * Completely independent of 3P Ultra clock and all approved modes. */
typedef struct {
    u32 *tile;
    int x,y,w,h;
    u32 second;
    u8 active,valid;
} TNT4PSprintClockCache;
static TNT4PSprintClockCache g_tnt_4p_sprint_clock;

static void __attribute__((optimize("Os")))
tnt_vita_4p_sprint_clock_free(void) {
    free(g_tnt_4p_sprint_clock.tile);
    memset(&g_tnt_4p_sprint_clock,0,sizeof(g_tnt_4p_sprint_clock));
}

static void __attribute__((optimize("Os")))
tnt_vita_4p_sprint_clock_prepare(
    int board_x,int board_y,int cell
) {
    TNT4PSprintClockCache *t=&g_tnt_4p_sprint_clock;
    int x0,x1,y0,y1;
    tnt_vita_4p_sprint_clock_free();
    t->active=TRUE;
    x0=tnt_vita_source_x(126,board_x,cell)-3;
    x1=tnt_vita_source_x(224,board_x,cell)+4;
    y0=tnt_vita_source_y(263,board_y,cell)-3;
    y1=tnt_vita_source_y(283,board_y,cell)+3;
    if (x0<0 || x1>DISPLAY_WIDTH || y0<0 || y1>DISPLAY_HEIGHT ||
        x0>=x1 || y0>=y1) return;
    t->x=x0;t->y=y0;t->w=x1-x0;t->h=y1-y0;
    t->tile=malloc((size_t)t->w*t->h*sizeof(u32));
}

static void __attribute__((optimize("Os")))
tnt_vita_4p_sprint_clock_render(
    void *buffer,int board_x,int board_y,int cell,u32 elapsed_ticks
) {
    TNT4PSprintClockCache *t=&g_tnt_4p_sprint_clock;
    u32 remaining=elapsed_ticks<TNTV_4P_SPRINT_TICKS ?
        TNTV_4P_SPRINT_TICKS-elapsed_ticks : 0U;
    u32 second=remaining/60U;
    if (t->active && t->tile) {
        if (!t->valid || t->second!=second) {
            for (int row=0;row<t->h;row++) {
                size_t off=(size_t)(row+t->y)*DISPLAY_STRIDE+t->x;
                memcpy(g_tnt_gameplay_base_scene+off,
                    g_tnt_static_scene+off,(size_t)t->w*sizeof(u32));
            }
            tnt_vita_draw_original_time(
                g_tnt_gameplay_base_scene,board_x,board_y,cell,
                216,266,remaining);
            for (int row=0;row<t->h;row++) {
                size_t off=(size_t)(row+t->y)*DISPLAY_STRIDE+t->x;
                memcpy(t->tile+(size_t)row*t->w,
                    g_tnt_gameplay_base_scene+off,
                    (size_t)t->w*sizeof(u32));
            }
            t->second=second;t->valid=TRUE;
        }
        for (int row=0;row<t->h;row++)
            memcpy((u32 *)buffer+
                (size_t)(row+t->y)*DISPLAY_STRIDE+t->x,
                t->tile+(size_t)row*t->w,
                (size_t)t->w*sizeof(u32));
    } else
        tnt_vita_draw_original_time(
            buffer,board_x,board_y,cell,216,266,remaining);
}

static void __attribute__((optimize("Os")))
tnt_vita_init_multiplayer_4p_match(
    Tetris *players[4], Landfill *landfill, u32 seed, u8 screen,
    u8 game_type, u8 garbage_type
) {
    tnt_vita_3p_hp_tint_free();
    tnt_vita_3p_directed_free();
    tnt_vita_3p_ultra_hp_clock_free();
    tnt_vita_3p_ultra_hp_lines_free();
    tnt_vita_4p_sprint_clock_free();
    g_tnt_local_2p_fast_render=FALSE;
    g_tnt_3p_prof_active=FALSE;
    g_tnt_4p_prof_active=FALSE;
    memset(g_tnt_4p_prof_us,0,sizeof(g_tnt_4p_prof_us));
    memset(g_tnt_4p_hp_peak_us,0,sizeof(g_tnt_4p_hp_peak_us));
    g_tnt_4p_hp_frame_us=0;
    g_tnt_4p_hp_peak_h_us=0;
    g_tnt_4p_hp_peak_total=0;
    g_tnt_4p_hp_fallbacks=0;
    g_tnt_4p_hp_cache_mask=0;
    g_tnt_4p_hp_avg_total=0;
    g_tnt_4p_hp_avg_h=0;
    g_tnt_4p_hp_window_total=0;
    g_tnt_4p_hp_window_h=0;
    g_tnt_4p_hp_window_start=0;
    g_tnt_4p_hp_window_frames=0;
    memset(g_tnt_3p_scene_buffer_ready,0,
        sizeof(g_tnt_3p_scene_buffer_ready));
    tnt_vita_2p_moving_tex_cache_reset();
    tnt_vita_4p_cube_tile_cache_init();
    for (int i=1;i<4;i++)memset(players[i],0,sizeof(*players[i]));
    memset(landfill,0,sizeof(*landfill));
    g_playercount=4;
    landfill->type=garbage_type;
    Landfill_Init(landfill);
    landfill->type=garbage_type;
#if TNTV_4P_DEV_TWO_PADS
    /* Repeatable diagnostic starting P1 holder to test red border,
     * directed holder transfers and new P4 routing with two real pads.
     * Original RNG holder remains normal in the four-pad build. */
    if (garbage_type==LANDFILLTYPE_HOTPOTATO)
        landfill->unk30.unk0=0;
#endif
    for (int i=0;i<4;i++)
        tnt_vita_init_tetris_player(players[i],seed,screen,(u8)i,4,landfill);
    if (game_type==GAMETYPE_ULTRA) {
        /* As with source 3P Ultra, every player starts with 0
         * handicap and the identical line target. */
        for (int i=0;i<4;i++)
            players[i]->ultraGoal=TNTV_4P_ULTRA_GOAL;
    }
    FUN_80041260_twoliner();
}

static void __attribute__((optimize("Os")))
tnt_vita_deinit_multiplayer_4p_match(
    Tetris *players[4], Landfill *landfill
) {
    FUN_8004129c_fourliner();
    for (int i=0;i<4;i++)
        tnt_vita_deinit_tetris_player_state(players[i],(u8)i);
    Landfill_Deinit(landfill);
    tnt_vita_3p_cube_tile_cache_term();
    tnt_vita_3p_hp_tint_free();
    tnt_vita_3p_directed_free();
    tnt_vita_3p_ultra_hp_clock_free();
    tnt_vita_3p_ultra_hp_lines_free();
    tnt_vita_4p_sprint_clock_free();
    g_tnt_4p_prof_active=FALSE;
    g_playercount=1;
}

static void __attribute__((optimize("Os")))
tnt_vita_compose_4p_names(
    int board_x,int board_y,int cell,const char *names[4]
) {
    static const s16 x[4]={39,123,207,292};
    static const u8 rgb[4][3]={
        {255,0,0},{0,255,0},{0,0,255},{255,255,0}
    };
    for (int i=0;i<4;i++)
        tnt_vita_draw_font_c_text(
            g_tnt_static_scene,board_x,board_y,cell,
            x[i],234,names[i],rgb[i][0],rgb[i][1],rgb[i][2],255);
}

static void __attribute__((optimize("Os")))
tnt_vita_4p_restore_scene(void *buffer,int board_x,int board_y,int cell,u8 full) {
    if (full || !g_tnt_3p_scene_buffer_ready[backBuffer]) {
        memcpy(buffer,g_tnt_static_scene,sizeof(g_tnt_static_scene));
        g_tnt_3p_scene_buffer_ready[backBuffer]=TRUE;
        return;
    }
    tnt_vita_3p_scene_rect(buffer,
        tnt_vita_source_x(0,board_x,cell)-3,
        tnt_vita_source_y(0,board_y,cell)-3,
        tnt_vita_source_x(400,board_x,cell)+3,
        tnt_vita_source_y(78,board_y,cell)+3);
    /* Widen test77 partial restore just enough to include fourth well
     * and P4 finish sprites centered around source x326,y148. */
    tnt_vita_3p_scene_rect(buffer,
        tnt_vita_source_x(15,board_x,cell)-6,
        tnt_vita_source_y(55,board_y,cell)-6,
        tnt_vita_source_x(400,board_x,cell)+6,
        tnt_vita_source_y(282,board_y,cell)+6);
}

/* Test94Q: combine the original 4P partial static restore with the
 * preblended Hot Potato body instead of copying the same playfield twice.
 * This is pixel-identical to test93/test94P:
 *   static rectangle restored -> tinted body overwrites exactly its bounds.
 * The active holder's already-preblended rows are now the restore source
 * *inside* those bounds; other pixels come from the same static scene.
 * The original red pulsating rails are drawn afterward, unchanged.
 * Only called for four-player Hot Potato; 1P/2P/3P and 4P NONE use
 * their original restore paths. Returns FALSE without touching buffer
 * if the tile/holder cannot be verified, preserving the old safe path. */
static u8 __attribute__((optimize("Os")))
tnt_vita_4p_restore_scene_hp_fused(
    void *buffer,int board_x,int board_y,int cell,u8 full,u8 holder
) {
    const TNT3PHotPotatoTint *tile;
    u32 *dst = (u32 *)buffer;
    int lx=tnt_vita_source_x(15,board_x,cell)-6;
    int rx=tnt_vita_source_x(400,board_x,cell)+6;
    int ty=tnt_vita_source_y(55,board_y,cell)-6;
    int by=tnt_vita_source_y(282,board_y,cell)+6;
    int row;
    if (holder>=4) return FALSE;
    tile=&g_tnt_3p_hp_tint[holder];
    if (!tile->pixels) return FALSE;
    if (lx<0)lx=0;
    if (rx>DISPLAY_WIDTH)rx=DISPLAY_WIDTH;
    if (ty<0)ty=0;
    if (by>DISPLAY_HEIGHT)by=DISPLAY_HEIGHT;
    if (lx>=rx||ty>=by ||
        tile->x<lx||tile->x+tile->w>rx ||
        tile->y<ty||tile->y+tile->h>by ||
        tile->w<=0||tile->h<=0) return FALSE;

    if (full || !g_tnt_3p_scene_buffer_ready[backBuffer]) {
        /* The rare complete scene restore (initial frame/pause) is
         * unchanged; tint is copied once rather than by separate HP pass. */
        tnt_vita_4p_restore_scene(buffer,board_x,board_y,cell,full);
        return tnt_vita_3p_hp_tint_copy(
            buffer,holder,tile->x,tile->y,tile->w,tile->h);
    }

    /* TEST94R: The original upper strip overlaps the lower strip
     * across native source y55..78, yet the lower strip is copied
     * afterward and completely overwrites its middle pixels. Draw
     * the upper strip ONLY until the lower one starts, then copy
     * the narrow side edges where lower's x-range doesn't reach.
     * This gives the exact original final image, including sprite
     * tails near the sides, without a redundant large framebuffer
     * write. Restricted to 4P HP fused restoration; 4P NONE and
     * all previously approved 1P/2P/3P paths are unchanged. */
    {
        const int ux0=tnt_vita_source_x(0,board_x,cell)-3;
        const int ux1=tnt_vita_source_x(400,board_x,cell)+3;
        const int uy0=tnt_vita_source_y(0,board_y,cell)-3;
        const int uy1=tnt_vita_source_y(78,board_y,cell)+3;
        const int join=ty>uy0 && ty<uy1 ? ty : uy1;
        tnt_vita_3p_scene_rect(buffer,ux0,uy0,ux1,join);
        if (join<uy1) {
            if (ux0<lx)
                tnt_vita_3p_scene_rect(buffer,ux0,join,lx,uy1);
            if (rx<ux1)
                tnt_vita_3p_scene_rect(buffer,rx,join,ux1,uy1);
        }
    }

    for (row=ty;row<by;row++) {
        u32 *d=dst+(size_t)row*DISPLAY_STRIDE;
        const u32 *src=g_tnt_static_scene+(size_t)row*DISPLAY_STRIDE;
        if (row<tile->y || row>=tile->y+tile->h) {
            memcpy(d+lx,src+lx,(size_t)(rx-lx)*sizeof(u32));
        } else {
            /* The source of the exact pre-tinted region is replaced in
             * this same row copy. Never overwrite it with un-tinted art
             * and then redundantly draw the tint for a second time. */
            memcpy(d+lx,src+lx,(size_t)(tile->x-lx)*sizeof(u32));
            memcpy(d+tile->x,
                tile->pixels+(size_t)(row-tile->y)*tile->w,
                (size_t)tile->w*sizeof(u32));
            memcpy(d+tile->x+tile->w,src+tile->x+tile->w,
                (size_t)(rx-tile->x-tile->w)*sizeof(u32));
        }
    }
    return TRUE;
}

/* Test92 retains the approved four-board core renderer and now routes
 * completed matches to the real four-row tally. The diagnostic shortcut
 * only seeds source GameFinish states, not the result display itself. */
static u8 __attribute__((optimize("Os")))
tnt_vita_4p_core_frame(
    Tetris *players[4],Landfill *landfill,
    TNTVitaPauseMenu *pause_menu,u16 *end_delay,u8 *winner_mask,
    u32 *elapsed_ticks,u8 *pause_owner,u8 game_type,
    int board_x,int board_y,int cell
) {
    static const s16 well_x[4]={34,118,202,286};
    const int pf_cell=(8*cell+5)/11;
    const int pf_y=board_y+tnt_floor_div_s64((s64)(68-36)*cell,11);
    void *buffer=buffers[backBuffer].data;
    const u8 prof=g_tnt_4p_prof_active;
    const u8 hp=(u8)(landfill && landfill->type==LANDFILLTYPE_HOTPOTATO);
    u8 hp_body_fused=FALSE;
    u8 was_paused=TNT_Vita_PauseIsActive(pause_menu);
    u8 all_live_start=(u8)(!was_paused && players[0]->unk2 &&
        players[1]->unk2 && players[2]->unk2 && players[3]->unk2);
    u64 t0=prof?sceKernelGetProcessTimeWide():0, stamp=t0;
    s32 pause_action;
    int live_count=0;
    D_801109F4=tnt_vita_60hz_delta(TRUE);
    if (!D_801109F4)D_801109F4=1;
#if TNTV_4P_DEV_TWO_PADS
    /* Diagnostic ONLY: mirror DS4-1 to P3 and DS4-2 to P4. P3 and P4
     * move, but these are NOT four independent controller inputs. */
    for (int i=2;i<4;i++) {
        int src=i-2;
        g_PV_arr[i].unk24=g_PV_arr[src].unk24;
        g_PV_arr[i].unk28.unk88=g_PV_arr[src].unk28.unk88;
        g_PV_arr[i].unk28.unk8C=g_PV_arr[src].unk28.unk8C;
        g_PV_arr[i].unk28.unk84=g_PV_arr[src].unk28.unk84;
    }
#endif
    if (!was_paused) {
        for (int i=0;i<4;i++) {
            if (players[i]->unk2 && !players[i]->frameAct.unkC &&
                (g_PV_arr[i].unk28.unk8C&CONT_START)) {
                *pause_owner=(u8)i;
                TNT_Vita_TetrisSetGlobalPointers(players[i],(u8)i);
                TNT_Vita_PauseStart(pause_menu,players[i]);
                for (int j=0;j<4;j++)
                    if (j!=i)
                        Tetris_FUN_80054220_fourliner(players[j],TRUE);
                was_paused=TRUE;
                break;
            }
        }
    }
    TNT_Vita_TetrisSetGlobalPointers(
        players[*pause_owner],*pause_owner);
    pause_action=TNT_Vita_PauseUpdate(pause_menu,
        players[*pause_owner],&g_PV_arr[*pause_owner]);
    for (int i=0;i<4;i++)
        TNT_Vita_SetGameplayAlpha(players[i],(u8)pause_menu->fade.alpha);
    if (pause_action==TNT_VITA_PAUSE_ACTION_CONTINUE)
        for (int i=0;i<4;i++)
            if (i!=*pause_owner) {
                Tetris_FUN_80054220_fourliner(players[i],FALSE);
                FUN_800535B8_fiveliner(&g_PV_arr[i]);
                g_PV_arr[i].unk20=0;
            }
    if (!was_paused) {
        for (int i=0;i<4;i++)live_count+=players[i]->unk2?1:0;
        if (live_count)*elapsed_ticks+=D_801109F4;
        TNT_Vita_MetalField_Update(D_801109F4);
        for (int i=0;i<4;i++) {
            TNT_Vita_TetrisSetGlobalPointers(players[i],(u8)i);
            (void)Tetris_Update(players[i]);
        }
        live_count=0;
        for (int i=0;i<4;i++) {
            Tetris *tp=players[i];
            if (tp->unk2&&tp->frameAct.unk3&&!tp->frameAct.unkC) {
                TNT_Vita_TetrisSetGlobalPointers(tp,(u8)i);
                FrameAct_calls_gamefinish_something2(
                    &tp->frameAct,GAMEFINISHTYPE_LOSE);
                tp->unk2=FALSE;
                func_800726A4(landfill,(u8)i);
            }
            live_count+=tp->unk2?1:0;
        }
#if TNTV_4P_DEV_TWO_PADS
        /* Test92 developer finish shortcut, no game-rule changes in the
         * normal VPK: hold BOTH L/R and PRESS B on controller 1 while all
         * four boards are alive. End P2/P3/P4 through the actual GameFinish
         * LOSE path and allow original one-survivor logic to WIN P1.
         * The normal animation still runs; P1 line count is not faked. */
        if (live_count==4 &&
            (g_PV_arr[0].unk28.unk88 & (CONT_L|CONT_R)) ==
                (CONT_L|CONT_R) &&
            (g_PV_arr[0].unk28.unk8C & CONT_B)) {
            for (int i=1;i<4;i++) {
                TNT_Vita_TetrisSetGlobalPointers(players[i],(u8)i);
                FrameAct_calls_gamefinish_something2(
                    &players[i]->frameAct,GAMEFINISHTYPE_LOSE);
                players[i]->unk2=FALSE;
                func_800726A4(landfill,(u8)i);
            }
            live_count=1;
        }
#endif
        if (live_count>1 && game_type==GAMETYPE_SPRINT &&
            *elapsed_ticks>=TNTV_4P_SPRINT_TICKS) {
            /* Source Game_QueryGameOver first processes all newly
             * topped boards. At 3:00 surviving participants with the
             * greatest line count WIN, including ties. Never award a
             * prior top-out. Identical test87/88D 3P ordering. */
            u32 max_lines=0;
            *elapsed_ticks=TNTV_4P_SPRINT_TICKS;
            for (int i=0;i<4;i++)
                if (players[i]->unk2) {
                    u32 lines=getLineCount(
                        &players[i]->gameStats.linesInfo);
                    if (lines>max_lines) max_lines=lines;
                }
            for (int i=0;i<4;i++) {
                if (players[i]->unk2) {
                    u32 lines=getLineCount(
                        &players[i]->gameStats.linesInfo);
                    u8 wins=(u8)(lines==max_lines);
                    TNT_Vita_TetrisSetGlobalPointers(players[i],(u8)i);
                    FrameAct_calls_gamefinish_something2(
                        &players[i]->frameAct,
                        wins?GAMEFINISHTYPE_WIN:GAMEFINISHTYPE_LOSE);
                    players[i]->unk2=FALSE;
                    func_800726A4(landfill,(u8)i);
                    if (wins) *winner_mask|=(u8)(1U<<i);
                }
            }
        }
        if (live_count>1 && game_type==GAMETYPE_ULTRA) {
            /* Identical source Game_QueryGameOver winner ordering to
             * hardware-approved 3P Ultra: process all new topouts first,
             * finish if ANY player reached the goal, then select among
             * living participants by maximum line count AND minimum
             * elapsed time. This permits tied winners, but never
             * resurrects an eliminated player. */
            u8 reached=FALSE;
            u32 max_lines=0;
            u32 min_time=0xFFFFFFFFU;
            for (int i=0;i<4;i++) {
                if (getLineCount(&players[i]->gameStats.linesInfo)
                    >= players[i]->ultraGoal)
                    reached=TRUE;
            }
            if (reached) {
                for (int i=0;i<4;i++) {
                    if (players[i]->unk2) {
                        u32 lines=getLineCount(
                            &players[i]->gameStats.linesInfo);
                        if (lines>max_lines)max_lines=lines;
                        if (players[i]->gameStats.elapsedTime<min_time)
                            min_time=players[i]->gameStats.elapsedTime;
                    }
                }
                for (int i=0;i<4;i++) {
                    if (players[i]->unk2) {
                        u32 lines=getLineCount(
                            &players[i]->gameStats.linesInfo);
                        u8 wins=(u8)(
                            (lines>=players[i]->ultraGoal ||
                             lines>=max_lines) &&
                            players[i]->gameStats.elapsedTime==min_time);
                        TNT_Vita_TetrisSetGlobalPointers(players[i],(u8)i);
                        FrameAct_calls_gamefinish_something2(
                            &players[i]->frameAct,
                            wins?GAMEFINISHTYPE_WIN:GAMEFINISHTYPE_LOSE);
                        players[i]->unk2=FALSE;
                        func_800726A4(landfill,(u8)i);
                        if (wins)*winner_mask|=(u8)(1U<<i);
                    }
                }
            }
        }
        if (live_count==1) {
            for (int i=0;i<4;i++)
                if (players[i]->unk2) {
                    TNT_Vita_TetrisSetGlobalPointers(players[i],(u8)i);
                    FrameAct_calls_gamefinish_something2(
                        &players[i]->frameAct,GAMEFINISHTYPE_WIN);
                    players[i]->unk2=FALSE;
                    func_800726A4(landfill,(u8)i);
                    *winner_mask|=(u8)(1U<<i);
                    break;
                }
        }
        if (!live_count || (*winner_mask != 0)) {
            if (*end_delay>D_801109F4)*end_delay-=(u16)D_801109F4;
            else {
                *end_delay=0;
                for (int i=0;i<4;i++)
                    if (g_PV_arr[i].unk28.unk8C&(CONT_A|CONT_START))
                        for (int j=0;j<4;j++)
                            gamefinish_80052D48_twoliner(
                                &players[j]->frameAct.unk10);
            }
        }
    }
    if (prof) {
        u64 now=sceKernelGetProcessTimeWide();
        g_tnt_4p_prof_us[0]=(u32)(now-stamp);stamp=now;
    }
    {
        u8 full=(u8)(was_paused||TNT_Vita_PauseIsActive(pause_menu));
        if (full)memset(g_tnt_3p_scene_buffer_ready,0,
            sizeof(g_tnt_3p_scene_buffer_ready));
        /* Same HotPotato_Update stage after Tetris_Update as before,
         * but done BEFORE deciding which preblended backdrop is needed.
         * 3P still calls HotPotato_Update in its legacy renderer. */
        if (hp && !full && landfill->list &&
            landfill->garbage_idx>=g_playercount)
            HotPotato_Update(&landfill->unk30);
        if (hp && landfill->list &&
            landfill->garbage_idx>=g_playercount)
            hp_body_fused=tnt_vita_4p_restore_scene_hp_fused(
                buffer,board_x,board_y,cell,full,landfill->unk30.unk0);
        if (!hp_body_fused)
            tnt_vita_4p_restore_scene(buffer,board_x,board_y,cell,full);
    }
    if (prof) {
        u64 now=sceKernelGetProcessTimeWide();
        g_tnt_4p_prof_us[1]=(u32)(now-stamp);stamp=now;
    }
    /* Test94Q: red static body already restored when cache valid.
     * The original 32-step pulsing rails are still drawn after the
     * background but before moving/locked minos. */
    if (hp)
        tnt_vita_3p_hotpotato_render(
            buffer,landfill,board_x,board_y,cell,
            (u8)(was_paused || TNT_Vita_PauseIsActive(pause_menu)),
            hp_body_fused);
    if (prof) {
        u64 now=sceKernelGetProcessTimeWide();
        g_tnt_4p_hp_frame_us=hp?(u32)(now-stamp):0;
        stamp=now; /* B0 now measures board 0 ONLY, excluding HP. */
    }
    g_tnt_local_2p_fast_render=TRUE;
    for (int i=0;i<4;i++) {
        int pf_x=board_x+
            tnt_floor_div_s64((s64)(well_x[i]-83)*cell,11)+1;
        tnt_vita_render_tetris_player(buffer,
            players[i],(u8)i,pf_x,pf_y,pf_cell,board_x,board_y,cell);
        if (tnt_vita_3p_ultra_hp_line_tile_enabled(i))
            tnt_vita_3p_ultra_hp_line_tile_render(
                buffer,i,board_x,board_y,cell,
                getLineCount(&players[i]->gameStats.linesInfo));
        else {
            static const s16 counter_x[4]={39,123,207,292};
            char text[16];
            snprintf(text,sizeof(text),"%u",(unsigned)getLineCount(
                &players[i]->gameStats.linesInfo));
            tnt_vita_draw_font_c_text(buffer,board_x,board_y,cell,
                counter_x[i],248,text,255,255,255,255);
        }
        if (prof) {
            u64 now=sceKernelGetProcessTimeWide();
            g_tnt_4p_prof_us[2+i]=(u32)(now-stamp);stamp=now;
        }
    }
    g_tnt_local_2p_fast_render=FALSE;
    /* 4P Sprint countdown is in the source compact footer, below all
     * line counts. The tile updates once per visible second. */
    if (game_type==GAMETYPE_SPRINT)
        tnt_vita_4p_sprint_clock_render(
            buffer,board_x,board_y,cell,*elapsed_ticks);
    else if (game_type==GAMETYPE_ULTRA) {
        /* Reuse test89F 3P once-per-second Ultra cache without its
         * old 3P frame loop. Goal was baked into g_tnt_static_scene;
         * original elapsed-time field is rendered at x217,y278.
         * Draw the existing Directed markers AFTER this tile to
         * protect P2/P3/P4's source y267..278 target display. */
        tnt_vita_3p_ultra_hp_clock_render(
            buffer,board_x,board_y,cell,*elapsed_ticks);
    }
    /* Test95: source original AimedAt target markers, one cache per
     * player; draw after the line-count tiles so no HUD restore can
     * obscure an indicator at source y267..278. */
    if (landfill && landfill->type==LANDFILLTYPE_DIRECTED)
        tnt_vita_3p_directed_render(
            buffer,landfill,board_x,board_y,cell);
    if (!TNT_Vita_PauseIsActive(pause_menu)||
        !g_tnt_milestone_gxm_ready||
        TNT_Vita_PauseGxmRender(
            &buffers[backBuffer].surface,buffers[backBuffer].sync,
            board_x,board_y,cell,pause_menu->selected,
            pause_menu->pulse)<0) {
        TNT_Vita_PauseRender(
            buffer,DISPLAY_STRIDE,DISPLAY_WIDTH,DISPLAY_HEIGHT,
            board_x,board_y,cell,pause_menu);
    }
    /* Single final exact opaque cybergrid pass in swap_buffers. */
    g_tnt_presentation_crop=(u8)tnt_vita_gameplay_side_mask_width(0);
    if (prof) {
        u64 now=sceKernelGetProcessTimeWide();
        g_tnt_4p_prof_us[6]=(u32)(now-stamp);stamp=now;
    }
    swap_buffers();
    if (prof) {
        u64 now=sceKernelGetProcessTimeWide();
        g_tnt_4p_prof_us[7]=(u32)(now-stamp);
        g_tnt_4p_prof_us[8]=(u32)(now-t0);
        if (hp) {
            if (all_live_start &&
                g_tnt_4p_prof_us[8]>g_tnt_4p_hp_peak_total) {
                g_tnt_4p_hp_peak_total=g_tnt_4p_prof_us[8];
                g_tnt_4p_hp_peak_h_us=g_tnt_4p_hp_frame_us;
                memcpy(g_tnt_4p_hp_peak_us,g_tnt_4p_prof_us,
                    sizeof(g_tnt_4p_hp_peak_us));
            }
            if (!g_tnt_4p_hp_window_start)
                g_tnt_4p_hp_window_start=now;
            g_tnt_4p_hp_window_total+=g_tnt_4p_prof_us[8];
            g_tnt_4p_hp_window_h+=g_tnt_4p_hp_frame_us;
            g_tnt_4p_hp_window_frames++;
            if (now-g_tnt_4p_hp_window_start>=500000U &&
                g_tnt_4p_hp_window_frames) {
                g_tnt_4p_hp_avg_total=(u32)(
                    g_tnt_4p_hp_window_total/g_tnt_4p_hp_window_frames);
                g_tnt_4p_hp_avg_h=(u32)(
                    g_tnt_4p_hp_window_h/g_tnt_4p_hp_window_frames);
                g_tnt_4p_hp_window_total=0;
                g_tnt_4p_hp_window_h=0;
                g_tnt_4p_hp_window_frames=0;
                g_tnt_4p_hp_window_start=now;
            }
        }
    }
    if (pause_action==TNT_VITA_PAUSE_ACTION_EXIT)return 2;
    if (pause_action==TNT_VITA_PAUSE_ACTION_PROGRESS)return 3;
    for (int i=0;i<4;i++)
        if (players[i]->unk2 || !players[i]->frameAct.unkC ||
            players[i]->unk0!=0) return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    OSContPad pad;
    OSContPad pad2;
    OSContPad pad3;
    OSContPad pad4;
    Tetris tetris;
    Tetris opponent_tetris;
    Tetris *third_tetris = NULL;
    Tetris *fourth_tetris = NULL;
    Landfill opponent_landfill;
    TNTMayanLargeFire mayan_large_fire;
    TNTVitaPauseMenu pause_menu;
    TNTVitaMainMenu main_menu;
    TNTVitaOptionsMenu options_menu;
    TNTVitaDataMenu data_menu;
    TNTVitaScoresMenu scores_menu;
    TNTVitaCredits credits;
    TNTVitaAudioMenu audio_menu;
    TNTVitaOnePlayerMenu onep_menu;
    TNTVitaMultiMenu multi_menu;
    TNTVitaTally tally;
    TNTVitaWonderViewer wonder_viewer;
    TNTVitaWonderMilestone wonder_milestone;
    TNTVitaWonderComplete wonder_complete;
    TNTVitaWonderFinale wonder_finale;
    TNTVitaWonderHall wonder_hall;
    TNTHaluciFrame haluci_frame;
    char current_player_name[9] = { 0 };
    char current_opponent_name[9] = { 0 };
    char current_player3_name[9] = { 0 };
    char current_player4_name[9] = { 0 };
    s8 active_profile_slot = -1;
    u8 active_game_type = GAMETYPE_MARATHON;
    u8 active_garbage_type = LANDFILLTYPE_NONE;
    u8 opponent_active = FALSE;
    u8 multiplayer_active = FALSE;
    u8 multiplayer_player_count = 2;
    u8 tally_return_to_multi = FALSE;
    u8 multiplayer_pause_owner = 0;
    u8 opponent_rank = 0;
    u8 opponent_winner_mask = 0;
    u16 opponent_end_delay = 120;
    u8 gameplay_needs_reset = TRUE;
    u8 attract_active = FALSE;
    u8 attract_exit_hold = 0; /* retail main state 17: 30 no-draw frames */
    u8 haluci_active = FALSE;
    u16 attract_ticks = 0;
    u8 attract_prompt_phase = 1;
    s8 pending_wonder_milestone = -1;
    u8 wonder_origin = TNT_WONDER_ORIGIN_NONE;
    u32 outer_game_elapsed = 0;
    u16 endgame_exit_delay = 120;
    u8 developer_active = TRUE;
    u16 developer_tick = 0;
    u8 intro_active = FALSE;
    u8 intro_fading = FALSE;
    u16 intro_fade_tick = 0;
    u8 intro_sfx_next = 0;
    u8 title_active = FALSE;
    u8 title_fade_tick = 0;
    u16 title_wait_tick = 0;
    memset(&pad, 0, sizeof(pad));
    memset(&pad2, 0, sizeof(pad2));
    memset(&pad3, 0, sizeof(pad3));
    memset(&pad4, 0, sizeof(pad4));
    memset(&tetris, 0, sizeof(tetris));
    memset(&opponent_tetris, 0, sizeof(opponent_tetris));
    memset(&opponent_landfill, 0, sizeof(opponent_landfill));
    TNT_Vita_PauseInit(&pause_menu);
    memset(&main_menu, 0, sizeof(main_menu));
    memset(&options_menu, 0, sizeof(options_menu));
    memset(&data_menu, 0, sizeof(data_menu));
    memset(&scores_menu, 0, sizeof(scores_menu));
    memset(&credits, 0, sizeof(credits));
    memset(&audio_menu, 0, sizeof(audio_menu));
    memset(&onep_menu, 0, sizeof(onep_menu));
    memset(&multi_menu, 0, sizeof(multi_menu));
    memset(&tally, 0, sizeof(tally));
    memset(&wonder_viewer, 0, sizeof(wonder_viewer));
    memset(&wonder_milestone, 0, sizeof(wonder_milestone));
    memset(&wonder_complete, 0, sizeof(wonder_complete));
    memset(&wonder_finale, 0, sizeof(wonder_finale));
    memset(&wonder_hall, 0, sizeof(wonder_hall));
    memset(&haluci_frame, 0, sizeof(haluci_frame));

    /* Test126R: provide original title/developer images exclusively from
     * the user's local ROM and cache, before any renderer uses them.
     * Private integration experiment; remaining data are still embedded.
     */
    /* Test196Q: show an actionable original-ROM setup screen on failure.
     * No original gameplay/save/audio initialization occurs until a valid
     * user-owned ROM and all derived assets have passed verification.
     * The regular successful startup still creates GXM at its old location.
     */
    int tntv_setup_graphics_ready=0;
    if(graphics_init()<0) { sceKernelExitProcess(1); return 1; }
    tntv_setup_graphics_ready=1;
    TNTV_ROM_SetProgress(tntv_rom_loading_progress);
    tntv_rom_loading_progress("FOUND");
    tntv_startup_trace("212D 01: main entered, validating local ROM");
    for (;;) {
        int boot_result=TNTV_ROM_BridgeBoot();
        if (boot_result==0) { TNTV_ROM_SetProgress(NULL); break; }
        if (!tntv_rom_setup_screen(boot_result,&tntv_setup_graphics_ready)) {
            sceKernelExitProcess(1);
            return 1;
        }
    }

    /*
     * Original boot-time player/global initialization.  This must happen
     * before PieceHold_Init so the sine table and 12-bit piece colors are
     * ready when MobilePiece and PieceHold animation state is created.
     */
    tntv_startup_trace("212D 02: verified ROM/cache/music; game globals");
    FUN_80053538_fiveliner();
    FUN_80053500_fiveliner();
    TNT_Vita_CubeTilesInit();
    TNT_Vita_MetalField_Init();
    TNT_Vita_MayanLargeFire_Init(&mayan_large_fire);
    tntv_startup_trace("212D 03: original geometry initialized");

    /* Load the original 0x1900-byte SRAM image from Vita storage, or create
     * source-faithful fresh SRAM (including the original converted game_id
     * quirk) on the first run. */
    (void)TNT_Vita_SramLoadOrInit(
        tnt_vita_cycles_to_nsec32(tnt_vita_n64_time_cycles32()));
    (void)TNT_Vita_AudioInit();
    tntv_startup_trace("212D 04: audio initialized");
    TNT_Vita_MusicPlayTitle();
    tnt_vita_onep_load_saved_profiles(&onep_menu);
    tnt_vita_init_tetris_state(&tetris, tnt_vita_game_seed_from_id(), 0);

    g_PV_ptr = &g_PV_arr[0];
    FUN_069580_800A34A8_tenliner_allocs_heap(&g_PV_ptr->contQ);
    PV_set_controller_no(0);

    FUN_026C80_80060ad4_oneliner_calls_fun(&g_PV_ptr->unk28);
    FUN_026C80_80060b04_twelveliner_loops_32t(&g_PV_ptr->unk28, 0x0F00, 8, 4);
    FUN_026C80_80060b04_twelveliner_loops_32t(&g_PV_ptr->unk28, 0xC000, 16, 16);

    /* Player 1 has no physical Vita controller in One Player OPPONENT, but
     * the original AI writes through a normal PlayerVars/controller-repeat
     * object. Allocate that queue once so its synthetic input is isolated. */
    g_PV_ptr = &g_PV_arr[1];
    FUN_069580_800A34A8_tenliner_allocs_heap(&g_PV_ptr->contQ);
    PV_set_controller_no(1);
    FUN_026C80_80060ad4_oneliner_calls_fun(&g_PV_ptr->unk28);
    FUN_026C80_80060b04_twelveliner_loops_32t(&g_PV_ptr->unk28, 0x0F00, 8, 4);
    FUN_026C80_80060b04_twelveliner_loops_32t(&g_PV_ptr->unk28, 0xC000, 16, 16);

    /* Local 3P/4P uses the same normal PlayerVars/controller-queue path as
     * P1/P2.  Keep P3/P4 queues allocated for the process lifetime too; they
     * remain dormant until physical controller ports 3/4 participate in the
     * MULTI frontend/game.  Test67 exposed that merely having PlayerVars slots
     * is insufficient: ControllerQueue owns heap-backed ring/controller data. */
    for (int i = 2; i < 4; i++) {
        g_PV_ptr = &g_PV_arr[i];
        FUN_069580_800A34A8_tenliner_allocs_heap(&g_PV_ptr->contQ);
        PV_set_controller_no((s16)i);
        FUN_026C80_80060ad4_oneliner_calls_fun(&g_PV_ptr->unk28);
        FUN_026C80_80060b04_twelveliner_loops_32t(
            &g_PV_ptr->unk28, 0x0F00, 8, 4);
        FUN_026C80_80060b04_twelveliner_loops_32t(
            &g_PV_ptr->unk28, 0xC000, 16, 16);
    }
    g_PV_ptr = &g_PV_arr[0];

    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    tntv_startup_trace("212D 05: input/player objects ready; graphics init");

    /* If the setup screen initialized GXM, keep those buffers. Otherwise
     * preserve the unchanged normal first-boot initialization order.
     */
    if (!tntv_setup_graphics_ready && graphics_init() < 0) {
        sceKernelExitProcess(1);
        return 1;
    }

    tntv_startup_trace("212D 06: display memory initialized; GXM scenes init");
    g_tnt_milestone_gxm_ready =
        TNT_Vita_WonderMilestoneGxmInit() == 0;
    g_tnt_gameplay_gxm_ready = g_tnt_milestone_gxm_ready;

    /*
     * Preserve the original 1P 11-pixel cell layout at approximately 20 Vita
     * pixels per cell.  Centering the original 400x300 coordinate space at
     * this scale places its board origin (83,36) at (267,65), rather than
     * anchoring the whole scene to the old diagnostic board at x=380.
     */
    const int board_x = TNT_PRESENT_BOARD_X;
    const int board_y = TNT_PRESENT_BOARD_Y;
    const int cell = TNT_PRESENT_CELL;

    /* Precompose both root-menu static frames before presentation begins.
     * This keeps title->menu handoff free of a one-frame software-resample
     * stall and leaves all steady-state menu frames cache-based. */
    tntv_startup_trace("212D 07: GXM scenes ready; precomposing menu caches");
    tnt_vita_main_menu_build_caches(board_x, board_y, cell);
    tnt_vita_onep_build_cache(board_x, board_y, cell);
    tnt_vita_tally_build_cache(board_x, board_y, cell);
    tnt_vita_wonder_build_cache(board_x, board_y, cell);

    /* Original boot flow reaches the Blue Planet Software / H2O developer
     * page, then the animated sand cinematic, then the Tetris start screen. */
    tnt_vita_build_ci8_index_scene(
        g_tnt_static_scene, board_x, board_y, cell, g_tnt_developer_ci8);
    clear_screen(buffers[0].data, COLOR(0, 0, 0));
    clear_screen(buffers[1].data, COLOR(0, 0, 0));

    tntv_startup_trace("212D 08: first main render loop entered");
    while (1) {
        /* TEST103A: do not show stale one-player stats in any other UI. */
        g_tnt103a_1p_active = FALSE;
        g_tnt_test103a_measure_gxm = FALSE;
        /* Most states use the full centered 4:3 surround. Gameplay overrides
         * this below with the established conservative CRT crop. */
        g_tnt_presentation_crop = 0;
        g_tnt_intro_shim_active = FALSE;
        /* Temporary 2P profiler retained dormant for future diagnostics. */
        g_tnt_ai_profile_active = FALSE;

        /* Refresh external controller topology while any local-multiplayer
         * state is relevant. Root-menu hotplug remains live; during setup/game
         * the first two verified DS4 ports become player 0 / player 1. */
        if (main_menu.active || multi_menu.active || multiplayer_active)
            tnt_vita_refresh_multiplayer_ports();

        g_PV_ptr = &g_PV_arr[0];
        if ((multi_menu.active || multiplayer_active) &&
            g_tnt_multi_ports[0] >= 0)
            vita_read_pad_port(&pad, g_tnt_multi_ports[0]);
        else
            vita_read_pad(&pad);

        /* SELECT is intentionally not mapped into the N64 controller state.
         * L+R+SELECT therefore makes a safe global developer chord.  Suppress
         * L/R on the chord frame so toggling the counter cannot also trigger a
         * gameplay shoulder-button action. */
        {
            SceCtrlData debug_ctrl;
            const u32 debug_chord =
                SCE_CTRL_LTRIGGER | SCE_CTRL_RTRIGGER | SCE_CTRL_SELECT;
            memset(&debug_ctrl, 0, sizeof(debug_ctrl));
            sceCtrlPeekBufferPositive(0, &debug_ctrl, 1);
            if ((debug_ctrl.buttons & debug_chord) == debug_chord) {
                pad.button &= (u16)~(CONT_L | CONT_R);
                if (!g_tnt_fps_combo_down) {
                    g_tnt_fps_counter_enabled ^= 1U;
                    g_tnt_fps_combo_down = TRUE;
                }
            } else {
                g_tnt_fps_combo_down = FALSE;
            }
        }

        FUN_069580_800A3300_nineliner_mod300(&g_PV_ptr->contQ, &pad);
        func_800A33E4(&g_PV_ptr->contQ);
        g_PV_ptr->unk1C = g_PV_ptr->contQ.unk14;
        g_PV_ptr->unk24 = (~g_PV_ptr->unk20) & g_PV_ptr->unk1C->unk0;
        g_PV_ptr->unk20 = g_PV_ptr->unk1C->unk0;

        g_PV_ptr->unk28.unk88 = g_PV_ptr->unk1C->unk0;
        g_PV_ptr->unk28.unk8C = g_PV_ptr->unk24;
        FUN_026C80_GU_or_ControllerRepeat_Update(
            &g_PV_ptr->unk28,
            g_PV_ptr->unk28.unk88,
            1
        );

        if (multi_menu.active || multiplayer_active) {
            OSContPad *extra_pads[3] = {&pad2, &pad3, &pad4};
            int max_extra = multi_menu.active ? 3 :
                (multiplayer_active ? (int)multiplayer_player_count - 1 : 0);
            if (max_extra < 0) max_extra = 0;
            if (max_extra > 3) max_extra = 3;
            for (int slot = 1; slot <= max_extra; slot++) {
                OSContPad *slot_pad = extra_pads[slot - 1];
                g_PV_ptr = &g_PV_arr[slot];
                memset(slot_pad, 0, sizeof(*slot_pad));
                if (g_tnt_multi_ports[slot] >= 0)
                    vita_read_pad_port(slot_pad, g_tnt_multi_ports[slot]);
                FUN_069580_800A3300_nineliner_mod300(&g_PV_ptr->contQ, slot_pad);
                func_800A33E4(&g_PV_ptr->contQ);
                g_PV_ptr->unk1C = g_PV_ptr->contQ.unk14;
                g_PV_ptr->unk24 = (~g_PV_ptr->unk20) & g_PV_ptr->unk1C->unk0;
                g_PV_ptr->unk20 = g_PV_ptr->unk1C->unk0;
                g_PV_ptr->unk28.unk88 = g_PV_ptr->unk1C->unk0;
                g_PV_ptr->unk28.unk8C = g_PV_ptr->unk24;
                FUN_026C80_GU_or_ControllerRepeat_Update(
                    &g_PV_ptr->unk28, g_PV_ptr->unk28.unk88, 1);
            }
            g_PV_ptr = &g_PV_arr[0];
        }

        /* Animated/cinematic states can run for long stretches without a
         * controller edge. Tell Vita power management the app is actively
         * presenting content so the display does not dim mid-cinematic.
         * This is a periodic activity tick, not a permanent power lock. */
        if (developer_active || intro_active || attract_active || attract_exit_hold ||
            haluci_active || credits.active || wonder_milestone.active ||
            wonder_complete.active || wonder_finale.active || wonder_hall.active)
            (void)sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DEFAULT);

        if (haluci_active) {
            void *buffer = buffers[backBuffer].data;
            u32 delta = tnt_vita_60hz_delta(TRUE);

            /* Retail main state 13 has no normal exit path. START toggles the
             * previous-frame feedback transform inside haluci.c; quitting the
             * original easter egg requires resetting the console.
             *
             * The validated Test-1 renderer was CPU-only and therefore far too
             * slow. Normal HALUCI now keeps only source animation/feedback
             * preparation on CPU and submits the actual spiral rasterization to
             * the existing GXM renderer. */
            if (g_tnt_gameplay_gxm_ready) {
                TNT_Vita_HaluciPrepareFrame(
                    &haluci_frame,
                    (const u32 *)buffers[frontBuffer].data,
                    DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                    board_x, board_y, cell,
                    (g_PV_ptr->unk24 & CONT_START) != 0, delta);
                if (TNT_Vita_HaluciGxmRender(
                        &buffers[backBuffer].surface, buffers[backBuffer].sync,
                        board_x, board_y, cell, &haluci_frame) < 0) {
                    /* HALUCI is reset-only, so if its GPU path unexpectedly
                     * fails on hardware, switch subsequent frames to the known
                     * software fallback rather than repeatedly failing GXM. */
                    g_tnt_gameplay_gxm_ready = FALSE;
                    clear_screen(buffer, COLOR(0, 0, 0));
                }
            } else {
                TNT_Vita_HaluciRender(
                    buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                    board_x, board_y, cell,
                    (g_PV_ptr->unk24 & CONT_START) != 0, delta);
            }
            swap_buffers();
            continue;
        }

        if (attract_exit_hold != 0) {
            /* Preserve the gameplay crop during the source-faithful two-buffer
             * hold so the surround does not jump inward for these 30 frames. */
            g_tnt_presentation_crop = 12;

            /* Retail main state 17 draws nothing for exactly 30 scheduler
             * frames after Game_Deinit, then reinitializes frontend state 4.
             * The N64 continues alternating its two untouched framebuffers,
             * effectively holding the final two attract frames. Do the same:
             * submit the existing back buffer without modifying either one. */
            swap_buffers();
            attract_exit_hold--;
            if (attract_exit_hold == 0) {
                D_800CFEE8 = 4;
                tnt_vita_main_menu_enter(&main_menu, g_PV_ptr);
            }
            continue;
        }

        if (developer_active) {
            void *buffer = buffers[backBuffer].data;
            u32 delta = tnt_vita_60hz_delta(TRUE);
            u8 fade_alpha;
            u8 leave_developer = FALSE;

            /* Source 046DD0.c: 10-tick fade from black, 300-tick hold,
             * skippable with A after the first 60 hold ticks, then a 10-tick
             * fade to black before the animated sand intro. */
            developer_tick += (u16)delta;
            if (developer_tick < 10) {
                fade_alpha = (u8)(((10U - developer_tick) * 255U) / 10U);
            } else if (developer_tick < 310) {
                fade_alpha = 0;
                if ((g_PV_ptr->unk24 & CONT_A) && developer_tick >= 70)
                    developer_tick = 310;
            } else if (developer_tick < 320) {
                fade_alpha = (u8)(((developer_tick - 310U) * 255U) / 10U);
            } else {
                fade_alpha = 255;
                leave_developer = TRUE;
            }

            tnt_vita_render_ci8_scene(
                buffer, board_x, board_y, cell,
                g_tnt_developer_rgba16, 0, 0, 0, fade_alpha);
            tnt_vita_draw_frontend_pillarbox(buffer, board_x, cell);
            swap_buffers();

            if (leave_developer) {
                developer_active = FALSE;
                intro_active = TRUE;
                intro_fading = FALSE;
                intro_fade_tick = 0;
                intro_sfx_next = 0;
                TNT_Vita_IntroReset();
                g_tnt_60hz_last_us = 0;
                g_tnt_60hz_fraction = 0;
            }
            continue;
        }

        if (intro_active) {
            u32 delta = tnt_vita_60hz_delta(TRUE);
            u8 white_fade = 0;
            u8 leave_intro = FALSE;

            /* Source 0449D0.c / resources 985+986: advance the canonical
             * Borg animation at retrace speed. A or Start skips immediately;
             * natural completion occurs at tick 3950. TNTV deliberately uses
             * a much quicker native handoff flash than the N64's 20+20 tick
             * white dissolve so the Start screen arrives promptly. */
            TNT_Vita_IntroAdvance(delta);

            /* Canonical func_8007F344 SFX schedule. Each threshold is the
             * animation tick at which the N64 starts the corresponding effect
             * from g_introSfxBank. Keep the state monotonic so a rare delayed
             * Vita frame cannot permanently miss a cue. */
            {
                static const u16 intro_sfx_tick[12] = {
                    4, 816, 1067, 1272, 1605, 1885,
                    2060, 2733, 2789, 2987, 3340, 3750
                };
                while (!intro_fading && intro_sfx_next < 12 &&
                       TNT_Vita_IntroTick() >= intro_sfx_tick[intro_sfx_next]) {
                    Audio2_Play_SFX(
                        &D_801235B0, &g_introSfxBank, (s32)intro_sfx_next);
                    intro_sfx_next++;
                }
            }

            if (!intro_fading &&
                (((g_PV_ptr->unk24 & (CONT_A | CONT_START)) != 0) ||
                 TNT_Vita_IntroTick() >= TNT_INTRO_END_TICK)) {
                intro_fading = TRUE;
                intro_fade_tick = 0;
                /* Source Audio2_GFXDone_SendStopMessage: all cinematic SFX
                 * stop as soon as the fade-to-white begins. */
                TNT_Vita_SfxStopAll();
            }

            if (intro_fading) {
                u32 next = (u32)intro_fade_tick + delta;
                if (next > TNT_INTRO_HANDOFF_FADE_TICKS) next = TNT_INTRO_HANDOFF_FADE_TICKS;
                intro_fade_tick = (u16)next;
                white_fade = (u8)((next * 255U) / TNT_INTRO_HANDOFF_FADE_TICKS);
                if (next >= TNT_INTRO_HANDOFF_FADE_TICKS)
                    leave_intro = TRUE;
            }

            if (g_tnt_milestone_gxm_ready) {
                if (TNT_Vita_IntroGxmRender(
                        &buffers[backBuffer].surface,
                        buffers[backBuffer].sync,
                        board_x, board_y, cell, white_fade) < 0) {
                    /* Keep boot recoverable if the native cinematic renderer
                     * cannot initialize on a particular device. */
                    leave_intro = TRUE;
                } else if (white_fade == 0) {
                    /* Fixed intro-only shim. Source pixels remain untouched and
                     * the cybergrid rails stay perfectly stationary in motion. */
                    g_tnt_intro_shim_active = TRUE;
                }
            } else {
                leave_intro = TRUE;
            }

            if (leave_intro) {
                void *buffer = buffers[backBuffer].data;

                /* The intro renderer has finished writing this backbuffer, but
                 * it has not been queued for display yet. Replace the terminal
                 * intro frame with the first title frame now so scanout never
                 * sees a one-frame mixture of intro-owned side pixels and the
                 * software-rendered 400x300 title viewport. At fade tick 0 the
                 * title itself is fully white, matching the completed source
                 * fade-to-white transition. */
                tnt_vita_build_ci8_index_scene(
                    g_tnt_static_scene, board_x, board_y, cell, g_tnt_title_ci8);
                clear_screen(buffer, COLOR(0, 0, 0));
                tnt_vita_render_ci8_scene(
                    buffer, board_x, board_y, cell,
                    g_tnt_title_rgba16, 255, 255, 255, 255);
                tnt_vita_draw_frontend_pillarbox(buffer, board_x, cell);

                intro_active = FALSE;
                /* First boot is complete. From the title onward, restore the
                 * approved normal inner rails everywhere except future intro
                 * playbacks, which use g_tnt_intro_shim_active independently. */
                g_tnt_boot_no_inner_rail_active = FALSE;
                title_active = TRUE;
                title_fade_tick = 1;
                title_wait_tick = 0;
                g_tnt_60hz_last_us = 0;
                g_tnt_60hz_fraction = 0;
            }

            swap_buffers();
            continue;
        }

        if (title_active) {
            void *buffer = buffers[backBuffer].data;
            u8 fade_alpha = 0;
            u8 leave_title = FALSE;

            /* Native handoff counterpart to the accelerated intro flash.
             * Keep a short debounce so intro input cannot skip straight through
             * the title, but do not preserve the source-era wait during repeated
             * Vita testing. Fifteen frontend ticks is about a quarter-second
             * at the intended 60 Hz cadence. */
            if (title_fade_tick < TNT_INTRO_HANDOFF_FADE_TICKS) {
                fade_alpha = (u8)(((TNT_INTRO_HANDOFF_FADE_TICKS - title_fade_tick) * 255U) / TNT_INTRO_HANDOFF_FADE_TICKS);
                title_fade_tick++;
            } else {
                if (((g_PV_ptr->unk24 & (CONT_A | CONT_START)) != 0 &&
                     title_wait_tick >= 15) ||
                    title_wait_tick >= 300) {
                    leave_title = TRUE;
                } else {
                    title_wait_tick++;
                }
            }

            /* The GXM intro owns the whole Vita frame, while the software
             * title renderer only redraws the centered 400x300 source field.
             * Clear the complete backbuffer first so no final sand pixels
             * survive in the Vita-only side areas during this handoff. */
            clear_screen(buffer, COLOR(0, 0, 0));
            tnt_vita_render_ci8_scene(
                buffer, board_x, board_y, cell,
                g_tnt_title_rgba16, 255, 255, 255, fade_alpha);
            tnt_vita_draw_frontend_pillarbox(buffer, board_x, cell);
            swap_buffers();

            if (leave_title) {
                /* Original state advances to the root menu here. Its large
                 * static layers were precomposed before the title, so this
                 * handoff changes only menu/controller state. */
                tnt_vita_main_menu_enter(&main_menu, g_PV_ptr);
                title_active = FALSE;
            }
            continue;
        }

        if (main_menu.active) {
            tnt_vita_refresh_multiplayer_ports();
            int action = tnt_vita_main_menu_update(
                &main_menu, g_PV_ptr, g_tnt_multi_available);
            void *buffer = buffers[backBuffer].data;

            tnt_vita_main_menu_render(
                buffer, board_x, board_y, cell, &main_menu,
                g_tnt_multi_available);
            swap_buffers();

            if (action == TNT_MAIN_MENU_ACTION_ENTER_MULTI_PLAYER) {
                tnt_vita_multi_enter(
                    &multi_menu, main_menu.pulse, main_menu.pulse_rate);
                main_menu.active = FALSE;
                continue;
            }

            if (action == TNT_MAIN_MENU_ACTION_ENTER_ONE_PLAYER) {
                tnt_vita_onep_enter(
                    &onep_menu, main_menu.pulse, main_menu.pulse_rate);
                main_menu.active = FALSE;
            } else if (action == TNT_MAIN_MENU_ACTION_ENTER_WONDERS) {
                s8 current_milestone = tnt_vita_wonder_milestone_index(
                    TNT_Vita_SramGetTotalWonderLines());

                /* Original root WONDERS GUI_FUNC sets the Wonder-progress
                 * state.  At the first reached Mayan milestone it enters the
                 * authored construction scene directly; B from that flow
                 * returns to the root menu (D_801109F0 == 4). */
                wonder_origin = TNT_WONDER_ORIGIN_ROOT;
                if (TNT_Vita_SramGetUnlockAll()) {
                    tnt_vita_wonder_hall_enter(
                        &wonder_hall, 0, board_x, board_y, cell);
                    main_menu.active = FALSE;
                } else if (current_milestone < 0) {
                    /* Before the first 357-line Mayan construction threshold,
                     * wonders1_anim_related(-1) immediately transfers to
                     * wonders3_wonder_viewer(0, TRUE, 0).  Our Hall/viewer
                     * implementation is that same selectable Wonder browser. */
                    tnt_vita_wonder_hall_enter(
                        &wonder_hall, 0, board_x, board_y, cell);
                    main_menu.active = FALSE;
                } else if (current_milestone <= 5) {
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                    main_menu.active = FALSE;
                } else if (current_milestone == 6) {
                    /* A fresh original session initializes old/current Wonder
                     * completion counts equal, so selecting WONDERS at 2500+
                     * enters the finished Mayan viewer rather than replaying
                     * the threshold-crossing ceremony. */
                    tnt_vita_wonder_complete_enter(&wonder_complete, 0, TRUE);
                    main_menu.active = FALSE;
                } else if (current_milestone >= 7 && current_milestone <= 12) {
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                    main_menu.active = FALSE;
                } else if (current_milestone == 13) {
                    tnt_vita_wonder_complete_enter(&wonder_complete, 1, TRUE);
                    main_menu.active = FALSE;
                } else if (current_milestone >= 14 && current_milestone <= 19) {
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                    main_menu.active = FALSE;
                } else if (current_milestone == 20) {
                    tnt_vita_wonder_complete_enter(&wonder_complete, 2, TRUE);
                    main_menu.active = FALSE;
                } else if (current_milestone >= 21 && current_milestone <= 26) {
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                    main_menu.active = FALSE;
                } else if (current_milestone == 27) {
                    tnt_vita_wonder_complete_enter(&wonder_complete, 3, TRUE);
                    main_menu.active = FALSE;
                } else if (current_milestone >= 28 && current_milestone <= 33) {
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                    main_menu.active = FALSE;
                } else if (current_milestone == 34) {
                    tnt_vita_wonder_complete_enter(&wonder_complete, 4, TRUE);
                    main_menu.active = FALSE;
                } else if (current_milestone >= 35 && current_milestone <= 40) {
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                    main_menu.active = FALSE;
                } else if (current_milestone == 41) {
                    tnt_vita_wonder_complete_enter(&wonder_complete, 5, TRUE);
                    main_menu.active = FALSE;
                } else if (current_milestone >= 42 && current_milestone <= 47) {
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                    main_menu.active = FALSE;
                } else if (current_milestone == 48) {
                    tnt_vita_wonder_complete_enter(&wonder_complete, 6, TRUE);
                    main_menu.active = FALSE;
                } else {
                    /* Finale construction/viewer remains a later pass. */
                    wonder_origin = TNT_WONDER_ORIGIN_NONE;
                    main_menu.leaving = FALSE;
                    main_menu.fade = 1.0f;
                }
            } else if (action == TNT_MAIN_MENU_ACTION_ENTER_OPTIONS) {
                tnt_vita_options_enter(
                    &options_menu, g_PV_ptr, board_x, board_y, cell,
                    main_menu.pulse, main_menu.pulse_rate, 0);
                main_menu.active = FALSE;
            } else if (action == TNT_MAIN_MENU_ACTION_ENTER_ATTRACT) {
                /* Canonical func_80090E08 attract-mode fork.  The original
                 * inclusive RNG draws 0..10: values 0..7 launch the GUEST
                 * Marathon AI demo (8/11); values 8..10 replay the complete
                 * intro cinematic (3/11), which naturally proceeds through
                 * the Start screen and returns to the root menu. */
                if (func_800A35EC(0, 10) >= 8) {
                    main_menu.active = FALSE;
                    intro_active = TRUE;
                    intro_fading = FALSE;
                    intro_fade_tick = 0;
                    intro_sfx_next = 0;
                    TNT_Vita_IntroReset();
                    g_tnt_60hz_last_us = 0;
                    g_tnt_60hz_fraction = 0;
                    continue;
                }

                /* Gameplay-demo branch: one GUEST Marathon player, no
                 * landfill, original hard AI rank (6), frontend state 12.
                 * Source func_80090E08 rolls the screen BEFORE Game_Init so
                 * PFGFX/NextPieces receive that culture's authored preview
                 * coordinates from the first frame. */
                g_tnt_active_gameplay_screen = (u8)func_800A35EC(
                    0, tnt_vita_max_restored_gameplay_screen());
                if (gameplay_needs_reset) {
                    tnt_vita_reset_gameplay_state(
                        &tetris, &mayan_large_fire,
                        g_tnt_active_gameplay_screen);
                    TNT_Vita_PauseInit(&pause_menu);
                    gameplay_needs_reset = FALSE;
                }
                /* Original Game_Init derives this game's Bag63/PieceHold seed
                 * from the previous SRAM game_id, then immediately saves a
                 * fresh osGetTime() game_id for the next game.  Attract mode
                 * must advance that same seed too, otherwise every demo replays
                 * an identical piece sequence. */
                tnt_vita_save_to_sram();
                active_profile_slot = -1;
                {
                    u16 generated_name_mask = 0;
                    tnt_vita_choose_guest_name(
                        current_player_name, g_tnt_active_gameplay_screen,
                        &generated_name_mask);
                }
                active_game_type = GAMETYPE_MARATHON;
                pending_wonder_milestone = -1;
                outer_game_elapsed = 0;
                endgame_exit_delay = 120;
                attract_ticks = 0;
                attract_prompt_phase = 1;
                attract_active = TRUE;
                D_800CFEE8 = 12;
                aiplayer_gameinit_related(1, 0, 6);
                tnt_vita_prepare_gameplay_background(
                    g_tnt_active_gameplay_screen, board_x, board_y, cell);
                tnt_vita_music_start_gameplay_screen(
                    g_tnt_active_gameplay_screen);
                tnt_vita_restore_gameplay_repeat(g_PV_ptr);
                main_menu.active = FALSE;
            }
            continue;
        }

        if (options_menu.active) {
            int action = tnt_vita_options_update(&options_menu, g_PV_ptr);
            void *buffer = buffers[backBuffer].data;
            tnt_vita_options_render(
                buffer, board_x, board_y, cell, &options_menu);
            swap_buffers();
            if (action == TNT_OPTIONS_ACTION_SCORES) {
                tnt_vita_scores_enter(
                    &scores_menu, g_PV_ptr, board_x, board_y, cell,
                    options_menu.pulse, options_menu.pulse_rate);
                options_menu.active = FALSE;
            } else if (action == TNT_OPTIONS_ACTION_DATA) {
                tnt_vita_data_enter(
                    &data_menu, g_PV_ptr, board_x, board_y, cell,
                    options_menu.pulse, options_menu.pulse_rate);
                options_menu.active = FALSE;
            } else if (action == TNT_OPTIONS_ACTION_AUDIO) {
                tnt_vita_audio_enter(
                    &audio_menu, g_PV_ptr, board_x, board_y, cell,
                    options_menu.pulse, options_menu.pulse_rate);
                options_menu.active = FALSE;
            } else if (action == TNT_OPTIONS_ACTION_CREDITS) {
                tnt_vita_credits_enter(&credits);
                options_menu.active = FALSE;
            } else if (action == TNT_OPTIONS_ACTION_BACK) {
                tnt_vita_main_menu_enter(&main_menu, g_PV_ptr);
                main_menu.selected = 3;
                main_menu.pulse = options_menu.pulse;
                main_menu.pulse_rate = options_menu.pulse_rate;
                options_menu.active = FALSE;
            }
            continue;
        }

        if (data_menu.active) {
            int action = tnt_vita_data_update(&data_menu, g_PV_ptr);
            void *buffer = buffers[backBuffer].data;
            tnt_vita_data_render(
                buffer, board_x, board_y, cell, &data_menu);
            swap_buffers();
            if (action == TNT_DATA_ACTION_BACK) {
                tnt_vita_options_enter(
                    &options_menu, g_PV_ptr, board_x, board_y, cell,
                    data_menu.pulse, data_menu.pulse_rate, 1);
                data_menu.active = FALSE;
            }
            continue;
        }

        if (scores_menu.active) {
            int action = tnt_vita_scores_update(&scores_menu, g_PV_ptr);
            void *buffer = buffers[backBuffer].data;
            tnt_vita_scores_render(
                buffer, board_x, board_y, cell, &scores_menu);
            swap_buffers();
            if (action == TNT_SCORES_ACTION_BACK) {
                tnt_vita_options_enter(
                    &options_menu, g_PV_ptr, board_x, board_y, cell,
                    scores_menu.pulse, scores_menu.pulse_rate, 0);
                scores_menu.active = FALSE;
            }
            continue;
        }

        if (credits.active) {
            int leave_credits = tnt_vita_credits_update(&credits, g_PV_ptr);
            void *buffer = buffers[backBuffer].data;
            tnt_vita_credits_render(
                buffer, &buffers[backBuffer].surface, buffers[backBuffer].sync,
                board_x, board_y, cell, &credits);
            swap_buffers();
            if (leave_credits) {
                tnt_vita_options_enter(
                    &options_menu, g_PV_ptr, board_x, board_y, cell,
                    1.0f, -0.03f, 3);
                credits.active = FALSE;
            }
            continue;
        }

        if (audio_menu.active) {
            int action = tnt_vita_audio_update(&audio_menu, g_PV_ptr);
            void *buffer = buffers[backBuffer].data;
            tnt_vita_audio_render(
                buffer, board_x, board_y, cell, &audio_menu);
            swap_buffers();
            if (action == TNT_AUDIO_ACTION_BACK) {
                tnt_vita_options_enter(
                    &options_menu, g_PV_ptr, board_x, board_y, cell,
                    audio_menu.pulse, audio_menu.pulse_rate, 2);
                audio_menu.active = FALSE;
            }
            continue;
        }

        if (multi_menu.active) {
            int action;
            void *buffer;
            tnt_vita_refresh_multiplayer_ports();
            action = tnt_vita_multi_update(
                &multi_menu, &g_PV_arr[0], &g_PV_arr[1],
                &g_PV_arr[2], &g_PV_arr[3]);
            buffer = buffers[backBuffer].data;
            tnt_vita_multi_render(buffer, board_x, board_y, cell, &multi_menu);
            swap_buffers();

            if (action == TNT_MULTI_ACTION_BACK) {
                multi_menu.active = FALSE;
                tnt_vita_main_menu_enter(&main_menu, &g_PV_arr[0]);
            } else if (action == TNT_MULTI_ACTION_START_GAME) {
                u32 shared_seed;
                if (!g_tnt_multi_available)
                    continue;
                multiplayer_player_count = (u8)(2U +
                    (multi_menu.slot_confirmed[2] ? 1U : 0U) +
                    (multi_menu.slot_confirmed[3] ? 1U : 0U));
#if TNTV_4P_DEV_TWO_PADS
                /* Test98: all nine original 4P combinations permitted.
                 * Two DS4 input streams mirrored into four separate
                 * simulation states ONLY in the diagnostic build. */
                if (multi_menu.pad_count==2 &&
                    multi_menu.game_type<=GAMETYPE_ULTRA &&
                    (multi_menu.garbage_type==LANDFILLTYPE_NONE ||
                     multi_menu.garbage_type==LANDFILLTYPE_HOTPOTATO ||
                     multi_menu.garbage_type==LANDFILLTYPE_DIRECTED))
                    multiplayer_player_count=4;
#endif
                /* Test90 completes the 3-player retail matrix:
                 * Marathon, Sprint, Ultra each support NONE, HOT POTATO
                 * and DIRECTED. The independent 4P renderer remains
                 * deliberately gated. Normal Ultra goal is 150 lines;
                 * dev builds can use TNTV_3P_ULTRA_GOAL=5. */
                u8 supported_3p = (u8)(
                    multi_menu.game_type <= GAMETYPE_ULTRA &&
                    (multi_menu.garbage_type == LANDFILLTYPE_NONE ||
                     multi_menu.garbage_type == LANDFILLTYPE_HOTPOTATO ||
                     multi_menu.garbage_type == LANDFILLTYPE_DIRECTED));
                if ((multiplayer_player_count > 3 &&
                     !(multiplayer_player_count == 4 &&
                       multi_menu.game_type<=GAMETYPE_ULTRA &&
                       (multi_menu.garbage_type == LANDFILLTYPE_NONE ||
                        multi_menu.garbage_type == LANDFILLTYPE_HOTPOTATO ||
                        multi_menu.garbage_type == LANDFILLTYPE_DIRECTED))) ||
                    (multiplayer_player_count == 3 && !supported_3p)) {
                    Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_3);
                    continue;
                }
                active_game_type = multi_menu.game_type;
                active_garbage_type = multi_menu.garbage_type;
                /* Source func_80090E08 rolls a stage in
                 * [0, D_800CF838] for ALL multiplayer counts.
                 * In 3P/4P, source screen 7 uses Russian background.
                 * Preserve the existing 2P culture range unchanged. */
                g_tnt_active_gameplay_screen = multiplayer_player_count >= 3
                    ? (u8)func_800A35EC(
                        0, tnt_vita_max_restored_gameplay_screen())
                    : (u8)func_800A35EC(0, 6);
#if TNTV_MP_CULTURE_TEST_CYCLE
                /* Isolated developer visual QA only: four/six/seven
                 * repeat-match cycles show all seven cultures without
                 * altering the saved wonder progression or any menu.
                 * Never enabled in normal retail builds. */
                if (multiplayer_player_count >= 3) {
                    static u8 next_culture;
                    g_tnt_active_gameplay_screen = next_culture;
                    next_culture = (u8)((next_culture + 1U) %
                        TNT_4P_CULTURE_COUNT);
                }
#endif
                shared_seed = tnt_vita_game_seed_from_id();
                tnt_vita_deinit_tetris_player_state(&tetris, 0);
                if (multiplayer_player_count >= 3) {
                    if (third_tetris == NULL)
                        third_tetris = (Tetris *)calloc(1, sizeof(*third_tetris));
                    if (third_tetris == NULL) {
                        Audio2_Play_SFX(&D_801235B0, &g_menuSfxBank, SFX_MENU_3);
                        tnt_vita_init_tetris_state(
                            &tetris, tnt_vita_game_seed_from_id(), 0);
                        continue;
                    }
                    if (multiplayer_player_count == 4) {
                        Tetris *four_players[4];
                        if (fourth_tetris == NULL)
                            fourth_tetris = (Tetris *)calloc(
                                1,sizeof(*fourth_tetris));
                        if (fourth_tetris == NULL) {
                            Audio2_Play_SFX(&D_801235B0,
                                &g_menuSfxBank,SFX_MENU_3);
                            tnt_vita_init_tetris_state(&tetris,
                                tnt_vita_game_seed_from_id(),0);
                            continue;
                        }
                        four_players[0]=&tetris;
                        four_players[1]=&opponent_tetris;
                        four_players[2]=third_tetris;
                        four_players[3]=fourth_tetris;
                        tnt_vita_init_multiplayer_4p_match(
                            four_players,&opponent_landfill,shared_seed,
                            g_tnt_active_gameplay_screen,
                            active_game_type,active_garbage_type);
                    } else {
                        tnt_vita_init_multiplayer_3p_match(
                            &tetris, &opponent_tetris, third_tetris,
                            &opponent_landfill, shared_seed,
                            g_tnt_active_gameplay_screen,
                            active_game_type, active_garbage_type);
                    }
                } else {
                    tnt_vita_init_multiplayer_match(
                        &tetris, &opponent_tetris, &opponent_landfill,
                        shared_seed, g_tnt_active_gameplay_screen,
                        active_game_type, active_garbage_type);
                }
                TNT_Vita_PauseInit(&pause_menu);
                opponent_winner_mask = 0;
                opponent_end_delay = 120;
                outer_game_elapsed = 0;
                snprintf(current_player_name, sizeof(current_player_name), "%s",
                    tnt_vita_multi_slot_name(&multi_menu, 0));
                snprintf(current_opponent_name, sizeof(current_opponent_name), "%s",
                    tnt_vita_multi_slot_name(&multi_menu, 1));
                snprintf(current_player3_name, sizeof(current_player3_name), "%s",
                    tnt_vita_multi_slot_name(&multi_menu, 2));
                snprintf(current_player4_name, sizeof(current_player4_name), "%s",
                    tnt_vita_multi_slot_name(&multi_menu, 3));
                active_profile_slot = -1;
                tnt_vita_save_to_sram();
                if (multiplayer_player_count == 4) {
                    const char *four_names[4] = {
                        current_player_name,current_opponent_name,
                        current_player3_name,current_player4_name
                    };
                    /* N64 culture-specific 4P art (shared with 3P). */
                    tnt_vita_prepare_3p_culture_background(
                        g_tnt_active_gameplay_screen,board_x,board_y,cell);
                    tnt_vita_compose_4p_names(
                        board_x,board_y,cell,four_names);
                    /* Prepare Directed static bars/marker-cache BEFORE
                     * the cached line-number tile; no extra per-frame
                     * Font-C alpha draws or background copies. */
                    if (active_garbage_type==LANDFILLTYPE_DIRECTED)
                        tnt_vita_3p_directed_prepare(
                            &opponent_landfill,board_x,board_y,cell);
                    /* Ultra goal is a static original Font C caption at
                     * source x188/y265; its elapsed clock uses the
                     * already optimized once-a-second test89F tile.
                     * Prepare before the line count tiles so caches
                     * include the final static goal artwork. */
                    if (active_game_type==GAMETYPE_ULTRA)
                        tnt_vita_3p_ultra_hp_clock_prepare(
                            board_x,board_y,cell,TNTV_4P_ULTRA_GOAL);
                    tnt_vita_4p_lines_prepare(board_x,board_y,cell);
                    if (active_game_type==GAMETYPE_SPRINT)
                        tnt_vita_4p_sprint_clock_prepare(
                            board_x,board_y,cell);
                    if (active_garbage_type==LANDFILLTYPE_HOTPOTATO) {
                        tnt_vita_3p_hp_tint_prepare(
                            board_x,board_y,cell);
                        tnt_vita_3p_hp_pulse_prepare();
                    }
                } else if (multiplayer_player_count == 3) {
                    tnt_vita_prepare_3p_culture_background(
                        g_tnt_active_gameplay_screen, board_x, board_y, cell);
                    tnt_vita_compose_3p_names(
                        board_x, board_y, cell,
                        current_player_name, current_opponent_name,
                        current_player3_name);
                    if (active_garbage_type == LANDFILLTYPE_HOTPOTATO) {
                        tnt_vita_3p_hp_tint_prepare(
                            board_x, board_y, cell);
                        tnt_vita_3p_hp_pulse_prepare();
                    } else if (active_garbage_type == LANDFILLTYPE_DIRECTED) {
                        tnt_vita_3p_directed_prepare(
                            &opponent_landfill, board_x, board_y, cell);
                    }
                    /* Test90: reuse test89F's exact high-performance
                     * Ultra footer (constant goal + 1Hz timer) and
                     * twice-alpha line-count tiles for DIRECTED too.
                     * Prepare AFTER static directed bars/markers so
                     * cached patches are based on the final scene. */
                    if (active_game_type == GAMETYPE_ULTRA &&
                        (active_garbage_type == LANDFILLTYPE_HOTPOTATO ||
                         active_garbage_type == LANDFILLTYPE_DIRECTED)) {
                        tnt_vita_3p_ultra_hp_clock_prepare(
                            board_x, board_y, cell, TNTV_3P_ULTRA_GOAL);
                        tnt_vita_3p_ultra_hp_lines_prepare(
                            board_x, board_y, cell);
                    }
                } else {
                    tnt_vita_prepare_opponent_background(
                        g_tnt_active_gameplay_screen, board_x, board_y, cell);
                    tnt_vita_compose_opponent_static_hud(
                        g_tnt_static_scene, board_x, board_y, cell,
                        g_tnt_active_gameplay_screen, active_game_type,
                        current_player_name, current_opponent_name);
                }
                tnt_vita_music_start_gameplay_screen(g_tnt_active_gameplay_screen);
                tnt_vita_restore_gameplay_repeat(&g_PV_arr[0]);
                tnt_vita_restore_gameplay_repeat(&g_PV_arr[1]);
                if (multiplayer_player_count >= 3)
                    tnt_vita_restore_gameplay_repeat(&g_PV_arr[2]);
                if (multiplayer_player_count == 4)
                    tnt_vita_restore_gameplay_repeat(&g_PV_arr[3]);
                opponent_active = FALSE;
                multiplayer_active = TRUE;
                gameplay_needs_reset = FALSE;
                multi_menu.active = FALSE;
                g_tnt_60hz_last_us = 0;
                g_tnt_60hz_fraction = 0;
            }
            continue;
        }

        if (onep_menu.active) {
            int action = tnt_vita_onep_update(&onep_menu, g_PV_ptr);
            void *buffer = buffers[backBuffer].data;

            tnt_vita_onep_render(
                buffer, board_x, board_y, cell, &onep_menu);
            swap_buffers();

            if (g_tnt_haluci_requested) {
                /* GUI_Textbox callback sets frontend substate 4, which the
                 * original main dispatcher converts to state 13. HALUCI owns
                 * the program until reset; no temporary profile is retained. */
                g_tnt_haluci_requested = FALSE;
                onep_menu.active = FALSE;
                haluci_active = TRUE;
                TNT_Vita_HaluciInit();
                g_tnt_60hz_last_us = 0;
                g_tnt_60hz_fraction = 0;
                continue;
            }

            if (action == TNT_ONEP_ACTION_START_GAME) {
                /* Source start_single_player_game selects the CPU mode first,
                 * then func_80090E08 rolls the gameplay screen before Game_Init. */
                active_game_type = onep_menu.game_type;
                opponent_active = onep_menu.opponent != 0;
                opponent_rank = opponent_active
                    ? (u8)(onep_menu.opponent - 1U) : 0;
                g_tnt_active_gameplay_screen = TNT_Vita_SramGetUnlockAll()
                    ? onep_menu.stage
                    : (u8)func_800A35EC(
                        0, tnt_vita_max_restored_gameplay_screen());

                if (gameplay_needs_reset) {
                    if (opponent_active) {
                        u32 shared_seed = tnt_vita_game_seed_from_id();

                        /* The frontend keeps one dormant 1P Tetris allocated.
                         * Release it, then initialize the source-shaped two-board
                         * DIRECTED Landfill match from one common game seed. */
                        tnt_vita_deinit_tetris_player_state(&tetris, 0);
                        tnt_vita_init_opponent_match(
                            &tetris, &opponent_tetris, &opponent_landfill,
                            shared_seed, g_tnt_active_gameplay_screen,
                            active_game_type, opponent_rank);
                        /* Game_Init owns one shared 16-tick gameplay alpha
                         * interpolator for every Tetris object. Reuse the Vita
                         * pause/fade state as that shared coordinator. */
                        TNT_Vita_PauseInit(&pause_menu);
                        opponent_winner_mask = 0;
                        opponent_end_delay = 120;

                        /* Player 1's physical queue is never sampled; reset only
                         * the repeat/synthetic state the AI writes through. */
                        g_PV_ptr = &g_PV_arr[1];
                        FUN_026C80_80060ad4_oneliner_calls_fun(&g_PV_ptr->unk28);
                        FUN_026C80_80060b04_twelveliner_loops_32t(
                            &g_PV_ptr->unk28, 0x0F00, 8, 4);
                        FUN_026C80_80060b04_twelveliner_loops_32t(
                            &g_PV_ptr->unk28, 0xC000, 16, 16);
                        FUN_026C80_nineliner_loops_32t(&g_PV_ptr->unk28);
                        g_PV_ptr->unk20 = g_PV_ptr->unk24 = 0;
                        g_PV_ptr = &g_PV_arr[0];
                    } else {
                        tnt_vita_reset_gameplay_state(
                            &tetris, &mayan_large_fire,
                            g_tnt_active_gameplay_screen);
                        TNT_Vita_PauseInit(&pause_menu);
                    }
                    gameplay_needs_reset = FALSE;
                }

                /* Original Game_Init advances the persistent game-id only after
                 * deriving this match's seed. */
                tnt_vita_save_to_sram();
                {
                    u16 generated_name_mask = 0;
                    if (onep_menu.name_entry == 0) {
                        active_profile_slot = -1;
                        tnt_vita_choose_guest_name(
                            current_player_name, g_tnt_active_gameplay_screen,
                            &generated_name_mask);
                    } else {
                        u8 profile_index = (u8)(onep_menu.name_entry - 2);
                        active_profile_slot =
                            profile_index < onep_menu.profile_count
                                ? (s8)onep_menu.profile_slot[profile_index]
                                : -1;
                        snprintf(
                            current_player_name, sizeof(current_player_name), "%s",
                            tnt_vita_onep_name_text(&onep_menu));
                    }
                    if (opponent_active) {
                        tnt_vita_choose_guest_name(
                            current_opponent_name, g_tnt_active_gameplay_screen,
                            &generated_name_mask);
                    } else {
                        current_opponent_name[0] = '\0';
                    }
                }
                pending_wonder_milestone = -1;
                outer_game_elapsed = 0;
                endgame_exit_delay = 120;
                g_tnt_60hz_last_us = 0;
                g_tnt_60hz_fraction = 0;
                if (active_game_type == GAMETYPE_ULTRA && !opponent_active)
                    tetris.ultraGoal = 150;

                if (opponent_active) {
                    tnt_vita_prepare_opponent_background(
                        g_tnt_active_gameplay_screen, board_x, board_y, cell);
                    tnt_vita_compose_opponent_static_hud(
                        g_tnt_static_scene, board_x, board_y, cell,
                        g_tnt_active_gameplay_screen, active_game_type,
                        current_player_name, current_opponent_name);
                } else
                    tnt_vita_prepare_gameplay_background(
                        g_tnt_active_gameplay_screen, board_x, board_y, cell);
                tnt_vita_music_start_gameplay_screen(
                    g_tnt_active_gameplay_screen);
                g_PV_ptr = &g_PV_arr[0];
                tnt_vita_restore_gameplay_repeat(g_PV_ptr);
                onep_menu.active = FALSE;
            } else if (action == TNT_ONEP_ACTION_BACK) {
                tnt_vita_main_menu_enter(&main_menu, g_PV_ptr);
                main_menu.pulse = onep_menu.pulse;
                main_menu.pulse_rate = onep_menu.pulse_rate;
                onep_menu.active = FALSE;
            }
            continue;
        }

        if (tally.active) {
            int action = tnt_vita_tally_update(&tally, g_PV_ptr);
            void *buffer = buffers[backBuffer].data;

            /* The original GUI pulse is global and func_80099674 continues
             * advancing it on the tally screen even though tally has no menu
             * selection. Carry that hidden phase into the returning menu. */
            if (tally_return_to_multi) {
                multi_menu.pulse += multi_menu.pulse_rate;
                if (multi_menu.pulse >= 1.0f) {
                    multi_menu.pulse = 1.0f;
                    multi_menu.pulse_rate *= -1.0f;
                }
                if (multi_menu.pulse <= 0.6f) {
                    multi_menu.pulse = 0.6f;
                    multi_menu.pulse_rate *= -1.0f;
                }
            } else {
                onep_menu.pulse += onep_menu.pulse_rate;
                if (onep_menu.pulse >= 1.0f) {
                    onep_menu.pulse = 1.0f;
                    onep_menu.pulse_rate *= -1.0f;
                }
                if (onep_menu.pulse <= 0.6f) {
                    onep_menu.pulse = 0.6f;
                    onep_menu.pulse_rate *= -1.0f;
                }
            }

            tnt_vita_tally_render(
                buffer, board_x, board_y, cell, &tally);
            swap_buffers();

            if (action == TNT_TALLY_ACTION_RETURN_ONE_PLAYER) {
                tally.active = FALSE;
                if (tally_return_to_multi) {
                    tally_return_to_multi = FALSE;
                    g_PV_ptr = &g_PV_arr[0];
                    tnt_vita_multi_enter(
                        &multi_menu, multi_menu.pulse, multi_menu.pulse_rate);
                    multi_menu.game_type = active_game_type;
                    multi_menu.garbage_type = active_garbage_type;
                    pending_wonder_milestone = -1;
                } else if (pending_wonder_milestone >= 0 &&
                    pending_wonder_milestone <= 48) {
                    /* tally.c diverts to state 7 only after its normal dismissal
                     * when the pre-award and post-award milestone indices differ. */
                    wonder_origin = TNT_WONDER_ORIGIN_TALLY;
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, pending_wonder_milestone);
                    pending_wonder_milestone = -1;
                } else {
                    /* Original tally returns to the one-player GUI using menu
                     * controller repeat timings. Preserve the game-type toggle. */
                    FUN_026C80_80060ad4_oneliner_calls_fun(&g_PV_ptr->unk28);
                    FUN_026C80_80060b04_twelveliner_loops_32t(
                        &g_PV_ptr->unk28, 0x0F00, 32, 10);
                    FUN_026C80_80060b04_twelveliner_loops_32t(
                        &g_PV_ptr->unk28, 0xC000, 16, 16);
                    FUN_026C80_nineliner_loops_32t(&g_PV_ptr->unk28);
                    tnt_vita_onep_enter(
                        &onep_menu, onep_menu.pulse, onep_menu.pulse_rate);
                    onep_menu.game_type = active_game_type;
                    pending_wonder_milestone = -1;
                }
            }
            continue;
        }

        if (wonder_milestone.active) {
            void *buffer = buffers[backBuffer].data;
            int action = tnt_vita_wonder_milestone_update_render(
                &wonder_milestone, g_PV_ptr, buffer,
                board_x, board_y, cell);

            swap_buffers();

            if (action == TNT_MILESTONE_ACTION_HALL) {
                wonder_milestone.active = FALSE;
                tnt_vita_wonder_hall_enter(
                    &wonder_hall, (u8)(wonder_milestone.milestone_index / 7),
                    board_x, board_y, cell);
            } else if (action == TNT_MILESTONE_ACTION_COMPLETE) {
                wonder_milestone.active = FALSE;
                tnt_vita_wonder_complete_enter(
                    &wonder_complete,
                    (u8)(wonder_milestone.milestone_index / 7), FALSE);
            } else if (action == TNT_MILESTONE_ACTION_EXIT) {
                FUN_026C80_80060ad4_oneliner_calls_fun(&g_PV_ptr->unk28);
                FUN_026C80_80060b04_twelveliner_loops_32t(
                    &g_PV_ptr->unk28, 0x0F00, 32, 10);
                FUN_026C80_80060b04_twelveliner_loops_32t(
                    &g_PV_ptr->unk28, 0xC000, 16, 16);
                FUN_026C80_nineliner_loops_32t(&g_PV_ptr->unk28);
                if (wonder_origin == TNT_WONDER_ORIGIN_ROOT) {
                    tnt_vita_main_menu_enter(&main_menu, g_PV_ptr);
                    main_menu.selected = 2;
                } else {
                    tnt_vita_onep_enter(
                        &onep_menu, onep_menu.pulse, onep_menu.pulse_rate);
                    onep_menu.game_type = active_game_type;
                }
                wonder_origin = TNT_WONDER_ORIGIN_NONE;
                wonder_milestone.active = FALSE;
            }
            continue;
        }

        if (wonder_complete.active) {
            void *buffer = buffers[backBuffer].data;
            u64 profile_start=sceKernelGetProcessTimeWide();
            int action = tnt_vita_wonder_complete_update_render(
                &wonder_complete, g_PV_ptr, buffer,
                board_x, board_y, cell);

            g_tnt_wonder_frame_us=(u32)(sceKernelGetProcessTimeWide()-profile_start);
            g_tnt_wonder_profile_active=TRUE;
            swap_buffers();

            if (action == TNT_COMPLETE_ACTION_HALL) {
                u8 completed_wonder = wonder_complete.wonder;
                wonder_complete.active = FALSE;
                tnt_vita_wonder_hall_enter(
                    &wonder_hall, completed_wonder, board_x, board_y, cell);
            } else if (action == TNT_COMPLETE_ACTION_FINALE) {
                wonder_complete.active = FALSE;
                tnt_vita_wonder_finale_enter(&wonder_finale);
            } else if (action == TNT_COMPLETE_ACTION_EXIT) {
                FUN_026C80_80060ad4_oneliner_calls_fun(&g_PV_ptr->unk28);
                FUN_026C80_80060b04_twelveliner_loops_32t(
                    &g_PV_ptr->unk28, 0x0F00, 32, 10);
                FUN_026C80_80060b04_twelveliner_loops_32t(
                    &g_PV_ptr->unk28, 0xC000, 16, 16);
                FUN_026C80_nineliner_loops_32t(&g_PV_ptr->unk28);
                if (wonder_origin == TNT_WONDER_ORIGIN_ROOT) {
                    tnt_vita_main_menu_enter(&main_menu, g_PV_ptr);
                    main_menu.selected = 2;
                } else {
                    tnt_vita_onep_enter(
                        &onep_menu, onep_menu.pulse, onep_menu.pulse_rate);
                    onep_menu.game_type = active_game_type;
                }
                wonder_origin = TNT_WONDER_ORIGIN_NONE;
                wonder_complete.active = FALSE;
            }
            continue;
        }

        if (wonder_finale.active) {
            void *buffer = buffers[backBuffer].data;
            u64 profile_start=sceKernelGetProcessTimeWide();
            int action = tnt_vita_wonder_finale_update_render(
                &wonder_finale, g_PV_ptr, buffer,
                board_x, board_y, cell);

            g_tnt_wonder_frame_us=(u32)(sceKernelGetProcessTimeWide()-profile_start);
            g_tnt_wonder_profile_active=TRUE;
            swap_buffers();

            if (action == TNT_FINALE_ACTION_HALL) {
                wonder_finale.active = FALSE;
                tnt_vita_wonder_hall_enter(
                    &wonder_hall, 7, board_x, board_y, cell);
            } else if (action == TNT_FINALE_ACTION_EXIT) {
                FUN_026C80_80060ad4_oneliner_calls_fun(&g_PV_ptr->unk28);
                FUN_026C80_80060b04_twelveliner_loops_32t(
                    &g_PV_ptr->unk28, 0x0F00, 32, 10);
                FUN_026C80_80060b04_twelveliner_loops_32t(
                    &g_PV_ptr->unk28, 0xC000, 16, 16);
                FUN_026C80_nineliner_loops_32t(&g_PV_ptr->unk28);
                if (wonder_origin == TNT_WONDER_ORIGIN_ROOT) {
                    tnt_vita_main_menu_enter(&main_menu, g_PV_ptr);
                    main_menu.selected = 2;
                } else {
                    tnt_vita_onep_enter(
                        &onep_menu, onep_menu.pulse, onep_menu.pulse_rate);
                    onep_menu.game_type = active_game_type;
                }
                wonder_origin = TNT_WONDER_ORIGIN_NONE;
                wonder_finale.active = FALSE;
            }
            continue;
        }

        if (wonder_hall.active) {
            void *buffer = buffers[backBuffer].data;
            const u64 hall_start=sceKernelGetProcessTimeWide();
            u64 hall_render_end,hall_end;
            u32 hall_gpu_wait;
            int action;
            g_tnt_test103a_gpu_wait_us=0;
            /* Each Hall render branch reports its own geometry/GXM phases;
             * clear the prior card's values when a CPU fallback occurs. */
            memset(g_tnt_wonder_profile_us,0,sizeof(g_tnt_wonder_profile_us));
            g_tnt_test103a_measure_gxm=TRUE;
            action = tnt_vita_wonder_hall_update_render(
                &wonder_hall, g_PV_ptr, buffer,
                board_x, board_y, cell);
            g_tnt_test103a_measure_gxm=FALSE;
            hall_gpu_wait=g_tnt_test103a_gpu_wait_us;
            hall_render_end=sceKernelGetProcessTimeWide();
            swap_buffers();
            hall_end=sceKernelGetProcessTimeWide();
            if(action==TNT_HALL_ACTION_NONE)
                tnt209_hall_profile_note(wonder_hall.current_screen,
                    wonder_hall.state,hall_start,hall_render_end,hall_end,
                    hall_gpu_wait);

            if (action == TNT_HALL_ACTION_ACCEPT) {
                u32 total = TNT_Vita_SramGetTotalWonderLines();
                s8 current_milestone = tnt_vita_wonder_milestone_index(total);

                /* TNTV4ALL remains pure access control: it exposes the same
                 * finished-Wonder and FINALE routes without spoofing real lines. */
                if (wonder_hall.current_screen == 7U &&
                    (TNT_Vita_SramGetUnlockAll() || total >= 500000U)) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_finale_enter(&wonder_finale);
                } else if (TNT_Vita_SramGetUnlockAll() &&
                    wonder_hall.current_screen <= 6U) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_complete_enter(
                        &wonder_complete, wonder_hall.current_screen, TRUE);
                } else if (wonder_hall.current_screen == 0 && total >= 2500U) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_complete_enter(&wonder_complete, 0, TRUE);
                } else if (wonder_hall.current_screen == 0 &&
                           current_milestone >= 0 && current_milestone <= 5) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                } else if (wonder_hall.current_screen == 1 && total >= 7500U) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_complete_enter(&wonder_complete, 1, TRUE);
                } else if (wonder_hall.current_screen == 1 &&
                           current_milestone >= 7 && current_milestone <= 12) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                } else if (wonder_hall.current_screen == 2 && total >= 20000U) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_complete_enter(&wonder_complete, 2, TRUE);
                } else if (wonder_hall.current_screen == 2 &&
                           current_milestone >= 14 && current_milestone <= 19) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                } else if (wonder_hall.current_screen == 3 && total >= 50000U) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_complete_enter(&wonder_complete, 3, TRUE);
                } else if (wonder_hall.current_screen == 3 &&
                           current_milestone >= 21 && current_milestone <= 26) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                } else if (wonder_hall.current_screen == 4 && total >= 100000U) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_complete_enter(&wonder_complete, 4, TRUE);
                } else if (wonder_hall.current_screen == 4 &&
                           current_milestone >= 28 && current_milestone <= 33) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                } else if (wonder_hall.current_screen == 5 && total >= 250000U) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_complete_enter(&wonder_complete, 5, TRUE);
                } else if (wonder_hall.current_screen == 5 &&
                           current_milestone >= 35 && current_milestone <= 40) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                } else if (wonder_hall.current_screen == 6 && total >= 500000U) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_complete_enter(&wonder_complete, 6, TRUE);
                } else if (wonder_hall.current_screen == 6 &&
                           current_milestone >= 42 && current_milestone <= 47) {
                    wonder_hall.active = FALSE;
                    tnt_vita_wonder_milestone_enter(
                        &wonder_milestone, current_milestone);
                }
            } else if (action == TNT_HALL_ACTION_BACK) {
                FUN_026C80_80060ad4_oneliner_calls_fun(&g_PV_ptr->unk28);
                FUN_026C80_80060b04_twelveliner_loops_32t(
                    &g_PV_ptr->unk28, 0x0F00, 32, 10);
                FUN_026C80_80060b04_twelveliner_loops_32t(
                    &g_PV_ptr->unk28, 0xC000, 16, 16);
                FUN_026C80_nineliner_loops_32t(&g_PV_ptr->unk28);
                if (wonder_origin == TNT_WONDER_ORIGIN_ROOT) {
                    tnt_vita_main_menu_enter(&main_menu, g_PV_ptr);
                    main_menu.selected = 2;
                } else {
                    tnt_vita_onep_enter(
                        &onep_menu, onep_menu.pulse, onep_menu.pulse_rate);
                    onep_menu.game_type = active_game_type;
                }
                wonder_origin = TNT_WONDER_ORIGIN_NONE;
                wonder_hall.active = FALSE;
            }
            continue;
        }

        if (wonder_viewer.active) {
            void *buffer = buffers[backBuffer].data;
            PlayerVars *viewer_pv = (multiplayer_active &&
                wonder_origin == TNT_WONDER_ORIGIN_PAUSE)
                ? &g_PV_arr[multiplayer_pause_owner] : g_PV_ptr;
            u8 leave_viewer = tnt_vita_wonder_update_render(
                &wonder_viewer, viewer_pv, buffer,
                board_x, board_y, cell);

            swap_buffers();

            if (leave_viewer) {
                TNT_Vita_PauseResumeGame(
                    &pause_menu, &tetris,
                    &g_PV_arr[multiplayer_active ? multiplayer_pause_owner : 0]);
                /* The Wonders viewer used both display buffers while 2P
                 * gameplay was suspended; start with complete scenes. */
                memset(g_tnt_opp60_scene_buffer_ready, 0,
                       sizeof(g_tnt_opp60_scene_buffer_ready));
                memset(g_tnt_opp60_dirty, 0, sizeof(g_tnt_opp60_dirty));
                memset(g_tnt_opp60_dirty_full, 0,
                       sizeof(g_tnt_opp60_dirty_full));
                g_tnt_opp60_tracking = FALSE;
                if (opponent_active || multiplayer_active) {
                    Tetris_FUN_80054220_fourliner(&opponent_tetris, FALSE);
                    TNT_Vita_SetGameplayAlpha(
                        &opponent_tetris, (u8)pause_menu.fade.alpha);
                    if (multiplayer_active && multiplayer_player_count >= 3 &&
                        third_tetris != NULL) {
                        Tetris_FUN_80054220_fourliner(third_tetris, FALSE);
                        if (multiplayer_player_count == 4 &&
                            fourth_tetris != NULL)
                            Tetris_FUN_80054220_fourliner(fourth_tetris,FALSE);
                        TNT_Vita_SetGameplayAlpha(
                            third_tetris, (u8)pause_menu.fade.alpha);
                        g_tnt_60hz_last_us = 0;
                        g_tnt_60hz_fraction = 0;
                        memset(g_tnt_3p_scene_buffer_ready, 0,
                            sizeof(g_tnt_3p_scene_buffer_ready));
                    }
                }
                wonder_origin = TNT_WONDER_ORIGIN_NONE;
                wonder_viewer.active = FALSE;
            }
            continue;
        }

        if (multiplayer_active && multiplayer_player_count == 4) {
            Tetris *four_players[4]={
                &tetris,&opponent_tetris,third_tetris,fourth_tetris
            };
            u8 action=0;
            g_tnt_4p_prof_active=FALSE; /* release: no dev overhead */
            g_tnt_3p_prof_active=FALSE;
            if (third_tetris != NULL && fourth_tetris != NULL)
                action=tnt_vita_4p_core_frame(
                    four_players,&opponent_landfill,&pause_menu,
                    &opponent_end_delay,&opponent_winner_mask,
                    &outer_game_elapsed,&multiplayer_pause_owner,
                    active_game_type,board_x,board_y,cell);
            if (action==3) {
                wonder_origin=TNT_WONDER_ORIGIN_PAUSE;
                tnt_vita_wonder_enter(
                    &wonder_viewer,board_x,board_y,cell);
            } else if (action==1 || action==2) {
                /* Like approved test78/3P, capture all player statistics
                 * BEFORE Game_Deinit. Pause EXIT skips tally as retail. */
                if (action==1) {
                    const char *names[4]={
                        current_player_name,current_opponent_name,
                        current_player3_name,current_player4_name
                    };
                    u32 lines[4],seconds[4],sum_lines=0;
                    u8 ranks[4],winner_count=0;
                    u8 result_winner_mask=opponent_winner_mask;
                    /* Source GameResults topOutOrder: if all four lose
                     * on the same update the last player in the scan
                     * is the nominal winner. Mirror 3P fallback. */
                    if (!result_winner_mask)result_winner_mask=8U;
                    for (int i=0;i<4;i++) {
                        if (result_winner_mask&(1U<<i))
                            winner_count++;
                        lines[i]=getLineCount(
                            &four_players[i]->gameStats.linesInfo);
                        seconds[i]=four_players[i]->gameStats.elapsedTime/60U;
                        ranks[i]=tnt_vita_multi_result_rank(
                            &multi_menu,i,lines[i],seconds[i]);
                        sum_lines+=lines[i];
                    }
                    if (!winner_count)winner_count=1;
                    tnt_vita_tally_enter_four(
                        &tally,board_x,board_y,cell,
                        names,ranks,lines,seconds,result_winner_mask,
                        sum_lines/winner_count);
                    tally_return_to_multi=TRUE;
                }
                tnt_vita_deinit_multiplayer_4p_match(
                    four_players,&opponent_landfill);
                tnt_vita_init_tetris_state(
                    &tetris,tnt_vita_game_seed_from_id(),0);
                TNT_Vita_PauseInit(&pause_menu);
                g_PV_ptr=&g_PV_arr[0];
                if (action==2) {
                    tally_return_to_multi=FALSE;
                    tnt_vita_multi_enter(
                        &multi_menu,multi_menu.pulse,multi_menu.pulse_rate);
                    multi_menu.game_type=active_game_type;
                    multi_menu.garbage_type=active_garbage_type;
                }
                multiplayer_active=FALSE;
                multiplayer_player_count=2;
                gameplay_needs_reset=TRUE;
                g_tnt_4p_prof_active=FALSE;
                g_tnt_60hz_last_us=0;
                g_tnt_60hz_fraction=0;
            }
            g_PV_ptr=&g_PV_arr[0];
            continue;
        }

        if (multiplayer_active && multiplayer_player_count == 3) {
            u8 match_action = 0;
            g_tnt_3p_prof_active = FALSE; /* release: no dev overhead */
            if (third_tetris != NULL) {
                match_action = tnt_vita_3p_core_frame(
                    &tetris, &opponent_tetris, third_tetris,
                    &opponent_landfill, &pause_menu,
                    &opponent_end_delay, &opponent_winner_mask,
                    &outer_game_elapsed, &multiplayer_pause_owner,
                    active_game_type, board_x, board_y, cell);
            }

            if (match_action != 0) g_tnt_3p_prof_active = FALSE;
            if (match_action == 3) {
                /* PROGRESS temporarily owns the screen; all boards stay
                 * frozen until the viewer returns. */
                wonder_origin = TNT_WONDER_ORIGIN_PAUSE;
                tnt_vita_wonder_enter(
                    &wonder_viewer, board_x, board_y, cell);
            } else if (match_action == 1 || match_action == 2) {
                /* The retail tally snapshots ALL participant stats before
                 * Game_Deinit. Pause EXIT (action 2) skips the tally. */
                if (match_action == 1) {
                    Tetris *players[3] = {
                        &tetris, &opponent_tetris, third_tetris
                    };
                    const char *names[3] = {
                        current_player_name, current_opponent_name,
                        current_player3_name
                    };
                    u32 lines[3], seconds[3];
                    u8 ranks[3];
                    u8 result_winner_mask = opponent_winner_mask;
                    u8 winner_count = 0;
                    u32 award_lines;
                    int pi;

                    /* If everyone topped out in one update, retail's last
                     * topOutOrder entry is the winner. Our 3P top-out loop
                     * visits P1, P2, P3, making P3 the last entry. */
                    if (result_winner_mask == 0) result_winner_mask = 4U;
                    for (pi = 0; pi < 3; pi++) {
                        if (result_winner_mask & (1U << pi)) winner_count++;
                        lines[pi] = getLineCount(
                            &players[pi]->gameStats.linesInfo);
                        seconds[pi] = players[pi]->gameStats.elapsedTime / 60U;
                        if (active_game_type == GAMETYPE_SPRINT &&
                            seconds[pi] >= 179U)
                            seconds[pi] = 180U;
                        ranks[pi] = tnt_vita_multi_result_rank(
                            &multi_menu, pi, lines[pi], seconds[pi]);
                    }
                    if (winner_count == 0) winner_count = 1;
                    award_lines = (lines[0] + lines[1] + lines[2]) /
                        winner_count;
                    tnt_vita_tally_enter_three(
                        &tally, board_x, board_y, cell,
                        names[0], names[1], names[2],
                        ranks, lines, seconds, result_winner_mask,
                        award_lines);
                    tally_return_to_multi = TRUE;
                }

                tnt_vita_deinit_multiplayer_3p_match(
                    &tetris, &opponent_tetris, third_tetris,
                    &opponent_landfill);
                tnt_vita_init_tetris_state(
                    &tetris, tnt_vita_game_seed_from_id(), 0);
                TNT_Vita_PauseInit(&pause_menu);
                g_PV_ptr = &g_PV_arr[0];
                if (match_action == 2) {
                    /* Preserve the existing pause EXIT -> MULTI behavior. */
                    tally_return_to_multi = FALSE;
                    tnt_vita_multi_enter(
                        &multi_menu, multi_menu.pulse, multi_menu.pulse_rate);
                    multi_menu.game_type = active_game_type;
                    multi_menu.garbage_type = active_garbage_type;
                }
                multiplayer_active = FALSE;
                multiplayer_player_count = 2;
                gameplay_needs_reset = TRUE;
                g_tnt_60hz_last_us = 0;
                g_tnt_60hz_fraction = 0;
            }
            g_PV_ptr = &g_PV_arr[0];
            continue;
        }

        if (opponent_active || multiplayer_active) {
            void *buffer = buffers[backBuffer].data;
            const int opponent_cell = (10 * cell + 5) / 11;
            const int opponent_y = board_y +
                tnt_floor_div_s64((s64)(43 - 36) * cell, 11);
            const int human_x = board_x +
                tnt_floor_div_s64((s64)(57 - 83) * cell, 11);
            const int cpu_x = board_x +
                tnt_floor_div_s64((s64)(247 - 83) * cell, 11);
            u8 human_topped;
            u8 cpu_topped;
            u8 active_count;
            u8 pause_was_active;
            s32 opponent_pause_action;
            u8 ai_prof_on = (u8)(opponent_active && g_tnt_fps_counter_enabled);
            u32 ai_prof_h=0, ai_prof_a=0, ai_prof_c=0;
            u32 ai_prof_b=0, ai_prof_p0=0, ai_prof_p1=0, ai_prof_x=0, ai_prof_v=0;
            u64 ai_prof_t0;
            g_tnt_2p_prof_mode = opponent_active ? 'O' : 'M';

            D_801109F4 = tnt_vita_60hz_delta(TRUE);
            if (D_801109F4 == 0)
                D_801109F4 = 1;

            /* Source Game_80051790 pauses every Tetris object when any human
             * player invokes the popup. The same shared Game alpha interpolator
             * also performs the initial 0->255 gameplay fade for both boards. */
            pause_was_active = TNT_Vita_PauseIsActive(&pause_menu);
            if (!pause_was_active &&
                !tetris.frameAct.unkC && !opponent_tetris.frameAct.unkC &&
                ((g_PV_arr[0].unk28.unk8C & CONT_START) ||
                 (multiplayer_active && (g_PV_arr[1].unk28.unk8C & CONT_START)))) {
                /* The original pause object remembers which player opened it.
                 * Preserve that ownership so P2 can navigate/accept the same
                 * popup it invoked instead of handing control to P1. */
                multiplayer_pause_owner = (u8)(multiplayer_active &&
                    (g_PV_arr[1].unk28.unk8C & CONT_START) &&
                    !(g_PV_arr[0].unk28.unk8C & CONT_START));
                TNT_Vita_PauseStart(&pause_menu, &tetris);
                Tetris_FUN_80054220_fourliner(&opponent_tetris, TRUE);
                pause_was_active = TRUE;
            }

            TNT_Vita_TetrisSetGlobalPointers(&tetris, 0);
            opponent_pause_action = TNT_Vita_PauseUpdate(
                &pause_menu, &tetris,
                &g_PV_arr[multiplayer_active ? multiplayer_pause_owner : 0]);
            TNT_Vita_SetGameplayAlpha(
                &opponent_tetris, (u8)pause_menu.fade.alpha);
            if (opponent_pause_action == TNT_VITA_PAUSE_ACTION_CONTINUE)
                Tetris_FUN_80054220_fourliner(&opponent_tetris, FALSE);

            if (!pause_was_active) {
                if (tetris.unk2 || opponent_tetris.unk2)
                    outer_game_elapsed += D_801109F4;
                TNT_Vita_MetalField_Update(D_801109F4);

            /* Source Game_Update switches the entire Tetris/PlayerVars context
             * before each board. Human player 0 consumes the physical Vita
             * repeat state prepared at the top of the loop. */
            TNT_Vita_TetrisSetGlobalPointers(&tetris, 0);
            if (ai_prof_on) ai_prof_t0 = sceKernelGetProcessTimeWide();
            (void)Tetris_Update(&tetris);
            if (ai_prof_on)
                ai_prof_h = (u32)(sceKernelGetProcessTimeWide() - ai_prof_t0);

            /* Player 1 uses the same ordinary Tetris_Update path in both
             * modes. One-player OPPONENT writes synthetic AI input first;
             * local multiplayer instead consumes the DS4 #2 PlayerVars state
             * prepared at the top of this frame. */
            TNT_Vita_TetrisSetGlobalPointers(&opponent_tetris, 1);
            if (opponent_active && opponent_tetris.unk2 &&
                !opponent_tetris.frameAct.unkC) {
                if (ai_prof_on) ai_prof_t0 = sceKernelGetProcessTimeWide();
                aiplayer_8004311c_largefunction();
                if (ai_prof_on)
                    ai_prof_a = (u32)(sceKernelGetProcessTimeWide() - ai_prof_t0);
            }
            if (ai_prof_on) ai_prof_t0 = sceKernelGetProcessTimeWide();
            (void)Tetris_Update(&opponent_tetris);
            if (ai_prof_on)
                ai_prof_c = (u32)(sceKernelGetProcessTimeWide() - ai_prof_t0);

            /* Game_QueryGameOver checks every top-out first. Only after all
             * newly topped players are marked LOSE does it award WIN to a lone
             * survivor, preserving simultaneous-top-out behavior. */
            human_topped = (u8)(
                tetris.unk2 && tetris.frameAct.unk3 && !tetris.frameAct.unkC);
            cpu_topped = (u8)(
                opponent_tetris.unk2 && opponent_tetris.frameAct.unk3 &&
                !opponent_tetris.frameAct.unkC);

            if (human_topped) {
                TNT_Vita_TetrisSetGlobalPointers(&tetris, 0);
                FrameAct_calls_gamefinish_something2(
                    &tetris.frameAct, GAMEFINISHTYPE_LOSE);
                tetris.unk2 = FALSE;
                func_800726A4(&opponent_landfill, 0);
            }
            if (cpu_topped) {
                TNT_Vita_TetrisSetGlobalPointers(&opponent_tetris, 1);
                FrameAct_calls_gamefinish_something2(
                    &opponent_tetris.frameAct, GAMEFINISHTYPE_LOSE);
                opponent_tetris.unk2 = FALSE;
                func_800726A4(&opponent_landfill, 1);
            }

            active_count = (u8)((tetris.unk2 ? 1 : 0) +
                                (opponent_tetris.unk2 ? 1 : 0));
            if (active_count == 1) {
                if (tetris.unk2) {
                    TNT_Vita_TetrisSetGlobalPointers(&tetris, 0);
                    FrameAct_calls_gamefinish_something2(
                        &tetris.frameAct, GAMEFINISHTYPE_WIN);
                    tetris.unk2 = FALSE;
                    func_800726A4(&opponent_landfill, 0);
                    opponent_winner_mask |= 1U;
                } else if (opponent_tetris.unk2) {
                    TNT_Vita_TetrisSetGlobalPointers(&opponent_tetris, 1);
                    FrameAct_calls_gamefinish_something2(
                        &opponent_tetris.frameAct, GAMEFINISHTYPE_WIN);
                    opponent_tetris.unk2 = FALSE;
                    func_800726A4(&opponent_landfill, 1);
                    opponent_winner_mask |= 2U;
                }
            } else if (active_count == 2) {
                u8 goal_completed = FALSE;
                u32 human_lines = getLineCount(&tetris.gameStats.linesInfo);
                u32 cpu_lines = getLineCount(&opponent_tetris.gameStats.linesInfo);

                if (active_game_type == GAMETYPE_SPRINT)
                    goal_completed = outer_game_elapsed >= 10800U;
                else if (active_game_type == GAMETYPE_ULTRA)
                    goal_completed = human_lines >= 150U || cpu_lines >= 150U;

                if (goal_completed) {
                    u32 max_lines = human_lines > cpu_lines
                        ? human_lines : cpu_lines;
                    u8 human_wins;
                    u8 cpu_wins;

                    if (active_game_type == GAMETYPE_SPRINT) {
                        human_wins = human_lines == max_lines;
                        cpu_wins = cpu_lines == max_lines;
                    } else {
                        /* Literal Game_isGameWinner() Ultra predicate: a surviving
                         * player must have reached its Ultra goal OR share the max
                         * line count, and must also share the minimum elapsed time.
                         * In ordinary 2P both clocks normally match, but preserving
                         * the time predicate matters for future 3/4P/disconnect work. */
                        u32 human_time = tetris.gameStats.elapsedTime;
                        u32 cpu_time = opponent_tetris.gameStats.elapsedTime;
                        u32 min_time = human_time < cpu_time ? human_time : cpu_time;
                        human_wins = (u8)(
                            (human_lines >= tetris.ultraGoal || human_lines >= max_lines) &&
                            human_time == min_time);
                        cpu_wins = (u8)(
                            (cpu_lines >= opponent_tetris.ultraGoal || cpu_lines >= max_lines) &&
                            cpu_time == min_time);
                    }

                    TNT_Vita_TetrisSetGlobalPointers(&tetris, 0);
                    FrameAct_calls_gamefinish_something2(
                        &tetris.frameAct,
                        human_wins ? GAMEFINISHTYPE_WIN : GAMEFINISHTYPE_LOSE);
                    tetris.unk2 = FALSE;
                    func_800726A4(&opponent_landfill, 0);
                    if (human_wins) opponent_winner_mask |= 1U;

                    TNT_Vita_TetrisSetGlobalPointers(&opponent_tetris, 1);
                    FrameAct_calls_gamefinish_something2(
                        &opponent_tetris.frameAct,
                        cpu_wins ? GAMEFINISHTYPE_WIN : GAMEFINISHTYPE_LOSE);
                    opponent_tetris.unk2 = FALSE;
                    func_800726A4(&opponent_landfill, 1);
                    if (cpu_wins) opponent_winner_mask |= 2U;
                }
            }

            /* Original Game_800519b4: once no board remains active, wait two
             * seconds before A/Start may dismiss the WIN/LOSE finish objects. */
            if (!tetris.unk2 && !opponent_tetris.unk2) {
                if (opponent_end_delay > D_801109F4) {
                    opponent_end_delay -= (u16)D_801109F4;
                } else {
                    opponent_end_delay = 0;
                    if ((g_PV_arr[0].unk28.unk8C & (CONT_A | CONT_START)) ||
                        (multiplayer_active &&
                         (g_PV_arr[1].unk28.unk8C & (CONT_A | CONT_START)))) {
                        gamefinish_80052D48_twoliner(&tetris.frameAct.unk10);
                        gamefinish_80052D48_twoliner(
                            &opponent_tetris.frameAct.unk10);
                    }
                }
            }
            }

            if (ai_prof_on) ai_prof_t0 = sceKernelGetProcessTimeWide();
            if (opponent_active) {
                u8 force_full = (u8)(
                    pause_was_active ||
                    TNT_Vita_PauseIsActive(&pause_menu) ||
                    !tetris.unk2 || !opponent_tetris.unk2);
                tnt_vita_opp60_restore_scene(buffer, force_full);
                /* Pause/finish sprites render outside ordinary Tetris bounds.
                 * Mark this buffer for full repair on its next reuse. */
                if (force_full) g_tnt_opp60_dirty_full[backBuffer] = TRUE;
                g_tnt_opp60_tracking = TRUE;
            } else {
                /* Preserve unmodified, approved local 2P multiplayer. */
                memcpy(buffer, g_tnt_static_scene, sizeof(g_tnt_static_scene));
            }
            tnt_vita_render_opponent_hud_dynamic(
                buffer, board_x, board_y, cell,
                active_game_type, outer_game_elapsed);
            if (ai_prof_on) { ai_prof_b=(u32)(sceKernelGetProcessTimeWide()-ai_prof_t0); ai_prof_t0=sceKernelGetProcessTimeWide(); }
            /* Both OPPONENT and local MULTI render the same two 10px source
             * playfields through 18px Vita cells. */
            g_tnt_local_2p_fast_render = (u8)(opponent_active || multiplayer_active);
            tnt_vita_render_tetris_player(
                buffer, &tetris, 0, human_x, opponent_y, opponent_cell,
                board_x, board_y, cell);
            if (ai_prof_on) { ai_prof_p0=(u32)(sceKernelGetProcessTimeWide()-ai_prof_t0); ai_prof_t0=sceKernelGetProcessTimeWide(); }
            tnt_vita_render_tetris_player(
                buffer, &opponent_tetris, 1, cpu_x, opponent_y, opponent_cell,
                board_x, board_y, cell);
            g_tnt_local_2p_fast_render = FALSE;
            if (opponent_active) {
                /* The SquareGlow renderer can paint beyond its board.
                 * Full restore on next reuse whenever glows animate. */
                Tetris *boards[2] = {&tetris, &opponent_tetris};
                for (int pi = 0; pi < 2; pi++)
                    for (int si = 0; si < 44; si++)
                        if (boards[pi]->squares.arr[si].type != SQUARETYPE_NONE &&
                            boards[pi]->squares.arr[si].glow.is_active)
                            g_tnt_opp60_dirty_full[backBuffer] = TRUE;
                g_tnt_opp60_tracking = FALSE;
            }
            if (multiplayer_active && active_garbage_type == LANDFILLTYPE_HOTPOTATO)
                tnt_vita_hotpotato_render(
                    buffer, &opponent_landfill, board_x, board_y, cell);
            if (ai_prof_on) { ai_prof_p1=(u32)(sceKernelGetProcessTimeWide()-ai_prof_t0); ai_prof_t0=sceKernelGetProcessTimeWide(); }
            if (!TNT_Vita_PauseIsActive(&pause_menu) ||
                !g_tnt_milestone_gxm_ready ||
                TNT_Vita_PauseGxmRender(
                    &buffers[backBuffer].surface, buffers[backBuffer].sync,
                    board_x, board_y, cell,
                    pause_menu.selected, pause_menu.pulse) < 0) {
                TNT_Vita_PauseRender(
                    buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                    board_x, board_y, cell, &pause_menu);
            }
            /* swap_buffers() already draws this exact full surround.
             * Do not draw it twice in CPU Opponent mode. */
            if (multiplayer_active)
                tnt_vita_draw_frontend_pillarbox(buffer, board_x, cell);
            if (ai_prof_on) { ai_prof_x=(u32)(sceKernelGetProcessTimeWide()-ai_prof_t0); ai_prof_t0=sceKernelGetProcessTimeWide(); }
            g_tnt_ai_profile_active = ai_prof_on;
            swap_buffers();
            if (ai_prof_on) {
                ai_prof_v=(u32)(sceKernelGetProcessTimeWide()-ai_prof_t0);
                tnt_vita_ai_prof_sample(ai_prof_h,ai_prof_a,ai_prof_c,
                    ai_prof_b,ai_prof_p0,ai_prof_p1,ai_prof_x,ai_prof_v);
            }

            if (opponent_pause_action == TNT_VITA_PAUSE_ACTION_PROGRESS) {
                /* Source pause PROGRESS keeps every board frozen/faded while
                 * the Wonder viewer temporarily owns the frontend. */
                wonder_origin = TNT_WONDER_ORIGIN_PAUSE;
                tnt_vita_wonder_enter(
                    &wonder_viewer, board_x, board_y, cell);
                g_PV_ptr = &g_PV_arr[0];
                continue;
            } else if (opponent_pause_action == TNT_VITA_PAUSE_ACTION_EXIT) {
                /* Source pause EXIT abandons the match without a results tally. */
                if (multiplayer_active)
                    tnt_vita_deinit_multiplayer_match(
                        &tetris, &opponent_tetris, &opponent_landfill);
                else
                    tnt_vita_deinit_opponent_match(
                        &tetris, &opponent_tetris, &opponent_landfill);
                tnt_vita_init_tetris_state(
                    &tetris, tnt_vita_game_seed_from_id(), 0);
                g_PV_ptr = &g_PV_arr[0];
                if (multiplayer_active) {
                    tnt_vita_multi_enter(
                        &multi_menu, multi_menu.pulse, multi_menu.pulse_rate);
                    multi_menu.game_type = active_game_type;
                    multi_menu.garbage_type = active_garbage_type;
                    multiplayer_active = FALSE;
                } else {
                    FUN_026C80_80060ad4_oneliner_calls_fun(&g_PV_ptr->unk28);
                    FUN_026C80_80060b04_twelveliner_loops_32t(
                        &g_PV_ptr->unk28, 0x0F00, 32, 10);
                    FUN_026C80_80060b04_twelveliner_loops_32t(
                        &g_PV_ptr->unk28, 0xC000, 16, 16);
                    FUN_026C80_nineliner_loops_32t(&g_PV_ptr->unk28);
                    tnt_vita_onep_enter(
                        &onep_menu, onep_menu.pulse, onep_menu.pulse_rate);
                    onep_menu.game_type = active_game_type;
                    opponent_active = FALSE;
                }
                gameplay_needs_reset = TRUE;
                g_tnt_60hz_last_us = 0;
                g_tnt_60hz_fraction = 0;
                continue;
            }

            if (multiplayer_active &&
                tetris.frameAct.unkC && opponent_tetris.frameAct.unkC &&
                tetris.unk0 == 0 && opponent_tetris.unk0 == 0) {
                u32 p1_lines = getLineCount(&tetris.gameStats.linesInfo);
                u32 p2_lines = getLineCount(&opponent_tetris.gameStats.linesInfo);
                u32 p1_seconds = tetris.gameStats.elapsedTime / 60U;
                u32 p2_seconds = opponent_tetris.gameStats.elapsedTime / 60U;
                u8 result_winner_mask = opponent_winner_mask;
                u8 winner_count;
                u32 award_lines;
                u8 p1_rank;
                u8 p2_rank;

                if (active_game_type == GAMETYPE_SPRINT) {
                    if (p1_seconds >= 179U) p1_seconds = 180U;
                    if (p2_seconds >= 179U) p2_seconds = 180U;
                }

                /* Game_isGameWinner() treats the last entry in topOutOrder as
                 * the winner. On an exact same-update 2P double top-out, the
                 * retail loop records P1 then P2, so the tally marks P2 winner
                 * even though both finish objects showed LOSE. Keep finish
                 * animation state separate from results state. */
                if (result_winner_mask == 0)
                    result_winner_mask = 2U;
                winner_count = (u8)(((result_winner_mask & 1U) ? 1U : 0U) +
                    ((result_winner_mask & 2U) ? 1U : 0U));
                if (winner_count == 0) winner_count = 1;
                award_lines = (p1_lines + p2_lines) / winner_count;

                p1_rank = tnt_vita_multi_result_rank(
                    &multi_menu, 0, p1_lines, p1_seconds);
                p2_rank = tnt_vita_multi_result_rank(
                    &multi_menu, 1, p2_lines, p2_seconds);

                tnt_vita_tally_enter_opponent(
                    &tally, board_x, board_y, cell,
                    current_player_name, current_opponent_name,
                    p1_rank, p2_rank,
                    p1_lines, p1_seconds, p2_lines, p2_seconds,
                    result_winner_mask, award_lines);
                tally_return_to_multi = TRUE;

                /* Retail tally snapshots results before Game_Deinit. TNTV
                 * deliberately keeps multiplayer identities display-only, so
                 * there are no SRAM/progression writes on this path. */
                tnt_vita_deinit_multiplayer_match(
                    &tetris, &opponent_tetris, &opponent_landfill);
                tnt_vita_init_tetris_state(
                    &tetris, tnt_vita_game_seed_from_id(), 0);
                TNT_Vita_PauseInit(&pause_menu);
                g_PV_ptr = &g_PV_arr[0];
                multiplayer_active = FALSE;
                gameplay_needs_reset = TRUE;
                g_tnt_60hz_last_us = 0;
                g_tnt_60hz_fraction = 0;
                continue;
            }

            /* Source tally.c handoff: once both WIN/LOSE finish objects report
             * complete, snapshot both players' results before Game_Deinit. The
             * saved human profile always records its own match stats; Wonder
             * lines are awarded only when the human is one of the winners. */
            if (tetris.frameAct.unkC && opponent_tetris.frameAct.unkC &&
                tetris.unk0 == 0 && opponent_tetris.unk0 == 0) {
                u32 human_lines = getLineCount(&tetris.gameStats.linesInfo);
                u32 guest_lines = getLineCount(&opponent_tetris.gameStats.linesInfo);
                u32 human_seconds = tetris.gameStats.elapsedTime / 60U;
                u32 guest_seconds = opponent_tetris.gameStats.elapsedTime / 60U;
                u32 award_lines;
                u8 winner_count = 0;
                u8 human_rank = 1;

                if (active_game_type == GAMETYPE_SPRINT) {
                    if (human_seconds >= 179U) human_seconds = 180U;
                    if (guest_seconds >= 179U) guest_seconds = 180U;
                }

                if (opponent_winner_mask & 1U) winner_count++;
                if (opponent_winner_mask & 2U) winner_count++;
                if (winner_count == 0) winner_count = 1;
                award_lines = (human_lines + guest_lines) / winner_count;

                if (active_profile_slot >= 0) {
                    s8 old_milestone = tnt_vita_wonder_milestone_index(
                        TNT_Vita_SramGetTotalWonderLines());
                    s8 new_milestone;

                    (void)TNT_Vita_SramRecordSinglePlayerResult(
                        active_profile_slot, active_game_type,
                        human_lines, (u32)tetris.lineScan.unk3EC,
                        human_seconds);

                    if (opponent_winner_mask & 1U)
                        TNT_Vita_SramAddWonderLines(award_lines);
                    tnt_vita_save_to_sram();

                    new_milestone = tnt_vita_wonder_milestone_index(
                        TNT_Vita_SramGetTotalWonderLines());
                    if (new_milestone != old_milestone)
                        pending_wonder_milestone = new_milestone;

                    {
                        TNTVitaStoredPlayer ranked_player;
                        if (TNT_Vita_SramGetPlayer(
                                active_profile_slot, &ranked_player) == 1) {
                            human_rank = (u8)(TNT_Vita_SramRankFor(
                                ranked_player.time_in_seconds,
                                ranked_player.lines) + 1U);
                        }
                    }
                }

                tally_return_to_multi = FALSE;
                tnt_vita_tally_enter_opponent(
                    &tally, board_x, board_y, cell,
                    current_player_name, current_opponent_name, human_rank, 1,
                    human_lines, human_seconds,
                    guest_lines, guest_seconds,
                    opponent_winner_mask, award_lines);

                tnt_vita_deinit_opponent_match(
                    &tetris, &opponent_tetris, &opponent_landfill);
                tnt_vita_init_tetris_state(
                    &tetris, tnt_vita_game_seed_from_id(), 0);
                TNT_Vita_PauseInit(&pause_menu);
                g_PV_ptr = &g_PV_arr[0];
                opponent_active = FALSE;
                gameplay_needs_reset = TRUE;
                g_tnt_60hz_last_us = 0;
                g_tnt_60hz_fraction = 0;
            } else {
                /* Rendering player 1 leaves the global context there. Restore
                 * human ownership before the next frame's physical pad sample. */
                g_PV_ptr = &g_PV_arr[0];
            }
            continue;
        }

        /* FrameAct signals the outer Game only after the WIN/GAME OVER
         * sequence itself has completed. Enter tally on the following loop,
         * after the final finish frame has already been presented. */
        if (tetris.frameAct.unkC && tetris.unk0 == 0) {
            if (attract_active) {
                aiplayer_80042b3c_calls_heap_unalloc();
                FUN_8004129c_fourliner();
                D_800CFEE8 = 17;
                attract_active = FALSE;
                attract_exit_hold = 30;
                gameplay_needs_reset = TRUE;
                continue;
            }
            u32 result_seconds = outer_game_elapsed / 60U;
            u32 result_lines = getLineCount(&tetris.gameStats.linesInfo);
            if (active_game_type == GAMETYPE_SPRINT && result_seconds >= 179U)
                result_seconds = 180U;

            /* gets_lineCounts... marks the sole 1P player as a winner.
             * FUN_SRAM_8007875c therefore awards results.unk38 (the same line
             * count shown on tally) only to saved type-13 players. GUEST has
             * no backing profile and intentionally contributes nothing. */
            {
                u8 tally_rank = 1;

            if (active_profile_slot >= 0) {
                s8 old_milestone = tnt_vita_wonder_milestone_index(
                    TNT_Vita_SramGetTotalWonderLines());
                s8 new_milestone;

                /* tally.c records total score-lines, physical cleared lines,
                 * and elapsed seconds separately before awarding Wonder lines.
                 * Preserve the same player/global score-table update here. */
                (void)TNT_Vita_SramRecordSinglePlayerResult(
                    active_profile_slot, active_game_type,
                    result_lines, (u32)tetris.lineScan.unk3EC,
                    result_seconds);
                TNT_Vita_SramAddWonderLines(result_lines);
                tnt_vita_save_to_sram();
                new_milestone = tnt_vita_wonder_milestone_index(
                    TNT_Vita_SramGetTotalWonderLines());
                if (new_milestone != old_milestone)
                    pending_wonder_milestone = new_milestone;

                /* The original tally recomputes the player's displayed rank
                 * after FUN_SRAM_8007875c has folded this game's time/lines
                 * into the persistent cumulative rank fields.  Our result
                 * recorder has already performed that update above, so read
                 * the fresh profile and use the same canonical rank helper. */
                {
                    TNTVitaStoredPlayer ranked_player;
                    if (TNT_Vita_SramGetPlayer(
                            active_profile_slot, &ranked_player) == 1) {
                        tally_rank = (u8)(TNT_Vita_SramRankFor(
                            ranked_player.time_in_seconds,
                            ranked_player.lines) + 1U);
                    }
                }
            }

            tally_return_to_multi = FALSE;
            tnt_vita_tally_enter(
                &tally, board_x, board_y, cell,
                current_player_name,
                tally_rank,
                result_lines,
                result_seconds);
            }
            gameplay_needs_reset = TRUE;
            continue;
        }

        if (attract_active) {
            /* Source state 12 sets its 4000-tick lifetime to the terminal value
             * on any controller activity.  Exit directly to the restored root
             * frontend; attract never records scores or Wonder progress. */
            attract_ticks++;
            if (g_PV_ptr->unk24 != 0 || attract_ticks >= 4000U) {
                aiplayer_80042b3c_calls_heap_unalloc();
                FUN_8004129c_fourliner();
                D_800CFEE8 = 17;
                attract_active = FALSE;
                attract_exit_hold = 30;
                gameplay_needs_reset = TRUE;
                continue;
            }
        }

        /* TEST103A: only collect timing on ordinary single-player
         * gameplay with user FPS toggle ON. All other modes unchanged. */
        const u8 perf103 = FALSE; /* release: retain the FPS counter, not internal sampling */
        u32 perf103_us[TNT103A_COUNT] = {0};
        u64 perf103_start = perf103 ? sceKernelGetProcessTimeWide() : 0;
        u64 perf103_mark = perf103_start;
        g_tnt103a_1p_active = perf103;
        g_tnt_test103a_gpu_wait_us = 0;
        if (perf103)
            memset(g_tnt103b_frame, 0, sizeof(g_tnt103b_frame));

        s32 pause_action = TNT_VITA_PAUSE_ACTION_NONE;
        {
            u8 pause_was_active = attract_active ? FALSE : TNT_Vita_PauseIsActive(&pause_menu);

            /* Original top-level gameplay state assigns D_801109F4 from
             * frametime_delta() every rendered frame. The N64 scheduler's
             * frameCount advances on every VI retrace, so a rendered frame
             * that spans more than one retrace must advance gameplay by the
             * same number of ticks. This matters for active-piece smoothing,
             * the 20-tick preview transfer, mino morphs, line effects, etc. */
            if (tetris.frameAct.unkC) {
                D_801109F4 = tnt_vita_gamefinish_60hz_delta(TRUE);
            } else {
                /* Keep the finish clock continuously synchronized while
                 * gameplay is active.  An organic top-out can make the last
                 * gameplay frame expensive (a tall settled field is being
                 * updated/rendered).  If GameFinish starts with a stale clock
                 * from the previous game, its first delta incorrectly includes
                 * the entire game and fast-forwards the row sweep + bouncing
                 * GAME OVER image to their final pose. */
                (void)tnt_vita_gamefinish_60hz_delta(FALSE);

                /* The shared 60 Hz clock is also used by the outer Sprint/
                 * Ultra timer below. Sample it once here and carry the same
                 * retrace delta into the original per-player update. */
                D_801109F4 = tnt_vita_60hz_delta(
                    (u8)(!pause_was_active && tetris.unk2));
                if (D_801109F4 == 0)
                    D_801109F4 = 1;
            }

            /* Original Game_800519b4 end-of-game gate. Once no Tetris player
             * remains active, Game_Init's 120-jiffy delay counts down. A new
             * A or Start press after that delay calls Game_80051880, which
             * forces GameFinish to state 4 so Tetris_Update can report done. */
            if (tetris.frameAct.unkC && !tetris.unk2) {
                if (endgame_exit_delay > D_801109F4) {
                    endgame_exit_delay -= (u16)D_801109F4;
                } else {
                    endgame_exit_delay = 0;
                    if (g_PV_ptr->unk28.unk8C & (CONT_A | CONT_START)) {
                        gamefinish_80052D48_twoliner(&tetris.frameAct.unk10);
                    }
                }
            }

            /* Original outer-Game pause trigger uses the controller's newly
             * pressed button mask.  The pause menu then freezes Tetris_Update
             * entirely while its object-alpha fade continues independently.
             */
            if (!attract_active && !pause_was_active && !tetris.frameAct.unkC &&
                (g_PV_ptr->unk28.unk8C & CONT_START)) {
                TNT_Vita_PauseStart(&pause_menu, &tetris);
                pause_was_active = TRUE;
            }

            /* PauseUpdate also owns the shared Game_Init 16-tick alpha
             * interpolator. Attract mode must keep that fade running even
             * though START is deliberately prevented from opening pause. */
            pause_action = TNT_Vita_PauseUpdate(
                &pause_menu, &tetris, g_PV_ptr);

            /* The original outer Game clock and Tetris update observe the
             * same scheduler retrace delta. Do not sample the wall clock a
             * second time here: that would split one source frametime_delta()
             * into two unrelated Vita measurements. */
            if (!pause_was_active && tetris.unk2 && !tetris.frameAct.unkC)
                outer_game_elapsed += D_801109F4;

            if (!pause_was_active) {
                /*
                 * Original New Tetris per-player gameplay update path.
                 * FrameAct now owns falling, locking, multisquare processing,
                 * KeySpin, LineScan, garbage-disabled transition, and spawning.
                 */
                TNT_Vita_MetalField_Update(D_801109F4);
                if (attract_active)
                    aiplayer_8004311c_largefunction();
                (void)Tetris_Update(&tetris);

                /* Original one-player Game_QueryGameOver ordering: top-out
                 * is resolved first, then Sprint/Ultra goal completion. This
                 * means a top-out on the same tick as a goal remains GAME OVER. */
                if (tetris.frameAct.unk3 && !tetris.frameAct.unkC) {
                    FrameAct_calls_gamefinish_something2(
                        &tetris.frameAct, GAMEFINISHTYPE_GAMEOVER);
                    tetris.unk2 = FALSE;
                } else if (tetris.unk2 && !tetris.frameAct.unkC) {
                    u8 goal_completed = FALSE;

                    if (active_game_type == GAMETYPE_SPRINT) {
                        goal_completed = outer_game_elapsed >= 10800U;
                    } else if (active_game_type == GAMETYPE_ULTRA) {
                        goal_completed =
                            getLineCount(&tetris.gameStats.linesInfo) >=
                            tetris.ultraGoal;
                    }

                    if (goal_completed) {
                        FrameAct_calls_gamefinish_something2(
                            &tetris.frameAct, GAMEFINISHTYPE_WIN);
                        tetris.unk2 = FALSE;
                    }
                }
            }
        }

        if (perf103) {
            u64 t = sceKernelGetProcessTimeWide();
            perf103_us[TNT103A_UPDATE] = (u32)(t - perf103_mark);
            perf103_mark = t;
            g_tnt_test103a_measure_gxm = TRUE;
        }
        void *buffer = buffers[backBuffer].data;

        if (g_tnt_active_gameplay_screen == 1) {
            /* Original Greek screen-1 order: rotating Borg skybox 1399 ->
             * static 400x300 foreground -> player name -> fire pot -> rain
             * scene 1318/animation 1319. Keep all five layers in one GXM
             * scene so the Vita does not synchronously finish the GPU twice
             * during every gameplay frame. */
            u8 greek_gxm_drawn = FALSE;
            u8 fire_frame = tnt_vita_advance_greek_fire_pot();
            g_tnt_greek_skybox_angle += 0.05f;
            if (g_tnt_greek_skybox_angle >= 360.0f)
                g_tnt_greek_skybox_angle -= 360.0f;
            g_tnt_greek_rain_ticks += D_801109F4 ? D_801109F4 : 1U;

            if (g_tnt_gameplay_gxm_ready) {
                /* TEST116A: Greek GXM clears the exact uniform base. */
                if (TNT_Vita_GreekGameplayGxmRender(
                        &buffers[backBuffer].surface, buffers[backBuffer].sync,
                        board_x, board_y, cell,
                        g_tnt_greek_skybox_angle,
                        g_tnt_greek_rain_ticks,
                        fire_frame,
                        current_player_name) < 0) {
                    g_tnt_gameplay_gxm_ready = FALSE;
                    memcpy(buffer, g_tnt_static_scene,
                           sizeof(g_tnt_static_scene));
                } else {
                    greek_gxm_drawn = TRUE;
                }
            } else {
                memcpy(buffer, g_tnt_static_scene, sizeof(g_tnt_static_scene));
            }

            if (!greek_gxm_drawn) {
                tnt_vita_draw_font_c_text(
                    buffer, board_x, board_y, cell,
                    293, 201, current_player_name,
                    0xFF, 0xFF, 0xFF, 0xFF);
                tnt_vita_draw_greek_fire_pot_frame(
                    buffer, board_x, board_y, cell, fire_frame);
            }
        } else if (g_tnt_active_gameplay_screen == 2) {
            /* FUN_027BF0_Render screen 2: static Egyptian background -> player
             * name -> coal pot -> candle A -> candle B. */
            /* TEST117A: restore exact pre-rasterized art via GPU; CPU fallback. */
            if (opponent_active || multiplayer_active || attract_active ||
                !g_tnt_gameplay_gxm_ready ||
                TNT_Vita_StaticFrameGxmRestore(
                    &buffers[backBuffer].surface, buffers[backBuffer].sync,
                    g_tnt_static_scene, g_tnt117_background_generation) < 0)
                memcpy(buffer, g_tnt_static_scene, sizeof(g_tnt_static_scene));
            tnt_vita_draw_font_c_text(
                buffer, board_x, board_y, cell,
                293, 202, current_player_name,
                0xFF, 0xFF, 0xFF, 0xFF);
            tnt_vita_draw_egyptian_animated_props(
                buffer, board_x, board_y, cell);
        } else if (g_tnt_active_gameplay_screen == 3) {
            /* FUN_027BF0_Render screen 3: animated Borg sky 487 -> static
             * foreground -> name -> lamp -> fresh-depth flying leaves 1335. */
            u32 celtic_delta = D_801109F4 ? D_801109F4 : 1U;
            u8 lamp_frame = tnt_vita_advance_sprite_frame(
                &g_tnt_celtic_lamp_frame, &g_tnt_celtic_lamp_hold, 4, 2);
            u8 celtic_gxm_drawn = FALSE;
            g_tnt_celtic_sky_ticks += celtic_delta;
            g_tnt_celtic_leaf_ticks += celtic_delta;
            if (g_tnt_gameplay_gxm_ready) {
                /* TEST118A: Celtic GXM clears the exact uniform base. */
                if (TNT_Vita_CelticGameplayGxmRender(
                        &buffers[backBuffer].surface, buffers[backBuffer].sync,
                        board_x, board_y, cell,
                        g_tnt_celtic_sky_ticks, g_tnt_celtic_leaf_ticks,
                        lamp_frame, current_player_name) < 0) {
                    g_tnt_gameplay_gxm_ready = FALSE;
                    memcpy(buffer, g_tnt_static_scene,
                           sizeof(g_tnt_static_scene));
                } else {
                    celtic_gxm_drawn = TRUE;
                }
            } else {
                memcpy(buffer, g_tnt_static_scene, sizeof(g_tnt_static_scene));
            }
            if (!celtic_gxm_drawn) {
                tnt_vita_draw_font_c_text(
                    buffer, board_x, board_y, cell,
                    293, 202, current_player_name,
                    0xFF, 0xFF, 0xFF, 0xFF);
                tnt_vita_draw_celtic_lamp_frame(
                    buffer, board_x, board_y, cell, lamp_frame);
            }
        } else if (g_tnt_active_gameplay_screen == 4) {
            /* FUN_027BF0_Render screen 4: animated African sky 567 -> static
             * foreground -> name -> D_800D2C38 volcano smoke. */
            u32 african_delta = D_801109F4 ? D_801109F4 : 1U;
            u8 smoke_frame = tnt_vita_advance_sprite_frame_ticks(
                &g_tnt_african_smoke_frame, &g_tnt_african_smoke_hold,
                TNT_AFRICAN_SMOKE_FRAME_COUNT, 3, african_delta);
            u8 african_gxm_drawn = FALSE;
            g_tnt_african_sky_ticks += african_delta;
            if (g_tnt_gameplay_gxm_ready) {
                /* TEST119A: African GXM clears the exact uniform base. */
                if (TNT_Vita_AfricanGameplayGxmRender(
                        &buffers[backBuffer].surface, buffers[backBuffer].sync,
                        board_x, board_y, cell,
                        g_tnt_african_sky_ticks, smoke_frame,
                        current_player_name) < 0) {
                    g_tnt_gameplay_gxm_ready = FALSE;
                    memcpy(buffer, g_tnt_static_scene,
                           sizeof(g_tnt_static_scene));
                } else {
                    african_gxm_drawn = TRUE;
                }
            } else {
                memcpy(buffer, g_tnt_static_scene, sizeof(g_tnt_static_scene));
            }
            if (!african_gxm_drawn) {
                tnt_vita_draw_font_c_text(
                    buffer, board_x, board_y, cell,
                    293, 202, current_player_name,
                    0xFF, 0xFF, 0xFF, 0xFF);
                tnt_vita_draw_african_smoke_frame(
                    buffer, board_x, board_y, cell, smoke_frame);
            }
        } else if (g_tnt_active_gameplay_screen == 5) {
            /* FUN_027BF0_Render screen 5: rotating Borg sky 770 -> static
             * foreground -> name -> water -> five staggered sparkles ->
             * fresh-depth butterfly scene 1360 driven by animation 1361. */
            u32 japanese_delta = D_801109F4 ? D_801109F4 : 1U;
            u8 japanese_gxm_drawn = FALSE;
            g_tnt_japanese_skybox_angle -= 0.05f;
            if (g_tnt_japanese_skybox_angle <= -360.0f)
                g_tnt_japanese_skybox_angle += 360.0f;
            g_tnt_japanese_effect_ticks += japanese_delta;
            g_tnt_japanese_butterfly_ticks += japanese_delta;
            if (g_tnt_gameplay_gxm_ready) {
                /* TEST120A: Japanese GXM clears the exact uniform base. */
                if (TNT_Vita_JapaneseGameplayGxmRender(
                        &buffers[backBuffer].surface, buffers[backBuffer].sync,
                        board_x, board_y, cell,
                        g_tnt_japanese_skybox_angle,
                        g_tnt_japanese_effect_ticks,
                        g_tnt_japanese_butterfly_ticks,
                        current_player_name) < 0) {
                    g_tnt_gameplay_gxm_ready = FALSE;
                    memcpy(buffer, g_tnt_static_scene,
                           sizeof(g_tnt_static_scene));
                } else {
                    japanese_gxm_drawn = TRUE;
                }
            } else {
                memcpy(buffer, g_tnt_static_scene, sizeof(g_tnt_static_scene));
            }
            if (!japanese_gxm_drawn) {
                tnt_vita_draw_font_c_text(
                    buffer, board_x, board_y, cell,
                    293, 202, current_player_name,
                    0xFF, 0xFF, 0xFF, 0xFF);
            }
        } else if (g_tnt_active_gameplay_screen == 6) {
            /* FUN_027BF0_Render screen 6: animated Russian sky 901/902 ->
             * keyed foreground -> name -> thirteen independently phased
             * candle flames -> fresh-depth snow scene 1370/1371. */
            u32 russian_delta = D_801109F4 ? D_801109F4 : 1U;
            u8 russian_gxm_drawn = FALSE;
            u8 ci;
            g_tnt_russian_sky_ticks += russian_delta;
            g_tnt_russian_snow_ticks += russian_delta;
            for (ci = 0; ci < TNT_RUSSIAN_CANDLE_INSTANCE_COUNT; ci++)
                (void)tnt_vita_advance_sprite_frame_ticks(
                    &g_tnt_russian_candle_frame[ci], &g_tnt_russian_candle_hold[ci],
                    TNT_RUSSIAN_CANDLE_FRAME_COUNT, g_tnt_russian_candle_intra[ci],
                    russian_delta);
            if (g_tnt_gameplay_gxm_ready) {
                /* TEST121A: Russian GXM clears the exact uniform base. */
                if (TNT_Vita_RussianGameplayGxmRender(
                        &buffers[backBuffer].surface, buffers[backBuffer].sync,
                        board_x, board_y, cell,
                        g_tnt_russian_sky_ticks, g_tnt_russian_snow_ticks,
                        g_tnt_russian_candle_frame, current_player_name) < 0) {
                    g_tnt_gameplay_gxm_ready = FALSE;
                    memcpy(buffer, g_tnt_static_scene,
                           sizeof(g_tnt_static_scene));
                } else {
                    russian_gxm_drawn = TRUE;
                }
            } else {
                memcpy(buffer, g_tnt_static_scene, sizeof(g_tnt_static_scene));
            }
            if (!russian_gxm_drawn) {
                tnt_vita_draw_font_c_text(
                    buffer, board_x, board_y, cell,
                    293, 202, current_player_name,
                    0xFF, 0xFF, 0xFF, 0xFF);
                tnt_vita_draw_russian_candles(
                    buffer, board_x, board_y, cell);
            }
        } else if (g_tnt_active_gameplay_screen == 7) {
            /* FUN_027BF0_Render screen 7: static FINALE/Industrial background
             * -> player name -> three boiler flashes -> eight staggered bulbs
             * -> seven-frame gauge needle. */
            u32 finale_delta = D_801109F4 ? D_801109F4 : 1U;
            /* TEST122A: exact full-scene GPU static-art restore. Keep
             * background+boiler CPU cache for the flashing boiler rect. */
            if (opponent_active || multiplayer_active || attract_active ||
                !g_tnt_gameplay_gxm_ready ||
                TNT_Vita_StaticFrameGxmRestore(
                    &buffers[backBuffer].surface, buffers[backBuffer].sync,
                    g_tnt_static_scene, g_tnt117_background_generation) < 0)
                memcpy(buffer, g_tnt_static_scene, sizeof(g_tnt_static_scene));
            tnt_vita_draw_font_c_text(
                buffer, board_x, board_y, cell,
                293, 202, current_player_name,
                0xFF, 0xFF, 0xFF, 0xFF);
            tnt_vita_draw_finale_effects(
                buffer, board_x, board_y, cell, finale_delta);
        } else {
            /* Original Mayan background order is moon -> Borg skybox 201 ->
             * playfield mask -> foreground artwork -> player name. Asset 201
             * rotates by 0.05 degrees each rendered background update. */
            u64 perf103b_bgstamp = perf103 ? sceKernelGetProcessTimeWide() : 0;
            g_tnt_mayan_skybox_angle += 0.05f;
            if (g_tnt_gameplay_gxm_ready) {
                /* Test 20: Mayan's dark underlay + moon are now rendered by
                 * GXM in source order, eliminating the ~2 MiB CPU memcpy. */
                if (TNT_Vita_MayanSkyboxGxmRender(
                        &buffers[backBuffer].surface, buffers[backBuffer].sync,
                        board_x, board_y, cell,
                        g_tnt_mayan_skybox_angle) < 0) {
                    g_tnt_gameplay_gxm_ready = FALSE;
                    memcpy(buffer, g_tnt_static_scene,
                           sizeof(g_tnt_static_scene));
                }
            } else {
                memcpy(buffer, g_tnt_static_scene, sizeof(g_tnt_static_scene));
            }

            if (perf103)
                tnt103b_mark(TNT103B_SKY, &perf103b_bgstamp);
            if (!opponent_active && !multiplayer_active && !attract_active)
                tnt115_draw_mayan_name(
                    buffer, board_x, board_y, cell, current_player_name);
            else
            tnt_vita_draw_font_c_text(
                buffer, board_x, board_y, cell,
                290, 202, current_player_name,
                0xFF, 0xFF, 0xFF, 0xFF);

            tnt_vita_draw_mayan_temple_fires(
                buffer, board_x, board_y, cell);
            if (perf103)
                tnt103b_mark(TNT103B_MINOR, &perf103b_bgstamp);

            TNT_Vita_MayanLargeFire_Update(&mayan_large_fire);
            TNT_Vita_MayanLargeFire_Render(
                buffer, DISPLAY_STRIDE, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                board_x, board_y, cell, &mayan_large_fire);
            if (perf103)
                tnt103b_mark(TNT103B_FIRE, &perf103b_bgstamp);
        }

        if (perf103) {
            u64 t = sceKernelGetProcessTimeWide();
            g_tnt_test103a_measure_gxm = FALSE;
            perf103_us[TNT103A_BACKGROUND] = (u32)(t - perf103_mark);
            perf103_us[TNT103A_GPUWAIT] = g_tnt_test103a_gpu_wait_us;
            perf103_mark = t;
        }
        /* TEST107A: active only for the 1P board's settled cells. */
        g_tnt107_hits=g_tnt107_misses=g_tnt107_fallbacks=0;
        g_tnt123_dynamic_rendered=0;
        g_tnt107_locked_active=(u8)(!attract_active &&
            !opponent_active && !multiplayer_active);
        /* Display locked cells from the actual New Tetris board state. */
        for (int by = 0; by < 20; by++) {
            for (int bx = 0; bx < 10; bx++) {
                Cell *board_cell =
                    g_boardPieces_ptr->cell_ptr_arr[bx + (by * 10)];

                if (board_cell->piece_type != EMPTY_CELL &&
                    !TNT_Vita_LineEffectCellHidden(
                        &tetris.lineScan, board_cell)) {
                    /*
                     * Original BoardPieces chooses the visible locked-cube
                     * color through BoardP_8006d3c0: normal cells use their
                     * piece definition color, while active multisquares use
                     * the original gold/silver colors.  BoardPieces also
                     * selects the same-piece 8-neighbour CubeTile target; the
                     * Vita Mino texture generator consumes the resulting live
                     * mino->unk24 gradient below, including its morph timing.
                     */
                    Color *locked_tnt_color =
                        BoardP_8006d3c0_fourliner(board_cell);

                    /*
                     * Locked cells now own real original-style Cube/Mino
                     * visual state. Use the Mino's current interpolated color
                     * so multisquare and metal transitions come from the actual
                     * original morph state rather than renderer-side timing.
                     */
                    if (board_cell->cubeInfo.cube_ptr != NULL) {
                        locked_tnt_color =
                            &board_cell->cubeInfo.cube_ptr->mino.unk4;
                    }

                    s32 locked_x_fixed = bx << 8;
                    s32 locked_y_fixed = by << 8;

                    /*
                     * A real original-style Cube now owns the visual transform.
                     * BoardPieces swaps update its target while MobileCubes_Move
                     * advances the current translation by 0x40 per tick.  The
                     * logical board still changes immediately, but rows above a
                     * clear can now visibly settle using the original motion.
                     */
                    if (board_cell->cubeInfo.cube_ptr != NULL) {
                        locked_x_fixed =
                            board_cell->cubeInfo.cube_ptr->unk2.unk6.p.x;
                        locked_y_fixed =
                            board_cell->cubeInfo.cube_ptr->unk2.unk6.p.y;
                    }

                    /* Do not invent a diagnostic gap around locked minos.
                     * Their visual separation is already supplied by the live
                     * source CubeTile/spotlight texture path. */
                    if (board_cell->cubeInfo.cube_ptr != NULL) {
                        draw_cube_mino_textured(
                            buffer,
                            tnt_fixed_edge_to_pixel(
                                locked_x_fixed, board_x, cell),
                            tnt_fixed_edge_to_pixel(
                                locked_y_fixed, board_y, cell),
                            cell, cell,
                            board_cell->cubeInfo.cube_ptr
                        );
                    } else {
                        draw_rect(
                            buffer,
                            tnt_fixed_edge_to_pixel(
                                locked_x_fixed, board_x, cell),
                            tnt_fixed_edge_to_pixel(
                                locked_y_fixed, board_y, cell),
                            cell,
                            cell,
                            COLOR(
                                tnt_color_channel_to_u8(locked_tnt_color->diffuse_red),
                                tnt_color_channel_to_u8(locked_tnt_color->diffuse_green),
                                tnt_color_channel_to_u8(locked_tnt_color->diffuse_blue)
                            )
                        );
                    }
                }
            }
        }

        g_tnt107_locked_active=FALSE;
        /*
         * Original FallingCubes remain active MobileCubes after a cleared
         * Board Cell releases ownership. Render those same Cube transforms
         * while FallingCubes_Update drops them by 0x80 per game tick.
         */
        if (g_fallingCubes_ptr != NULL &&
            g_fallingCubes_ptr->cube_ptr_list != NULL) {
            for (int fi = 0; fi < g_fallingCubes_ptr->max_allowed; fi++) {
                Cube *falling_cube =
                    g_fallingCubes_ptr->cube_ptr_list[fi];

                if (falling_cube != NULL) {
                    draw_cube_mino_textured(
                        buffer,
                        tnt_fixed_edge_to_pixel(
                            falling_cube->unk2.unk6.p.x,
                            board_x,
                            cell),
                        tnt_fixed_edge_to_pixel(
                            falling_cube->unk2.unk6.p.y,
                            board_y,
                            cell),
                        cell, cell,
                        falling_cube
                    );
                }
            }
        }

        if (perf103) {
            u64 t = sceKernelGetProcessTimeWide();
            perf103_us[TNT103A_LOCKED] = (u32)(t - perf103_mark);
            perf103_mark = t;
        }
        u64 perf103b_xstamp = perf103 ? sceKernelGetProcessTimeWide() : 0;
        /* Original GameStats line count: Font C, original 1P board-relative
         * placement and variable-width digit spacing.
         */
        draw_original_font_c_line_count(
            buffer,
            board_x,
            board_y,
            cell,
            getLineCount(&tetris.gameStats.linesInfo)
        );

        /* Original Mayan one-player mode HUD from FUN_027BF0_Render. Sprint
         * counts down from 3:00; Ultra shows elapsed time and its line goal. */
        if (active_game_type == GAMETYPE_SPRINT) {
            u32 elapsed = outer_game_elapsed;
            u32 remaining = elapsed < 10800U ? 10800U - elapsed : 0U;
            tnt_vita_draw_original_time(
                buffer, board_x, board_y, cell, 343, 230, remaining);
        } else if (active_game_type == GAMETYPE_ULTRA) {
            char goal_text[20];
            tnt_vita_draw_original_time(
                buffer, board_x, board_y, cell,
                343, 230, outer_game_elapsed);
            snprintf(goal_text, sizeof(goal_text), "%u",
                (unsigned)tetris.ultraGoal);
            tnt_vita_draw_font_c_text(
                buffer, board_x, board_y, cell,
                280, 260, goal_text, 0xFF, 0xFF, 0xFF, 0xFF);
        }

        if (perf103)
            tnt103b_mark(TNT103B_HUD, &perf103b_xstamp);
        /*
         * Vita backend for the original NextPieces_Render state.
         * The queue itself already owns the original 20-frame slide/fade/scale
         * animation; this draws those exact positions instead of the old blue
         * diagnostic list.
         */
        /* TEST104A DIAGNOSTIC ONLY — isolate the already field-tested 2P
         * moving-mino fast sampler and safely cached texture setup to the
         * one-player NEXT queue, while preserving N64 source positions,
         * interpolation, alpha, and order. The fast path only handles
         * axis-aligned residual==0 minos; animated rotations fall back
         * to original affine raster. Restore global immediately afterward.
         * Keep this change out of the approved production source until
         * hardware comparison shows no alignment, texture or fade change. */
        u8 tnt104a_prev_fast = g_tnt_local_2p_fast_render;
        if (!opponent_active && !multiplayer_active && !attract_active)
            g_tnt_local_2p_fast_render = TRUE;
        u8 tnt113_prev_scratch = g_tnt113_next_scratch_active;
        g_tnt113_next_scratch_active =
            !opponent_active && !multiplayer_active && !attract_active;
        for (int ni = 0; ni < 4; ni++) {
            NextPiece *next_piece = &tetris.board.nextPieces.buf[ni];
            NextPieceShadow *shadow = &next_piece->shadow;
            Piece *shadow_piece = &shadow->piece;
            Piece *piece = &next_piece->piece;
            int shadow_alpha;
            int next_alpha;

            /* Same state copies/alpha math as original NextPieceShadow_Render. */
            shadow_piece->physicalPos = shadow->physicalPos;
            shadow_piece->physicalPos.x += 0x80;
            shadow_piece->physicalPos.y += 0x80;
            if (tetris.board.nextPieces.alpha == 0xFF) {
                shadow_alpha = shadow->alpha_sl4b >> 4;
            } else {
                shadow_alpha =
                    (shadow->alpha_sl4b * tetris.board.nextPieces.alpha) / 4096U;
            }
            if (shadow_alpha < 0)
                shadow_alpha = 0;
            if (shadow_alpha > 0xFF)
                shadow_alpha = 0xFF;

            for (int mi = 0; mi < 4; mi++) {
                Point mp;

                TNT_Vita_MobilePieceGetMinoPointScaled(
                    shadow_piece, mi, shadow->mino_scale, &mp);
                draw_piece_mino_textured_alpha(
                    buffer,
                    board_x,
                    board_y,
                    cell,
                    mp,
                    shadow->mino_scale,
                    shadow_piece,
                    (u8)mi,
                    tnt_mino_alpha(
                        &shadow_piece->mino_arr[mi], (u8)shadow_alpha)
                );
            }

            /* Same state copies/alpha math as original NextPiece_Render. */
            piece->physicalPos = next_piece->physicalPos;
            piece->physicalRotOrigin = next_piece->physicalRotOrigin;
            if (tetris.board.nextPieces.alpha == 0xFF) {
                next_alpha = next_piece->alpha_sl4b >> 4;
            } else {
                next_alpha =
                    (next_piece->alpha_sl4b * tetris.board.nextPieces.alpha) / 4096U;
            }
            if (next_alpha < 0)
                next_alpha = 0;
            if (next_alpha > 0xFF)
                next_alpha = 0xFF;

            for (int mi = 0; mi < 4; mi++) {
                Point mp;

                TNT_Vita_MobilePieceGetMinoPointScaled(
                    piece, mi, next_piece->mino_scale, &mp);
                draw_piece_mino_textured_alpha(
                    buffer,
                    board_x,
                    board_y,
                    cell,
                    mp,
                    next_piece->mino_scale,
                    piece,
                    (u8)mi,
                    tnt_mino_alpha(
                        &piece->mino_arr[mi], (u8)next_alpha)
                );
            }
        }

        /* TEST104B: keep the same existing axis-aligned raster path enabled
         * through 1P HOLD and CURRENT/GHOST drawing, while protecting the
         * entire surrounding scene by restoring the original flag after it.
         * Any residual physical rotation automatically takes original affine
         * raster path. No controls, timing, mino state or draw order modified.
         * Existing 2P/3P/4P routes remain entirely unchanged. */
        if (perf103)
            tnt103b_mark(TNT103B_NEXT, &perf103b_xstamp);
        /*
         * Vita backend for the original PieceHoldPiece render state.
         * PieceHold_Update owns position/rotation/scale/flashing; this only
         * converts that existing N64 8.8 geometry to the diagnostic board.
         * Both buffers are drawn because original PieceHold_Render does so
         * during the 16-frame swap animation.
         */
        for (int hi = 0; hi < 2; hi++) {
            PieceHoldPiece *hold_piece = &tetris.pieceHold.buf[hi];

            if (hold_piece->state != 0) {
                PieceHoldPieceShadow *hold_shadow = &hold_piece->shadow;
                Piece *piece = &hold_piece->piece;

                /* Original PieceHoldPiece_Render draws the black swap shadow
                 * first through the same MobilePiece/Mino texture path.
                 */
                if (hold_shadow->unk0 != 0) {
                    Piece *shadow_piece = &hold_shadow->piece;
                    int shadow_alpha;

                    if (hold_shadow->alpha == 0xFF) {
                        shadow_alpha = hold_shadow->unk2 >> 4;
                    } else {
                        shadow_alpha =
                            (hold_shadow->unk2 * hold_shadow->alpha) >> 12;
                    }
                    if (shadow_alpha < 0) shadow_alpha = 0;
                    if (shadow_alpha > 0xFF) shadow_alpha = 0xFF;

                    for (int mi = 0; mi < 4; mi++) {
                        Point mp;

                        TNT_Vita_MobilePieceGetMinoPointScaled(
                            shadow_piece,
                            mi,
                            hold_shadow->mino_scale,
                            &mp
                        );
                        draw_piece_mino_textured_alpha(
                            buffer,
                            board_x,
                            board_y,
                            cell,
                            mp,
                            hold_shadow->mino_scale,
                            shadow_piece,
                            (u8)mi,
                            tnt_mino_alpha(
                                &shadow_piece->mino_arr[mi],
                                (u8)shadow_alpha)
                        );
                    }
                }

                /* Same state copy performed by original PieceHoldPiece_Render. */
                piece->physicalRotOrigin = hold_piece->physicalRotOrigin;
                piece->physicalPos = hold_piece->physicalPos;
                piece->physicalCkwRotValue =
                    hold_piece->physicalCkwRotValue;

                for (int mi = 0; mi < 4; mi++) {
                    Point mp;

                    TNT_Vita_MobilePieceGetMinoPointScaled(
                        piece,
                        mi,
                        hold_piece->mino_scale,
                        &mp
                    );

                    draw_piece_mino_textured_alpha(
                        buffer,
                        board_x,
                        board_y,
                        cell,
                        mp,
                        hold_piece->mino_scale,
                        piece,
                        (u8)mi,
                        tnt_mino_alpha(&piece->mino_arr[mi], piece->alpha)
                    );
                }
            }
        }

        if (perf103)
            tnt103b_mark(TNT103B_HOLD, &perf103b_xstamp);
        /*
         * Vita backend for the original CurrentPiece_Render path.  The active
         * piece uses its real physical position/rotation/color state and the
         * original GhostPiece state is drawn first at 0x60/0x100 opacity.
         */
        if (tetris.board.currentPiece.state == 1) {
            CurrentPiece *current_piece = &tetris.board.currentPiece;
            GhostPiece *ghost = &current_piece->ghostPiece;
            Piece *ghost_piece = &ghost->piece;
            Piece *piece = &current_piece->piece;

            ghost_piece->alpha = (current_piece->alpha * 0x60) >> 8;

            /* Same visibility test performed by original GhostPiece_Render. */
            if (ghost->unk0 &&
                MobilePiece_800651c8_sevenliner_loops_4_times_v2_retbool(
                    ghost_piece)) {
                for (int mi = 0; mi < 4; mi++) {
                    Point mp;
                    u8 alpha = tnt_mino_alpha(
                        &ghost_piece->mino_arr[mi], ghost_piece->alpha);

                    TNT_Vita_MobilePieceGetMinoPointScaled(
                        ghost_piece, mi, 0x100, &mp);
                    draw_piece_mino_textured_alpha(
                        buffer,
                        board_x,
                        board_y,
                        cell,
                        mp,
                        0x100,
                        ghost_piece,
                        (u8)mi,
                        alpha
                    );
                }
            }

            piece->alpha = current_piece->alpha;
            for (int mi = 0; mi < 4; mi++) {
                Point mp;
                u8 alpha = tnt_mino_alpha(
                    &piece->mino_arr[mi], piece->alpha);

                TNT_Vita_MobilePieceGetMinoPointScaled(
                    piece, mi, 0x100, &mp);
                draw_piece_mino_textured_alpha(
                    buffer,
                    board_x,
                    board_y,
                    cell,
                    mp,
                    0x100,
                    piece,
                    (u8)mi,
                    alpha
                );
            }
        }

        g_tnt113_next_scratch_active = tnt113_prev_scratch;
        g_tnt_local_2p_fast_render = tnt104a_prev_fast;
        if (perf103)
            tnt103b_mark(TNT103B_CURRENT, &perf103b_xstamp);
        /* Original Tetris_Render draws active multisquare glows after the
         * MobilePiece/hold/preview layers. Use the ROM-derived 66x66 IA8 mask
         * and the SquareGlow half-sine alpha state restored above.
         */
        TNT_Vita_RenderMultisquareGlows(
            buffer,
            DISPLAY_STRIDE,
            DISPLAY_WIDTH,
            DISPLAY_HEIGHT,
            board_x,
            board_y,
            cell,
            &tetris.squares
        );

        /* Original FrameAct_Render is the final Tetris render stage. */
        TNT_Vita_GameFinish_Render(
            buffer,
            DISPLAY_WIDTH,
            DISPLAY_HEIGHT,
            DISPLAY_STRIDE,
            board_x,
            board_y,
            cell,
            &tetris.frameAct.unk10
        );

        if (perf103)
            tnt103b_mark(TNT103B_GLOW, &perf103b_xstamp);
        /* FUN_027BF0_Render attract state 12: Font C "PRESS START" at
         * native (265,110), visible for phase 1..19/0 of each 30-tick cycle.
         * The source advances the phase by frametime_delta() after drawing. */
        if (attract_active) {
            if (attract_prompt_phase < 20U) {
                /* Attract shares the ordinary gameplay render budget. Keep its
                 * canonical Font-C PRESS START overlay, but submit the same
                 * source glyphs through GXM so the flashing prompt cannot push
                 * heavier boards (notably Russian) below their normal 30 FPS. */
                if (!g_tnt_milestone_gxm_ready ||
                    TNT_Vita_FontCGxmRenderText(
                        &buffers[backBuffer].surface, buffers[backBuffer].sync,
                        board_x, board_y, cell, 265, 110, TNTV_ROM_OriginalText(3U)) < 0) {
                    tnt_vita_draw_font_c_text(
                        buffer, board_x, board_y, cell,
                        265, 110, TNTV_ROM_OriginalText(3U),
                        0xFF, 0xFF, 0xFF, 0xFF);
                }
            }
            attract_prompt_phase = (u8)(
                (attract_prompt_phase +
                 (D_801109F4 ? D_801109F4 : 1U)) % 30U);
        }

        /* Original outer pause renderer is drawn after the game. */
        if (!TNT_Vita_PauseIsActive(&pause_menu) ||
            !g_tnt_milestone_gxm_ready ||
            TNT_Vita_PauseGxmRender(
                &buffers[backBuffer].surface, buffers[backBuffer].sync,
                board_x, board_y, cell,
                pause_menu.selected, pause_menu.pulse) < 0) {
            TNT_Vita_PauseRender(
                buffer,
                DISPLAY_STRIDE,
                DISPLAY_WIDTH,
                DISPLAY_HEIGHT,
                board_x,
                board_y,
                cell,
                &pause_menu
            );
        }

        /* Select the gameplay crop for the final presentation pass.  Drawing
         * the bezel inside swap_buffers() guarantees it is above HUD, pause and
         * attract overlays and avoids a second inner rail at the 4:3 boundary. */
        g_tnt_presentation_crop = (u8)tnt_vita_gameplay_side_mask_width(
            g_tnt_active_gameplay_screen);

        if (perf103)
            tnt103b_mark(TNT103B_OTHER, &perf103b_xstamp);
        if (perf103) {
            u64 t = sceKernelGetProcessTimeWide();
            perf103_us[TNT103A_EXTRA] = (u32)(t - perf103_mark);
            perf103_mark = t;
        }
        g_tnt110_mayan_bezel_eligible=(u8)(
            !attract_active && !opponent_active &&
            !multiplayer_active && g_tnt_active_gameplay_screen==0 &&
            g_tnt_gameplay_gxm_ready);
        swap_buffers();
        g_tnt110_mayan_bezel_eligible=FALSE;
        if (perf103) {
            u64 t = sceKernelGetProcessTimeWide();
            perf103_us[TNT103A_PRESENT] = (u32)(t - perf103_mark);
            perf103_us[TNT103A_TOTAL] = (u32)(t - perf103_start);
            tnt103a_sample(perf103_us);
            tnt103b_sample();
        }

        if (pause_action == TNT_VITA_PAUSE_ACTION_PROGRESS) {
            /* Original pause PROGRESS tears down the popup after this final
             * paused game frame, then state 8 owns the Wonder viewer until B. */
            wonder_origin = TNT_WONDER_ORIGIN_PAUSE;
            tnt_vita_wonder_enter(
                &wonder_viewer, board_x, board_y, cell);
        } else if (pause_action == TNT_VITA_PAUSE_ACTION_EXIT) {
            /* Original PAUSEMENU_EXIT routes through tally's pause-exit
             * branch, which immediately tears down the game and restores the
             * one-player GUI without showing results. Do the same after the
             * final paused gameplay frame has been presented. */
            FUN_026C80_80060ad4_oneliner_calls_fun(&g_PV_ptr->unk28);
            FUN_026C80_80060b04_twelveliner_loops_32t(
                &g_PV_ptr->unk28, 0x0F00, 32, 10);
            FUN_026C80_80060b04_twelveliner_loops_32t(
                &g_PV_ptr->unk28, 0xC000, 16, 16);
            FUN_026C80_nineliner_loops_32t(&g_PV_ptr->unk28);
            tnt_vita_onep_enter(
                &onep_menu, onep_menu.pulse, onep_menu.pulse_rate);
            onep_menu.game_type = active_game_type;
            gameplay_needs_reset = TRUE;
        }
    }

    FallingCubes_Deinit();
    MobileCubes_Deinit();
    TNT_Vita_MetalField_Deinit();
    TNT_Vita_CubeTilesDeinit();
    TNT_Vita_AudioTerm();
    graphics_term();

    sceKernelExitProcess(0);
    return 0;
}
