#include "core.h"
#include "resources.h"
#include <libdragon.h>
#include <math.h>


static const u32 FADE_FACTOR = 0x0A;

static sprite_t* _textMap = 0;
static sprite_t* _spriteSheet = 0;
static sprite_t* _timerSheet = 0;
static bool resourcesInitted = false;

typedef struct {
    u32 SheetIndex;
    u32 SpriteCode;
    Transformation Transformation;
    sprite_t* Sprite;
    u32 Position;
} SpriteLookup;

#define TRANSFORM_CACHE_SIZE 127
static SpriteLookup _transformCache[TRANSFORM_CACHE_SIZE] = {};

/**
 * Fade/Brighten a colour by a given amount.
 * @param colour The 16bit colour
 * @param fadeAmount This amount will be added to each component.
 * @returns The new faded colour.
 */
u16 fadeColour(const u16 colour, const u32 fadeAmount) {
    u8 red = colour >> 11;
    u8 green = (colour >> 6) & 0x1F;
    u8 blue = (colour >> 1) & 0x1F;
    u8 t = colour & 1;

    // Find how much space we have to fade.
    u8 room[3] = {
        0x1F - red,
        0x1F - green,
        0x1F - blue
    };

    u8 factor = fadeAmount;
    for (u32 i = 0; i < 3; i++) {
        factor = factor < room[i] ? factor : room[i];
    }

    red = red + factor;
    green = green + factor;
    blue = blue + factor;

    return (red << 11) | (green << 6) | (blue << 1) | t;
}

/**
 * Gets the sprite from the cache if it exists.
 * @param sheetIndex Identifies the sprite sheet the sprite is in.
 * @param spriteCode Identifies the source sprite on the sheet.
 * @param transformation How the source sprite was tranformed.
 * @return pointer to sprite in the cache, or 0 if not in the cache.
 * @private
 */
static sprite_t* getFromCache(const u32 sheetIndex, const u32 spriteCode, const Transformation transformation) {
    sprite_t* result = 0;
    s32 oldPosition = -1;
    for (u32 i = 0; i < TRANSFORM_CACHE_SIZE; i++) {
        if (
            sheetIndex == _transformCache[i].SheetIndex
            && spriteCode == _transformCache[i].SpriteCode
            && transformation == _transformCache[i].Transformation
        ) {
            oldPosition = _transformCache[i].Position;
            _transformCache[i].Position = TRANSFORM_CACHE_SIZE;
            result = _transformCache[i].Sprite;
        }
    }

    // If the sprite was in the cache, put it back on the end of the queue, and push everything that was behind it down one.
    if (result) {
        for (u32 i = 0; i < TRANSFORM_CACHE_SIZE; i++) {
            if (_transformCache[i].Position > oldPosition) {
                _transformCache[i].Position--;
            }
        }
    }

    return result;
}

/**
 * Adds a sprite to the cache.
 * @param lookup The sprite along with information used to index/identify it.
 * @private
 */
static void cacheSprite(SpriteLookup* lookup) {
    u32 cacheTop = 0;

    // Find any slot with position 0
    while(_transformCache[cacheTop].Position) {
        cacheTop++;
    }

    // If the cache is full, free up some memory
    if (_transformCache[cacheTop].Sprite) {
        free(_transformCache[cacheTop].Sprite->data);
        free(_transformCache[cacheTop].Sprite);
        _transformCache[cacheTop].Sprite = 0;
    }

    // Add to the cache.
    _transformCache[cacheTop].Sprite = lookup->Sprite;
    _transformCache[cacheTop].SheetIndex = lookup->SheetIndex;
    _transformCache[cacheTop].SpriteCode = lookup->SpriteCode;
    _transformCache[cacheTop].Transformation = lookup->Transformation;
    _transformCache[cacheTop].Position = TRANSFORM_CACHE_SIZE;

    // Shift everything down by 1 so as things are added they get closer to 0.
    for (u32 i = 0; i < TRANSFORM_CACHE_SIZE; i++) {
        if (_transformCache[i].Position > 0) {
            _transformCache[i].Position--;
        }
    }
}

static sprite_t* loadSheet(char* fileName) {
    // Read in character sprite sheet.
    s32 filePointer = dfs_open(fileName);
    if (!filePointer) {
        return 0;
    }
    sprite_t* result = malloc(dfs_size(filePointer));
    dfs_read(result, 1, dfs_size(filePointer), filePointer);
    dfs_close(filePointer);

    return result;
}

/**
 * Loads oft-used resources in to memory.
 * @return success/error code
 ** 0   initted successfully
 ** -1  could not find expected resource file.
 */
s32 initResources() {
    if (resourcesInitted) {
        return 0;
    }

    _textMap = loadSheet("/font.sprite");
    _spriteSheet = loadSheet("/sprites.sprite");
    _timerSheet = loadSheet("/timer.sprite");

    for (u32 i = 0; i < TRANSFORM_CACHE_SIZE; i++) {
        _transformCache[i].Sprite = 0;
    }

    resourcesInitted = true;
    return 0;
}

/**
 * Gets sprite sheet of textual characters.
 * @return pointer to sprite sheet.
 */
sprite_t* getCharacterSheet() {
    return _textMap;
}

sprite_t* getTimerSheet() {
    return _timerSheet;
}

/**
 * Gets sprite sheet of icons and textures.
 * @return pointer to sprite sheet.
 */
sprite_t* getSpriteSheet() {
    return _spriteSheet;
}

/**
 * Takes an existing sprite, transforms it, and stashes it in a cache for next time it's needed.
 * @param sheet Source sprite sheet.
 * @param spriteCode Identifies source sprite on the sheet.
 * @param transformation How the sprite should be transformed.
 * @return Pointer to the new transformed sprite.
 */
sprite_t* transformSprite(const sprite_t* sheet, const u32 spriteCode, const Transformation transformation) {
    u32 sheetIndex = (sheet == _textMap) ? 0 : 1;

    // Get from cache if we already have it.
    sprite_t* result = getFromCache(sheetIndex, spriteCode, transformation);
    if (result) {
        return result;
    }

    u32 spriteWidth = sheet->width / sheet->hslices;
    u32 spriteHeight = sheet->height / sheet->vslices;

    u32* data = calloc(spriteHeight * spriteWidth, sheet->bitdepth);

    u32 x = spriteCode % sheet->hslices;
    u32 y = floor(spriteCode / sheet->hslices);

    u32* source = calloc(sheet->height * sheet->width, sheet->bitdepth);
    memcpy(source, sheet->data, sheet->height * sheet->width * sheet->bitdepth);

    u32 index = 0;
    u32 destRow = 0;
    u32 destColumn = 0;

    switch(transformation) {
        case FADE:
            for (u32 row = y * spriteHeight; row < y * spriteHeight + spriteHeight; row++) {
                for (u32 column = x * spriteWidth; column < x * spriteWidth + spriteWidth; column++) {
                    switch(sheet->bitdepth) {
                        case 2: ;
                            u32 colour = source[(row * sheet->width + column) * sheet->bitdepth] << 8 | source[(row * sheet->width + column) * sheet->bitdepth + 1]; 
                            colour = fadeColour(colour, FADE_FACTOR);

                            data[index] = colour >> 8;
                            data[index + 1] = colour & 0xFF;

                            break;

                        default: 
                            // Not implemented, so perform straight copy.
                            memcpy(
                                data + index,
                                source + (row * sheet->width + column) * sheet->bitdepth,
                                sheet->bitdepth
                            );
                            break;
                    }

                    index += sheet->bitdepth;
                }
            }
            break;
        case ROTATE_90:
            destRow = spriteHeight * sheet->bitdepth - sheet->bitdepth;
            for (u32 sourceRow = y * spriteHeight; sourceRow < y * spriteHeight + spriteHeight; sourceRow++) {
                destColumn = 0;
                for (u32 sourceColumn = x * spriteWidth; sourceColumn < x * spriteWidth + spriteWidth; sourceColumn++) {
                    memcpy(
                        data + destColumn * spriteWidth + destRow,
                        source + (sourceRow * sheet->width + sourceColumn) * sheet->bitdepth,
                        sheet->bitdepth
                    );

                    destColumn += sheet->bitdepth;
                }
                destRow -= sheet->bitdepth;
            }
        break;
        case ROTATE_180:
            destRow = spriteHeight * sheet->bitdepth - sheet->bitdepth;
            for (u32 sourceRow = y * spriteHeight; sourceRow < y * spriteHeight + spriteHeight; sourceRow++) {
                destColumn = spriteWidth * sheet->bitdepth - sheet->bitdepth;
                for (u32 sourceColumn = x * spriteWidth; sourceColumn < x * spriteWidth + spriteWidth; sourceColumn++) {
                    memcpy(
                        data + destRow * spriteWidth + destColumn,
                        source + (sourceRow * sheet->width + sourceColumn) * sheet->bitdepth,
                        sheet->bitdepth
                    );
                    destColumn -= sheet->bitdepth;
                }
                destRow -= sheet->bitdepth;
            }
        break;
        case ROTATE_270:
            destRow = 0;
            for (u32 sourceRow = y * spriteHeight; sourceRow < y * spriteHeight + spriteHeight; sourceRow++) {
                destColumn = spriteWidth * sheet->bitdepth - sheet->bitdepth;
                for (u32 sourceColumn = x * spriteWidth; sourceColumn < x * spriteWidth + spriteWidth; sourceColumn++) {
                    memcpy(
                        data + destColumn * spriteWidth + destRow,
                        source + (sourceRow * sheet->width + sourceColumn) * sheet->bitdepth,
                        sheet->bitdepth
                    );
                    destColumn -= sheet->bitdepth;
                }
                destRow += sheet->bitdepth;
            }
        break;
        case FLIP_HORIZONTAL:
            destRow = 0;
            for (u32 sourceRow = y * spriteHeight; sourceRow < y * spriteHeight + spriteHeight; sourceRow++) {
                destColumn = spriteWidth * sheet->bitdepth - sheet->bitdepth;
                for (u32 sourceColumn = x * spriteWidth; sourceColumn < x * spriteWidth + spriteWidth; sourceColumn++) {
                    memcpy(
                        data + destRow * spriteWidth + destColumn,
                        source + (sourceRow * sheet->width + sourceColumn) * sheet->bitdepth,
                        sheet->bitdepth
                    );

                    destColumn -= sheet->bitdepth;
                }
                destRow += sheet->bitdepth;
            }
        break;
        case FLIP_VERTICAL:
            destRow = spriteHeight * sheet->bitdepth - sheet->bitdepth;
            for (u32 sourceRow = y * spriteHeight; sourceRow < y * spriteHeight + spriteHeight; sourceRow++) {
                destColumn = 0;
                for (u32 sourceColumn = x * spriteWidth; sourceColumn < x * spriteWidth + spriteWidth; sourceColumn++) {
                    memcpy(
                        data + destRow * spriteWidth + destColumn,
                        source + (sourceRow * sheet->width + sourceColumn) * sheet->bitdepth,
                        sheet->bitdepth
                    );

                    destColumn += sheet->bitdepth;
                }
                destRow -= sheet->bitdepth;
            }
        break;
        default:
            // Straight copy
            for (u32 row = y * spriteHeight; row < y * spriteHeight + spriteHeight; row++) {
                for (u32 column = x * spriteWidth; column < x * spriteWidth + spriteWidth; column++) {
                    memcpy(
                        data + index,
                        source + (row * sheet->width + column) * sheet->bitdepth,
                        sheet->bitdepth
                    );

                    index += sheet->bitdepth;
                }
            }
    }

    result = calloc(1, sizeof(sprite_t) + spriteHeight * spriteWidth * sheet->bitdepth);
    result->width = spriteWidth;
    result->height = spriteHeight;
    result->vslices = 1;
    result->hslices = 1;
    result->bitdepth = sheet->bitdepth;
    result->format = sheet->format;
    memcpy(result->data, data, result->width * result->height * result->bitdepth);

    // Put the newly generated sprite in to the cache.
    SpriteLookup cacheable;
    cacheable.Sprite = result;
    cacheable.SheetIndex = sheetIndex;
    cacheable.SpriteCode = spriteCode;
    cacheable.Transformation = transformation;
    cacheSprite(&cacheable);

    free(source);
    source = 0;
    free(data);
    data = 0;

    return result;
}

/**
 * Releases memory held by all resources. only rotate
 */
void freeResources() {
    free(_textMap);
    _textMap = 0;
    free(_spriteSheet);
    _spriteSheet = 0;

    emptyResourceCache();
    resourcesInitted = false;
}

/**
 * Frees up the cache but leaves the resources subsystem initialised.
 */
void emptyResourceCache() {
    for (u32 i = 0; i < TRANSFORM_CACHE_SIZE; i++) {
        if (_transformCache[i].Sprite) {
            free(_transformCache[i].Sprite->data);
            free(_transformCache[i].Sprite);
            _transformCache[i].Sprite = 0;
        }
    }
}
