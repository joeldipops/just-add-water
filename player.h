#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED

#include "core.h"
#include "cloth.h"

typedef struct {
    u32 hangX;
    u32 hangY;

    u32 takeX;
    u32 takeY;

    bool isPaused;

    Cloth* clothToHang;

    u32 score;
} Player;

void gameOver();

void initPlayer();

Player* getPlayer();

void drawPlayer();

bool handleController(N64ControllerState* pressed, N64ControllerState* released);

#endif