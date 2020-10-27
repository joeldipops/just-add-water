#include "text.h"
#include "config.h"

#include <libdragon.h>

static bool flashStart = true;
static timer_link_t* titleAnimationTimer;

static void animateTitleScreen() {
    flashStart = !flashStart;
}

void closeTitle() {
    delete_timer(titleAnimationTimer);
}

void initTitle() {
    new_timer(TIMER_TICKS(TICKS_PER_SECOND) / 3, TF_CONTINUOUS, animateTitleScreen);
}

void drawTitle() {
    drawText("SHRUNK IN THE WASH!", 100, STANDARD_MARGIN, 2);

    float scale = 1;
    drawText("Hang wet laundry with your left hand.", STANDARD_MARGIN, STANDARD_MARGIN * 4, scale);
    drawText("Take down or move clothes with your right.", STANDARD_MARGIN, STANDARD_MARGIN * 6, scale);
    drawText("Clothes will change size as they dry.", STANDARD_MARGIN, STANDARD_MARGIN * 8, scale);
    drawText("Don't run out of space", STANDARD_MARGIN, STANDARD_MARGIN * 9, scale);
    // TODO - a pretty diagram.
    drawText("Keep an eye on the weather forecast.", STANDARD_MARGIN, STANDARD_MARGIN * 12, scale);
    drawText("Watch out for rain!", STANDARD_MARGIN, STANDARD_MARGIN * 13, scale);

    if (flashStart) {
        drawText("Press Start", 224, STANDARD_MARGIN * 16, 1.5);
    }
}
