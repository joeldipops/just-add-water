#ifndef CLOTH_INCLUDED
#define CLOTH_INCLUDED

#include "core.h"
#include "animation.h"
#include <libdragon.h>


typedef enum {
    // Force this enum to be signed.
    DRYING_INVALID = -1,

    DRYING_COMPLETE = 0,
    DRYING_DRY = 1,
    DRYING_DAMP = 2,
    DRYING_MOIST = 3,
    DRYING_SPUN = 4,
    DRYING_DRENCHED = 5,
    DRYING_DIRTY = 6
} DryingState;

typedef enum {
    GROWTH_NONE,
    GROWTH_LINEAR,
    GROWTH_QUADRATIC
} GrowthType;

typedef struct {
    // We no longer care about this cloth.
    bool isFreeable;
    // number of discrete chunks.
    s32 size;
    // previous sized, used when animating.
    s32 oldSize;
    // Degrees of wetness.
    DryingState dryingState;
    // Only linear growth at this stage.
    GrowthType growthType;
    // can be negative
    s32 growthFactor;
    // what tile is the cloth being held by.
    s32 grabPoint;    
    // used to calculate score.
    s32 initialSize;
    // used to calculate score.
    DryingState initialDryingState;
} Cloth;

void initCloths();
void initCloth(Cloth* cloth, s32 size, DryingState dryingState);
Cloth* newCloth(s32 size, DryingState dryingState);
s32 calculateScore(Cloth* cloth);
bool isClothDry(Cloth* cloth);
void drawCloth(Cloth* cloth, s32 x, s32 y);
void updateCloth();

void setClothAnimationFrames(Cloth* cloth, s32 pixelLength, Animation** animations, s32 frameIndex, s32 x, s32 y);

void prepareClothAnimation(Cloth*, s32,s32);
#endif