// - - - - -
// GFXDraw - A Arduino library for drawing shapes on a GFX display using paths describing the borders.
// gfxDrawWidget.cpp: Library implementation of a widget base class functionality
// based on vector graphics and paths with transformation, coloring options to simplify drawing with gfxDraw functions.
//
// Copyright (c) 2024-2024 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changelog: See gfxDraw.h and documentation files in this library.
//
// - - - - -

#include "gfxDrawWidget.h"

#define TRACE(...)  // printf(__VA_ARGS__)


namespace gfxDraw {

/// @brief Create segments from a textual path definition.
void gfxDrawWidget::setPath(const char *path) {
  _segments = parsePath(path);
}

/// @brief Create segments that form a with rectangle.
/// @param width width of the rectangle.
/// @param height height of the rectangle.
void gfxDrawWidget::setRect(int16_t width, int16_t height) {
  _segments.push_back(Segment(Segment::Move, 0, 0));
  _segments.push_back(Segment(Segment::Line, 0, height - 1));
  _segments.push_back(Segment(Segment::Line, width - 1, height - 1));
  _segments.push_back(Segment(Segment::Line, width - 1, 0));
  _segments.push_back(Segment(Segment::Close));
}


// ===== Background ====

/// @brief The Background class is used to save the background of the drawn pixels for later restore / undraw.
class Background {
public:
  // initialize without data.
  Background() {
    _w = 0;
  };

  void set(int16_t x, int16_t y, RGBA color) {
    TRACE("bg::set(%d,%d)=%08x\n", x, y, color.raw);
    if (_w == 0) {
      _createData(x, y);
      TRACE(" init to (%d/%d)-(%d/%d)\n", _x, _y, _x + _w - 1, _y + _h - 1);
    }

    if (x < _x) {
      _insertLeft(_x - x);
      TRACE(" resize to (%d/%d)-(%d/%d)\n", _x, _y, _x + _w - 1, _y + _h - 1);

    } else if (x >= _x + _w) {
      _insertRight(x - (_x + _w) + 1);
      TRACE(" resize to (%d/%d)-(%d/%d)\n", _x, _y, _x + _w - 1, _y + _h - 1);
    }

    if (y < _y) {
      // _insertTop(_y - y);
      printf(" resize to %d/%d\n", x, y);
      printf("undone!\n");

    } else if (y >= _y + _h) {
      _insertBottom(y - (_y + _h) + 1);
      TRACE(" resize to (%d/%d)-(%d/%d)\n", _x, _y, _x + _w - 1, _y + _h - 1);
    }

    RGBA *cell = &data[(x - _x) + (y - _y) * _w];
    if (cell->Alpha > 0) {
      printf("double write at (%d,%d)!\n", x, y);
    } else {
      *cell = color;
    }
    // data[(x - _x) + (y - _y) * _w] = color;
    // data.at((x - _x) + (y - _y) * _w) = color;
  }


  void draw(gfxDraw::fDrawPixel cbDraw) {

    // insert blanks on each line
    for (int16_t y = 0; y < _h; y++) {
      for (int16_t x = 0; x < _w; x++) {
        RGBA col = data[(y * _w) + x];
        if (col.Alpha > 0)
          cbDraw(x + _x, y + _y, col);
      }
    }

  }  // draw();



private:
  int16_t _x;
  int16_t _y;
  int16_t _w;
  int16_t _h;
  std::vector<RGBA> data;

  // first initializing the data
  void _createData(int16_t x, int16_t y) {
    // first initialization.
    _x = x & 0xFFF0;
    _y = y;
    _w = 16;
    _h = 16;
    data.resize(16 * 16);  // initialized with 0, all pixels with Alpha 0
  };


  // resize the array based 0/0
  void _insertLeft(uint16_t count) {
    RGBA blank('L', 'L', 'L', 0x00);

    // expand by multiple of 16
    count = (count + 15) & 0xFFF0;
    int16_t x2 = _x - count;
    int16_t w2 = _w + count;

    TRACE(" l-expand %d -> %d\n", _w, w2);
    data.reserve(w2 * _h);  // alloc at once.

    // insert blanks on each line
    for (int16_t l = 0; l < _h; l++) {
      data.insert(data.begin() + (l * w2), count, blank);
    }
    _x = x2;
    _w = w2;
  }  // _insertLeft


  // resize the array based 0/0
  void _insertRight(uint16_t count) {
    RGBA blank('R', 'R', 'R', 0x00);

    // expand by multiple of 16
    count = (count + 15) & 0xFFF0;
    int16_t w2 = _w + count;
    TRACE(" r-expand %d -> %d\n", _w, w2);

    data.reserve(w2 * _h);  // alloc at once.

    // insert blanks on each line
    for (int16_t l = 0; l < _h; l++) {
      data.insert(data.begin() + (l * w2) + _w, count, blank);
    }

    _w = w2;
  }  // _insertRight


  // resize the array based 0/0
  void _insertBottom(uint16_t count) {
    RGBA blank('b', 'b', 'b', 0x00);

    // expand by multiple of 16
    count = (count + 15) & 0xFFF0;
    int16_t h2 = _h + count;
    TRACE(" b-expand %d -> %d\n", _h, h2);

    data.reserve(_w * h2);  // alloc at once.

    data.insert(data.end(), count * _w, blank);
    _h = h2;
  }  // _insertBottom
};  // class Background


// ===== Transformation Matrix Manipulations =====


void gfxDrawWidget::resetTransformation() {
  _initMatrix(_matrix);
}

// apply the scaling factors to the transformation matrix;
void gfxDrawWidget::move(int16_t dx, int16_t dy) {
  if ((dx != 0) || (dy != 0)) {
    Matrix1000 MoveMatrix;
    _initMatrix(MoveMatrix);
    MoveMatrix[0][2] = dx * 1000;
    MoveMatrix[1][2] = dy * 1000;
    _multiplyMatrix(_matrix, MoveMatrix);
  }
};

// apply the scaling factors to the transformation matrix;
void gfxDrawWidget::scale(int16_t scale100) {
  if (scale100 != 100) {
    Matrix1000 scaleMatrix;
    _initMatrix(scaleMatrix);
    scaleMatrix[0][0] = scaleMatrix[1][1] = scale100 * 10;
    _multiplyMatrix(_matrix, scaleMatrix);
  }
};


// apply the rotation factors to the transformation matrix.
// rotating is using the center 0/0 by default
void gfxDrawWidget::rotate(int16_t angle, int16_t cx, int16_t cy) {
  if (angle != 0) {
    Matrix1000 m;

    if ((cx != 0) && (cy != 0)) {
      // move given center to 0/0
      _initMatrix(m);
      m[0][2] = -cx * 1000;
      m[1][2] = -cy * 1000;
      _multiplyMatrix(_matrix, m);
    }

    double radians = (angle * M_PI) / 180;

    int32_t sinFactor1000 = floor(sin(radians) * 1000);
    int32_t cosFactor1000 = floor(cos(radians) * 1000);

    _initMatrix(m);
    m[0][0] = m[1][1] = cosFactor1000;
    m[1][0] = sinFactor1000;
    m[0][1] = -sinFactor1000;

    _multiplyMatrix(_matrix, m);

    if ((cx != 0) && (cy != 0)) {
      // move given center back
      _initMatrix(m);
      m[0][2] = cx * 1000;
      m[1][2] = cy * 1000;
      _multiplyMatrix(_matrix, m);
    }
  }
};


// ===== Drawing =====

void gfxDrawWidget::draw(gfxDraw::fDrawPixel cbDraw, gfxDraw::fReadPixel cbRead) {
  TRACE("draw()\n");

  TRACE(" stroke = %02x.%02x.%02x.%02x\n", _stroke.Alpha, _stroke.Red, _stroke.Green, _stroke.Blue);
  TRACE(" fill   = %02x.%02x.%02x.%02x\n", _fillColor1.Alpha, _fillColor1.Red, _fillColor1.Green, _fillColor1.Blue);

  // create a copy
  std::vector<gfxDraw::Segment> tSegments = _segments;

  // transform with matrix
  transformSegments(tSegments, [&](int16_t &x, int16_t &y) {
    int32_t tx, ty;
    tx = x * _matrix[0][0] + y * _matrix[0][1] + _matrix[0][2];
    ty = x * _matrix[1][0] + y * _matrix[1][1] + _matrix[1][2];

    x = tx / 1000;
    y = ty / 1000;
  });

  // create Background
  if (cbRead) {
    _bg = new Background();
  }

  // draw...
  if (_fillColor1.Alpha == 0) {
    // need to draw the strike pixels only
    gfxDraw::drawSegments(tSegments, [&](int16_t x, int16_t y) {
      if (cbRead) { _bg->set(x, y, cbRead(x, y)); }
      cbDraw(x, y, _stroke);
    });

  } else if ((_stroke.Alpha > 0)) {
    gfxDraw::fillSegments(
      tSegments,
      [&](int16_t x, int16_t y) {
        if (cbRead) { _bg->set(x, y, cbRead(x, y)); }
        cbDraw(x, y, _stroke);
      },
      [&](int16_t x, int16_t y) {
        if (cbRead) { _bg->set(x, y, cbRead(x, y)); }
        cbDraw(x, y, _getColor(x, y));
      });

  } else if (_stroke.Alpha == 0) {
    gfxDraw::fillSegments(
      tSegments,
      nullptr,
      [&](int16_t x, int16_t y) {
        if (cbRead) { _bg->set(x, y, cbRead(x, y)); }
        cbDraw(x, y, _getColor(x, y));
      });
  }
};


void gfxDrawWidget::undraw(gfxDraw::fDrawPixel cbDraw) {
  if (_bg) {
    _bg->draw(cbDraw);
  }
  free(_bg);  //  = new Background();
  _bg = nullptr;
  printf("\n");
}





}  // gfxDraw namespace

// End.
