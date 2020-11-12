#ifndef TEXT_INCLUDED
#define TEXT_INCLUDED

#include "core.h"
#include "resources.h"
#include <libdragon.h>

typedef enum {
    BASE_CLOTH_SPRITE,
    DRY_SPRITE,
    DAMP_SPRITE,
    SPUN_SPRITE,
    DRENCHED_SPRITE,
    DIRTY_SPRITE,

    SUN_SPRITE,
    CLOUD_SPRITE,
    RAIN_SPRITE,
    STORM_SPRITE,

    ROOF_SPRITE,
    tbc_SPRITE,
    LINE_SPRITE,
    BG_SPRITE,

    HANG_SPRITE,
    HANG_ANIM_1,
    TAKE_SPRITE,
    TAKE_ANIM_1,

    CURSOR_TOP_SPRITE,
    CURSOR_LEFT_SPRITE,
    CURSOR_BOTTOM_SPRITE,
    CURSOR_RIGHT_SPRITE,
    CURSOR_TOP_LEFT_SPRITE,
    CURSOR_TOP_RIGHT_SPRITE,
    CURSOR_BOTTOM_LEFT_SPRITE,
    CURSOR_BOTTOM_RIGHT_SPRITE,

    CURSOR_OVERLAY,

    QUEUED_GREEN_SPRITE,
    QUEUED_AMBER_SPRITE,
    QUEUED_RED_SPRITE,

    SUN_BG,
    CLOUD_BG,
    RAIN_BG,
    STORM_BG,
    INSIDE_BG,
    INSIDE_SPRITE,

    D_SPRITE,
    C_SPRITE,
    L_SPRITE,
    R_SPRITE,
    A_SPRITE,

    LINEAR_GROW_SPRITE,
    LINEAR_SHRINK_SPRITE,

    GROWTH_2_SPRITE,
    GROWTH_3_SPRITE,
    GROWTH_4_SPRITE,

    TIMER_SPRITE_1 = 64,
    TIMER_SPRITE_2,
    TIMER_SPRITE_3,
    TIMER_SPRITE_4,
    TIMER_SPRITE_5,
    TIMER_SPRITE_6,
    TIMER_SPRITE_7,
    TIMER_SPRITE_8,
    TIMER_SPRITE_9
} SpriteCode;

/**
 * Initialises text subsystem by loading sprites etc.
 */
s32 initText();

/**
 * Frees resources used by the text subsystem when done.
 */
void freeText();

/**
 * Draws a horizontal string of text starting at the given location.
 * @param frame The id of the frame to draw on.
 * @param x The x co-ordinate to start the string at.
 * @param y The y co-ordinate to start the string at.
 * @param scale size of the text sprites.
 */
void drawText(const string text, const u32 x, const u32 y, const float scale);

/**
 *
 */ 
void drawSprite(const SpriteCode spriteCode, const u32 x, const u32 y, const float scale);

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
);

void drawBox(SpriteCode texture, u32 x, u32 y, u32 width, u32 height);


#endif
