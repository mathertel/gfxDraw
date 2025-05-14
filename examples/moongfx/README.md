# moongfx example

The moongfx example demonstrates how to use gfxDraw with the [GFX Library for Arduino].

The [GFX Library for Arduino] has some excellent support for devices based on the ESP32 chips and graphics displays.

The moongfx example has been tested with the the ESP32_2432S028R also known as Cheap Yellow Display Board
<https://randomnerdtutorials.com/cheap-yellow-display-esp32-2432s028r/> and the
[SC01 Plus](https://homeding.github.io/boards/esp32s3/sc01-plus.htm).

To use other devices you can use one of the working boards with the device setup you can find in the provided
PDQgraphicstest example in the [GFX Library for Arduino].

You need to define the pin enabling the LCD backlight using a definition for GFX_BL and find the right bus and gfx classes with parameters:

For ESP32_2432S028R / Cheap Yellow Display Board:

```cpp
#define GFX_BL 21
  bus = new Arduino_ESP32SPI(2 /* DC */, 15 /* CS */, 14 /* SCK */, 13 /* MOSI */, 12 /* MISO */);
  gfx = new Arduino_ILI9341(bus, GFX_NOT_DEFINED /* RST */, 3 /* rotation */);
```

For SC01 Plus Display Board:

```cpp
#define GFX_BL 23
  bus = new Arduino_ESP32SPI(21 /* DC */, 15 /* CS */, 14 /* SCK */, 13 /* MOSI */, GFX_NOT_DEFINED /* MISO */);
  gfx = new Arduino_ST7796(bus, 22 /* RST */, 3 /* rotation */);
```

## The Example Code

The example draws different static and dynamic screens on the display in a loop.

[GFX Library for Arduino]: https://github.com/moononournation/Arduino_GFX
