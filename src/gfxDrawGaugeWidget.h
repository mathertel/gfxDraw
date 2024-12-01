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
//
// The variant of the draw function that takes a stroke and fill callback
// will use the fill color for drawing the segment and the stroke color for drawing the pointer.
//
// Advanced coloring and multiple segments is available by using the addSegment function
// to specify the value range and the color of the segments.

// is a For coloring it is req
// color fill: Filling Color of the gauge.
//
// segments: [
//   { value: 18, color: '#aaaadd' },
//   { value: 22, color: '#33cc33' },
//   { color: '#ff4444' }
// ]

// Angles are given in 360° and Angle = 0 is exactly down.
// A good start and end pair of angles is 45 to 360-45 or 90 to 270


struct GFXDrawGaugeConfig {
public:

  /// @brief x position of the bounding box for the gauge
  uint16_t x;
  /// @brief y position of the bounding box for the gauge
  uint16_t y;
  /// @brief width of the bounding box for the gauge
  uint16_t w;
  /// @brief height of the bounding box for the gauge
  uint16_t h;

  /// @brief Color for drawing the pointer
  ARGB strokeColor;
  /// @brief Color for drawing the default gauge segment.
  ARGB fillColor;

  /// @brief minimal value that the gauge can show.
  float minValue;
  /// @brief maximal value that the gauge can show.
  float maxValue;

  /// @brief minimal overall anagle for the gauge segments.
  int16_t minAngle;
  /// @brief maximal overall anagle for the gauge segments.
  int16_t maxAngle;

  /// @brief outer radius of the segments in percentage of the available radius.
  uint16_t segmentRadius = 100;
  /// @brief width of the segments in percentage of the available radius.
  uint16_t segmentWidth = 35;
};


class gfxDrawGaugeWidget {
public:
  gfxDrawGaugeWidget() = default;

  /// @brief initialize with the configuration of the gauge display.
  /// @param conf configuration structure. 
  void setConfig(GFXDrawGaugeConfig *conf);

  /// @brief add a segment with a defined color.
  /// @param minValue minimal value for the segment.
  /// @param maxValue maximal value for the segment.
  /// @param color The color for the segment.
  void addSegment(float minValue, float maxValue, ARGB color);

  /// @brief set the current value to be displayed.
  /// @param value The value.
  void setValue(float value);


  // ===== drawing functions =====

  /// @brief transform and draw the widget
  /// @param cbDraw Pixel drawing callback function
  void draw(gfxDraw::fDrawPixel cbDraw);

  // /// @brief undo drawing by restoring all background pixels.
  // /// @param cbDraw Pixel drawing callback function
  // void undraw(gfxDraw::fDrawPixel cbDraw);

private:
  /// @brief Internal structure for a segment definition.
  struct _GFXDrawGaugeSegment {
    int16_t minAngle = 0;
    int16_t maxAngle = 0;
    ARGB color;
  };

  /// @brief Center Point of the gauge display.
  Point _centerPoint;
  /// @brief Maximal radius of the gauge
  int16_t _radius;

  /// @brief THe angle of the current value.
  int16_t _valueAngle;

  // default color definitions
  ARGB _pointerColor = ARGB_BLACK;
  ARGB _segmentColor = 0xFF4060A0;

  /// @brief calculate the Point at angle and radius.
  /// @param alpha The Angle in degree
  /// @param radius The Radius in pixels
  /// @return Point for drawing segments and scales.
  Point _piePoint(int16_t alpha, uint16_t radius);

  /// @brief Draw a single segment.
  /// @param minAngle 
  /// @param maxAngle 
  /// @param cbFill 
  void _drawSegment(int16_t minAngle, int16_t maxAngle, gfxDraw::fSetPixel cbFill);

  /// @brief Draw the pointer
  /// @param cbStroke 
  void _drawPointer(gfxDraw::fSetPixel cbStroke);

  GFXDrawGaugeConfig conf;
  std::vector<_GFXDrawGaugeSegment> _gaugeSegments;
};

}  // namespace gfxDraw

// End.
