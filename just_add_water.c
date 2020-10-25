#include "weather.h"
#include "line.h"
#include "core.h"
#include "clothManager.h"
#include "text.h"
#include "config.h"
#include "player.h"
#include "day.h"
#include "fps.h"
#include "resources.h"

#include <stdio.h>
#include <libdragon.h>


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

void inputStep() {
    controller_scan();
    N64ControllerState keysPressed = get_keys_pressed();
    N64ControllerState keysReleased = get_keys_up();

    if (!INIT_TURN_SECONDS && keysReleased.c[0].Z) {
        startNewDay();
    }

    handleController(&keysPressed, &keysReleased);
};

display_context_t nextFrame = 0;

void renderStep() {
    fps_frame();

    // while(!(nextFrame = display_lock()))

    rdp_attach_display(nextFrame);

    drawBox(BG_SPRITE, 0, 0, SCREEN_WIDTH, SCREEN_WIDTH);

    if (getPlayer()->state == STATE_PAUSE) {
        drawText("PAUSE", 100, 100, 2);
    } else {
        drawDay();
        drawLines();
        drawQueue();
        drawWeather();
        drawPlayer();
    }

#ifdef SHOW_FRAME_COUNT
    string text;
    sprintf(text, "FPS: %d", fps_get());
    drawText(text, 0, 230, 1);
#endif

    rdp_detach_display();
    display_show(nextFrame);
}

void resetScreen() {
    graphics_fill_screen(1, 0xffffffff);
    graphics_fill_screen(2, 0xffffffff);
}

void mainLoop() {
    nextFrame = display_lock();
    if (!nextFrame) { return; }

    disable_interrupts();
    rdp_sync(SYNC_PIPE);
    inputStep();
    renderStep();
    enable_interrupts();
}

int main(void) {
    initialiseSubsystems();
    resetScreen();

    #ifdef SHOW_FRAME_COUNT
        new_timer(TIMER_TICKS(1000000), TF_CONTINUOUS, fps_timer);
    #endif

    // Assure RDP is ready for new commands
    rdp_sync(SYNC_PIPE);
    // Remove any clipping windows
    rdp_set_default_clipping();
    // Enable sprite display instead of solid color fill
    rdp_enable_texture_copy();

    startNewDay();

    // Render a frame 30 times a second.
    new_timer(TIMER_TICKS(TICKS_PER_SECOND) / 30, TF_CONTINUOUS, mainLoop);
}