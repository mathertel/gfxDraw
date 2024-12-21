// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawSprite.h: Implementation of a dynamic-size 2D bitmap.
//  * Whena pixel is modified the bitmap will be expanded when required.
//  * ARGB 32-bit coloring.
// Alpha channel can be used for makring pixels as transparent, old, new.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog:
// * 20.12.2024 universal Sprite base clase

#pragma once

#include "gfxDraw.h"

namespace gfxDraw {

// ===== Sprite ====

/// The Sprite class allows off-screen pixels to be saved and used at a later time for various purpose.
///
/// * drawing something in memory and transferring to the display at a time to avid flickering.
/// * drawing an image that was loaded from disk.
/// * un-drawing something ba capturing the background pixels.
///
/// This sprite class can be used without prior knowing the required size because it is extending automatically the internal buffer.

class Sprite {
public:
  // initialize a new Sprite without data.
  Sprite();

  Sprite(int16_t x, int16_t y, int16_t w, int16_t h);

  void drawPixel(int16_t x, int16_t y, ARGB color);

  /// @brief draw all current non-transparent pixels
  void draw(Point pos, fDrawPixel cbDraw);

private:
  /// @brief Left boundary of the image
  int16_t _x;
  /// @brief Top boundary of the image
  int16_t _y;
  /// @brief Width of the image
  int16_t _w;
  /// @brief Height of the image
  int16_t _h;

  /// @brief Allocated data
  std::vector<ARGB> data;

  // first initializing the data
  void _createData(int16_t x, int16_t y);

  // resize the data array to include more pixels at the top
  void _insertTop(uint16_t count);

  // resize the data array to include more pixels at the left
  void _insertLeft(uint16_t count);

  // resize the data array to include more pixels at the right
  void _insertRight(uint16_t count);

  // resize the data array to include more pixels at the bottom
  void _insertBottom(uint16_t count);

};  // class Sprite




}  // namespace gfxDraw

// End.
