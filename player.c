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
    if (!player.clothToHang) {
        player.clothToHang = dequeueCloth();
    }
    // If nothing to dequeue, ignore the press.
    if (!player.clothToHang) {
        return;
    }

    if (!hangCloth(player.hangY, player.hangX, player.clothToHang)) {
        // Ideally play a NOPE sound.
        return;
    }

    player.clothToHang = dequeueCloth();
}

static void handleTake() {
    Cloth* taken = takeCloth(player.takeY, player.takeX);
    // Not able to take a cloth.
    if (!taken) {
        // Ideally play a NOPE sound.
        return;
    }

    if (isClothDry(taken)) {
        player.score++;
        taken->isFreeable = true;
    } else if (taken->dryingState != DRYING_DIRTY) {
        // TODO - allow moving
        ; 
    } else {
        // It's dirty, we're done with it.  Mark it for cleanup.
        taken->isFreeable = true;
    }
}

static void handleHangY() {
    player.hangY = !player.hangY;

    if (player.hangY && player.hangX >= INSIDE_LINE_SIZE) {
        player.hangX = INSIDE_LINE_SIZE - 1;
    }

    if (!player.hangY && player.hangX >= OUTSIDE_LINE_SIZE) {
        player.hangX = OUTSIDE_LINE_SIZE - 1;
    }
}
static void handleTakeY() {
    player.takeY = !player.takeY;

    if (player.takeY && player.takeX >= INSIDE_LINE_SIZE) {
        player.takeX = INSIDE_LINE_SIZE - 1;
    }

    if (!player.takeY && player.takeX >= OUTSIDE_LINE_SIZE) {
        player.takeX = OUTSIDE_LINE_SIZE - 1;
    }
}


void initPlayer() {
    player.hangX = 0;
    player.hangY = 0;
    player.takeX = 0;
    player.takeY = 0;
    player.score = 0;
    player.isPaused = false;
}

void gameOver() {
    player.isPaused = true;
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

/**
 * 
 * @returns true if action was taken, false otherwise.
 */
bool handleController(N64ControllerState* pressed, N64ControllerState* released) {
    if (released->c[0].start) {
        player.isPaused = !player.isPaused;
        return true;
    }

    if (player.isPaused) {
        return false;
    }

    if (released->c[0].L) {
        handleHang();
        return true;
    }
    if (released->c[0].R) {
        handleTake();
        return true;
    }

    bool result = false;

    if (pressed->c[0].up || pressed->c[0].down) {
        handleHangY();
        result = true;
    }
    if (pressed->c[0].C_up || pressed->c[0].C_down) {
        handleTakeY();
        result = true;
    }

    if (pressed->c[0].left || pressed->c[0].right) {
        handleHangX(pressed->c[0].left);
        result = true;
    }

    if (pressed->c[0].C_left || pressed->c[0].C_right) {
        handleTakeX(pressed->c[0].C_left);
        result = true;
    }

    return result;
}

Player* getPlayer() {
    return &player;
}