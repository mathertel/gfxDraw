# Bezier Curve Command

Paths may can segments that are defined by cubic bezier curves.  The
[SVG cubic Bézier curve command](https://www.w3.org/TR/SVG11/paths.html#PathDataCubicBezierCommands) as defined by the SVG
specification enables drawing these curves defined by using the following notation:

```txt
    C <x1> <y1> <x2> <y2> <xDest> <yDest>
```

* The curve starts at the end of the previous command.
* `<x1>` / `<y1>` are the coordinates of the first control point.
* `<x2>` / `<y2>` are the coordinates of the second control point.
* `<xDest>` / `<yDest>` defines the end point of the arc.


## Drawing the curve

The implementation of the drawing command uses a Casteljau's algorithm.
The interim and curve points are calculated in a 1024 scale to support smooth and mostly acurate curves.
All calculated points are then proposed to be part of the final curve.

As the number of calculated points is estimated by a heuristic calculation ther may be multiple calculations resulting
in the same point as well as corder-steps and gaps between points.  The `proposePixel` function is smoothing and filling
these pixels to create a gap-less series of pixels that can be used for filling.

# See also

* [SVG cubic Bézier curve command](https://www.w3.org/TR/SVG11/paths.html#PathDataCubicBezierCommands)
* A good article for reading: <https://pomax.github.io/bezierinfo/>

