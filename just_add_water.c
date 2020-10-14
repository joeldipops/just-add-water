#include "weather.h"
#include "line.h"
#include "core.h"
#include "clothManager.h"
#include <libdragon.h>

void initialiseSubsystems() {
    initLine();
    init_interrupts();
    timer_init();
    controller_init();
    dfs_init(DFS_DEFAULT_LOCATION);

    display_init(RESOLUTION_320x240, DEPTH_32_BPP, 2, GAMMA_NONE, ANTIALIAS_OFF);
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

int main(void) {
    initialiseSubsystems();

    updateTimer(10 * TICKS_PER_SECOND);

    while(true) {
        display_context_t frameId;
        while(!(frameId = display_lock()));

        controller_scan();
        N64ControllerState keysPressed = get_keys_pressed();
        N64ControllerState keysReleased = get_keys_up();

        display_show(frameId);
    }
}