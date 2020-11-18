#include "cloth.h"
#include "config.h"
#include "text.h"
#include "weather.h"
#include "renderer.h"
#include <stdint.h>
#include <stdlib.h>
#include <libdragon.h>

static void changeClothState(Cloth* cloth, DryingState newState) {
    // There's no coming back from dirty.
    if (cloth->dryingState == DRYING_DIRTY) {
        return;
    }

    if (newState <= DRYING_DRY) {
        newState = DRYING_DRY;
    }

    s32 diff = newState - cloth->dryingState;

    switch(cloth->growthType) {
        case GROWTH_LINEAR:
            // -1: Grow under sun, shrink in rain.
            cloth->size += diff * cloth->growthFactor * -1;
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

    if (cloth->size < 1) {
        cloth->size = 1;
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

void drawCloth(Cloth* cloth, u32 x, u32 y) {
    SpriteCode spriteId;

    drawScaledSprite(BASE_CLOTH_SPRITE, x, y, 0, cloth->size, 2);

    // Draw border.

    u32 drawPriority = 1;
    if (cloth->dryingState > DRYING_DRY) {


        // Normal border.
        drawSprite(CURSOR_TOP_LEFT_SPRITE, x, y, drawPriority, 1);
        drawSprite(CURSOR_BOTTOM_LEFT_SPRITE, x, y + TILE_WIDTH, drawPriority, 1);

        for (u32 i = 0; i < cloth->size; i++) {
            u32 xPos = x + TILE_WIDTH * i;

            if (i + 1 == cloth->size) {
                drawSprite(CURSOR_TOP_RIGHT_SPRITE, xPos, y, drawPriority, 1);
                drawSprite(CURSOR_BOTTOM_RIGHT_SPRITE, xPos, y + TILE_WIDTH, drawPriority, 1);
            } else {
                drawSprite(CURSOR_TOP_SPRITE, xPos, y, drawPriority, 1);
                drawSprite(CURSOR_BOTTOM_SPRITE, xPos, y + TILE_WIDTH, drawPriority, 1);
            }
        }
    } else {
        // Gilded border to show we're done.
        drawSprite(GILDED_TOP_LEFT_SPRITE, x, y, drawPriority, 1);
        drawSprite(GILDED_BOTTOM_LEFT_SPRITE, x, y + TILE_WIDTH, drawPriority, 1);

        for (u32 i = 0; i < cloth->size; i++) {
            u32 xPos = x + TILE_WIDTH * i;

            if (i + 1 == cloth->size) {
                drawSprite(GILDED_TOP_RIGHT_SPRITE, xPos, y, drawPriority, 1);
                drawSprite(GILDED_BOTTOM_RIGHT_SPRITE, xPos, y + TILE_WIDTH, drawPriority, 1);
            } else {
                drawSprite(GILDED_TOP_SPRITE, xPos, y, drawPriority, 1);
                drawSprite(GILDED_BOTTOM_SPRITE, xPos, y + TILE_WIDTH, drawPriority, 1);
            }
        }
    }

    drawPriority = 2;

    // Draw water gauge.
    switch(cloth->dryingState) {
        case DRYING_DRENCHED:
            drawSprite(FULL_WATER_SPRITE, x, y, drawPriority, 1);
            drawSprite(FULL_WATER_SPRITE, x, y + TILE_WIDTH, drawPriority, 1);
            break;
        case DRYING_SPUN:
            drawSprite(HALF_WATER_SPRITE, x, y, drawPriority, 1);
            drawSprite(FULL_WATER_SPRITE, x, y + TILE_WIDTH, drawPriority, 1);
            break;
        case DRYING_MOIST:
            drawSprite(NO_WATER_SPRITE, x, y, drawPriority, 1);
            drawSprite(FULL_WATER_SPRITE, x, y + TILE_WIDTH, drawPriority, 1);
            break;
        case DRYING_DAMP:
            drawSprite(NO_WATER_SPRITE, x, y, drawPriority, 1);
            drawSprite(HALF_WATER_SPRITE, x, y + TILE_WIDTH, drawPriority, 1);
            break;
        case DRYING_DRY:
        case DRYING_COMPLETE:
            drawSprite(NO_WATER_SPRITE, x, y, drawPriority, 1);
            drawSprite(NO_WATER_SPRITE, x, y + TILE_WIDTH, drawPriority, 1);
            break;
        case DRYING_DIRTY:
            drawSprite(DIRTY_WATER_SPRITE, x, y, drawPriority, 1);
            drawSprite(DIRTY_WATER_SPRITE, x, y + TILE_WIDTH, drawPriority, 1);
            break;
        default: break;
    }

    if (cloth->growthFactor > 0) {
        drawSprite(BIG_DRY_SPRITE, x + TILE_WIDTH * (cloth->size - 1), y, drawPriority, 1);
        drawSprite(SMALL_WET_SPRITE, x + TILE_WIDTH * (cloth->size - 1), y + TILE_WIDTH, drawPriority, 1);
    } else if (cloth->growthFactor < 0) {
        drawSprite(BIG_WET_SPRITE, x + TILE_WIDTH * (cloth->size - 1), y, drawPriority, 1);
        drawSprite(SMALL_DRY_SPRITE, x + TILE_WIDTH * (cloth->size - 1), y + TILE_WIDTH, drawPriority, 1);
    }

    if (cloth->growthFactor) {
        drawSprite(GROWTH_1_SPRITE + abs(cloth->growthFactor) - 1, x + TILE_WIDTH * (cloth->size - 1) + 6, y  + 6, drawPriority, 1);
    }
}

bool isClothDry(Cloth* cloth) {
    return cloth->dryingState <= DRYING_DRY;
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