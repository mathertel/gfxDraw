// Pre-defined drawing Colors

#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <stdint.h>

namespace gfxDraw {

#pragma pack(push, 1)
/// @brief The ARGB class is used to define the opacity and color for a single abstract pixel.
/// using 32-bits per pixel for Alpha, Red, Green, Blue
/// The layout of this class is to also have a raw 32-bit value in format #aaRRGGBB
/// and conversions to 16-bit values using 0brrrrrggggggbbbbb.

class ARGB {
public:
  ARGB() = default;

  /// create color by components.
  ARGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

  /// create opaque color.
  ARGB(uint32_t raw);

  union {
    struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      uint8_t Blue;
      uint8_t Green;
      uint8_t Red;
      uint8_t Alpha;
#else
      uint8_t Alpha;
      uint8_t Red;
      uint8_t Green;
      uint8_t Blue;
#endif
    };
    uint32_t raw;  // equals #AArrggbb
  };

  constexpr bool
  operator==(const ARGB &col2);
  constexpr bool operator!=(const ARGB &col2);

  /// @brief Convert into a 3*8 bit value using #rrggbb.
  /// @return color value.
  uint32_t toColor24();

  /// @brief Convert into a 16 bit value using 5(R)+6(G)+5(B) .
  /// @return color value.
  uint16_t toColor16();
};
#pragma pack(pop)


// Some useful constants for simple colors

// clang-format off
const ARGB ARGB_BLACK (    0,    0,    0);
const ARGB ARGB_SILVER( 0xDD, 0xDD, 0xDD);
const ARGB ARGB_GRAY  ( 0xCC, 0xCC, 0xCC);
const ARGB ARGB_RED   ( 0xFF,    0,    0);
const ARGB ARGB_ORANGE( 0xE9, 0x76,    0);
const ARGB ARGB_YELLOW( 0xF6, 0xC7,    0);
const ARGB ARGB_GREEN (    0, 0x80,    0);
const ARGB ARGB_LIME  ( 0x32, 0xCD, 0x32);
const ARGB ARGB_BLUE  (    0,    0, 0xFF);
const ARGB ARGB_CYAN  (    0, 0xFF, 0xFF);
const ARGB ARGB_PURPLE( 0x99, 0x46, 0x80);
const ARGB ARGB_WHITE ( 0xFF, 0xFF, 0xFF);

const ARGB ARGB_TRANSPARENT( 0x00, 0x00, 0x00, 0x00);
// c lang-format on

void dumpColorTable();

}  // namespace gfxDraw

// End.
