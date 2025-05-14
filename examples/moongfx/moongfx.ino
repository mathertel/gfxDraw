// gfxDraw
// gfxDraw example using the "GFX Library for Arduino"


#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#include <gfxDraw.h>
#include <gfxDrawText.h>

#include <fonts/font10.h>
#include <fonts/font16.h>
#include <fonts/font24.h>

#include <gfxDrawPathWidget.h>
#include <gfxDrawSprite.h>

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "secrets.h"

Arduino_DataBus *bus;
Arduino_GFX *gfx;

uint16_t width;   // width of display
uint16_t height;  // height of display
uint16_t cx;      // Center-x of display
uint16_t cy;      // Center-y of display

uint16_t mode = 0;
uint16_t step = 0;
unsigned long nextDraw = 0;

// A SVG path defining the shape of a heard
// Size = (87/80), center = (44/40)
const char *heardPath = "M43 7 a1 1 0 00-36 36l36 36 36-36a1 1 0 00-36-36z";

// A SVG path defining the shape of an arrow
// Size = (20/24), center / points to = (0/0)
const char *arrowPath = "M0 0l12-12 0 8 22 0 0 8-22 0 0 8Z";


// helpful "inline" function to
// #define gfxSetPixel(col) [](int16_t x, int16_t y) { gfx->writePixel(x, y, (col)); }

std::function<void(int16_t x, int16_t y)> gfxWriteColor(gfxDraw::ARGB color) {
  uint16_t col16 = color.toColor16();
  return [col16](int16_t x, int16_t y) {
    gfx->writePixel(x, y, col16);
  };
}

/// draw callback for writing picels using the gfx library
void gfxWritePixelColor(int16_t x, int16_t y, gfxDraw::ARGB col) {
  gfx->writePixel(x, y, col.toColor16());
}

void setup() {
  Serial.begin(115200);
  Serial.println("gfxDraw example using the GFX Library for Arduino.");

  // Serial.setDebugOutput(true);
  // while(!Serial);

  // add your WIFI and WIFI access code in secrets.h
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else  // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR)
        Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR)
        Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR)
        Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR)
        Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // The following setup is working with the board ESP32_2432S028R also known as
  // Cheap Yellow Display Board <https://randomnerdtutorials.com/cheap-yellow-display-esp32-2432s028r/>
  // https://cool-web.de/esp8266-esp32/esp32-2432s028-cheap-yellow-display-touchscreen-vorstellung-hardware-pinout.htm
  // https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/

  // #define GFX_BL 21
  //   bus = new Arduino_ESP32SPI(2 /* DC */, 15 /* CS */, 14 /* SCK */, 13 /* MOSI */, 12 /* MISO */);
  //   gfx = new Arduino_ILI9341(bus, GFX_NOT_DEFINED /* RST */, 3 /* rotation */);

  // The following setup is working with the board ZX3D50CE02S or called "WT32-SC01 PLUS".
  // See <https://homeding.github.io/boards/esp32s3/sc01-plus.htm.

#define GFX_BL 45
  bus = new Arduino_ESP32LCD8(
    0 /* DC */, GFX_NOT_DEFINED /* CS */, 47 /* WR */, GFX_NOT_DEFINED /* RD */,
    9 /* D0 */, 46 /* D1 */, 3 /* D2 */, 8 /* D3 */, 18 /* D4 */, 17 /* D5 */, 16 /* D6 */, 15 /* D7 */);
  gfx = new Arduino_ST7796(bus, 4 /* RST */, 0 /* rotation */, true /* IPS */);

  // To adapt other devices or screens please check Arduino_GFX_dev_device.h in the PDQgraphicstest example
  // from the GFX Library for Arduino.


  // Init Display
  if (!gfx->begin()) {
    Serial.printf("gfx->begin() failed!\n");
  }

  width = gfx->width();
  height = gfx->height();
  Serial.printf("gfx width=%d height=%d\n", width, height);

  cx = width / 2;
  cy = height / 2;

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  Serial.println("starting...");

  // add some fonts
  gfxDraw::addFont(&font10);
  gfxDraw::addFont(&font16);
  gfxDraw::addFont(&font24);

  gfx->fillScreen(RGB565_GREEN);
  nextDraw = millis() + 2000;
}

/// @brief initialize the screen for the next demo-page.
/// @param nr -- Number of the screen
/// @param color -- color of the background (default = white).
void newScreen(uint16_t nr, uint16_t color = RGB565_WHITE) {
  Serial.printf("screen %d\n", nr);

  gfx->fillScreen(color);
  String sNr(nr);
  gfxDraw::Point dim = gfxDraw::textBox(10, sNr.c_str());
  gfxDraw::Point pos(width - dim.x, 0);

  gfx->startWrite();
  gfxDraw::drawText(pos, 10, sNr.c_str(), gfxWriteColor(gfxDraw::ARGB_BLACK));
  gfx->endWrite();
}  // newScreen()


void loop(void) {
  unsigned long now = millis();

  ArduinoOTA.handle();

  if (now > nextDraw) {

    if (mode == 0) {
      // demonstrate text drawing with different fot sizes.
      newScreen(mode, RGB565_GREEN);

      gfxDraw::Point p(10, 10);
      gfx->startWrite();

      gfxDraw::drawText(p, 8, "Hello gfxDraw 8!", gfxWriteColor(gfxDraw::ARGB_BLACK));
      p.y += gfxDraw::lineHeight();

      gfxDraw::drawText(p, 10, "Hello gfxDraw 10!", gfxWriteColor(gfxDraw::ARGB_BLACK));
      p.y += gfxDraw::lineHeight();

      gfxDraw::drawText(p, 16, "Hello gfxDraw 16!", gfxWriteColor(gfxDraw::ARGB_BLACK));
      p.y += gfxDraw::lineHeight();

      gfxDraw::drawText(p, 20, "Hello gfxDraw 20!", gfxWriteColor(gfxDraw::ARGB_BLACK));
      p.y += gfxDraw::lineHeight();

      gfxDraw::drawText(p, 24, "Hello gfxDraw 24!", gfxWriteColor(gfxDraw::ARGB_BLACK));
      p.y += gfxDraw::lineHeight();

      gfxDraw::drawText(p, 32, "Hello gfxDraw 32!", gfxWriteColor(gfxDraw::ARGB_BLACK));

      gfx->endWrite();
      gfx->flush();


    } else if (mode == 1) {
      // demonstrate line drawing
      newScreen(mode);

      gfx->startWrite();
      for (int16_t x = 10; x <= width - 10; x += 4) {
        gfxDraw::drawLine(10, 10, x, height - 10, gfxWriteColor(gfxDraw::ARGB_BLACK));
      }

      for (int16_t y = height - 10; y >= 10; y -= 4) {
        gfxDraw::drawLine(10, 10, width - 10, y, gfxWriteColor(gfxDraw::ARGB_BLACK));
      }
      gfx->endWrite();
      gfx->flush();


    } else if (mode == 2) {
      // demonstrate line drawing
      newScreen(mode);

      gfx->startWrite();
      for (int16_t x = 10; x <= width - 10; x += 4) {
        gfxDraw::drawLine(x, 10, x, height - 10, gfxWriteColor(gfxDraw::ARGB_BLACK));
      }

      for (int16_t y = 10; y <= height - 10; y += 4) {
        gfxDraw::drawLine(10, y, width - 10, y, gfxWriteColor(gfxDraw::ARGB_BLACK));
      }
      gfx->endWrite();
      gfx->flush();


    } else if (mode == 3) {
      newScreen(mode);

      gfx->startWrite();
      for (int16_t n = 0; n < 10; n++) {
        gfxDraw::pathByText(heardPath, 10 + 10 * n, 10 + 8 * n, 30 + (n * 8), gfxWriteColor(gfxDraw::ARGB_BLUE), gfxWriteColor(gfxDraw::ARGB_YELLOW));
      }
      gfx->endWrite();

    } else if (mode == 4) {
      // demonstrate drawing rectangles
      newScreen(mode);

      gfx->startWrite();
      gfxDraw::drawRect(8, 8, 88, 80, nullptr, gfxWriteColor(gfxDraw::ARGB_BLUE));
      gfxDraw::drawRect(48, 48, 88, 80, nullptr, gfxWriteColor(gfxDraw::ARGB_RED));
      gfx->endWrite();
      gfx->flush();

    } else if (mode == 5) {
      // demonstrate using the Path widget class
      newScreen(mode, RGB565_LIGHTGREY);

      gfxDraw::gfxDrawPathWidget widget;
      widget.setPath(heardPath);
      widget.setFillColor(gfxDraw::ARGB_RED);
      widget.setStrokeColor(gfxDraw::ARGB_ORANGE);

      for (int16_t n = 0; n <= 360; n += 9) {
        widget.resetTransformation();
        widget.rotate(n, 44, 40);
        widget.move(100, 60);

        gfx->startWrite();
        widget.draw(gfxWritePixelColor);
        gfx->endWrite();
        delay(10);
      }
      delay(200);

      gfxDraw::gfxDrawPathWidget arrow;
      arrow.setFillColor(gfxDraw::ARGB_YELLOW);
      arrow.setPath(arrowPath);
      arrow.scale(200);
      arrow.rotate(-45);
      arrow.move(100 + 44, 60 + 40);
      gfx->startWrite();
      arrow.draw(gfxWritePixelColor);
      gfx->endWrite();
      gfx->flush();

    } else if (mode == 6) {
      // demonstrate using the Path widget class with sprite
      gfxDraw::ARGB bg = gfxDraw::ARGB_SILVER;
      newScreen(mode, bg.toColor16());

      gfxDraw::gfxDrawPathWidget widget;
      gfxDraw::Sprite sprite;

      widget.setPath(heardPath);
      widget.setFillColor(gfxDraw::ARGB_RED);
      // widget.setStrokeColor(gfxDraw::ARGB_ORANGE);

      for (int16_t n = 0; n <= 360; n += 3) {
        widget.resetTransformation();
        widget.rotate(n, 44, 40);

        // draw to sprite
        widget.draw([&](int16_t x, int16_t y, gfxDraw::ARGB color) {
          sprite.drawPixel(x, y, color);
        });

        gfx->startWrite();
        sprite.draw(gfxDraw::Point(100, 60), gfxWritePixelColor);
        gfx->endWrite();

        sprite.mapColor(gfxDraw::ARGB_SILVER, gfxDraw::ARGB_TRANSPARENT);
        sprite.mapColor(gfxDraw::ARGB_RED, gfxDraw::ARGB_SILVER);

        delay(10);
      }
      delay(200);

      gfxDraw::gfxDrawPathWidget arrow;
      arrow.setFillColor(gfxDraw::ARGB_YELLOW);
      arrow.setPath(arrowPath);
      arrow.scale(200);
      arrow.rotate(-45);
      arrow.move(100 + 44, 60 + 40);
      gfx->startWrite();
      arrow.draw(gfxWritePixelColor);
      gfx->endWrite();
      gfx->flush();


    } else {
      newScreen(mode, RGB565_LIGHTGREY);
    }

    nextDraw = millis() + 4000;
    mode = (mode + 1) % 7;
  }


  //     delay(100);
  //     gfxDraw::gfxDrawPathWidget arrow;
  //     arrow.setFillColor(gfxDraw::ARGB_YELLOW);
  //     arrow.setPath(arrowPath);
  //     arrow.scale(200);
  //     arrow.rotate(-45);
  //     arrow.move(posX + 44, posY + 40);
  //     arrow.draw(gfxWritePixelColor);
  //   }

  //   nextDraw = now + 2000;
  // }
}  // loop
