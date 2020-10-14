#include "cloth.h"
#include "weather.h"
#include <stdint.h>
#include <stdlib.h>
#include <libdragon.h>

static Cloth* clothQueue;
static u32 clothQueueSize = 0;

static u32 CLOTH_COLOURS[4];

void initCloths() {
    clothQueue = calloc(sizeof(Cloth), 5);
    CLOTH_COLOURS[0] = graphics_make_color(0xff, 0xff, 0xff, 0xff); // white/dry
    CLOTH_COLOURS[1] = graphics_make_color(0x00, 0xff, 0x00, 0xff); // green/damp
    CLOTH_COLOURS[2] = graphics_make_color(0xff, 0xa5, 0x00, 0xff); // orange/spun
    CLOTH_COLOURS[3] = graphics_make_color(0xff, 0xff, 0xff, 0xff); // red/drenched
}

u32 getClothColour(Cloth* cloth) {
    return CLOTH_COLOURS[cloth->dryingState];
}

static void changeClothState(Cloth* cloth, DryingState newState) {
    if (newState <= DRYING_DRY) {
        newState = DRYING_DRY;
    }

    s32 diff = newState - cloth->dryingState;

    switch(cloth->growthType) {
        case GROWTH_QUADRATIC:
            cloth->size = cloth->size * diff * cloth->growthFactor;
        break;
        case GROWTH_LINEAR:
            cloth->size += diff * cloth->growthFactor;
        break;
        case GROWTH_NONE:
        break;
        default: ;
            // throw;
    }

    cloth->dryingState = newState;
}

void updateCloth(Cloth* cloth, Weather weather) {
    switch(weather) {
        case WEATHER_STORM:
            cloth->dryingState = DRYING_DIRTY;
            break;
        case WEATHER_RAIN:
            changeClothState(cloth, DRYING_DRENCHED);
            break;
        case WEATHER_CLOUDY:
            changeClothState(cloth, cloth->dryingState - 1);
            break;
        case WEATHER_SUNNY:
            changeClothState(cloth, cloth->dryingState - 2);
            break;
    }


}