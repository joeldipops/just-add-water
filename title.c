#include "text.h"
#include "config.h"

#include <stdio.h>
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

    string text;
    sprintf(text, "Hang wet laundry with your left hand ( $%02x $%02x )", L_SPRITE, D_SPRITE);
    drawText(text, STANDARD_MARGIN, STANDARD_MARGIN * 4, scale);

    sprintf(text, "Take down or move clothes with your right. ( $%02x $%02x )", R_SPRITE, C_SPRITE);
    drawText(text, STANDARD_MARGIN, STANDARD_MARGIN * 6, scale);

    sprintf(text, "Discard something you picked up with $%02x", A_SPRITE);
    drawText(text,STANDARD_MARGIN, STANDARD_MARGIN * 8, scale);

    drawText("Clothes will change size as they dry.", STANDARD_MARGIN, STANDARD_MARGIN * 10, scale);
    drawText("(Don't run out of space!)", STANDARD_MARGIN, STANDARD_MARGIN * 11, scale);

    drawText("Keep an eye on the weather forecast.", STANDARD_MARGIN, STANDARD_MARGIN * 12, scale);
    drawText("(Watch out for rain!)", STANDARD_MARGIN, STANDARD_MARGIN * 13, scale);

    if (flashStart) {
        drawText("Press Start", 224, STANDARD_MARGIN * 16, 1.5);
    }
}
