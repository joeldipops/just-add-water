#include "clothManager.h"
#include "cloth.h"
#include "text.h"
#include "config.h"
#include "renderer.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


static Cloth** _masterClothList;
static u32 _clothListLength;
static Cloth* _clothQueue[CLOTH_QUEUE_SIZE];
static u32 _queueIndex = 0;

// Can be extended if necessary.
static u32 clothListMaxLength = 32;

void initClothManager() {
    _masterClothList = calloc(sizeof(Cloth*), clothListMaxLength);
    _clothListLength = 0;
    for (u32 i = 0; i < INIT_TURN_CLOTHS; i++) {
        enqueueCloth();
    }
}

void drawQueue() {
    u32 y = QUEUE_MARGIN_TOP;
    drawText("NEXT", QUEUE_MARGIN_LEFT, y, 1);

    y += TILE_WIDTH ;
    // Draw the full details of the next cloth
    Cloth* next = _clothQueue[0];
    if (next) {
        drawCloth(next, QUEUE_MARGIN_LEFT + TILE_WIDTH + 2, y);
    }

    // And then the glowing pile of pending cloths.

    // The outline of the pile.
    y += TILE_WIDTH * 3;
    drawSprite(CURSOR_TOP_LEFT_SPRITE, QUEUE_MARGIN_LEFT, y, 0, 1);
    drawSprite(CURSOR_TOP_SPRITE, QUEUE_MARGIN_LEFT + (TILE_WIDTH / 2), y, 0, 1);
    drawSprite(CURSOR_TOP_RIGHT_SPRITE, QUEUE_MARGIN_LEFT + TILE_WIDTH, y, 0, 1);

    y += TILE_WIDTH / 2;

    for (u32 i = 1; i < (CLOTH_QUEUE_SIZE - 3) / 3; i++) {
        drawSprite(CURSOR_LEFT_SPRITE, QUEUE_MARGIN_LEFT, y, 0, 1);
        drawSprite(CURSOR_RIGHT_SPRITE, QUEUE_MARGIN_LEFT + TILE_WIDTH, y, 0, 1);
        y += TILE_WIDTH;
    }

    y -= TILE_WIDTH / 2;
    drawSprite(CURSOR_BOTTOM_LEFT_SPRITE, QUEUE_MARGIN_LEFT, y, 0, 1);
    drawSprite(CURSOR_BOTTOM_SPRITE, QUEUE_MARGIN_LEFT + (TILE_WIDTH / 2), y, 0, 1);
    drawSprite(CURSOR_BOTTOM_RIGHT_SPRITE, QUEUE_MARGIN_LEFT + TILE_WIDTH, y, 0, 1);

    // Then fill it up with cloths from the bottom up.
    y -= TILE_WIDTH / 4;

    SpriteCode spriteId = QUEUED_GREEN_SPRITE;
    for (u32 i = 0; i < _queueIndex; i++) {
        if (i == CLOTH_QUEUE_SIZE / 6 * 5) {
            spriteId = QUEUED_RED_SPRITE;;
        } else if (i == CLOTH_QUEUE_SIZE / 2) {
            spriteId = QUEUED_AMBER_SPRITE;
        }

        drawScaledSprite(spriteId, QUEUE_MARGIN_LEFT + 2, y, 0, 2, 1);
        y -= TILE_WIDTH / 4;
    }
}

/**
 * Return the cloth at the head of the queue and shuffle the rest along.
 */
Cloth* dequeueCloth() {
    Cloth* result = _clothQueue[0];

    // The queue is empty.
    if (!result) {
        return result;
    }

    for (u32 i = 0; i < CLOTH_QUEUE_SIZE - 1; i++) {
        _clothQueue[i] = _clothQueue[i+1];
    }

    _clothQueue[CLOTH_QUEUE_SIZE - 1] = 0;

    _queueIndex--;

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
    cloth->growthFactor = 1;
    cloth->size = 1;
#endif
}

/**
 * Create a new cloth and add it to the upcoming queue
 * @return true if successful, false if the queue has overflowed.
 */
bool enqueueCloth() {
    if (_queueIndex > CLOTH_QUEUE_SIZE) {
        return false;
    }

    Cloth* newCloth = calloc(sizeof(Cloth), 1);

    initNewCloth(newCloth);

    _masterClothList[_clothListLength] = newCloth;

    _clothListLength++;
    if (_clothListLength >= clothListMaxLength) {
        u32 newMaxLength = clothListMaxLength * 2;
        Cloth** temp = _masterClothList;
        _masterClothList = calloc(sizeof(Cloth*), newMaxLength);
        memcpy(_masterClothList, temp, clothListMaxLength * sizeof(Cloth*));
        clothListMaxLength = newMaxLength;

        free(temp);
    }

    _clothQueue[_queueIndex] = newCloth;
    _queueIndex++;

    return true;
}

/**
 * Shifts cleaned up cloths out of existence.
 */
void removeFinishedCloths(u32 oldLength) {
    Cloth* temp[_clothListLength];
    u32 tempIndex = 0;

    for (u32 i = 0; i < oldLength; i++) {
        if (_masterClothList[i]) {
            temp[tempIndex] = _masterClothList[i];
            tempIndex++;
        }
    }

    memcpy(_masterClothList, temp, sizeof(Cloth*) * _clothListLength);
}

/**
 * Checks if cloths can be freed from memory, then does so.
 */
void processFinishedCloths() {
    u32 removedCloths = 0;

    for(u32 i = 0; i < _clothListLength; i++) {
        if (_masterClothList[i] && _masterClothList[i]->isFreeable) {
            // Do some fiddling with scores or something.
            free(_masterClothList[i]);
            _masterClothList[i] = 0;
            removedCloths++;
        }
    }

    u32 oldLength = _clothListLength;

    _clothListLength = _clothListLength - removedCloths;

    removeFinishedCloths(oldLength);
}

