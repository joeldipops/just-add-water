#include "line.h"
#include "core.h"
#include "cloth.h"
#include "weather.h"
#include "config.h"
#include "text.h"
#include <stdbool.h>
#include <string.h>

static Cloth* outsideLine[OUTSIDE_LINE_SIZE];
static Cloth* insideLine[INSIDE_LINE_SIZE];

void initLine() {}

void drawLine() {
    for (u32 i = 0; i < OUTSIDE_LINE_SIZE; i++) {
        if (outsideLine[i]) {
            drawCloth(
                outsideLine[i],
                LEFT_MARGIN + i * TILE_WIDTH,
                OUTSIDE_LINE_POSITION
            );
            i += outsideLine[i]->size - 1;
        }
    }

    for (u32 i = 0; i < INSIDE_LINE_SIZE; i++) {
        if (insideLine[i]) {
            drawCloth(
                insideLine[i],
                LEFT_MARGIN + i * TILE_WIDTH,
                INSIDE_LINE_POSITION
            );
            i += insideLine[i]->size - 1;
        }
    }

    for (u32 i = LEFT_MARGIN; i < 320; i += 8) {
        drawSprite(OUTSIDE_LINE, i, OUTSIDE_LINE_POSITION, 1);
        drawSprite(ROOF_SPRITE, i, ROOF_POSITION, 1);
        drawSprite(INSIDE_LINE, i, INSIDE_LINE_POSITION, 1);
    }
}

bool hangCloth(u32 lineId, u32 x, Cloth* cloth) {
    Cloth** line;
    if (lineId == 0) {
        line = outsideLine;
    } else {
        line = insideLine;
    }

    // If there's already a cloth where we want to put this new one
    for (u32 i = x; i < x + cloth->size; i++) {
        if (line[i]) {
            return false;
        }
    }

    // Otherwise hang the new cloth
    for (u32 i = x; i < x + cloth->size; i++) {
        line[i] = cloth;
    }

    return true;
}

Cloth* takeCloth(u32 lineId, u32 x) {
    Cloth** line;
    if (lineId == 0) {
        line = outsideLine;
    } else {
        line = insideLine;
    }

    if (line[x] && line[x]->dryingState <= DRYING_DRY) {
        return line[x];
    } else {
        return 0;
    }
}

void updateHangingCloths(Weather weather) {
    int i = 0;

    // First pass - resize the cloths.
    while (i < OUTSIDE_LINE_SIZE) {
        if (outsideLine[i]) {
            u32 oldSize = outsideLine[i]->size;
            updateCloth(outsideLine[i], weather);
            i += oldSize;
        } else {
            i++;
        }
    }

    i = 0;
    while (i < OUTSIDE_LINE_SIZE) {
        if (insideLine[i]) {
            u32 oldSize = outsideLine[i]->size;
            // Inside is always considered 'cloudy'
            updateCloth(insideLine[i], WEATHER_CLOUDY);
            i += oldSize;
        } else {
            i++;
        }
    }

    // Second pass - shuffle them along with the change in size and dump any that fell off.

    Cloth* outsideTemp[OUTSIDE_LINE_SIZE];
    memcpy(outsideTemp, outsideLine, sizeof(Cloth*) * OUTSIDE_LINE_SIZE);

    u32 iSource = 0;
    u32 iDest = 0;

    Cloth* lastCloth = 0;

    while (iSource < OUTSIDE_LINE_SIZE) {
        // Copy the cloths back to the line, but at the new size.
        if (outsideTemp[iSource] && lastCloth != outsideTemp[iSource]) {
            lastCloth = outsideTemp[iSource];

            // If cloth can't fit on the line, it falls and becomes dirty.
            if (iDest + lastCloth->size > OUTSIDE_LINE_SIZE) {
                lastCloth->dryingState = DRYING_DIRTY;
            } else {
                for (u32 j = 0; j < lastCloth->size; j++) {
                    outsideLine[iDest] = lastCloth;
                    iDest++;
                }
            }
        }

        iSource++;
    }

    // Do the same for the inside line.
    iSource = 0;
    iDest = 0;

    Cloth* insideTemp[INSIDE_LINE_SIZE];
    memcpy(insideTemp, insideLine, sizeof(Cloth*) * INSIDE_LINE_SIZE);

    while (iSource < INSIDE_LINE_SIZE) {
        // Copy the cloths back to the line, but at the new size.
        if (insideTemp[iSource] && lastCloth != insideTemp[iSource]) {
            lastCloth = insideTemp[iSource];

            // If cloth can't fit on the line, it falls and becomes dirty.
            if (iDest + lastCloth->size > INSIDE_LINE_SIZE) {
                lastCloth->dryingState = DRYING_DIRTY;
            } else {
                for (u32 j = 0; j < lastCloth->size; j++) {
                    insideTemp[iDest] = lastCloth;
                    iDest++;
                }
            }
        }

        iSource++;
    }
}