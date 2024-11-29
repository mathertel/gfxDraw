# Gauge Widget

gfxDrawGaugeWidget.cpp


  // analog gauge options

  analog: {
    numTicks: 10,
    minAngle: 150,
    maxAngle: 390,

    lineWidth: 10,
    arrowWidth: 10,

    arrowColor:'#486e85',
    inset:false
  },


Doughnut ???


## Element configuration

The size of the gauge is specified by the DisplayOutputElement properties:

    "x": "20",
    "y": "20",
    "h": "160"

The Gauge Element always is using a square shape even if a part of the square is not used while drawing. When width and height
is specified with different values the inner square is used only.

The overall Gauge angle and value range must be given by using the parameters `min`, `max` to specify what range the expected
values and `minAngle`, `maxAngle` to specify the layout of the gauge.

It is important to define these before further definitions of gauge segments as they refer to the min/max values.

    "min": "-20",
    "max": "60",
    "minAngle": 45,
    "maxAngle": 315,

When a value is given outside the min -- max range it will be ensured that the pointer is rotated to the min or the max position.
The textual representation of the value will always show the correct current value.


### Gauge Segments

The gauge segments all use the same inner and outer radius values that can be specified by the parameters gauge-radius and gauge-width.
The values are given in percentage of the available radius defined by the

    "gauge-radius"
.

As default a single segment is defined automatically covering the whole min/max range with a gray color.
In case segments are defined in the configuration this default segment will not be shown.


### Pointer

The pointer will always start in the center if the square and is rotated to point to the current value.

The shape of a pointer can be changed by providing a path pointing from 0/0 (center) to 0/1000 (value). It will be scaled and
rotated by the gauge implementation.

Example configuration:
  "pointer": "M-150,800 L0,950 Z",
  "pointer-stroke": "black",
  "pointer-fill": "yellow"


**pointer**

$('#example').simpleGauge({

  // gauge type
  type: 'analog digital',

  // container styles
  container: {
    style: {},
    size: 90
  },

  // gauge title
  title: {
    text: '',
    style: {
      padding: '5px 7px',
      'font-size': '30px'
    }
  },

  // digital gauge options
  digital: {
    text: '{value.1}',
    style: {
      padding: '5px 7px',
      color: 'auto',
      'font-size': '25px',
      'font-family': '"Digital Dream Skew Narrow","Helvetica Neue",Helvetica,Arial,sans-serif',
      'text-shadow': '#999 2px 2px 4px'
    }
  },

  // analog gauge options
  analog: {
    numTicks: 10,
    minAngle: 150,
    maxAngle: 390,
    lineWidth: 10,
    arrowWidth: 10,
    arrowColor: '#486e85',
    inset: false
  },

  // bar colors
  barColors: [
    [ 0,  '#666' ],
    [ 50, '#ffa500' ],
    [ 90, '#e01010' ]
  ],
  
});
