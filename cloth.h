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
    u32 size;
    // previous sized, used when animating.
    u32 oldSize;
    // Degrees of wetness.
    DryingState dryingState;
    // Only linear growth at this stage.
    GrowthType growthType;
    // can be negative
    s32 growthFactor;
    // what tile is the cloth being held by.
    u32 grabPoint;    
    // used to calculate score.
    u32 initialSize;
    // used to calculate score.
    DryingState initialDryingState;
} Cloth;

void initCloths();
void initCloth(Cloth* cloth, u32 size, DryingState dryingState);
Cloth* newCloth(u32 size, DryingState dryingState);
u32 calculateScore(Cloth* cloth);
bool isClothDry(Cloth* cloth);
void drawCloth(Cloth* cloth, u32 x, u32 y);
void updateCloth();

void setClothAnimationFrames(Cloth* cloth, u32 pixelLength, Animation** animations, u32 frameIndex, u32 x, u32 y);

void prepareClothAnimation(Cloth*, u32,u32);
#endif