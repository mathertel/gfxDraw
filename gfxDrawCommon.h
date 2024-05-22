// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxdraw.h: Library header file
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// These pixel oriented drawing functions are implemented to use callback functions for the effective drawing
// to make them independent from an specific canvas or GFX implementation and can be used for drawing and un-drawing.
//
// The functions have minimized use of float and arc arithmetics.
// Path drawing is supporting on any given path.
// Filled paths are supported on closed paths only.
//
// CHANGELOG:
// 15.05.2024  creation of the GFXDraw library.
//
// - - - - -

#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <functional>

namespace gfxDraw {

/// @brief Callback function definition to address a pixel on a display
typedef std::function<void(int16_t x, int16_t y)> fSetPixel;

/// @brief Callback function to transform all points in the segments
typedef std::function<void(int16_t &x, int16_t &y)> fTransform;


#define POINT_BREAK_Y INT16_MAX
#define POINT_INVALID_Y INT16_MAX - 1

/// @brief The Point holds a pixel position and provides some useful static methods.
class Point {
public:
  Point()
    : x(0), y(POINT_INVALID_Y){};

  Point(int16_t _x, int16_t _y)
    : x(_x), y(_y){};

  /// @brief X coordinate of the Point
  int16_t x;

  /// @brief Y coordinate of the Point
  int16_t y;

  /// @brief compare function for std::sort to sort points by (y) and ascending (x)
  /// @param p1 first point
  /// @param p2 second point
  /// @return when p1 is lower than p2
  static bool compare(const Point &p1, const Point &p2) {
    if (p1.y != p2.y)
      return (p1.y < p2.y);
    return (p1.x < p2.x);
  };

  constexpr bool operator==(const Point &p2) {
    return ((x == p2.x) && (y == p2.y));
  };
};


}  // gfxDraw:: namespace


// typedef uint32_t COLOR32;

// /// @brief Callback function definition to address a pixel on a display
// typedef std::function<void(int16_t x, int16_t y, COLOR32 color)> fDrawPixel;



// End.
