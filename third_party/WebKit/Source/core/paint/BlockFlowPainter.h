// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BlockFlowPainter_h
#define BlockFlowPainter_h

namespace blink {

class LayoutPoint;
struct PaintInfo;
class LayoutBlockFlow;

class BlockFlowPainter {
public:
    BlockFlowPainter(LayoutBlockFlow& layoutBlockFlow) : m_layoutBlockFlow(layoutBlockFlow) { }
    void paintFloats(const PaintInfo&, const LayoutPoint&, bool preservePhase);
    void paintSelection(const PaintInfo&, const LayoutPoint&);
private:
    LayoutBlockFlow& m_layoutBlockFlow;
};

} // namespace blink

#endif // BlockFlowPainter_h
