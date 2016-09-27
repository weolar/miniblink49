// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NinePieceImageGrid_h
#define NinePieceImageGrid_h

#include "core/CoreExport.h"
#include "platform/geometry/FloatRect.h"
#include "platform/geometry/FloatSize.h"
#include "platform/geometry/IntRect.h"
#include "platform/geometry/IntSize.h"
#include "platform/graphics/Image.h"
#include "platform/heap/Heap.h"

namespace blink {

class IntRectOutsets;
class NinePieceImage;

enum NinePiece {
    MinPiece = 0,
    TopLeftPiece = MinPiece,
    BottomLeftPiece,
    LeftPiece,
    TopRightPiece,
    BottomRightPiece,
    RightPiece,
    TopPiece,
    BottomPiece,
    MiddlePiece,
    MaxPiece
};

inline NinePiece& operator++(NinePiece& piece)
{
    piece = static_cast<NinePiece>(static_cast<int>(piece) + 1);
    return piece;
}

// The NinePieceImageGrid class is responsible for computing drawing information for the nine piece image.
//
// http://dev.w3.org/csswg/css-backgrounds-3/#border-image-process
//
// Given an image, a set of slices and a border area:
//
//       |         |
//   +---+---------+---+          +------------------+
//   | 1 |    7    | 4 |          |      border      |
// --+---+---------+---+---       |  +------------+  |
//   |   |         |   |          |  |            |  |
//   | 3 |    9    | 6 |          |  |    css     |  |
//   |   |  image  |   |          |  |    box     |  |
//   |   |         |   |          |  |            |  |
// --+---+---------+---+---       |  |            |  |
//   | 2 |    8    | 5 |          |  +------------+  |
//   +---+---------+---+          |                  |
//       |         |              +------------------+
//
// it generates drawing information for the nine border pieces.
class CORE_EXPORT NinePieceImageGrid {
    STACK_ALLOCATED();

public:
    NinePieceImageGrid(const NinePieceImage&, IntSize imageSize, IntRect borderImageArea,
        const IntRectOutsets& borderWidths);

    struct CORE_EXPORT NinePieceDrawInfo {
        bool isDrawable;
        bool isCornerPiece;
        FloatRect destination;
        FloatRect source;

        // tileScale and tileRule are only useful for non-corners, i.e. edge and center pieces.
        FloatSize tileScale;
        struct {
            Image::TileRule horizontal;
            Image::TileRule vertical;
        } tileRule;
    };
    NinePieceDrawInfo getNinePieceDrawInfo(NinePiece) const;

    struct Edge {
        bool isDrawable() const { return slice > 0 && width > 0; }
        float scale() const { return isDrawable() ? (float)width / slice : 1; }
        int slice;
        int width;
    };

private:
    void setDrawInfoCorner(NinePieceDrawInfo&, NinePiece) const;
    void setDrawInfoEdge(NinePieceDrawInfo&, NinePiece) const;
    void setDrawInfoMiddle(NinePieceDrawInfo&) const;

    IntRect m_borderImageArea;
    IntSize m_imageSize;
    Image::TileRule m_horizontalTileRule;
    Image::TileRule m_verticalTileRule;
    bool m_fill;

    Edge m_top;
    Edge m_right;
    Edge m_bottom;
    Edge m_left;
};

} // namespace blink

#endif // NinePieceImageGrid_h
