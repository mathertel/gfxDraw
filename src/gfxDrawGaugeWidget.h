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


namespace gfxDraw {

// Matrix type definition for transformation using 1000 factor and numbers.
typedef int32_t Matrix1000[3][3];


/// @brief A gfxDrawGaugeWidget is used to define the dimensions, features and colors to draw gauges.


struct gfxDrawGaugeConfig {
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
  ARGB pointerColor = ARGB_BLACK;
  /// @brief Color for drawing the default gauge segment.
  ARGB segmentColor = 0xFF6699FF;

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

  uint16_t scaleRadius = 100;
  uint16_t scaleWidth = 5;
  float scaleSteps = 0;  // no scale with scaleSteps == 0

  const char *pointerPath = nullptr;
};


class gfxDrawGaugeWidget {
public:
  /// @brief initialize the gauge widget on creation with the configuration of the gauge display.
  /// @param conf configuration structure.
  gfxDrawGaugeWidget(gfxDrawGaugeConfig *conf);

  /// @brief add a segment with a defined color.
  /// @param minValue minimal value for the segment.
  /// @param maxValue maximal value for the segment.
  /// @param color The color for the segment.
  void addSegment(float minValue, float maxValue, ARGB color);

  /// @brief set the current value to be displayed.
  /// @param value The value.
  void setValue(float value);

  /// @brief Draw the widget
  /// @param cbDraw Pixel drawing callback function
  void draw(gfxDraw::fDrawPixel cbDraw);

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

  uint16_t _scaleAngleStep = 0;

  /// @brief Vector of Segments for drawing the pointer 
  std::vector<gfxDraw::Segment> _pointerSegments;

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

  /// @brief configuration of the Gauge Widget
  gfxDrawGaugeConfig _conf;

  /// @brief configuration of special colored segments
  std::vector<_GFXDrawGaugeSegment> _segments;
};

}  // namespace gfxDraw

// End.
