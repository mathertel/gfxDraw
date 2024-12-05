# Creating and Transforming Segments

A vector (array) of Segments is created when parsing a string defining a path.

## Parsing

Converting a path to a vector of Segments is done by using the `gfxDraw::parsePath` function: After parsing the path
syntax a list (vector) of segments can be created as an interim result.  It is a binary representation of the given path
by using absolute coordinates only.  

```cpp
  std::vector<Segment> segs = gfxDraw::parsePath("M1 1 h7 v7 h-7 z M4 4 h1 v1 h-1 z");
```

The created vector of Segments stores all move, lines and arc commands by using absolute coordinates from converting any
relative measures from the `m`, `l`, `v`, `h`, `c` and `a` commands.


## Drawing with Segments

To avoid repeated efforts of parsing the path can be parsed into a vector of segments upfront.  This also allows some
more control over drawing and creates some effectiveness when repeated drawing is required because parsing the path
syntax consumes time, cpu and memory resources.  However storing the Segments also requires memory.

There are 2 drawing functions available:

**`drawSegments`** -- This function takes the vector of segments and calls a drawing callback function for each pixel on the
  border.  Pixels are drawn in the order they appear on the border.

  ```cpp
  gfxDraw::drawSegments(segs,
    [&](int16_t x, int16_t y) { gfx->setPixel(x, y, BLACK); },  // hard-coded stroke color here.
  ```

**`fillSegments`** -- This function internally uses the `drawSegments` function but instead of direct drawing the border pixels are
  analyzed and the contained pixels are calculated by the [filling algorithm](filling.md).  Pixels are drawn in y/x
  sorted order.

  ```cpp
  gfxDraw::fillSegments(segs,
    [&](int16_t x, int16_t y) { gfx->setPixel(x, y, BLACK); },  // hard-coded stroke color here.
    [&](int16_t x, int16_t y) { gfx->setPixel(x, y, WHITE); }); // hard-coded fill color here.
  ```

The `drawSegments` function requires one callback function for the border drawing while the `fillSegments` function uses
2 callback functions for border and filling.


## Transforming

Vectors of Segments can be transformed in place by algorithms provided by the library:

**`gfxDraw::scaleSegments`** -- This function scales all points and radius values by the given factor / base.  The factor and a
base (default = 100) is given as parameters.  So using factor == base will not scale the Segments.

**`gfxDraw::moveSegments`** -- This function adds a vector (dx, dy) to all absolute points in the segments.  The offset
factor is given in pixels.

**`gfxDraw::rotateSegments`** -- This function rotates all points in the segment around the center 0/0.  The angle is
given in degrees.

**`gfxDraw::transformSegments`** -- This function is used internally by the transform functions and uses a callback for
transforming points from the segment.  This function can also be used for combined transformations.

```cpp
  std::vector<Segment> segs = gfxDraw::parsePath(SmilieCurvePath);
  gfxDraw::scaleSegments(segs, 200); // double the size
  gfxDraw::rotateSegments(segs, 25);    // rotate some degrees clockwise
  gfxDraw::moveSegments(segs, 40, 40);  // move right and down

  gfxDraw::fillSegments(segs,
    [&](int16_t x, int16_t y) { gfx->setPixel(x, y, BLACK); },  // hard-coded stroke color here.
    [&](int16_t x, int16_t y) { gfx->setPixel(x, y, WHITE); }); // hard-coded fill color here.
```

