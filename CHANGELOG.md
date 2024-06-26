# Changelog

All notable changes to this project will be documented in this file.

## todo

* estimate bounding box from segments.
* adjust bounding box on drawing.

## unpublished

* gfxDrawWidget supports undraw by saving background pixels during drawing and restoring background.
* [moongfx example](examples/moongfx/README.md) using [GFX Library for Arduino](https://github.com/moononournation/Arduino_GFX).
* gfxDrawBitmap class to save background while drawing
* ARGB color class using 32-bits per pixel for Alpha + Red, Green, Blue



## 2024-05-30 Version 0.8.0

* arc command parsing without spaces around the flag parameters
* arc command implementation with optimized drawing for circular arcs.
* more docu and references
* [png Example](examples/png/README.md) for building and testing on Windows.
* library.properties file added.

## 2024-04-19

* adding more path commands
* fix path with repeated commands

## 2024-04-17

* using [PNG encoder and decoder in C and C++](https://github.com/lvandeve/lodepng)
  for creating and saving test images.
* moveSegments, rotateSegments implemented.


## 2024-04-15

* Documentation added.
* The naming of functions now more intuitive.
* More drawing examples in main.cpp.
* some unit test in main.cpp.
* RGBA colors are separated into gfxDrawColors.


## 2024-04-10

Initial commit with a `main.cpp` file that compiles the library and produces bmp files
for testing the implementation on windows using the MSVC compiler and VS Code.

First test cases run successfully.
