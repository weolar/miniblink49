// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ListMarkerPainter_h
#define ListMarkerPainter_h

namespace blink {

struct PaintInfo;
class Path;
class LayoutListMarker;
class LayoutPoint;

class ListMarkerPainter {
public:
    ListMarkerPainter(LayoutListMarker& layoutListMarker) : m_layoutListMarker(layoutListMarker) { }

    void paint(const PaintInfo&, const LayoutPoint& paintOffset);

private:
    Path getCanonicalPath() const;
    Path getPath(const LayoutPoint& origin) const;

    LayoutListMarker& m_layoutListMarker;
};

} // namespace blink

#endif // ListMarkerPainter_h
