// gfxDraw
// gfxDraw example using the "GFX Library for Arduino"


#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#include <gfxDraw.h>
#include <gfxDrawColors.h>
#include <gfxDrawObject.h>

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

Arduino_DataBus *bus;
Arduino_GFX *gfx;

uint16_t w;   // width of display
uint16_t h;   // height of display
uint16_t cx;  // Center-x of display
uint16_t cy;  // Center-y of display

uint16_t mode = 0;
uint16_t step = 0;
unsigned long nextDraw;

// a path drawing a heard. size = (87/80), center = (44/40)
const char *heardPath = "M43 7 a1 1 0 00-36 36l36 36 36-36a1 1 0 00-36-36z";

#define gfxWrite(col) [](int16_t x, int16_t y) { \
  gfx->writePixel(x, y, (col)); \
}


void setup() {
  Serial.begin(115200);

  // Serial.setDebugOutput(true);
  // while(!Serial);

  Serial.println("gfxDraw example using the GFX Library for Arduino.");

  WiFi.mode(WIFI_STA);
  WiFi.begin("KHMH", "hk-2012FD2926");
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
  // See https://homeding.github.io/boards/esp32s3/sc01-plus.htm
  //
  // To adapt other devices or screens please check Arduino_GFX_dev_device.h in the PDQgraphicstest example from the
  // GFX Library for Arduino.

#define GFX_BL 45

  bus = new Arduino_ESP32LCD8(
    0 /* DC */, GFX_NOT_DEFINED /* CS */, 47 /* WR */, GFX_NOT_DEFINED /* RD */,
    9 /* D0 */, 46 /* D1 */, 3 /* D2 */, 8 /* D3 */, 18 /* D4 */, 17 /* D5 */, 16 /* D6 */, 15 /* D7 */);

  gfx = new Arduino_ST7796(bus, 4 /* RST */, 3 /* rotation */, true /* IPS */);


  // Init Display
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }

  w = gfx->width();
  h = gfx->height();
  cx = w / 2;
  cy = h / 2;

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  gfx->fillScreen(GREEN);
}

void loop(void) {
  unsigned long now = millis();

  ArduinoOTA.handle();

  if (now > nextDraw) {
    Serial.println("next...");

    if (mode == 0) {
      gfx->fillScreen(WHITE);
    } else if (mode == 1) {
      gfx->fillScreen(BLUE);


      // gfxDraw::pathByText(heardPath, 8, 8, 100, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

      gfxDraw::drawSolidRect(8, 8, 87, 80, [](int16_t x, int16_t y) {
        gfx->writePixel(x, y, RGB565_LIGHTGREY);
      });

      // (int16_t x, int16_t y, uint16_t color));
      gfxDraw::pathByText(heardPath, 8, 8, 100, gfxWrite(RGB565_BLUE), gfxWrite(RGB565_YELLOW));

    } else if (mode == 2) {
      gfx->fillScreen(RGB565_LIGHTGREY);

      for (int n = 0; n < 220; n += 8) {

        gfxDraw::drawSolidRect(10 - 8 + n, 10 - 8 + n, 87, 80,
                               gfxWrite(RGB565_LIGHTGREY));

        gfxDraw::pathByText(heardPath, 10 + n, 10 + n, 100,
                            gfxWrite(RGB565_BLUE),
                            gfxWrite(RGB565_YELLOW));
        delay(20);
      }

    } else if (mode == 3) {
      gfx->fillScreen(BLACK);
      gfxDraw::gfxDrawObject widget;
      widget.setStrokeColor(gfxDraw::RGBA_BLUE);
      widget.setFillColor(gfxDraw::RGBA_RED);
      widget.setPath(heardPath);

      for (int n = 0; n < 200; n += 10) {
        widget.move(8, 8);
        widget.draw([](int16_t x, int16_t y, gfxDraw::RGBA col) {
          gfx->writePixel(x, y, col.toColor16());
        });

        delay(20);
      }

    } else if (mode == 4) {
      gfx->fillScreen(BLACK);
      gfxDraw::gfxDrawObject widget;
      widget.setStrokeColor(gfxDraw::RGBA_BLUE);
      widget.setFillColor(gfxDraw::RGBA_RED);
      widget.setPath(heardPath);

      int16_t posX = 0;
      int16_t posY = 0;
      for (int n = 0; n < 72; n++) {
        widget.move(1, 1);
        posX += 1;
        posY += 1;

        widget.rotate(5, posX + 44, posY + 40);

        widget.draw([](int16_t x, int16_t y, gfxDraw::RGBA col) {
          gfx->writePixel(x, y, col.toColor16());
        });

        delay(1);
      }
    }


    nextDraw = now + 2000;
    mode = (mode + 1) % 6;
  }
}  // loop
