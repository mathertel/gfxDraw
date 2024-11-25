// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawGaugeWidget.cpp: Library implementation file for functions to calculate all points of a circle, circle quadrant and circle segment.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDrawGaugeWidget.h and documentation files in this library.
//
// - - - - -

#include "gfxDraw.h"
#include "gfxDrawGaugeWidget.h"

namespace gfxDraw {

// See https://reference.arduino.cc/reference/en/language/functions/math/constrain/
float constrain(float value, float in_min, float in_max) {
  if (value < in_min) value = in_min;
  if (value > in_max) value = in_max;
  return (value);
};

// see https://reference.arduino.cc/reference/en/language/functions/math/map/
float map(float value, float in_min, float in_max, float out_min, float out_max) {
  return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int16_t map(float value, float in_min, float in_max, int16_t out_min, int16_t out_max) {
  float result = (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  return (static_cast<int16_t>(std::lround(result)));
}

// ===== private functions

Point gfxDrawGaugeWidget::_piePoint(int16_t alpha, uint16_t radius) {
  return (Point(
    sin256(alpha + 180) * radius / 256,
    cos256(alpha) * radius / 256));
}

void gfxDrawGaugeWidget::_drawSegment(int16_t minAngle, int16_t maxAngle, gfxDraw::fSetPixel cbFill) {
  std::vector<gfxDraw::Segment> segments;

  int16_t _innerRadius = (_radius * 8) / 10;
  bool largeFlag = (maxAngle - minAngle) > 180;

  Point startOut = center + _piePoint(minAngle, _radius);
  Point startIn = center + _piePoint(minAngle, _innerRadius);
  Point endOut = center + _piePoint(maxAngle, _radius);
  Point endIn = center + _piePoint(maxAngle, _innerRadius);

  segments.reserve(5);
  segments.push_back(Segment::createMove(startOut));
  segments.push_back(Segment::createArc(_radius, largeFlag, true, endOut.x, endOut.y));
  segments.push_back(Segment::createLine(endIn));
  segments.push_back(Segment::createArc(_innerRadius, largeFlag, false, startIn.x, startIn.y));
  segments.push_back(Segment::createClose());

  fillSegments(segments, nullptr, cbFill);
}

void gfxDrawGaugeWidget::_drawNeedle(gfxDraw::fSetPixel cbStroke) {
  std::vector<gfxDraw::Segment> segments;
  Point vPoint = center + _piePoint(_valueAngle, _radius - 2);
  Point lPoint = center + _piePoint(_valueAngle + 120, 5);
  Point rPoint = center + _piePoint(_valueAngle - 120, 5);

  segments.reserve(4);
  segments.push_back(Segment::createMove(lPoint));
  segments.push_back(Segment::createLine(vPoint));
  segments.push_back(Segment::createLine(rPoint));
  segments.push_back(Segment::createClose());
  fillSegments(segments, nullptr, cbStroke);

  // gfxDraw::drawLine(center.x, center.y, vPoint.x, vPoint.y, cbStroke);
}



void gfxDrawGaugeWidget::setConfig(GFXDrawGaugeConfig *c) {
  conf = *c;
  if (conf.w % 2 == 0) { conf.w--; }  // width must be odd number

  _radius = (conf.w - 1) / 2;

  center = Point(conf.x + _radius, conf.y + _radius + 1);  // why +1 ???
}


void gfxDrawGaugeWidget::addSegment(float minValue, float maxValue, uint32_t color) {
  _GFXDrawGaugeSegment s;

  minValue = constrain(minValue, conf.minValue, conf.maxValue);
  s.minAngle = (int16_t)map(minValue, conf.minValue, conf.maxValue, conf.minAngle, conf.maxAngle);

  maxValue = constrain(maxValue, conf.minValue, conf.maxValue);
  s.maxAngle = (int16_t)map(maxValue, conf.minValue, conf.maxValue, conf.minAngle, conf.maxAngle),
  s.color = color;
  _gaugeSegments.push_back(s);
}

void gfxDrawGaugeWidget::setValue(float value) {
  value = constrain(value, conf.minValue, conf.maxValue);
  _valueAngle = (int16_t)map(value, conf.minValue, conf.maxValue, conf.minAngle, conf.maxAngle);
}  // setValue()


void gfxDrawGaugeWidget::draw(gfxDraw::fSetPixel cbStroke, gfxDraw::fSetPixel cbFill) {
  _drawSegment(conf.minAngle, conf.maxAngle, cbFill);
  _drawNeedle(cbStroke);
}  // draw()


void gfxDrawGaugeWidget::draw(gfxDraw::fDrawPixel cbDraw) {
  ARGB drawColor;
  auto drawHelper = [&](int16_t x, int16_t y) {
    cbDraw(x, y, drawColor);
  };

  // _GFXDrawGaugeSegment *seg = _gaugeSegments[0];
  int16_t a = conf.minAngle;

  // draw all gaugeSegments using the defined color
  for (_GFXDrawGaugeSegment &pSeg : _gaugeSegments) {
    if (a < pSeg.minAngle) {
      // draw a range without special colors
      drawColor = this->_segmentColor;
      _drawSegment(a, pSeg.minAngle, drawHelper);
      a = pSeg.minAngle;
    }
    // draw the gaugeSegment
    drawColor = pSeg.color;
    _drawSegment(pSeg.minAngle, pSeg.maxAngle, drawHelper);
    a = pSeg.maxAngle;
  }
  // draw remaining range if any
  if (a < conf.maxAngle) {
    drawColor = this->_segmentColor;
    _drawSegment(a, conf.maxAngle, drawHelper);
  }

  drawColor = this->_needleColor;
  _drawNeedle(drawHelper);
}



}  // namespace gfxDraw

// End.
