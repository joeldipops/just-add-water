#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED

#include "core.h"
#include "cloth.h"


typedef struct {
    s32 x;
    s32 y;
    Cloth* cloth;
} Hand;

typedef enum {
    HAND_HANG = 0,
    HAND_TAKE = 1
} HandType;

typedef struct {
    Hand hands[2];

    State state;

    s32 score;
    s32 dropped;
} Player;

void gameOver();

void initPlayer();

Player* getPlayer();

void drawPlayer();

bool handleController(N64ControllerState* pressed, N64ControllerState* released);

#endif