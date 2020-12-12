#include "text.h"
#include "config.h"
#include <libdragon.h>

typedef struct {
    u32 x;
    u32 y;
    SpriteCode spriteId;
    float xScale;
    float yScale;
} Sprite;

typedef struct {
    Sprite list[SPRITE_COUNT][64];
    u32 index[SPRITE_COUNT];
} SpriteList;

#define MAX_PRIORITY 4

static SpriteList drawLists[MAX_PRIORITY];

void resetRenderer() {
    for(u32 z = 0; z < MAX_PRIORITY; z++) {
        memset(&drawLists[z].index, 0, sizeof(drawLists[z].index));
    }
}

void drawScaledSprite(SpriteCode spriteId, u32 x, u32 y, u32 z, float xScale, float yScale) {
    u32 index = drawLists[z].index[spriteId];
    Sprite* sprite = &drawLists[z].list[spriteId][index];
    sprite->x = x;
    sprite->y = y;
    sprite->xScale = xScale;
    sprite->yScale = yScale;

    drawLists[z].index[spriteId]++;
};

/**
 * @param z priority 0 - MAX_PRIORITY
 */
void drawSprite(SpriteCode spriteId, u32 x, u32 y, u32 z, float scale) {
    drawScaledSprite(spriteId, x, y, z, scale, scale);
}

void renderSprites() {
    for (u32 z = 0; z < MAX_PRIORITY; z++) {
        for (u32 spriteId = 0; spriteId < SPRITE_COUNT; spriteId++) {
            if (!drawLists[z].index[spriteId]) {
                continue;
            }
            sprite_t* sheet;
            u32 adjustedCode = spriteId;
            if (spriteId >= TIMER_SPRITE_1) {
                sheet = getTimerSheet();
                adjustedCode = spriteId - TIMER_SPRITE_1;
            } else {
                sheet = getSpriteSheet();
            }

            rdp_load_texture_stride(0, 0, MIRROR_DISABLED, sheet, adjustedCode);

            for (u32 i = 0; i < drawLists[z].index[spriteId]; i++) {
                Sprite* sprite = &drawLists[z].list[spriteId][i];
                rdp_draw_sprite_scaled(0, sprite->x, sprite->y, sprite->xScale, sprite->yScale, MIRROR_DISABLED);
            }
        }
    }


    resetRenderer();
}