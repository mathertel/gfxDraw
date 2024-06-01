# Line Command

Paths may have segments that are defined by straight lines.  The various
[SVG "lineto" commands](https://www.w3.org/TR/SVG11/paths.html#PathDataLinetoCommands) draw straight lines from the
current point to a new point as defined by the SVG specification:

```txt
  L <xDest> <yDest>
  l <xDest-delta> <yDest-delta>
  H <xDest>
  h <xDest-delta>
  V <yDest>
  v <yDest-delta>
```

## Implementation

The implementation for lines can be found in the function `gfxDraw::drawLine()` that has optimized version for pure
horizontal and vertical lines and is using a Bresenham algorithm for drawing other lines.

The function calculates all pixels of the line in the order from start to end and calls the `setPixel` callback function
for each pixel.

The drawLine() function is also used by the [textarc drawing function](elliptical_arc_command.md) and
[bezier drawing function](???) when they calculate pixel sequences that are not continuous.


## See also

* [SVG specification](https://www.w3.org/TR/SVG2/)
* [SVG paths](https://www.w3.org/TR/SVG2/paths.html)
* [Bresenham efficient drawing functions](http://members.chello.at/easyfilter/bresenham.html)
