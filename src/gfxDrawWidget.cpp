// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawWidget.cpp: Library implementation of a widget base class functionality
// based on vector graphics and paths with transformation, coloring options to simplify drawing with gfxDraw functions.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDrawWidget.h and documentation files in this library.
//
// - - - - -

#include "gfxDrawWidget.h"

#include "gfxDrawBitmap.h"

/// unconditional print out this
#define LOG_PRINT(...)  // GFXD_TRACE(__VA_ARGS__)

// tracing information (left from development for future problem analysis) can be disabled
#define LOG_TRACE(...)  // GFXD_TRACE(__VA_ARGS__)


namespace gfxDraw {

/// @brief Create segments from a textual path definition.
void gfxDrawWidget::setPath(const char *path) {
  _segments = parsePath(path);
  _initMatrix(_matrix);
}

/// @brief add a segment to the path.
void gfxDrawWidget::addSegment(Segment s) {
  _segments.push_back(s);
}

/// @brief Create segments that form a with rectangle.
/// @param width width of the rectangle.
/// @param height height of the rectangle.
void gfxDrawWidget::setRect(int16_t width, int16_t height) {
  _segments.push_back(Segment(Segment::Move, 0, 0));
  _segments.push_back(Segment(Segment::Line, 0, height - 1));
  _segments.push_back(Segment(Segment::Line, width - 1, height - 1));
  _segments.push_back(Segment(Segment::Line, width - 1, 0));
  _segments.push_back(Segment(Segment::Close));
}



// ===== Transformation Matrix Manipulations =====


void gfxDrawWidget::resetTransformation() {
  _initMatrix(_matrix);
}

void gfxDrawWidget::resetBackground() {
  free(_bg);  //  = new Background();
  _bg = nullptr;
}

// apply the scaling factors to the transformation matrix;
void gfxDrawWidget::move(int16_t dx, int16_t dy) {
  if ((dx != 0) || (dy != 0)) {
    Matrix1000 MoveMatrix;
    _initMatrix(MoveMatrix);
    MoveMatrix[0][2] = dx * 1000;
    MoveMatrix[1][2] = dy * 1000;
    _multiplyMatrix(_matrix, MoveMatrix);
  }
};

// apply the scaling factors to the transformation matrix;
void gfxDrawWidget::scale(int16_t scale100) {
  if (scale100 != 100) {
    Matrix1000 scaleMatrix;
    _initMatrix(scaleMatrix);
    scaleMatrix[0][0] = scaleMatrix[1][1] = scale100 * 10;
    _multiplyMatrix(_matrix, scaleMatrix);
  }
};


// apply the rotation factors to the transformation matrix.
// rotating is using the center 0/0 by default
void gfxDrawWidget::rotate(int16_t angle, int16_t cx, int16_t cy) {
  if (angle != 0) {
    Matrix1000 m;

    if ((cx != 0) && (cy != 0)) {
      // move given center to 0/0
      _initMatrix(m);
      m[0][2] = -cx * 1000;
      m[1][2] = -cy * 1000;
      _multiplyMatrix(_matrix, m);
    }

    double radians = (angle * M_PI) / 180;

    int32_t sinFactor1000 = floor(sin(radians) * 1000);
    int32_t cosFactor1000 = floor(cos(radians) * 1000);

    _initMatrix(m);
    m[0][0] = m[1][1] = cosFactor1000;
    m[1][0] = sinFactor1000;
    m[0][1] = -sinFactor1000;

    _multiplyMatrix(_matrix, m);

    if ((cx != 0) && (cy != 0)) {
      // move given center back
      _initMatrix(m);
      m[0][2] = cx * 1000;
      m[1][2] = cy * 1000;
      _multiplyMatrix(_matrix, m);
    }
  }
};


// ===== Drawing =====

void gfxDrawWidget::_extendBox(int16_t x, int16_t y) {
  if (x < x_min) { x_min = x; }
  if (x > x_max) { x_max = x; }

  if (y < y_min) { y_min = y; }
  if (y > y_max) { y_max = y; }
}

void gfxDrawWidget::draw(gfxDraw::fDrawPixel cbDraw, gfxDraw::fReadPixel cbRead) {
  LOG_TRACE("draw()");
  LOG_TRACE(" stroke= %08lX", _stroke.raw);
  LOG_TRACE(" fill  = %08lX", _fillColor1.raw);

  x_min = INT16_MAX;
  y_min = INT16_MAX;
  x_max = INT16_MIN;
  y_max = INT16_MIN;

  // create a copy
  std::vector<gfxDraw::Segment> tSegments = _segments;

  // transform with matrix
  transformSegments(tSegments, [&](int16_t &x, int16_t &y) {
    int32_t tx, ty;
    tx = x * _matrix[0][0] + y * _matrix[0][1] + _matrix[0][2] + 500;
    ty = x * _matrix[1][0] + y * _matrix[1][1] + _matrix[1][2] + 500;

    x = tx / 1000;
    y = ty / 1000;
  });

  // create Background
  if (cbRead) {
    if (_bg) {
      // mark all real background (Alpha 0xFF) pixels with (Alpha=0xFE == "old background")
      _bg->changeColors([&](ARGB color) {
        if (color.Alpha == 0xFF) color.Alpha = 0xFE;
        return (color);
      });

    } else {
      _bg = new Background();
    }
  }

  // draw...
  if (_fillColor1.Alpha == 0) {
    // need to draw the border pixels only
    gfxDraw::drawSegments(tSegments, [&](int16_t x, int16_t y) {
      if (y < POINT_INVALID_Y) {
        if (cbRead) { _bg->set(x, y, cbRead(x, y)); }
        _extendBox(x, y);
        cbDraw(x, y, _stroke);
      }
    });

  } else if ((_stroke.Alpha > 0)) {
    gfxDraw::fillSegments(
      tSegments,
      [&](int16_t x, int16_t y) {
        if (cbRead) { _bg->set(x, y, cbRead(x, y)); }
        _extendBox(x, y);
        cbDraw(x, y, _stroke);
      },
      [&](int16_t x, int16_t y) {
        if (cbRead) { _bg->set(x, y, cbRead(x, y)); }
        _extendBox(x, y);
        cbDraw(x, y, _getColor(x, y));
      });

  } else if (_stroke.Alpha == 0) {
    gfxDraw::fillSegments(
      tSegments,
      nullptr,
      [&](int16_t x, int16_t y) {
        if (cbRead) { _bg->set(x, y, cbRead(x, y)); }
        _extendBox(x, y);
        cbDraw(x, y, _getColor(x, y));
      });
  }

  // draw background pixels that where not touched during drawing and restore the background color
  if (_bg) {
    // uint8_t qa = (drawOld ? 0xFE : 0xFF);
    _bg->draw(
      [&](int16_t x, int16_t y, ARGB color) {
        if (color.Alpha == 0xFE)
          cbDraw(x, y, color);
      });

    _bg->changeColors([&](ARGB color) {
      if (color.Alpha == 0xFE) color.Alpha = 0xFF;
      return (color);
    });
  }
};


void gfxDrawWidget::undraw(gfxDraw::fDrawPixel cbDraw) {
  LOG_TRACE("gfxDrawWidget::undraw()");
  if (_bg) {
    LOG_TRACE("  .bg");
    _bg->draw(cbDraw);
  }
  resetBackground();
}  // undraw

// ===== gradient filling =====

void gfxDrawWidget::setStrokeColor(gfxDraw::ARGB stroke) {
  LOG_TRACE("gfx::setStroke %08lx", stroke.raw);
  _stroke = stroke;
};

void gfxDrawWidget::setFillColor(gfxDraw::ARGB fill) {
  LOG_TRACE("gfx::setFill %08lx", fill.raw);
  _fillMode = Solid;
  _fillColor1 = fill;
};


void gfxDrawWidget::setFillGradient(gfxDraw::ARGB fill1, int16_t x1, int16_t y1, gfxDraw::ARGB fill2, int16_t x2, int16_t y2) {
  LOG_TRACE("setFillGradient(#%08lx %d/%d #%08lx %d/%d )\n", fill1.toColor24(), x1, y1, fill2.toColor24(), x2, y2);

  _fillColor1 = fill1;
  _fillColor2 = fill2;
  _gradientX1 = x1;
  _gradientY1 = y1;

  if (y1 == y2) {
    _fillMode = Horizontal;
    d1000 = (x2 - x1) * 1000;

  } else if (x1 == x2) {
    _fillMode = Vertical;
    d1000 = (y2 - y1) * 1000;

  } else {
    _fillMode = Linear;

    // pre-calculate all factors for dist calculation in linear Gradient color function.
    // instead of using floats use the factor 1000.
    // Line formula: `y = mx + b` for the line where fill1 color will be used;
    // This is the orthogonal line through p1 (slope = 1/m)
    int16_t dx = (x2 - x1);
    int16_t dy = (y2 - y1);
    LOG_PRINT(" dx=%d, dy=%d\n", dx, dy);

    m1000 = -dx * 1000 / dy;  // = -2000
    LOG_PRINT(" m1000=%d\n", m1000);
    b1000 = (y1 * 1000) - (m1000 * x1);  // 6000 - ( -2000 * 4)
    LOG_PRINT(" b1000=%d\n", b1000);

    // distance of the 2 gradient points.
    d1000 = sqrt(dx * dx + dy * dy) * 1000;

    // Distance formula: `d = (mx - y + b) / sqrt(m*m + 1)`
    // divisor for the distance formula
    int16_t nen1000 = sqrt(m1000 * m1000 + 1000000);
    LOG_PRINT(" nen1000=%d\n", nen1000);

    // `y = (m1000 * x + b1000) / 1000`

    // distance between the 2 points:
    // int32_t d = sqrt((x2 - x1) * (y2 - y1));

    // distance of point x/y from this line:
    // d = (mx - y + b) /  sqrt(m*m + 1) // (-2 * x - y + 14) / sqrt(4 +1)
    // (-2 * 10 - 9 + 14) / sqrt(2*2 +1)

    // sqrt(m*m + 1) = sqrt(m1000*m1000 + 1*1000*1000) / 1000
    // can be simplified with a small error by just
    // = sqrt(m1000 / 1000 * m1000 / 1000 + 1)
    // = sqrt(m1000 / 1000 * m1000 / 1000 ) = m/1000

    // d = (m1000 x - y * 1000 + b1000) /  sqrt(m*m + 1)
    // (-2000 * 10 - 9 * 1000 + 14000) / sqrt(m1000*m1000 / 1000 / 1000 + 1*1000*1000)
    // (-2000 * 9 - 5 * 1000 + 14000) / sqrt(m1000*m1000 / 1000 / 1000 + 1*1000*1000)

    int16_t px = 5;
    int16_t py = 9;
    int16_t pd = 1000 * ((1000 * py - m1000 * px - b1000) + 500) / nen1000;
    LOG_PRINT(" pd=%d\n", pd);
  };
  LOG_PRINT(" d1000=%d\n", d1000);
};


/// @brief calculate gradient color of coordinate x/y
/// @param x
/// @param y
/// @return
gfxDraw::ARGB gfxDrawWidget::_getColor(int16_t x, int16_t y) {
  if (_fillMode == Solid) {
    return (_fillColor1);

  } else if (_fillMode == Horizontal) {
    int32_t dp1000 = (x - _gradientX1) * 1000;
    int32_t f100 = (dp1000 * 100) / d1000;

    if (f100 <= 0) {
      return (_fillColor1);

    } else if (f100 >= 100) {
      return (_fillColor2);

    } else {
      int32_t q100 = (100 - f100);
      gfxDraw::ARGB col = gfxDraw::ARGB(
        (q100 * _fillColor1.Red + f100 * _fillColor2.Red) / 100,
        (q100 * _fillColor1.Green + f100 * _fillColor2.Green) / 100,
        (q100 * _fillColor1.Blue + f100 * _fillColor2.Blue) / 100,
        (q100 * _fillColor1.Alpha + f100 * _fillColor2.Alpha) / 100);
      return (col);
    };

  } else if (_fillMode == Vertical) {
    int32_t dp1000 = (y - _gradientY1) * 1000;
    int32_t f100 = (dp1000 * 100) / d1000;

    if (f100 <= 0) {
      return (_fillColor1);

    } else if (f100 >= 100) {
      return (_fillColor2);

    } else {
      int32_t q100 = (100 - f100);
      gfxDraw::ARGB col = gfxDraw::ARGB(
        (q100 * _fillColor1.Red + f100 * _fillColor2.Red) / 100,
        (q100 * _fillColor1.Green + f100 * _fillColor2.Green) / 100,
        (q100 * _fillColor1.Blue + f100 * _fillColor2.Blue) / 100,
        (q100 * _fillColor1.Alpha + f100 * _fillColor2.Alpha) / 100);
      return (col);
    };
  } else {
    // not implemented yet
    return (gfxDraw::ARGB_PURPLE);
  }
}

/// ===== Transformations

// Initialize a transformation matrix
void gfxDrawWidget::_initMatrix(Matrix1000 &m) {
  memset(&m, 0, sizeof(m));
  m[0][0] = 1000;
  m[1][1] = 1000;
  m[2][2] = 1000;
}

// Matrix muliplicates for combining transformations.
void gfxDrawWidget::_multiplyMatrix(Matrix1000 &m1, Matrix1000 &m2) {
  Matrix1000 r;
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      r[x][y] = 0;
      for (int k = 0; k < 3; k++) {
        r[x][y] += (m2[x][k] * m1[k][y]);
      }
      r[x][y] /= 1000;
    }
  }
  memcpy(&m1, &r, sizeof(Matrix1000));
};

}  // gfxDraw namespace


// End.
