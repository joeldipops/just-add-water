#include "clothManager.h"
#include "cloth.h"
#include "text.h"
#include "config.h"
#include "renderer.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


static Cloth** masterClothList;
static u32 clothListLength;
static Cloth* clothQueue[CLOTH_QUEUE_SIZE];
static u32 queueIndex = 0;

// Can be extended if necessary.
static u32 clothListMaxLength = 32;

void initClothManager() {
    masterClothList = calloc(sizeof(Cloth*), clothListMaxLength);
    clothListLength = 0;
    for (u32 i = 0; i < INIT_TURN_CLOTHS; i++) {
        enqueueCloth();
    }
}

void drawQueue() {
    u32 y = QUEUE_MARGIN_TOP;
    drawText("NEXT", QUEUE_MARGIN, y, 1);

    y += STANDARD_MARGIN;
    // Draw the full details of the next cloth
    Cloth* next = clothQueue[0];
    if (next) {
        drawCloth(next, QUEUE_MARGIN + TILE_WIDTH * 2.5, y);
    }

    // And then the glowing pile of pending cloths.

    // The outline of the pile.
    y += STANDARD_MARGIN * 3;
    drawSprite(CURSOR_TOP_LEFT_SPRITE, QUEUE_MARGIN, y, 0, 1);
    drawSprite(CURSOR_TOP_SPRITE, QUEUE_MARGIN + (TILE_WIDTH / 2), y, 0, 1);
    drawSprite(CURSOR_TOP_RIGHT_SPRITE, QUEUE_MARGIN + TILE_WIDTH, y, 0, 1);

    y += TILE_WIDTH / 2;

    for (u32 i = 1; i < (CLOTH_QUEUE_SIZE - 3) / 3; i++) {
        drawSprite(CURSOR_LEFT_SPRITE, QUEUE_MARGIN, y, 0, 1);
        drawSprite(CURSOR_RIGHT_SPRITE, QUEUE_MARGIN + TILE_WIDTH, y, 0, 1);
        y += TILE_WIDTH;
    }

    y -= TILE_WIDTH / 2;
    drawSprite(CURSOR_BOTTOM_LEFT_SPRITE, QUEUE_MARGIN, y, 0, 1);
    drawSprite(CURSOR_BOTTOM_SPRITE, QUEUE_MARGIN + (TILE_WIDTH / 2), y, 0, 1);
    drawSprite(CURSOR_BOTTOM_RIGHT_SPRITE, QUEUE_MARGIN + TILE_WIDTH, y, 0, 1);

    // Then fill it up with cloths from the bottom up.
    y -= TILE_WIDTH / 4;
    rdp_load_texture_stride(0, 0, MIRROR_DISABLED, getSpriteSheet(), QUEUED_GREEN_SPRITE);
    for (u32 i = 0; i < queueIndex; i++) {
        if (i == CLOTH_QUEUE_SIZE / 6 * 5) {
            rdp_load_texture_stride(0, 0, MIRROR_DISABLED, getSpriteSheet(), QUEUED_RED_SPRITE);
        } else if (i == CLOTH_QUEUE_SIZE / 2) {
            rdp_load_texture_stride(0, 0, MIRROR_DISABLED, getSpriteSheet(), QUEUED_AMBER_SPRITE);
        }
        rdp_draw_sprite_scaled(0, QUEUE_MARGIN + 2, y, 2, 1, MIRROR_DISABLED);
        y -= TILE_WIDTH / 4;
    }
}

/**
 * Return the cloth at the head of the queue and shuffle the rest along.
 */
Cloth* dequeueCloth() {
    Cloth* result = clothQueue[0];

    // The queue is empty.
    if (!result) {
        return result;
    }

    for (u32 i = 0; i < CLOTH_QUEUE_SIZE - 1; i++) {
        clothQueue[i] = clothQueue[i+1];
    }

    clothQueue[CLOTH_QUEUE_SIZE - 1] = 0;

    queueIndex--;

    return result;
}

#define DRYING_SIZE 6
const DryingState DryingDie[DRYING_SIZE] = {
    DRYING_SPUN, DRYING_SPUN, DRYING_SPUN, DRYING_SPUN, DRYING_SPUN,
    DRYING_DRENCHED
};

#define SIZE_SIZE 8
const u32 SizeDie[SIZE_SIZE] = {
    2, 2, 2, 2,
    3, 3, 3,
    4
};


#define GROWTH_TYPE_SIZE 4
// No growth quadratic until I can be arsed sorting out the algorithm for it.
const u32 GrowthTypeDie[GROWTH_TYPE_SIZE]= {
    GROWTH_NONE,
    GROWTH_LINEAR, GROWTH_LINEAR, GROWTH_LINEAR
};

#define FACTOR_SIZE 8
const s32 LinearFactorDie[FACTOR_SIZE] = {
    -2,
    -1,
    +1, +1, +1,
    +2, +2,
    +3
};

const s32 QuadraticFactorDie[FACTOR_SIZE] = {
    -2, -2,
    +2, +2, +2, +2
};


static void initNewCloth(Cloth* cloth) {
#ifdef RANDOMISE_CLOTHS
    cloth->dryingState = DryingDie[rand() % DRYING_SIZE];

    // Will be randomised - increasing in complexity as time goes on.
    cloth->growthType = GrowthTypeDie[rand() % GROWTH_TYPE_SIZE];
    cloth->size = SizeDie[rand() % SIZE_SIZE];

    switch(cloth->growthType) {
        case GROWTH_LINEAR:
            cloth->growthFactor = LinearFactorDie[rand() % FACTOR_SIZE];
            break;
        case GROWTH_QUADRATIC:
            cloth->growthFactor = QuadraticFactorDie[rand() % FACTOR_SIZE];
            break;
        case GROWTH_NONE:
            cloth->growthFactor = 0;
            break;
    }
#else
    cloth->dryingState = DRYING_SPUN;
    cloth->growthType = GROWTH_LINEAR;
    cloth->growthFactor = 4;
    cloth->size = 1;
#endif

    buildClothText(cloth);
}

/**
 * Create a new cloth and add it to the upcoming queue
 * @return true if successful, false if the queue has overflowed.
 */
bool enqueueCloth() {
    if (queueIndex > CLOTH_QUEUE_SIZE) {
        return false;
    }

    Cloth* newCloth = calloc(sizeof(Cloth), 1);

    initNewCloth(newCloth);

    masterClothList[clothListLength] = newCloth;

    clothListLength++;
    if (clothListLength >= clothListMaxLength) {
        u32 newMaxLength = clothListMaxLength * 2;
        Cloth** temp = masterClothList;
        masterClothList = calloc(sizeof(Cloth*), newMaxLength);
        memcpy(masterClothList, temp, clothListMaxLength * sizeof(Cloth*));
        clothListMaxLength = newMaxLength;

        free(temp);
    }

    clothQueue[queueIndex] = newCloth;
    queueIndex++;

    return true;
}

/**
 * Recursively shifts cleaned up cloths out of existence.
 */
void removeFinishedCloths(u32 startIndex) {
    bool freedFound = false;
    for (u32 i = startIndex; i < clothListLength; i++) {
        if (!masterClothList[i]) {
            removeFinishedCloths(i + 1);
            freedFound = true;
        }

        if (freedFound) {
            masterClothList[i] = masterClothList[i + 1];
        }
    }
}

/**
 * Checks if cloths can be freed from memory, then does so.
 */
void processFinishedCloths() {
    u32 removedCloths = 0;

    for(u32 i = 0; i < clothListLength; i++) {
        if (masterClothList[i]->isFreeable) {
            // Do some fiddling with scores or something.
            free(masterClothList[i]);
            masterClothList[i] = 0;
            removedCloths++;
        }
    }

    clothListLength = clothListLength - removedCloths;

    removeFinishedCloths(0);
}

