#ifndef LINE_INCLUDED
#define LINE_INCLUDED

#include "weather.h"
#include "core.h"
#include "cloth.h"
#include <stdbool.h>

void initLine();

void drawLines();

bool hangCloth(u32 lineId, u32 x, Cloth* cloth);
Cloth* takeCloth(u32 lineId, u32 x);

void updateHangingCloths(Weather weather);


#endif