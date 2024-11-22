// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawRect.h: Header file for functions to calculate all points of a circle, circle quadrant and circle segment.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// These pixel oriented drawing functions are implemented to use callback functions for the effective drawing
// to make them independent from an specific canvas or GFX implementation and can be used for drawing and un-drawing.
//
// The functions have minimized use of float and arc arithmetics.
// Path drawing is supporting on any given path.
// Filled paths are supported on closed paths only.
//
// Changelog:
// * 22.05.2024 creation 
// * 01.11.2024 drawing rectangles with optional rounded corners
//
// - - - - -

#pragma once

#include "gfxDrawCommon.h"

namespace gfxDraw {

/// ===== Basic draw functions with callback =====

/// @brief Draw the border line of a rectangle and optionally fill pixels.
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, fSetPixel cbBorder = nullptr, fSetPixel cbFill = nullptr);


/// @brief Draw a rounded rectangle with border and fill callbacks.
/// @param x0 Starting Point X coordinate.
/// @param y0 Starting Point Y coordinate.
/// @param w width of the rect in pixels
/// @param h height of the rect in pixels
/// @param radius corner radius
/// @param cbBorder Callback with coordinates of rect border pixels.
/// @param cbFill Callback with coordinates of rect fill pixels.
void drawRoundedRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, fSetPixel cbBorder = nullptr, fSetPixel cbFill = nullptr);


}  // gfxDraw:: namespace




// End.
