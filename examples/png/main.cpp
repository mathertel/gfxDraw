// test and develop shell for using gfxDraw functions on windows producing PNG files for testing.


#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include "gfxDraw.h"
#include "gfxDrawWidget.h"
#include "gfxDrawGaugeWidget.h"

#include "lodepng.h"

#define TRACE(...)  // GFXD_TRACE(__VA_ARGS__)


// https://doc.magnum.graphics/magnum/

using namespace std;
using namespace gfxDraw;

// ===== image memory allocation and functions

// The image memory is "just" an 2-dimensional array of bytes (4 * width * height)
// each pixel is using 4 bytes for (ARGB)

std::vector<unsigned char> image;
uint16_t imageWidth;
uint16_t imageHeight;

void newImage(uint16_t w, uint16_t h) {
  imageWidth = w;
  imageHeight = h;
  image.resize(w * h * 4);
}

void saveImage(const char *fileName) {
  std::vector<unsigned char> png;
  unsigned error = lodepng::encode(png, image, imageWidth, imageHeight);
  if (!error) lodepng::save_file(png, fileName);

  // if there's an error, display it
  if (error) {
    printf("encoder error %d: %s\n", error, lodepng_error_text(error));
  }
}


// Draw a pixel using a color.
void setImagePixel(int16_t x, int16_t y, gfxDraw::ARGB color) {
  if ((x >= 0) && (x < imageWidth) && (y >= 0) && (y < imageHeight)) {
    image[4 * imageWidth * y + 4 * x + 0] = color.Red;
    image[4 * imageWidth * y + 4 * x + 1] = color.Green;
    image[4 * imageWidth * y + 4 * x + 2] = color.Blue;
    image[4 * imageWidth * y + 4 * x + 3] = 255;
  }
};

gfxDraw::ARGB readImagePixel(int16_t x, int16_t y) {
  gfxDraw::ARGB col;
  if ((x >= 0) && (x < imageWidth) && (y >= 0) && (y < imageHeight)) {
    col.Red = image[4 * imageWidth * y + 4 * x + 0];
    col.Green = image[4 * imageWidth * y + 4 * x + 1];
    col.Blue = image[4 * imageWidth * y + 4 * x + 2];
    col.Alpha = 255;
  }
  return (col);
}

void fillImage(gfxDraw::ARGB color) {
  for (unsigned y = 0; y < imageHeight; y++) {
    for (unsigned x = 0; x < imageWidth; x++) {
      setImagePixel(x, y, color);
    }
  }
}


// helpful "inline" function to draw to the png image with a fixed color
#define pngSetPixel(col) [](int16_t x, int16_t y) { \
  setImagePixel(x, y, (col)); \
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

// A SVG path showing a sword
const char *SwordPath = R"==(
M 40 80 L 100 10 L 130 0 L 120 30 L 50 90 C 60 100 60 110 70 100 C 70 110 80 120 70 120 A 14 14 0 0 1 60 130 A 50 50 0 0 0 40 100 C 36 99 36 99 35 105 l -15 13 C 10 121 10 121 12 110 L 25 95 C 31 94 31 94 30 90 A 50 50 90 0 0 0 70 A 14 14 0 0 1 10 60 C 10 50 20 60 30 60 C 20 70 30 70 40 80 M 100 10 L 100 30 L 120 30 L 102 28 L 100 10z
)==";

// A SVG path defining the shape of a heard
const char *heardPath = "M43 7 a1 1 0 00-36 36l36 36 36-36a1 1 0 00-36-36z";

// A SVG path defining the shape of an arrow
const char *arrowPath = "M0 0l12-12 0 8 22 0 0 8-22 0 0 8Z";

const char *keyPath =
  "M177 10l-87 128 1 1 16 11 88-128L177 10z"
  "m45 31c-10 15-22 12-34 4l-24 36c11 8 18 18 9 31l21 15 14-21-7-5c-2 1-3 1-5 1-6 0-11-5-11-11 0-6 5-11 11-11s11 5 11 11c0 0 0 0 0 0l8 5 9-13-19-12 6-9 7 5 5-8 12 8 9-12-21-15z"
  "M70 136c0 0 0 0 0 0 0 0-1 0-1 0-2 1-4 2-5 4-1 2-2 4-2 6 0 2 1 3 3 5l38 27c2 1 3 1 5 1 2-1 4-2 5-4 1-2 2-4 2-6 0-2 0-3-2-4L73 137c-1-1-2-1-3-1z"
  "M41 158c-12 0-23 5-29 14-8 12-6 30 5 45-1 2-1 5-1 8 0 12 10 21 21 21 4 0 8-1 12-4 22 9 44 6 54-8 9-14 5-33-8-50l-32-22c-7-3-14-4-21-4 0 0-1 0-1 0z"
  "m-4 55c7 0 12 5 12 12 0 7-5 12-12 12-7 0-12-5-12-12s5-12 12-12z";

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
void assert(bool b, const char *text) {
  if (!b) {
    printf("ALERT: %s\n", text);
  }
}

// draw the test paths for outTest.bmp
void silentTests() {
  uint16_t count;
  TRACE("\nSilent Tests:\n");

  if (sizeof(gfxDraw::ARGB) != 4) {
    printf("error: size(ARGB) is not 4!\n");
  }

  if (sizeof(gfxDraw::Segment) != 14) {
    printf("error: size(Segment) is not 14!\n");
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
    moveSegments(vSeg, x, y);
    fillSegments(vSeg, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB(255, 255, 128)));
  };

  TRACE("\nTest-01:\n");
  // 8*8 rectange needs 2 edges on horizontal lines
  gfxDraw::drawRect(10, 10, 10, 10, nullptr, pngSetPixel(gfxDraw::ARGB_SILVER));
  drawHelper("M1 1 h7 v7 h-7 z", 10, 10);

  TRACE("\nTest-02:\n");
  // 7*8 rectange needs 2 more points on horizontal lines
  gfxDraw::drawRect(20, 10, 10, 10, nullptr, pngSetPixel(gfxDraw::ARGB_GRAY));
  drawHelper("M1 1 h6 v7 h-6 z", 20, 10);

  TRACE("\nTest-03:\n");
  // closed W
  gfxDraw::drawRect(30, 10, 11, 10, nullptr, pngSetPixel(gfxDraw::ARGB_SILVER));
  drawHelper("M1 1 h8 v7 l-4,-4 l-4 4 z", 30, 10);

  TRACE("\nTest-04: closed M\n");
  gfxDraw::drawRect(41, 10, 11, 10, nullptr, pngSetPixel(gfxDraw::ARGB_GRAY));
  drawHelper("M1 1 l4,4 l4 -4 v7 h-8 z", 41, 10);

  TRACE("\nTest-05: route\n");
  gfxDraw::drawRect(52, 10, 11, 10, nullptr, pngSetPixel(gfxDraw::ARGB_SILVER));
  drawHelper("M5 1 l4,4 l-4 4 l-4 -4 z", 52, 10);

  TRACE("\nTest-06: |> symbol\n");
  gfxDraw::drawRect(63, 10, 10, 10, nullptr, pngSetPixel(gfxDraw::ARGB_GRAY));
  drawHelper("M1 1 l7,4 l-7 4 z", 63, 10);

  TRACE("\nTest-07: <| symbol\n");
  gfxDraw::drawRect(73, 10, 10, 10, nullptr, pngSetPixel(gfxDraw::ARGB_SILVER));
  drawHelper("M8 1 l-7,4 l7 4 z", 73, 10);

  TRACE("\nTest-10: [[]] \n");
  gfxDraw::drawRect(10, 22, 10, 10, nullptr, pngSetPixel(gfxDraw::ARGB_SILVER));
  drawHelper("M1 1 h7 v7 h-7 z M3 3 h3 v3 h-3 z", 10, 22);

  TRACE("\nTest-11: [*] \n");
  gfxDraw::drawRect(20, 22, 10, 10, nullptr, pngSetPixel(gfxDraw::ARGB_GRAY));
  drawHelper("M1 1 h7 v7 h-7 z M4 4 h1 v1 h-1 z", 20, 22);

  gfxDraw::drawRect(10, 34, 27, 24, nullptr, pngSetPixel(gfxDraw::ARGB_SILVER));
  gfxDraw::pathByText("M2 52 l60-50 v20 h20 l-30 50z", 10, 34, 30,
                      pngSetPixel(gfxDraw::ARGB_BLUE),
                      [](int16_t x, int16_t y) {
                        gfxDraw::ARGB col = gfxDraw::ARGB(0xC0, 0x0, 0, 0xFF);
                        col.Green += (7 * x);
                        col.Blue += (7 * y);
                        setImagePixel(x, y, col);
                      });


  gfxDraw::drawRect(40, 34, 43, 26, nullptr, pngSetPixel(gfxDraw::ARGB_SILVER));
  gfxDraw::pathByText(
    "M2 42 l20-33 l20 38 l20 -38 l20 33 l0 -40 l-70 0 z",
    40, 34, 50,
    pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_YELLOW));

  // radio with 2 inner paths
  gfxDraw::drawRect(91, 34, 43, 35, nullptr, pngSetPixel(gfxDraw::ARGB_SILVER));
  gfxDraw::pathByText(RadioPath, 90, 34, 50, pngSetPixel(gfxDraw::ARGB_BLACK), pngSetPixel(gfxDraw::ARGB_YELLOW));

  // smiley with bezier curves
  gfxDraw::drawRect(10, 80, 99, 99, nullptr, pngSetPixel(gfxDraw::ARGB_GREEN));
  gfxDraw::pathByText(SmilieCurvePath, 11, 81, 200, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_YELLOW));

  // smiley with Arcs
  gfxDraw::drawRect(120, 80, 99, 99, nullptr, pngSetPixel(gfxDraw::ARGB_GREEN));
  gfxDraw::pathByText(SmileyArcPath, 120, 81, 200, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_YELLOW));

  // gfxDraw::drawCubicBezier(5, 10, 10, -5, 20, 25, 26, 10,
  //                          [](int16_t x, int16_t y) {
  //                            setImagePixel(x + 110, y + 80, gfxDraw::RED);
  //                          });

}  // drawTest01()


// draw the test paths for test02.png
void drawTest02() {
  std::vector<gfxDraw::Segment> segs = gfxDraw::parsePath("M1 1 h7 v7 h-7 z M3 3 h3 v3 h-3 z");
  gfxDraw::scaleSegments(segs, 400);                                                                      // scale by 400%
  gfxDraw::moveSegments(segs, 100, 0);                                                                    // move to the right
  gfxDraw::fillSegments(segs, pngSetPixel(gfxDraw::ARGB_BLACK), pngSetPixel(gfxDraw::ARGB(0xEEEEEEFF)));  // hard-coded fill color here.

  gfxDraw::moveSegments(segs, 22, 15);                                                                    // move right down
  gfxDraw::fillSegments(segs, pngSetPixel(gfxDraw::ARGB_BLACK), pngSetPixel(gfxDraw::ARGB(0xEEEEEEFF)));  // hard-coded fill color here.

  segs = gfxDraw::parsePath("M1 1 h7 v7 h-7 z M3 3 h3 v3 h-3 z");
  gfxDraw::scaleSegments(segs, 400);                                                                      // scale by 400%
  gfxDraw::rotateSegments(segs, 25);                                                                      // rotate some degrees
  gfxDraw::moveSegments(segs, 200, 40);                                                                   // move right down
  gfxDraw::fillSegments(segs, pngSetPixel(gfxDraw::ARGB_BLACK), pngSetPixel(gfxDraw::ARGB(0xEEEEEEFF)));  // hard-coded fill color here.


  segs = gfxDraw::parsePath(SmilieCurvePath);
  gfxDraw::scaleSegments(segs, 200);
  gfxDraw::rotateSegments(segs, 25);                                                               // rotate some degrees
  gfxDraw::moveSegments(segs, 40, 40);                                                             // move right down
  gfxDraw::fillSegments(segs, pngSetPixel(gfxDraw::ARGB_BLACK), pngSetPixel(gfxDraw::ARGB_LIME));  // hard-coded fill color here.

  gfxDraw::gfxDrawWidget *rect = new gfxDraw::gfxDrawWidget(gfxDraw::ARGB_BLUE, gfxDraw::ARGB_YELLOW);
  rect->setRect(32, 16);

  // rotate around center
  rect->move(-16, -8);
  rect->rotate(30);
  rect->move(16, 8);

  rect->scale(200);
  rect->move(20, 20);

  rect->draw(setImagePixel);

  // // rect2->setFillGradient(gfxDraw::ARGB_YELLOW, 10, 0, gfxDraw::ARGB_ORANGE, 36, 0);
  // rect2->setFillGradient(gfxDraw::ARGB_YELLOW, 0, 2, gfxDraw::ARGB_BLUE, 0, 22);
  // rect2->draw(4, 240, setImagePixel);

  // gfxDraw::rect(4, 200, 47, 31, nullptr, pngSetPixel(gfxDraw::SILVER));
  // gfxDraw::pathByText("M0 12 l24-12 l20 20 h-16 v8 h-12 z", 5, 201, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_YELLOW));

  // gfxDrawWidget *rect1 = new gfxDrawWidget(gfxDraw::TRANSPARENT, gfxDraw::SILVER);
  // rect1->setRect(46, 30);
  // rect1->draw(4, 200, setImagePixel);

  // gfxDrawWidget *draw1 = new gfxDrawWidget(gfxDraw::ARGB_BLUE, gfxDraw::ARGB_ORANGE);
  // draw1->setPath("M0 12 l24-12 l20 20 h-16 v8 h-12 z");
  // draw1->draw(5, 201, setImagePixel);


#if 0

  // gfxDraw::drawCubicBezier(80, 200, 80, 240, 150, 240, 150, 200, pngSetPixel(gfxDraw::ARGB_BLACK));

  // da->rect(180, 200, 84, 74, nullptr, pngSetPixel(gfxDraw::ARGB_SILVER), 2);
  // gfxDraw::pathByText("M80 200 C 80,240 150,240 150,200 z", 0,0, pngSetPixel(gfxDraw::ARGB_BLACK), pngSetPixel(gfxDraw::ARGB_YELLOW));
#endif


  //  C 6,23 4,26 4,30 v 48 c 0,4 3,8 8,8 h 64 c 4,0 8,-3 8,-8 V 30 c 0,-4 -3,-8 -8,-8 H 29 L 62,8 59,2 Z",
  // M 8.94,22.6 C 6.04,23.74 4,26.66 4,30 v 48 c 0,4.42 3.58,8 8,8 h 64 c 4.42,0 8,-3.58 8,-8 V 30 c 0,-4.42 -3.58,-8 -8,-8 H 29.22 L 62.28,8.66 59.52,2 Z M 24,78 c -6.62,0 -12,-5.38 -12,-12 0,-6.62 5.38,-12 12,-12 6.62,0 12,5.38 12,12 0,6.62 -5.38,12 -12,12 z M 76,46 H 12 V 30 h 64 z
};


void drawTest03() {

  // gfxDraw::pathByText("M24 0c-14 0-24 10-24 24 c0 14 10 24 24 24 c14 0 24-10 24-24 c0-14-10-24-24-24Z",
  // gfxDraw::pathByText("M24 0c-14 0-24 10-24 24  0 14 10 24 24 24  14 0 24-10 24-24  0-14-10-24-24-24Z", 11, 81, 200, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_YELLOW));

  // Simple Arc drawing test cases
  // gfxDraw::pathByText("M2 2 A 10 30 0 0 0 20 10 Z", 2, 2, 200, pngSetPixel(gfxDraw::ARGB_BLUE), nullptr);
  // gfxDraw::pathByText("M112 102 A 12 12 0 0 0 128 118 ", 2, 2, 100, pngSetPixel(gfxDraw::ARGB_BLUE), nullptr);

  // gfxDraw::pathByText("M30 112 A 100 100 0 0 1 180 220 Z", 2, 2, 10, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_YELLOW));

  // gfxDraw::drawRect(210, 10, 99, 99, nullptr, pngSetPixel(gfxDraw::ARGB_GREEN));
  // gfxDraw::pathByText(SmilieCurvePath, 211, 11, 200, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_YELLOW));

  // gfxDraw::drawRect(210, 120, 99, 99, nullptr, pngSetPixel(gfxDraw::ARGB_GREEN));

  gfxDraw::pathByText(PiePath, 8, 8, 30, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_YELLOW));

  gfxDraw::pathByText(SwordPath, 10, 160, 100, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_YELLOW));

  gfxDraw::pathByText(SmileyArcPath, 200, 100, 100, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_YELLOW));
  gfxDraw::pathByText(SmileyArcPath, 140, 10, 200, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_GREEN));
  gfxDraw::pathByText(SmileyArcPath, 220, 10, 50, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_YELLOW));
  gfxDraw::pathByText(SmileyArcPath, 100, 80, 150, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_YELLOW));

  gfxDraw::drawRoundedRect(260, 10, 120, 50, 22, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB(0xFF, 0xEE, 0x88)));

  gfxDraw::pathByText("O 20 20 20 O 20 20 10", 10, 120, 100, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB(0xFF, 0xFF, 0)));
}


using namespace gfxDraw;

// draw a heard by using the gfxDrawWidget functionality
// demonstrate on how to undraw
void drawTest04_Widget() {
  // gfxDraw::pathByText(heardPath, 8, 8, 100, pngSetPixel(gfxDraw::ARGB_BLUE), pngSetPixel(gfxDraw::ARGB_YELLOW));

  // setup the widget for drawing a heard
  gfxDrawWidget widget;
  widget.setStrokeColor(ARGB_BLACK);
  widget.setFillColor(ARGB_RED);
  widget.setPath(heardPath);

  // draw a background
  drawRect(10, 10, 87, 80, nullptr, pngSetPixel(ARGB_SILVER));

  // simple drawing on the rectangle behind
  widget.move(10, 10);
  widget.draw(setImagePixel);

  // draw a rotated heard on top
  widget.rotate(40, 44 + 10, 40 + 10);  // rotate on current center
  widget.draw(setImagePixel);

  // simple drawing on the rectangle behind and complete undraw.
  // and save of background colors
  drawRect(110, 10, 87, 80, nullptr, pngSetPixel(ARGB_SILVER));
  widget.resetTransformation();
  widget.move(110, 10);
  widget.draw(setImagePixel, readImagePixel);
  widget.undraw(setImagePixel);

  // draw and draw after transformations
  // with removing not overdrawn pixels.
  drawRect(210, 10, 87, 80, nullptr, pngSetPixel(ARGB_SILVER));
  widget.resetTransformation();
  widget.move(210, 10);
  widget.draw(setImagePixel, readImagePixel);

  saveImage("test04.png");

  // rotate heard on top
  widget.rotate(45, 44 + 210, 40 + 10);  // rotate on current center
  widget.draw(setImagePixel, readImagePixel);
  saveImage("test04.png");

  widget.rotate(-45, 44 + 210, 40 + 10);  // rotate on current center
  widget.draw(setImagePixel, readImagePixel);
  saveImage("test04.png");

  // non solid version of the same
  drawRect(310, 10, 87, 80, nullptr, pngSetPixel(ARGB_SILVER));
  widget.setFillColor(ARGB_TRANSPARENT);
  widget.resetBackground();
  widget.resetTransformation();
  widget.move(310, 10);
  widget.draw(setImagePixel, readImagePixel);

  saveImage("test04.png");

  // rotate heard on top
  widget.rotate(45, 44 + 310, 40 + 10);  // rotate on current center
  widget.draw(setImagePixel, readImagePixel);
  saveImage("test04.png");

  widget.rotate(-45, 44 + 310, 40 + 10);  // rotate on current center
  widget.draw(setImagePixel, readImagePixel);
  saveImage("test04.png");
}

// gfxDraw::drawCircle(gfxDraw::Point(20, 20), 12,
//                     pngSetPixel(gfxDraw::ARGB_GREEN),
//                     pngSetPixel(gfxDraw::ARGB_YELLOW));

// draw a heard by using the gfxDrawWidget functionality
// demonstrate on how to undraw
void drawTest05_Gauge() {

  newImage(400, 220);
  fillImage(gfxDraw::ARGB_WHITE);

  gfxDrawGaugeWidget::GFXDrawGaugeConfig conf;
  gfxDraw::gfxDrawGaugeWidget *g;

  conf = {
    .x = 10,
    .y = 10,
    .w = 180,

    .minValue = 0,
    .maxValue = 100,
    .minAngle = 30,
    .maxAngle = 360 - 30
  };

  // draw a background to check drawing area
  drawRect(conf.x, conf.y, conf.w, conf.w, nullptr, pngSetPixel(gfxDraw::ARGB_SILVER));

  g = new gfxDraw::gfxDrawGaugeWidget();
  g->setConfig(&conf);
  g->setValue(70);

  // drawing using stroke and fill
  g->draw(pngSetPixel(gfxDraw::ARGB_BLACK), pngSetPixel(gfxDraw::ARGB_GRAY));


  conf = {
    .x = 200,
    .y = 10,
    .w = 200,

    .minValue = 0,
    .maxValue = 100,
    .minAngle = 30,
    .maxAngle = 360 - 30
  };

  // draw a background to check drawing area
  drawRect(conf.x, conf.y, conf.w, conf.w, nullptr, pngSetPixel(gfxDraw::ARGB_SILVER));

  g = new gfxDraw::gfxDrawGaugeWidget();
  g->setConfig(&conf);
  g->setColors(ARGB_BLUE, ARGB_YELLOW);
  g->addSegment(0, 16, 0xff8080ff);
  g->addSegment(16, 24, 0xff60ff60);
  g->addSegment(45, 75, 0xffff8080);
  g->addSegment(75, 100, 0xffff0000);
  g->setValue(32);

  // drawing using colorful drawing
  g->draw(setImagePixel);

  saveImage("test05.png");
}


uint16_t conv2d(const char *p) {
  return (10 * (*p - '0')) + (*(p + 1) - '0');
}


// 5 min marker : small V
// const char *mark05Path = "M-6 -259 l6 10 l6 -10 z";
const char *mark05Path = "M0 -252 l8 -8 l-8 -8 l-8 8 z";
const char *mark15Path = "M-6 -259 v16 h12 v-16 z";
// const char *hhPath = "M-8 16 v-132 h16 v132z";
const char *hhPath = "M-8 16 v-132 a 1 1 0 0 1 16 0 v132z";
// const char *mmPath = "M-2 32 v-220 h4 v220z";
const char *mmPath = "M-4 32 v-180 a 15,15 0 0 1 0 -30 v-20 h8 v20 a 15,15 0 0 1 0 30  v180z";
const char *ssPath = "M0 48 v-286";

void drawClock(int16_t hh, int16_t mm, int16_t ss, bool redraw = false) {
  printf("drawClock(%02d:%02d:%02d)\n", hh, mm, ss);

  static int16_t hhDegLast = -1;
  static int16_t mmDegLast = -1;

  static gfxDrawWidget handSS;  // static to preserve background

  // time of Day in 0... 12*60*60
  int32_t tod = ((hh % 12) * 60 * 60) + (mm * 60) + ss;
  int16_t hhDeg = tod * 360 / (12 * 60 * 60);
  int16_t mmDeg = (tod % (60 * 60)) * 360 / (60 * 60);
  int16_t ssDeg = (tod % 60) * 360 / (60);

  printf("%3d %3d %3d\n", hhDeg, mmDeg, ssDeg);

  uint32_t _cx = 128;
  uint32_t _cy = 128;
  uint32_t _radius = 120;

  if (hhDeg != hhDegLast) redraw = true;
  if (mmDeg != mmDegLast) redraw = true;

  if (redraw) {
    gfxDrawWidget mark05, mark15;
    mark05.setPath(mark05Path);
    mark05.setStrokeColor(ARGB_TRANSPARENT);
    mark05.setFillColor(ARGB_BLACK);

    mark15.setPath(mark15Path);
    mark15.setStrokeColor(ARGB_TRANSPARENT);
    mark15.setFillColor(ARGB_BLACK);

    // draw a background
    drawRect(_cx - _radius, _cy - _radius, 2 * _radius, 2 * _radius, nullptr, pngSetPixel(ARGB_SILVER));
    setImagePixel(_cx, _cy, gfxDraw::ARGB_BLACK);

    saveImage("test05.png");

    // draw the face of the clock
    for (uint16_t i = 0; i < 360; i += 6) {

      if (i % 90 == 0) {
        mark15.resetTransformation();
        mark15.scale(_radius * 100 / 256);
        mark15.rotate(i);
        mark15.move(_cx, _cy);
        mark15.draw(setImagePixel);

      } else if (i % 30 == 0) {
        mark05.resetTransformation();
        mark05.scale(_radius * 100 / 256);
        mark05.rotate(i);
        mark05.move(_cx, _cy);
        mark05.draw(setImagePixel);

      } else {
        int32_t dx = sin256(i) * _radius;
        int32_t dy = -cos256(i) * _radius;
        setImagePixel(_cx + SCALE256(dx), _cy + SCALE256(dy), gfxDraw::ARGB_BLACK);
      }
    }

    saveImage("test05.png");
  }

  // Hand for hours
  if ((redraw) || (hhDeg != hhDegLast)) {
    gfxDrawWidget hand;

    hand.setPath(hhPath);
    hand.setStrokeColor(ARGB_BLACK);
    hand.setFillColor(ARGB_BLACK);

    // hand.resetTransformation();
    hand.scale(_radius * 100 / 256);
    hand.rotate(hhDeg);
    hand.move(_cx, _cy);
    hand.draw(setImagePixel);
    hhDegLast = hhDeg;
  }

  // Hand for Minutes
  if ((redraw) || (mmDeg != mmDegLast)) {
    gfxDrawWidget hand;
    hand.setPath(mmPath);
    hand.setStrokeColor(ARGB_TRANSPARENT);
    hand.setFillColor(ARGB_BLUE);
    // hand.resetTransformation();
    hand.scale(_radius * 100 / 256);
    hand.rotate(mmDeg);
    hand.move(_cx, _cy);
    hand.draw(setImagePixel);
    mmDegLast = mmDeg;
  }

  // Hand for Seconds
  handSS.setPath(ssPath);
  handSS.setStrokeColor(ARGB_RED);
  handSS.setFillColor(ARGB_TRANSPARENT);
  handSS.resetTransformation();
  handSS.scale(_radius * 100 / 256);
  handSS.rotate(ssDeg);
  handSS.move(_cx, _cy);
  handSS.draw(setImagePixel, readImagePixel);

  saveImage("test05.png");
}

int main() {
  silentTests();

#if (0)
  newImage(400, 300);
  fillImage(gfxDraw::ARGB_WHITE);
  drawTest01();
  saveImage("test01.png");
#endif

#if (0)
  newImage(400, 300);
  fillImage(gfxDraw::ARGB_WHITE);

  drawTest02();

  saveImage("test02.png");
#endif


#if (0)
  newImage(400, 300);
  fillImage(gfxDraw::ARGB_WHITE);

  drawTest03();

  saveImage("test03.png");
#endif

#if (0)
  newImage(400, 300);
  fillImage(gfxDraw::ARGB_WHITE);

  // test using test04.png
  drawTest04_Widget();
#endif

#if (1)
  drawTest05_Gauge();
#endif

#if (0)
  int16_t hh = conv2d(__TIME__);
  int16_t mm = conv2d(__TIME__ + 3);
  int16_t ss = conv2d(__TIME__ + 6);

  newImage(280, 280);
  fillImage(gfxDraw::ARGB_WHITE);

  drawClock(hh, mm, ss);
  drawClock(hh, mm, ss + 1);

#endif


  // gfxDraw::rect(10, 50, 12, 8, pngSetPixel(gfxDraw::ARGB_BLACK), pngSetPixel(gfxDraw::CYAN), 2);
  // gfxDraw::rect(10, 60, 12, 8, nullptr, pngSetPixel(gfxDraw::CYAN), 2);
  // gfxDraw::rect(10, 70, 12, 8, pngSetPixel(gfxDraw::ARGB_BLACK), nullptr, 2);

  // gfxDraw::rect(30, 70, 10, 8, pngSetPixel(gfxDraw::RED), nullptr, 2);
  // gfxDraw::rect(39, 70, -10, 8, pngSetPixel(gfxDraw::ARGB_BLACK), nullptr, 2);

  // gfxDraw::rect(42, 70, 10, 8, nullptr, pngSetPixel(gfxDraw::RED), 2);
  // gfxDraw::rect(51, 70, -10, 8, nullptr, pngSetPixel(gfxDraw::ARGB_BLACK), 2);

  // gfxDraw::rect(54, 70, 10, 8, nullptr, pngSetPixel(gfxDraw::RED), 2);
  // gfxDraw::rect(54, 77, 10, -8, nullptr, pngSetPixel(gfxDraw::ARGB_BLACK), 2);

  // drawDigits14(1, 0, 10, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));
  // drawDigits14(2, 80, 10, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));
  // drawDigits14(5, 160, 10, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));
  // drawDigits14(9, 250, 10, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));

  // drawDigits7(0, 0, 130, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));
  // drawDigits7(1, 34, 130, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));
  // drawDigits7(2, 68, 130, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));
  // drawDigits7(3, 102, 130, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));
  // drawDigits7(4, 136, 130, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));
  // drawDigits7(5, 170, 130, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));
  // drawDigits7(6, 204, 130, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));
  // drawDigits7(7, 238, 130, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));
  // drawDigits7(8, 282, 130, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));
  // drawDigits7(9, 316, 130, pngSetPixel(gfxDraw::RED), pngSetPixel(gfxDraw::ARGB_YELLOW));

  printf("end.");
}
