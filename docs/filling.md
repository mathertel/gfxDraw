# Filling Paths

Closed paths can be filled with a filling color or a filling pattern.

## Filling a Path

The `fillSegments` function allows filling closed paths.  2 callback functions are required for drawing:

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

The pixel order of the callback functions is y-line oriented and therefore different to the order from the `drawSegments`
function.


## The filling algorithm

The filling algorithm is implemented by the internal **fillSegments** function and is implemented using a pixel
oriented **Scanline Fill Algorithm** and works not only with polygons but with paths of any shape.

* First all border pixels that from the draw function are collected into `Edges`.
* It is important tht the pixels are in order of the
  borderline and sequences of pixels on the same y-line are combined into one Edge with the length covering all pixels.
* All Edges are then sorted so all pixels on the same line (same y-value) are in order with ascending x-value.
* All pixels between the odd and following even Edge on one line are assumed to be "inside" the path.

But there are some known problems that need to be addressed:

This requires some analysis of the given points given from the basic draw functionality before the pixels are sorted.
In this implementation the horizontal pixel sequences that start and end with different slopes are producing 2 Edges (see internal class `_Edge`) entries.

These situations appear on local extremes (minimum or maximum) sequences where the Edge is not a simple start or stop edge but rather both.

The first created Edge starts with the leftmost pixel and all following pixels define the length.  There is another Edge record created in the list with the position of the rightmost pixel but with the length = 0.

This ensures that there are always 2 Edges for the pixel sequences creating local extremes.

This ensures that filling is required between the odd-to-even eEdges and not the even-to-odd Edges.

