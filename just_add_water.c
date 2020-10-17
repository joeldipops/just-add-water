#include "weather.h"
#include "line.h"
#include "core.h"
#include "clothManager.h"
#include "text.h"
#include "config.h"
#include "player.h"
#include <stdio.h>
#include <libdragon.h>

#include "resources.h"

void initialiseSubsystems() {
    initLine();
    initPlayer();
    initClothManager();

    init_interrupts();
    display_init(RESOLUTION_640x480, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_OFF);    
    dfs_init(DFS_DEFAULT_LOCATION);
    rdp_init();
    controller_init();
    timer_init();
    initText();
    initResources();
}

static timer_link_t* dayTimer = 0;

void gameOver() {
    delete_timer(dayTimer);
}

void onNewDay() {
    newDayWeather();
    updateHangingCloths(getCurrentWeather());

    processFinishedCloths();
    if(!enqueueCloth()) {
        gameOver();
    }

    Cloth* heldCloth = dequeueCloth();
}

void updateTimer(u32 interval) {
    if (dayTimer) {
        delete_timer(dayTimer);
    }
    new_timer(TIMER_TICKS(interval), TF_CONTINUOUS, onNewDay);
}

void inputStep() {
    controller_scan();
    N64ControllerState keysPressed = get_keys_pressed();
    N64ControllerState keysReleased = get_keys_up();

    handleController(&keysPressed, &keysReleased);
};

void drawHud() {
    for (u32 i = LEFT_MARGIN; i < 320; i += 8) {
        drawSprite(OUTSIDE_LINE, i, OUTSIDE_LINE_POSITION, 1);
        drawSprite(ROOF_SPRITE, i, ROOF_POSITION, 1);
        drawSprite(INSIDE_LINE, i, INSIDE_LINE_POSITION, 1);
    }
}

void drawCursors() {
    Player* player = getPlayer();
    drawSprite(
        HANG_SPRITE,
        LEFT_MARGIN + (TILE_WIDTH * player->hangX),
        player->hangY 
            ? INSIDE_LINE_POSITION
            : OUTSIDE_LINE_POSITION
        , 1
    );

    drawSprite(
        TAKE_SPRITE,
        LEFT_MARGIN + (TILE_WIDTH * player->takeX),
        player->takeY 
            ? INSIDE_LINE_POSITION
            : OUTSIDE_LINE_POSITION
        , 1
    );
}

void renderStep() {
    display_context_t frameId;
    while(!(frameId = display_lock()));

    graphics_fill_screen(frameId, 0xffffffff);
    rdp_attach_display(frameId);

    if (getPlayer()->isPaused) {
        drawText("PAUSE", 100, 100, 2);
    } else {
        drawHud();
        drawCursors();
    }

    rdp_detach_display();
    display_show(frameId);
}

void resetScreen() {
    graphics_fill_screen(1, 0xffffffff);
    graphics_fill_screen(2, 0xffffffff);
}

int main(void) {
    initialiseSubsystems();
    resetScreen();

    updateTimer(3 * TICKS_PER_SECOND);

    // Assure RDP is ready for new commands
    rdp_sync(SYNC_PIPE);
    // Remove any clipping windows
    rdp_set_default_clipping();
    // Enable sprite display instead of solid color fill
    rdp_enable_texture_copy();

    while(true) {
        inputStep();
        renderStep();
    }
}