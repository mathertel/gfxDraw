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


## See also

[Line commands](../../docs/line_command.md)
[Bezier Curve Commands](../../docs/bezier_command.md)
[Arc Curve Commands](../../docs/elliptical_arc_command.md)