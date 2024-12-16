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

#ifndef GFX_TRACE
#define GFX_TRACE(...)  // GFXDRAWTRACE(__VA_ARGS__)
#endif

namespace gfxDraw {

// See https://reference.arduino.cc/reference/en/language/functions/math/constrain/
float constrain_float(float value, float in_min, float in_max) {
  if (value < in_min) value = in_min;
  if (value > in_max) value = in_max;
  return (value);
};


// See https://reference.arduino.cc/reference/en/language/functions/math/map/
int16_t map(float value, float in_min, float in_max, int16_t out_min, int16_t out_max) {
  float result = (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  return (static_cast<int16_t>(std::lround(result)));
}


gfxDrawGaugeWidget::gfxDrawGaugeWidget(gfxDrawGaugeConfig *c) {
  _conf = *c; // copy
  _radius = (_conf.w - 1) / 2;
  _centerPoint = Point(_conf.x + _radius, _conf.y + _radius + 1);  // why +1 ???

  if ((_conf.scaleSteps > 0) && (_conf.minValue + _conf.scaleSteps < _conf.maxValue)) {
    _scaleAngleStep = (int16_t)map(_conf.minValue + _conf.scaleSteps, _conf.minValue, _conf.maxValue, _conf.minAngle, _conf.maxAngle)
                      - _conf.minAngle;
  };

  if (!_conf.pointerPath) {
    _conf.pointerPath = "M-60,-60L0,999L60-60Z";
  }
  _pointerSegments = parsePath(_conf.pointerPath);
  _conf.pointerPath = nullptr;
}  // gfxDrawGaugeWidget()


void gfxDrawGaugeWidget::addSegment(float minValue, float maxValue, ARGB color) {
  _GFXDrawGaugeSegment s;

  minValue = constrain_float(minValue, _conf.minValue, _conf.maxValue);
  s.minAngle = map(minValue, _conf.minValue, _conf.maxValue, _conf.minAngle, _conf.maxAngle);

  maxValue = constrain_float(maxValue, _conf.minValue, _conf.maxValue);
  s.maxAngle = map(maxValue, _conf.minValue, _conf.maxValue, _conf.minAngle, _conf.maxAngle),
  s.color = color;
  _segments.push_back(s);
}

void gfxDrawGaugeWidget::setValue(float value) {
  value = constrain_float(value, _conf.minValue, _conf.maxValue);
  _valueAngle = map(value, _conf.minValue, _conf.maxValue, _conf.minAngle, _conf.maxAngle);
}  // setValue()


void gfxDrawGaugeWidget::draw(gfxDraw::fDrawPixel cbDraw) {
  ARGB drawColor;
  auto drawHelper = [&](int16_t x, int16_t y) {
    cbDraw(x, y, drawColor);
  };

  // draw segments
  int16_t a = _conf.minAngle;

  // draw all gaugeSegments using the defined color
  for (_GFXDrawGaugeSegment &pSeg : _segments) {
    if (a < pSeg.minAngle) {
      // draw a range without special colors
      drawColor = _conf.segmentColor;
      _drawSegment(a, pSeg.minAngle, drawHelper);
      a = pSeg.minAngle;
    }
    // draw the gaugeSegment
    drawColor = pSeg.color;
    _drawSegment(pSeg.minAngle, pSeg.maxAngle, drawHelper);
    a = pSeg.maxAngle;
  }
  // draw remaining range if any
  if (a < _conf.maxAngle) {
    drawColor = _conf.segmentColor;
    _drawSegment(a, _conf.maxAngle, drawHelper);
  }

  // draw scale
  if (_scaleAngleStep > 0) {
    int16_t _outerRadius = (_radius * _conf.scaleRadius) / 100;
    int16_t _innerRadius = (_radius * (_conf.scaleRadius - _conf.scaleWidth)) / 100;

    drawColor = _conf.pointerColor;
    a = _conf.minAngle;

    while (a <= _conf.maxAngle) {
      Point pOut = _centerPoint + _piePoint(a, _outerRadius);
      Point pIn = _centerPoint + _piePoint(a, _innerRadius);
      drawLine(pOut, pIn, drawHelper);
      a += _scaleAngleStep;
    }
  }

  // draw pointer
  drawColor = _conf.pointerColor;
  std::vector<gfxDraw::Segment> segments = _pointerSegments;

  rotateSegments(segments, _valueAngle);
  scaleSegments(segments, _radius, 1000);
  moveSegments(segments, _centerPoint);
  fillSegments(segments, nullptr, drawHelper);
} // draw()


// ===== private functions

Point gfxDrawGaugeWidget::_piePoint(int16_t alpha, uint16_t radius) {
  // GFXDRAWTRACE("piePoint %d, r=%d", alpha, radius);

  return (Point(
    sin256(alpha + 180) * radius / 256,
    cos256(alpha) * radius / 256));
}

void gfxDrawGaugeWidget::_drawSegment(int16_t minAngle, int16_t maxAngle, gfxDraw::fSetPixel cbFill) {
  std::vector<gfxDraw::Segment> segments;

  int16_t _outerRadius = (_radius * _conf.segmentRadius) / 100;
  int16_t _innerRadius = (_radius * (_conf.segmentRadius - _conf.segmentWidth)) / 100;
  bool largeFlag = (maxAngle - minAngle) > 180;

  Point startOut = _centerPoint + _piePoint(minAngle, _outerRadius);
  Point startIn = _centerPoint + _piePoint(minAngle, _innerRadius);
  Point endOut = _centerPoint + _piePoint(maxAngle, _outerRadius);
  Point endIn = _centerPoint + _piePoint(maxAngle, _innerRadius);

  segments.reserve(5);
  segments.push_back(Segment::createMove(startOut));
  segments.push_back(Segment::createArc(_outerRadius, largeFlag, true, endOut.x, endOut.y));
  segments.push_back(Segment::createLine(endIn));
  segments.push_back(Segment::createArc(_innerRadius, largeFlag, false, startIn.x, startIn.y));
  segments.push_back(Segment::createClose());

  fillSegments(segments, nullptr, cbFill);
}  // _drawSegment()

}  // namespace gfxDraw

// End.
