# gfxDraw Library - AI Coding Agent Instructions

## Project Overview

**gfxDraw** is an Arduino C++ library for drawing vector graphics on pixel-based GFX displays. It targets resource-constrained microprocessors (ESP32) with limited memory and CPU, prioritizing low-level drawing primitives that use callback functions for display independence.

**Key Design Philosophy:**
- Hardware abstraction via callback functions, not direct GFX library dependencies
- Minimized floating-point and arc arithmetic operations
- Primitives calculate points; callbacks handle rendering
- Support for complex SVG path syntax without rasterization/anti-aliasing

## Architecture Layers

### 1. **Primitives** (`gfxDrawLine.h`, `gfxDrawRect.h`, `gfxDrawCircle.h`, `gfxDrawBezier.h`)
Low-level drawing functions that calculate pixel coordinates. Each uses a callback (`fSetPixel`) to receive points:
```cpp
// Example callback pattern used throughout the library
typedef std::function<void(int16_t x, int16_t y)> fSetPixel;
drawLine(x0, y0, x1, y1, cbDraw);  // calculates line points, calls cbDraw for each
```

### 2. **Color & Pixel Processing** (`gfxDrawColors.h`)
- `ARGB` class: 32-bit color with alpha channel (8-bit each: A, R, G, B)
- Conversion methods: `toColor16()` for 5-6-5 RGB display formats
- Callback pattern for pixel filters: `fDrawPixel` passes ARGB color directly

### 3. **Path System** (`gfxDrawPath.h`, `gfxDrawPathWidget.h`)
- **Segments**: Union-based struct defining Move, Line, Arc, Close operations
- **SVG Path Parsing**: `parsePath()` converts text like `"M4 8l12-6l10 10h-8v4h-6z"` to segment vectors
- **Transformations**: Combined into 3×3 matrices (Matrix1000) to avoid rounding errors from sequential transforms
- **Key Files**: `gfxDrawPathWidget` wraps segments with stroke/fill colors and transformation state

### 4. **Filling Algorithm** (embedded in `gfxDrawPath.cpp`)
Uses scan-line approach for closed paths only. Processes filled pixels through callbacks, supporting gradients via `fMapColor` function.

### 5. **Widgets** (`gfxDrawGaugeWidget.h`, `gfxDrawPathWidget.h`)
High-level UI components. `gfxDrawGaugeWidget` manages segments with color ranges mapped to values. Configuration structs (`gfxDrawGaugeConfig`) separate display logic from state.

### 6. **Sprites** (`gfxDrawSprite.h`)
Off-screen ARGB buffer supporting:
- Dynamic allocation (expands as pixels are added)
- Color mapping transformations
- Flicker-free rendering via batch drawing

## Critical Patterns

### Callback-Based Architecture
All drawing is decoupled via callbacks. **Never call `gfx->drawPixel()` directly in library code.** Always receive a callback and use it:
```cpp
// ✓ Correct: library receives callback
void drawRect(int16_t x, ..., fSetPixel cbDraw) {
  // ... calculate points
  cbDraw(px, py);  // invoke callback
}

// ✗ Wrong: library hardcodes GFX dependency
gfx->drawPixel(px, py);
```

### Point Specialization
`Point` struct (in `gfxDrawCommon.h`) has static helpers for trigonometry:
- `circleQuadrant(Point)`: determines quadrant 0-3
- Operator overloads: `Point + Point`, `Point - Point`
- Use `Point::compare()` for sorting (by y then x)

### Path Segment Union Pattern
`Segment` type field determines which union members are valid:
```cpp
enum Type { Move=0x0102, Line=0x0202, Arc=0x0407, Close=0xFF00 };
// Type encodes parameter count in low byte for validation
```

### Color Conversions
Always use `ARGB::toColor16()` before passing to GFX libraries. Byte order depends on endianness flag `__BYTE_ORDER__` in `gfxDrawColors.h`.

### Transformation Matrices
Combine Move, Scale, Rotate into single 3×3 matrix using 1000x fixed-point factor. Applied **just before drawing**, not during segment generation, to minimize rounding:
```cpp
// In gfxDrawPathWidget: transformations recorded, applied during render
void draw(fDrawPixel cbDraw);  // applies matrix to all segments
```

## File Organization

```
src/
├── gfxDraw.h                    # Main include, aggregates all modules
├── gfxDrawCommon.h              # Point class, fSetPixel/fDrawPixel/fTransform typedefs
├── gfxDrawLine.cpp/h            # Bresenham-style line drawing
├── gfxDrawRect.cpp/h            # Rectangle primitives
├── gfxDrawCircle.cpp/h          # Circle/arc drawing with quadrant logic
├── gfxDrawBezier.cpp/h          # Cubic Bézier curve subdivision
├── gfxDrawPath.cpp/h            # Segment parsing, SVG path → vector<Segment>
├── gfxDrawColors.cpp/h          # ARGB class, predefined color constants
├── gfxDrawText.cpp/h            # Glyph rendering via segments
├── gfxDrawPathWidget.cpp/h      # High-level widget combining path + transforms
├── gfxDrawGaugeWidget.cpp/h     # Value-driven gauge display
├── gfxDrawSprite.cpp/h          # Off-screen buffer with dynamic sizing
├── gfxfont.h                    # Font metadata struct
└── fonts/                       # Pre-compiled binary fonts (font8.h, font16.h, etc.)
```

## Development Workflows

### Building for Arduino/ESP32
- Target: Arduino IDE or PlatformIO
- Uses `Arduino.h`, standard C++ `<functional>`, `<vector>`, `<algorithm>`
- Enable tracing with `#define GFX_TRACE GFXDRAWTRACE` in `gfxDraw.h` (line 52)

### Building for Windows Testing
- Examples in `examples/png/` compile with MSVC
- Output to PNG via `lodepng` library (not part of main library)
- Replaces Arduino headers with standard C++

### Key Integration Points
1. **Display Output**: Pass GFX-compatible callback:
   ```cpp
   auto gfxWriteColor = [](int16_t x, int16_t y) { gfx->writePixel(x, y, color); };
   drawLine(0, 0, 10, 10, gfxWriteColor);
   ```

2. **Sprite Rendering**: Use `Sprite::draw(pos, callback)` for flicker-free updates
3. **Widget Configuration**: Modify config structs before `draw()`, not after

## Testing & Debugging

- Use Windows build (`examples/png/main.cpp`) for rapid iteration with PNG output
- Check `_out.txt` in src/ for debug output from test builds
- Segment validation: confirm `Type` field low byte matches actual parameters
- Path parsing: test with SVG visualizer (referenced in comments: https://svg-path-visualizer.netlify.app)

## Common Conventions

- **Namespace**: Always in `gfxDraw {}`
- **Coordinates**: int16_t, supports ±32760 display resolution
- **Color**: ARGB with alpha=0 for transparent; use `ARGB_BLACK`, `ARGB_WHITE` constants
- **Special Y Values**: `POINT_BREAK_Y` (INT16_MAX) marks path end; `POINT_INVALID_Y` (INT16_MAX-1) for skipping
- **SVG Commands**: Relative (lowercase) and absolute (uppercase) both supported in `parsePath()`

## When Adding Features

1. **New Primitive**: Add callback parameter, implement point calculation only
2. **New Widget**: Extend `gfxDrawPathWidget` or create config struct + draw method
3. **Transformations**: Add to 3×3 matrix in `gfxDrawPathWidget::move/scale/rotate`
4. **Font Support**: Extend `gfxDrawText.h` with glyph segment definitions
5. **Fills**: Modify scan-line algorithm in `gfxDrawPath.cpp`, not primitives
