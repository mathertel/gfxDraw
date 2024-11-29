// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxdraw.cpp: Library implementation file
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxdraw.h and documentation files in this library.
//
// - - - - -

#include "gfxDraw.h"
#include "gfxDrawCommon.h"
#include "gfxDrawBezier.h"
#include "gfxDrawCircle.h"

#define TRACE(...)  // printf(__VA_ARGS__)

// scaling: factors are that are in a unit of 100 (percent)
#define GFXSCALE100(p, f100) (((int32_t)(p) * f100 + 50) / 100)

namespace gfxDraw {


// ===== fixed decimal precision sin & cos functions =====





// ===== Basic drawing algorithm implementations =====


/// @brief Calculate the center parameterization for an arc from endpoints
/// The radius values may be scaled up when there is no arc possible.
void arcCenter(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t &rx, int16_t &ry, int16_t phi, int16_t flags, int32_t &cx256, int32_t &cy256) {
  // Conversion from endpoint to center parameterization
  // see also http://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes

  // <https://github.com/canvg/canvg/blob/937668eced93e0335c67a255d0d2277ea708b2cb/src/Document/PathElement.ts#L491>

  double sinphi = sin(phi);
  double cosphi = cos(phi);

  // middle of (x1/y1) to (x2/y2)
  double xMiddle = (x1 - x2) / 2;
  double yMiddle = (y1 - y2) / 2;

  double xTemp = (cosphi * xMiddle) + (sinphi * yMiddle);
  double yTemp = (-sinphi * xMiddle) + (cosphi * yMiddle);

  // adjust x & y radius when too small
  if (rx == 0 || ry == 0) {
    double dist = sqrt(((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
    rx = ry = (int16_t)(dist / 2);
    TRACE("rx=ry= %d\n", rx);

  } else {
    double dist2 = (xTemp * xTemp) / (rx * rx) + (yTemp * yTemp) / (ry * ry);

    if (dist2 > 1) {
      double dist = sqrt(dist2);
      rx = static_cast<int16_t>(std::lround(rx * dist));
      ry = static_cast<int16_t>(std::lround(ry * dist));
    }
    TRACE("rx=%d ry=%d \n", rx, ry);
  }

  // center calculation
  double centerDist = 0;
  double distNumerator = ((rx * rx) * (ry * ry) - (rx * rx) * (yTemp * yTemp) - (ry * ry) * (xTemp * xTemp));
  if (distNumerator > 0) {
    centerDist = sqrt(distNumerator / ((rx * rx) * (yTemp * yTemp) + (ry * ry) * (xTemp * xTemp)));
  }

  if ((flags == 0x00) || (flags == 0x03)) {
    centerDist = -centerDist;
  }

  double cX = (centerDist * rx * yTemp) / ry;
  double cY = (centerDist * -ry * xTemp) / rx;

  double centerX = (cosphi * cX) - (sinphi * cY) + (x1 + x2) / 2;
  double centerY = (sinphi * cX) + (cosphi * cY) + (y1 + y2) / 2;

  cx256 = std::lround(256 * centerX);
  cy256 = std::lround(256 * centerY);
}  // arcCenter()


/// Calculate the angle of a vector in degrees.
int16_t vectorAngle(int16_t dx, int16_t dy) {
  // TRACE("vectorAngle(%d, %d)\n", dx, dy);
  double rad = atan2(dy, dx);
  int16_t angle = static_cast<int16_t>(std::lround(rad * 180 / M_PI));
  if (angle < 0) angle = 360 + angle;
  return (angle % 360);
}  // vectorAngle()


/// @brief Draw an arc according to svg path arc parameters.
/// @param x1 Starting Point X coordinate.
/// @param y1 Starting Point Y coordinate.
/// @param x2 Ending Point X coordinate.
/// @param y2 Ending Point Y coordinate.
/// @param rx
/// @param ry
/// @param phi  rotation of the ellipsis
/// @param flags
/// @param cbDraw Callback with coordinates of line pixels.
void drawArc(int16_t x1, int16_t y1, int16_t x2, int16_t y2,
             int16_t rx, int16_t ry,
             int16_t phi, int16_t flags,
             fSetPixel cbDraw) {
  TRACE("drawArc(%d/%d)-(%d/%d)\n", x1, y1, x2, y2);

  int32_t cx256, cy256;

  arcCenter(x1, y1, x2, y2, rx, ry, phi, flags, cx256, cy256);

  TRACE("  flags   = 0x%02x\n", flags);
  TRACE("  center = %d/%d\n", SCALE256(cx256), SCALE256(cy256));
  TRACE("  radius = %d/%d\n", rx, ry);

  proposePixel(x1, y1, cbDraw);
  if (rx == ry) {
    // draw a circle segment faster. ellipsis rotation can be ignored.
    gfxDraw::drawCircleSegment(gfxDraw::Point(SCALE256(cx256), SCALE256(cy256)), rx,
                               gfxDraw::Point(x1, y1),
                               gfxDraw::Point(x2, y2),
                               (gfxDraw::ArcFlags)(flags & gfxDraw::ArcFlags::Clockwise),
                               [&](int16_t x, int16_t y) {
                                 proposePixel(x, y, cbDraw);
                               });
  } else {
    int startAngle = vectorAngle(256 * x1 - cx256, 256 * y1 - cy256);
    int endAngle = vectorAngle(256 * x2 - cx256, 256 * y2 - cy256);
    int stepAngle = (flags & 0x02) ? 1 : 359;

    // Iterate through the ellipse
    for (int16_t angle = startAngle; angle != endAngle; angle = (angle + stepAngle) % 360) {
      int16_t x = SCALE256(cx256 + (rx * gfxDraw::cos256(angle)));
      int16_t y = SCALE256(cy256 + (ry * gfxDraw::sin256(angle)));
      proposePixel(x, y, cbDraw);
    }
  }
  proposePixel(x2, y2, cbDraw);

  proposePixel(0, POINT_BREAK_Y, cbDraw);

}  // drawArc()


// ===== Segment transformation functions =====

/// @brief Scale the points of a path by factor
/// @param segments
/// @param f100
void scaleSegments(std::vector<Segment> &segments, int16_t f100) {
  if (f100 != 100) {
    transformSegments(segments, [&](int16_t &x, int16_t &y) {
      x = GFXSCALE100(x, f100);
      y = GFXSCALE100(y, f100);
    });
  }
}  // scaleSegments()


/// @brief move all points by the given offset in x and y.
/// @param segments Segment vector to be changed
/// @param dx X-Offset
/// @param dy Y-Offset
void moveSegments(std::vector<Segment> &segments, int16_t dx, int16_t dy) {
  if ((dx != 0) || (dy != 0)) {
    transformSegments(segments, [&](int16_t &x, int16_t &y) {
      x += dx;
      y += dy;
    });
  }
}  // moveSegments()


void rotateSegments(std::vector<Segment> &segments, int16_t angle) {
  if (angle != 0) {

    double radians = (angle * M_PI) / 180;

    int32_t sinFactor1000 = std::lround(sin(radians) * 1000);
    int32_t cosFactor1000 = std::lround(cos(radians) * 1000);

    transformSegments(segments, [&](int16_t &x, int16_t &y) {
      int32_t tx = cosFactor1000 * x - sinFactor1000 * y;
      int32_t ty = sinFactor1000 * x + cosFactor1000 * y;
      x = (tx / 1000);
      y = (ty / 1000);
    });
  }
}  // rotateSegments()


/// @brief transform all points in the segment list.
/// @param segments Segment vector to be changed
void transformSegments(std::vector<Segment> &segments, fTransform cbTransform) {
  int16_t p0_x, p0_y, p1_x, p1_y;
  int16_t angle;
  int32_t scale1000;
  bool scaleKnown = false;

  for (Segment &pSeg : segments) {

    switch (pSeg.type) {
      case Segment::Type::Move:
      case Segment::Type::Line:
        cbTransform(pSeg.p[0], pSeg.p[1]);
        break;

      case Segment::Type::Curve:
        cbTransform(pSeg.p[0], pSeg.p[1]);
        cbTransform(pSeg.p[2], pSeg.p[3]);
        cbTransform(pSeg.p[4], pSeg.p[5]);
        break;

      case Segment::Type::Arc:
        if (!scaleKnown) {
          // extract scale and rotation from transforming (0,0)-(1000,0) once for the whole sement vector.
          p0_x = p0_y = p1_y = 0;
          p1_x = 1000;  // length = 1000

          cbTransform(p0_x, p0_y);
          cbTransform(p1_x, p1_y);

          // ignore any translation
          p1_x -= p0_x;
          p1_y -= p0_y;

          if (p1_y == 0) {
            // simplify for non rotated or 180° rotated transformations
            if (p1_x > 0) {
              scale1000 = p1_x;
              angle = 0;
            } else {
              scale1000 = -p1_x;
              angle = 180;
            }

          } else {
            double p1_length = sqrt((p1_x * p1_x) + (p1_y * p1_y));
            scale1000 = std::lround(p1_length);
            angle = vectorAngle(p1_x, p1_y);
          }
          scaleKnown = true;
        }

        // scale x & y radius
        pSeg.p[0] = static_cast<int16_t>((pSeg.p[0] * scale1000 + 500) / 1000);
        pSeg.p[1] = static_cast<int16_t>((pSeg.p[1] * scale1000 + 500) / 1000);

        // rotate ellipsis rotation
        pSeg.p[2] += angle;

        // transform endpoint
        cbTransform(pSeg.p[4], pSeg.p[5]);  // endpoint
        break;

      case Segment::Type::Circle:
        // TODO:
        TRACE("Transform circle is missing.\n");
        break;

      case Segment::Type::Close:
        break;

      default:
        TRACE("unknown segment-%04x\n", pSeg.type);
        break;
    }
  }  // for
};


/// @brief draw a path using a border and optional fill drawing function.
/// @param path The path definition using SVG path syntax.
/// @param x Starting Point X coordinate.
/// @param y Starting Point Y coordinate.
/// @param scale scaling factor * 100.
/// @param cbBorder Draw function for border pixels. cbFill is used when cbBorder is null.
/// @param cbFill Draw function for filling pixels.
void pathByText(const char *pathText, int16_t x, int16_t y, int16_t scale100, fSetPixel cbBorder, fSetPixel cbFill) {
  std::vector<Segment> vSeg = parsePath(pathText);
  if (scale100 != 100)
    gfxDraw::scaleSegments(vSeg, scale100);

  if ((x != 0) || (y != 0))
    gfxDraw::moveSegments(vSeg, x, y);

  if (cbFill) {
    fillSegments(vSeg, cbBorder, cbFill);
  } else {
    drawSegments(vSeg, cbBorder);
  }
}



}  // gfxDraw:: namespace

// End.
