#ifndef RESOURCES_INCLUDED
#define RESOURCES_INCLUDED
#include <libdragon.h>
#include <stdlib.h>
#include <string.h>
#include "core.h"

typedef enum {
    ROOF_SPRITE,
    DRY_SPRITE,
    DRENCHED_SPRITE,
    SPUN_SPRITE,
    DAMP_SPRITE,
    RAIN_SPRITE,
    SUN_SPRITE, 
    CLOUD_SPRITE,
    INSIDE_LINE,
    OUTSIDE_LINE,
    HANG_SPRITE,
    TAKE_SPRITE
} SpriteCode;

typedef enum {
    ROTATE_90 = '>',
    ROTATE_180 = 'v',
    ROTATE_270 = '<',
    FLIP_HORIZONTAL = 'W',
    FLIP_VERTICAL = 'V',
    FADE = '~'
} Transformation;


/**
 * Loads oft-used resources in to memory.
 * @return success/error code
 ** 0   initted successfully
 ** -1  could not find expected resource file.
 */
s32 initResources();

/**
 * Releases memory held by all resources.
 */
void freeResources();

/**
 * Frees up the cache but leaves the resources subsystem initialised.
 */
void emptyResourceCache();

/**
 * Gets sprite sheet of textual characters.
 * @return pointer to sprite sheet.
 */
sprite_t* getCharacterSheet();

/**
 * Gets sprite sheet of icons and textures.
 * @return pointer to sprite sheet.
 */
sprite_t* getSpriteSheet();

/**
 * Takes an existing sprite, transforms it, and stashes it in a cache for next time it's needed.
 * @param sheet Source sprite sheet.
 * @param spriteCode Identifies source sprite on the sheet.
 * @param rotation How the sprite should be transformed.
 * @return Pointer to the new transformed sprite.
 */
sprite_t* transformSprite(const sprite_t* sheet, const u32 spriteCode, const Transformation transformation);

#endif
