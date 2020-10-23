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
    char text[3]; // text that appears on the icon.
    u32 size;
    DryingState dryingState;
    GrowthType growthType;
    s32 growthFactor; // can be negative
} Cloth;

void initCloths();
bool isClothDry(Cloth* cloth);
void drawCloth(Cloth* cloth, u32 x, u32 y);
void updateCloth();
void buildClothText(Cloth* cloth);

#endif