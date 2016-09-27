// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGFilterPainter_h
#define SVGFilterPainter_h

#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "wtf/OwnPtr.h"

namespace blink {

class FilterData;
class LayoutObject;
class LayoutSVGResourceFilter;

class SVGFilterRecordingContext {
public:
    explicit SVGFilterRecordingContext(GraphicsContext* initialContext) : m_initialContext(initialContext) { }

    GraphicsContext* beginContent(FilterData*);
    void endContent(FilterData*);

    GraphicsContext* paintingContext() const { return m_initialContext; }

private:
    OwnPtr<DisplayItemList> m_displayItemList;
    OwnPtr<GraphicsContext> m_context;
    GraphicsContext* m_initialContext;
};

class SVGFilterPainter {
public:
    SVGFilterPainter(LayoutSVGResourceFilter& filter) : m_filter(filter) { }

    // Returns the context that should be used to paint the filter contents, or
    // null if the content should not be recorded.
    GraphicsContext* prepareEffect(LayoutObject&, SVGFilterRecordingContext&);
    void finishEffect(LayoutObject&, SVGFilterRecordingContext&);

private:
    LayoutSVGResourceFilter& m_filter;
};

} // namespace blink

#endif // SVGFilterPainter_h
