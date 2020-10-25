#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED

#include "core.h"
#include "cloth.h"


typedef struct {
    u32 x;
    u32 y;
    Cloth* cloth;
} Hand;

typedef enum {
    HAND_HANG = 0,
    HAND_TAKE = 1
} HandType;

typedef enum {
    STATE_TITLE,
    STATE_PLAY,
    STATE_PAUSE,
    STATE_GAMEOVER
} State;

typedef struct {
    Hand hands[2];

    State state;

    Cloth* clothToHang;

    u32 score;
    u32 dropped;
} Player;

void gameOver();

void initPlayer();

Player* getPlayer();

void drawPlayer();

bool handleController(N64ControllerState* pressed, N64ControllerState* released);

#endif