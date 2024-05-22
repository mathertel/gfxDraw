// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxdrawCircle.cpp: Library implementation file for circle drawing functions
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxdraw.h and documentation files in this library.
//
// - - - - -

#include "gfxDraw.h"
#include "gfxDrawCircle.h"

#define TRACE(...) printf(__VA_ARGS__)

namespace gfxDraw {

// ===== internal class definitions =====

// draw a circle
// the draw function is not called in order of the pixels on the circle.

void drawCircleHelper(Point center, int16_t r, uint8_t cornername, fSetPixel cbDraw) {

  int16_t x0 = center.x;
  int16_t y0 = center.y;

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;

  // start with angle = 0;
  int16_t x = r;
  int16_t y = 0;

  while (y < x) {
    if (f >= 0) {
      x--;
      ddF_y += 2;
      f += ddF_y;
    }
    y++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
      // writePixel(x0 + x, y0 + y, color);
      // writePixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      // writePixel(x0 + x, y0 - y, color);
      // writePixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      // writePixel(x0 - y, y0 + x, color);
      // writePixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      // writePixel(x0 - y, y0 - x, color);
      // writePixel(x0 - x, y0 - y, color);
    }
  }
}


// from: http://members.chello.at/easyfilter/bresenham.html
void drawCircleQuadrant(int16_t r, bool clockwise, fSetPixel cbDraw) {
  TRACE("drawCircleQuadrant(r=%d)\n", r);

  int x = -r, y = 0;
  int err = 2 - 2 * r; /* II. Quadrant */

  do {
    if (clockwise) {
      cbDraw(-x, y); 
    } else {
      cbDraw(y, -x);
    }
    r = err;
    if (r <= y) err += ++y * 2 + 1;           /* e_xy+e_y < 0 */
    if (r > x || err > y) err += ++x * 2 + 1; /* e_xy+e_x > 0 or no 2nd y-step */
  } while (x < 0);
} // drawCircleQuadrant()


void drawCircle(Point center, int16_t r, int16_t startAngle, int16_t endAngle, bool clockwise, fSetPixel cbDraw) {
  TRACE("drawCircle(%d/%d r=%d)\n", center.x, center.y, r);

  int16_t xm = center.x;
  int16_t ym = center.y;

  if (clockwise) {
    drawCircleQuadrant(r, true, [&](int16_t x, int16_t y) { cbDraw(xm + x, ym + y); });
    drawCircleQuadrant(r, true, [&](int16_t x, int16_t y) { cbDraw(xm - y, ym + x); });
    drawCircleQuadrant(r, true, [&](int16_t x, int16_t y) { cbDraw(xm - x, ym - y); });
    drawCircleQuadrant(r, true, [&](int16_t x, int16_t y) { cbDraw(xm + y, ym - x); });

  } else {
    drawCircleQuadrant(r, false, [&](int16_t x, int16_t y) { cbDraw(xm + y, ym - x); });
    drawCircleQuadrant(r, false, [&](int16_t x, int16_t y) { cbDraw(xm - x, ym - y); });
    drawCircleQuadrant(r, false, [&](int16_t x, int16_t y) { cbDraw(xm - y, ym + x); });
    drawCircleQuadrant(r, false, [&](int16_t x, int16_t y) { cbDraw(xm + x, ym + y); });

  }
};  // drawCircle()


void drawCircle(Point center, int16_t r, fSetPixel cbDraw) {
  drawCircle(center, r, 0,360, false, cbDraw);
}

#if 0
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = r;
  int16_t y = 0;

  cbDraw(x0, y0 + r);
  cbDraw(x0, y0 - r);
  cbDraw(x0 + r, y0);
  cbDraw(x0 - r, y0);

  while (y <= x) {
    cbDraw(x0 + x, y0 + y);
    if (f >= 0) {
      x--;
      ddF_y += 2;
      f += ddF_y;
    }
    y++;
    ddF_x += 2;
    f += ddF_x;
    cbDraw(x0 + x, y0 + y);
    cbDraw(x0 - x, y0 + y);
    cbDraw(x0 + x, y0 - y);
    cbDraw(x0 - x, y0 - y);
    cbDraw(x0 + y, y0 + x);
    cbDraw(x0 - y, y0 + x);
    cbDraw(x0 + y, y0 - x);
    cbDraw(x0 - y, y0 - x);
  }
#endif

}  // gfxDraw:: namespace

// End.
