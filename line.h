#ifndef LINE_INCLUDED
#define LINE_INCLUDED

#include "weather.h"
#include "core.h"
#include "cloth.h"
#include <stdbool.h>

void initLine();

void drawLines();

bool hangCloth(s32 lineId, s32 x, Cloth* cloth);
Cloth* takeCloth(s32 lineId, s32 x);

void updateHangingCloths(Weather weather);
void updateHangingClothSize();
void updateHangingClothPosition();

#endif