#include "player.h"
#include "config.h"

static Player player;

static void handleHangX(bool isLeft) {
    u32 max = player.hangY 
        ? INSIDE_LINE_SIZE - 1 
        : OUTSIDE_LINE_SIZE - 1
    ;

    if (isLeft) {
        if (player.hangX > 0) {
            player.hangX--;
        } else {
            player.hangX = max;
            ;
        }
    } else {
        if (player.hangX < max) {
            player.hangX++;
        } else {
            player.hangX = 0;
        }
    }
}

static void handleTakeX(bool isLeft) {
    u32 max = player.takeY 
        ? INSIDE_LINE_SIZE - 1 
        : OUTSIDE_LINE_SIZE - 1
    ;

    if (isLeft) {
        if (player.takeX > 0) {
            player.takeX--;
        } else {
            player.takeX = max;
            ;
        }
    } else {
        if (player.takeX < max) {
            player.takeX++;
        } else {
            player.takeX = 0;
        }
    }
}

static void handleHang() {}
static void handleTake() {}

void initPlayer() {
    player.hangX = 0;
    player.hangY = 0;
    player.takeX = 0;
    player.takeY = 0;
    player.isPaused = false;
}

void handleController(N64ControllerState* pressed, N64ControllerState* released) {
    if (released->c[0].start) {
        player.isPaused = !player.isPaused;
        return;
    }
    if (released->c[0].L) {
        handleHang();
        return;
    }
    if (released->c[0].R) {
        handleTake();
        return;
    }

    if (pressed->c[0].up || pressed->c[0].down) {
        player.hangY = !player.hangY;
    }
    if (pressed->c[0].C_up || pressed->c[0].C_down) {
        player.takeY = !player.takeY;
    }

    if (pressed->c[0].left || pressed->c[0].right) {
        handleHangX(pressed->c[0].left);
    }

    if (pressed->c[0].C_left || pressed->c[0].C_right) {
        handleTakeX(pressed->c[0].C_left);
    }
}

Player* getPlayer() {
    return &player;
}