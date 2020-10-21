#include "line.h"
#include "core.h"
#include "cloth.h"
#include "weather.h"
#include "config.h"
#include "text.h"
#include <stdbool.h>
#include <string.h>


typedef struct {
    Cloth** line;
    u32 length;
} Line;

static Cloth* _outsideLine[OUTSIDE_LINE_SIZE];
static Cloth* _insideLine[INSIDE_LINE_SIZE];

static Line outsideLine;
static Line insideLine;

void initLine() {
    outsideLine.line = _outsideLine;
    outsideLine.length = OUTSIDE_LINE_SIZE;

    insideLine.line = _insideLine;
    insideLine.length = INSIDE_LINE_SIZE;
}

static void drawLine(Line* line, u32 x, u32 y) {
    for (u32 i = 0; i < line->length; i++) {
        if (line->line[i]) {
            drawCloth(
                line->line[i],
                x + i * TILE_WIDTH, 
                y
            );
            i += line->line[i]->size - 1;
        }
    }
}

void drawLines() {
    drawLine(&outsideLine, LEFT_MARGIN, OUTSIDE_LINE_POSITION);
    drawLine(&insideLine, LEFT_MARGIN, INSIDE_LINE_POSITION);

    for (u32 i = LEFT_MARGIN; i < 320; i += 8) {
        drawSprite(OUTSIDE_LINE, i, OUTSIDE_LINE_POSITION, 1);
        drawSprite(ROOF_SPRITE, i, ROOF_POSITION, 1);
        drawSprite(INSIDE_LINE, i, INSIDE_LINE_POSITION, 1);
    }
}

bool hangCloth(u32 lineId, u32 x, Cloth* cloth) {
    Cloth** line;
    if (lineId == 0) {
        line = outsideLine.line;
    } else {
        line = insideLine.line;
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
        line = outsideLine.line;
    } else {
        line = insideLine.line;
    }

    if (line[x] && line[x]->dryingState <= DRYING_DRY) {
        return line[x];
    } else {
        return 0;
    }
}

static void updateClothsOnLine(Line* line, Weather weather) {
    u32 i = 0;
    // First pass - resize the cloths.
    while (i < line->length) {
        if (line->line[i]) {
            u32 oldSize = line->line[i]->size;
            updateCloth(line->line[i], weather);
            i += oldSize;
        } else {
            i++;
        }
    }

    // Second pass - shuffle them along with the change in size and dump any that fell off.
    u32 iSource = 0;
    u32 iDest = 0;
    Cloth* lastCloth = 0;

    Cloth* temp[line->length];
    memcpy(temp, line->line, sizeof(Cloth*) * line->length);
    memset(line->line, 0x00, sizeof(Cloth*) * line->length);

    while (iSource < line->length) {
        // Copy the cloths back to the line, but at the new size.
        if (temp[iSource] && lastCloth != temp[iSource]) {
            lastCloth = temp[iSource];

            // If cloth can't fit on the line, it falls and becomes dirty.
            if ((iDest + lastCloth->size) > line->length) {
                lastCloth->dryingState = DRYING_DIRTY;
            } else {
                for (u32 j = 0; j < lastCloth->size; j++) {
                    line->line[iDest] = lastCloth;
                    iDest++;
                }
            }
        } else {
            iDest++;
        }

        iSource++;
    }



}

void updateHangingCloths(Weather weather) {
    updateClothsOnLine(&outsideLine, weather);

    // Inside is always considered 'cloudy'
    updateClothsOnLine(&insideLine, WEATHER_CLOUDY);
}