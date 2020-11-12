#include "cloth.h"
#include "config.h"
#include "text.h"
#include "weather.h"
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
            return DIRTY_SPRITE;
        default:
            return ROOF_SPRITE;
    }
}

static SpriteCode getGrowthSprite(Cloth* cloth) {
    // TODO- quadratic
    if (cloth->growthFactor > 0) {
        return LINEAR_GROW_SPRITE;
    } else if (cloth->growthFactor < 0) {
        return LINEAR_SHRINK_SPRITE;
    } else {
        return 0;
    }
}

static SpriteCode getFactorSprite(Cloth* cloth) {
    // TODO- quadratic
    u32 factor = abs(cloth->growthFactor);
    return GROWTH_0_SPRITE + factor;
}

void drawCloth(Cloth* cloth, u32 x, u32 y) {
    SpriteCode spriteId;

    drawBox(BASE_CLOTH_SPRITE, x, y, TILE_WIDTH * cloth->size, TILE_WIDTH * 2);

    if (cloth->growthFactor && cloth->dryingState > DRYING_DRY) {
        spriteId = getGrowthSprite(cloth);
        rdp_load_texture_stride(0, 0, MIRROR_DISABLED, getSpriteSheet(), spriteId);
        rdp_draw_sprite_scaled(0, x, y, 1, 2, MIRROR_DISABLED);

        spriteId = getFactorSprite(cloth);
        rdp_load_texture_stride(0, 0, MIRROR_DISABLED, getSpriteSheet(), spriteId);
        rdp_draw_sprite_scaled(0, x + 3, y + TILE_WIDTH / 2, 1.5, 1.5, MIRROR_DISABLED);
    }

    spriteId = getClothSprite(cloth);

    rdp_load_texture_stride(0, 0, MIRROR_DISABLED, getSpriteSheet(), spriteId);
    rdp_draw_sprite_scaled(0, x, y, 1, 2, MIRROR_DISABLED);
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