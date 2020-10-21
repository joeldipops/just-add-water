#include "clothManager.h"
#include "cloth.h"
#include "text.h"
#include "config.h"
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
    for (u32 i = 0; i < CLOTH_QUEUE_SHOWN; i++) {
        drawText("NEXT", QUEUE_MARGIN, QUEUE_MARGIN_TOP, 1);

        Cloth* next = clothQueue[i];
        if (!next) {
            return;
        }

        drawCloth(
            next,
            QUEUE_MARGIN,
            QUEUE_MARGIN_TOP + STANDARD_MARGIN + ((TILE_WIDTH + STANDARD_MARGIN) * i) 
        );
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
    1,
    2, 2, 2,
    3, 3,
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
    -1, -1,
    +1, +1, +1,
    +2, +2
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
    cloth->growthFactor = -1;
    cloth->size = 2;
#endif

    buildClothText(cloth);
}

/**
 * Create a new cloth and add it to the upcoming queue
 * @return true if successful, false if the queue has overflowed.
 */
bool enqueueCloth() {
    if (queueIndex >= CLOTH_QUEUE_SIZE) {
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

