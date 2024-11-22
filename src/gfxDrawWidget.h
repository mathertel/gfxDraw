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
// Changelog: See gfxDrawWidget.h and documentation files in this library.

// https://svg-path-visualizer.netlify.app/#M2%2C2%20Q8%2C2%208%2C8

#pragma once

#include "gfxDraw.h"
#include "gfxDrawColors.h"

namespace gfxDraw {

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

  /// @brief add a segment to the path.
  void addSegment(Segment s);

  /// @brief Create segments that form a with rectangle.
  /// @param width width of the rectangle.
  /// @param height height of the rectangle.
  void setRect(int16_t width, int16_t height);


  // ===== transformation functions =====

  // the transformations are recorded in an internal 3*3 matrix
  // and is applied just before drawing.

  /// @brief reset the internal transformation matrix
  void resetTransformation();

  /// @brief reset the internaly saved background
  void resetBackground();


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


  // ===== coloring functions =====

  void setStrokeColor(gfxDraw::ARGB stroke);

  void setFillColor(gfxDraw::ARGB fill);

  void setFillGradient(gfxDraw::ARGB fill1, int16_t x1, int16_t y1, gfxDraw::ARGB fill2, int16_t x2, int16_t y2);


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


  /// @brief calculate gradient color of coordinate x/y
  /// @param x
  /// @param y
  /// @return
  gfxDraw::ARGB _getColor(int16_t x, int16_t y);


  /// @brief Initialize a transformation matrix
  /// @param m Matrix
  void _initMatrix(Matrix1000 &m);

  /// @brief Matrix muliplicates for combining transformations.
  /// @param m1 Matrix1
  /// @param m2 Matrix2
  void _multiplyMatrix(Matrix1000 &m1, Matrix1000 &m2);

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
