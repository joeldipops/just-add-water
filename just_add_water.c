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
#include "title.h"
#include "renderer.h"
#include "animation.h"

#include <stdio.h>
#include <stdlib.h> 
#include <libdragon.h>

/**
 *************** TODOS ********************
 * * Make the days longer at first, and gradually shorten then
 * ** Which means tweaking the timer icons again.
 * * Fix that segfault.
 * * Make the left and right hands easier to tell apart.
 * * There might be something wrong with the growth, double check.
 * * Sound effects
 * * Animate the growth between days!
 * * Animate the hands as put and take things.
 */


static void initialiseSubsystems() {
    initLine();
    initPlayer();
    initClothManager();
    initAnimation();

    init_interrupts();
    display_init(RESOLUTION, COLOUR_DEPTH, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE_FETCH_ALWAYS);
    dfs_init(DFS_DEFAULT_LOCATION);
    rdp_init();
    controller_init();
    timer_init();
    initText();
    initResources();
    resetRenderer();
}

static void inputStep() {
    controller_scan();
    N64ControllerState keysReleased = get_keys_up();

    handleController(0, &keysReleased);

    if (!INIT_TURN_SECONDS && keysReleased.c[0].Z) {
        startNewDay();
    }
};

static void drawPause() {
    drawText("PAUSED", 128, 128, 2);
    drawWeatherGuide(224);
}

static void resetScreen() {
    graphics_fill_screen(1, 0xffffffff);
    graphics_fill_screen(2, 0xffffffff);
}

display_context_t nextFrame = 0;
bool isRenderAllowed = true;

void renderFrame() {
    if (!isRenderAllowed) {
        return;
    }

    isRenderAllowed = false;

    while(!(nextFrame = display_lock()))
    drawAnimations();

    rdp_sync(SYNC_PIPE);

    fps_frame();

    rdp_attach_display(nextFrame);

    drawBox(BG_SPRITE, 0, 0, SCREEN_WIDTH, SCREEN_WIDTH);

    switch (getPlayer()->state) {
        case STATE_TITLE:
            drawTitle();
            break;
        case STATE_PAUSE:
            drawPause();
            break;
        case STATE_GAMEOVER:
        case STATE_PLAY:
            drawDay();
            drawWeather();
            drawLines();
            drawQueue();
            drawPlayer();
            break;
        default:
            drawText("Error", 100, 100, 2);
            break;
    }

    drawAnimations();

    renderSprites();

    // If gameover, keep drawing the background, but just plop gameover over the top of it.
    if (getPlayer()->state == STATE_GAMEOVER) {

        drawBox(BG_SPRITE, 0, 112, SCREEN_WIDTH, TILE_WIDTH * 3);
        drawText("Game Over", 200, 112, 2);
        drawText("Press Start", 200, 142, 1);
    }

#ifdef SHOW_FRAME_COUNT
    string text;
    sprintf(text, "FPS: %d", fps_get());
    drawText(text, 0, 454, 1);
#endif

    rdp_detach_display();

    display_show(nextFrame);
}

void allowRender() {
    isRenderAllowed = true;
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

    initTitle();

    // Render a frame 30 times a second.
    new_timer(TIMER_TICKS(TICKS_PER_SECOND) / 30, TF_CONTINUOUS, allowRender);

    while(true) {
        inputStep();
        renderFrame();
    }
}