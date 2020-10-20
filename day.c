#include "core.h"
#include "config.h"
#include "text.h"

#include <stdio.h>
#include <libdragon.h>

static u32 secondsLeft = 0;
timer_link_t secondsTimer;

static void onSecondTick() {
    if (secondsLeft > 0) {
        secondsLeft--;
    }
}

void startNewDay() {
    secondsLeft = INIT_TURN_SECONDS;
    new_timer(TIMER_TICKS(TICKS_PER_SECOND), TF_CONTINUOUS, onSecondTick);
}

void drawDay() {
    string text = "";
    sprintf(text, "%lu", secondsLeft);
    //drawText(text, 150, 100, 1);
}



