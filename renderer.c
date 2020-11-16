#include "text.h"
#include "config.h"
#include <libdragon.h>

typedef struct {
    u32 x;
    u32 y;
    SpriteCode spriteId;
    float scale;
} Sprite;

static Sprite drawList[SPRITE_COUNT][64];
static u32 drawIndex[SPRITE_COUNT];

void resetRenderer() {
    memset(&drawIndex, 0, sizeof(drawIndex));
}

void drawSprite(SpriteCode spriteId, u32 x, u32 y, float scale) {
    u32 index = drawIndex[spriteId];
    Sprite* sprite = &drawList[spriteId][index];
    sprite->x = x;
    sprite->y = y;
    sprite->scale = scale;

    drawIndex[spriteId]++;
}

void renderSprites() {
    for (u32 spriteId = 0; spriteId < SPRITE_COUNT; spriteId++) {
        sprite_t* sheet;
        u32 adjustedCode = spriteId;
        if (spriteId >= TIMER_SPRITE_1) {
            sheet = getTimerSheet();
            adjustedCode = spriteId - TIMER_SPRITE_1;
        } else {
            sheet = getSpriteSheet();
        }
        rdp_load_texture_stride(0, 0, MIRROR_DISABLED, sheet, adjustedCode);

        for (u32 i = 0; i < drawIndex[spriteId]; i++) {
            Sprite sprite = drawList[spriteId][i];
            rdp_draw_sprite_scaled(0, sprite.x, sprite.y, sprite.scale, sprite.scale, MIRROR_DISABLED);
        }
    }

    resetRenderer();
}