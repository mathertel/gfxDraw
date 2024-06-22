// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawWidget.cpp: Library implementation of a widget base class functionality
// based on vector graphics and paths with transformation, coloring options to simplify drawing with gfxDraw functions.
//  * All transformations are combined into a transformation matrix to avoid intermediate transformations with rounding
//    effects.
//  * The fill color can be specified using simple linear gradients.
//  * The Transformations can be
//  * Example of a gauge based on gfxDrawWidget that manipulates segments based on a given value.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDraw.h and documentation files in this library.

// https://svg-path-visualizer.netlify.app/#M2%2C2%20Q8%2C2%208%2C8

#pragma once

#include "gfxDraw.h"
#include "gfxDrawColors.h"

#define TRACE(...)  // printf(__VA_ARGS__)

namespace gfxDraw {

/// @brief Callback function definition to address a pixel on a display
typedef std::function<void(int16_t x, int16_t y, ARGB color)> fDrawPixel;

/// @brief Callback function definition to read a pixel from a display
typedef std::function<ARGB(int16_t x, int16_t y)> fReadPixel;


// Matrix type definition for transformation using 1000 factor and numbers.
typedef int32_t Matrix1000[3][3];


/// @brief A gfxDrawWidget is used to define a stroke path and coloring of a graphical object.
/// It supports creating paths by simple functions like lines and rectangles or by using the svg path notation (not fully supported)
/// The stroke color can be set.
/// The fill color can be set to solid or gradient functions.
/// TODO: collect the current colors from the canvas so the object can be "un-drawn" later.

class gfxDrawWidget {
public:
  enum FillMode { None,
                  Solid,
                  Horizontal,
                  Vertical,
                  Linear };

  // constructor functions

  gfxDrawWidget() {
    _fillMode = None;
    _stroke = gfxDraw::ARGB_BLACK;
    _initMatrix(_matrix);
  };

  gfxDrawWidget(gfxDraw::ARGB stroke, gfxDraw::ARGB fill)
    : gfxDrawWidget() {

    _stroke = stroke;
    if (fill.Alpha != 0) {
      _fillMode = Solid;
      _fillColor1 = fill;
    }
  };

  gfxDrawWidget(const char *pathText, gfxDraw::ARGB stroke, gfxDraw::ARGB fill)
    : gfxDrawWidget(stroke, fill) {
    setPath(pathText);
  };


  // ===== path creation functions =====

  /// @brief Create segments from a textual path definition.
  void setPath(const char *path);

  /// @brief Create segments that form a with rectangle.
  /// @param width width of the rectangle.
  /// @param height height of the rectangle.
  void setRect(int16_t width, int16_t height);


  // ===== coloring functions =====

  void setStrokeColor(gfxDraw::ARGB stroke) {
    _stroke = stroke;
  };

  void setFillColor(gfxDraw::ARGB fill) {
    _fillMode = Solid;
    _fillColor1 = fill;
  };


  // ===== transformation functions =====

  // the transformations are recorded in an internal 3*3 matrix
  // and is applied just before drawing.

  /// @brief reset the internal transformation matrix
  void resetTransformation();

  /// @brief apply a movement vector to the transformation matrix.
  /// @param dx moving in x direction.
  /// @param dy moving in y direction.
  void move(int16_t dx, int16_t dy);

  /// @brief apply a scaling factor to the transformation matrix.
  /// @param scale100 scaling in percent.
  void scale(int16_t scale100);

  /// @brief apply a rotation to the transformation matrix.
  /// @param angle Angle of rotation clockwise in degree.
  /// @param cx x-coordinate of the center of rotation. Default = 0.
  /// @param cy y-coordinate of the center of rotation. Default = 0.
  void rotate(int16_t angle, int16_t cx = 0, int16_t cy = 0);


  // ===== drawing functions =====

  /// @brief transform and draw the widget
  /// @param cbDraw Pixel drawing callback function
  /// @param cbRead optional Pixel reading current color from the image
  /// when `cbRead` is present the background of the drawing is collected and saved to an internal background image.
  void draw(gfxDraw::fDrawPixel cbDraw, gfxDraw::fReadPixel cbRead = nullptr);


  /// @brief undo drawing by restoring all background pixels.
  /// @param cbDraw Pixel drawing callback function
  void undraw(gfxDraw::fDrawPixel cbDraw);


  // ===== gradient filling =====

  void setFillGradient(gfxDraw::ARGB fill1, int16_t x1, int16_t y1, gfxDraw::ARGB fill2, int16_t x2, int16_t y2) {
    TRACE("setFillGradient(#%08lx %d/%d #%08lx %d/%d )\n", fill1.toColor24(), x1, y1, fill2.toColor24(), x2, y2);

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
      TRACE(" dx=%d, dy=%d\n", dx, dy);

      m1000 = -dx * 1000 / dy;  // = -2000
      TRACE(" m1000=%d\n", m1000);
      b1000 = (y1 * 1000) - (m1000 * x1);  // 6000 - ( -2000 * 4)
      TRACE(" b1000=%d\n", b1000);

      // distance of the 2 gradient points.
      d1000 = sqrt(dx * dx + dy * dy) * 1000;

      // Distance formula: `d = (mx - y + b) / sqrt(m*m + 1)`
      // divisor for the distance formula
      int16_t nen1000 = sqrt(m1000 * m1000 + 1000000);
      TRACE(" nen1000=%d\n", nen1000);

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
      TRACE(" pd=%d\n", pd);
    };
    TRACE(" d1000=%d\n", d1000);
  };



  /// @brief calculate color of coordinate x/y
  /// @param x
  /// @param y
  /// @return
  gfxDraw::ARGB _getColor(int16_t x, int16_t y) {
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

  // outer dimension of last drawn Widget
  int16_t x_min;
  int16_t y_min;
  int16_t x_max;
  int16_t y_max;

protected:
  std::vector<gfxDraw::Segment> _segments;

private:
  // combined transformation matrix
  Matrix1000 _matrix;


  void _initMatrix(Matrix1000 &m) {
    memset(&m, 0, sizeof(m));
    m[0][0] = 1000;
    m[1][1] = 1000;
    m[2][2] = 1000;
  }

  void _multiplyMatrix(Matrix1000 &m1, Matrix1000 &m2) {
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

  void _extendBox(int16_t x, int16_t y);

  // Stroke coloring
  gfxDraw::ARGB _stroke;

  // Fill coloring
  FillMode _fillMode;

  /// @brief used for solid filling and gradient start color.
  gfxDraw::ARGB _fillColor1;

  /// @brief used for gradient end color.
  gfxDraw::ARGB _fillColor2;

  // point of fillColor1
  int16_t _gradientX1 = 0;
  int16_t _gradientY1 = 0;

  int32_t m1000;
  int32_t b1000;

  // distance of the 2 gradient points.
  int32_t d1000;


  // Reference to background data
  class Background *_bg = nullptr;
};

}  // namespace gfxDraw

// End.
