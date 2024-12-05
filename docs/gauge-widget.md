# Gauge Widget

We are used to see Gauge Widgets to visualize a meassued values like temperature or speed.

The Gauge Widgets support variants of gauges for single values by the following features:

* The total Gauge angle to be used.
* Multiple segments of the gauge that can be drawn with different colors
* Scale marks
* The Pointer shape can be customized by providing a path text.

## Configuraton

When creating a Guage widget a configuration struct can be given using the `setConfig()` function
to specify some the gneral settings:

The position and dimension of the widget on the screen is given by the `x`, `y`, `w` and `h` parameters.

The Gauge Element always is using a square shape even if a part of the square is not used while drawing.  When width and
height is specified with different values the inner square is used only and the available radius is calculated by half
of the square width.

The overall Gauge angle is specified by the parameters `minAngle` and `maxAngle`.  All segments specified later by the
`addSegment()` function will be drawn inside this range of angles.  The angles are given in 360° and a angle = 0 is
exactly down.  A good start and end pair of angles is 45 to 360-45 or 90 to 270

The overall value range must be given by using the parameters `minValue` and `maxValue`. This specifies
what values can be shown using the gauge. When a given value is outside that range the displayed value by the pointer will be `minValue` or `maxValue`.
The textual representation of the value will always show the correct current value.

The outer radius of the segments are defined by the `segmentRadius` parameter and the width od the segments are defined
by the `segmentWidth` parameter. Both are given in percentage of the available radius.

Simple scale markers can be drawn by specifying the parameters `scaleRadius`, `scaleWidth` and `scaleSteps`.

outer radius of the segments are defined by the `segmentRadius` parameter and the width od the segments are defined
by the `segmentWidth` parameter. Both are given in percentage of the available radius.

The color of the pointer and the default segment are given by the `pointerColor` and `segmentColor`.


## Configuration Example

``` cpp
GFXDrawGaugeConfig gaugeConfig = {
  .x = 10,
  .y = 10,
  .w = 200,
  .segmentRadius = 100,
  .segmentWidth = 35,
  .pointerColor = ARGB_GREEN,
  .segmentColor = ARGB_GRAY,
  .minValue = 0,
  .maxValue = 100,
  .minAngle = 30,
  .maxAngle = 360 - 30,
  .scaleRadius = 95,
  .scaleSteps = 10
};
```


## Colored Segments

Advanced coloring and multiple segments is available by using the `addSegment()` function.  To add segments with a
specific color the `addSegment()` function can be called multiple times.

Each `addSegment()` call will specify the part of the overall segment by using the `minValue` and `maxValue` parameters
to specify the range and the color to be used.

```cpp
  // The range 16...22 is optimal, colored in light green
  gauge->addSegment(16, 22, 0xff60ff60);
```


<!-- ## Scale markers -->

### Pointer

The pointer will always start in the center if the square and is rotated to point to the current value.

The color of the pointer can be set by providing the `pointerColor` in the configuration.

As of now the pointer has a fixed shape.

<!-- 
By providing a path pointing from 0/0 (center) to 0/1000 (value) the shape of a pointer can be customized.  It will be
scaled and rotated by the gauge implementation.

by providing a path pointing from 0/0 (center) to 0/1000 (value).  It will be scaled and rotated by the gauge
implementation.

```cpp
GFXDrawGaugeConfig gaugeConfig = {
  ... 
  .pointerpath = "M-150,800 L0,950 Z",
};
```
 -->


## Values and drawing

To set the current to be displayed the `setValue()` can be used.  Finally calling the `draw()` function has to called
and a fDrawPixel callback must be provided.


