// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawPathWidget.cpp: Library implementation of a widget base class functionality
// based on vector graphics and paths with transformation, coloring options to simplify drawing with gfxDraw functions.
//  * All transformations are combined into a transformation matrix to avoid intermediate transformations with rounding
//    effects.
//  * The fill color can be specified using simple linear gradients.
//  * The Transformations can be
//  * Example of a gauge based on gfxDrawPathWidget that manipulates segments based on a given value.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDrawPathWidget.h and documentation files in this library.

// https://svg-path-visualizer.netlify.app/#M2%2C2%20Q8%2C2%208%2C8

#pragma once

#include "gfxDraw.h"
#include "gfxDrawColors.h"

namespace gfxDraw {

// Matrix type definition for transformation using 1000 factor and numbers.
typedef int32_t Matrix1000[3][3];


struct gfxDrawPathConfig {
  uint16_t x = 0;
  uint16_t y = 0;

  ARGB strokeColor;
  ARGB fillColor;
};

/// @brief A gfxDrawPathWidget is used to combine a path with stroke and fill coloring to simplify drawing of widgets.
/// It supports creating a path by text or segments and enables rotating and scaling.

class gfxDrawPathWidget {
public:
  // constructor functions

  gfxDrawPathWidget();

  void setConfig(gfxDrawPathConfig *c);

  // ===== coloring functions =====

  void setStrokeColor(gfxDraw::ARGB stroke);

  void setFillColor(gfxDraw::ARGB fill);

  // ===== path creation functions =====

  /// @brief Create segments from a textual path definition.
  void setPath(const char *path);

  /// @brief add a segment to the path.
  void addSegment(Segment s);

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
  void scale(int16_t factor, int16_t base = 100);

  /// @brief apply a rotation to the transformation matrix.
  /// @param angle Angle of rotation clockwise in degree.
  /// @param cx x-coordinate of the center of rotation. Default = 0.
  /// @param cy y-coordinate of the center of rotation. Default = 0.
  void rotate(int16_t angle, int16_t cx = 0, int16_t cy = 0);

  // ===== drawing functions =====

  /// @brief transform and draw the widget
  /// @param cbDraw Pixel drawing callback function
  void draw(gfxDraw::fDrawPixel cbDraw);

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

  /// @brief used for solid filling and gradient start color.
  gfxDraw::ARGB _fillColor1;

  int32_t m1000;
  int32_t b1000;

  // distance of the 2 gradient points.
  int32_t d1000;


  // Reference to background data
  class Background *_bg = nullptr;
};

}  // namespace gfxDraw

// End.
