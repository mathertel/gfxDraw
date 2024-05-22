// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxdrawCircle.h: Header file for circle drawing functions
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
// Changelog: See gfxdraw.h and documentation files in this library.
//
// - - - - -

#pragma once

#include "gfxDrawCommon.h"

namespace gfxDraw {

/// ===== Basic draw functions with callback =====

void drawCircle(Point center, int16_t r, fSetPixel cbDraw);

void drawCircle(Point center, int16_t radius, int16_t startAngle, int16_t endAngle, bool clockwise, fSetPixel cbDraw);

// void drawCircleSegment(int16_t x0, int16_t y0, int16_t x1, int16_t y1, fSetPixel cbDraw);

}  // gfxDraw:: namespace




// End.
