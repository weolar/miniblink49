// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_INPUT_LAYER_SCROLL_OFFSET_DELEGATE_H_
#define CC_INPUT_LAYER_SCROLL_OFFSET_DELEGATE_H_

#include "base/basictypes.h"
#include "base/callback_forward.h"
#include "base/time/time.h"
#include "ui/gfx/geometry/scroll_offset.h"
#include "ui/gfx/geometry/size_f.h"

namespace cc {

// The LayerScrollOffsetDelegate allows for the embedder to take ownership of
// the scroll offset of the root layer.
//
// The LayerScrollOffsetDelegate is only used on the impl thread.
class LayerScrollOffsetDelegate {
public:
    // This is called by the compositor to query the current scroll offset of the
    // layer.
    // There is no requirement that the return values of this method are
    // stable in time (two subsequent calls may yield different results).
    // The return value is not required to be related to the values passed in to
    // the SetTotalScrollOffset method in any way however it is required to be no
    // more than the value passed to the most recent SetMaxScrollOffset call.
    virtual gfx::ScrollOffset GetTotalScrollOffset() = 0;

    // This is called by the compositor to notify the delegate of any change to
    // the following parameters:
    // |total_scroll_offset| current scroll offset of the root layer,
    // |max_scroll_offset| total scroll offset upper bound for the root layer,
    // |scrollable_size| root layer scrollable size,
    // |page_scale_factor| current page scale,
    // |min_page_scale_factor| page scale lower limit,
    // |max_page_scale_factor| page scale upper limit.
    virtual void UpdateRootLayerState(
        const gfx::ScrollOffset& total_scroll_offset,
        const gfx::ScrollOffset& max_scroll_offset,
        const gfx::SizeF& scrollable_size,
        float page_scale_factor,
        float min_page_scale_factor,
        float max_page_scale_factor)
        = 0;

    // This is called by the compositor to check whether a delegate-managed fling
    // is active or not.
    // TODO(hush): Remove after WebView's smooth scrolling path is unified with
    // Chrome's.
    virtual bool IsExternalScrollActive() const = 0;

    // This is called by the compositor when a fling hitting the root layer
    // requires a scheduled animation update.
    typedef base::Callback<void(base::TimeTicks)> AnimationCallback;
    virtual void SetNeedsAnimate(const AnimationCallback& animation) = 0;

protected:
    LayerScrollOffsetDelegate() { }
    virtual ~LayerScrollOffsetDelegate() { }

private:
    DISALLOW_COPY_AND_ASSIGN(LayerScrollOffsetDelegate);
};

} // namespace cc

#endif // CC_INPUT_LAYER_SCROLL_OFFSET_DELEGATE_H_
