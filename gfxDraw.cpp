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

#define _USE_MATH_DEFINES
#include <math.h>
// #include <iostream>

#include "gfxDraw.h"
#include "gfxDrawColors.h"


#define TRACE(...) // printf(__VA_ARGS__)

#define STAT 1

#if (STAT)
uint16_t stat_skipped = 0;
uint16_t stat_added = 0;
uint16_t stat_removed = 0;
#endif


#define SLOPE_UNKNOWN 0
#define SLOPE_FALLING 1
#define SLOPE_RAISING 2
#define SLOPE_HORIZONTAL 3


#define POINT_BREAK_Y INT16_MAX
#define POINT_INVALID_Y INT16_MAX - 1

namespace gfxDraw {

// ===== internal class definitions =====

/// @brief The Point holds a pixel position and provides some useful static methods.
class Point {
public:
  Point()
    : x(0), y(POINT_INVALID_Y){};

  Point(int16_t _x, int16_t _y)
    : x(_x), y(_y){};

  /// @brief X coordinate of the Point
  int16_t x;

  /// @brief Y coordinate of the Point
  int16_t y;

  /// @brief compare function for std::sort to sort points by (y) and ascending (x)
  /// @param p1 first point
  /// @param p2 second point
  /// @return when p1 is lower than p2
  static bool compare(const Point &p1, const Point &p2) {
    if (p1.y != p2.y)
      return (p1.y < p2.y);
    return (p1.x < p2.x);
  };

  constexpr bool operator==(const Point &p2) {
    return ((x == p2.x) && (y == p2.y));
  };
};


/// @brief The _Edge class holds a horizontal pixel sequence for path boundaries and provides some useful static methods.
class _Edge : public Point {
public:
  _Edge(int16_t _x, int16_t _y)
    : Point(_x, _y), len(1){};

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

const int32_t tab_sin256[] = {
  0, 4, 9, 13, 18, 22, 27, 31, 35, 40, 44,
  49, 53, 57, 62, 66, 70, 75, 79, 83, 87,
  91, 96, 100, 104, 108, 112, 116, 120, 124, 128,
  131, 135, 139, 143, 146, 150, 153, 157, 160, 164,
  167, 171, 174, 177, 180, 183, 186, 190, 192, 195,
  198, 201, 204, 206, 209, 211, 214, 216, 219, 221,
  223, 225, 227, 229, 231, 233, 235, 236, 238, 240,
  241, 243, 244, 245, 246, 247, 248, 249, 250, 251,
  252, 253, 253, 254, 254, 254, 255, 255, 255, 255
};

int32_t sin256(int32_t degree) {
  degree = ((degree % 360) + 360) % 360;  // Math. Modulo Operator
  if (degree <= 90) {
    return (tab_sin256[degree]);
  } else if (degree <= 180) {
    return (tab_sin256[90 - (degree - 90)]);
  } else if (degree <= 270) {
    return (-tab_sin256[degree - 180]);
  } else {
    return (-tab_sin256[90 - (degree - 270)]);
  }
}

int32_t cos256(int32_t degree) {
  return (sin256(degree + 90));
}

#define SCALE256(v) (v >> 8)




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

// ===== gfxDraw helper functions =====

void dumpColor(char *name, RGBA col) {
  TRACE(" %-12s: %02x.%02x.%02x.%02x %08lx\n", name, col.Alpha, col.Red, col.Green, col.Blue, col.toColor24());
  int_fast16_t t1 = INT16_MIN;
}

void dumpColorTable() {
  TRACE("        Color: A  R  G  B  #col24\n");
  dumpColor("Red", gfxDraw::RED);
  dumpColor("Green", gfxDraw::GREEN);
  dumpColor("Blue", gfxDraw::BLUE);
  dumpColor("Orange", gfxDraw::ORANGE);
  dumpColor("Transparent", gfxDraw::TRANSPARENT);
}



// ===== Basic drawing algorithm implementations =====

/// @brief Draw a line using the most efficient algorithm
/// @param x0 Starting Point X coordinate.
/// @param y0 Starting Point Y coordinate.
/// @param x1 Ending Point X coordinate.
/// @param y1 Ending Point Y coordinate.
/// @param cbDraw Callback with coordinates of line pixels.
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, fSetPixel cbDraw) {
  // TRACE("Draw Line %d/%d %d/%d\n", x0, y0, x1, y1);

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


/// @brief Draw a rectangle with border and fill callbacks
/// @param x0 Starting Point X coordinate.
/// @param y0 Starting Point Y coordinate.
/// @param w width of the rect in pixels
/// @param h height of the rect in pixels
/// @param cbDraw Callback with coordinates of rect pixels.
void drawRect(int16_t x0, int16_t y0, int16_t w, int16_t h, fSetPixel cbDraw) {
  if ((w != 0) && (h != 0)) {

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

    // draw lines on the border clockwise
    for (int16_t x = x0; x <= endX; x++) cbDraw(x, y0);
    for (int16_t y = y0; y <= endY; y++) cbDraw(endX, y);
    for (int16_t x = endX; x >= x0; x--) cbDraw(x, endY);
    for (int16_t y = endY; y >= y0; y--) cbDraw(x0, y);
  }
}  // rect()


/// @brief Draw a solidrectangle with border and fill callbacks
/// @param x0 Starting Point X coordinate.
/// @param y0 Starting Point Y coordinate.
/// @param w width of the rect in pixels
/// @param h height of the rect in pixels
/// @param cbDraw Callback with coordinates of rect pixels.
void drawSolidRect(int16_t x0, int16_t y0, int16_t w, int16_t h, fSetPixel cbDraw) {
  if ((w != 0) && (h != 0)) {

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

    // draw all pixels in the rect.
    for (int16_t y = y0; y <= endY; y++) {
      for (int16_t x = x0; x <= endX; x++) {
        cbDraw(x, y);
      }
    }
  }
}  // rect()


/// @brief Calculate the center parameterization for an arc from endpoints
/// The radius values may be scaled up when there is no arc possible.
void arcCenter(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t &rx, int16_t &ry, int16_t phi, int16_t flags, int16_t &cx, int16_t &cy) {
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
      rx = (rx * dist) + 0.5;
      ry = (ry * dist) + 0.5;
    }
    TRACE("rx=%d ry=%d \n", rx, ry);
  }

  // center calculation
  double centerDist = 0;
  double distNumerator = ((rx * rx) * (ry * ry) - (rx * rx) * (yTemp * yTemp) - (ry * ry) * (xTemp * xTemp));
  if (distNumerator > 0) {
    centerDist = sqrt(distNumerator / ((rx * rx) * (yTemp * yTemp) + (ry * ry) * (xTemp * xTemp)));
  }

  if ((flags == 0x01) || (flags == 0x02)) {
    centerDist = -centerDist;
  }

  double cX = (centerDist * rx * yTemp) / ry;
  double cY = (centerDist * -ry * xTemp) / rx;

  double centerX = (cosphi * cX) - (sinphi * cY) + (x1 + x2) / 2;
  double centerY = (sinphi * cX) + (cosphi * cY) + (y1 + y2) / 2;

  cx = (centerX + 0.5);
  cy = (centerY + 0.5);
}  // arcCenter()


int16_t vectorAngle(int16_t dx, int16_t dy) {
  // TRACE("vectorAngle(%d, %d)\n", dx, dy);

  double rad = acos((double)dx / sqrt((dx * dx) + (dy * dy)));
  int16_t angle = ((rad * 180 / M_PI) + 0.5);

  if (dy < 0) { angle = 360 - angle; }
  // TRACE(" = %d\n", angle);

  return (angle % 360);
}  // vectorAngle()


// add the next path pixel to cbDraw
// * ignore duplicates
// * remove corner-type pixels
// * fill missing 1-pixel
// * draw streight line when more pixels are missing.

void _addPixel(int16_t x, int16_t y, fSetPixel cbDraw) {
  static Point lastPoints[3];

  TRACE("_addPixel(%d, %d)\n", x, y);

  if ((x == lastPoints[0].x) && (y == lastPoints[0].y)) {
    // don't collect duplicates
    TRACE("  duplicate!\n");
#if (STAT)
    stat_skipped++;
#endif

  } else if (y == POINT_BREAK_Y) {
    // draw all remaining points and invalidate lastPoints
    TRACE("  flush!\n");
    for (int n = 2; n >= 0; n--) {
      if (lastPoints[n].y != POINT_INVALID_Y)
        cbDraw(lastPoints[n].x, lastPoints[n].y);
      lastPoints[n].y = POINT_INVALID_Y;
    }  // for

  } else if (lastPoints[0].y == POINT_BREAK_Y) {
    lastPoints[0].x = x;
    lastPoints[0].y = y;

  } else {
    // draw oldest point and shift new point in
    if (lastPoints[2].y != POINT_INVALID_Y)
      cbDraw(lastPoints[2].x, lastPoints[2].y);
    lastPoints[2] = lastPoints[1];
    lastPoints[1] = lastPoints[0];
    lastPoints[0].x = x;
    lastPoints[0].y = y;

    if (lastPoints[1].y != POINT_INVALID_Y) {
      bool delFlag = false;
      bool insFlag = false;

      if ((lastPoints[0].y == lastPoints[1].y) && (abs(lastPoints[0].x - lastPoints[1].x) == 1)) {
        delFlag = (lastPoints[1].x == lastPoints[2].x);
      } else if ((lastPoints[0].x == lastPoints[1].x) && (abs(lastPoints[0].y - lastPoints[1].y) == 1)) {
        delFlag = (lastPoints[1].y == lastPoints[2].y);

      } else if ((abs(lastPoints[0].x - lastPoints[1].x) == 2) || (abs(lastPoints[0].y - lastPoints[1].y) == 2)) {
        // simple interpolate new lastPoints[1]
        // TRACE("  gap!\n");
        if (lastPoints[2].y != POINT_INVALID_Y)
          cbDraw(lastPoints[2].x, lastPoints[2].y);
        lastPoints[2] = lastPoints[1];
        lastPoints[1].x = (lastPoints[0].x + lastPoints[1].x) / 2;
        lastPoints[1].y = (lastPoints[0].y + lastPoints[1].y) / 2;
#if (STAT)
        stat_added++;
#endif

      } else if ((abs(lastPoints[0].x - lastPoints[1].x) > 2) || (abs(lastPoints[0].y - lastPoints[1].y) > 2)) {
        // TRACE("  big gap!\n");

        // draw a streight line from lastPoints[1] to lastPoints[0]
        if (lastPoints[2].y != POINT_INVALID_Y)
          cbDraw(lastPoints[2].x, lastPoints[2].y);
        drawLine(lastPoints[1].x, lastPoints[1].y, lastPoints[0].x, lastPoints[0].y, cbDraw);
        for (int n = 2; n >= 0; n--) {
          lastPoints[n].y = POINT_INVALID_Y;
        }  // for
      }

      if (delFlag) {
        // remove lastPoints[1];
        lastPoints[1] = lastPoints[2];
        lastPoints[2].y = POINT_INVALID_Y;
#if (STAT)
        stat_removed++;
#endif
      }
    }
  }
}  // _addPixel()



/// @brief Draw an arc according to svg path arc parameters.
/// @param x0 Starting Point X coordinate.
/// @param y0 Starting Point Y coordinate.
/// @param rx
/// @param ry
/// @param phi  rotation of the ellipsis
/// @param flags
/// @param x1 Ending Point X coordinate.
/// @param y1 Ending Point Y coordinate.
/// @param cbDraw Callback with coordinates of line pixels.
void drawArc(int16_t x1, int16_t y1, int16_t x2, int16_t y2,
             int16_t rx, int16_t ry,
             int16_t phi, int16_t flags,
             fSetPixel cbDraw) {

#if (STAT)
  stat_skipped = 0;
  stat_added = 0;
  stat_removed = 0;
#endif

  int16_t cx, cy;

  arcCenter(x1, y1, x2, y2, rx, ry, phi, flags, cx, cy);

  TRACE("flags=: 0x%02x\n", flags);
  TRACE("center=: %d/%d\n", cx, cy);

  int startAngle = vectorAngle(x1 - cx, y1 - cy);
  int endAngle = vectorAngle(x2 - cx, y2 - cy);
  int stepAngle = (flags & 0x02) ? -1 : 1;

  // Iterate through the ellipse
  _addPixel(x1, y1, cbDraw);
  for (uint16_t angle = startAngle; angle != endAngle; angle = (angle + stepAngle) % 360) {
    int16_t x = cx + SCALE256(rx * cos256(angle));
    int16_t y = cy + SCALE256(ry * sin256(angle));
    _addPixel(x, y, cbDraw);
  }
  _addPixel(x2, y2, cbDraw);
  _addPixel(0, POINT_BREAK_Y, cbDraw);

#if (STAT)
  TRACE("skipped: %d\n", stat_skipped);
  TRACE("added:   %d\n", stat_added);
  TRACE("removed: %d\n", stat_removed);
#endif

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
/// @param dx X-Offset
/// @param dy Y-Offset
void transformSegments(std::vector<Segment> &segments, fTransform cbTransform) {
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
  drawSegments(segments, 0, 0, cbDraw);
};

// Draw a path (no fill).
void drawSegments(std::vector<Segment> &segments, int16_t dx, int16_t dy, fSetPixel cbDraw) {
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
          gfxDraw::drawLine(posX + dx, posY + dy, endPosX + dx, endPosY + dy, cbDraw);
          break;

        case Segment::Type::Curve:
          endPosX = pSeg.p[4];
          endPosY = pSeg.p[5];
          gfxDraw::drawCubicBezier(
            dx + posX, dy + posY,
            dx + pSeg.p[0], dy + pSeg.p[1],
            dx + pSeg.p[2], dy + pSeg.p[3],
            dx + endPosX, dy + endPosY, cbDraw);
          break;

        case Segment::Type::Arc:
          endPosX = pSeg.p[4];
          endPosY = pSeg.p[5];
          gfxDraw::drawArc(posX + dx, posY + dy,        // start-point
                           endPosX + dx, endPosY + dy,  // end-point
                           pSeg.p[0], pSeg.p[1],        // x & y radius
                           pSeg.p[2],                   // phi, ellipsis rotation
                           pSeg.p[3],                   // flags
                           cbDraw);

          break;

        case Segment::Type::Close:
          endPosX = startPosX;
          endPosY = startPosY;
          if ((posX != endPosX) || (posY != endPosY)) {
            gfxDraw::drawLine(posX + dx, posY + dy, endPosX + dx, endPosY + dy, cbDraw);
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
  fillSegments(segments, 0, 0, cbBorder, cbFill);
}

/// @brief Draw a path with filling.
void fillSegments(std::vector<Segment> &segments, int16_t dx, int16_t dy, fSetPixel cbBorder, fSetPixel cbFill) {
  TRACE("fillSegments()\n");
  std::vector<_Edge> edges;
  _Edge *lastEdge = nullptr;

  size_t n;
  fSetPixel cbStroke = cbBorder ? cbBorder : cbFill;  // use cbFill when no cbBorder is given.

  // create the path and collect edges
  drawSegments(segments, dx, dy,
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
};  // fillSegments()


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

  if (cbFill) {
    fillSegments(vSeg, x, y, cbBorder, cbFill);
  } else {
    drawSegments(vSeg, x, y, cbBorder);
  }
  int a;
}


/// @brief draw a path using a border and optional fill drawing function.
/// @param path The path definition using SVG path syntax.
/// @param x Starting Point X coordinate.
/// @param y Starting Point Y coordinate.
/// @param cbBorder Draw function for border pixels. cbFill is used when cbBorder is null.
/// @param cbFill Draw function for filling pixels.
void pathByText100(const char *pathText, int16_t x, int16_t y, fSetPixel cbBorder, fSetPixel cbFill) {
  pathByText(pathText, x, y, 100, cbBorder, cbFill);
}

/// @brief draw the a path.
/// @param path The path definition using SVG path syntax.
/// @param cbDraw Draw function for border pixels. cbFill is used when cbBorder is null.
void drawPath(const char *pathText, fSetPixel cbDraw) {
  std::vector<Segment> vSeg = parsePath(pathText);
  drawSegments(vSeg, 0, 0, cbDraw);
}


// ===== Cubic Bezier Curve Segments =====



// This implementation of cubic bezier curve with a start and an end point given and by using 2 control points.
// C x1 y1, x2 y2, x y
// good article for reading: <https://pomax.github.io/bezierinfo/>
// Here the Casteljau's algorithm approach is used.

void drawCubicBezier(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, fSetPixel cbDraw) {
  // TRACE("cubicBezier: %d/%d %d/%d %d/%d %d/%d\n", x0, y0, x1, y1, x2, y2, x3, y3);

  // Line 1 is x0/y0 to x1/y1, dx1/dy1 is the relative vector from x0/y0 to x1/y1
  int16_t dx1 = (x1 - x0);
  int16_t dy1 = (y1 - y0);
  int16_t dx2 = (x2 - x1);
  int16_t dy2 = (y2 - y1);
  int16_t dx3 = (x3 - x2);
  int16_t dy3 = (y3 - y2);

  // heuristic: calc the steps we need
  uint16_t steps = (abs(dx1) + abs(dy1) + abs(dx2) + abs(dy2) + abs(dx3) + abs(dy3)) / 2;  // p0 - 1 - 2 - 3 - 4 - p3
                                                                                           // TRACE("steps:%d\n", steps);
#if (STAT)
  stat_skipped = 0;
  stat_added = 0;
  stat_removed = 0;
#endif

  _addPixel(x0, y0, cbDraw);

  for (uint16_t n = 1; n <= steps; n++) {
    int16_t f = (1000 * n) / steps;
    // 3 points and 3 deltas in 1000 units
    int32_t x4 = x0 * 1000 + (f * dx1);
    int32_t y4 = y0 * 1000 + (f * dy1);
    int32_t x5 = x1 * 1000 + (f * dx2);
    int32_t y5 = y1 * 1000 + (f * dy2);
    int32_t x6 = x2 * 1000 + (f * dx3);
    int32_t y6 = y2 * 1000 + (f * dy3);
    int32_t dx5 = (x5 - x4);
    int32_t dy5 = (y5 - y4);
    int32_t dx6 = (x6 - x5);
    int32_t dy6 = (y6 - y5);

    // 2 points
    int32_t x7 = x4 + (f * dx5) / 1000;
    int32_t y7 = y4 + (f * dy5) / 1000;
    int32_t x8 = x5 + (f * dx6) / 1000;
    int32_t y8 = y5 + (f * dy6) / 1000;
    int32_t dx8 = (x8 - x7);
    int32_t dy8 = (y8 - y7);

    // 1 points
    int32_t x9 = x7 + (f * dx8) / 1000;
    int32_t y9 = y7 + (f * dy8) / 1000;

    int16_t nextX = (x9 + 500) / 1000;
    int16_t nextY = (y9 + 500) / 1000;

    _addPixel(nextX, nextY, cbDraw);
  }  // for
  _addPixel(x3, y3, cbDraw);

  // flush all Pixels
  _addPixel(0, POINT_BREAK_Y, cbDraw);

#if (STAT)
  TRACE("skipped: %d\n", stat_skipped);
  TRACE("added:   %d\n", stat_added);
  TRACE("removed: %d\n", stat_removed);
#endif
}  // drawCubicBezier()


/// @brief Scan and parset a path text with the svg/path/d syntax to create a vector(array) of Segments.
/// @param pathText path definition as String
/// @return Vector with Segments.
/// @example pathText="M4 8l12-6l10 10h-8v4h-6z"
std::vector<Segment> parsePath(const char *pathText) {
  TRACE("parsePath: '%s'\n", pathText);
  char command = '-';

  char *path = (char *)pathText;
  int16_t lastX = 0, lastY = 0;

  /// A lambda function to parse a parameter from the inputText.
  auto getParam = [&]() {
    while (isblank(*path) || (*path == ',')) { path++; }
    int16_t p = strtol(path, &path, 10);
    return (p);
  };

  Segment Seg;
  std::vector<Segment> vSeg;

  while (path && *path) {
    memset(&Seg, 0, sizeof(Seg));
    int parameters = -1;

    while (isblank(*path)) path++;

    if (strchr("MmLlCcZHhVvAaz", *path))
      command = *path++;

    switch (command) {
      case 'M':
        Seg.type = Segment::Move;
        lastX = Seg.p[0] = getParam();
        lastY = Seg.p[1] = getParam();
        break;

      case 'm':
        // convert to absolute coordinates
        Seg.type = Segment::Move;
        lastX = Seg.p[0] = lastX + getParam();
        lastY = Seg.p[1] = lastY + getParam();
        break;

      case 'L':
        Seg.type = Segment::Line;
        lastX = Seg.p[0] = getParam();
        lastY = Seg.p[1] = getParam();
        break;

      case 'l':
        // convert to absolute coordinates
        Seg.type = Segment::Line;
        lastX = Seg.p[0] = lastX + getParam();
        lastY = Seg.p[1] = lastY + getParam();
        break;

      case 'C':
        // curve defined with absolute points - no convertion required
        Seg.type = Segment::Curve;
        Seg.p[0] = getParam();
        Seg.p[1] = getParam();
        Seg.p[2] = getParam();
        Seg.p[3] = getParam();
        lastX = Seg.p[4] = getParam();
        lastY = Seg.p[5] = getParam();
        break;

      case 'c':
        // curve defined with relative points - convert to absolute coordinates
        Seg.type = Segment::Curve;
        Seg.p[0] = lastX + getParam();
        Seg.p[1] = lastY + getParam();
        Seg.p[2] = lastX + getParam();
        Seg.p[3] = lastY + getParam();
        lastX = Seg.p[4] = lastX + getParam();
        lastY = Seg.p[5] = lastY + getParam();
        break;

      case 'H':
        // Horizontal line with absolute horizontal end point coordinate - convert to absolute line
        Seg.type = Segment::Line;
        lastX = Seg.p[0] = getParam();
        Seg.p[1] = lastY;  // stay;
        break;

      case 'h':
        // Horizontal line with relative horizontal end-point coordinate - convert to absolute line
        Seg.type = Segment::Line;
        lastX = Seg.p[0] = lastX + getParam();
        Seg.p[1] = lastY;  // stay;
        break;

      case 'V':
        // Vertical line with absolute vertical end point coordinate - convert to absolute line
        Seg.type = Segment::Line;
        Seg.p[0] = lastX;  // stay;
        lastY = Seg.p[1] = getParam();
        break;

      case 'v':
        // Vertical line with relative horizontal end-point coordinate - convert to absolute line
        Seg.type = Segment::Line;
        Seg.p[0] = lastX;  // stay;
        lastY = Seg.p[1] = lastY + getParam();
        break;

      case 'A':
        // Ellipsis arc with absolute end-point coordinate. - calculate center and
        Seg.type = Segment::Arc;
        Seg.p[0] = getParam();       // rx
        Seg.p[1] = getParam();       // ry
        Seg.p[2] = getParam();       // rotation
        Seg.p[3] = getParam();       // flag1
        Seg.p[3] += getParam() * 2;  // flag2
        lastX = Seg.p[4] = getParam();
        lastY = Seg.p[5] = getParam();
        break;

      case 'a':
        // Ellipsis arc with absolute end-point coordinate. - calculate center and
        // Ellipsis arc with absolute end-point coordinate. - calculate center and
        Seg.type = Segment::Arc;
        Seg.p[0] = getParam();       // rx
        Seg.p[1] = getParam();       // ry
        Seg.p[2] = getParam();       // rotation
        Seg.p[3] = getParam();       // flag1
        Seg.p[3] += getParam() * 2;  // flag2
        lastX = Seg.p[4] = lastX + getParam();
        lastY = Seg.p[5] = lastY + getParam();
        break;

      case 'z':
      case 'Z':
        Seg.type = Segment::Close;
        break;

      default:
        printf("unknown path type: %c\n", *path);
        return (vSeg);
        break;
    }

    vSeg.push_back(Seg);
  }

  // TRACE("  scanned: %d segments\n", vSeg.size());
  // for (Segment &seg : vSeg) {
  //   TRACE("  %04x - %d %d\n", seg.type, seg.p[0], seg.p[1]);
  // }

  return (vSeg);
}

}  // gfxDraw:: namespace

// End.
