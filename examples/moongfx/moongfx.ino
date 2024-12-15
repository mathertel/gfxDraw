// gfxDraw
// gfxDraw example using the "GFX Library for Arduino"


#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#include <gfxDraw.h>
#include <gfxDrawColors.h>
#include <gfxDrawPathWidget.h>

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "secrets.h"

Arduino_DataBus *bus;
Arduino_GFX *gfx;

uint16_t w;   // width of display
uint16_t h;   // height of display
uint16_t cx;  // Center-x of display
uint16_t cy;  // Center-y of display

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
#define gfxSetPixel(col) [](int16_t x, int16_t y) { \
  gfx->writePixel(x, y, (col)); \
}

// draw on gfx
void gfxDrawColor(int16_t x, int16_t y, gfxDraw::ARGB col) {
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

  // This setup is working with the board ZX3D50CE02S or called "WT32-SC01 PLUS".
  // See
  //  * https://homeding.github.io/boards/esp32s3/sc01-plus.htm
  //
  // To adapt other devices or screens please check Arduino_GFX_dev_device.h in the PDQgraphicstest example from the
  // GFX Library for Arduino.

// This setup is working with the board ESP32_2432S028R also known as
// Cheap Yellow Display Board <https://randomnerdtutorials.com/cheap-yellow-display-esp32-2432s028r/>
// https://cool-web.de/esp8266-esp32/esp32-2432s028-cheap-yellow-display-touchscreen-vorstellung-hardware-pinout.htm
// https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/
#define GFX_BL 21
  Arduino_DataBus *bus = new Arduino_ESP32SPI(2 /* DC */, 15 /* CS */, 14 /* SCK */, 13 /* MOSI */, 12 /* MISO */);
  Arduino_GFX *gfx = new Arduino_ILI9341(bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation */);

  // Init Display
  if (!gfx->begin()) {
    Serial.printf("gfx->begin() failed!\n");
  }

  w = gfx->width();
  h = gfx->height();
  Serial.printf("gfx width=%d height=%d\n", w, h);

  cx = w / 2;
  cy = h / 2;

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  gfx->fillScreen(RGB565_GREEN);
  delay(2000);
  Serial.println("starting...");
      gfx->fillScreen(RGB565_WHITE);
      Serial.println("2");

}

void loop(void) {
  unsigned long now = millis();

  // ArduinoOTA.handle();

  if (now > nextDraw) {
    Serial.printf("next(%d)...\n", mode);

    if (mode == 0) {
      gfx->fillScreen(RGB565_RED);
      Serial.println("2");
      delay(1000);

      gfx->fillScreen(RGB565_RED);
      Serial.println("2");
      delay(1000);

    } else if (mode == 1) {
      gfx->fillScreen(BLUE);


      // gfxDraw::pathByText(heardPath, 8, 8, 100, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

      gfxDraw::drawRect(8, 8, 87, 80, nullptr, [](int16_t x, int16_t y) {
        gfx->writePixel(x, y, RGB565_LIGHTGREY);
      });

      // (int16_t x, int16_t y, uint16_t color));
      gfxDraw::pathByText(heardPath, 8, 8, 100, gfxSetPixel(RGB565_BLUE), gfxSetPixel(RGB565_YELLOW));

    } else if (mode == 2) {
      gfx->fillScreen(RGB565_LIGHTGREY);

      for (int n = 0; n < 220; n += 8) {

        gfxDraw::drawRect(10 - 8 + n, 10 - 8 + n, 87, 80,
                          nullptr,
                          gfxSetPixel(RGB565_LIGHTGREY));

        gfxDraw::pathByText(heardPath, 10 + n, 10 + n, 100,
                            gfxSetPixel(RGB565_BLUE),
                            gfxSetPixel(RGB565_YELLOW));
        delay(20);
      }

    } else if (mode == 3) {
      gfx->fillScreen(BLACK);
      gfxDraw::gfxDrawPathWidget widget;
      widget.setStrokeColor(gfxDraw::ARGB_BLUE);
      widget.setFillColor(gfxDraw::ARGB_RED);
      widget.setPath(heardPath);

      for (int n = 0; n < 200; n += 10) {
        widget.move(8, 8);
        widget.draw([](int16_t x, int16_t y, gfxDraw::ARGB col) {
          gfx->writePixel(x, y, col.toColor16());
        });

        delay(20);
      }

    } else if (mode == 4) {
      gfx->fillScreen(BLACK);
      gfxDraw::gfxDrawPathWidget heard;
      heard.setStrokeColor(gfxDraw::ARGB_BLUE);
      heard.setFillColor(gfxDraw::ARGB_RED);
      heard.setPath(heardPath);

      int16_t posX = 0;
      int16_t posY = 0;
      for (int n = 0; n < 72; n++) {
        heard.move(1, 1);
        posX += 1;
        posY += 1;
        heard.rotate(5, posX + 44, posY + 40);
        heard.draw(gfxDrawColor);
        delay(1);
      }

      delay(100);
      gfxDraw::gfxDrawPathWidget arrow;
      arrow.setFillColor(gfxDraw::ARGB_YELLOW);
      arrow.setPath(arrowPath);
      arrow.scale(200);
      arrow.rotate(-45);
      arrow.move(posX + 44, posY + 40);
      arrow.draw(gfxDrawColor);
    }

    nextDraw = now + 2000;
    mode = (mode + 1) % 6;
  }
}  // loop
