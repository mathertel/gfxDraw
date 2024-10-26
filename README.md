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

## General Library Implementation Rules

This library aims to support graphics implementations that are used in microprocessors like ESP32 based boards with
pixel oriented graphic displays.

The functions are optimized for low resolution pixel displays. They do not implement antialiasing and have minimized use of float
and arc arithmetics.  

The library supports up to 16 bit (-32760 ... +32760) display resolutions.

By design the drawing functionionality is independent of the color depth.

The Widget Class and Bitmap Class supports drawing using a 32-bit (or less) color setup using RGB+Alpha for a specific pixel.


## Software Architecture

The library offers the following levels of functionality:

* graphic primitives like lines, arcs and cubic bezier curves,
* graphical primitive objects like rectangles, rounded rectangles and full circles,
* text based path definitions that combine the primitives to build a visual graphic element,
* transformation functions on path based vector graphics
* a drawing widget class that combines path, transformations and fill definitions.


<!-- ![gfxdraw architecture](docs/gfxdraw-architecture.png) -->

The library uses the Namespace `gfxDraw` to implement the types, functions and the classes to avoid conflicts with other libraries.

<!-- [text](https://mermaid.js.org/syntax/flowchart.html) -->

```mermaid
flowchart

subgraph gfxLibrary
  direction TB
  classDef Impl stroke:blue,color:blue,fill:white,stroke-width:4px;

  subgraph widgets
    Object:::Impl
  end

  subgraph path
    direction LR
    Object --> Segments;
    Object --> Path;
  end

    Segments-->Fill;


  subgraph calculate points
    Segments-->gfxDrawLine:::Impl;
    Segments-->gfxDrawCircle:::Impl;
    Segments-->gfxDrawBezier:::Impl;
  end

  gfxDrawLine -->    gfxdrawCommon:::Impl;
  gfxDrawCircle-->    gfxdrawCommon:::Impl;
  gfxDrawBezier-->    gfxdrawCommon:::Impl;
end

subgraph Graphics-Driver
  Fill -->setPixel
  Object-->getPixel
end 

```

**gfxdrawCommon** -- This file defines the Point data type and implements basic functions like table driven sin/cos arithmetic for low resolutions.

**gfxdrawCircle** -- This file implements the functions to calculate all points of a circle, circle quadrant and circle segment.

**gfxDrawBezier** -- This file implements the function to calculate all points of a bezier segment.


## Drawing on a display

The easiest way to draw path based widgets is to use the provided Widget class (gfxDrawWidget) and specify all the
transformations and colors by attributes.

```cpp
  using namespace gfxDraw; // use gfxDraw library namespace

  // A SVG path defining the shape of a heard
  const char *heardPath = 
    "M48 20a1 1 0 00-36 36l36 36 36-36a1 1 0 00-36-36z";

  // draw a background rectangle
  drawRect(8, 8, 87, 80, nullptr, bmpSet(SILVER));

  // draw a heard
  gfxDrawWidget widget;
  widget.setStrokeColor(YELLOW);
  widget.setFillColor(RED);
  widget.setPath(heardPath);
  widget.move(8, 8);
  widget.draw(bmpDraw());
```

The gfxDrawWidget class offers further functions for moving, scaling and rotating the given path.
See [Widgets Class](docs/widgets.md).


## SVG Path Syntax

To create a vector (array) of segments that build the borders of the vector graphics object the `path` syntax from the SVG standard is used.  

There are helpful web applications to create or edit such paths definitions:

* The [SVG Path Editor](https://yqnn.github.io/svg-path-editor/) with source available in
  [Github/Yqnn](https://github.com/Yqnn/svg-path-editor) from Yann Armelin

* The [SVG Path Editor](https://aydos.com/svgedit/) with source available in
  [Github/aydos](https://github.com/aydos/svgpath) from Fahri Aydos.

Both tools let you directly change the individual segments of paths and also offer some graphical view or even edit capabilities. You can also use full SVG editors and extract the path from there.

For using paths with pixel oriented displays is is important to use integer based coordinates and scalar values only. Better to use larger numbers as scaling the result down to a smaller size is possible.


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


## Contributions

Contributions are welcome. Please use GitHub Issues for discussing and Pull Request. Need more -- just ask.


## The Examples

The examples that come with this library demonstrate on how to use gfxdraw in several situations and project setup.

* using Adafruit GFX
* using the GFX Library for Arduino
* draw primitives
* path drawing and transformation functions

<!-- ### Adafruit GFX Example

This example demonstrates how to use gfxDraw with the
[Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library) that is on of the most often used GFX libraries
for Arduino. -->


### Moon GFX Example

This example demonstrates how to use gfxDraw with the
[GFX Library for Arduino](https://github.com/moononournation/Arduino_GFX) that has some excellent support for devices
based on the ESP32 chips and graphics displays.


### VSCode PNG Example

In the `examples/png` folder you can find a implementation for using the library by a windows executable to produce
several png files with test images.

This example is especially helpful while engineering the library with fast turn-around cycles and debugging capabilities.


<!-- ### gfxprimitives Example

To draw the paths the primitive functions for drawing lines, arc and curves are part of the library and also can be used directly. -->


<!-- ### gfxsegments Example

After parsing the path syntax a list (vector) of segments is created than can be used for transformations and drawing.

This example shows how to draw using these functions. They are also used by the gfxDrawWidget implementation. -->


## See also

* [Bresenham efficient drawing functions](http://members.chello.at/easyfilter/bresenham.html)
* [Wikipedia Bresenham Algorithm](https://de.wikipedia.org/wiki/Bresenham-Algorithmus)
* [Wikipedia Scanline Rendering](https://en.wikipedia.org/wiki/Scanline_rendering)
* [Math background for using transformation matrixes in 2D drawings](https://www.matheretter.de/wiki/homogene-koordinaten)
* [SVG Game Icons](https://game-icons.net/)
* [SVG IoT Icons](https://github.com/HomeDing/WebFiles/tree/master/i)
