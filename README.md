# Drawing Vector Graphics on Arduino GFX displays

Arduino library for drawing vector graphics on displays with GFX support.

Drawing and transforming graphics elements specified by a path of lines, arcs and curves using the svg path notation.
The advanced drawing algorithms that can use GFX libraries as the low-level interface to various displays.

There are various GFX libraries available in the Arduino Community that support many differend displays with simple
graphics can be drawn like lines, rectangles, circles and some more.  The features are limited e.g.  rectangles can only
be drawn without rotation and complex figure drawings are not available at all.

The gfxDraw library overcomes these limitations by offering more advanced drawing capabilities with vectorized drawing
input and by using an existing GFX library for sending the output to the displays.

In contrast to the rasterization implementations used in Desktop programs or Browsers this library is dedicated to
microprocessor circumstances with limited memory and cpu power and avoids arithmetic floating point calculations and
doesn't support anti-aliased drawing.


## Software Architecture

![gfxdraw architecture](docs/gfxdraw-architecture.png)

The functions are optimized for pixel oriented displays, do not implement antialiasing and have minimized use of float
and arc arithmetics.  This makes this library usable in microprocessor programming like ESP32 based boards with graphic
displays.

The library supports up to 16 bit display resolutions and 32-bit (or less) colors.

The library offers some entry points for:

* graphic primitives like lines, arcs and cubic bezier curves
* text based path definitions that combine the primitives to build a visual graphic element,
* transformation functions on path based vector graphics
* a drawing object / widget class that combines path, transformations and fill definitions.


## Drawing on a display

The easiest way to draw path based widgets is to use the provided Widget class (gfxDrawObject) and specify all the
transformations and colors by attributes.

```cpp
  using namespace gfxDraw; // use gfxDraw library namespace

  // A SVG path defining the shape of a heard
  const char *heardPath = 
    "M48 20a1 1 0 00-36 36l36 36 36-36a1 1 0 00-36-36z";

  // draw a background rectangle
  drawSolidRect(8, 8, 87, 80, bmpSet(SILVER));

  // draw a heard
  gfxDrawObject widget;
  widget.setStrokeColor(YELLOW);
  widget.setFillColor(RED);
  widget.setPath(heardPath);
  widget.move(8, 8);
  widget.draw(bmpDraw());
```

The gfxDrawObject class offers further functions for scaling and rotating the given path.


## Path Syntax

To create a vector (array) of segments the `path` syntax from the SVG is used.  There are several helpful web sites
and tools to create such paths definitions like the [yqnn SVG Path Editor](https://yqnn.github.io/svg-path-editor/) or
the [aydos SVG Path Editor](https://aydos.com/svgedit/).

Examples for paths are:

* Simple Line: `"M 0,0 L 30,40"`
* Rectange: `"M 10,10 h40 v30 h-40 z"`
* Bezier Curve: `"M16 12c5 0 5 7 0 7"`
* Smiley:  
    `"M12,2h64c4,0 8,4 8,8v48c0,4 -4,8 -8,8h-64c-4,0 -8,-4 -8,-8v-48c0,-4 4,-8 8,-8z"`  
    `"M12,10 h60v20h-60z"`  
    `"M24,36c6,0 12,6 12,12 0,6 -6,12 -12,12-6,0 -12,-6 -12,-12 0,-6 6,-12 12,-12z"`  

More details about the implementation can be found in

[Line Command](docs/line_command.md)
[Bezier Arc Command](docs/bezier_command.md)
[Elliptical Arc Command](docs/elliptical_arc_command.md)
[Filling Paths](docs/filling.md)
[gfxDraw Widgtes](docs/widgets.md)


## The Examples

The examples that come with this library demonstrate on how to use gfxdraw in several situations and project setup.

* using Adafruit GFX
* using the GFX Library for Arduino
* draw primitives
* path drawing and transformation functions


### adafruitgfx Example

This example demonstrates how to use gfxDraw with the
[Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library) that is the most often used GFX library.


### moongfx Example

This example demonstrates how to use gfxDraw with the
[GFX Library for Arduino](https://github.com/moononournation/Arduino_GFX) that has some excellent support for devices
based on the ESP32 chips and graphics displays.


### gfxprimitives Example

To draw the paths the primitive functions for drawing lines, arc and curves are part of the library and also can be used directly.


### gfxsegments Example

After parsing the path syntax a list (vector) of segments is created than can be used for transformations and drawing.

This example shows how to draw using these functions. They are also used by the gfxDrawObject implementation.


### VSCode PNG Example

In the `examples/png` folder you can find a implementation for using the library by a windows executable to produce several png files with test images.

This example is especially helpful while engineering the library with fast turn-around cycles and debugging capabilities.


## See also

* [Bresenham efficient drawing functions](http://members.chello.at/easyfilter/bresenham.html)
* [Wikipedia Bresenham Algorithm](https://de.wikipedia.org/wiki/Bresenham-Algorithmus)
* [Wikipedia Scanline Rendering](https://en.wikipedia.org/wiki/Scanline_rendering)
* [Math background for using transformation matrixes in 2D drawings](https://www.matheretter.de/wiki/homogene-koordinaten)

