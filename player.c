#include "player.h"
#include "text.h"
#include "resources.h"
#include "config.h"
#include "clothManager.h"
#include "line.h"
#include "stdio.h"
#include "title.h"
#include "day.h"
#include "renderer.h"
#include "animation.h"

#include <time.h> 

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
        }
    } else {
        if (hand->x < max) {
            hand->x++;
        } else {
            hand->x = 0;
        }
    }
}

static timer_link_t* clothRequestTimer;

static void handleClothRequest() {
    player.hands[HAND_HANG].cloth = dequeueCloth();

    delete_timer(clothRequestTimer);
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

static void animateDropped(u32 x, u32 y) {
    Animation* anim = newAnimation(8);

    x = LINES_MARGIN_LEFT + (TILE_WIDTH * x);
    y = (player.hands[HAND_TAKE].y
                ? INSIDE_LINE_POSITION
                : OUTSIDE_LINE_POSITION
            ) + 4;

    for (u32 i = 0; i < 8; i++) {
        setSimpleFrame(&anim->frames[i], GROWTH_0_SPRITE, x, y + i, 0.1);
        anim->frames[i].remainingCycles = 2;
    }

    startAnimation(anim);
}

static void handleDrop() {
    Hand* hand = &player.hands[HAND_TAKE];
    // If hand is empty, first grab whatever cloth we're sitting on.
    if (!hand->cloth) {
        hand->cloth = takeCloth(hand->y, hand->x);
    }
    // If nothing to drop, we're done.
    if (!hand->cloth) {
        return;
    }

    hand->cloth->isFreeable = true;
    hand->cloth = 0;
    player.dropped++;
    animateDropped(hand->x, hand->y);
}

static void animateScore(u32 score, u32 x, u32 y) {
    Animation* anim = newAnimation(8);

    x = LINES_MARGIN_LEFT + (TILE_WIDTH * x);
    y = (player.hands[HAND_TAKE].y
                ? INSIDE_LINE_POSITION
                : OUTSIDE_LINE_POSITION
            ) + 4;

    for (u32 i = 0; i < 8; i++) {
        setSimpleFrame(&anim->frames[i], PLUS_ONE_SPRITE + score - 1, x, y - i, 0.1);
        anim->frames[i].remainingCycles = 1;
    }

    startAnimation(anim);
}

static void handleTake() {
    Hand* hand = &player.hands[HAND_TAKE];

    // If we're already holding something, put it back on the line.
    if (hand->cloth) {
        if (hangCloth(hand->y, hand->x, hand->cloth)) {
            hand->cloth = 0;
        }
        return;
    }

    Cloth* taken = takeCloth(hand->y, hand->x);
    // Not able to take a cloth.
    if (!taken) {
        // Ideally play a NOPE sound.
        return;
    }

    if (isClothDry(taken)) {
        // If it's dry, chuck it in the basket.
        u32 clothScore = calculateScore(taken);
        player.score += clothScore;
        taken->isFreeable = true;

        animateScore(clothScore, hand->x, hand->y);
    } else if (taken->dryingState != DRYING_DIRTY) {
        // If it's not, take it to be hung somewhere else.
        hand->cloth = taken;
    } else {
        // It's dirty, we're done with it.  Mark it for cleanup.
        player.dropped++;
        taken->isFreeable = true;
        animateDropped(hand->x, hand->y);
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
    player.hands[HAND_TAKE].x = OUTSIDE_LINE_SIZE -1;
    player.hands[HAND_TAKE].y = 0;
    player.hands[HAND_HANG].cloth = 0;
    player.hands[HAND_TAKE].cloth = 0;
    player.score = 0;
    player.dropped = 0;
    player.state = STATE_TITLE;
}

void gameOver() {
    player.state = STATE_GAMEOVER;
    cancelDayTimers();
}

void drawPlayer() {
    if (player.hands[HAND_HANG].cloth) {
        drawCloth(
            player.hands[HAND_HANG].cloth,
            LINES_MARGIN_LEFT + (TILE_WIDTH * player.hands[HAND_HANG].x),
            (player.hands[HAND_HANG].y
                ? INSIDE_LINE_POSITION
                : OUTSIDE_LINE_POSITION
            ) + HELD_OFFSET
        );
    }

    // Hanger selector
    drawSprite(
        HANG_SPRITE,
        LINES_MARGIN_LEFT + (TILE_WIDTH * player.hands[HAND_HANG].x),
        (player.hands[HAND_HANG].y
            ? INSIDE_LINE_POSITION
            : OUTSIDE_LINE_POSITION
        ) + HELD_OFFSET + (TILE_WIDTH * 1.8)
        , 3, 1
    );

    if (player.hands[HAND_TAKE].cloth) {
        drawCloth(
            player.hands[HAND_TAKE].cloth,
            LINES_MARGIN_LEFT + (TILE_WIDTH * player.hands[HAND_TAKE].x),
            (player.hands[HAND_TAKE].y
                ? INSIDE_LINE_POSITION
                : OUTSIDE_LINE_POSITION
            ) + HELD_OFFSET
        );

        // Cloth selector
        drawSprite(
            TAKE_SPRITE,
            LINES_MARGIN_LEFT + (TILE_WIDTH * player.hands[HAND_TAKE].x),
            (player.hands[HAND_TAKE].y
                ? INSIDE_LINE_POSITION
                : OUTSIDE_LINE_POSITION
            ) + HELD_OFFSET + TILE_WIDTH * 1.8
            , 3, 1
        );
    } else {
        // Cloth selector
        drawSprite(
            TAKE_SPRITE,
            LINES_MARGIN_LEFT + (TILE_WIDTH * player.hands[HAND_TAKE].x),
            (player.hands[HAND_TAKE].y
                ? INSIDE_LINE_POSITION
                : OUTSIDE_LINE_POSITION
            ) + TILE_WIDTH * 1.8
            , 3, 1
        );
    }

    drawText(
        "SCORE",
        LEFT_MARGIN,
        TOP_MARGIN + SCREEN_HEIGHT - (TILE_WIDTH * 3),
        1
    );

    string score = "";
    sprintf(score, "%lu", player.score);
    drawText(
        score,
        LEFT_MARGIN,
        TOP_MARGIN + SCREEN_HEIGHT - (TILE_WIDTH * 2.5),
        1.1
    );
}

/**
 * 
 * @returns true if action was taken, false otherwise.
 */
bool handleController(N64ControllerState* pressed, N64ControllerState* released) {
    if (released->c[0].start) {
        switch (player.state) {
            case STATE_TITLE:
                srand(timer_ticks());
                player.state = STATE_PLAY;
                closeTitle();
                startFirstDay();
                player.hands[HAND_HANG].cloth = dequeueCloth();
                break;
            case STATE_PLAY:
                player.state = STATE_PAUSE;
                break;
            case STATE_PAUSE:
                player.state = STATE_PLAY;
                break;
            case STATE_GAMEOVER:
                player.score = 0;
                player.dropped = 0;
                player.state = STATE_PLAY;
                break;
            default:
                player.state = STATE_ERROR;
                return false;
        }

        return true;
    }

    if (player.state == STATE_PAUSE) {
        return false;
    }

    if (!isWaiting()) {
        if (released->c[0].L) {
            handleHang();
            return true;
        }

        // Discard whatever you're holding in your right hand so you can pick up something else.
        if (released->c[0].A || released->c[0].B) {
            handleDrop();
            return true;
        }

        if (released->c[0].R) {
            handleTake();
            return true;
        }
    }

    bool result = false;

    if (released->c[0].up || released->c[0].down) {
        handleMoveY(&player.hands[HAND_HANG]);
        result = true;
    }
    if (released->c[0].C_up || released->c[0].C_down) {
        handleMoveY(&player.hands[HAND_TAKE]);
        result = true;
    }

    if (released->c[0].left || released->c[0].right) {
        handleMoveX(&player.hands[HAND_HANG], released->c[0].left);
        result = true;
    }

    if (released->c[0].C_left || released->c[0].C_right) {
        handleMoveX(&player.hands[HAND_TAKE], released->c[0].C_left);
        result = true;
    }

    return result;
}

Player* getPlayer() {
    return &player;
}