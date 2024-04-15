#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>

#include "gfxDraw.h"
#include "bitmap.h"

// https://doc.magnum.graphics/magnum/

using namespace std;

CBitmap *bmp = nullptr;  //  new CBitmap();

#define bmpSet(col) [](int16_t x, int16_t y) { \
  bmp->setPixel(x, y, col); \
}

#define bmpDraw() [](int16_t x, int16_t y, gfxDraw::RGBA col) { \
  bmp->setPixel(x, y, col); \
}

void drawDigits14(int n, uint16_t x, uint16_t y, gfxDraw::fSetPixel cbBorder, gfxDraw::fSetPixel cbFill = nullptr) {

  uint16_t SD[10] = {
    0b0011000100100011,  // 0
    0b0001100000100000,  // 1
    0b0011000011000011,  // 2
    0b0011000010100001,  // 3
    0b0001000111100000,  // 4
    0b0010000111100001,  // 5
    0b0010000111100011,  // 6
    0b0010100000001000,  // 7
    0b0011000111100011,  // 8
    0b0011000111100001   // 9
  };
  uint16_t sd = SD[n % 10];

  // clang-format off
  if (sd & 0b0000000000000001) gfxDraw::pathByText("M 10 1h49l3 3l-5 5h-45l-5 -5Z",       x, y, 100, cbBorder, cbFill);
  if (sd & 0b0000000000000010) gfxDraw::pathByText("M1 10v42h4l5 -5v-36l-5 -5Z",          x, y, 100, cbBorder, cbFill);
  if (sd & 0b0000000000000100) gfxDraw::pathByText("M13 12h4l10 20v15h-3l-11 -23z",       x, y, 100, cbBorder, cbFill);
  if (sd & 0b0000000000001000) gfxDraw::pathByText("M30 12h9v34l-4 4h-1l-4 -4z",          x, y, 100, cbBorder, cbFill);
  if (sd & 0b0000000000010000) gfxDraw::pathByText("M56 12h-4l-10 20v15h3l11 -23z",       x, y, 100, cbBorder, cbFill);
  if (sd & 0b0000000000100000) gfxDraw::pathByText("M68 10v42l-4 0l-5 -5v-36 l5-5z",      x, y, 100, cbBorder, cbFill);
  if (sd & 0b0000000001000000) gfxDraw::pathByText("M11 50h18l4 4v1l-4 4h-18l-4 -4v-1z",  x, y, 100, cbBorder, cbFill);
  if (sd & 0b0000000010000000) gfxDraw::pathByText("M58 50h-18l-4 4v1l4 4h18l4 -4v-1z",   x, y, 100, cbBorder, cbFill);
  if (sd & 0b0000000100000000) gfxDraw::pathByText("M1, 99v-42l4, 0l5, 5v36l-5, 5z",      x, y, 100, cbBorder, cbFill);
  if (sd & 0b0000001000000000) gfxDraw::pathByText("M13, 97h4 l10, -20v-15h-3l-11, 23z",  x, y, 100, cbBorder, cbFill);
  if (sd & 0b0000010000000000) gfxDraw::pathByText("M30, 97h9v-34l-4, -4h-1l-4, 4",       x, y, 100, cbBorder, cbFill);
  if (sd & 0b0000100000000000) gfxDraw::pathByText("M56, 97h-4l-10, -20v-15h3l11, 23z",   x, y, 100, cbBorder, cbFill);
  if (sd & 0b0001000000000000) gfxDraw::pathByText("M68, 99v-42l-4, 0l-5, 5v36l5, 5z",    x, y, 100, cbBorder, cbFill);
  if (sd & 0b0010000000000000) gfxDraw::pathByText("M10, 108h49l3, -3l-5, -5h-45l-5, 5z", x, y, 100, cbBorder, cbFill);
  // clang-format on
}


void drawDigits7(int n, uint16_t x, uint16_t y, gfxDraw::fSetPixel cbBorder, gfxDraw::fSetPixel cbFill = nullptr) {

  uint8_t SD[10] = {
    0b01110111,  // 0
    0b00100100,  // 1
    0b01101011,  // 2
    0b01101101,  // 3
    0b00111100,  // 4
    0b01011101,  // 5
    0b01011111,  // 6
    0b01100100,  // 7
    0b01111111,  // 8
    0b01111101   // 9
  };
  uint16_t sd = SD[n % 10];

  if (sd & 0b00000001) gfxDraw::pathByText100("M3 1h23l-4 4h-15z", x, y, cbBorder, cbFill);
  if (sd & 0b00000010) gfxDraw::pathByText100("M 1 2 l4 4 v15 l-4 4 z", x, y, cbBorder, cbFill);
  if (sd & 0b00000100) gfxDraw::pathByText100("M28 2 l-4 4 v15 l4 4 z", x, y, cbBorder, cbFill);
  if (sd & 0b00001000) gfxDraw::pathByText100("M3 26 l2-2 h19 l2 2 l -2 2 h-19z", x, y, cbBorder, cbFill);
  if (sd & 0b00010000) gfxDraw::pathByText100("M1 27 l4 4 v15 l-4 4 z", x, y, cbBorder, cbFill);
  if (sd & 0b00100000) gfxDraw::pathByText100("M28 27 l-4 4 v15 l4 4 z", x, y, cbBorder, cbFill);
  if (sd & 0b01000000) gfxDraw::pathByText100("M3 51 h23 l-4 -4h-15z", x, y, cbBorder, cbFill);
}


// draw the test paths for outTest.bmp
void assert(bool b, char *text) {
  if (!b) {
    printf("ALERT: %s\n", text);
  }
}

// draw the test paths for outTest.bmp
void silentTests() {
  uint16_t count;
  printf("\nSilent Tests\n");

  count = 0;
  gfxDraw::drawLine(5, 5, 10, 5, [&](int16_t x, int16_t y) {
    count++;
    assert((y == 5), "draw false point");
  });
  assert((count <= 6), "draw too many points");
  assert((count >= 6), "draw not enough points");

  count = 0;
  gfxDraw::drawLine(5, 5, 5, 10, [&](int16_t x, int16_t y) {
    count++;
    assert((x == 5), "draw false point");
  });
  assert((count <= 6), "draw too many points");
  assert((count >= 6), "draw not enough points");

  count = 0;
  gfxDraw::drawLine(5, 5, 20, 10, [&](int16_t x, int16_t y) {
    count++;
  });
  assert((count <= 16), "draw too many points");
  assert((count >= 16), "draw not enough points");

  count = 0;
  gfxDraw::drawCubicBezier(10, 10, 11, 2, 25, 18, 26, 10, [&](int16_t x, int16_t y) {
    count++;
  });
  assert((count <= 18), "draw too many points");
  assert((count >= 18), "draw not enough points");

  printf("\n");
}


// draw the test paths for outTest.bmp
void drawTest1() {
  printf("\nDraw Tests #1\n");

#if 1
  printf("\nTest-01:\n");
  // gfxDraw::rect(10, 10, 10, 10, nullptr, bmpSet(gfxDraw::SILVER));
  // 8*8 rectange needs 2 more points on horizontal lines
  gfxDraw::pathByText100("M1 1 h7 v7 h-7 z",
                         10, 10,
                         bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  printf("\nTest-02:\n");
  // gfxDraw::rect(20, 10, 10, 10, nullptr, bmpSet(gfxDraw::GRAY));
  // 7*8 rectange needs 2 more points on horizontal lines
  gfxDraw::pathByText100("M1 1 h6 v7 h-6 z",
                         20, 10,
                         bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  printf("\nTest-03:\n");
  // gfxDraw::rect(30, 10, 11, 10, nullptr, bmpSet(gfxDraw::SILVER));
  // closed W
  gfxDraw::pathByText100("M1 1 h8 v7 l-4,-4 l-4 4 z",
                         30, 10,
                         bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  printf("\nTest-04: closed M\n");
  // gfxDraw::rect(41, 10, 11, 10, nullptr, bmpSet(gfxDraw::GRAY));
  gfxDraw::pathByText100("M1 1 l4,4 l4 -4 v7 h-8 z",
                         41, 10,
                         bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  printf("\nTest-05: route\n");
  // gfxDraw::rect(52, 10, 11, 10, nullptr, bmpSet(gfxDraw::SILVER));
  gfxDraw::pathByText100("M5 1 l4,4 l-4 4 l-4 -4 z",
                         52, 10,
                         bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  printf("\nTest-06: |> symbol\n");
  gfxDraw::drawRect(63, 10, 10, 10, bmpSet(gfxDraw::GRAY));
  gfxDraw::pathByText100("M1 1 l7,4 l-7 4 z",
                         63, 10,
                         bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  printf("\nTest-07: <| symbol\n");
  gfxDraw::drawRect(73, 10, 10, 10, bmpSet(gfxDraw::SILVER));
  gfxDraw::pathByText100("M8 1 l-7,4 l7 4 z",
                         73, 10,
                         bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));
#endif

  printf("\nTest-10: [[]] \n");
  gfxDraw::drawRect(10, 22, 10, 10, bmpSet(gfxDraw::SILVER));
  gfxDraw::pathByText100("M1 1 h7 v7 h-7 z M3 3 h3 v3 h-3 z",
                         10, 22,
                         bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));


#if 1
  printf("\nTest-11: [*] \n");
  gfxDraw::drawRect(20, 22, 10, 10, bmpSet(gfxDraw::GRAY));
  gfxDraw::pathByText100("M1 1 h7 v7 h-7 z M4 4 h1 v1 h-1 z",
                         20, 22,
                         bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));
#endif

#if 1
  gfxDraw::drawRect(10, 34, 27, 24, bmpSet(gfxDraw::SILVER));
  gfxDraw::pathByText("M2 52 l60-50 v20 h20 l-30 50z", 10, 34, 30,
                      bmpSet(gfxDraw::BLUE),
                      [](int16_t x, int16_t y) {
                        gfxDraw::RGBA col = gfxDraw::RGBA(0xC0, 0x0, 0, 0xFF);
                        col.Green += (7 * x);
                        col.Blue += (7 * y);
                        bmp->setPixel(x, y, col);
                      });
#endif

#if 1
  // gfxDrawObject *rect2 = new gfxDrawObject(gfxDraw::TRANSPARENT, gfxDraw::GREEN);
  // rect2->setRect(46, 30);
  // // rect2->setFillGradient(gfxDraw::YELLOW, 10, 0, gfxDraw::ORANGE, 36, 0);
  // rect2->setFillGradient(gfxDraw::YELLOW, 0, 2, gfxDraw::BLUE, 0, 22);
  // rect2->draw(4, 240, bmpDraw());

  gfxDraw::drawRect(40, 34, 43, 25, bmpSet(gfxDraw::SILVER));
  gfxDraw::pathByText(
    "M2 42 l20-33 l20 38 l20 -38 l20 33 l0 -40 l-70 0 z",
    40, 34, 50,
    bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  // radio with 2 inner paths
  gfxDraw::pathByText(
    "M12,2h64c4,0 8,4 8,8v48c0,4 -4,8 -8,8h-64c-4,0 -8,-4 -8,-8v-48c0,-4 4,-8 8,-8z"
    "M12,10 h60v20h-60z"
    "M24,36c6,0 12,6 12,12c0,6 -6,12 -12,12c-6,0 -12,-6 -12,-12c0,-6 6,-12 12,-12z",
    90, 34, 50,
    bmpSet(gfxDraw::BLACK), bmpSet(gfxDraw::YELLOW));
#endif
}

void drawP() {

  gfxDraw::drawRect(100, 10, 49, 49, bmpSet(gfxDraw::RED));
  gfxDraw::pathByText(
    "M24 0c-14 0-24 10-24 24 c0 14 10 24 24 24 c14 0 24-10 24-24 c0-14-10-24-24-24Z"
    "M16 12c5 0 5 7 0 7 c-5 0-5-7 0-7Z"
    "M32 12c5 0 5 7 0 7 c-5 0-5-7 0-7Z"
    "M38 32c0 1-1 2-2 3 c-6 5-12 6-19 3 c-2-2-5-3-5-6 c1-2 3 0 4 1 c4 3 9 5 15 2 c3-2 3-2 5-4 c1-1 2-1 2 1Z",
    100, 10, 200, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  gfxDraw::drawCubicBezier(5, 10,
                           10, -5,
                           20, 25,
                           26, 10,
                           bmpSet(gfxDraw::RED));

  std::vector<gfxDraw::Segment> segs = gfxDraw::parsePath("M1 1 h7 v7 h-7 z M4 4 h1 v1 h-1 z");
  gfxDraw::scaleSegments(segs, 200);  // scale by 200%
  gfxDraw::fillSegments(segs, 0, 20, 
                        bmpSet(gfxDraw::BLACK),
                        bmpSet(gfxDraw::WHITE));  // hard-coded fill color here.


  // gfxDraw::rect(4, 200, 47, 31, nullptr, bmpSet(gfxDraw::SILVER));
  // gfxDraw::pathByText("M0 12 l24-12 l20 20 h-16 v8 h-12 z", 5, 201, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  // gfxDrawObject *rect1 = new gfxDrawObject(gfxDraw::TRANSPARENT, gfxDraw::SILVER);
  // rect1->setRect(46, 30);
  // rect1->draw(4, 200, bmpDraw());

  // gfxDrawObject *draw1 = new gfxDrawObject(gfxDraw::BLUE, gfxDraw::ORANGE);
  // draw1->setPath("M0 12 l24-12 l20 20 h-16 v8 h-12 z");
  // draw1->draw(5, 201, bmpDraw());


#if 0

  // gfxDraw::drawCubicBezier(80, 200, 80, 240, 150, 240, 150, 200, bmpSet(gfxDraw::BLACK));

  // da->rect(180, 200, 84, 74, nullptr, bmpSet(gfxDraw::SILVER), 2);
  // gfxDraw::pathByText("M80 200 C 80,240 150,240 150,200 z", 0,0, bmpSet(gfxDraw::BLACK), bmpSet(gfxDraw::YELLOW));
#endif


  //  C 6,23 4,26 4,30 v 48 c 0,4 3,8 8,8 h 64 c 4,0 8,-3 8,-8 V 30 c 0,-4 -3,-8 -8,-8 H 29 L 62,8 59,2 Z",
  // M 8.94,22.6 C 6.04,23.74 4,26.66 4,30 v 48 c 0,4.42 3.58,8 8,8 h 64 c 4.42,0 8,-3.58 8,-8 V 30 c 0,-4.42 -3.58,-8 -8,-8 H 29.22 L 62.28,8.66 59.52,2 Z M 24,78 c -6.62,0 -12,-5.38 -12,-12 0,-6.62 5.38,-12 12,-12 6.62,0 12,5.38 12,12 0,6.62 -5.38,12 -12,12 z M 76,46 H 12 V 30 h 64 z
};


#if 0
void drawClock(uint16_t _cx, uint16_t _cy, uint16_t _radius) {

  float rad1 = (M_PI * 2 / 60);
  int16_t x0, y0, x1, y1;

  // uint32_t color;

  // _display->setBackgroundColor(_backgroundColor);
  // _display->setBorderColor(_borderColor);
  // _display->drawCircle(_cx, _cy, _radius);

  da->drawCircle(
    _cx, _cy, _radius,
    [bmp](int16_t x, int16_t y) {
      bmp->setPixel(x, y, gfxDraw::BLACK);
    });


  for (uint8_t i = 0; i < 60; i++) {
    float deg = (rad1 * i);

    x0 = sin(deg) * _radius;
    y0 = -cos(deg) * _radius;

    x1 = 0;
    y1 = 0;

    if ((i % 15) == 0) {
      // x1 = (x0 * 9.0) / 12.0;
      // y1 = (y0 * 9.0) / 12.0;
      da->line(_cx + x0, _cy + y0, _cx + x1, _cy + y1, bmpSet(gfxDraw::BLUE), 5);

    } else if ((i % 5) == 0) {
      // x1 = (x0 * 10.0) / 12.0;
      // y1 = (y0 * 10.0) / 12.0;
      da->line(_cx + x0, _cy + y0, _cx + x1, _cy + y1, bmpSet(gfxDraw::GREEN), 3);

    } else {
      // x1 = (x0 * 11.5) / 12.0;
      // y1 = (y0 * 11.5) / 12.0;
      da->line(_cx + x0, _cy + y0, _cx + x1, _cy + y1, bmpSet(gfxDraw::BLACK), 1);
    }
  }  // for
}

#endif

// fSetPixel fBlack(CBitmap *bm, int32_t x, int32_t y) {
//   bm->setPixel(x, y, gfxDraw::BLACK);
// }

int main() {
  if (sizeof(gfxDraw::RGBA) != 4) {
    // cout << "size(Segment):" << sizeof(gfxDraw::Segment) << endl;
    cout << "error: size(RGBA) is not 4!" << endl;
  }

  if (sizeof(gfxDraw::Segment) != 14) {
    // cout << "size(RGBA):" << sizeof(testRGBA) << endl;
    cout << "error: size(Segment) is not 14!" << endl;
  }

  silentTests();

  bmp = new CBitmap("startTest.bmp");

  cout << "w:" << bmp->GetWidth() << endl;
  cout << "h:" << bmp->GetHeight() << endl;
  drawTest1();

  bmp->Save("test.bmp");


  bmp = new CBitmap("start.bmp");

  if (1) {

    // gfxDraw::rect(10, 50, 12, 8, bmpSet(gfxDraw::BLACK), bmpSet(gfxDraw::CYAN), 2);
    // gfxDraw::rect(10, 60, 12, 8, nullptr, bmpSet(gfxDraw::CYAN), 2);
    // gfxDraw::rect(10, 70, 12, 8, bmpSet(gfxDraw::BLACK), nullptr, 2);

    // gfxDraw::rect(30, 70, 10, 8, bmpSet(gfxDraw::RED), nullptr, 2);
    // gfxDraw::rect(39, 70, -10, 8, bmpSet(gfxDraw::BLACK), nullptr, 2);

    // gfxDraw::rect(42, 70, 10, 8, nullptr, bmpSet(gfxDraw::RED), 2);
    // gfxDraw::rect(51, 70, -10, 8, nullptr, bmpSet(gfxDraw::BLACK), 2);

    // gfxDraw::rect(54, 70, 10, 8, nullptr, bmpSet(gfxDraw::RED), 2);
    // gfxDraw::rect(54, 77, 10, -8, nullptr, bmpSet(gfxDraw::BLACK), 2);
  }

  // drawClock(120, 205, 80);

  drawP();

  // drawDigits14(1, 0, 10, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));
  // drawDigits14(2, 80, 10, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));
  // drawDigits14(5, 160, 10, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));
  // drawDigits14(9, 250, 10, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));

  // drawDigits7(0, 0, 130, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));
  // drawDigits7(1, 34, 130, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));
  // drawDigits7(2, 68, 130, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));
  // drawDigits7(3, 102, 130, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));
  // drawDigits7(4, 136, 130, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));
  // drawDigits7(5, 170, 130, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));
  // drawDigits7(6, 204, 130, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));
  // drawDigits7(7, 238, 130, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));
  // drawDigits7(8, 282, 130, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));
  // drawDigits7(9, 316, 130, bmpSet(gfxDraw::RED), bmpSet(gfxDraw::YELLOW));

  bmp->Save("out.bmp");
}
