# gfx Segments example

This example demonstrates how to use the Segments vector drawing functions.

## Path as Segments

After parsing the path syntax a list (vector) of segments can be created as an interim result.  It is a binary
representation of the given path by using absolute coordinates only.  
To avoid repeated efforts of parsing the path can be parsed into a vector of segments upfront.  This also allows some
more control over drawing and creates some effectiveness when repeated drawing is required because parsing the path
syntax consumes time, cpu and memory resources.

These Segments can be modified to draw the path figure in different scales and rotations.

```cpp
  std::vector<Segment> segs = gfxDraw::parsePath("M1 1 h7 v7 h-7 z M4 4 h1 v1 h-1 z");

  gfxDraw::scaleSegments(segs, 300); // scale by 300%
  gfxDraw::rotateSegments(segs, 45); // rotate 45° clockwise

  gfxDraw::drawSegments(segs, [&](int16_t x, int16_t y) {
    gfx->setPixel(x, y, RED); // hard-coded stroke color here.
  });
```


## Filling Algorithm Insights

Closed paths can be filled with a filling color or pattern that is implemented by the internal **fillSegments** function.
This is implemented using a pixel oriented **Scanline Fill Algorithm** and works not only with polygons but with paths of any shape.

In short, this is done by sorting the border pixels from the draw function so all pixels on the same line (same y-value) are in order with ascending x-value.
When a subsequent pixel is on the same line this is recorded in a _Edge with the length covering all pixels.

But there are some known problems that need to be addressed:

This requires some analysis of the given points given from the basic draw functionality before the pixels are sorted.
In this implementation the horizontal pixel sequences that start and end with different slopes are producing 2 Edges (see internal class `_Edge`) entries.  The first starts with the leftmost pixel and all following pixels define the length.  There is another Edge record in the list with the position of the rightmost pixel but with the length = 0.

This ensures that there are always 2 Edges for the pixel sequences creating local extremes.

This ensures that filling is required between the odd-to-even eEdges and not the even-to-odd Edges.


## Transforming

Segment vectors can be transformed in place by algorithms provided by the library:

gfxDraw::scaleSegments -- This function scales all points and radius values by the given factor.  The factor is given in
percentages so `100` will not scale when given as input.

gfxDraw::moveSegments -- This function adds a vector (dx, dy) to all points in the segments.  The offset factor is given
in pixels.

gfxDraw::rotateSegments -- This function rotates all points in the segment around the center 0/0. The angle is given in degrees.

gfxDraw::transformSegments -- This function is used internally by the transform functions and uses a callback for transforming points from the segment. This function can also be used for combined transformations.


```cpp
  std::vector<Segment> segs = gfxDraw::parsePath("M1 1 h7 v7 h-7 z M4 4 h1 v1 h-1 z");

  segs = gfxDraw::parsePath(SmilieCurvePath);
  gfxDraw::scaleSegments(segs, 200);
  gfxDraw::rotateSegments(segs, 25);    // rotate some degrees
  gfxDraw::moveSegments(segs, 40, 40);  // move right down

  gfxDraw::fillSegments(segs,
    [&](int16_t x, int16_t y) { gfx->setPixel(x, y, BLACK); },  // hard-coded stroke color here.
    [&](int16_t x, int16_t y) { gfx->setPixel(x, y, WHITE); }); // hard-coded fill color here.
```

