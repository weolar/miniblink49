// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ListItemPainter_h
#define ListItemPainter_h

namespace blink {

struct PaintInfo;
class Path;
class LayoutListItem;
class LayoutPoint;

class ListItemPainter {
public:
    ListItemPainter(LayoutListItem& layoutListItem) : m_layoutListItem(layoutListItem) { }

    void paint(const PaintInfo&, const LayoutPoint& paintOffset);

private:
    Path getCanonicalPath() const;
    Path getPath(const LayoutPoint& origin) const;

    LayoutListItem& m_layoutListItem;
};

} // namespace blink

#endif // ListItemPainter_h
