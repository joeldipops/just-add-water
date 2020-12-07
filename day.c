#include "day.h"
#include "core.h"
#include "config.h"
#include "text.h"
#include "weather.h"
#include "player.h"
#include "line.h"
#include "clothManager.h"
#include "renderer.h"
#include "animation.h"

#include <stdbool.h>
#include <stdio.h>
#include <libdragon.h>

static u32 secondsLeft = 0;
timer_link_t* secondsTimer = 0;
u32 _turnCount = 1;


static u32 secondsPerDay = INIT_TURN_SECONDS;

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
    _turnCount++;


    newDayWeather();

    updateHangingClothPosition();
    processFinishedCloths();

    increaseComplexity(_turnCount);
    if (!enqueueClothsPerDay()) {
        gameOver();
        return;
    }

    // This is dodgy - of course we could have other animations going on.
    // But this is for a jam, not a retail product, so just cancel them all.
    abandonAllAnimations();

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

    secondsTimer = new_timer(TIMER_TICKS(TICKS_PER_SECOND / 1.5), TF_CONTINUOUS, continueNewDay);

    prepareNewDayWeather();
    updateHangingClothSize(getForecast());

    _waiting = true;
}

void startFirstDay() {
    newDayWeather();

    secondsLeft = secondsPerDay;
    secondsTimer = new_timer(TIMER_TICKS(TICKS_PER_SECOND), TF_CONTINUOUS, onSecondTick);

    _waiting = false;
}

void drawDay() {
    drawSprite(TIMER_SPRITE_1 + secondsLeft, QUEUE_MARGIN_LEFT, STANDARD_MARGIN, 0, 1);
}



