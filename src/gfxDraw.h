// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDraw.h: Library header file
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
// CHANGELOG:
// 15.05.2024  creation of the GFXDraw library.
// 09.06.2024  extended gfxDrawPathWidget
//
// - - - - -

#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <functional>
#include <vector>
#include <algorithm>

#include <cctype>
#include <string.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "gfxDrawCommon.h"
#include "gfxDrawColors.h"
#include "gfxDrawLine.h"
#include "gfxDrawCircle.h"
#include "gfxDrawRect.h"
#include "gfxDrawBezier.h"
#include "gfxDrawPath.h"

#ifdef ARDUINO
#define GFXD_TRACE(fmt, ...) Serial.printf(fmt "\n" __VA_OPT__(, ) __VA_ARGS__)
#else
#define GFXD_TRACE(fmt, ...) printf(fmt "\n" __VA_OPT__(, ) __VA_ARGS__)
#endif


namespace gfxDraw {

// ===== Callback function definitions used by drawing methods and widget classes =====

/// @brief Callback function definition to address a pixel on a display
typedef std::function<void(int16_t x, int16_t y)> fSetPixel;

/// @brief Callback function to transform all points in the segments
typedef std::function<void(int16_t &x, int16_t &y)> fTransform;

/// @brief Callback function definition to change a pixel on a display by applying the given color.
typedef std::function<void(int16_t x, int16_t y, ARGB color)> fDrawPixel;

/// @brief Callback function definition to read a pixel from a display
typedef std::function<ARGB(int16_t x, int16_t y)> fReadPixel;

/// ===== Basic draw functions with callback =====

// arc
// void drawArc(..., fSetPixel cbDraw);


/// @brief Draw an arc using the most efficient algorithm
void drawArc(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t rx, int16_t ry, int16_t phi, int16_t flags, fSetPixel cbDraw);


/// ====== internally used functions - maybe helpful for generating paths

/// @brief Calculate the angle of a vector in degrees
/// @param dx x value of the vector
/// @param dy y value of the vector
/// @return the angle n range 0...359
int16_t vectorAngle(int16_t dx, int16_t dy);


}  // gfxDraw:: namespace




// End.
