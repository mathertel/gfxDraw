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

// ===== ARGB class members =====


// RGB+A Color
ARGB::ARGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
  : Red(r), Green(g), Blue(b), Alpha(a){};

// raw 32 bit color with no transparency
ARGB::ARGB(uint32_t color)
  : raw(color) {
  Alpha = 0xFF;
};

constexpr bool ARGB::operator==(const ARGB &col2) {
  return (raw == col2.raw);
}

constexpr bool ARGB::operator!=(const ARGB &col2) {
  return (raw != col2.raw);
}

/// @brief Convert into a 3*8 bit value using #RRGGBB.
/// @return color value.
uint32_t ARGB::toColor24() {
  return (raw & 0x0FFFFFF);
}

// Convert into a 16 bit value using 5(R)+6(G)+5(B)
uint16_t ARGB::toColor16() {
  return ((((Red) & 0xF8) << 8) | (((Green) & 0xFC) << 3) | ((Blue) >> 3));
}


// ===== gfxDraw helper functions =====

void dumpColor(const char *name, ARGB col) {
  TRACE(" %-12s: %02x.%02x.%02x.%02x %08lx\n", name, col.Alpha, col.Red, col.Green, col.Blue, col.toColor24());
  int_fast16_t t1 = INT16_MIN;
}

void dumpColorTable() {
  TRACE("        Color: A  R  G  B  #col24\n");
  dumpColor("Red", gfxDraw::ARGB_RED);
  dumpColor("Green", gfxDraw::ARGB_GREEN);
  dumpColor("Blue", gfxDraw::ARGB_BLUE);
  dumpColor("Orange", gfxDraw::ARGB_ORANGE);
  dumpColor("Transparent", gfxDraw::ARGB_TRANSPARENT);
}





}  // gfxDraw:: namespace

// End.
