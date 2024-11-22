// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawGaugeWidget.h: Display a value driven Gauge Widget.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog:
// * 22.11.2024 creation
//
// - - - - -

// https://svg-path-visualizer.netlify.app/#M2%2C2%20Q8%2C2%208%2C8

#pragma once

#include "gfxDraw.h"
#include "gfxDrawCommon.h"

#include "gfxDrawLine.h"  // just for now

namespace gfxDraw {

// Matrix type definition for transformation using 1000 factor and numbers.
typedef int32_t Matrix1000[3][3];


/// @brief A gfxDrawGaugeWidget is used to define the dimensions and colors to draw gauges.


// a stroke path and coloring of a graphical object.
/// It supports creating paths by simple functions like lines and rectangles or by using the svg path notation (not fully supported)
/// The stroke color can be set.
/// The fill color can be set to solid or gradient functions.
/// TODO: collect the current colors from the canvas so the object can be "un-drawn" later.

// The Gauge is configured by the following struct:
// String title,
// String unit
// uint16_t x, y, w = h = 2*outerRadius,
// inner radius is 80% of outer radius
// float minValue, Values are limited to this minimum showing the gauge at the lowest angle.
// float maxValue, Values are limited to this maximum showing the gauge at the highest angle.
// float minAngle, lowest angle used for the lowest value.
// float maxAngle, highest angle used for the lowest value.
// color fill: Filling Color of the gauge.
//
// segments: [
//   { value: 18, color: '#aaaadd' },
//   { value: 22, color: '#33cc33' },
//   { color: '#ff4444' }
// ]

// Angles are given in 360° and Angle = 0 is exactly down.
// A good start and end pair of angles is 45 to 360-45 or 90 to 270


class gfxDrawGaugeWidget {
public:
  struct GFXDrawGaugeConfig {
    uint16_t x;
    uint16_t y;
    uint16_t w;  //  = h = 2*outerRadius,

    float minValue;
    float maxValue;

    float minAngle;
    float maxAngle;
  };

  gfxDrawGaugeWidget() {

  };

  void setConfig(GFXDrawGaugeConfig *c);
  void setValue(float value);

  // ===== drawing functions =====

  /// @brief transform and draw the widget
  /// @param cbDraw Pixel drawing callback function
  /// @param cbRead optional Pixel reading current color from the image
  /// when `cbRead` is present the background of the drawing is collected and saved to an internal background image.
  void draw(gfxDraw::fSetPixel cbStroke, gfxDraw::fSetPixel cbFill);


  // /// @brief undo drawing by restoring all background pixels.
  // /// @param cbDraw Pixel drawing callback function
  // void undraw(gfxDraw::fDrawPixel cbDraw);

private:
  Point center;
  int16_t _radius;

  int16_t _valueAngle;

  Point _piePoint(int16_t alpha, uint16_t radius);

  void _drawSegment(gfxDraw::fSetPixel cbFill);

  void _drawNeedle(gfxDraw::fSetPixel cbStroke);

  GFXDrawGaugeConfig conf;
  std::vector<gfxDraw::Segment> _segments;
};

}  // namespace gfxDraw

// End.
