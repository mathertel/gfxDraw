// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawPathWidget.cpp: Library implementation of a widget base class functionality
// based on vector graphics and paths with transformation, coloring options to simplify drawing with gfxDraw functions.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDrawPathWidget.h and documentation files in this library.
//
// - - - - -

#include "gfxDrawPathWidget.h"

// #include "gfxDrawBitmap.h"

#ifndef GFX_TRACE
#define GFX_TRACE(...)  // GFXDRAWTRACE(__VA_ARGS__)
#endif

namespace gfxDraw {

gfxDrawPathWidget::gfxDrawPathWidget() {
  _initMatrix(_matrix);
}


void gfxDrawPathWidget::setConfig(gfxDrawPathConfig *c) {
  _stroke = c->strokeColor;

  if (c->fillColor.Alpha > 0) {
    _fillColor1 = c->fillColor;
  }
  _initMatrix(_matrix);
  // rotate
  // scale
  move(c->x, c->y);
}


/// @brief Create segments from a textual path definition.
void gfxDrawPathWidget::setPath(const char *path) { 
  _segments = parsePath(path);
  _initMatrix(_matrix);
}

/// @brief add a segment to the path.
void gfxDrawPathWidget::addSegment(Segment s) {
  _segments.push_back(s);
}


// ===== Transformation Matrix Manipulations =====


void gfxDrawPathWidget::resetTransformation() {
  _initMatrix(_matrix);
}


// apply the scaling factors to the transformation matrix;
void gfxDrawPathWidget::move(int16_t dx, int16_t dy) {
  if ((dx != 0) || (dy != 0)) {
    Matrix1000 MoveMatrix;
    _initMatrix(MoveMatrix);
    MoveMatrix[0][2] = dx * 1000;
    MoveMatrix[1][2] = dy * 1000;
    _multiplyMatrix(_matrix, MoveMatrix);
  }
};


// apply the scaling factors to the transformation matrix;
void gfxDrawPathWidget::scale(int16_t factor, int16_t base) {
  if (factor != base) {
    Matrix1000 scaleMatrix;
    _initMatrix(scaleMatrix);
    scaleMatrix[0][0] = scaleMatrix[1][1] = ((factor * 1000) + (base / 2)) / base;
    _multiplyMatrix(_matrix, scaleMatrix);
  }
};


// apply the rotation factors to the transformation matrix.
// rotating is using the center 0/0 by default
void gfxDrawPathWidget::rotate(int16_t angle, int16_t cx, int16_t cy) {
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

    int32_t sinFactor1000 = std::lround(sin(radians) * 1000);
    int32_t cosFactor1000 = std::lround(cos(radians) * 1000);

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

void gfxDrawPathWidget::_extendBox(int16_t x, int16_t y) {
  if (x < x_min) { x_min = x; }
  if (x > x_max) { x_max = x; }

  if (y < y_min) { y_min = y; }
  if (y > y_max) { y_max = y; }
}

void gfxDrawPathWidget::draw(gfxDraw::fDrawPixel cbDraw) {
  GFX_TRACE("draw()");
  GFX_TRACE(" stroke= %08lX", _stroke.raw);
  GFX_TRACE(" fill  = %08lX", _fillColor1.raw);

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

  // draw...
  if (_fillColor1.Alpha == 0) {
    // need to draw the border pixels only
    gfxDraw::drawSegments(tSegments, [&](int16_t x, int16_t y) {
      if (y < POINT_INVALID_Y) {
        _extendBox(x, y);
        cbDraw(x, y, _stroke);
      }
    });

  } else if ((_stroke.Alpha > 0)) {
    gfxDraw::fillSegments(
      tSegments,
      [&](int16_t x, int16_t y) {
        _extendBox(x, y);
        cbDraw(x, y, _stroke);
      },
      [&](int16_t x, int16_t y) {
        _extendBox(x, y);
        cbDraw(x, y, _fillColor1);
      });

  } else if (_stroke.Alpha == 0) {
    gfxDraw::fillSegments(
      tSegments,
      nullptr,
      [&](int16_t x, int16_t y) {
        _extendBox(x, y);
        cbDraw(x, y, _fillColor1);
      });
  }
};

// ===== gradient filling =====

void gfxDrawPathWidget::setStrokeColor(gfxDraw::ARGB stroke) {
  GFX_TRACE("gfx::setStroke %08lx", stroke.raw);
  _stroke = stroke;
};

void gfxDrawPathWidget::setFillColor(gfxDraw::ARGB fill) {
  GFX_TRACE("gfx::setFill %08lx", fill.raw);
  _fillColor1 = fill;
};

/// ===== Transformations

// Initialize a transformation matrix
void gfxDrawPathWidget::_initMatrix(Matrix1000 &m) {
  memset(&m, 0, sizeof(m));
  m[0][0] = 1000;
  m[1][1] = 1000;
  m[2][2] = 1000;
}

// Matrix muliplicates for combining transformations.
void gfxDrawPathWidget::_multiplyMatrix(Matrix1000 &m1, Matrix1000 &m2) {
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
