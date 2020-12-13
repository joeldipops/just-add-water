#include "clothManager.h"
#include "cloth.h"
#include "text.h"
#include "config.h"
#include "renderer.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "text.h"

static u32 _clothsPerDay = 2;


static Cloth _masterClothList[CLOTH_MASTER_LIST_SIZE];
static u32 _clothListLength;
static Cloth* _clothQueue[CLOTH_QUEUE_SIZE];
static u32 _queueIndex = 0;

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

void drawQueue() {
    u32 y = QUEUE_MARGIN_TOP;
    drawText("NEXT", QUEUE_MARGIN_LEFT, y, 1);

    y += TILE_WIDTH ;
    // Draw the full details of the next cloth
    Cloth* next = _clothQueue[0];
    if (next && next->size) {
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
    if (!result || !result->size) {
        return 0;
    }

    for (u32 i = 0; i < CLOTH_QUEUE_SIZE - 1; i++) {
        _clothQueue[i] = _clothQueue[i+1];
    }

    _clothQueue[CLOTH_QUEUE_SIZE - 1] = 0;

    _queueIndex--;

    return result;
}

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
    switch (turnCount) {
        case 1:
            _clothsPerDay = 3;
            DryingDie[0] = DRYING_SPUN;
            DryingDie[1] = DRYING_SPUN;
            DryingDie[2] = DRYING_SPUN;
            DryingDie[3] = DRYING_SPUN;
            DryingDie[4] = DRYING_SPUN;
            DryingDie[5] = DRYING_SPUN;

            SizeDie[0] = 1;
            SizeDie[1] = 1;
            SizeDie[2] = 1;
            SizeDie[3] = 1;
            SizeDie[4] = 1;
            SizeDie[5] = 1;
            SizeDie[6] = 1;
            SizeDie[7] = 1;

            GrowthTypeDie[0] = GROWTH_LINEAR;
            GrowthTypeDie[1] = GROWTH_LINEAR;
            GrowthTypeDie[2] = GROWTH_LINEAR;
            GrowthTypeDie[3] = GROWTH_LINEAR;

            
            LinearFactorDie[0] = 0;
            LinearFactorDie[1] = 0;
            LinearFactorDie[2] = 0;
            LinearFactorDie[3] = 0;
            LinearFactorDie[4] = 0;
            LinearFactorDie[5] = 0;
            LinearFactorDie[6] = 0;
            LinearFactorDie[7] = 0;
            break;

        // Turn 2, 3 Teach growth
        case 2:
            LinearFactorDie[0] = 1;
            LinearFactorDie[1] = 1;
            LinearFactorDie[2] = 1;
            LinearFactorDie[3] = 1;
            LinearFactorDie[4] = 1;
            LinearFactorDie[5] = 1;
            LinearFactorDie[6] = 1;
            LinearFactorDie[7] = 1;
            break;
        // Turn 4, 5 Start mixing things up
        case 4:
            _clothsPerDay = 2;
            LinearFactorDie[0] = 0;
            LinearFactorDie[1] = 0;
            LinearFactorDie[2] = 0;
            LinearFactorDie[3] = 0;
            break;
        // Turn 6 First size increase
        case 6:
            SizeDie[0] = 2;
            SizeDie[1] = 2;
            SizeDie[2] = 2;
            SizeDie[3] = 2;
            SizeDie[4] = 2;
            SizeDie[5] = 2;
            SizeDie[6] = 2;
            SizeDie[7] = 2;

            LinearFactorDie[0] = 1;
            LinearFactorDie[1] = 1;
            LinearFactorDie[2] = 1;
            LinearFactorDie[3] = 1;
            break;
        // Turn 7, 8, 9 Continue mixing things up
        case 7:
            SizeDie[0] = 1;
            SizeDie[1] = 1;
            SizeDie[2] = 1;
            SizeDie[3] = 1;
            SizeDie[4] = 1;
            SizeDie[5] = 1;
            SizeDie[6] = 2;
            SizeDie[7] = 2;

            LinearFactorDie[0] = 0;
            LinearFactorDie[1] = 0;
            LinearFactorDie[2] = 0;
            LinearFactorDie[3] = 0;
            break;
        // Turn 10 A big mother with full dampness (just one)
        case 10:
            _clothsPerDay = 1;

            SizeDie[0] = 3;
            SizeDie[1] = 3;
            SizeDie[2] = 3;
            SizeDie[3] = 3;
            SizeDie[4] = 3;
            SizeDie[5] = 3;
            SizeDie[6] = 3;
            SizeDie[7] = 3;

            LinearFactorDie[0] = 1;
            LinearFactorDie[1] = 1;
            LinearFactorDie[2] = 1;
            LinearFactorDie[3] = 1;

            DryingDie[0] = DRYING_DRENCHED;
            DryingDie[1] = DRYING_DRENCHED;
            DryingDie[2] = DRYING_DRENCHED;
            DryingDie[3] = DRYING_DRENCHED;
            DryingDie[4] = DRYING_DRENCHED;
            DryingDie[5] = DRYING_DRENCHED;
            break;
        // Turn 11 - 15 Things mixed up but with the possibility of a big mother every now and then
        case 11:
            _clothsPerDay = 2;
            SizeDie[0] = 1;
            SizeDie[1] = 1;
            SizeDie[2] = 1;
            SizeDie[3] = 1;
            SizeDie[4] = 1;
            SizeDie[5] = 1;
            SizeDie[6] = 2;
            SizeDie[7] = 3;

            LinearFactorDie[0] = 0;
            LinearFactorDie[1] = 0;
            LinearFactorDie[2] = 0;
            LinearFactorDie[3] = 0;

            DryingDie[0] = DRYING_DAMP;
            DryingDie[1] = DRYING_MOIST;
            DryingDie[2] = DRYING_SPUN;
            DryingDie[3] = DRYING_SPUN;
            DryingDie[4] = DRYING_SPUN;
            DryingDie[5] = DRYING_DRENCHED;
            break;
        // Turn 16 - introduce reverse growth
        case 16:
            LinearFactorDie[0] = -1;
            LinearFactorDie[1] = -1;
            LinearFactorDie[2] = -1;
            LinearFactorDie[3] = -1;
            LinearFactorDie[4] = -1;
            LinearFactorDie[5] = -1;
            LinearFactorDie[6] = -1;
            LinearFactorDie[7] = -1;


            SizeDie[0] = 2;
            SizeDie[1] = 2;
            SizeDie[2] = 2;
            SizeDie[3] = 2;
            SizeDie[4] = 2;
            SizeDie[5] = 2;
            SizeDie[6] = 3;
            SizeDie[7] = 3;
            break;
        // Turn 17 - 20 mix up with reverse growth a possibility.
        case 17:
            SizeDie[0] = 1;
            SizeDie[1] = 1;
            SizeDie[2] = 1;
            SizeDie[3] = 1;
            SizeDie[4] = 1;
            SizeDie[5] = 1;
            SizeDie[6] = 2;
            SizeDie[7] = 3;

            LinearFactorDie[0] = 0;
            LinearFactorDie[1] = 0;
            LinearFactorDie[2] = 1;
            LinearFactorDie[3] = 1;
            LinearFactorDie[4] = 1;
            LinearFactorDie[5] = 1;
            LinearFactorDie[6] = -1;
            LinearFactorDie[7] = -1;


            DryingDie[0] = DRYING_DAMP;
            DryingDie[1] = DRYING_MOIST;
            DryingDie[2] = DRYING_SPUN;
            DryingDie[3] = DRYING_SPUN;
            DryingDie[4] = DRYING_SPUN;
            DryingDie[5] = DRYING_DRENCHED;
            break;
        // Turn 21 - Cloth with Double growth rate
        case 21:
            _clothsPerDay = 1;
            LinearFactorDie[0] = 2;
            LinearFactorDie[1] = 2;
            LinearFactorDie[2] = 2;
            LinearFactorDie[3] = 2;
            LinearFactorDie[4] = 2;
            LinearFactorDie[5] = 2;
            LinearFactorDie[6] = 2;
            LinearFactorDie[7] = 2;


            SizeDie[0] = 1;
            SizeDie[1] = 1;
            SizeDie[2] = 1;
            SizeDie[3] = 1;
            SizeDie[4] = 1;
            SizeDie[5] = 1;
            SizeDie[6] = 1;
            SizeDie[7] = 1;
            break;
        case 22:
            _clothsPerDay = 2;
            SizeDie[0] = 1;
            SizeDie[1] = 1;
            SizeDie[2] = 1;
            SizeDie[3] = 1;
            SizeDie[4] = 1;
            SizeDie[5] = 1;
            SizeDie[6] = 2;
            SizeDie[7] = 3;

            LinearFactorDie[0] = 2;
            LinearFactorDie[1] = 0;
            LinearFactorDie[2] = 1;
            LinearFactorDie[3] = 1;
            LinearFactorDie[4] = 1;
            LinearFactorDie[5] = 1;
            LinearFactorDie[6] = -1;
            LinearFactorDie[7] = -1;

            break;
        // A little breather here.
        case 25:
            break;
        // Turn 30 - Triple growth rate
        case 30:
            _clothsPerDay = 1;
            LinearFactorDie[0] = 3;
            LinearFactorDie[1] = 3;
            LinearFactorDie[2] = 3;
            LinearFactorDie[3] = 3;
            LinearFactorDie[4] = 3;
            LinearFactorDie[5] = 3;
            LinearFactorDie[6] = 3;
            LinearFactorDie[7] = 3;


            SizeDie[0] = 1;
            SizeDie[1] = 1;
            SizeDie[2] = 1;
            SizeDie[3] = 1;
            SizeDie[4] = 1;
            SizeDie[5] = 1;
            SizeDie[6] = 1;
            SizeDie[7] = 1;
            break;
        case 31:
            SizeDie[0] = 1;
            SizeDie[1] = 1;
            SizeDie[2] = 1;
            SizeDie[3] = 1;
            SizeDie[4] = 1;
            SizeDie[5] = 1;
            SizeDie[6] = 2;
            SizeDie[7] = 3;

            LinearFactorDie[0] = -1;
            LinearFactorDie[1] = -1;
            LinearFactorDie[2] = 0;
            LinearFactorDie[3] = 1;
            LinearFactorDie[4] = 1;
            LinearFactorDie[5] = 1;
            LinearFactorDie[6] = 2;
            LinearFactorDie[7] = 3;
            break;
        default:
            if (turnCount > 31 && !(turnCount % 5) && _clothsPerDay < (CLOTH_QUEUE_SIZE - 1)) {
                _clothsPerDay++;
            }
            // Just continue with whatever the last setting was.
            break;
    }
}

bool enqueueCloth(Cloth* cloth) {
    if (_queueIndex > CLOTH_QUEUE_SIZE) {
        return false;
    }
    
    _clothQueue[_queueIndex] = cloth;
    _queueIndex++;

    return true;
}

/**
 * Create a new cloth and add it to the upcoming queue
 * @return true if successful, false if the queue has overflowed.
 */
bool enqueueNewCloth() {
    if (_queueIndex > CLOTH_QUEUE_SIZE) {
        return false;
    }

    Cloth* cloth = &_masterClothList[_clothListLength];
    initCloth(
        cloth,
        SizeDie[rand() % SIZE_SIZE],
        DryingDie[rand() % DRYING_SIZE]
    );

    //Cloth* cloth = newCloth(SizeDie[rand() % SIZE_SIZE]);
    cloth->growthType = GrowthTypeDie[rand() % GROWTH_TYPE_SIZE];

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

    _clothListLength++;

    enqueueCloth(cloth);
    return true;
}

bool enqueueClothsPerDay() {
    if (_queueIndex + _clothsPerDay > CLOTH_QUEUE_SIZE) {
        return false;
    }

    for (u32 i = 0; i < _clothsPerDay; i++) {
        Cloth* cloth = &_masterClothList[_clothListLength];
        initCloth(cloth, SizeDie[rand() % SIZE_SIZE], DryingDie[rand() % DRYING_SIZE]);

        cloth->growthType = GrowthTypeDie[rand() % GROWTH_TYPE_SIZE];

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

        //Cloth* cloth = newCloth(SizeDie[rand() % SIZE_SIZE]);
        //_masterClothList[_clothListLength] = cloth;
        _clothListLength++;

        enqueueCloth(cloth);
    }

    return true;
}


/**
 * Shifts cleaned up cloths out of existence.
 * @deprecated until after the jam when I try to fix the crash.
 */
void removeFinishedCloths(u32 oldLength) {
    /*
    Cloth* temp[_clothListLength];
    u32 tempIndex = 0;

    for (u32 i = 0; i < oldLength; i++) {
        if (_masterClothList[i]) {
            temp[tempIndex] = _masterClothList[i];
            tempIndex++;
        }
    }

    memcpy(_masterClothList, temp, sizeof(Cloth*) * _clothListLength);
    */
}

/**
 * Checks if cloths can be freed from memory, then does so.
 * @deprecated until after the jam when I try to fix the crash.
 */
void processFinishedCloths() {
    /*
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
    */
}

void initClothManager() {
    _queueIndex = 0;

    memset(_masterClothList, 0, sizeof(Cloth) * CLOTH_MASTER_LIST_SIZE);

    /*
    for(u32 i = 0; i < _clothListLength; i++) {
        if (_masterClothList[i]) {
            free(_masterClothList[i]);
            _masterClothList[i] = 0;
        }
    }*/

    increaseComplexity(1);

    _clothListLength = 0;
    for (u32 i = 0; i < INIT_TURN_CLOTHS; i++) {
        enqueueNewCloth();
    }
}