// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxdrawCircle.cpp: Library implementation file for functions to calculate all points of a circle, circle quadrant and circle segment.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxdrawCircle.h and documentation files in this library.
//
// - - - - -

#include "gfxDraw.h"
#include "gfxDrawCircle.h"

#define TRACE(...)  // printf(__VA_ARGS__)

namespace gfxDraw {

// ===== internal class definitions =====

// draw a circle
// the draw function is not called in order of the pixels on the circle.


// from: http://members.chello.at/easyfilter/bresenham.html

/// @brief Draw the circle quadrant with the pixels in the given order.
/// @param radius radius of the circle
/// @param q number of quadrant (see header file)
/// @param cbDraw will be called for all pixels in the Circle Quadrant
void drawCircleQuadrant(int16_t radius, int16_t q, fSetPixel cbDraw) {
  TRACE("drawCircleQuadrant(r=%d)\n", radius);

  int16_t x = -radius, y = 0;
  int16_t err = 2 - 2 * radius; /* II. Quadrant */

  do {
    if (q == 0) {
      cbDraw(-x, y);
    } else if (q == 1) {
      cbDraw(-y, -x);
    } else if (q == 2) {
      cbDraw(x, -y);
    } else if (q == 3) {
      cbDraw(y, x);
    }
    radius = err;
    if (radius <= y) err += ++y * 2 + 1;           // y step and error adjustment
    if (radius > x || err > y) err += ++x * 2 + 1; // x step and error adjustment
  } while (x < 0);
}  // drawCircleQuadrant()


/// @brief draw a circle segment
void drawCircleSegment(Point center, int16_t radius, Point startPoint, Point endPoint, ArcFlags flags, fSetPixel cbDraw) {
  TRACE("drawCircleSegment(%d/%d r=%d)  (%d/%d) -> (%d/%d)\n",
        center.x, center.y, radius startPoint.x, startPoint.y, endPoint.x, endPoint.y);

  int16_t xm = center.x;
  int16_t ym = center.y;

  if ((startPoint == endPoint) && (flags & ArcFlags::LongPath)) {
    // draw a full circle
    for (int16_t q = 0; q < 4; q++)
      drawCircleQuadrant(radius, q, [&](int16_t x, int16_t y) {
        cbDraw(xm + x, ym + y);
      });
    return;
  }

  if (!(flags & ArcFlags::Clockwise)) {
    // flip vertically and adjust angles to use clockwise processing.
    int16_t ym2 = 2 * center.y;
    drawCircleSegment(
      center, radius,
      Point(startPoint.x, ym2 - startPoint.y),
      Point(endPoint.x, ym2 - endPoint.y),
      (ArcFlags)(flags | ArcFlags::Clockwise),
      [&](int16_t x, int16_t y) {
        cbDraw(x, ym2 - y);
      });
    return;
  }  // if (! clockwise)


  // Clockwise processing only from here.

  // Draw from startPoint to endPoint using Quadrants
  startPoint = startPoint - center;
  endPoint = endPoint - center;

  uint16_t sQ = Point::circleQuadrant(startPoint);
  uint16_t eQ = Point::circleQuadrant(endPoint);

  if (sQ == eQ) {
    if (Point::compareCircle(endPoint, startPoint)) {
      eQ += 4;
    }

  } else if (sQ > eQ) {
    eQ += 4;
  }

  // s=1: before the start
  // s=2: draw pixels
  // s=3: after the end
  uint16_t s = 1;

  for (uint16_t q = sQ; q <= eQ; q++) {
    drawCircleQuadrant(radius, q % 4, [&](int16_t x, int16_t y) {
      if (s == 1) {
        if (Point::compareCircle(startPoint, Point(x, y))) {
          // start drawing the points
          s = 2;
        }

      } else if (s == 2) {
        if (q != eQ) {
          // draw

        } else if (q == eQ) {
          if (Point::compareCircle(endPoint, Point(x, y))) {
            // stop drawing the points
            s = 3;
          }
        }
      }

      if (s == 2) {
        cbDraw(xm + x, ym + y);
      }
    });
  }  // for()
  // } while (q <= eQ);

};  // drawCircleSegment()


// Draw a whole circle. The draw function is not called in order of the pixels on the circle.
void drawCircle(Point center, int16_t radius, fSetPixel cbDraw) {
  int16_t xm = center.x;
  int16_t ym = center.y;

  drawCircleQuadrant(radius, 0, [&](int16_t x, int16_t y) {
    cbDraw(xm + x, ym + y);
    cbDraw(xm - y, ym + x);
    cbDraw(xm - x, ym - y);
    cbDraw(xm + y, ym - x);
  });
}  // drawCircle()

}  // gfxDraw:: namespace

// End.
