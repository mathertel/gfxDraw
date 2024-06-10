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



}  // gfxDraw namespace

// End.
