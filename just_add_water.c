#include "weather.h"
#include "line.h"
#include "core.h"
#include "clothManager.h"
#include "text.h"
#include <stdio.h>
#include <libdragon.h>

#include "resources.h"

void initialiseSubsystems() {
    //initLine();
    init_interrupts();

    display_init(RESOLUTION_320x240, DEPTH_32_BPP, 2, GAMMA_NONE, ANTIALIAS_OFF);    
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
        new_timer(TIMER_TICKS(interval), TF_CONTINUOUS, onNewDay);
    }
}

void inputStep() {
    controller_scan();
    N64ControllerState keysPressed = get_keys_pressed();
    N64ControllerState keysReleased = get_keys_up();
};

u32 boxpos = 0;

u32 timesFucked = 0;

void renderStep() {
    display_context_t frameId;
    while(!(frameId = display_lock()));

    // Assure RDP is ready for new commands
    rdp_sync(SYNC_PIPE);
    // Remove any clipping windows
    rdp_set_default_clipping();
    // Enable sprite display instead of solid color fill
    rdp_enable_texture_copy();
    // Attach RDP to display
    rdp_attach_display(frameId);

    rdp_load_texture_stride(0, 0, MIRROR_DISABLED, getSpriteSheet(), 2);
    rdp_draw_sprite_scaled(0, 8, 8, 1, 1, MIRROR_DISABLED);

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

    //updateTimer(10 * TICKS_PER_SECOND);

    while(true) {
        inputStep();
        renderStep();
    }
}