#include "cloth.h"
#include "config.h"
#include "text.h"
#include "weather.h"
#include "renderer.h"
#include "animation.h"
#include <stdint.h>
#include <stdlib.h>
#include <libdragon.h>

#include <stdio.h>

static void changeClothState(Cloth* cloth, DryingState newState) {
    cloth->oldSize = cloth->size;

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


/**
 * TODO: This only handles discrete 'size' chunks which is next to useless, but good to have on the board.
 * The next step is to nudge those x values along so stuff is actually growing with each frame...
 */
void setClothAnimationFrames(Cloth* cloth, u32 pixelLength, Animation** animations, u32 frameIndex, u32 x, u32 y) {
    #define currentFrame() (&animations[animationIndex]->frames[frameIndex])

    SpriteCode spriteId;

    u32 animationIndex = 0;
    float secsPerFrame = 0.05;


    setSimpleFrame(currentFrame(), BASE_CLOTH_SPRITE, x, y, secsPerFrame);
    currentFrame()->z = 0;
    currentFrame()->scaleX = ((float)pixelLength / (float)TILE_WIDTH);
    currentFrame()->scaleY = 2;
    animationIndex++;

    u32 tilesUsed = pixelLength / TILE_WIDTH;
    u32 overHang = pixelLength % TILE_WIDTH;

    // Draw border.
    u32 drawPriority = 1;
    if (cloth->dryingState > DRYING_DRY) {
        // Left end
        setSimpleFrame(currentFrame(), CURSOR_TOP_LEFT_SPRITE, x, y, secsPerFrame);
        currentFrame()->z = drawPriority;
        animationIndex++;

        setSimpleFrame(currentFrame(), CURSOR_BOTTOM_LEFT_SPRITE, x, y + TILE_WIDTH, secsPerFrame);
        currentFrame()->z = drawPriority;
        animationIndex++;

        // Right end
        u32 xPos = x + TILE_WIDTH * (tilesUsed - 1) + overHang;
        setSimpleFrame(currentFrame(), CURSOR_TOP_RIGHT_SPRITE, xPos, y, secsPerFrame);
        currentFrame()->z = drawPriority;
        animationIndex++;

        setSimpleFrame(currentFrame(), CURSOR_BOTTOM_RIGHT_SPRITE, xPos, y + TILE_WIDTH, secsPerFrame);
        currentFrame()->z = drawPriority;
        animationIndex++;

        // Only show the middle tile if we have to.
        // This isn't working but I don't understand the behaviour I'm seeing at all.
        // The middle tile disappears when I want it to show, and shows offset by 16px when
        // I don't want to see it at all and it's doing my head in.

        // if (pixelLength > TILE_WIDTH * 2) {
        //     for (u32 i = TILE_WIDTH; i < pixelLength; i += TILE_WIDTH) {
        //         u32 xPos = x + i;

        //         setSimpleFrame(currentFrame(), CURSOR_TOP_SPRITE, xPos, y, secsPerFrame);
        //         currentFrame()->z = drawPriority;
        //         animationIndex++;

        //         setSimpleFrame(currentFrame(), CURSOR_BOTTOM_SPRITE, xPos, y + TILE_WIDTH, secsPerFrame);
        //         currentFrame()->z = drawPriority;
        //         animationIndex++;
        //     }
        // }
    } else {
        // TODO - how to cover the progression to gilding.  Probably doing one side at a time so the gilding gradually 
        // circles the cloth.
        // Left end
        setSimpleFrame(currentFrame(), GILDED_TOP_LEFT_SPRITE, x, y, secsPerFrame);
        currentFrame()->z = drawPriority;
        animationIndex++;

        setSimpleFrame(currentFrame(), GILDED_BOTTOM_LEFT_SPRITE, x, y + TILE_WIDTH, secsPerFrame);
        currentFrame()->z = drawPriority;
        animationIndex++;

        // Right end
        u32 xPos = x + TILE_WIDTH * (tilesUsed - 1) + overHang;
        setSimpleFrame(currentFrame(), GILDED_TOP_RIGHT_SPRITE, xPos, y, secsPerFrame);
        currentFrame()->z = drawPriority;
        animationIndex++;

        setSimpleFrame(currentFrame(), GILDED_BOTTOM_RIGHT_SPRITE, xPos, y + TILE_WIDTH, secsPerFrame);
        currentFrame()->z = drawPriority;
        animationIndex++;
    }

    drawPriority = 2;

    // Draw water gauge.
    switch(cloth->dryingState) {
        case DRYING_DRENCHED:
            setSimpleFrame(currentFrame(), FULL_WATER_SPRITE, x, y, secsPerFrame);
            currentFrame()->z = drawPriority;
            animationIndex++;

            setSimpleFrame(currentFrame(), FULL_WATER_SPRITE, x, y + TILE_WIDTH, secsPerFrame);
            currentFrame()->z = drawPriority;
            animationIndex++;
            break;
        case DRYING_SPUN:
            setSimpleFrame(currentFrame(), HALF_WATER_SPRITE, x, y, secsPerFrame);
            currentFrame()->z = drawPriority;
            animationIndex++;

            setSimpleFrame(currentFrame(), FULL_WATER_SPRITE, x, y + TILE_WIDTH, secsPerFrame);
            currentFrame()->z = drawPriority;
            animationIndex++;
            break;
        case DRYING_MOIST:
            setSimpleFrame(currentFrame(), NO_WATER_SPRITE, x, y, secsPerFrame);
            currentFrame()->z = drawPriority;
            animationIndex++;

            setSimpleFrame(currentFrame(), FULL_WATER_SPRITE, x, y + TILE_WIDTH, secsPerFrame);
            currentFrame()->z = drawPriority;
            animationIndex++;
            break;
        case DRYING_DAMP:
            setSimpleFrame(currentFrame(), NO_WATER_SPRITE, x, y, secsPerFrame);
            currentFrame()->z = drawPriority;
            animationIndex++;

            setSimpleFrame(currentFrame(), HALF_WATER_SPRITE, x, y + TILE_WIDTH, secsPerFrame);
            currentFrame()->z = drawPriority;
            animationIndex++;
            break;
        case DRYING_DRY:
        case DRYING_COMPLETE:
            setSimpleFrame(currentFrame(), NO_WATER_SPRITE, x, y, secsPerFrame);
            currentFrame()->z = drawPriority;
            animationIndex++;

            setSimpleFrame(currentFrame(), NO_WATER_SPRITE, x, y + TILE_WIDTH, secsPerFrame);
            currentFrame()->z = drawPriority;
            animationIndex++;
            break;
        case DRYING_DIRTY:
            setSimpleFrame(currentFrame(), DIRTY_WATER_SPRITE, x, y, secsPerFrame);
            currentFrame()->z = drawPriority;
            animationIndex++;

            setSimpleFrame(currentFrame(), DIRTY_WATER_SPRITE, x, y + TILE_WIDTH, secsPerFrame);
            currentFrame()->z = drawPriority;
            animationIndex++;
            break;
        default: break;
    }

    u32 xPos = x + TILE_WIDTH * (tilesUsed - 1) + overHang;

    if (cloth->growthFactor > 0) {
        setSimpleFrame(
            currentFrame(), BIG_DRY_SPRITE,
            xPos, y, secsPerFrame
        );
        currentFrame()->z = drawPriority;
        animationIndex++;

        setSimpleFrame(
            currentFrame(), SMALL_WET_SPRITE,
            xPos, y + TILE_WIDTH, secsPerFrame
        );
        currentFrame()->z = drawPriority;
        animationIndex++;
    } else if (cloth->growthFactor < 0) {
        setSimpleFrame(
            currentFrame(), BIG_WET_SPRITE,
            xPos, y, secsPerFrame
        );
        currentFrame()->z = drawPriority;
        animationIndex++;

        setSimpleFrame(
            currentFrame(), SMALL_DRY_SPRITE,
            xPos, y + TILE_WIDTH, secsPerFrame
        );
        currentFrame()->z = drawPriority;
        animationIndex++;
    }

    if (cloth->growthFactor) {
        setSimpleFrame(
            currentFrame(), GROWTH_1_SPRITE + abs(cloth->growthFactor) - 1,
            xPos + 6,
            y + 6, secsPerFrame
        );
        currentFrame()->z = drawPriority;
        animationIndex++;
    }

    #undef currentFrame
}

void prepareClothAnimation(Cloth* cloth, u32 x, u32 y) {
    u32 minSize;
    u32 maxSize;

    if (cloth->oldSize > cloth->size) {
       minSize = cloth->size;
       maxSize = cloth->oldSize; 
    } else {
       maxSize = cloth->size;
       minSize = cloth->oldSize;
    }
    float diff = (maxSize - minSize) * TILE_WIDTH;

    // Some arbitrary number, we should actually calculate this from the cloth properties...
    u32 spritesNeeded = 16;

    // We will have 16 animation frames.
    const float numberOfFrames = 16;

    Animation* animations[spritesNeeded];
    for (u32 i = 0; i < spritesNeeded; i++) {
        animations[i] = newAnimation(numberOfFrames);
        animations[i]->numberOfFrames = numberOfFrames;
    }

    for (u32 i = 0; i < numberOfFrames; i++) {
        u32 pixelWidth;
        if (cloth->size > cloth->oldSize) {
            // Only assumes growth.
            pixelWidth = (cloth->oldSize * TILE_WIDTH) + ((diff / numberOfFrames) * i);
        } else {
            pixelWidth = (cloth->size * TILE_WIDTH) + ((diff / numberOfFrames) * (numberOfFrames - i));
        }

        setClothAnimationFrames(cloth, pixelWidth, animations, i, x, y);
    }

    // Ensure all sprites start and end at the same time.
    disable_interrupts();
    for (u32 i = 0; i < spritesNeeded; i++) {
        startAnimation(animations[i]);
    }
    enable_interrupts();

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
            // Stop the animation bouncing around.
            cloth->oldSize = cloth->size;
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