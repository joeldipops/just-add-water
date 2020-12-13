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

static s32 secondsLeft = 0;
timer_link_t* secondsTimer = 0;
s32 _turnCount = 1;

static s32 secondsPerDay = INIT_TURN_SECONDS;

void initDay() {
    _turnCount = 1;
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
    secondsTimer = 0;
}

/**
 * Called after the cloths have been animated
 */
void continueNewDay() {
    _waiting = false;
    _turnCount++;

    newDayWeather();

    updateHangingClothPosition();

    increaseComplexity(_turnCount);
    if (!enqueueClothsPerDay()) {
        gameOver();
        return;
    }

    abandonAnimationsOfType(1);

    disable_interrupts();
    delete_timer(secondsTimer);
    secondsLeft = secondsPerDay;
    secondsTimer = new_timer(TIMER_TICKS(TICKS_PER_SECOND), TF_CONTINUOUS, onSecondTick);
    enable_interrupts();
}

/**
 * Set the new weather and update the size of the cloths.
 * Then pause everything for a second while we animate the change-over.
 */
void startNewDay() {
    disable_interrupts();
    delete_timer(secondsTimer);
    secondsTimer = new_timer(TIMER_TICKS(TICKS_PER_SECOND / 1.5), TF_CONTINUOUS, continueNewDay);
    enable_interrupts();

    prepareNewDayWeather();
    updateHangingClothSize(getForecast());

    _waiting = true;
}

void startFirstDay() {
    prepareNewDayWeather();
    newDayWeather();

    disable_interrupts();
    delete_timer(secondsTimer);
    secondsLeft = secondsPerDay;
    secondsTimer = new_timer(TIMER_TICKS(TICKS_PER_SECOND), TF_CONTINUOUS, onSecondTick);
    enable_interrupts();

    _turnCount = 1;
    _waiting = false;
}

void drawDay() {
    drawSprite(TIMER_SPRITE_1 + secondsLeft, QUEUE_MARGIN_LEFT, STANDARD_MARGIN, 0, 1);
}



