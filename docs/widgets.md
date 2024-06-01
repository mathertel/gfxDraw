# gfxDraw Widgets

The `gfxDrawObject` class implements the necessary steps to draw a object based on a path with transformation and
coloring options to simplify drawing without using the gfxDraw basic functions.

* All transformations are combined into a transformation matrix to avoid intermediate transformations with rounding
  effects.
* The fill color can be specified using simple linear gradients.
* The Transformations can be cascaded

* Example of a gauge based on gfxDrawObject that manipulates segments based on a given value.
* Several Widget Examples ... ???

In the documentation you can find some internal details explained about transformation procedures to draw the path in different
positions, angles and scales.

