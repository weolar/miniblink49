// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LayoutPagedFlowThread_h
#define LayoutPagedFlowThread_h

#include "core/layout/LayoutMultiColumnFlowThread.h"

namespace blink {

// A flow thread for paged overflow. FIXME: The current implementation relies on the multicol
// implementation, but it in the long run it would be better to have what's common between
// LayoutMultiColumnFlowThread and LayoutPagedFlowThread in LayoutFlowThread, and have both of them
// inherit from that one.
class LayoutPagedFlowThread : public LayoutMultiColumnFlowThread {
public:
    static LayoutPagedFlowThread* createAnonymous(Document&, const ComputedStyle& parentStyle);

    LayoutBlockFlow* pagedBlockFlow() const { return toLayoutBlockFlow(parent()); }

    // Return the number of pages. Will never be less than 1.
    int pageCount();

    virtual bool isLayoutPagedFlowThread() const override { return true; }
    virtual const char* name() const override { return "LayoutPagedFlowThread"; }
    virtual bool needsNewWidth() const override;
    virtual void updateLogicalWidth() override;
    virtual void layout();

private:
    virtual bool descendantIsValidColumnSpanner(LayoutObject* /*descendant*/) const override { return false; }
};

} // namespace blink

#endif // LayoutPagedFlowThread_h
