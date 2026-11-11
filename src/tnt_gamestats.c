#include "vita_compat.h"
#include "tnt_game_types.h"

GameStats *g_gameStats_ptr;
f32 g_pieceFallAcceleration = 0.000003f;

u32 g_vita_lines_before = 0;
s32 g_vita_lines_added = 0;
u32 g_vita_lines_after = 0;

static void initLinesInfo(LinesInfo *linesInfo_ptr) {
    linesInfo_ptr->odd_bits = 0;
    linesInfo_ptr->even_bits = 0;
    linesInfo_ptr->linesToBeAdded = 0;
    linesInfo_ptr->linesMultiplier = 1;
    linesInfo_ptr->unk14 = 0;
}

static void setLineCount(LinesInfo *linesInfo_ptr, u32 lines) {
    linesInfo_ptr->odd_bits = lines & 0xAAAAAAAA;
    linesInfo_ptr->even_bits = lines & 0x55555555;
}

u32 getLineCount(LinesInfo *linesInfo_ptr) {
    return linesInfo_ptr->odd_bits | linesInfo_ptr->even_bits;
}

void updateLinesToBeAdded(LinesInfo *linesInfo_ptr, s32 lines) {
    linesInfo_ptr->linesToBeAdded += lines;
}

void GameStats_saveCurrentPieceFallVelocity(GameStats *gameStats_ptr) {
    f32 velocity;

    velocity =
        gameStats_ptr->pieceFallVelocityInitial +
        (gameStats_ptr->elapsedTime *
         gameStats_ptr->pieceFallAcceleration);

    if (velocity > gameStats_ptr->pieceFallVelocityMax) {
        velocity = gameStats_ptr->pieceFallVelocityMax;
    }

    g_currentPiece_ptr->fallVelocityCopy =
        (s32)((velocity * 256.0f) + 0.5f);
}

void GameStats_Update(GameStats *gameStats_ptr, u32 arg1) {
    if (gameStats_ptr->linesInfo.linesToBeAdded != 0) {
        g_vita_lines_before =
            getLineCount(&gameStats_ptr->linesInfo);

        g_vita_lines_added =
            gameStats_ptr->linesInfo.linesToBeAdded *
            gameStats_ptr->linesInfo.linesMultiplier;

        setLineCount(
            &gameStats_ptr->linesInfo,
            g_vita_lines_before + g_vita_lines_added
        );

        g_vita_lines_after =
            getLineCount(&gameStats_ptr->linesInfo);

        gameStats_ptr->linesInfo.linesMultiplier = 1;
        gameStats_ptr->linesInfo.linesToBeAdded = 0;
    }

    gameStats_ptr->elapsedTime += arg1;
}

void GameStats_Init(GameStats *gameStats_ptr) {
    initLinesInfo(&gameStats_ptr->linesInfo);

    gameStats_ptr->pieceFallVelocityInitial = 0.01f;
    gameStats_ptr->pieceFallVelocityMax = 1.0f;
    gameStats_ptr->pieceFallAcceleration = g_pieceFallAcceleration;
    gameStats_ptr->elapsedTime = 0;
}

void GameStats_Deinit(GameStats *gameStats_ptr) {
    (void)gameStats_ptr;
}

void GameStats_Render(GameStats *gameStats_ptr) {
    (void)gameStats_ptr;
}
