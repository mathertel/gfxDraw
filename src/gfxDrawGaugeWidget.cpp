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


// ===== private functions

Point gfxDrawGaugeWidget::_piePoint(int16_t alpha, uint16_t radius) {
  return (Point(
    sin256(alpha + 180) * radius / 256,
    cos256(alpha) * radius / 256));
}

void gfxDrawGaugeWidget::_drawSegment(gfxDraw::fSetPixel cbFill) {
  int16_t _innerRadius = (_radius * 8) / 10;
  bool largeFlag = (conf.maxAngle - conf.minAngle) > 180;

  Point startOut = center + _piePoint(conf.minAngle, _radius);
  Point startIn = center + _piePoint(conf.minAngle, _innerRadius);
  Point endOut = center + _piePoint(conf.maxAngle, _radius);
  Point endIn = center + _piePoint(conf.maxAngle, _innerRadius);

  _segments.clear();
  _segments.reserve(5);
  _segments.push_back(Segment::createMove(startOut));
  _segments.push_back(Segment::createArc(_radius, largeFlag, true, endOut.x, endOut.y));
  _segments.push_back(Segment::createLine(endIn));
  _segments.push_back(Segment::createArc(_innerRadius, largeFlag, false, startIn.x, startIn.y));
  _segments.push_back(Segment::createClose());

  fillSegments(_segments, nullptr, cbFill);
}

void gfxDrawGaugeWidget::_drawNeedle(gfxDraw::fSetPixel cbStroke) {
  Point vPoint = center + _piePoint(_valueAngle, _radius - 2);
  Point lPoint = center + _piePoint(_valueAngle + 120, 5);
  Point rPoint = center + _piePoint(_valueAngle - 120, 5);

  _segments.clear();
  _segments.reserve(5);
  _segments.push_back(Segment::createMove(lPoint));
  _segments.push_back(Segment::createLine(vPoint));
  _segments.push_back(Segment::createLine(rPoint));
  _segments.push_back(Segment::createClose());
  fillSegments(_segments, nullptr, cbStroke);

  // gfxDraw::drawLine(center.x, center.y, vPoint.x, vPoint.y, cbStroke);
}



void gfxDrawGaugeWidget::setConfig(GFXDrawGaugeConfig *c) {
  conf = *c;
  if (conf.w % 2 == 0) { conf.w--; }  // width must be odd number

  _radius = (conf.w - 1) / 2;

  center = Point(conf.x + _radius, conf.y + _radius + 1);  // why +1 ???
}


void gfxDrawGaugeWidget::setValue(float value) {
  value = constrain(value, conf.minValue, conf.maxValue);
  _valueAngle = (int16_t)map(value, conf.minValue, conf.maxValue, conf.minAngle, conf.maxAngle);
}  // setValue()


void gfxDrawGaugeWidget::draw(gfxDraw::fSetPixel cbStroke, gfxDraw::fSetPixel cbFill) {
  _drawSegment(cbFill);
  _drawNeedle(cbStroke);
}  // draw()



}  // namespace gfxDraw

// End.
