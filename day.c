#include "day.h"
#include "core.h"
#include "config.h"
#include "text.h"
#include "weather.h"
#include "player.h"
#include "line.h"
#include "clothManager.h"

#include <stdio.h>
#include <libdragon.h>

static u32 secondsLeft = 0;
timer_link_t* secondsTimer;

static u32 secondsPerDay = INIT_TURN_SECONDS;
static u32 clothsPerDay = INIT_TURN_CLOTHS;

static void onSecondTick() {
    if (getPlayer()->state == STATE_PAUSE) {
        return;
    }

    if (secondsLeft > 1) {
        secondsLeft--;
    } else {
        startNewDay();
    }

    onStateChanged();
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
    string text = "";
    sprintf(text, "%lu", secondsLeft);
    drawText(text, STANDARD_MARGIN, STANDARD_MARGIN, 1);
}



