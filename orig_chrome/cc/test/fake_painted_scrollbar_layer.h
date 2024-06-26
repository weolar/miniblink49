// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_PAINTED_SCROLLBAR_LAYER_H_
#define CC_TEST_FAKE_PAINTED_SCROLLBAR_LAYER_H_

#include "base/memory/scoped_ptr.h"
#include "cc/layers/painted_scrollbar_layer.h"
#include "cc/test/fake_scrollbar.h"

namespace base {
template <typename T>
class AutoReset;
}

namespace cc {

class FakePaintedScrollbarLayer : public PaintedScrollbarLayer {
public:
    static scoped_refptr<FakePaintedScrollbarLayer> Create(
        const LayerSettings& settings,
        bool paint_during_update,
        bool has_thumb,
        int scrolling_layer_id);
    int update_count() const { return update_count_; }
    void reset_update_count() { update_count_ = 0; }

    bool Update() override;

    void PushPropertiesTo(LayerImpl* layer) override;

    scoped_ptr<base::AutoReset<bool>> IgnoreSetNeedsCommit();

    size_t push_properties_count() const { return push_properties_count_; }
    void reset_push_properties_count() { push_properties_count_ = 0; }

    // For unit tests
    UIResourceId track_resource_id()
    {
        return PaintedScrollbarLayer::track_resource_id();
    }
    UIResourceId thumb_resource_id()
    {
        return PaintedScrollbarLayer::thumb_resource_id();
    }
    FakeScrollbar* fake_scrollbar()
    {
        return fake_scrollbar_;
    }
    using PaintedScrollbarLayer::UpdateInternalContentScale;
    using PaintedScrollbarLayer::UpdateThumbAndTrackGeometry;

private:
    FakePaintedScrollbarLayer(const LayerSettings& settings,
        FakeScrollbar* fake_scrollbar,
        int scrolling_layer_id);
    ~FakePaintedScrollbarLayer() override;

    int update_count_;
    size_t push_properties_count_;
    FakeScrollbar* fake_scrollbar_;
};

} // namespace cc

#endif // CC_TEST_FAKE_PAINTED_SCROLLBAR_LAYER_H_
