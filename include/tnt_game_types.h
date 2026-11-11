#ifndef TNT_GAME_TYPES_H
#define TNT_GAME_TYPES_H

#include "vita_compat.h"

/* Small original utility types needed by the gameplay structures. */
typedef struct {
    s16 x;
    s16 y;
} Point;

typedef struct UnkStruct_0 UnkStruct_0;

/* Original GameVars layout. */
typedef struct {
    u32 seed;
    UnkStruct_0 *unk4;
    u8 screen;
    u8 handicap;
    u8 gameType;
} GameVars;

#define GAMETYPE_MARATHON 0
#define GAMETYPE_SPRINT   1
#define GAMETYPE_ULTRA    2

typedef struct {
    s8 x;
    s8 y;
} Position;

typedef struct {
    s16 x_min;
    s16 y_min;
    s16 x_max;
    s16 y_max;
} BoundingBox;

typedef struct {
    s16 diffuse_red;
    s16 diffuse_green;
    s16 diffuse_blue;
    s16 specular_red;
    s16 specular_green;
    s16 specular_blue;
} Color;

typedef struct {
    Point p;
    u16 angle;
} PointAngle;

typedef struct {
    PointAngle unk0;
    PointAngle unk6;
    PointAngle unkC;
} UnkStruct_8;

typedef struct {
    s8 y;
    s8 x;
} Gradient2D;

typedef struct {
    Gradient2D gradients[64];
} CubeTile;

typedef struct {
    CubeTile unk0[440];
    CubeTile *unkDC00[256];
} CubeTiles;

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 pad03;
    Color unk4;
    Color unk10;
    u8 unk1C;
    s8 unk1D;
    u8 unk1E;
    u8 pad1F;
    s16 unk20;
    s16 unk22;
    CubeTile unk24;
    u8 padA4[0x0C];
    u8 unkB0[0x40];
    u8 unkF0[0x40];
    CubeTile *unk130;
    u8 pad134[4];
} Mino;

typedef struct {
    u8 is_active;
    u8 pad01;
    UnkStruct_8 unk2;
    UnkStruct_8 unk14;
    u8 unk26;
    u8 pad27;
    Mino mino;
} Cube;

typedef struct {
    u8 piece_type;
    Position local_mino_pos_arr[4];
    Point center;
    Position rot_origin_pos_arr[4];
    Color *color_ptr;
    Position curpiece_spawn_offset_pos;
    u8 starting_rot_state;
} PieceDefinition;

typedef struct {
    u8 alpha;
    u8 pad01[3];
    PieceDefinition *pieceDef_ptr;
    u16 physicalCkwRotValue;
    Point physicalRotOrigin;
    Point physicalPos;
    u8 pad12[6];
    Mino mino_arr[4];
} Piece;

typedef struct {
    u8 enabled;
    u8 state;
    Color alt_color;
    Color main_color;
    Piece *piece_ptr;
} PieceColorFlasher;

typedef struct {
    u8 unk0;
    u8 unk1[7];
    Piece piece;
    PieceColorFlasher flasher;
    PieceDefinition *pieceDef_ptr;
    s16 unk524;
    s16 unk526;
    u16 unk528;
    u8 unk52A;
    u8 pad52B;
} GhostPiece;

typedef struct {
    u8 state;
    u8 qWinLoseIndicator;
    u8 possibleMoves;
    u8 isLocked;
    u8 lockDelay;
    u8 pad05;
    s16 fallVelocity;
    s16 fallVelocityCopy;
    u8 logicalRotCkwState;
    u8 pad0B;
    s16 rotate_angle_step;
    u8 num_rotate_angle_steps;
    Position logicalRotOrigin;
    Position logicalPos;
    u8 pieceType;
    u8 alpha;
    Position minoLockedPos_arr[4];
    u8 pad1D[3];
    Piece piece;
    PieceColorFlasher flasher;
    u8 unk538;
    u8 pad539[7];
    GhostPiece ghostPiece;
} CurrentPiece;

/* Original structures progressively replacing opaque placeholders. */

typedef struct Cell_s Cell;

typedef struct {
    u8 state;
    u8 type;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 col_step;
    u8 pad06[2];
    Cell *cells[10];
} LineEffect;

typedef struct {
    u8 unk0;
    u8 unk1;
} UnkStruct_5;

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    UnkStruct_5 unk3[20];
    LineEffect unk2C[20];
    s32 unk3EC;
    u32 unk3F0;
    u8 unk3F4;
    u8 pad3F5[3];
} LineScan;

typedef struct {
    u8 is_active;
    u8 size;
    u16 val;
    u16 rate;
    u8 num_steps;
    u8 pad07;
    Point location;
    u8 alpha;
    u8 pad0D;
} SquareGlow;

typedef struct {
    u8 type;
    u8 unk1;
    u8 id;
    u8 unk3;
    SquareGlow glow;
} Square;

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 delay;
    u8 cnt;
    Square arr[44];
} Squares;

typedef struct {
    u8 unk0;
    u8 unk1;
    s8 unk2;
    s8 unk3;
    s8 unk4;
    u8 pad05;
    s16 unk6;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s8 unkE;
    s8 unkF;
    u8 unk10;
    u8 unk11;
    u8 unk12;
    u8 unk13;
} UnkStruct_19;

typedef struct {
    u8 unk0;
    s8 unk1;
    s8 unk2;
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    u8 unk8;
} UnkStruct_36;

typedef struct {
    u8 unk0;
    s8 unk1;
    s8 unk2;
    u8 unk3;
    s8 unk4;
    s8 unk5;
} UnkStruct_37;

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 pad03;
    CurrentPiece *currentPiece_ptr;
    UnkStruct_19 unk8[4];
    u8 unk58;
    UnkStruct_36 unk59;
    UnkStruct_37 unk62;
} UnkStruct_9;

typedef struct {
    u8 pattern;          /* 0x00 */
    u8 pad1;             /* 0x01 */
    u16 unk2;            /* 0x02 */
    u16 unk4;            /* 0x04 */
    u16 scale;           /* 0x06 */
    u16 unk8;            /* 0x08 */
    s16 dsdx;            /* 0x0A */
    s16 dtdy;            /* 0x0C */
    Point unkE;          /* 0x0E */
    Point unk12;         /* 0x12 */
    Point unk16;         /* 0x16 */
    Point unk1A;         /* 0x1A */
    u8 unk1E;            /* 0x1E */
    u8 pad1F;            /* 0x1F */
    u8 renderer_data[0x40]; /* original Vtx[4], deferred */
} Minos;

typedef struct {
    Cube *cube_list;
    u8 max_allowed;
    u8 num_active;
    u8 alpha;
    u8 pad07;
    Minos *minos_ptr;
} MobileCubes;

typedef struct {
    Cube **cube_ptr_list;
    u8 max_allowed;
    u8 num_active;
    u8 alpha;
    u8 pad07;
} FallingCubes;

typedef struct {
    u32 next_idx;
    u32 refill_idx;
    u32 queue[25];
    u8 pad6C[0x8];
    u16 bag_len;
    u8 bag[63];
    u32 seed;
} Bag63;

typedef struct {
    Point unk0[4];
    u8 unk10;
    u8 unk11;
    u16 unk12;
    u16 unk14;
} UnkStruct_17;

typedef struct {
    Point unk0;
    Point unk4;
    u8 unk8;
    u8 unk9;
    u16 unkA;
    u16 unkC;
} UnkStruct_13;

typedef struct {
    UnkStruct_13 unk0[4];
    UnkStruct_13 unk38[4];
} UnkStruct_18;

typedef struct {
    u8 piece_type;
    u8 starting_rot_state;
    Point physicalPos;
    Point unk6;
    u16 mino_scale;
    s16 unkC;
    s16 alpha_sl4b;
    s16 unk10;
    u8 unk12[0x6];
    Piece piece;
} NextPieceShadow;

typedef struct {
    u8 piece_type;
    u8 starting_rot_state;
    Point physicalPos;
    Point unk6;
    Point physicalRotOrigin;
    Point unkE;
    u16 mino_scale;
    s16 unk14;
    s16 alpha_sl4b;
    s16 unk18;
    u8 pad1A[0x6];
    Piece piece;
    NextPieceShadow shadow;
} NextPiece;

typedef struct {
    u8 state;
    u8 unk1;
    UnkStruct_18 unk2;
    u8 pad72[0x6];
    NextPiece buf[4];
    u8 buf_idx;
    u8 spawnDelay;
    u8 alpha;
    u8 pad291B;
    Bag63 *bag63_ptr;
} NextPieces;

typedef struct {
    CurrentPiece currentPiece;      /* 0x0000 */
    u8 padA6C[0x4];                /* 0x0A6C */
    NextPieces nextPieces;          /* 0x0A70 */
    MobileCubes mobileCubes;        /* 0x3390 */
    FallingCubes fallingCubes;      /* 0x339C */
    u8 next_cell_id;                /* 0x33A4 */
    u8 pad33A5[0x3];
    u8 pad33A8[0x6018 - 0x33A8];   /* BoardPieces remains separate */
} Board;

typedef struct {
    PieceDefinition *pieceDef_ptr;
    u8 board_width;
    u8 board_height;
} UnkStruct_16;

typedef struct {
    u8 unk0;
    u8 alpha;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    u16 mino_scale;
    u8 unkA[0x6];
    Piece piece;
} PieceHoldPieceShadow;

typedef struct {
    u8 state;
    u8 piece_type;
    u16 physicalCkwRotValue;
    s16 unk4;
    s16 mino_scale;
    s16 unk8;
    Point physicalRotOrigin;
    Point unkE;
    Point physicalPos;
    Point unk16;
    u8 unk1A;
    u8 pad1B[5];
    Piece piece;
    PieceColorFlasher flasher;
    PieceHoldPieceShadow shadow;
    u8 unkA40[0x8];
} PieceHoldPiece;

typedef struct {
    PieceHoldPiece buf[2];
    Point unk1490;
    u8 buf_idx;
    u8 can_swap;
    u8 num_anim_frames;
    u8 pad1497;
    UnkStruct_16 unk1498;
} PieceHold;

typedef struct {
    u8 data[0xB8];
} PfGfx;

typedef struct {
    s16 x;
    s16 y;
    u32 odd_bits;
    u32 even_bits;
    s32 linesToBeAdded;
    s32 linesMultiplier;
    s32 unk14;
} LinesInfo;

typedef struct {
    float pieceFallVelocityInitial;
    float pieceFallVelocityMax;
    float pieceFallAcceleration;
    s32 unkC;
    u32 elapsedTime;
    LinesInfo linesInfo;
} GameStats;

typedef struct {
    Point location;
    s16 unitSize;
} BoardInfo;

typedef struct {
    f32 alpha;
    f32 unk4;
    f32 unk8;
} UnkStruct_2;

typedef struct {
    void *image;
    Point image_loc;
    Point center_of_board;
    u16 unkC;
    s16 unkE;
    s16 unk10;
    s16 unk12;
    s16 unk14;
    s16 unk16;
} UnkStruct_7;

typedef struct UnkStruct_20 {
    u8 state;
    u8 unk1;
    u8 pad02[2];
    s32 unk4;
    s32 unk8;
    UnkStruct_2 unkC;
    u8 unk18;
    u8 pad19[3];
    s32 gf_type;
    UnkStruct_7 unk20;
    u8 unk38;
    u8 pad39[3];
} UnkStruct_20;

typedef struct {
    u8 state;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    PieceHold *pieceHold_ptr;
    s32 *unk8;
    u8 unkC;
    u8 pad0D[3];
    UnkStruct_20 unk10;
} FrameAct;

_Static_assert(sizeof(UnkStruct_2) == 0x0C, "UnkStruct_2 size");
_Static_assert(sizeof(UnkStruct_7) == 0x18, "UnkStruct_7 size");
_Static_assert(sizeof(UnkStruct_20) == 0x3C, "UnkStruct_20 size");
_Static_assert(sizeof(FrameAct) == 0x4C, "FrameAct size");
_Static_assert(sizeof(UnkStruct_19) == 0x14, "UnkStruct_19 size");
_Static_assert(sizeof(UnkStruct_36) == 0x09, "UnkStruct_36 size");
_Static_assert(sizeof(UnkStruct_37) == 0x06, "UnkStruct_37 size");
_Static_assert(sizeof(UnkStruct_9) == 0x68, "UnkStruct_9 size");
_Static_assert(sizeof(UnkStruct_16) == 0x8, "UnkStruct_16 size");
_Static_assert(sizeof(PieceHoldPieceShadow) == 0x508, "PieceHoldPieceShadow size");
_Static_assert(sizeof(PieceHoldPiece) == 0xA48, "PieceHoldPiece size");
_Static_assert(sizeof(PieceHold) == 0x14A0, "PieceHold size");

/* Original Tetris layout and offsets. */
typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 pad05[3];

    FrameAct frameAct;            /* 0x0008 */
    LineScan lineScan;            /* 0x0054 */
    Squares squares;              /* 0x044C */
    UnkStruct_9 unk768;           /* 0x0768 */
    Minos minos;                  /* 0x07D0 */
    Board board;                  /* 0x0830 */
    PieceHold pieceHold;          /* 0x51F0 */
    Bag63 bag63;                  /* 0x6690 */
    void *garbage_ptr;            /* 0x674C */
    PfGfx pfGfx;                  /* 0x6750 */
    GameStats gameStats;          /* 0x6808 */
    u32 ultraGoal;                /* 0x6834 */
    BoardInfo boardInfo;          /* 0x6838 */
    s32 unk6840;                  /* 0x6840 */
    u8 pad6844[4];
} Tetris;

extern Color g_color_black;

Point *getCurPieceSpawnPoint(Point *point_ptr, u8 pieceType);
Point *getStartingOriginOfRotationPoint(Point *point_ptr, u8 pieceType);
void MobilePiece_8006531c_fourliner_calls_fun_loops_4_times_v1(
    Piece *piece_ptr,
    Color *color_ptr
);

extern Tetris *g_tetris_ptr;
void TNT_Vita_TetrisSetGlobalPointers(Tetris *, u8);

extern u8 g_playercount;
extern u8 g_currentplayer;


/* Landfill / garbage compatibility. */
typedef struct {
    u8 unk0;
    u8 hole;
    u8 unk2;
    u8 pad03;
    u32 seed;
} UnkStruct_110;

typedef struct Garbage {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 pad07;
    s32 unk8;
    s32 unkC;
    u8 unk10;
    u8 pad11[3];
    UnkStruct_110 unk14;
    u8 unk1C;
    u8 unk1D;
    u8 unk1E[4];
    u8 pad22[2];
} Garbage;

typedef struct {
    s16 red, green, blue, alpha;
} TNTAimedAtRGBA;

typedef struct {
    u8 unk0;
    u8 unk1[4];
    u8 pad05;
    Point unk6;
    u8 unkA;
    u8 pad0B;
} UnkStruct_111;

typedef struct {
    UnkStruct_111 *list;
    u8 numIndicators;
    u8 pad05;
    TNTAimedAtRGBA color[4];
} UnkStruct_112;

typedef struct {
    u8 unk0;
    u8 unk1;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    u16 unk12;
} UnkStruct_29;

typedef struct {
    Garbage *list;
    u8 garbage_idx;
    u8 type;
    u8 pad06[2];
    UnkStruct_112 unk8;
    UnkStruct_29 unk30;
} Landfill;

#define LANDFILLTYPE_NONE      0
#define LANDFILLTYPE_HOTPOTATO 1
#define LANDFILLTYPE_DIRECTED  2

#define SFX_GAME_1  1
#define SFX_GAME_14 14
#define SFX_GAME_16 16
#define SFX_GAME_8  8
#define SFX_GAME_20 20
#define SFX_GAME_21 21

extern PlayerVars *g_PV_ptr;

extern u8 D_800CFEE8;
extern u8 D_800CFF50;
extern u8 D_800CFF00[];
extern u32 D_801109F4;

extern Landfill *g_landfill_ptr;
extern Garbage *g_garbage_ptr;
void func_80072300(u8);
u32 func_8007232C(void);
void Landfill_800723a0_calls_garbage_fun(u8, u8, u8);
void Landfill_8007240c_fiveliner(u8, u8);
void func_80072508(u8);
void func_8007257C(u8);
void func_800726A4(Landfill *, u8);
Garbage *Landfill_Garbage(void);
void Landfill_Init(Landfill *);
void Landfill_Deinit(Landfill *);
void func_80073690(UnkStruct_112 *, u8, u8, u8);
void func_800736E0(UnkStruct_112 *, u8, u8);
void func_80073784(UnkStruct_112 *, u8);
void func_80073940(UnkStruct_112 *);
void func_80073AC4(UnkStruct_112 *);
void HotPotato_Init(UnkStruct_29 *);
void HotPotato_Update(UnkStruct_29 *);
void HotPotato_Deinit(UnkStruct_29 *);

extern FrameAct *g_frameAct_ptr;
extern UnkStruct_9 *D_8011FBC0;
u8 KeySpin_8006cb10_switch_arg0_2_to_8_playsfx_35(UnkStruct_9 *, s32);
typedef struct CubeInfo_s CubeInfo;

extern Minos *g_minos_ptr;
extern MobileCubes *g_mobileCubes_ptr;
extern FallingCubes *g_fallingCubes_ptr;
extern u8 *g_next_cell_id_ptr;
extern CubeTiles *g_cubeTiles_ptr;

void CubeTiles_Init(CubeTiles *);
void CubeTiles_Deinit(CubeTiles *);
void TNT_Vita_CubeTilesInit(void);
void TNT_Vita_CubeTilesDeinit(void);
void TNT_Vita_MetalField_Init(void);
void TNT_Vita_MetalField_Update(u32);
void TNT_Vita_MetalField_Deinit(void);
void TNT_Vita_MinoTextures_Update(Mino *, Cube *);
void TNT_Vita_MinoTextures_UpdatePiece(Mino *, Piece *, u8, u16);

void TNT_Vita_MobilePieceGetMinoPointScaled(Piece *, u8, u16, Point *);

void MobileCubes_Init(s32, Minos *);
void MobileCubes_Deinit(void);
Cube *MobileCubes_Alloc(void);
void MobileCubes_Unalloc(Cube *);
void FallingCubes_Init(s32);
void FallingCubes_Deinit(void);
void FallingCubes_SetAlpha(FallingCubes *, u8);
Cube **FallingCubes_Alloc(void);
void Cube_Alloc(CubeInfo *, s32, s32, Color *);
void Cube_NewCellInfo(CubeInfo *);
void Cube_8006e8f8_sets_struct_0x14_to_0x24_to_mostly_zeros(
    CubeInfo *, s32, s32);


/* Opaque audio types for now. */
typedef struct { u8 data[1]; } SfxPlayer;
typedef struct { u8 data[1]; } SfxBank;

extern SfxPlayer D_801235B0;
extern SfxBank g_gameSfxBank;

/* Gameplay/update declarations used by tetris.c. */
void CurrentPiece_80067dd8_big_fiftyliner(u8);
void MobilePiece_80065238_sevenliner_loops_4_times(Piece *, Piece *);
void Minos_8007104c_fiveliner_nuts(Point *, s16, s16);
void Minos_80071100_fiveliner_nuts2(Point *, PointAngle *);

void FUN_033310_setPieceDef(UnkStruct_16 *, u8);
Point *FUN_033310_getPhysicalCurPieceSpawnPos(Point *, UnkStruct_16 *);
Point *FUN_033310_getPhysicalStartingRotOrigin(Point *, UnkStruct_16 *);
void FUN_033310_Init(UnkStruct_16 *, u8, u8);
void FUN_033310_Deinit(UnkStruct_16 *);

void PieceHold_Enable(PieceHold *);
void PieceHold_Disable(PieceHold *);

void PieceHold_Swap(PieceHold *, CurrentPiece *);
void Audio2_Play_SFX(SfxPlayer *, SfxBank *, s32);
void CurrentPiece_800676ac_fourliner_looper(CurrentPiece *);
void func_80072508(u8);
void func_8007257C(u8);
u8 func_80072F1C(s32);
void func_80072FC4(void);
void aiplayer_8004370c_threeliner(void);
void aiplayer_8004376c_fiveliner(void);

#ifndef GAMEFINISHTYPE_WIN
#define GAMEFINISHTYPE_WIN      0
#define GAMEFINISHTYPE_LOSE     1
#define GAMEFINISHTYPE_GAMEOVER 2
#define GAMEFINISHTYPE_NULL     3
#endif

u8 gamefinish_c_line_383(UnkStruct_20 *, u32);
void gamefinish_80052D48_twoliner(UnkStruct_20 *);
void gamefinish_80053280_twoliner(UnkStruct_20 *);
void gamefinish_something2(UnkStruct_20 *, s32, u8);
void gamefinish_80053390_fiveliner(UnkStruct_20 *, BoardInfo *);
void gamefinish_80053400_fiveliner(UnkStruct_20 *);
void gamefinish_800532DC_nevercalled(UnkStruct_20 *);

void FrameAct_ScheduleEvents(FrameAct *, u32);
void FrameAct_calls_gamefinish_something2(FrameAct *, s32);
void debug_print2(const char *, const char *);
void PieceHold_Update(PieceHold *, s32);
void Board_Update(Board *, s32);
void GameStats_Update(GameStats *, u32);
void FUN_800535B8_fiveliner(PlayerVars *);
void Tetris_FUN_80054220_fourliner(Tetris *, u8);
u8 Tetris_Update(Tetris *);
void FallingCubes_Update(void);
void MobileCubes_Update(void);
void Multisquares_UpdateGlows(Squares *, u32);
void MultisquareGlow_Init(SquareGlow *, Point, u8);
void MultisquareGlow_8006af00_eightliner(SquareGlow *, s32);
void TNT_Vita_RenderMultisquareGlows(void *, s32, s32, s32, s32, s32, s32, Squares *);

void GameCamera_Render(void);
void MobileCubes_Render(void);
void CurrentPiece_Render(CurrentPiece *);
void PieceHold_Render(PieceHold *);
void NextPieces_Render(void);
void GameStats_Render(GameStats *);
void Multisquares_RenderGlows(Squares *);
void FrameAct_Render(FrameAct *);

void LineScan_80069f10_threeliner_lowestscanableline(u8);
u8 BoardP_8006dd64_check_for_nonempty_cell_downwards(s32);
u8 BoardP_8006ddd0_check_for_nonempty_cell_upwards(s32);
void BoardP_8006de40_nineliner(s32, s32, u8);
void FrameAct_80053F40_oneliner(s32 *, s32);

u32 FUN_026900_PRNG_1(void);
u32 FUN_026900_PRNG_2(u32 seed);
u32 Bag63_GetNextPiece(Bag63 *);
extern NextPieces *g_nextPieces_ptr;
void NextPieces_Init(Bag63 *);
u8 CurrentPiece_800677b8_twohundredliner_doit_last(CurrentPiece *, s32);
void NextPieces_800688e0_sets_global_var_element(void);
void NextPieces_80068904_twoliner_if_fb80_eq_1_call_fun(void);
u8 NextPieces_80068b18_threeliner(void);
void LineScan_80069e84_sevenliner_loops_20_times(void);
void NextPieces_Deinit(void);

void Bag63_Init(Bag63 *, u32 *);
void Minos_Init(Minos *, UnkStruct_0 *);
void PFGFX_Playfield_Init(u8);
void Board_Init(Board *, Bag63 *, Minos *);
void PieceHold_Init(PieceHold *, u32 *, u8, u8, u8);
void Multisquare_8006ada4_Init(void);
void LineScan_Init(void);
u8 TNT_Vita_LineEffectCellHidden(LineScan *, Cell *);
void GameStats_Init(GameStats *);
void GameStats_saveCurrentPieceFallVelocity(GameStats *);
Point *PFGFX_Init(Point *, PfGfx *);
void BoardInfo_Init(BoardInfo *, Point, s16);
void FrameAct_Init(FrameAct *, PieceHold *, s32 *, BoardInfo *);

Garbage *Landfill_Garbage(void);
void Garbage_Init(Garbage *, u32 *);
void Garbage_Deinit(Garbage *);
void Garbage_80072e2c_set_arg0p_to_arg1(Garbage *, u8);
void func_80072CC8(Garbage *, u8, u8);
u8 func_80072D24(Garbage *, u8);
void func_80072E34(Garbage *, s32);
u8 func_80072F1C(s32);
void func_80072FC4(void);
void func_80073030(u32);
void Landfill_800723a0_calls_garbage_fun(u8, u8, u8);
void Landfill_8007240c_fiveliner(u8, u8);

void debug_print_reason_routine_linenum(const char *, const char *, s32);
void FrameAct_set_state(u8);

void FrameAct_Deinit_doesnothing(s32 *);
void FrameAct_Deinit(FrameAct *);
void PieceHold_Deinit(PieceHold *);
void Board_Deinit(Board *);
void Minos_Deinit_doesnothing(Minos *);
void Garbage_Deinit(Garbage *);
void Multisquare_Deinit_doesnothing(void);
void Bag63_Deinit(Bag63 *);
void GameStats_Deinit(GameStats *);


#define EMPTY_CELL 7
#define SFX_GAME_0 0
#define SFX_GAME_2 2
#define SFX_GAME_3 3
#define SFX_GAME_4 4
#define SFX_GAME_5 5
#define SFX_GAME_6 6
#define SFX_GAME_7 7
#define SFX_GAME_9 9
#define SFX_GAME_10 10
#define SFX_GAME_11 11
#define SFX_GAME_12 12
#define SFX_GAME_13 13

typedef struct {
    s32 sin;
    s32 cos;
} Trig;

typedef struct {
    Position *minoLockedPos_ptr;
    u8 pieceType;
    u8 numMinos;
} UnkStruct_4;

struct CubeInfo_s {
    u8 cell_id;
    Position pos;
    Cube *cube_ptr;
};

typedef struct Cell_s {
    u8 properties;
    u8 piece_type;
    u8 piece_id;
    u8 square_id;
    struct Cell_s *next;
    CubeInfo cubeInfo;
} Cell;


extern PieceDefinition *g_pieceDef_ptr_arr[7];
extern Color g_color_silver;
extern Color g_color_gold;
extern Trig g_trig_tbl[];
extern Color g_color_L;
extern Color g_color_J;
extern Color g_color_Z;
extern Color g_color_S;
extern Color g_color_T;
extern Color g_color_I;
extern Color g_color_O;
extern Color g_color_red;
extern Color g_color_white;
extern Color g_color_metal;

Point *getCurPieceSpawnOffsetAsPoint(Point *, PieceDefinition *);
Point *getStartingRotOriginAsPoint(Point *, PieceDefinition *);

void GhostPiece_800667dc_twelveliner_sets_struct(GhostPiece *, Point, Point, u16);
void GhostPiece_800666d0_sixliner_loops_arg1_times(GhostPiece *, s32);
void GhostPiece_800668ec_sixliner_inits_struct(GhostPiece *, PieceDefinition *, u8);
void GhostPiece_Render(GhostPiece *);

u8 MobilePiece_80065154_sevenliner_loops_4_times_v1_retbool(Piece *, s32);
u8 MobilePiece_800651c8_sevenliner_loops_4_times_v2_retbool(Piece *);
void MobilePiece_80065620_threeliner_if_zero_calls_fun_537c(PieceColorFlasher *);
void MobilePiece_8006567c_threeliner_if_notzero_calls_fun_537c(PieceColorFlasher *);
void MobilePiece_800655c4_oneliner_if_calls_fun(PieceColorFlasher *, s32);
void MobilePiece_80064f54_fourliner_loops_4_times(Piece *, s32);
void MobilePiece_800653e4_eightliner_sets_arg0_struct_to_other_args_loops_4_times(
    Piece *, PieceDefinition *, Point, Point, u16, u8);
void MobilePiece_8006553c_nineliner_sets_arg0_struct(PieceColorFlasher *, Piece *);
void MobilePiece_SetAlternateColor(PieceColorFlasher *, Color *);
void MobilePiece_Render(Piece *);

u8 FUN_003E40_8003dda8_twentyfiveliner_nested_loops(u8, s8, u8);
u8 FUN_003E40_8003dbc0_twentyliner_nested_loops(u8, s8, u8);

extern CurrentPiece *g_currentPiece_ptr;
u8 CurrentPiece_80066d8c_maybeRotCkw_retbool(CurrentPiece *, s32, s32, s32);

u8 FUN_800412e0_fifteenliner_logic_cell(u8 *);
u8 FUN_800413c0_twentyfive_liner(u8, u8, u8, u8);
u8 FUN_800416cc_fourliner(u8, u8, u8);
u8 FUN_80041810_twentyliner(u8, u8, u8);
u8 check_square_completion(u8, u8, u8 *, u8 *, u8 *);

void BoardP_8006df38_twentyfiveliner_3_loops(UnkStruct_4 *);
Color *BoardP_8006d3c0_fourliner(Cell *);
void BoardP_8006d5bc_sixliner_calls_UpdateQueue_AddEntry(Cell *);
void BoardP_8006d674_tenliner_alloc_fallingcubes(Cell *);
void BoardP_8006e288_adds_8_entries_and_swaps_cubes(Cell *);
u8 BoardP_8006d9d8_calls_pf_xy_to_logic_cell_loops(Cell *, s32, s32);
s32 BoardP_8006dcd0_eightliner_loops_upto_19_times_increment(void);
s32 BoardP_8006dd1c_eightliner_loops_upto_19_times_decrement(void);
void BoardPieces_SwapCubes(s32, s32, s32, s32);
Cell *BoardP_playfield_xy_to_logic_cell_struct_8006e5e0(s32, s32);

void FUN_026900_sets_arg0_struct_to_arg1_arg2(BoundingBox *, s32, s32);

void Minos_SetScale(u16);
void Minos_80070a34_twentyliner(void);
void Minos_80070c40_twoliner_set_OR_1(u16);
void Minos_80070c70_threeliner_set_OR_8(s16, s16);
void Minos_80070cb8_threeliner_set_OR_4(s16, s16);
void Minos_8007104c_fiveliner_nuts(Point *, s16, s16);
void Minos_80071100_fiveliner_nuts2(Point *, PointAngle *);
void Color_Blend(Color *, Color *, u32);


#define setPointAngle(pa, _x, _y, _angle) \
    (pa).p.x = (_x); \
    (pa).p.y = (_y); \
    (pa).angle = (_angle)

s16 FUN_026900_800608f4_oneliner_shift6_1(u16);
s16 FUN_026900_80060914_oneliner_shift6_2(u16);
u8 FUN_026900_80060770_sevenliner(UnkStruct_2 *, f32);

s8 CurrentPiece_80067bc0_sixliner_loops4times(CurrentPiece *);
s32 CurrentPiece_80067d1c_twoliner(CurrentPiece *);
u8 BoardP_IsPositionEmpty(s32, s32);
u8 BoardP_IsPointEmpty(s32, s32);

void Minos_800702e4_nineliner(Mino *);
void Minos_80070398_fourteenliner(Mino *, Mino *);
void Minos_80070528_Morph(Mino *);
void Minos_80070818_oneliner_sets_arg0_0x130_to_arg1(Mino *, CubeTile *);
void Minos_80070820_fiveliner_sets_arg0_2_4_8_12_to_arg1(Mino *, Color *);
void Minos_80070860_fortyliner(Mino *, Color *, s32);
void Minos_800709EC(Mino *, u8, s32);
void Minos_800709d8_threeliner_sets_arg0_1c_1e_1d(Mino *, u8);
void Minos_Mino_Render(Mino *, UnkStruct_8 *);
void Minos_SetAlpha(u8);
void Minos_BeginRender(Minos *);
void Minos_EndRender(Minos *);
void Minos_SetVtxTransform(u8);

void NextPieces_80068b7c_largeliner_sets_lots_of_struct_elems(UnkStruct_17 *);
void PFGFX_VitaInit1PLayout(u8);
void PFGFX_VitaInit2PLayout(u8);
void PFGFX_VitaInit3PLayout(u8);
void PFGFX_VitaInit4PLayout(u8);

#endif

typedef struct {
    u8 cnt;
} Id;

typedef struct {
    Id *list;
    u32 max_ids;
    u32 next_id;
} Ids;

/* Original AI placement/search structures used by attract/demo mode. */
typedef struct {
    u8 unk0;  /* x */
    s8 unk1;  /* y */
    u8 unk2;  /* rot */
} UnkStruct_108;

typedef struct {
    UnkStruct_108 unk0;
    u8 unk3;
} UnkStruct_98;

typedef struct {
    s8 unk0;
    UnkStruct_98 unk1[10];
} UnkStruct_105;

typedef struct {
    u8 unk0;  /* x */
    u8 unk1;  /* y */
    u8 unk2;
} UnkStruct_106;

typedef struct {
    u8 unk0;
    s8 unk1[6];
    UnkStruct_106 unk7[6];
} UnkStruct_107;

typedef struct {
    u8 unk0[2];
    UnkStruct_108 unk2[2][400];
} UnkStruct_109;

typedef struct {
    s16 unk0;
    UnkStruct_108 unk2;
    u8 unk5;
    u8 unk6;
} UnkStruct_99;

typedef struct {
    u8 player_idx;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    UnkStruct_105 unk5;
    UnkStruct_107 unk2E;
} UnkStruct_85;

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
} UnkStruct_27;

typedef struct {
    s8 unk0[4];
    s8 unk4[4];
} UnkStruct_28;

typedef struct {
    UnkStruct_27 unk0[200];
    Ids unk258;
} UnkStruct_6;

typedef void *Entry;

typedef struct {
    Entry entry;
    u8 type;
    u8 pad05[3];
} TypedEntry;

typedef struct {
    TypedEntry entries[600];
    u16 maxlen;
    u16 curlen;
} TypedQueue;

typedef struct {
    Cell *cell_list;
    Cell *cell_ptr_arr[200];
    Cell cell_200;
    Cell cell_201;
    TypedQueue updQ;
    Ids pieceIds;
    u16 num_cells;
} BoardPieces_Min;

extern BoardPieces_Min *g_boardPieces_ptr;
extern GameStats *g_gameStats_ptr;
extern f32 g_pieceFallAcceleration;

u32 getLineCount(LinesInfo *);
void updateLinesToBeAdded(LinesInfo *, s32);

void Ids_Init(Ids *, u32);
void Ids_Deinit(Ids *);
u32 can_run_out_of_ids(Ids *);
void FUN_80050800_oneliner(Ids *, u32);
void FUN_80050820_oneliner(Ids *, u32, u32);
void FUN_80050840_fiveliner(Ids *, u32);
void FUN_800508a8_fiveliner(Ids *, u32, u32);
void FUN_800508ec_fifteen_liner_allocs_heap(Ids *, Ids *);

/* LineEffect interface used by original LineScan. */
u8 LineEffect_Update(LineEffect *, u8);
void LineEffect_80069668_fourliner_sets_arg0_struct_v1(LineEffect *);
void LineEffect_Init(LineEffect *, s32, u8);

/* Original multisquare types used by LineScan scoring. */
#define SQUARETYPE_MULTI 1
#define SQUARETYPE_MONO  2

s32 Multisquare_8006ac2c_loops_10_times(s32, s32);
void func_80073030(u32);
void func_80072300(u8);

/* Original game SFX IDs referenced by LineScan. */
#ifndef SFX_GAME_15
#define SFX_GAME_15 15
#endif
#ifndef SFX_GAME_17
#define SFX_GAME_17 17
#endif
#ifndef SFX_GAME_18
#define SFX_GAME_18 18
#endif
#ifndef SFX_GAME_19
#define SFX_GAME_19 19
#endif

extern LineScan *g_lineScan_ptr;
void LineScan_Init(void);
void LineScan_80069ec0_sevenliner_loops_4_times(void);
void LineScan_80069fb8_elevenliner_case_and_loop_interesting(void);

void GameStats_Update(GameStats *, u32);

extern Squares *g_squares_ptr;
void Multisquare_8006ada4_Init(void);

#ifndef SQUARETYPE_NONE
#define SQUARETYPE_NONE 0

u8 FUN_006B30_800408b0_fiveliner_loops_4t(u8, u8, u8);
u8 FUN_800416cc_fourliner(u8, u8, u8);
u8 FUN_8004172c_twentyliner(s8, s8, s8);
void aisquarelist_8003e1e0_sevenliner_loops_6t(UnkStruct_107 *);
void aisquarelist_8003e22c_tenliner_loops_6t(UnkStruct_107 *, u8, u8);
void aisquarelist_8003e2c8_sevenliner(UnkStruct_107 *, u8);
u8 aisquarelist_8003e458_thirteenliner_loops_7t(UnkStruct_107 *, u8 *, u8 *);
u8 aisquarelist_8003e55c_eightliner_loops_arg0_t_1(UnkStruct_107 *);
void aisquarelist_8003e608_eightliner_loops_arg0_t_2(UnkStruct_107 *, u8);
u8 aisquarelist_8003ed40_largeliner_many_loops(UnkStruct_107 *, u8, u8 *, u8, u8);
s32 func_800A35EC(s32, s32);
extern PlayerVars g_PV_arr[4];
extern u32 D_801109F4;
void CurrentPiece_8006715c_twelveliner_plays_SFX(CurrentPiece *);
void CurrentPiece_80067254_twelveliner_plays_SFX(CurrentPiece *);
void CurrentPiece_8006734c_thirtyliner_looks_interesting(CurrentPiece *, s8);
void CurrentPiece_800676ac_fourliner_looper(CurrentPiece *);
void CurrentPiece_80067dd8_big_fiftyliner(u8);
void FUN_80041260_twoliner(void);
void FUN_8004129c_fourliner(void);
/* Attract/demo AI. */
extern UnkStruct_109 *D_800E1F20;
extern UnkStruct_6 *D_800E1F30;
extern UnkStruct_99 **D_800E1F40;
extern UnkStruct_85 *D_800C5E04;
extern u8 D_800E1F50;
extern UnkStruct_28 D_800C5CF0[7][4];
extern u8 D_800C55B0[5];
void aiplayer_gameinit_related(u8, u8, u8);
void aiplayer_80042b3c_calls_heap_unalloc(void);
void aiplayer_8004311c_largefunction(void);
void aiplayer_8004370c_threeliner(void);
void aiplayer_8004376c_fiveliner(void);
void FUN_004220_8003dfa0_sixliner_allocs_heap_x962(void);
void FUN_004220_8003e004_threeliner_unallocs_heap(void);
void FUN_004220_8003e038_twentytwoliner_nested_loops(u8, u8);
u8 FUN_005580_8003f300_fourteenliner_recursive(s8 [4][4], u8, u8, u8, u8);
u8 FUN_005580_8003f4f4_thirtyliner_nested_loops(s8 [4][4], u8, u8, u8);
void FUN_005580_8003f760_twoliner_calls_fun(UnkStruct_105 *, u8, s8, u8, u8);
u8 FUN_005580_800400fc_largefun_more_currpiece_stuff(UnkStruct_105 *, u8);
u8 FUN_006780_80040500_twentyfiveliner_loops(u8, u8);
void FUN_006930_800406b0_fifteenliner_two_loops(u8, u8 *, u8 *);
void FUN_006930_80040784_nested_loop_20t_inside_loop_10t(u8 *, u8 *, u8 *, u8 *);
void FUN_006BB0_80040f9c_twentyfiveliner_loops_200t(u8, u8, s8);
void FUN_008050_80041dd0_alloc_heap_4_and_x640(void);
void FUN_008050_80041e24_maybe_calls_unalloc_heap(void);
void FUN_008050_80041e88_loops_200_times(void);
void FUN_008050_800423f0_uses_dat_800e1f40_ptr(u8);
void FUN_008050_800427b8_passes_dat_800e1f40_ptr(u8, UnkStruct_108 *);
u8 FUN_8004191c_sixliner_loop_shift_right(u8, u8);
u8 FUN_8004199c_twentyliner_loop(u8, u8 *, u8, u8);
u8 FUN_80041ab4_tenliner_loop(u8, u8);
void FUN_80041da0_threeliner(UnkStruct_108 *, UnkStruct_108 *);
#endif

u8 Multisquare_isArg0_lessthan_44(u32);

void CurrentPiece_80067c1c_fifteenliner_loops3times(
    CurrentPiece *,
    BoundingBox *
);

void BoardP_8006d450_fourteenliner_pf_logic_cell_loops(
    Cell *,
    Cell *
);
void BoardP_8006e64c_calls_UpdateQueue_AddEntry(s32, s32);
void BoardPieces_Update(BoardPieces_Min *, s32);
void TNT_BoardPiecesMin_Init(void);
void TNT_BoardPiecesMin_Deinit(void);

u8 Multisquare_8006ad0c_checks_1_2_3_4(void);

s32 TNT_Vita_CountFilled4x4(void);

s32 TNT_Vita_Filled4x4Status(void);

extern s32 g_tnt_vita_last_square_probe;

void Multisquare_8006a410_fourliner(Square *, u8 *);
Color *Multisquare_pGetColor(Square *);

extern u32 g_vita_lines_before;
extern s32 g_vita_lines_added;
extern u32 g_vita_lines_after;
