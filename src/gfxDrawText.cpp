// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawLine.cpp: Library implementation file for functions to calculate all points of a staight line.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDrawLine.h and documentation files in this library.
//
// - - - - -

#include "gfxDrawText.h"

#include "fonts/font10.h"


#define TRACE(...)  // printf(__VA_ARGS__)

namespace gfxDraw {



Point _textCursor;  // current text cursor position

// current font and its dimensions.
const GFXfont *_font;   // current font data
uint16_t _fontHeight;   // current font height of chars (baseline + below baseline used pixels)
int16_t _fontBaseLine;  // baseline offset in the font.

uint16_t _fontSize;        // current font size (as specified by parameter)
uint16_t _fontRealSize;    // current effective font size (equal or less than _fontSize)
uint16_t _fontFactor = 2;  // current font factor


/// @brief Draw a single character at _textCursor and advance the _textCursor to the right
/// @param c The character to be drawn.
void drawChar(unsigned char c, fSetPixel cbDraw) {

  if ((_font) && (c >= _font->first) && (c <= _font->last)) {
    uint8_t cOffset = c - _font->first;

    GFXglyph *glyph = _font->glyph + cOffset;
    uint8_t *bitmap = _font->bitmap;

    uint16_t bo = glyph->bitmapOffset;
    uint8_t bitCounter = 0;
    uint8_t bits = 0;

    int16_t xo = _textCursor.x + (_fontFactor * glyph->xOffset);
    int16_t yo = _textCursor.y + _fontFactor * (_fontBaseLine + glyph->yOffset);

    // Pointer to character specific bitmap.
    uint8_t *characterBytes = &bitmap[glyph->bitmapOffset];

    for (uint8_t yy = 0; yy < glyph->height; yy++) {
      for (uint8_t xx = 0; xx < glyph->width; xx++) {

        if (!(bitCounter++ & 7)) {
          // need a new byte from the bitmap
          bits = *characterBytes++;
        }

        // set pixels
        if (bits & 0x80) {
          if (_fontFactor == 1) {
            cbDraw(xo + xx, yo + yy);

          } else {
            for (int16_t fy = 0; fy < _fontFactor; fy++) {
              for (int16_t fx = 0; fx < _fontFactor; fx++) {
                cbDraw(xo + (_fontFactor * xx) + fx, yo + (_fontFactor * yy) + fy);
              }
            }
          }
        }
        bits <<= 1;
      }
    }
    _textCursor.x += glyph->xAdvance * _fontFactor;
  }  // if
}


// https://github.com/ScottFerg56/GFXFontEditor#readme

void setupFont() {
  addFont(&Font_10);
}


Point textBox(int16_t h, const char *text) {
  Point dim(0, 0);

  if (text) {

    // calculate char by char
    while (*text) {
      char c = *text++;

      if ((c >= _font->first) && (c <= _font->last)) {
        uint8_t cOffset = c - _font->first;
        GFXglyph *glyph = _font->glyph + cOffset;
        dim.x += (glyph->xAdvance * _fontFactor);
      }
    }
    dim.y = (_fontHeight * _fontFactor);
  }
  return (dim);
}  // textBox()


void addFont(const GFXfont *newFont) {
  // register new font
  _font = newFont;

  // find baseline height and total used height from Glyphs.
  _fontBaseLine = 0;
  _fontHeight = 0;

  GFXglyph *glyph = _font->glyph;
  uint16_t chars = _font->last - _font->first + 1;

  if (_font) {
    for (uint16_t c = 0; c < chars; c++) {
      if (glyph[c].yOffset < _fontBaseLine)
        _fontBaseLine = glyph[c].yOffset;

      if (glyph[c].yOffset + glyph[c].height > _fontHeight)
        _fontHeight = glyph[c].yOffset + glyph[c].height;
    }
  }
  _fontBaseLine = -_fontBaseLine;
  _fontHeight += _fontBaseLine;
}


void loadFont() {
  // https://github.com/ScottFerg56/BinaryFontDemo/tree/main
}


void drawText(Point &p, int16_t size, const char *text, fSetPixel cbDraw) {
  _textCursor = p;



  if (size) {
    // select best font
  }

  // draw char by char
  while (*text) {
    drawChar(*text++, cbDraw);
  }
}


void drawText(int16_t x, int16_t y, int16_t size, const char *text, fSetPixel cbDraw) {
  Point p(x, y);
  drawText(p, size, text, cbDraw);
}

}  // gfxDraw:: namespace

// End.
