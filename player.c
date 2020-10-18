#include "player.h"
#include "text.h"
#include "resources.h"
#include "config.h"
#include "clothManager.h"
#include "line.h"
#include "stdio.h"

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

static void handleHang() {
    if (!hangCloth(player.hangY, player.hangX, player.clothToHang)) {
        // Ideally play a NOPE sound.
        return;
    }

    player.clothToHang = dequeueCloth();
}

static void handleTake() {
    if (!takeCloth(player.takeY, player.takeX)) {
        // Ideally play a NOPE sound.
        return;
    }

    player.score++;
}

void initPlayer() {
    player.hangX = 0;
    player.hangY = 0;
    player.takeX = 0;
    player.takeY = 0;
    player.score = 0;
    player.isPaused = false;
}

void drawPlayer() {
    // Hanger selector
    drawSprite(
        HANG_SPRITE,
        LEFT_MARGIN + (TILE_WIDTH * player.hangX),
        player.hangY 
            ? INSIDE_LINE_POSITION
            : OUTSIDE_LINE_POSITION
        , 1
    );

    // Cloth selector
    drawSprite(
        TAKE_SPRITE,
        LEFT_MARGIN + (TILE_WIDTH * player.takeX),
        player.takeY 
            ? INSIDE_LINE_POSITION
            : OUTSIDE_LINE_POSITION
        , 1
    );

    // Scoreboard
    drawText(
        "SCORE",
        SCREEN_WIDTH - STANDARD_MARGIN - TILE_WIDTH * 5,
        STANDARD_MARGIN,
        1
    );

    string score = "";
    sprintf(score, "%lu", player.score);
    drawText(
        score,
        SCREEN_WIDTH - STANDARD_MARGIN - TILE_WIDTH * 5,
        STANDARD_MARGIN * 2,
        1
    );
}

void handleController(N64ControllerState* pressed, N64ControllerState* released) {
    if (released->c[0].start) {
        player.isPaused = !player.isPaused;
        return;
    }

    if (player.isPaused) {
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