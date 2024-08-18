// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawLine.cpp: Library implementation file for functions to calculate all points of a staight line.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDrawLine.h and documentation files in this library.
//
// - - - - -

#include "gfxDraw.h"
#include "gfxDrawLine.h"

#define TRACE(...)  // printf(__VA_ARGS__)

namespace gfxDraw {

// This implementation of cubic bezier curve with a start and an end point given and by using 2 control points.

/// @brief Draw a line using the most efficient algorithm
/// @param x0 Starting Point X coordinate.
/// @param y0 Starting Point Y coordinate.
/// @param x1 Ending Point X coordinate.
/// @param y1 Ending Point Y coordinate.
/// @param cbDraw Callback with coordinates of line pixels.
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, fSetPixel cbDraw) {
  TRACE("Draw Line (%d/%d)--(%d/%d)\n", x0, y0, x1, y1);

  int16_t dx = abs(x1 - x0);
  int16_t dy = abs(y1 - y0);
  int16_t sx = (x0 < x1) ? 1 : -1;
  int16_t sy = (y0 < y1) ? 1 : -1;

  if (x0 == x1) {
    // fast draw vertical lines
    int16_t endY = y1 + sy;
    for (int16_t y = y0; y != endY; y += sy) {
      cbDraw(x0, y);
    }

  } else if (y0 == y1) {
    // fast draw horizontal lines
    int16_t endX = x1 + sx;
    for (int16_t x = x0; x != endX; x += sx) {
      cbDraw(x, y0);
    }

  } else {
    int16_t err = dx - dy;

    while (true) {
      cbDraw(x0, y0);
      if ((x0 == x1) && (y0 == y1)) break;

      int16_t err2 = err << 1;

      if (err2 > -dy) {
        err -= dy;
        x0 += sx;
      }
      if (err2 < dx) {
        err += dx;
        y0 += sy;
      }
    }
  }
};

}  // gfxDraw:: namespace

// End.
