#include "day.h"
#include "core.h"
#include "config.h"
#include "text.h"
#include "weather.h"
#include "player.h"
#include "line.h"
#include "clothManager.h"
#include "renderer.h"

#include <stdbool.h>
#include <stdio.h>
#include <libdragon.h>

static u32 secondsLeft = 0;
timer_link_t* secondsTimer = 0;

static u32 secondsPerDay = INIT_TURN_SECONDS;
static u32 clothsPerDay = INIT_TURN_CLOTHS;

void initDay() {

}

static bool _waiting = false;

bool isWaiting() {
    return _waiting;
} 

void onSecondTick() {
    if (getPlayer()->state == STATE_PAUSE) {
        return;
    }

    if (secondsLeft > 1) {
        secondsLeft--;
    } else {
        startNewDay();
    }
}

void cancelDayTimers() {
    delete_timer(secondsTimer);
}


/**
 * Called after the cloths have been animated
 */
void continueNewDay() {
    _waiting = false;

    updateHangingClothPosition();
    processFinishedCloths();

    for (u32 i = 0; i < clothsPerDay; i++) {
        if (!enqueueCloth()) {
            gameOver();
            return;
        }
    }

    if (secondsTimer) {
        delete_timer(secondsTimer);
    }

    if (secondsPerDay) {
        secondsLeft = secondsPerDay;
        secondsTimer = new_timer(TIMER_TICKS(TICKS_PER_SECOND), TF_CONTINUOUS, onSecondTick);
    }
}

/**
 * Set the new weather and update the size of the cloths.
 * Then pause everything for a second while we animate the change-over.
 */
void startNewDay() {
    if (secondsTimer) {
        delete_timer(secondsTimer);
    }

    secondsTimer = new_timer(TIMER_TICKS(TICKS_PER_SECOND), TF_CONTINUOUS, continueNewDay);

    newDayWeather();
    updateHangingClothSize(getCurrentWeather());

    _waiting = true;
}

void drawDay() {
    drawSprite(TIMER_SPRITE_1 + secondsLeft, QUEUE_MARGIN_LEFT, STANDARD_MARGIN, 0, 1);
}



