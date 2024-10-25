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

extern ARGB const ARGB_BLACK;
extern ARGB const ARGB_SILVER;
extern ARGB const ARGB_GRAY;
extern ARGB const ARGB_RED;
extern ARGB const ARGB_ORANGE;
extern ARGB const ARGB_YELLOW;
extern ARGB const ARGB_GREEN;
extern ARGB const ARGB_LIME;
extern ARGB const ARGB_BLUE;
extern ARGB const ARGB_CYAN;
extern ARGB const ARGB_PURPLE;
extern ARGB const ARGB_WHITE;

extern ARGB const ARGB_TRANSPARENT;

void dumpColorTable();

}  // namespace gfxDraw

// End.
