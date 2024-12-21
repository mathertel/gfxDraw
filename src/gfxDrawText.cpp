// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawText.cpp: Library implementation file for functions to calculate all points of a staight line.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDrawText.h and documentation files in this library.
//
// - - - - -

#include "gfxDraw.h"

#include "fonts/fonts.h"

#ifndef GFX_TRACE
#define GFX_TRACE(...)  // GFXDRAWTRACE(__VA_ARGS__)
#endif


namespace gfxDraw {

struct _gfxDrawFontDetails {
  /// @brief font data
  const GFXfont *font;

  /// effective font height (above and below baseline used pixels)
  uint16_t height;

  // baseline of the font (max found pixels above)
  int16_t baseLine;
};

/// @brief current text cursor position
Point _textCursor;

/// @brief Font registry
std::vector<_gfxDrawFontDetails> _registry;

/// @brief current Font Details
const _gfxDrawFontDetails *_currentFont;

/// @brief current scaling factor
uint16_t _currentScale = 2;  // current font factor

/// @brief requested size for current font
uint16_t _currentSize = 0;



/// @brief Draw a single character at _textCursor and advance the _textCursor to the right
/// @param c The character to be drawn.
void drawChar(unsigned char c, fSetPixel cbDraw) {
  const GFXfont *_font;

  if (_currentFont) {
    _font = _currentFont->font;

    if ((c >= _font->first) && (c <= _font->last)) {
      uint8_t cOffset = c - _font->first;

      GFXglyph *glyph = _font->glyph + cOffset;
      uint8_t *bitmap = _font->bitmap;

      uint16_t bo = glyph->bitmapOffset;
      uint8_t bitCounter = 0;
      uint8_t bits = 0;

      int16_t xo = _textCursor.x + (_currentScale * glyph->xOffset);
      int16_t yo = _textCursor.y + _currentScale * (_currentFont->baseLine + glyph->yOffset);

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
            if (_currentScale == 1) {
              cbDraw(xo + xx, yo + yy);

            } else {
              for (int16_t fy = 0; fy < _currentScale; fy++) {
                for (int16_t fx = 0; fx < _currentScale; fx++) {
                  cbDraw(xo + (_currentScale * xx) + fx, yo + (_currentScale * yy) + fy);
                }
              }
            }
          }
          bits <<= 1;
        }
      }
      _textCursor.x += glyph->xAdvance * _currentScale;
    }  // if
  }  // if
}  // drawChar()


void addFont(const GFXfont *newFont) {
  GFX_TRACE("addFont()");

  _gfxDrawFontDetails f;
  int16_t baseline;  // baseline offset in the font.
  uint16_t height;   // current font height of chars (baseline + below baseline used pixels)

  if (newFont) {
    f.font = newFont;

    // find baseline height and total used height from Glyphs.
    baseline = 0;
    height = 0;

    GFXglyph *glyph = newFont->glyph;
    uint16_t chars = newFont->last - newFont->first + 1;

    for (uint16_t c = 0; c < chars; c++) {
      if (glyph[c].yOffset < baseline)
        baseline = glyph[c].yOffset;

      if (glyph[c].yOffset + glyph[c].height > height)
        height = glyph[c].yOffset + glyph[c].height;
    }
    f.baseLine = -baseline;
    f.height = f.baseLine + height;

    GFX_TRACE(" height=%d", f.height);
    GFX_TRACE(" baseLine=%d", f.baseLine);
    GFX_TRACE(" lineHeight=%d", f.font->yAdvance);

    // register new font Details
    _registry.push_back(f);
  }
}  // addFont()


// loading GFX fonts in the GFX Font Binary format (.gfxfntb)
// effectively the file is the in-memory of the GFX font structures:
// GFXfont + GFXglyph + Bitmaps  with pointers in GFXfont set to the offsets in the file.
// use https://github.com/ScottFerg56/GFXFontEditor to export binary files.
void loadFont(const char *fName) {
  GFX_TRACE("loadFont(%s)", fName);
  FILE *file;
  long size;

  file = fopen(fName, "rb");

  if (file) {
    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    rewind(file);

    GFXfont *mem = (GFXfont *)(malloc(size));

    if (mem) {
      size_t readsize = fread(mem, 1, size, file);
      if (size == readsize) {
        // re-base the pointers to the memory used for loading
        mem->glyph = (GFXglyph *)((char *)(mem) + (size_t)(mem->glyph));
        mem->bitmap = (uint8_t *)(mem) + (size_t)(mem->bitmap);
        addFont((const GFXfont *)mem);

      } else {
        free(mem);
      }
    }

    fclose(file);
  }
}  // loadFont()


/// @brief find the font from the registry that fits best for the given height.
/// sets the global variables:
/// _currentFont -- best found registered font.
/// _currentScale -- scaling factor to be used.
/// _currentSize -- the requested size,
void _findBestFont(int16_t size) {
  GFX_TRACE("_findBestFont(%d)", size);

  if (size && (size != _currentSize)) {
    // only search best font in case of a different size requested
    const _gfxDrawFontDetails *bestFont = nullptr;
    int16_t bestFit = 0;
    int16_t bestScale = 0;

    // search all fonts and find better fit.
    for (const _gfxDrawFontDetails &f : _registry) {
      // GFX_TRACE(" check f_%d...", f.height);

      int16_t scale = (size / f.height);
      int16_t fit = size - (scale * f.height);

      if ((!bestFont)
          || ((scale > 0) && (fit < bestFit))
          || ((scale > 0) && (scale < bestScale) && (fit == bestFit))) {
        // better font found.
        bestFont = &f;
        bestFit = fit;
        bestScale = scale ? scale : 1;
      }
    }  // for

    _currentFont = bestFont;
    _currentSize = size;
    _currentScale = bestScale;
    GFX_TRACE(" => f_%d * %d", _currentFont->height, _currentScale);
  }
}


// initialize at least one font.

void setupFont() {
  addFont(&font8);
  // addFont(&font10);
  // addFont(&font16);
  // addFont(&font24);
}


int16_t lineHeight(int16_t size) {
  _findBestFont(size);
  return (_currentFont->font->yAdvance * _currentScale);
};


Point textBox(int16_t size, const char *text) {
  Point dim(0, 0);

  if (size) {
    _findBestFont(size);
  }

  if ((_currentFont) && (text)) {
    const GFXfont *_font = _currentFont->font;
    int16_t lineWidth = 0;
    int16_t maxWidth = 0;
    int16_t lineCount = 1;

    // calculate char by char
    while (*text) {
      char c = *text++;

      if (c == '\n') {
        lineCount++;
        if (lineWidth > maxWidth) {
          maxWidth = lineWidth;
        }
        lineWidth = 0;
      } else if ((c >= _font->first) && (c <= _font->last)) {
        uint8_t cOffset = c - _font->first;
        GFXglyph *glyph = _font->glyph + cOffset;
        lineWidth += (glyph->xAdvance * _currentScale);
      }
    }

    if (lineWidth > maxWidth) {
      maxWidth = lineWidth;
    }

    dim.x = maxWidth;
    dim.y = (lineCount * _currentFont->height * _currentScale);
  }
  return (dim);
}  // textBox()


Point drawText(Point &p, int16_t size, const char *text, fSetPixel cbDraw) {
  _textCursor = p;

  if (size) {
    // select best font
    _findBestFont(size);
  }

  if (_currentFont) {
    // draw char by char and advance _textCursor
    while (*text) {
      char c = *text++;
      if (c == '\n') {
        _textCursor.x = p.x;  // reset to the beginning of the line
        _textCursor.y += _currentFont->height * _currentScale;  // move to the next line
      } else {
        drawChar(c, cbDraw);
      }
    }
  }
  return (_textCursor);
}  // drawText()


Point drawText(int16_t x, int16_t y, int16_t size, const char *text, fSetPixel cbDraw) {
  Point p(x, y);
  return (drawText(p, size, text, cbDraw));
}  // drawText()

}  // gfxDraw:: namespace

// End.
