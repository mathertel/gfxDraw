// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawRect.cpp: Library implementation file for functions to calculate all points of a staight line.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDrawRect.h and documentation files in this library.
//
// - - - - -

#include "gfxDraw.h"
#include "gfxDrawCircle.h"

#define TRACE(...)  // printf(__VA_ARGS__)

namespace gfxDraw {

/// @brief Draw a rectangle with border and fill callbacks
/// @param x Starting Point X coordinate.
/// @param y Starting Point Y coordinate.
/// @param w width of the rect in pixels
/// @param h height of the rect in pixels
/// @param cbBorder Callback with coordinates of rect border pixels.
/// @param cbFill Callback with coordinates of rect fill pixels.
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, fSetPixel cbBorder, fSetPixel cbFill) {

  // draw the border in fill color
  if (!cbBorder) { cbBorder = cbFill; }

  if ((cbBorder) && (w != 0) && (h != 0)) {

    // ensure w >= 0
    if (w < 0) {
      w = -w;
      x = x - w + 1;
    }

    // ensure h >= 0
    if (h < 0) {
      h = -h;
      y = y - h + 1;
    }


    int16_t endX = x + w - 1;
    int16_t endY = y + h - 1;

    // draw lowest line
    for (int16_t nx = x; nx <= endX; nx++) cbBorder(nx, y);

    // draw first point, fill color and last point
    for (int16_t ny = y + 1; ny < endY; ny++) {
      cbBorder(x, ny);
      if (cbFill) {
        for (int16_t nx = x + 1; nx < endX; nx++) cbFill(nx, ny);
      }
      cbBorder(endX, ny);
    }

    // draw highest line
    for (int16_t nx = x; nx <= endX; nx++) cbBorder(nx, endY);
  }
}  // drawRect()


/// @brief Draw a rectangle with border and fill callbacks
/// @param x Starting Point X coordinate.
/// @param y Starting Point Y coordinate.
/// @param w width of the rect in pixels
/// @param h height of the rect in pixels
/// @param radius corner radius
/// @param cbBorder Callback with coordinates of rect border pixels.
/// @param cbFill Callback with coordinates of rect fill pixels.
void drawRoundedRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, fSetPixel cbBorder, fSetPixel cbFill) {

  // draw the border in fill color
  if (!cbBorder) { cbBorder = cbFill; }

  if ((cbBorder) && (w != 0) && (h != 0)) {

    // ensure w >= 0
    if (w < 0) {
      w = -w;
      x = x - w + 1;
    }

    // ensure h >= 0
    if (h < 0) {
      h = -h;
      y = y - h + 1;
    }

    // ensure meaningful radius
    if (radius > h / 2) radius = h / 2;
    if (radius > w / 2) radius = w / 2;

    int16_t endX = x + w - 1;
    int16_t endY = y + h - 1;
    int16_t oldY = y - 1;  // cy = current y line, start with

    // draw upper part: rounded start corner, fill between the corner, rounded end-corner
    drawCircleQuadrant(radius, 0, [&](int16_t x, int16_t y) {
      int16_t cx = x + radius - y;
      int16_t cy = y + radius - x;

      cbBorder(cx, cy);
      if (cy != oldY) {
        cx++;
        if (cy == y) {
          while (cx < endX - radius + y) cbBorder(cx++, cy);
        } else {
          while (cx < endX - radius + y) cbFill(cx++, cy);
        }
        oldY = cy;
      }
      cbBorder(endX - radius + y, cy);
    });

    oldY++;
    // draw first point, fill color and last point
    while (oldY <= endY - radius) {
      cbBorder(x, oldY);
      if (cbFill) {
        for (int16_t nx = x + 1; nx < endX; nx++) cbFill(nx, oldY);
      }
      cbBorder(endX, oldY);
      oldY++;
    }

    // draw lower part: rounded start corner, fill between the corner, rounded end-corner
    drawCircleQuadrant(radius, 0, [&](int16_t x, int16_t y) {
      int16_t cx = x + radius - y;
      int16_t cy = endY - radius + x;

      cbBorder(cx, cy);
      if (cy != oldY) {
        cx++;
        if (cy == endY) {
          while (cx < endX - radius + y) cbBorder(cx++, cy);
        } else {
          while (cx < endX - radius + y) cbFill(cx++, cy);
        }
        oldY = cy;
      }
      cbBorder(endX - radius + y, cy);
    });
  }
}  // drawRoundedRect()




}  // gfxDraw:: namespace

// End.
