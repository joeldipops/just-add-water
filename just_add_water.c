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

static u32 clothsPerDay = INIT_TURN_CLOTHS;
static u32 secondsPerDay = INIT_TURN_SECONDS;

void initialiseSubsystems() {
    initLine();
    initPlayer();
    initClothManager();

    init_interrupts();
    display_init(RESOLUTION, COLOUR_DEPTH, 2, GAMMA_NONE, ANTIALIAS_OFF);    
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
    if (getPlayer()->isPaused) {
        return;
    }

    newDayWeather();
    updateHangingCloths(getCurrentWeather());
    //processFinishedCloths();

    for (u32 i = 0; i < clothsPerDay; i++) {
        if (!enqueueCloth()) {
            gameOver();
            return;
        }
    }
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

    if (!secondsPerDay && keysReleased.c[0].A) {
        onNewDay();
    }

    handleController(&keysPressed, &keysReleased);
};

void renderStep() {
    display_context_t frameId;
    while(!(frameId = display_lock()));

    graphics_fill_screen(frameId, 0xffffffff);
    rdp_attach_display(frameId);

    if (getPlayer()->isPaused) {
        drawText("PAUSE", 100, 100, 2);
    } else {
        drawLine();
        drawQueue();
        drawWeather();
        drawPlayer();
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

    if (secondsPerDay) {
        updateTimer(secondsPerDay * TICKS_PER_SECOND);
    }

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