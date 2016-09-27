// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RemoteFrameView_h
#define RemoteFrameView_h

#include "platform/Widget.h"
#include "platform/geometry/IntRect.h"
#include "platform/heap/Handle.h"

namespace blink {

class RemoteFrame;

class RemoteFrameView final : public Widget {
public:
    static PassRefPtrWillBeRawPtr<RemoteFrameView> create(RemoteFrame*);

    virtual ~RemoteFrameView();

    virtual bool isRemoteFrameView() const override { return true; }

    RemoteFrame& frame() const
    {
        ASSERT(m_remoteFrame);
        return *m_remoteFrame;
    }

    // Override to notify remote frame that its viewport size has changed.
    virtual void frameRectsChanged() override;

    virtual void invalidateRect(const IntRect&) override;

    virtual void setFrameRect(const IntRect&) override;

    DECLARE_VIRTUAL_TRACE();

private:
    explicit RemoteFrameView(RemoteFrame*);

    // The properties and handling of the cycle between RemoteFrame
    // and its RemoteFrameView corresponds to that between LocalFrame
    // and FrameView. Please see the FrameView::m_frame comment for
    // details.
    RefPtrWillBeMember<RemoteFrame> m_remoteFrame;
};

DEFINE_TYPE_CASTS(RemoteFrameView, Widget, widget, widget->isRemoteFrameView(), widget.isRemoteFrameView());

} // namespace blink

#endif // RemoteFrameView_h
