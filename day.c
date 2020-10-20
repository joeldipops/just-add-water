#include "day.h"
#include "core.h"
#include "config.h"
#include "text.h"
#include "weather.h"
#include "player.h"

#include <stdio.h>
#include <libdragon.h>

static u32 secondsLeft = 0;
timer_link_t* secondsTimer;

static void onSecondTick() {
    if (getPlayer()->isPaused) {
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

    secondsLeft = INIT_TURN_SECONDS;
    secondsTimer = new_timer(TIMER_TICKS(TICKS_PER_SECOND), TF_CONTINUOUS, onSecondTick);

    newDayWeather();

    /*
    updateHangingCloths(getCurrentWeather());
    processFinishedCloths();

    for (u32 i = 0; i < clothsPerDay; i++) {
        if (!enqueueCloth()) {
            gameOver();
            return;
        }
    }
    */
}

void drawDay() {
    string text = "";
    sprintf(text, "%lu", secondsLeft);
    drawText(text, STANDARD_MARGIN, STANDARD_MARGIN, 1);
}



