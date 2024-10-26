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

#define SLOPE_UNKNOWN 0
#define SLOPE_FALLING 1
#define SLOPE_RAISING 2
#define SLOPE_HORIZONTAL 3

// scaling: factors are that are in a unit of 100 (percent)
#define GFXSCALE100(p, f100) (((int32_t)(p) * f100 + 50) / 100)

namespace gfxDraw {

// ===== internal class definitions =====


/// @brief The _Edge class holds a horizontal pixel sequence for path boundaries and provides some useful static methods.
class _Edge : public Point {
public:
  _Edge(int16_t _x, int16_t _y)
    : Point(_x, _y), len(1) {};

  uint16_t len;

  /// @brief compare function for std::sort to sort points by (y) and ascending (x)
  /// @param p1 first Edge-point
  /// @param p2 second Edge-point
  /// @return when p1 is lower than p2
  static bool compare(const _Edge &p1, const _Edge &p2) {
    if (p1.y != p2.y)
      return (p1.y < p2.y);
    if (p1.x != p2.x)
      return (p1.x < p2.x);
    return (p1.len < p2.len);
  };

  /// @brief add another point or Edge to the Edge
  /// @param p2
  /// @return true when this Edge could be expanded.
  bool expand(_Edge p2) {
    if (y == p2.y) {
      if (x > p2.x + p2.len) {
        // no
        return (false);
      } else if (x + len < p2.x) {
        // no
        return (false);

      } else {
        // overlapping or joining edges
        int16_t left = (x < p2.x ? x : p2.x);
        int16_t right = (x + len > p2.x + p2.len ? x + len : p2.x + p2.len);
        x = left;
        len = right - left;
        return (true);
      }
    }
    return (false);
  };
};


// ===== Segments implementation =====

Segment::Segment(Type _type, int16_t p1, int16_t p2) {
  type = _type;
  p[0] = p1;
  p[1] = p2;
};


// ===== fixed decimal precision sin & cos functions =====





// ===== Debug helping functions... =====

void dumpPoints(std::vector<Point> &points) {
  TRACE("\nPoints:\n");
  size_t size = points.size();
  for (size_t i = 0; i < size; i++) {
    if (i % 10 == 0) {
      if (i > 0) { TRACE("\n"); }
      TRACE("  p%02d:", i);
    }
    TRACE(" (%2d/%2d)", points[i].x, points[i].y);
  }
  TRACE("\n");
}


void dumpEdges(std::vector<_Edge> &edges) {
  TRACE("\nEdges:\n");
  size_t size = edges.size();
  for (size_t i = 0; i < size; i++) {
    if (i % 10 == 0) {
      if (i > 0) { TRACE("\n"); }
      TRACE("  e%02d:", i);
    }
    TRACE(" (%3d/%3d)-%2d", edges[i].x, edges[i].y, edges[i].len);
  }
  TRACE("\n");
}

// ===== Basic drawing algorithm implementations =====

/// @brief Draw a rectangle with border and fill callbacks
/// @param x0 Starting Point X coordinate.
/// @param y0 Starting Point Y coordinate.
/// @param w width of the rect in pixels
/// @param h height of the rect in pixels
/// @param cbBorder Callback with coordinates of rect border pixels.
/// @param cbFill Callback with coordinates of rect fill pixels.
void drawRect(int16_t x0, int16_t y0, int16_t w, int16_t h, fSetPixel cbBorder, fSetPixel cbFill) {

  // draw the border in fill color
  if (!cbBorder) { cbBorder = cbFill; }

  if ((cbBorder) && (w != 0) && (h != 0)) {

    // ensure w > 0
    if (w < 0) {
      w = -w;
      x0 = x0 - w + 1;
    }

    // ensure h > 0
    if (h < 0) {
      h = -h;
      y0 = y0 - h + 1;
    }


    int16_t endX = x0 + w - 1;
    int16_t endY = y0 + h - 1;

    // draw lowest line
    for (int16_t x = x0; x <= endX; x++) cbBorder(x, y0);

    // draw first point, fill color and last point
    for (int16_t y = y0 + 1; y < endY; y++) {
      cbBorder(x0, y);
      if (cbFill) {
        for (int16_t x = x0 + 1; x < endX; x++) cbFill(x, y);
      }
      cbBorder(endX, y);
    }

    // draw highest line
    for (int16_t x = x0; x <= endX; x++) cbBorder(x, endY);
  }
}  // drawRect()


/// @brief Draw a rectangle with border and fill callbacks
/// @param x0 Starting Point X coordinate.
/// @param y0 Starting Point Y coordinate.
/// @param w width of the rect in pixels
/// @param h height of the rect in pixels
/// @param radius corner radius
/// @param cbBorder Callback with coordinates of rect border pixels.
/// @param cbFill Callback with coordinates of rect fill pixels.
void drawRoundedRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, fSetPixel cbBorder, fSetPixel cbFill) {

  // draw the border in fill color
  if (!cbBorder) { cbBorder = cbFill; }

  if ((cbBorder) && (w != 0) && (h != 0)) {
    // ensure w > 0
    if (w < 0) {
      w = -w;
      x0 = x0 - w + 1;
    }

    // ensure h > 0
    if (h < 0) {
      h = -h;
      y0 = y0 - h + 1;
    }

    // ensure meaningful radius
    if (radius > h / 2) radius = h / 2;
    if (radius > w / 2) radius = w / 2;

    int16_t endX = x0 + w - 1;
    int16_t endY = y0 + h - 1;
    int16_t oldY = y0 - 1;  // cy = current y line, start with

    // draw upper part: rounded start corner, fill between the corner, rounded end-corner
    drawCircleQuadrant(radius, 0, [&](int16_t x, int16_t y) {
      int16_t cx = x0 + radius - y;
      int16_t cy = y0 + radius - x;

      cbBorder(cx, cy);
      if (cy != oldY) {
        cx++;
        if (cy == y0) {
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
      cbBorder(x0, oldY);
      if (cbFill) {
        for (int16_t x = x0 + 1; x < endX; x++) cbFill(x, oldY);
      }
      cbBorder(endX, oldY);
      oldY++;
    }

    // draw lower part: rounded start corner, fill between the corner, rounded end-corner
    drawCircleQuadrant(radius, 0, [&](int16_t x, int16_t y) {
      int16_t cx = x0 + radius - y;
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
    rx = ry = dist / 2;
    TRACE("rx=ry= %d\n", rx);

  } else {
    double dist2 = (xTemp * xTemp) / (rx * rx) + (yTemp * yTemp) / (ry * ry);

    if (dist2 > 1) {
      double dist = sqrt(dist2);
      rx = std::round(rx * dist);
      ry = std::round(ry * dist);
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

  cx256 = (256 * centerX);
  cy256 = (256 * centerY);
}  // arcCenter()


/// Calculate the angle of a vector in degrees.
int16_t vectorAngle(int16_t dx, int16_t dy) {
  // TRACE("vectorAngle(%d, %d)\n", dx, dy);
  double rad = atan2(dy, dx);
  int16_t angle = std::round(rad * 180 / M_PI);
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

    int32_t sinFactor1000 = floor(sin(radians) * 1000);
    int32_t cosFactor1000 = floor(cos(radians) * 1000);

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
  int16_t scale1000;
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
            scale1000 = round(p1_length);
            angle = vectorAngle(p1_x, p1_y);
          }
          scaleKnown = true;
        }

        // scale x & y radius
        pSeg.p[0] = (pSeg.p[0] * scale1000 + 500) / 1000;
        pSeg.p[1] = (pSeg.p[1] * scale1000 + 500) / 1000;

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


// ===== Segment drawing functions =====

// Draw a path (no fill).
void drawSegments(std::vector<Segment> &segments, fSetPixel cbDraw) {
  TRACE("drawSegments()\n");
  int16_t startPosX = 0;
  int16_t startPosY = 0;
  int16_t posX = 0;
  int16_t posY = 0;
  int16_t endPosX = 0;
  int16_t endPosY = 0;

  if (segments.size()) {
    for (Segment &pSeg : segments) {
      switch (pSeg.type) {
        case Segment::Type::Move:
          startPosX = endPosX = pSeg.x1;
          startPosY = endPosY = pSeg.y1;
          break;

        case Segment::Type::Line:
          endPosX = pSeg.x1;
          endPosY = pSeg.y1;
          gfxDraw::drawLine(posX, posY, endPosX, endPosY, cbDraw);
          break;

        case Segment::Type::Curve:
          endPosX = pSeg.p[4];
          endPosY = pSeg.p[5];
          gfxDraw::drawCubicBezier(
            posX, posY,
            pSeg.p[0], pSeg.p[1],
            pSeg.p[2], pSeg.p[3],
            endPosX, endPosY, cbDraw);
          break;

        case Segment::Type::Arc:
          endPosX = pSeg.p[4];
          endPosY = pSeg.p[5];
          gfxDraw::drawArc(posX, posY,            // start-point
                           endPosX, endPosY,      // end-point
                           pSeg.p[0], pSeg.p[1],  // x & y radius
                           pSeg.p[2],             // phi, ellipsis rotation
                           pSeg.p[3],             // flags
                           cbDraw);
          break;

        case Segment::Type::Circle:
          if (1) {
            Point pCenter(pSeg.p[0], pSeg.p[1]);
            Point pStart(pSeg.p[0] + pSeg.p[2], pSeg.p[1]);

            // drawCircle(gfxDraw::Point(30, 190), 20, gfxDraw::Point(30 + 20, 190), gfxDraw::Point(30 + 20, 190), true, bmpSet(gfxDraw::RED));
            // The simplified drawCircleSegment cannot be used as for filling the circle the pixels must be in order.
            gfxDraw::drawCircleSegment(pCenter, pSeg.p[2], pStart, pStart, ArcFlags::Clockwise | ArcFlags::LongPath, cbDraw);
          }
          break;

        case Segment::Type::Close:
          endPosX = startPosX;
          endPosY = startPosY;
          if ((posX != endPosX) || (posY != endPosY)) {
            gfxDraw::drawLine(posX, posY, endPosX, endPosY, cbDraw);
          }
          cbDraw(0, POINT_BREAK_Y);
          break;

        default:
          TRACE("unknown segment-%04x\n", pSeg.type);
          break;
      }

      posX = endPosX;
      posY = endPosY;
    }  // for
    cbDraw(0, POINT_BREAK_Y);
  }
}  // drawSegments()


// find local extreme sequences and mark them with a double-edge
size_t slopeEdges(std::vector<_Edge> &edges, size_t start, size_t end) {
  int prevSlope;  // slope before any horizontal border points.

  if (edges[end].y < edges[start].y) {
    prevSlope = SLOPE_RAISING;
  } else {
    prevSlope = SLOPE_FALLING;
  }

  _Edge *prevEdge = &edges[end];
  uint16_t prevY = edges[end].y;

  size_t n = start;
  while (n <= end) {
    uint16_t thisY = edges[n].y;

    if (thisY > prevY) {
      if (prevSlope == SLOPE_FALLING) {
        // maximum extreme ends here: duplicate previous point
        TRACE("  ins %d\n", n);
        edges.insert(edges.begin() + n, *prevEdge);
        edges[n].len = 0;
        n++;
        end++;
      }
      prevSlope = SLOPE_RAISING;

    } else if (thisY < prevY) {
      if (prevSlope == SLOPE_RAISING) {
        // minimum extreme ends here: duplicate previous point
        TRACE("  ins %d\n", n);
        edges.insert(edges.begin() + n, *prevEdge);
        edges[n].len = 0;
        n++;
        end++;
      }
      prevSlope = SLOPE_FALLING;
    }

    prevEdge = &edges[n];
    prevY = prevEdge->y;
    n++;
  }

  // last edge is extreme ?
  if (edges[start].y > prevY) {
    if (prevSlope == SLOPE_FALLING) {
      // maximum extreme ends here: duplicate previous point
      TRACE("  ins+ %d\n", n);
      edges.insert(edges.begin() + n, *prevEdge);
      edges[n].len = 0;
      end++;
    }

  } else if (edges[start].y < prevY) {
    if (prevSlope == SLOPE_RAISING) {
      // minimum extreme ends here: duplicate previous point
      TRACE("  ins+ %d\n", n);
      edges.insert(edges.begin() + n, *prevEdge);
      edges[n].len = 0;
      end++;
    }
  }


  return (end);
}  // slopeEdges()


/// @brief Draw a path with filling.
void fillSegments(std::vector<Segment> &segments, fSetPixel cbBorder, fSetPixel cbFill) {
  TRACE("fillSegments()\n");
  std::vector<_Edge> edges;
  _Edge *lastEdge = nullptr;

  size_t n;
  fSetPixel cbStroke = cbBorder ? cbBorder : cbFill;  // use cbFill when no cbBorder is given.

  // create the path and collect edges
  drawSegments(segments,
               [&](int16_t x, int16_t y) {
                 //  TRACE("    P(%d/%d)\n", x, y);
                 if ((lastEdge) && (lastEdge->expand(_Edge(x, y)))) {
                   // fine
                 } else {
                   // first in sequence on on new line.
                   edges.push_back(_Edge(x, y));
                   lastEdge = &edges.back();
                 }
               });
  dumpEdges(edges);

  // sub-paths are separated by (0/POINT_BREAK_Y) points;
  size_t eSize = edges.size();

  size_t eStart = 0;
  n = eStart;
  while (n < eSize) {
    if (edges[n].y != POINT_BREAK_Y) {
      n++;
    } else {
      if (eStart < n - 1) {

        // Normalize (*2) sub-path for fill algorithm.
        // TRACE("  sub-paths %d ... %d\n", eStart, n - 1);

        if (edges[eStart].expand(edges[n - 1])) {
          // last point is in first edge
          edges.erase(edges.begin() + n - 1);
          TRACE("  del %d\n", n - 1);
          n--;
        }
        // dumpEdges(edges);

        n = slopeEdges(edges, eStart, n - 1) + 1;
        // dumpEdges(edges);

        TRACE("  sub-paths %d ... %d\n", eStart, n - 1);

      }  // if

      n = eStart = n + 1;
      eSize = edges.size();
    }
  }
  dumpEdges(edges);
  TRACE("\n");

  // sort edges by ascending lines (y)
  std::sort(edges.begin(), edges.end(), _Edge::compare);

  int16_t y = INT16_MAX;
  int16_t x = INT16_MAX;

  bool isInside = false;

  // Draw borderpoints and lines on inner segments
  for (_Edge &p : edges) {

    // if (p.y == 38) {
    //   TRACE("  P %d/%d-%d\n", p.x, p.y, p.len);
    // };

    if (p.y != y) {
      // start a new line
      isInside = false;
      y = p.y;
    }

    if (y == POINT_BREAK_Y) continue;

    if (p.len == 0) {
      // don't draw, it is just an marker for a extreme sequence.

    } else {
      // draw the border
      for (uint16_t l = 0; l < p.len; l++) {
        cbStroke(p.x + l, y);
      }
    }

    // draw the fill
    if (isInside) {
      while (x < p.x) {
        cbFill(x++, y);
      }
    }
    isInside = (!isInside);
    // if (p.x + p.len > x)
    x = p.x + p.len;
  }
}


// /// @brief Draw a path with filling.
// void XfillSegments(std::vector<Segment> &segments, int16_t dx, int16_t dy, fSetPixel cbBorder, fSetPixel cbFill) {
//   TRACE("fillSegments()\n");
//   std::vector<_Edge> edges;
//   _Edge *lastEdge = nullptr;

//   size_t n;
//   fSetPixel cbStroke = cbBorder ? cbBorder : cbFill;  // use cbFill when no cbBorder is given.

//   // create the path and collect edges
//   drawSegments(segments,  // dx, dy,
//                [&](int16_t x, int16_t y) {
//                  //  TRACE("    P(%d/%d)\n", x, y);
//                  if ((lastEdge) && (lastEdge->expand(_Edge(x, y)))) {
//                    // fine
//                  } else {
//                    // first in sequence on on new line.
//                    edges.push_back(_Edge(x, y));
//                    lastEdge = &edges.back();
//                  }
//                });
//   dumpEdges(edges);

//   // sub-paths are separated by (0/POINT_BREAK_Y) points;
//   size_t eSize = edges.size();

//   size_t eStart = 0;
//   n = eStart;
//   while (n < eSize) {
//     if (edges[n].y != POINT_BREAK_Y) {
//       n++;
//     } else {
//       if (eStart < n - 1) {

//         // Normalize (*2) sub-path for fill algorithm.
//         // TRACE("  sub-paths %d ... %d\n", eStart, n - 1);

//         if (edges[eStart].expand(edges[n - 1])) {
//           // last point is in first edge
//           edges.erase(edges.begin() + n - 1);
//           TRACE("  del %d\n", n - 1);
//           n--;
//         }
//         // dumpEdges(edges);

//         n = slopeEdges(edges, eStart, n - 1) + 1;
//         // dumpEdges(edges);

//         TRACE("  sub-paths %d ... %d\n", eStart, n - 1);

//       }  // if

//       n = eStart = n + 1;
//       eSize = edges.size();
//     }
//   }
//   dumpEdges(edges);
//   TRACE("\n");

//   // sort edges by ascending lines (y)
//   std::sort(edges.begin(), edges.end(), _Edge::compare);

//   int16_t y = INT16_MAX;
//   int16_t x = INT16_MAX;

//   bool isInside = false;

//   // Draw borderpoints and lines on inner segments
//   for (_Edge &p : edges) {

//     // if (p.y == 38) {
//     //   TRACE("  P %d/%d-%d\n", p.x, p.y, p.len);
//     // };

//     if (p.y != y) {
//       // start a new line
//       isInside = false;
//       y = p.y;
//     }

//     if (y == POINT_BREAK_Y) continue;

//     if (p.len == 0) {
//       // don't draw, it is just an marker for a extreme sequence.

//     } else {
//       // draw the border
//       for (uint16_t l = 0; l < p.len; l++) {
//         cbStroke(p.x + l, y);
//       }
//     }

//     // draw the fill
//     if (isInside) {
//       while (x < p.x) {
//         cbFill(x++, y);
//       }
//     }
//     isInside = (!isInside);
//     // if (p.x + p.len > x)
//     x = p.x + p.len;
//   }
// };  // fillSegments()


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
  int a;
}


/// @brief draw the a path.
/// @param path The path definition using SVG path syntax.
/// @param cbDraw Draw function for border pixels. cbFill is used when cbBorder is null.
void drawPath(const char *pathText, fSetPixel cbDraw) {
  std::vector<Segment> vSeg = parsePath(pathText);
  drawSegments(vSeg, cbDraw);
}


/// @brief Scan and parset a path text with the svg/path/d syntax to create a vector(array) of Segments.
/// @param pathText path definition as String
/// @return Vector with Segments.
/// @example pathText="M4 8l12-6l10 10h-8v4h-6z"
std::vector<Segment> parsePath(const char *pathText) {
  TRACE("parsePath: '%s'\n", pathText);
  char command = '-';

  char *path = (char *)pathText;
  int16_t lastX = 0, lastY = 0;

  /// A lambda function to parse a numeric parameter from the inputText.
  auto getNumParam = [&]() {
    while (isspace(*path) || (*path == ',')) { path++; }
    int16_t p = strtol(path, &path, 10);
    return (p);
  };

  /// A lambda function to parse a flag parameter from the inputText.
  auto getBoolParam = [&]() {
    while (isspace(*path) || (*path == ',')) { path++; }
    bool flag = (*path++ == '1');
    return (flag);
  };

  Segment Seg;
  std::vector<Segment> vSeg;

  while (path && *path) {

    if (isspace(*path)) {
      path++;

    } else {
      if (strchr("MmLlCcZHhVvAazO", *path))
        command = *path++;

      memset(&Seg, 0, sizeof(Seg));
      bool f1, f2;  // flags

      switch (command) {
        case 'M':
          Seg.type = Segment::Type::Move;
          lastX = Seg.p[0] = getNumParam();
          lastY = Seg.p[1] = getNumParam();
          break;

        case 'm':
          // convert to absolute coordinates
          Seg.type = Segment::Type::Move;
          lastX = Seg.p[0] = lastX + getNumParam();
          lastY = Seg.p[1] = lastY + getNumParam();
          break;

        case 'L':
          Seg.type = Segment::Type::Line;
          lastX = Seg.p[0] = getNumParam();
          lastY = Seg.p[1] = getNumParam();
          break;

        case 'l':
          // convert to absolute coordinates
          Seg.type = Segment::Type::Line;
          lastX = Seg.p[0] = lastX + getNumParam();
          lastY = Seg.p[1] = lastY + getNumParam();
          break;

        case 'C':
          // curve defined with absolute points - no convertion required
          Seg.type = Segment::Type::Curve;
          Seg.p[0] = getNumParam();
          Seg.p[1] = getNumParam();
          Seg.p[2] = getNumParam();
          Seg.p[3] = getNumParam();
          lastX = Seg.p[4] = getNumParam();
          lastY = Seg.p[5] = getNumParam();
          break;

        case 'c':
          // curve defined with relative points - convert to absolute coordinates
          Seg.type = Segment::Type::Curve;
          Seg.p[0] = lastX + getNumParam();
          Seg.p[1] = lastY + getNumParam();
          Seg.p[2] = lastX + getNumParam();
          Seg.p[3] = lastY + getNumParam();
          lastX = Seg.p[4] = lastX + getNumParam();
          lastY = Seg.p[5] = lastY + getNumParam();
          break;

        case 'H':
          // Horizontal line with absolute horizontal end point coordinate - convert to absolute line
          Seg.type = Segment::Type::Line;
          lastX = Seg.p[0] = getNumParam();
          Seg.p[1] = lastY;  // stay;
          break;

        case 'h':
          // Horizontal line with relative horizontal end-point coordinate - convert to absolute line
          Seg.type = Segment::Type::Line;
          lastX = Seg.p[0] = lastX + getNumParam();
          Seg.p[1] = lastY;  // stay;
          break;

        case 'V':
          // Vertical line with absolute vertical end point coordinate - convert to absolute line
          Seg.type = Segment::Type::Line;
          Seg.p[0] = lastX;  // stay;
          lastY = Seg.p[1] = getNumParam();
          break;

        case 'v':
          // Vertical line with relative horizontal end-point coordinate - convert to absolute line
          Seg.type = Segment::Type::Line;
          Seg.p[0] = lastX;  // stay;
          lastY = Seg.p[1] = lastY + getNumParam();
          break;

        case 'A':
          // Ellipsis arc with absolute end-point coordinates.
          Seg.type = Segment::Type::Arc;
          Seg.p[0] = getNumParam();  // rx
          Seg.p[1] = getNumParam();  // ry
          Seg.p[2] = getNumParam();  // rotation
          f1 = getBoolParam();
          f2 = getBoolParam();
          Seg.p[3] = (f1 ? 0x01 : 0x00) + (f2 ? 0x02 : 0x00);  // flags
          lastX = Seg.p[4] = getNumParam();
          lastY = Seg.p[5] = getNumParam();
          break;

        case 'a':
          // Ellipsis arc with relative end-point coordinates.
          Seg.type = Segment::Type::Arc;
          Seg.p[0] = getNumParam();  // rx
          Seg.p[1] = getNumParam();  // ry
          Seg.p[2] = getNumParam();  // rotation
          f1 = getBoolParam();
          f2 = getBoolParam();
          Seg.p[3] = (f1 ? 0x01 : 0x00) + (f2 ? 0x02 : 0x00);  // flags
          lastX = Seg.p[4] = lastX + getNumParam();
          lastY = Seg.p[5] = lastY + getNumParam();
          break;

        case 'z':
        case 'Z':
          Seg.type = Segment::Type::Close;
          break;

          // non svg path types:
        case 'O':
          // Draw a whole circle by center and radius
          Seg.type = Segment::Type::Circle;
          Seg.p[0] = getNumParam();  // Center.x
          Seg.p[1] = getNumParam();  // Center.y
          Seg.p[2] = getNumParam();  // radius
          lastX = lastY = 0;
          break;
      }
      vSeg.push_back(Seg);
      // } else { TRACE("unknown segment '%c'\n", *path);
    }
  }

  // TRACE("  scanned: %d segments\n", vSeg.size());
  // for (Segment &seg : vSeg) {
  //   TRACE("  %04x - %d %d\n", seg.type, seg.p[0], seg.p[1]);
  // }

  return (vSeg);
}

}  // gfxDraw:: namespace

// End.
