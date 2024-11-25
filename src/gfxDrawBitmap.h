// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawBitmap.h: Implementation of a dynamic-size 2D bitmap.
//  * Whena pixel is modified the bitmap will be expanded when required.
//  * ARGB 32-bit coloring.
// Alpha channel can be used for makring pixels as transparent, old, new.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDraw.h and documentation files in this library.

#pragma once

#include "gfxDraw.h"
#include "gfxDrawColors.h"

#define BMP_TRACE(...) //  GFXD_TRACE(__VA_ARGS__)

#define GFXD_ALPHA_UNUSED 0x00
#define GFXD_ALPHA_BACKGROUND 0xFF
#define GFXD_ALPHA_OLDBACK 0xFE

namespace gfxDraw {

// ===== Background ====

/// @brief The Background class is used to save the background of the drawn pixels for later restore / undraw.
class Background {
public:
  // initialize without data.
  Background() {
    _x = _y = _w =_h = 0;
  };

  void set(int16_t x, int16_t y, ARGB color) {
    // BMP_TRACE("bg::set(%d,%d)=%08x", x, y, color.raw);
    if (_w == 0) {
      _createData(x, y);
      BMP_TRACE(" init to (%d/%d)-(%d/%d)", _x, _y, _x + _w - 1, _y + _h - 1);
    }

    if (x < _x) {
      _insertLeft(_x - x);
      BMP_TRACE(" resize to (%d/%d)-(%d/%d)", _x, _y, _x + _w - 1, _y + _h - 1);

    } else if (x >= _x + _w) {
      _insertRight(x - (_x + _w) + 1);
      BMP_TRACE(" resize to (%d/%d)-(%d/%d)\n", _x, _y, _x + _w - 1, _y + _h - 1);
    }

    if (y < _y) {
      _insertTop(_y - y);
      BMP_TRACE(" resize to (%d/%d)-(%d/%d)", _x, _y, _x + _w - 1, _y + _h - 1);

    } else if (y >= _y + _h) {
      _insertBottom(y - (_y + _h) + 1);
      BMP_TRACE(" resize to (%d/%d)-(%d/%d)", _x, _y, _x + _w - 1, _y + _h - 1);
    }

    ARGB *cell = &data[(x - _x) + (y - _y) * _w];
    if (cell->Alpha > 0) {
      // LOG_PRINT("double write at (%d,%d)!", x, y);
      cell->Alpha = GFXD_ALPHA_BACKGROUND;  // known background is kept as background of new drawing.
    } else {
      *cell = color;  // save the background color
    }
  }


  // draw all current non-transparent pixels
  void draw(gfxDraw::fDrawPixel cbDraw) {
    BMP_TRACE("Background::draw()");

    for (int16_t y = 0; y < _h; y++) {
      for (int16_t x = 0; x < _w; x++) {
        uint32_t offset = (y * _w) + x;
        ARGB col = data[offset];
        if (col.Alpha > 0) {
          cbDraw(x + _x, y + _y, col);
        }
      }
    }
  }  // draw();


  // changeColor: map all given pixels by color
  void changeColors(std::function<ARGB(ARGB color)> f) {
    BMP_TRACE("Background::changeColors()");

    for (int16_t y = 0; y < _h; y++) {
      for (int16_t x = 0; x < _w; x++) {
        ARGB *cell = &data[(y * _w) + x];
        *cell = f(*cell);
      }
    }
  }  // changeColors();


  // // mark all Alpha=0xFF (current) with Alpha=0xFE (old)
  // void markOld() {
  //   BMP_TRACE("markOld()");

  //   for (int16_t y = 0; y < _h; y++) {
  //     for (int16_t x = 0; x < _w; x++) {
  //       uint32_t offset = (y * _w) + x;
  //       if (data[offset].Alpha == 0xFF)
  //         data[offset].Alpha = 0xFE;
  //     }
  //   }
  // }  // markOld();


private:
  int16_t _x;
  int16_t _y;
  int16_t _w;
  int16_t _h;
  std::vector<ARGB> data;

  // first initializing the data
  void _createData(int16_t x, int16_t y) {
    // first initialization.
    _x = x & 0xFFF0;
    _y = y;
    _w = 16;
    _h = 16;
    data.resize(16 * 16);  // initialized with 0, all pixels with Alpha 0
  };


  // resize the array based 0/0
  void _insertTop(uint16_t count) {
    ARGB blank('T', 'T', 'T', GFXD_ALPHA_UNUSED);

    // expand by multiple of 16
    count = (count + 15) & 0xFFF0;
    int16_t y2 = _y - count;
    int16_t h2 = _h + count;

    BMP_TRACE(" t-expand %d -> %d", _h, h2);
    data.reserve(_w * h2);  // alloc at once.

    // insert blanks on top
    data.insert(data.begin(), count * _w, blank);

    _y = y2;
    _h = h2;
  }  // _insertTop()


  // resize the array based 0/0
  void _insertLeft(uint16_t count) {
    ARGB blank(0, 0, 0, GFXD_ALPHA_UNUSED);

    // expand by multiple of 16
    count = (count + 15) & 0xFFF0;
    int16_t x2 = _x - count;
    int16_t w2 = _w + count;

    BMP_TRACE(" l-expand %d -> %d", _w, w2);
    data.reserve(w2 * _h);  // alloc at once.

    // insert blanks on each line
    for (int16_t l = 0; l < _h; l++) {
      data.insert(data.begin() + (l * w2), count, blank);
    }
    _x = x2;
    _w = w2;
  }  // _insertLeft


  // resize the array based 0/0
  void _insertRight(uint16_t count) {
    ARGB blank(0, 0, 0, GFXD_ALPHA_UNUSED);

    // expand by multiple of 16
    count = (count + 15) & 0xFFF0;
    int16_t w2 = _w + count;
    BMP_TRACE(" r-expand %d -> %d", _w, w2);

    data.reserve(w2 * _h);  // alloc at once.

    // insert blanks on each line
    for (int16_t l = 0; l < _h; l++) {
      data.insert(data.begin() + (l * w2) + _w, count, blank);
    }

    _w = w2;
  }  // _insertRight


  // resize the array based 0/0
  void _insertBottom(uint16_t count) {
    ARGB blank(0, 0, 0, GFXD_ALPHA_UNUSED);

    // expand by multiple of 16
    count = (count + 15) & 0xFFF0;
    int16_t h2 = _h + count;
    BMP_TRACE(" b-expand %d -> %d", _h, h2);

    data.reserve(_w * h2);  // alloc at once.

    data.insert(data.end(), count * _w, blank);
    _h = h2;
  }  // _insertBottom
};  // class Background




}  // namespace gfxDraw

// End.
