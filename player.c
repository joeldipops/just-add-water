#include "player.h"
#include "text.h"
#include "resources.h"
#include "config.h"
#include "clothManager.h"
#include "line.h"
#include "stdio.h"

static Player player;

static void handleMoveX(Hand* hand, bool isLeft) {
    u32 max = hand->y 
        ? INSIDE_LINE_SIZE - 1 
        : OUTSIDE_LINE_SIZE - 1
    ;

    if (isLeft) {
        if (hand->x > 0) {
            hand->x--;
        } else {
            hand->x = max;
            ;
        }
    } else {
        if (hand->x < max) {
            hand->x++;
        } else {
            hand->x = 0;
        }
    }
}

static void handleHang() {
    Hand* hand = &player.hands[HAND_HANG];
    Cloth* cloth = hand->cloth;
    if (!cloth) {
        hand->cloth = dequeueCloth();
    }
    // If nothing to dequeue, ignore the press.
    if (!cloth) {
        return;
    }

    if (!hangCloth(hand->y, hand->x, cloth)) {
        // Ideally play a NOPE sound.
        return;
    }

    hand->cloth = dequeueCloth();
}

static void handleTake() {
    Hand* hand = &player.hands[HAND_TAKE];
    Cloth* taken = takeCloth(hand->y, hand->x);
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

static void handleMoveY(Hand* hand) {
    hand->y = !hand->y;

    if (hand->y && hand->x >= INSIDE_LINE_SIZE) {
        hand->x = INSIDE_LINE_SIZE - 1;
    } else if (!hand->y && hand->x >= OUTSIDE_LINE_SIZE) {
        hand->x = OUTSIDE_LINE_SIZE - 1;
    }
}

void initPlayer() {
    player.hands[HAND_HANG].x = 0;
    player.hands[HAND_HANG].y = 0;
    player.hands[HAND_TAKE].x = 0;
    player.hands[HAND_TAKE].y = 0;
    player.score = 0;
    player.state= STATE_PLAY;
}

void gameOver() {
    player.state = STATE_GAMEOVER;
}

void drawPlayer() {
    // Hanger selector
    drawSprite(
        HANG_SPRITE,
        LEFT_MARGIN + (TILE_WIDTH * player.hands[HAND_HANG].x),
        player.hands[HAND_HANG].y
            ? INSIDE_LINE_POSITION
            : OUTSIDE_LINE_POSITION
        , 1
    );

    // Cloth selector
    drawSprite(
        TAKE_SPRITE,
        LEFT_MARGIN + (TILE_WIDTH * player.hands[HAND_TAKE].x),
        player.hands[HAND_TAKE].y
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
        player.state = STATE_PAUSE ? STATE_PLAY : STATE_PAUSE;
        return true;
    }

    if (player.state == STATE_PAUSE) {
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
        handleMoveY(&player.hands[HAND_HANG]);
        result = true;
    }
    if (pressed->c[0].C_up || pressed->c[0].C_down) {
        handleMoveY(&player.hands[HAND_TAKE]);
        result = true;
    }

    if (pressed->c[0].left || pressed->c[0].right) {
        handleMoveX(&player.hands[HAND_HANG], pressed->c[0].left);
        result = true;
    }

    if (pressed->c[0].C_left || pressed->c[0].C_right) {
        handleMoveX(&player.hands[HAND_TAKE], pressed->c[0].C_left);
        result = true;
    }

    return result;
}

Player* getPlayer() {
    return &player;
}