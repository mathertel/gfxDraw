// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawText.h: Header file for functions to display text.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// These pixel oriented drawing functions are implemented to use callback functions for the effective drawing
// to make them independent from an specific canvas or GFX implementation and can be used for drawing and un-drawing.
//
// Changelog:
// * 16.12.2024 creation
//
// - - - - -

#pragma once

#include "gfxDraw.h"
#include "gfxfont.h"

namespace gfxDraw {

void setupFont();

void loadFont(const char *fName);

void addFont(const GFXfont *newFont);

/// @brief calculate the Bounding box of a text drawn at 0/0
/// @param h font height
/// @param text the text.
/// @return Dimension of the text with box.x and .y == 0.
Point textBox(int16_t h, const char *text);


/// @brief calculate the defined lineHeight 
/// @param size Size of font to be used or 0 to use the last font size
/// @return total lineHeight
int16_t lineHeight(int16_t size = 0);


/// @brief Draw a text
/// @param p1 Starting Point
/// @param size Text size
/// @param text character to print
/// @param cbDraw Callback with coordinates of line pixels.
/// @return Text cursor position after drawing.
Point drawText(Point &p, int16_t size, const char *text, fSetPixel cbDraw);


/// @brief Draw a text
/// @param x Starting Point X coordinate.
/// @param y Starting Point Y coordinate.
/// @param size Text size
/// @param text character to print
/// @return Text cursor position after drawing.
Point drawText(int16_t x, int16_t y, int16_t size, const char *text, fSetPixel cbDraw);


}  // gfxDraw:: namespace


// End.
