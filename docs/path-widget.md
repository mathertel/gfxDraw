# gfxDraw Path Widget

The `gfxDrawPathWidget` class implements the necessary steps to draw an object based on a path with transformation and
coloring options.  This class simplifies drawing by avoiding using the gfxDraw basic functions.

The paths used for drawing symbols or diagrams typically use the x/y range from 0/0 to X-size/Y-size and a fixed
rotation center and can be drawn at any position on the screen by finally moving.

* The rotate, scale and move transformations can be cascaded in any order.
* All transformations are combined into a transformation matrix to avoid intermediate transformations with rounding
  effects.
* The stroke and fill color can be specified using the configuration structure.

In the inline documentation you can find some internal details explained about transformation procedures to draw the
path in different positions, angles and scales.

The typical order of transformations is:

* rotate the widget around the center of the widget.
* scale the widget  by the percent factor
* move the widget to the final position

As the rotation requires the center of rotation it is a good proctice to rotate before moving or scaling.
Otherwise yiu have to calculate the center of rotation by using the scale factor and move vector.


## Rotating the Widget

The `widget.rotate(r, x=0, y=0)` function rotates the path with a angle of r degrees (0...360).

The center of rotation is 0/0 by default but can be given as 2.  and 3.  parameter.  This is internally used to apply a
reverse move before and a move after rotation.

## Scaling the Widget

The `widget.scale(f)` function scales all points and radius values by the given factor.  The factor is
given in percentages.  So using `100` will not scale the Segments.

The center of scaling is 0/0.

## Moving the Widget

The `widget.move(x, y)` function adds a linear transformation vector to the existing transformaton matrix to move the resulting
coordinates in this direction.


## Drawing the Widget

The `widget.draw(drawCallback)` function draws the path with the defined transformations.

The drawCallback function has the patameters defined by the type `gfxDraw::fDrawPixel` and get passed the position of the pixel and the color to be used.

```cpp
void drawCallback(int16_t x, int16_t y, gfxDraw::ARGB color) { 
  ...
};
```
