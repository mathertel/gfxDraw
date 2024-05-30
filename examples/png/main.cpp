#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>
#include <complex>

#include "gfxDraw.h"
#include "gfxDrawCircle.h"
#include "gfxDrawObject.h"

#include "lodepng.h"


// https://doc.magnum.graphics/magnum/

using namespace std;

// ===== image memory allocation and functions

// The image memory is "just" an 2-dimensional array of bytes (4 * width * height)
// each pixel is using 4 bytes for (RGBA)

std::vector<unsigned char> image;
uint16_t imageWidth;
uint16_t imageHeight;

void newImage(uint16_t w, uint16_t h) {
  imageWidth = w;
  imageHeight = h;
  image.resize(w * h * 4);
}

void saveImage(char *fileName) {
  std::vector<unsigned char> png;
  unsigned error = lodepng::encode(png, image, imageWidth, imageHeight);
  if (!error) lodepng::save_file(png, fileName);

  // if there's an error, display it
  if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

/// A lambda function to parse a parameter from the inputText.
auto setImagePixel = [&](int16_t x, int16_t y, gfxDraw::RGBA color) {
  if ((x >= 0) && (x < imageWidth) && (y >= 0) && (y < imageHeight)) {
    image[4 * imageWidth * y + 4 * x + 0] = color.Red;
    image[4 * imageWidth * y + 4 * x + 1] = color.Green;
    image[4 * imageWidth * y + 4 * x + 2] = color.Blue;
    image[4 * imageWidth * y + 4 * x + 3] = 255;
  }
};

void fillImage(gfxDraw::RGBA color) {
  for (unsigned y = 0; y < imageHeight; y++) {
    for (unsigned x = 0; x < imageWidth; x++) {
      setImagePixel(x, y, color);
    }
  }
}

#define bmpSet(col) [](int16_t x, int16_t y) { \
  setImagePixel(x, y, (col)); \
}

#define bmpDraw() [](int16_t x, int16_t y, gfxDraw::RGBA col) { \
  setImagePixel(x, y, col); \
}

// ===== Test paths

const char *SmilieCurvePath =
  "M24 0c-14 0-24 10-24 24 c0 14 10 24 24 24 c14 0 24-10 24-24 c0-14-10-24-24-24Z"
  "M16 12c5 0 5 7 0 7 c-5 0-5-7 0-7Z"
  "M32 12c5 0 5 7 0 7 c-5 0-5-7 0-7Z"
  "M38 32c0 1-1 2-2 3 c-6 5-12 6-19 3 c-2-2-5-3-5-6 c1-2 3 0 4 1 c4 3 9 5 15 2 c3-2 3-2 5-4 c1-1 2-1 2 1Z";

const char *SmileyArcPath =
  "M44 24 a20 20 0 0 1 -20 20A20 20 0 0 1 4 24 A20 20 0 0 1 24 4 a20 20 0 0 1 20 20z"
  "M20 16 a 4 4 0 0 1  -4 4 a4 4 0 0 1 -4 -4 a4 4 0 0 1 4-4 a4 4 0 0 1 4 4z"
  "M36 16 a 4 4 0 0 1  -4 4 a4 4 0 0 1 -4 -4 a4 4 0 0 1 4-4 a4 4 0 0 1 4 4z"
  "M36 32 a 12 4 0 0 1 -12 4 a12 4 0 0 1-12-4 a12 4 0 0 1 12-4 a12 4 0 0 1 12 4z";

const char *RadioPath =
  "M12,2h64c4,0 8,4 8,8v48c0,4 -4,8 -8,8h-64c-4,0 -8,-4 -8,-8v-48c0,-4 4,-8 8,-8z"
  "M12,10 h60v20h-60z"
  "M24,36c6,0 12,6 12,12c0,6 -6,12 -12,12c-6,0 -12,-6 -12,-12c0,-6 6,-12 12,-12z";

const char *PiePath = R"==(
   M150,150 v-150 a150,150 0 0,0 -150,150 z
   M175,175 h-150 a150,150 0 1,0 150,-150 z
   )==";

const char *SwordPath = R"==(
M 40 80 L 100 10 L 130 0 L 120 30 L 50 90 C 60 100 60 110 70 100 C 70 110 80 120 70 120 A 14 14 0 0 1 60 130 A 50 50 0 0 0 40 100 C 36 99 36 99 35 105 l -15 13 C 10 121 10 121 12 110 L 25 95 C 31 94 31 94 30 90 A 50 50 90 0 0 0 70 A 14 14 0 0 1 10 60 C 10 50 20 60 30 60 C 20 70 30 70 40 80 M 100 10 L 100 30 L 120 30 L 102 28 L 100 10z
)==";

// =====



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

  if (sd & 0b00000001) gfxDraw::pathByText("M3 1h23l-4 4h-15z", x, y, 100, cbBorder, cbFill);
  if (sd & 0b00000010) gfxDraw::pathByText("M 1 2 l4 4 v15 l-4 4 z", x, y, 100, cbBorder, cbFill);
  if (sd & 0b00000100) gfxDraw::pathByText("M28 2 l-4 4 v15 l4 4 z", x, y, 100, cbBorder, cbFill);
  if (sd & 0b00001000) gfxDraw::pathByText("M3 26 l2-2 h19 l2 2 l -2 2 h-19z", x, y, 100, cbBorder, cbFill);
  if (sd & 0b00010000) gfxDraw::pathByText("M1 27 l4 4 v15 l-4 4 z", x, y, 100, cbBorder, cbFill);
  if (sd & 0b00100000) gfxDraw::pathByText("M28 27 l-4 4 v15 l4 4 z", x, y, 100, cbBorder, cbFill);
  if (sd & 0b01000000) gfxDraw::pathByText("M3 51 h23 l-4 -4h-15z", x, y, 100, cbBorder, cbFill);
}  // drawDigits7()


// draw the test paths for outTest.bmp
void assert(bool b, char *text) {
  if (!b) {
    printf("ALERT: %s\n", text);
  }
}

// draw the test paths for outTest.bmp
void silentTests() {
  uint16_t count;
  TRACE("\nSilent Tests:\n");

  if (sizeof(gfxDraw::RGBA) != 4) {
    // cout << "size(Segment):" << sizeof(gfxDraw::Segment) << endl;
    cout << "error: size(RGBA) is not 4!" << endl;
  }

  if (sizeof(gfxDraw::Segment) != 14) {
    // cout << "size(RGBA):" << sizeof(testRGBA) << endl;
    cout << "error: size(Segment) is not 14!" << endl;
  }

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
  assert((count <= 19), "draw too many points");
  assert((count >= 19), "draw not enough points");

  int16_t a;
  a = gfxDraw::vectorAngle(1, 1);
  assert((a == 45), "vectorAngle( 1,  1) error");
  a = gfxDraw::vectorAngle(-1, 1);
  assert((a == 135), "vectorAngle(-1,  1) error");
  a = gfxDraw::vectorAngle(-1, -1);
  assert((a == 225), "vectorAngle(-1, -1) error");
  a = gfxDraw::vectorAngle(1, -1);
  assert((a == 315), "vectorAngle( 1, -1) error");

  TRACE("\n");
}


void drawTest01() {
  TRACE("\nDraw Tests #1\n");

  auto drawHelper = [](const char *pathText, int16_t x, int16_t y) {
    std::vector<gfxDraw::Segment> vSeg = gfxDraw::parsePath(pathText);
    fillSegments(vSeg, x, y, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::RGBA(255, 255, 128)));
  };

  TRACE("\nTest-01:\n");
  // 8*8 rectange needs 2 edges on horizontal lines
  gfxDraw::drawSolidRect(10, 10, 10, 10, bmpSet(gfxDraw::SILVER));
  drawHelper("M1 1 h7 v7 h-7 z", 10, 10);

  TRACE("\nTest-02:\n");
  // 7*8 rectange needs 2 more points on horizontal lines
  gfxDraw::drawSolidRect(20, 10, 10, 10, bmpSet(gfxDraw::GRAY));
  drawHelper("M1 1 h6 v7 h-6 z", 20, 10);

  TRACE("\nTest-03:\n");
  // closed W
  gfxDraw::drawSolidRect(30, 10, 11, 10, bmpSet(gfxDraw::SILVER));
  drawHelper("M1 1 h8 v7 l-4,-4 l-4 4 z", 30, 10);

  TRACE("\nTest-04: closed M\n");
  gfxDraw::drawSolidRect(41, 10, 11, 10, bmpSet(gfxDraw::GRAY));
  drawHelper("M1 1 l4,4 l4 -4 v7 h-8 z", 41, 10);

  TRACE("\nTest-05: route\n");
  gfxDraw::drawSolidRect(52, 10, 11, 10, bmpSet(gfxDraw::SILVER));
  drawHelper("M5 1 l4,4 l-4 4 l-4 -4 z", 52, 10);

  TRACE("\nTest-06: |> symbol\n");
  gfxDraw::drawSolidRect(63, 10, 10, 10, bmpSet(gfxDraw::GRAY));
  drawHelper("M1 1 l7,4 l-7 4 z", 63, 10);

  TRACE("\nTest-07: <| symbol\n");
  gfxDraw::drawSolidRect(73, 10, 10, 10, bmpSet(gfxDraw::SILVER));
  drawHelper("M8 1 l-7,4 l7 4 z", 73, 10);

  TRACE("\nTest-10: [[]] \n");
  gfxDraw::drawSolidRect(10, 22, 10, 10, bmpSet(gfxDraw::SILVER));
  drawHelper("M1 1 h7 v7 h-7 z M3 3 h3 v3 h-3 z", 10, 22);

  TRACE("\nTest-11: [*] \n");
  gfxDraw::drawSolidRect(20, 22, 10, 10, bmpSet(gfxDraw::GRAY));
  drawHelper("M1 1 h7 v7 h-7 z M4 4 h1 v1 h-1 z", 20, 22);

  gfxDraw::drawSolidRect(10, 34, 27, 24, bmpSet(gfxDraw::SILVER));
  gfxDraw::pathByText("M2 52 l60-50 v20 h20 l-30 50z", 10, 34, 30,
                      bmpSet(gfxDraw::BLUE),
                      [](int16_t x, int16_t y) {
                        gfxDraw::RGBA col = gfxDraw::RGBA(0xC0, 0x0, 0, 0xFF);
                        col.Green += (7 * x);
                        col.Blue += (7 * y);
                        setImagePixel(x, y, col);
                      });


  gfxDraw::drawSolidRect(40, 34, 43, 26, bmpSet(gfxDraw::SILVER));
  gfxDraw::pathByText(
    "M2 42 l20-33 l20 38 l20 -38 l20 33 l0 -40 l-70 0 z",
    40, 34, 50,
    bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  // radio with 2 inner paths
  gfxDraw::drawSolidRect(91, 34, 43, 35, bmpSet(gfxDraw::SILVER));
  gfxDraw::pathByText(RadioPath, 90, 34, 50, bmpSet(gfxDraw::BLACK), bmpSet(gfxDraw::YELLOW));

  // smiley with beziers
  gfxDraw::drawSolidRect(10, 80, 99, 99, bmpSet(gfxDraw::GREEN));
  gfxDraw::pathByText(SmilieCurvePath, 11, 81, 200, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  // smiley with Arcs
  gfxDraw::drawSolidRect(120, 80, 99, 99, bmpSet(gfxDraw::GREEN));
  gfxDraw::pathByText(SmileyArcPath, 120, 81, 200, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  // gfxDraw::drawCubicBezier(5, 10, 10, -5, 20, 25, 26, 10,
  //                          [](int16_t x, int16_t y) {
  //                            setImagePixel(x + 110, y + 80, gfxDraw::RED);
  //                          });

}  // drawTest01()


// draw the test paths for test02.png
void drawTest02() {
  std::vector<gfxDraw::Segment> segs = gfxDraw::parsePath("M1 1 h7 v7 h-7 z M3 3 h3 v3 h-3 z");
  gfxDraw::scaleSegments(segs, 400);                                                       // scale by 400%
  gfxDraw::moveSegments(segs, 100, 0);                                                     // move to the right
  gfxDraw::fillSegments(segs, bmpSet(gfxDraw::BLACK), bmpSet(gfxDraw::RGBA(0xEEEEEEFF)));  // hard-coded fill color here.

  gfxDraw::moveSegments(segs, 22, 15);                                                     // move right down
  gfxDraw::fillSegments(segs, bmpSet(gfxDraw::BLACK), bmpSet(gfxDraw::RGBA(0xEEEEEEFF)));  // hard-coded fill color here.

  segs = gfxDraw::parsePath("M1 1 h7 v7 h-7 z M3 3 h3 v3 h-3 z");
  gfxDraw::scaleSegments(segs, 400);                                                       // scale by 400%
  gfxDraw::rotateSegments(segs, 25);                                                       // rotate some degrees
  gfxDraw::moveSegments(segs, 200, 40);                                                    // move right down
  gfxDraw::fillSegments(segs, bmpSet(gfxDraw::BLACK), bmpSet(gfxDraw::RGBA(0xEEEEEEFF)));  // hard-coded fill color here.


  segs = gfxDraw::parsePath(SmilieCurvePath);
  gfxDraw::scaleSegments(segs, 200);
  gfxDraw::rotateSegments(segs, 25);                                           // rotate some degrees
  gfxDraw::moveSegments(segs, 40, 40);                                         // move right down
  gfxDraw::fillSegments(segs, bmpSet(gfxDraw::BLACK), bmpSet(gfxDraw::LIME));  // hard-coded fill color here.

  gfxDraw::gfxDrawObject *rect = new gfxDraw::gfxDrawObject(gfxDraw::BLUE, gfxDraw::YELLOW);
  rect->setRect(32, 16);

  // rotate around center
  rect->move(-16, -8);
  rect->rotate(30);
  rect->move(16, 8);

  rect->scale(200);
  rect->move(20, 20);
  rect->draw(bmpDraw());

  // // rect2->setFillGradient(gfxDraw::YELLOW, 10, 0, gfxDraw::ORANGE, 36, 0);
  // rect2->setFillGradient(gfxDraw::YELLOW, 0, 2, gfxDraw::BLUE, 0, 22);
  // rect2->draw(4, 240, bmpDraw());

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


using namespace gfxDraw;

// A SVG path defining the shape of a heard
const char *heardPath = "M43 7 a1 1 0 00-36 36l36 36 36-36a1 1 0 00-36-36z";

void drawTest04() {
  // gfxDraw::pathByText(heardPath, 8, 8, 100, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  drawSolidRect(8, 8, 87, 80, bmpSet(SILVER));
  // pathByText(heardPath, 8, 8, 100, bmpSet(BLUE), bmpSet(YELLOW));

  // gfxDrawObject *widget = new gfxDrawObject();
  // widget->setStrokeColor(YELLOW);
  // widget->setFillColor(RED);
  // widget->setPath(heardPath);
  // widget->move(8, 8);
  // widget->draw(bmpDraw());

  gfxDrawObject widget;
  widget.setStrokeColor(YELLOW);
  widget.setFillColor(RED);
  widget.setPath(heardPath);
  widget.move(8, 8);
  widget.draw(bmpDraw());
}

void drawTest03() {

  // gfxDraw::pathByText("M24 0c-14 0-24 10-24 24 c0 14 10 24 24 24 c14 0 24-10 24-24 c0-14-10-24-24-24Z",
  // gfxDraw::pathByText("M24 0c-14 0-24 10-24 24  0 14 10 24 24 24  14 0 24-10 24-24  0-14-10-24-24-24Z", 11, 81, 200, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  // Simple Arc drawing test cases
  // gfxDraw::pathByText("M2 2 A 10 30 0 0 0 20 10 Z", 2, 2, 200, bmpSet(gfxDraw::BLUE), nullptr);
  // gfxDraw::pathByText("M112 102 A 12 12 0 0 0 128 118 ", 2, 2, 100, bmpSet(gfxDraw::BLUE), nullptr);

  // gfxDraw::pathByText("M30 112 A 100 100 0 0 1 180 220 Z", 2, 2, 10, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  // gfxDraw::drawSolidRect(210, 10, 99, 99, bmpSet(gfxDraw::GREEN));
  // gfxDraw::pathByText(SmilieCurvePath, 211, 11, 200, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  // gfxDraw::drawSolidRect(210, 120, 99, 99, bmpSet(gfxDraw::GREEN));

  gfxDraw::pathByText(PiePath, 8, 8, 30, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  gfxDraw::pathByText(SwordPath, 10, 160, 100, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  gfxDraw::pathByText(SmileyArcPath, 200, 100, 100, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));
  gfxDraw::pathByText(SmileyArcPath, 140, 10, 200, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::GREEN));
  gfxDraw::pathByText(SmileyArcPath, 220, 10, 50, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));
  gfxDraw::pathByText(SmileyArcPath, 100, 80, 150, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));

  // gfxDraw::pathByText("O 20 20 20", 10, 120, 100, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::YELLOW));
  gfxDraw::pathByText("O 20 20 20 O 20 20 10", 10, 120, 100, bmpSet(gfxDraw::BLUE), bmpSet(gfxDraw::RGBA(0xFF, 0xFF, 0)));
}


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


int main() {
  // silentTests();

#if (1)
  newImage(400, 300);
  fillImage(gfxDraw::WHITE);
  drawTest01();
  saveImage("test01.png");
#endif

#if (1)
  newImage(400, 300);
  fillImage(gfxDraw::WHITE);

  drawTest02();

  saveImage("test02.png");
#endif


#if (1)
  newImage(400, 300);
  fillImage(gfxDraw::WHITE);

  drawTest03();

  saveImage("test03.png");
#endif

#if (1)
  newImage(400, 300);
  fillImage(gfxDraw::WHITE);

  drawTest04();

  saveImage("test04.png");
#endif

  // gfxDraw::rect(10, 50, 12, 8, bmpSet(gfxDraw::BLACK), bmpSet(gfxDraw::CYAN), 2);
  // gfxDraw::rect(10, 60, 12, 8, nullptr, bmpSet(gfxDraw::CYAN), 2);
  // gfxDraw::rect(10, 70, 12, 8, bmpSet(gfxDraw::BLACK), nullptr, 2);

  // gfxDraw::rect(30, 70, 10, 8, bmpSet(gfxDraw::RED), nullptr, 2);
  // gfxDraw::rect(39, 70, -10, 8, bmpSet(gfxDraw::BLACK), nullptr, 2);

  // gfxDraw::rect(42, 70, 10, 8, nullptr, bmpSet(gfxDraw::RED), 2);
  // gfxDraw::rect(51, 70, -10, 8, nullptr, bmpSet(gfxDraw::BLACK), 2);

  // gfxDraw::rect(54, 70, 10, 8, nullptr, bmpSet(gfxDraw::RED), 2);
  // gfxDraw::rect(54, 77, 10, -8, nullptr, bmpSet(gfxDraw::BLACK), 2);


  // drawClock(120, 205, 80);


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

  printf("end.");
}