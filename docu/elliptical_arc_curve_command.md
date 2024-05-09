# Elliptical Arc Curve Command

Paths may can segments that are defined by parts of an ellipse or a circle.  The
[SVG elliptical arc curve command](https://www.w3.org/TR/SVG11/paths.html#PathDataEllipticalArcCommands) as defined by
the SVG specification enables drawing these curves defined by using the following notation:

```txt
    A <rx> <ry> <rot> <flag1> <flag2> <xDest> <yDest>
```

* `<rx>` and `<ry>` are the minor and major radius of the ellipse.
* `<rot>` defines the rotation of the ellipse.
* `<flag1>` and `<flag2>` define which of the possible arcs has to be used.
* `<xDest>` and `<yDest>` define which of the possible arcs has to be used.

In addition the last point from the path also is part of the curve.

When `<rx>` is equal to `<ry>` the arc is a segment of a circle and the `<rot>` parameter can be ignored.

There are not many implementation and papers around that implement od describe rotated and shifted ellipses.  The
[arc reference implementation notes](https://www.w3.org/TR/SVG2/implnote.html#ArcImplementationNotes) from w3c are
really helpful.


## Findig the ellipse center

As the drawing needs the center point of the ellipse it is calculated from the given parameters before drawing.

Drawing these usually needs much trigonometric computation and the
[SVG arc reference implementation notes](https://www.w3.org/TR/SVG2/implnote.html#ArcImplementationNotes) from the W3C
standard also includes several rules how to deal with the parameters.  The `arcCenter(...)` function calculates the
center of the ellipsis according these formulas and adjusts the x- and y-radius when required.


## Drawing the arc

The function `drawArc(...)` is using the parameters and calculates all pixels for every degree in the order of the path.

The current implementation still uses double calculations and generates too much pixels on small sized arcs.

TODO: avoid sin, cos, acos functions

However there are implementations that are maybe more efficient - See references below.


Pie:
M275,175 v-150 a150,150 0 0,0 -150,150 z
M300,200 h-150 a150,150 0 1,0 150,-150 z


## See also

* [SVG specification](https://www.w3.org/TR/SVG2/)
* [SVG paths](https://www.w3.org/TR/SVG2/paths.html)
* [SVG elliptical arc curve command](https://www.w3.org/TR/SVG2/paths.html#PathDataEllipticalArcCommands)
* [SVG arc reference implementation notes](https://www.w3.org/TR/SVG2/implnote.html#ArcImplementationNotes)
* [SVG Arc Conversion from Endpoint to Center](https://www.w3.org/TR/SVG2/implnote.html#ArcConversionEndpointToCenter)


* [Ellipse on wikipedia](https://en.wikipedia.org/wiki/Ellipse) including illustrations of drawing methods and many
  references.


### Papers on ellipse rasterization

* [About ellipse drawing](https://dai.fmph.uniba.sk/upload/0/01/Ellipse.pdf)  
  (unrotated drawing)
  
* [Drawing ellipse with bresenhams algorithm](https://stackoverflow.com/questions/49498633/drawing-ellipse-with-bresenhams-algorithm)  
  (unrotated drawing)

* [Simplifying the arcCenter calculation](https://math.stackexchange.com/questions/53093/how-to-find-the-center-of-an-ellipse)

* Fellner, Dieter W., and Christoph Helmberg.
  "[Robust Rendering of General Ellipses and Elliptical Arcs](https://dl.acm.org/doi/10.1145/169711.169704)"
  ACM Transactions on Graphics (TOG) 12, no.  3 (1993): 251-276.

* Chen, X. & Niu, L. & Song, C.. (2015).
  [A fast algorithm for rendering general ellipse controlled by residuals](https://www.researchgate.net/publication/282050308_A_fast_algorithm_for_rendering_general_ellipse_controlled_by_residuals). ICIC Express Letters, Part B: Applications. 6. 2065-2072.  
  (unrotated drawing)

* [Getting Raster Ellipses Right](https://dl.acm.org/doi/pdf/10.1145/130881.130892)
  M. DOUGLAS McILROY, AT& T Bell Laboratories  
  (unrotated drawing)

* [fix point ellipse drawing algorithm](http://wscg.zcu.cz/wscg2001/Papers_2001/R18.pdf)


## future work (maybe)

* Jerry R. Van Aken,
  ["An Efficient Ellipse-Drawing Algorithm"](https://doi.ieeecomputersociety.org/10.1109/MCG.1984.275994) in IEEE
  Computer Graphics and Applications, vol.  4, no.  09, pp.  24-35, 1984.  
  doi: 10.1109/MCG.1984.275994  
  keywords: {Displays;Computer graphics;Error correction;Microcomputers;Instruments;Shape;Workstations;Floating-point
  arithmetic;Computer errors;Application software}  
  abstract: Low-cost PCs and workstations with bit-mapped graphics possess limited processing power.  As a result,
  efficient algorithms are needed to draw curves interactively.  
  url: <https://doi.ieeecomputersociety.org/10.1109/MCG.1984.275994>  
  url: <https://arxiv.org/pdf/2009.03434>


## existing rastering implementations

* [rasterx on GO](https://github.com/srwiley/rasterx/blob/master/shapes.go)

* [canvg - A NodeJS implementation of Paths using Canvas](https://github.com/canvg/canvg/blob/937668eced93e0335c67a255d0d2277ea708b2cb/src/Document/PathElement.ts#L581)

* [About ellipse rendering](https://stackoverflow.com/questions/49619231/calculating-and-storing-pixelated-ellipse)

* [graphics.h ellipse function](https://github.com/genpfault/sdl-bgi/blob/master/src/SDL_bgi.c)

* [SDL2 bresenhams style optimized drawing](https://github.com/giroletm/SDL2_gfx/blob/master/SDL2_gfxPrimitives.c#L1175)
