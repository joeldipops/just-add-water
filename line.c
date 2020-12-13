#include "line.h"
#include "core.h"
#include "cloth.h"
#include "weather.h"
#include "config.h"
#include "text.h"
#include "player.h"
#include "day.h"
#include "animation.h"
#include "clothManager.h"
#include <stdbool.h>
#include <string.h>


typedef struct {
    Cloth** cloths;
    s32 length;
} Line;

static Cloth* _outsideLine[LINE_SIZE];
static Cloth* _insideLine[LINE_SIZE];

static Line outsideLine;
static Line insideLine;

void initLine() {
    memset(_outsideLine, 0, sizeof(Cloth*) * LINE_SIZE);
    memset(_insideLine, 0, sizeof(Cloth*) * LINE_SIZE);
    outsideLine.cloths = _outsideLine;
    outsideLine.length = LINE_SIZE;

    insideLine.cloths = _insideLine;
    insideLine.length = LINE_SIZE;
}

static void drawLine(Line* line, s32 x, s32 y) {
    for (s32 i = 0; i < line->length; i++) {
        if (line->cloths[i]) {
            drawCloth(
                line->cloths[i],
                x + i * TILE_WIDTH,
                y
            );
            i += line->cloths[i]->size - 1;
        }
    }
}

void drawLines() {
    drawBox(LINE_SPRITE, LINES_MARGIN_LEFT, OUTSIDE_LINE_POSITION, LINE_SIZE * TILE_WIDTH, TILE_WIDTH);
    drawBox(LINE_SPRITE, LINES_MARGIN_LEFT, INSIDE_LINE_POSITION, LINE_SIZE * TILE_WIDTH, TILE_WIDTH);
    drawBox(ROOF_SPRITE, LINES_MARGIN_LEFT, ROOF_POSITION, LINE_SIZE * TILE_WIDTH, TILE_WIDTH);

    if (!isWaiting()) {
        drawLine(&outsideLine, LINES_MARGIN_LEFT, OUTSIDE_LINE_POSITION);
        drawLine(&insideLine, LINES_MARGIN_LEFT, INSIDE_LINE_POSITION);
    }
}

bool hangCloth(s32 lineId, s32 x, Cloth* cloth) {
    Cloth** cloths;
    if (lineId == 0) {
        cloths = outsideLine.cloths;
    } else {
        cloths = insideLine.cloths;
    }

    // If there's already a cloth where we want to put this new one
    for (s32 i = x; i < x + cloth->size; i++) {
        if (cloths[i]) {
            return false;
        }
    }

    // Otherwise hang the new cloth
    for (s32 i = x; i < x + cloth->size; i++) {
        cloths[i] = cloth;
    }

    return true;
}

static Cloth* takeClothFromLine(Line* line, s32 x) {
    if (line->cloths[x]) {
        Cloth* result = line->cloths[x];

        s32 firstTile = 0;

        // Remove from the line.
        for (s32 i = 0; i < line->length; i++) {
            if (line->cloths[i] == result) {
                if (!firstTile) {
                    firstTile = i;
                }
                
                line->cloths[i] = 0;
            }
        }

        result->grabPoint = x - firstTile;

        return result;
    } else {
        return 0;
    }
}

Cloth* takeCloth(s32 lineId, s32 x) {
    Cloth** line;
    if (lineId == 0) {
        return takeClothFromLine(&outsideLine, x);
    } else {
        return takeClothFromLine(&insideLine, x);
    }
}


static void dropCloth(Line* line, Cloth* cloth) {
    // Animate the cloth falling off the line.
    s32 animsNeeded = 10 + (cloth->size * 2);
    const s32 numberOfFrames = 16;
    Animation* animations[animsNeeded];
    for (s32 i = 0; i < animsNeeded; i++) {
        animations[i] = newAnimation(numberOfFrames);
    }

    s32 y = (line == &outsideLine) ? OUTSIDE_LINE_POSITION : INSIDE_LINE_POSITION;
    const s32 x = LINES_MARGIN_LEFT + ((LINE_SIZE - cloth->size) * TILE_WIDTH);

    for (s32 i = 0; i < numberOfFrames; i++) {
        setClothAnimationFrames(cloth, cloth->size * TILE_WIDTH, animations, i, x + (i % 2), y + i * 2);
    }

    for (s32 i = 0; i < animsNeeded; i++) {
        startAnimation(animations[i]);
    }

    // Cloths that fall off the line go back in to the queue, their size already changed.
    cloth->dryingState = cloth->initialDryingState;
    cloth->initialSize = cloth->size;
    cloth->oldSize = cloth->size;
    enqueueCloth(cloth);

    getPlayer()->dropped++;
}

/**
 * Grow or shrink the cloths based on the weather.
 */
void updateClothSize(Line* line, Weather weather) {
    s32 i = 0;
    while (i < line->length) {
        if (line->cloths[i]) {
            s32 oldSize = line->cloths[i]->size;
            updateCloth(line->cloths[i], weather);
            prepareClothAnimation(
                line->cloths[i],
                LINES_MARGIN_LEFT + i * TILE_WIDTH,
                line == &outsideLine ? OUTSIDE_LINE_POSITION : INSIDE_LINE_POSITION
            );
            i += oldSize;
        } else {
            i++;
        }
    }
}

/**
 * Shuffle clothes along the line now that they have a new size, and dump any that fell off.
 */
void updateClothPosition(Line* line) {
    s32 iSource = 0;
    s32 iDest = 0;
    Cloth* lastCloth = 0;

    Cloth* temp[line->length];
    memcpy(temp, line->cloths, sizeof(Cloth*) * line->length);
    memset(line->cloths, 0x00, sizeof(Cloth*) * line->length);

    while (iSource < line->length) {
        // Copy the cloths back to the line, but at the new size.
        if (temp[iSource] && lastCloth != temp[iSource]) {
            lastCloth = temp[iSource];

            while(line->cloths[iDest]) {
                iDest++;
            }

            // If cloth can't fit on the line, it falls and becomes dirty.
            if ((iDest + lastCloth->size) > line->length) {
                dropCloth(line, lastCloth);
            } else {
                for (s32 j = 0; j < lastCloth->size; j++) {
                    line->cloths[iDest] = lastCloth;
                    iDest++;
                }
                iDest = iSource + 1;
            }
        } else {
            iDest++;
        }

        iSource++;
    }
}


void updateHangingClothSize(Weather weather) {
    updateClothSize(&outsideLine, weather);

    // Inside is always considered 'cloudy'
    updateClothSize(&insideLine, WEATHER_CLOUDY);
}

void updateHangingClothPosition() {
    updateClothPosition(&outsideLine);
    updateClothPosition(&insideLine);
}

static void updateClothsOnLine(Line* line, Weather weather) {
    updateClothSize(line, weather);
    updateClothPosition(line);
}

void updateHangingCloths(Weather weather) {
    updateClothsOnLine(&outsideLine, weather);

    // Inside is always considered 'cloudy'
    updateClothsOnLine(&insideLine, WEATHER_CLOUDY);
}