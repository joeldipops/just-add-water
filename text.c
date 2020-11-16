#include "core.h"
#include "text.h"
#include "resources.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <libdragon.h>

static bool textInitted = false;

static const u32 SPRITE_SIZE = 16;

/**
 * Initialises text subsystem by loading sprites etc.
 * @return 0 result code
 ** 0   success
 ** -1  expected file not present
 */
s32 initText() {
    if (textInitted) {
        return 0;
    }

    s32 result = initResources();
    if (result) {
        return result;
    } else {
        textInitted = true;
        return result;
    }
}

/**
 * Frees resources used by the text subsystem when done.
 */
void freeText() {
    //free(_strings);
    textInitted = false;
}

/**
 * Reads a number from a substring, stopping after a certain number of characters.
 * @param start pointer to the start of the number in the string.
 * @param maxLength max length of the number as characters.
 * @param base base of the number eg. 10, 16
 * @return The parsed byte.
 */
u8 parseByte(const char* start, const u8 maxLength, const u8 base) {
    // sprite is 2 digit hex, we need to parse it from the string.
    string code = {};
    memcpy(code, start, maxLength);
    char* end = 0;
    return strtol(code, &end, base);
}

/**
 * Draws a sprite from a sheet at the given location & scale.
 * @param spriteCode id of the sprite on the sheet.
 * @param spriteSheet the sheet of sprites.
 * @param x The x screen position.
 * @param y The y screen position.
 * @param scale Scale the sprite.
 * @private
 */
static void draw(const u32 spriteCode, sprite_t* spriteSheet, const u32 x, const u32 y, const float scale) {
    rdp_load_texture_stride(0, 0, MIRROR_DISABLED, spriteSheet, spriteCode);
    rdp_draw_sprite_scaled(0, x, y, scale, scale, MIRROR_DISABLED);
}


/*
void drawSprite(const SpriteCode spriteCode, const u32 x, const u32 y, const float scale) {
    sprite_t* sheet;
    u32 code = spriteCode;
    if (spriteCode >= TIMER_SPRITE_1) {
        sheet = getTimerSheet();
        code = spriteCode - TIMER_SPRITE_1;
    } else {
        sheet = getSpriteSheet();
    }
    draw(code, sheet, x, y, scale);
}*/



/**
 * Draws a text character from the sprite sheet at a given location with the given transformation.
 * @param character the ASCII character to draw
 * @param x The x co-ordinate to draw at.
 * @param y The y co-ordinate to draw at.
 * @param scale size of the image
 * @param transformation Flip/Fade/Shift etc the character.
 * @private
 */
static void drawTransformedCharacter(const char character, const u32 x, const u32 y, const float scale, const Transformation transformation) {
    // Avoid printing any control characters, we don't at this point know what
    // wackiness will ensue.
    if (character <= 0x20) {
        return;
    }

    sprite_t* sheet = getCharacterSheet();

    u32 offset = character - 0x20;
    if (transformation) {
        sheet = transformSprite(sheet, offset, transformation);
        offset = 0;
    }

    draw(offset, sheet, x, y, scale);
}

/**
 * Draws a text character from the sprite sheet at a given location.
 * @param character the ASCII character to draw
 * @param x The x co-ordinate to draw at.
 * @param y The y co-ordinate to draw at.
 * @param scale size of the image
 * @private
 */
static void drawCharacter(const char character, const u32 x, const u32 y, const float scale) {
    drawTransformedCharacter(character, x, y, scale, 0);
}

/**
 * Draws an image from a sprite sheet, as specified by a token in the string, rotating if requested.
 * @param text the string containing the image to draw.
 * @param textIndex the position in text that of the token.
 * @param length Length of the string to avoid recalculating each iteration.
 * @param x The x co-ordinate to draw at.
 * @param y The y co-ordinate to draw at.
 * @param scale size of the image
 * @return
 ** next index of the string after the token if positive.
 ** error code if negative
 *** -1 token is not complete.
 *** -2 badly formatted token.
 * @private
 */
static s32 drawImage(const string text, const u32 textIndex, const u32 length, const u32 x, const u32 y, const float scale) {
    u32 i = textIndex;
    char transformation = text[i+1];
    if (length <= i + 2) {
        return -1;
    } else if (
        transformation == ROTATE_90
        || transformation == ROTATE_180
        || transformation == ROTATE_270
        || transformation == FLIP_HORIZONTAL
        || transformation == FLIP_VERTICAL
    ) {
        // Flip/Rotation specifier optionally follows the $ sign.
        if (length <= i + 3) {
            return -1;
        }
        i++;
    } else {
        transformation = 0;
    }

    // sprite is 2 digit hex, we need to parse it from the string
    u32 spriteCode = parseByte(&text[i+1], 2, 16);

    sprite_t* sheet = getSpriteSheet();
    if (transformation) {
        sheet = transformSprite(sheet, spriteCode, transformation);
        spriteCode = 0;
    }

    draw(spriteCode, sheet, x, y, ceil(scale));
    i += 2;
    return i;
}

/**
 * Draws characters in a line.
 * @param x The x co-ordinate to start the string at.
 * @param y The y co-ordinate to start the string at.
 * @param scale size of the text sprites.
 * @return result code
 ** 0   success
 ** -1  token is not complete.
 * @private
 */
static s32 drawTextLine(const string text, const u32 x, const u32 y, const float scale) {
    u32 length = strlen(text);
    u32 left = x;
    Transformation transform = 0;

    u32 i = 0;

    // If the first character is a ~, fade the whole line.
    if (text[0] == '~') {
        transform = FADE;
        i++;
    }

    for (; i < length; i++) {
        // $ token means draw a sprite instead of a text character.
        if (text[i] == '$') {
            s32 result = drawImage(text, i, length, left, y, scale);
            if (result < 0) {//
                return result;
            } else {
                i = result;
            }
            left += SPRITE_SIZE * ceil(scale);
            continue;
        }
        // Do literal draw of whatever follows slash.
        if (text[i] == '\\') {
            i++;
        }
        drawTransformedCharacter(text[i], left, y, scale, transform);
        left += CHARACTER_SIZE * scale;
    }
    return 0;
}


/**
 * Draws a horizontal string of text starting at the given location.
 * @param frame The id of the frame to draw on.
 * @param x The x co-ordinate to start the string at.
 * @param y The y co-ordinate to start the string at.
 * @param scale size of the text sprites.
 */
void drawText(const string text, const u32 x, const u32 y, const float scale) {
    if (!textInitted) {
        initText();
    }
    drawTextLine(text, x, y, scale);
}

/**
 * Draws a horizontal string of text starting at the given location.
 * @param frame The id of the frame to draw on.
 * @param x The x co-ordinate to start the string at.
 * @param y The y co-ordinate to start the string at.
 * @param scale size of the text sprites.
 * @param width width of the area available for text.
 */
void drawTextParagraph(
    const string text,
    const u32 x,
    const u32 y,
    const float scale,
    const u32 width
) {

    u32 top = y;

    u32 stringLength = strlen(text);
    u32 maxLength = width / (CHARACTER_SIZE * scale);

    u32 i = 0;
    while(i < stringLength) {
        u32 lineAvailable = 0;
        lineAvailable = (stringLength - i < maxLength) ? stringLength - i : maxLength;

        // Split on spaces.
        u32 lineBreak = lineAvailable;
        for(u32 j = 0; j < lineAvailable; j++) {
            if (text[i + j] == '$') {
                // 3 characters, but only takes up 1 space.
                j += 2;
                lineAvailable += 2;
            } else if (text[i + j] == '\\') {
                // 2 characters, 1 space.
                j += 1;
                lineAvailable += 1;
            }
            if (0x20 == *(text + i + j)) {
                lineBreak = j;
            }
        }

        // if we're don't need any more lines, just run to the end.
        if (i + lineAvailable >= stringLength) {
            lineBreak = lineAvailable;
        } else {
            // make sure if we end on one of these, we keep the whole token.
            if (text[lineBreak] == '$') {
                lineBreak += 2;
            } else if (text[lineBreak-1] == '$') {
                lineBreak += 1;
            } else if (text[lineBreak] == '\\') {
                lineBreak += 1;
            }

            // absorb spaces into the newline
            while(lineBreak < stringLength && *(text + i + lineBreak) == 0x20) {
                lineBreak++;
            }
        }

        string line = "";
        memcpy(line, text + i, lineBreak);

        drawTextLine(line, x, top, scale);

        i += lineBreak;
        top += CHARACTER_SIZE;
    }
}

void drawBox(SpriteCode texture, u32 x, u32 y, u32 width, u32 height) {
    rdp_load_texture_stride(0, 0, MIRROR_DISABLED, getSpriteSheet(), texture);
    rdp_draw_textured_rectangle(0, x, y, x + width, y + height - 1, MIRROR_DISABLED);
}
