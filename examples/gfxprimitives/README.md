# gfx Primitives example

## Drawing Primitives

The algorithms in this library include a collection of efficient basic drawing algorithms like the drawing approach of
bresenham for lines, bezier curves and arcs. These are availabe for drawing directly as they call a callBack function for every pixel that is part of the primitives:

```cpp
  gfxDraw::drawLine(5, 5, 10, 5, [&](int16_t x, int16_t y) {
    gfx->setPixel(x, y, WHITE); // hard-coded stroke color here.
  });

  gfxDraw::drawCubicBezier(10, 10, 11, 2, 25, 18, 26, 10, [&](int16_t x, int16_t y) {
    gfx->setPixel(x, y, YELLOW); // hard-coded stroke color here.
  });
```

These primitive can be combined as segments to create polygons and paths of complex drawings that can be open with 2
ends or closed.  Closed paths are given when the last element is a `Close` command `Z` that ensures the last point to be
equal to the first point of the path.

Path lines can also be drawn by using a primitive function:

```cpp
  gfxDraw::drawPath("M1 1 h7 v7 h-7 z M4 4 h1 v1 h-1 z", [&](int16_t x, int16_t y) {
    gfx->setPixel(x, y + 40, GREEN); // hard-coded stroke color here.
  });
```

This function will parse the given path definition and draws the border in GREEN color. This consumes a lot of cpu and all
intermediate computing and memory requirements will be allocated and freed during the call.


In the calback function the figure may be shifted by adding the offset to `x` and `y` and by providing a color to the gfy setPixel function.


## Filling a Path

There is also a function available that allows filling closed paths.
2 callback functions are required for drawing:

* The Border draw callback is used for drawing all border pixels.
* The Fill draw callback is used for drawing all inner pixels.

```cpp
  std::vector<Segment> segs = gfxDraw::parsePath("M1 1 h7 v7 h-7 z M4 4 h1 v1 h-1 z");

  gfxDraw::fillSegments(segs,
    [&](int16_t x, int16_t y) { gfx->setPixel(x, y, BLACK); }, // hard-coded stroke color here.
    [&](int16_t x, int16_t y) { gfx->setPixel(x, y, WHITE); }); // hard-coded fill color here.
```

When no Border draw callback is provided the Fill draw callback will also be used for the border pixels.

The implementation of fillSegments will use a filling algorithm described in detail below.

The pixel order of the callback functions is y-line oriented and therefore different to the order from the drawSegments
function.


