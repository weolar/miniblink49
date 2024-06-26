// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_PICTURE_LAYER_H_
#define CC_TEST_FAKE_PICTURE_LAYER_H_

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "cc/layers/picture_layer.h"
#include "cc/playback/recording_source.h"

namespace cc {
class FakePictureLayer : public PictureLayer {
public:
    static scoped_refptr<FakePictureLayer> Create(const LayerSettings& settings,
        ContentLayerClient* client)
    {
        return make_scoped_refptr(new FakePictureLayer(settings, client));
    }

    static scoped_refptr<FakePictureLayer> CreateWithRecordingSource(
        const LayerSettings& settings,
        ContentLayerClient* client,
        scoped_ptr<RecordingSource> source)
    {
        return make_scoped_refptr(
            new FakePictureLayer(settings, client, source.Pass()));
    }

    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;

    int update_count() const { return update_count_; }
    void reset_update_count() { update_count_ = 0; }

    size_t push_properties_count() const { return push_properties_count_; }
    void reset_push_properties_count() { push_properties_count_ = 0; }

    void set_always_update_resources(bool always_update_resources)
    {
        always_update_resources_ = always_update_resources;
    }

    bool Update() override;

    void PushPropertiesTo(LayerImpl* layer) override;

private:
    FakePictureLayer(const LayerSettings& settings, ContentLayerClient* client);
    FakePictureLayer(const LayerSettings& settings,
        ContentLayerClient* client,
        scoped_ptr<RecordingSource> source);
    ~FakePictureLayer() override;

    int update_count_;
    size_t push_properties_count_;
    bool always_update_resources_;
};

} // namespace cc

#endif // CC_TEST_FAKE_PICTURE_LAYER_H_
