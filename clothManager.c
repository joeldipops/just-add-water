#include "clothManager.h"
#include "cloth.h"
#include "text.h"
#include "config.h"
#include "renderer.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "text.h"

static u32 _clothsPerDay = 3;

static Cloth** _masterClothList;
static u32 _clothListLength;
static Cloth* _clothQueue[CLOTH_QUEUE_SIZE];
static u32 _queueIndex = 0;

// Can be extended if necessary.
static u32 clothListMaxLength = 1024;

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
// Initially simple, will increase in complexity as turnCount increases.
DryingState DryingDie[DRYING_SIZE] = {
    DRYING_SPUN, DRYING_SPUN, DRYING_SPUN, DRYING_SPUN, DRYING_SPUN, DRYING_SPUN
};

#define SIZE_SIZE 8
u32 SizeDie[SIZE_SIZE] = { 1, 1, 1, 1, 1, 1, 1, 1 };


#define GROWTH_TYPE_SIZE 4
// No growth quadratic until I can be arsed sorting out the algorithm for it.
u32 GrowthTypeDie[GROWTH_TYPE_SIZE]= { 
    GROWTH_LINEAR, GROWTH_LINEAR, GROWTH_LINEAR, GROWTH_LINEAR
};

#define FACTOR_SIZE 8
s32 LinearFactorDie[FACTOR_SIZE] = { 0, 0, 0, 0, 0, 0 };

const s32 QuadraticFactorDie[FACTOR_SIZE] = {
    -2, -2,
    +2, +2, +2, +2
};

/**
 * Should be called every turn as we update the dice.
 *
 * Turn 1 Basic placement
 * 
 * Turn 2, 3 Teach growth
 * 
 * Turn 4, 5 Start mixing things up
 *
 * Turn 6 First size increase (just two instead of three cloths this turn)
 * 
 * Turn 7, 8, 9 Continue mixing things up
 * 
 * Turn 10 A big mother with full dampness (just one)
 * 
 * Turn 11 - 15 Things mixed up but with the possibility of a big mother every now and then
 * 
 * Turn 16 - introduce reverse growth
 * 
 * Turn 17 - 20 mix up with reverse growth a possibility.
 * 
 * Turn 21 - Cloth with Double growth rate
 * 
 * Turn 25 - increase cloths per turn
 * 
 * Turn 30 - Triple growth rate
 * 
 * Every 5 turns onwards - increase cloths per turn until it's half the max each turn.
 */
void increaseComplexity(u32 turnCount) {
    if (turnCount == 2) {
        LinearFactorDie[0] = 1;
        LinearFactorDie[1] = 1;
        LinearFactorDie[2] = 1;
        LinearFactorDie[3] = 1;
        LinearFactorDie[4] = 1;
        LinearFactorDie[5] = 1;
        LinearFactorDie[6] = 1;
        LinearFactorDie[7] = 1;
    } else if (turnCount == 3) {

    } 
}


static void initNewCloth(Cloth* cloth) {
#ifdef RANDOMISE_CLOTHS

    cloth->dryingState = DryingDie[rand() % DRYING_SIZE];

    // Will be randomised - increasing in complexity as time goes on.
    cloth->growthType = GrowthTypeDie[rand() % GROWTH_TYPE_SIZE];
    cloth->size = SizeDie[rand() % SIZE_SIZE];
    cloth->initialSize = cloth->size;

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

bool enqueueClothsPerDay() {
    if (_queueIndex + _clothsPerDay > CLOTH_QUEUE_SIZE) {
        return false;
    }

    for (u32 i = 0; i < _clothsPerDay; i++) {
        Cloth* newCloth = calloc(sizeof(Cloth), 1);
        initNewCloth(newCloth);
        _masterClothList[_clothListLength] = newCloth;
        _clothListLength++;

        _clothQueue[_queueIndex] = newCloth;
        _queueIndex++;
    }

    return true;
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
    // Maybe is reason for crash?  Have blown it out to a very large number so this code can hopefully be ignored.
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

    // Shouldn't happen, but maybe is why things crash?
    if (removedCloths < _clothListLength) {
        _clothListLength = _clothListLength - removedCloths;
    } else {
        _clothListLength = 0;
    }

    removeFinishedCloths(oldLength);
}

