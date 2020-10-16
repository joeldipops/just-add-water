#ifndef TEXT_INCLUDED
#define TEXT_INCLUDED

#include "core.h"
#include <libdragon.h>

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
void drawText(const display_context_t frame, const string text, const u32 x, const u32 y, const float scale);

/**
 * Draws a horizontal string of text starting at the given location.
 * @param frame The id of the frame to draw on.
 * @param x The x co-ordinate to start the string at.
 * @param y The y co-ordinate to start the string at.
 * @param scale size of the text sprites.
 * @param width width of the area available for text.
 */
void drawTextParagraph(
    const display_context_t frame,
    const string text,
    const u32 x,
    const u32 y,
    const float scale,
    const u32 width
);

/**
 * Calculates the length in pixels of a given string, including any sprites.
 * @param text the string.
 * @return the length in pixels.
 */
u32 getStringWidth(const string text);

#endif
