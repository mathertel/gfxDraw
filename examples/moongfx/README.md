# moongfx example

This example demonstrates how to use gfxDraw with the
[GFX Library for Arduino](https://github.com/moononournation/Arduino_GFX) that has some excellent support for devices
based on the ESP32 chips and graphics displays.

The example draws a heard in the center of the screen and rotates is continuously.


Have a look to the PDQgraphicstest example of this library to find out that the device is supported.

The display is started using the sequence and pin assignments:

``` cpp
#define GFX_BL 45
Arduino_DataBus *bus = new Arduino_ESP32LCD8(
    0 /* DC */, GFX_NOT_DEFINED /* CS */, 47 /* WR */, GFX_NOT_DEFINED /* RD */,
    9 /* D0 */, 46 /* D1 */, 3 /* D2 */, 8 /* D3 */, 18 /* D4 */, 17 /* D5 */, 16 /* D6 */, 15 /* D7 */);
Arduino_GFX *gfx = new Arduino_ST7796(bus, 4 /* RST */, 0 /* rotation */, true /* IPS */);

pinMode(GFX_BL, OUTPUT);
digitalWrite(GFX_BL, HIGH);

gfx->begin();
gfx->fillScreen(BLACK);
```

The example draws a heard in the center of the screen and rotates is continuously.

  w = gfx->width();
  h = gfx->height();

testLines

    gfx->drawLine(x1, y1, x2, y2, BLUE);



PDQgraphicstest

# elif defined(ESP32_2432S028)
# define GFX_DEV_DEVICE ESP32_2432S028
# define GFX_BL 21
Arduino_DataBus *bus = new Arduino_ESP32SPI(2 /* DC */, 15 /* CS */, 14 /* SCK */, 13 /* MOSI */, 12 /* MISO */);
Arduino_GFX *gfx = new Arduino_ILI9341(bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation*/);


# elif defined(ZX3D50CE02S)
# define GFX_DEV_DEVICE ZX3D50CE02S
# define GFX_BL 45
Arduino_DataBus *bus = new Arduino_ESP32LCD8(
    0 /* DC */, GFX_NOT_DEFINED /* CS */, 47 /* WR */, GFX_NOT_DEFINED /* RD */,
    9 /* D0 */, 46 /* D1 */, 3 /* D2 */, 8 /* D3 */, 18 /* D4 */, 17 /* D5 */, 16 /* D6 */, 15 /* D7 */);
Arduino_GFX *gfx = new Arduino_ST7796(bus, 4 /* RST */, 0 /* rotation */, true /* IPS*/);

