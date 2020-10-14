#ifndef CLOTH_INCLUDED
#define CLOTH_INCLUDED

#include "core.h"
#include <libdragon.h>


typedef enum {
    DRYING_COMPLETE = 0,
    DRYING_DRY = 1,
    DRYING_DAMP = 2,
    DRYING_SPUN = 3,
    DRYING_DRENCHED = 4,
    DRYING_DIRTY = 5
} DryingState;

typedef enum {
    GROWTH_NONE,
    GROWTH_LINEAR,
    GROWTH_QUADRATIC
} GrowthType;

typedef struct {
    bool isFreeable;
    u32 size;
    DryingState dryingState;
    GrowthType growthType;
    s32 growthFactor; // can be negative
} Cloth;

void initCloths();

void updateCloth();

u32 getClothColour(Cloth* cloth);

#endif