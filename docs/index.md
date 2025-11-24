## Optimized for simplicity and small footprints

**gfxDraw** is optimized for situations where multiple graphics are drawn independantly
side by side.

## Support of overlapping graphics

When there are overlapping graphics the re-drawing of the graphics in the background
must be handeled by the application using the gfxDraw library.

As there is support for reading the colors of the pixels on the current display it is
possible to capture background pixels before writing the new graphics and use them later
to "undraw" the graphics.

This enables fast graphics effects but requires more memory.

**One buffer mode** -- **gfxDraw** draws always on an internal buffer that starts with
the current graphics shown on the display.  After drawing the pixels in the bounding
rectangle of the drawing are sent to the display.  The bounding rectangle is calculated
based on the coordinates of the pixels that have been changed.

**No chunked drawing support** -- Chunked drawing refers to rendering UI graphical elements as
multiple smaller chunks.  This technique is sometimes used to avoid flickering when
double-buffering is not available.  when using the **gfxDraw** library at least one
complete buffer containing the current graphics shown on the display must fit into
memory.  This assumption enables simplified sending the changed pixels to the display
and guarantees that the drawing is always done row by row.

**Limited video** -- while it is possible to update the display frequently the **gfxDraw**
library is not optimized for updating a display on a high refresh rate.  By using a fast
hardware solution with DMA or direct access to the video buffer allows drawing without
waiting for any display data transfer and may result in video alike solutions.



## Display Adaption

The **gfxDraw** library needs just some simple driver level functions:

    * init() -- Initializing the display
    * set() -- Set a single pixel to a new color

To get further optimizations and more speed in updating the display the following
functions can be implemented as well:

    * setBuffer() -- Set a (rectangular) buffer of pixels to the colors provided in the (rectangular) buffer.
    * flush() -- This marks the end of a drawing that is worth of beeing synced to the display hardware.

## init()

* Filling a given C-struct that describes the underlying features of the hardware
* Ensuring the display is working and 
* initialize the hole display with black pixels

## set()

 to copy an array of pixels into a given area of the display. If you can do this
with your display then you can use it with LVGL.



## set()

## flush()

## setBuffer()


*  the display