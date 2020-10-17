#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED

#include "core.h"

typedef struct {
    u32 hangX;
    u32 hangY;

    u32 takeX;
    u32 takeY;

    bool isPaused;
} Player;

void initPlayer();

Player* getPlayer();

void handleController(N64ControllerState* pressed, N64ControllerState* released);

#endif