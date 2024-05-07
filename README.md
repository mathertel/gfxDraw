# gfxDraw Arduino Drawing Library for Vector Graphics

There are various GFX libraries available in the Arduino Community that support many differend displays with simple
graphics can be drawn like lines, rectangles, circles and some more.  The features are limited e.g.  rectangles can only
be drawn without rotation and complex figure drawings are not available at all.

The gfxDraw library aims to overcome these limitations by offering more advanced drawing capabilities with vectorized
drawing by using a GFX library under the hood for sending the output to the displays.

The functions are optimized for pixel oriented displays, do not implement antialiasing and have minimized use of float
and arc arithmetics.  This makes this library usable in microprocessor programming like ESP32 based boards with graphic
displays.

The library supports up to 16 bit display resolutions and 32-bit (or less) colors.


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


## Path Syntax

To create a vector (array) of segments the `path` syntax from the SVG was chosen.  There are several helpful web sites
and tools to create such paths definitions like the [yqnn SVG Path Editor](https://yqnn.github.io/svg-path-editor/) ot
the [aydos SVG Path Editor](https://aydos.com/svgedit/).

Examples:

* Simple Line: `"M 0,0 L 30,40"`
* Rectange: `"M 10,10 h40 v30 h-40 z"`
* Bezier Curve: `"M16 12c5 0 5 7 0 7"`
* Smiley:
    `"M12,2h64c4,0 8,4 8,8v48c0,4 -4,8 -8,8h-64c-4,0 -8,-4 -8,-8v-48c0,-4 4,-8 8,-8z"`
    `"M12,10 h60v20h-60z"`
    `"M24,36c6,0 12,6 12,12 0,6 -6,12 -12,12-6,0 -12,-6 -12,-12 0,-6 6,-12 12,-12z"`


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


---

[text](docu/elliptical_arc_curve_command.md)

## gfxDrawObject

This gfxDrawObject class implements the necessary steps to draw a object based on a path with transformation and
coloring options to simplify drawing with gfxDraw native functions.

* All transformations are combined into a transformation matrix to avoid intermediate transformations with rounding
  effects.
* The fill color can be specified using simple linear gradients.
* The Transformations can be
* Example of a gauge based on gfxDrawObject that manipulates segments based on a given value.


## See also

* [Bresenham efficient drawing functions](http://members.chello.at/easyfilter/bresenham.html)
* [Wikipedia Bresenham Algorithm](https://de.wikipedia.org/wiki/Bresenham-Algorithmus)
* [Wikipedia Scanline Rendering](https://en.wikipedia.org/wiki/Scanline_rendering)
* [Math background for using transformation matrixes in 2D drawings](https://www.matheretter.de/wiki/homogene-koordinaten)

* [Constructing Lines and Curves in Pixel Art](https://www.youtube.com/watch?v=ye21r27kN9I)



* <https://svg-path-visualizer.netlify.app/>
* [text](https://css-tricks.com/tools-visualize-edit-svg-paths-kinda/)
* [text](https://github.com/srwiley/rasterx)
* [text](https://wiki.gnome.org/Projects/LibRsvg)
* [text](https://oreillymedia.github.io/Using_SVG/extras/ch04-rasterizers.html)
* [Matrix Transformation Overview](https://web.cse.ohio-state.edu/~shen.94/681/Site/Slides_files/transformation_review.pdf)

* [text](https://www.codeguru.com/multimedia/drawing-rotated-and-skewed-ellipses/)

[^^](https://docs.opencv.org/4.x/d6/d6e/group__imgproc__draw.html#ga28b2267d35786f5f890ca167236cbc69)

[aseprite](https://github.com/aseprite/aseprite/blob/2942abae3ec2cd280c988308963f6c340ced29fe/src/doc/algo.cpp#L221)




