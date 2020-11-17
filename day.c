#include "day.h"
#include "core.h"
#include "config.h"
#include "text.h"
#include "weather.h"
#include "player.h"
#include "line.h"
#include "clothManager.h"
#include "renderer.h"

#include <stdio.h>
#include <libdragon.h>

static u32 secondsLeft = 0;
timer_link_t* secondsTimer = 0;

static u32 secondsPerDay = INIT_TURN_SECONDS;
static u32 clothsPerDay = INIT_TURN_CLOTHS;

void initDay() {

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

void startNewDay() {
    if (secondsTimer) {
        delete_timer(secondsTimer);
    }

    if (secondsPerDay) {
        secondsLeft = secondsPerDay;
        secondsTimer = new_timer(TIMER_TICKS(TICKS_PER_SECOND), TF_CONTINUOUS, onSecondTick);
    }

    newDayWeather();

    updateHangingCloths(getCurrentWeather());
    processFinishedCloths();

    for (u32 i = 0; i < clothsPerDay; i++) {
        if (!enqueueCloth()) {
            gameOver();
            return;
        }
    }
}

void drawDay() {
    drawSprite(TIMER_SPRITE_1 + secondsLeft, STANDARD_MARGIN, STANDARD_MARGIN, 0, 1);
    drawSprite(TIMER_SPRITE_1 + 8 + secondsLeft, STANDARD_MARGIN, STANDARD_MARGIN + 32, 0, 1);
}



