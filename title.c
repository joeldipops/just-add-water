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
    drawText("SHRUNK IN THE WASH!", 32, TOP_MARGIN, 2);

    float scale = 1;

    string text;
    sprintf(text, "Hang wet laundry with your left hand $%02x$%02x", L_SPRITE, D_SPRITE);
    drawText(text, QUEUE_MARGIN_LEFT, TOP_MARGIN + TILE_WIDTH * 4, scale);

    sprintf(text, "Take down or move with the right $%02x $%02x", R_SPRITE, C_SPRITE);
    drawText(text, QUEUE_MARGIN_LEFT, TOP_MARGIN + TILE_WIDTH * 6, scale);

    sprintf(text, "Discard something you picked up with $%02x", A_SPRITE);
    drawText(text, QUEUE_MARGIN_LEFT, TOP_MARGIN + TILE_WIDTH * 8, scale);

    drawText("Clothes will change size as they dry.", QUEUE_MARGIN_LEFT, TOP_MARGIN + TILE_WIDTH * 10, scale);
    drawText("(Don't run out of space!)", QUEUE_MARGIN_LEFT, TOP_MARGIN + TILE_WIDTH * 11, scale);

    drawText("Keep an eye on the weather forecast.", QUEUE_MARGIN_LEFT, TOP_MARGIN + TILE_WIDTH  * 12, scale);
    drawText("(Watch out for rain!)", QUEUE_MARGIN_LEFT, TOP_MARGIN + TILE_WIDTH * 13, scale);

    if (flashStart) {
        drawText("Press Start", 128, TOP_MARGIN + TILE_WIDTH * 2, 1);
    }
}
