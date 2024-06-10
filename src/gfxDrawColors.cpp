// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawColors.cpp: Library implementation for handling Color values.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDraw.h and documentation files in this library.
//
// - - - - -


#include "gfxDrawColors.h"


#define TRACE(...)  // printf(__VA_ARGS__)

namespace gfxDraw {

// ===== RGBA class members =====

RGBA::RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
  : Red(r), Green(g), Blue(b), Alpha(a){};

RGBA::RGBA(uint32_t col24) {
  Red = (col24 >> 16) & 0xFF;
  Green = (col24 >> 8) & 0xFF;
  Blue = col24 & 0xFF;
  Alpha = 0xFF;
};

constexpr bool RGBA::operator==(const RGBA &col2) {
  return ((Red == col2.Red) && (Green == col2.Green) && (Blue == col2.Blue) && (Alpha == col2.Alpha));
}

constexpr bool RGBA::operator!=(const RGBA &col2) {
  return ((Red != col2.Red) || (Green != col2.Green) || (Blue != col2.Blue) || (Alpha != col2.Alpha));
}

/// @brief Convert into a 3*8 bit value using #RRGGBB.
/// @return color value.
uint32_t RGBA::toColor24() {
  return ((Red << 16) | (Green << 8) | Blue);
}

// Convert into a 16 bit value using 5(R)+6(G)+5(B)
uint16_t RGBA::toColor16() {
  return ((((Red) & 0xF8) << 8) | (((Green) & 0xFC) << 3) | ((Blue) >> 3));
}

// ===== gfxDraw helper functions =====

void dumpColor(char *name, RGBA col) {
  TRACE(" %-12s: %02x.%02x.%02x.%02x %08lx\n", name, col.Alpha, col.Red, col.Green, col.Blue, col.toColor24());
  int_fast16_t t1 = INT16_MIN;
}

void dumpColorTable() {
  TRACE("        Color: A  R  G  B  #col24\n");
  dumpColor("Red", gfxDraw::RGBA_RED);
  dumpColor("Green", gfxDraw::RGBA_GREEN);
  dumpColor("Blue", gfxDraw::RGBA_BLUE);
  dumpColor("Orange", gfxDraw::RGBA_ORANGE);
  dumpColor("Transparent", gfxDraw::RGBA_TRANSPARENT);
}





}  // gfxDraw:: namespace

// End.
