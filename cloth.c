#include "cloth.h"
#include "config.h"
#include "text.h"
#include "weather.h"
#include <stdint.h>
#include <stdlib.h>
#include <libdragon.h>

static void changeClothState(Cloth* cloth, DryingState newState) {
    if (newState <= DRYING_DRY) {
        newState = DRYING_DRY;
    }

    s32 diff = newState - cloth->dryingState;

    switch(cloth->growthType) {
        case GROWTH_LINEAR:
            cloth->size += diff * cloth->growthFactor;
        break;
        case GROWTH_QUADRATIC:
            // TODO
            //cloth->size = cloth->size * diff * cloth->growthFactor;
            break;
        case GROWTH_NONE:
            break;
        default: ;
            // throw;
    }

    if (cloth->size <= 1) {
        cloth->size = 2;
    } else if (cloth->size > 16) {
        cloth->size = 16;
    }

    cloth->dryingState = newState;
}

static char charMap[10] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

void buildClothText(Cloth* cloth) {
    if (cloth->growthFactor < 0) {\
        if (cloth->growthType == GROWTH_LINEAR) {
            cloth->text[0] = '-';
        } else {
            cloth->text[0] = '/';
        }
    } else if (cloth->growthFactor == 0) {
        cloth->text[0] = ' ';
        cloth->text[1] = '0';
    } else {
        if (cloth->growthType == GROWTH_LINEAR) {
            cloth->text[0] = '+';
        } else {
            cloth->text[0] = '-';
        }
    }

    cloth->text[1] = charMap[abs(cloth->growthFactor)];
    cloth->text[2] = 0;
}

static SpriteCode getClothSprite(Cloth* cloth) {
    switch(cloth->dryingState) {
        case DRYING_SPUN:
            return SPUN_SPRITE;
        case DRYING_DAMP:
            return DAMP_SPRITE;
        case DRYING_DRENCHED:
            return DRENCHED_SPRITE;
        case DRYING_DRY:
        case DRYING_COMPLETE:
            return DRY_SPRITE;
        case DRYING_DIRTY:
            return SUN_SPRITE;
        default:
            return ROOF_SPRITE;
    }
}

void drawCloth(Cloth* cloth, u32 x, u32 y) {
    SpriteCode spriteId = getClothSprite(cloth);
    for (u32 i = 0; i < cloth->size; i++) {
        drawSprite(
            spriteId,
            x + i * TILE_WIDTH,
            y,
            1
        );
        drawSprite(
            spriteId,
            x + i * TILE_WIDTH,
            y + TILE_WIDTH,
            1
        );
    }

    drawText(cloth->text, x, y + (TILE_WIDTH / 2) , 1);
}

void updateCloth(Cloth* cloth, Weather weather) {
    switch(weather) {
        case WEATHER_STORM:
            cloth->dryingState = DRYING_DIRTY;
            return;
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