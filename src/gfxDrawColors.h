// Pre-defined drawing Colors

#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <stdint.h>

namespace gfxDraw {

#pragma pack(push, 1)
/// @brief The RGBA class is used to define the color and opacity of a single abstract pixel.
class RGBA {
public:
  RGBA() = default;
  RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
  RGBA(uint32_t col24);

  uint8_t Red;
  uint8_t Green;
  uint8_t Blue;
  uint8_t Alpha;
  
  constexpr bool operator==(const RGBA &col2);
  constexpr bool operator!=(const RGBA &col2);

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
const RGBA RGBA_BLACK (    0,    0,    0);
const RGBA RGBA_SILVER( 0xDD, 0xDD, 0xDD);
const RGBA RGBA_GRAY  ( 0xCC, 0xCC, 0xCC);
const RGBA RGBA_RED   ( 0xFF,    0,    0);
const RGBA RGBA_ORANGE( 0xE9, 0x76,    0);
const RGBA RGBA_YELLOW( 0xF6, 0xC7,    0);
const RGBA RGBA_GREEN (    0, 0x80,    0);
const RGBA RGBA_LIME  ( 0x32, 0xCD, 0x32);
const RGBA RGBA_BLUE  (    0,    0, 0xFF);
const RGBA RGBA_CYAN  (    0, 0xFF, 0xFF);
const RGBA RGBA_PURPLE( 0x99, 0x46, 0x80);
const RGBA RGBA_WHITE ( 0xFF, 0xFF, 0xFF);

const RGBA RGBA_TRANSPARENT( 0x00, 0x00, 0x00, 0x00);
// c lang-format on

void dumpColorTable();

}  // namespace gfxDraw

// End.
