// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawPath.cpp: Library implementation file for functions to calculate all points of a staight line.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDrawPath.h and documentation files in this library.
//
// - - - - -

#include "gfxDraw.h"
#include "gfxDrawPath.h"

#define TRACE(...)  // printf(__VA_ARGS__)

#define SLOPE_UNKNOWN 0
#define SLOPE_FALLING 1
#define SLOPE_RAISING 2
#define SLOPE_HORIZONTAL 3


namespace gfxDraw {
// ===== internal class definitions =====


// ===== Segments implementation =====

Segment::Segment(Type _type, int16_t p1, int16_t p2) {
  type = _type;
  p[0] = p1;
  p[1] = p2;
}

Segment Segment::createMove(int16_t x, int16_t y) {
  return (Segment(Type::Move, x, y));
}

Segment Segment::createMove(Point p) {
  return (Segment(Type::Move, p.x, p.y));
}

Segment Segment::createLine(int16_t x, int16_t y) {
  return (Segment(Type::Line, x, y));
}

Segment Segment::createLine(Point p) {
  return (Segment(Type::Line, p.x, p.y));
}

Segment Segment::createClose() {
  Segment s;
  s.type = Close;
  return (s);
}

Segment Segment::createArc(int16_t radius, bool f1, bool f2, int16_t xEnd, int16_t yEnd) {
  Segment s;
  s.type = Arc;
  s.rx = s.ry = radius;
  s.rotation = 0;
  s.f1f2 = (f1 ? 0x01 : 0x00) + (f2 ? 0x02 : 0x00);  // flags
  s.xEnd = xEnd;
  s.yEnd = yEnd;
  return (s);
};


void dumpSegments(std::vector<Segment> &vSeg) {
  TRACE("\nSegments: %zu\n", vSeg.size());

  for (Segment &seg : vSeg) {
    const char *s = "";
    if (seg.type == Segment::Type::Move) s = "M";
    if (seg.type == Segment::Type::Line) s = "L";
    if (seg.type == Segment::Type::Arc) s = "A";
    if (seg.type == Segment::Type::Close) s = "Z";
    if (seg.type == Segment::Type::Curve) s = "C";
    TRACE("  %s(0x%04x) - %d/%d\n", s, seg.type, seg.p[0], seg.p[1]);
  }
}

// ===== Edge functionality =====

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

void dumpEdges(std::vector<_Edge> &edges) {
  size_t size = edges.size();
  TRACE("\nEdges: %zu\n", size);
#if 0
  for (size_t i = 0; i < size; i++) {
    if (i % 10 == 0) {
      if (i > 0) { TRACE("\n"); }
      TRACE("  e%02d:", i);
    }
    TRACE(" (%3d/%3d)-%2d", edges[i].x, edges[i].y, edges[i].len);
  }
  TRACE("\n");
#endif
}

// =====

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
    int16_t p = static_cast<int16_t>(strtol(path, &path, 10));
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


/// @brief Draw a path with filling.
void fillSegments(std::vector<Segment> &segments, fSetPixel cbBorder, fSetPixel cbFill) {
  TRACE("fillSegments()\n");
  std::vector<_Edge> edges;
  _Edge *lastEdge = nullptr;

  size_t n;
  fSetPixel cbStroke = cbBorder ? cbBorder : cbFill;  // use cbFill when no cbBorder is given.

  // dumpSegments(segments);

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
  // dumpEdges(edges);

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
  // dumpEdges(edges);
  TRACE(" ... sort\n");

  // sort edges by ascending lines (y)
  std::sort(edges.begin(), edges.end(), _Edge::compare);
  TRACE(" ...a\n");

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


// /// @brief draw the a path.
// /// @param path The path definition using SVG path syntax.
// /// @param cbDraw Draw function for border pixels. cbFill is used when cbBorder is null.
// void drawPath(const char *pathText, fSetPixel cbDraw) {
//   std::vector<Segment> vSeg = parsePath(pathText);
//   drawSegments(vSeg, cbDraw);
// }


}  // gfxDraw:: namespace
