// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawSprite.cpp: Library implementation file for functions to save a image in memory.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDrawSprite.h and documentation files in this library.
//
// - - - - -

#include "gfxDraw.h"
#include "gfxDrawSprite.h"

#ifndef GFX_TRACE
#define GFX_TRACE(...)  // GFXDRAWTRACE(__VA_ARGS__)
#endif

namespace gfxDraw {

Sprite::Sprite() {
  _x = _y = _w = _h = 0;
}

Sprite::Sprite(int16_t x, int16_t y, int16_t w, int16_t h)
  : _x(x), _y(y), _w(w), _h(h) {
  // allocate data memory
  data.resize(_w * _h);  // initialized with 0, all pixels with Alpha 0
};

void Sprite::drawPixel(int16_t x, int16_t y, ARGB color) {
  // GFX_TRACE("bg::set(%d,%d)=%08x", x, y, color.raw);
  if (_w == 0) {
    _createData(x, y);
    GFX_TRACE(" init to (%d/%d)-(%d/%d)", _x, _y, _x + _w - 1, _y + _h - 1);
  }

  if (x < _x) {
    _insertLeft(_x - x);
    GFX_TRACE(" resize to (%d/%d)-(%d/%d)", _x, _y, _x + _w - 1, _y + _h - 1);

  } else if (x >= _x + _w) {
    _insertRight(x - (_x + _w) + 1);
    GFX_TRACE(" resize to (%d/%d)-(%d/%d)\n", _x, _y, _x + _w - 1, _y + _h - 1);
  }

  if (y < _y) {
    _insertTop(_y - y);
    GFX_TRACE(" resize to (%d/%d)-(%d/%d)", _x, _y, _x + _w - 1, _y + _h - 1);

  } else if (y >= _y + _h) {
    _insertBottom(y - (_y + _h) + 1);
    GFX_TRACE(" resize to (%d/%d)-(%d/%d)", _x, _y, _x + _w - 1, _y + _h - 1);
  }

  ARGB *cell = &data[(x - _x) + (y - _y) * _w];
  *cell = color;  // save the background color
}  // drawPixel


void Sprite::draw(Point pos, fDrawPixel cbDraw) {
  GFX_TRACE("Sprite::draw()");
  int16_t xPos = _x + pos.x;
  int16_t yPos = _y + pos.y;

  for (int16_t y = 0; y < _h; y++) {
    uint32_t rowOffset = (y * _w);
    for (int16_t x = 0; x < _w; x++) {
      ARGB col = data[rowOffset++];
      if (col.Alpha > 0) {
        cbDraw(xPos + x, yPos + y, col);
      }
    }
  }
}  // draw()


// // changeColor: map all given pixels by color
// void changeColors(std::function<ARGB(ARGB color)> f) {
//   GFX_TRACE("Background::changeColors()");

//   for (int16_t y = 0; y < _h; y++) {
//     for (int16_t x = 0; x < _w; x++) {
//       ARGB *cell = &data[(y * _w) + x];
//       *cell = f(*cell);
//     }
//   }
// }  // changeColors();


// // mark all Alpha=0xFF (current) with Alpha=0xFE (old)
// void markOld() {
//   GFX_TRACE("markOld()");

//   for (int16_t y = 0; y < _h; y++) {
//     for (int16_t x = 0; x < _w; x++) {
//       uint32_t offset = (y * _w) + x;
//       if (data[offset].Alpha == 0xFF)
//         data[offset].Alpha = 0xFE;
//     }
//   }
// }  // markOld();

// first initializing the data
void Sprite::_createData(int16_t x, int16_t y) {
  // first initialization.
  _x = x & 0xFFF0;
  _y = y;
  _w = 16;
  _h = 16;
  data.resize(16 * 16);  // initialized with 0, all pixels with Alpha 0
};


// resize the array based 0/0
void Sprite::_insertTop(uint16_t count) {
  ARGB blank;

  // expand by multiple of 16
  count = (count + 15) & 0xFFF0;
  int16_t y2 = _y - count;
  int16_t h2 = _h + count;

  GFX_TRACE(" t-expand %d -> %d", _h, h2);
  data.reserve(_w * h2);  // alloc at once.

  // insert blanks on top
  data.insert(data.begin(), count * _w, blank);

  _y = y2;
  _h = h2;
}  // _insertTop()


// resize the array based 0/0
void Sprite::_insertLeft(uint16_t count) {
  ARGB blank;  // (0, 0, 0, GFXD_ALPHA_UNUSED);

  // expand by multiple of 16
  count = (count + 15) & 0xFFF0;
  int16_t x2 = _x - count;
  int16_t w2 = _w + count;

  GFX_TRACE(" l-expand %d -> %d", _w, w2);
  data.reserve(w2 * _h);  // alloc at once.

  // insert blanks on each line
  for (int16_t l = 0; l < _h; l++) {
    data.insert(data.begin() + (l * w2), count, blank);
  }
  _x = x2;
  _w = w2;
}  // _insertLeft


// resize the array based 0/0
void Sprite::_insertRight(uint16_t count) {
  ARGB blank;  // (0, 0, 0, GFXD_ALPHA_UNUSED);

  // expand by multiple of 16
  count = (count + 15) & 0xFFF0;
  int16_t w2 = _w + count;
  GFX_TRACE(" r-expand %d -> %d", _w, w2);

  data.reserve(w2 * _h);  // alloc at once.

  // insert blanks on each line
  for (int16_t l = 0; l < _h; l++) {
    data.insert(data.begin() + (l * w2) + _w, count, blank);
  }

  _w = w2;
}  // _insertRight


// resize the array based 0/0
void Sprite::_insertBottom(uint16_t count) {
  ARGB blank;  // ARGB blank(0, 0, 0, GFXD_ALPHA_UNUSED);

  // expand by multiple of 16
  count = (count + 15) & 0xFFF0;
  int16_t h2 = _h + count;
  GFX_TRACE(" b-expand %d -> %d", _h, h2);

  data.reserve(_w * h2);  // alloc at once.

  data.insert(data.end(), count * _w, blank);
  _h = h2;
}  // _insertBottom

}  // namespace