// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_LAYER_TREE_PIXEL_TEST_H_
#define CC_TEST_LAYER_TREE_PIXEL_TEST_H_

#include <vector>

#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "cc/resources/single_release_callback.h"
#include "cc/test/layer_tree_test.h"
#include "ui/gl/gl_implementation.h"

class SkBitmap;

namespace gpu {
class GLInProcessContext;
}

namespace cc {
class CopyOutputRequest;
class CopyOutputResult;
class LayerTreeHost;
class PixelComparator;
class SolidColorLayer;
class TextureLayer;
class TextureMailbox;

class LayerTreePixelTest : public LayerTreeTest {
public:
    enum PixelTestType {
        PIXEL_TEST_GL,
        PIXEL_TEST_SOFTWARE,
    };

protected:
    LayerTreePixelTest();
    ~LayerTreePixelTest() override;

    scoped_ptr<OutputSurface> CreateOutputSurface() override;
    void WillCommitCompleteOnThread(LayerTreeHostImpl* impl) override;

    virtual scoped_ptr<CopyOutputRequest> CreateCopyOutputRequest();

    void ReadbackResult(scoped_ptr<CopyOutputResult> result);

    void BeginTest() override;
    void SetupTree() override;
    void AfterTest() override;
    void EndTest() override;

    void TryEndTest();

    scoped_refptr<SolidColorLayer> CreateSolidColorLayer(const gfx::Rect& rect,
        SkColor color);
    scoped_refptr<SolidColorLayer> CreateSolidColorLayerWithBorder(
        const gfx::Rect& rect,
        SkColor color,
        int border_width,
        SkColor border_color);
    scoped_refptr<TextureLayer> CreateTextureLayer(const gfx::Rect& rect,
        const SkBitmap& bitmap);

    void RunPixelTest(PixelTestType type,
        scoped_refptr<Layer> content_root,
        base::FilePath file_name);

    void RunSingleThreadedPixelTest(PixelTestType test_type,
        scoped_refptr<Layer> content_root,
        base::FilePath file_name);

    void RunPixelTestWithReadbackTarget(PixelTestType type,
        scoped_refptr<Layer> content_root,
        Layer* target,
        base::FilePath file_name);

    scoped_ptr<SkBitmap> CopyTextureMailboxToBitmap(
        const gfx::Size& size,
        const TextureMailbox& texture_mailbox);

    void CopyBitmapToTextureMailboxAsTexture(
        const SkBitmap& bitmap,
        TextureMailbox* texture_mailbox,
        scoped_ptr<SingleReleaseCallback>* release_callback);

    void ReleaseTextureMailbox(scoped_ptr<gpu::GLInProcessContext> context,
        uint32 texture,
        uint32 sync_point,
        bool lost_resource);

    void Finish();

    void set_enlarge_texture_amount(const gfx::Vector2d& enlarge_texture_amount)
    {
        enlarge_texture_amount_ = enlarge_texture_amount;
    }

    // Common CSS colors defined for tests to use.
    static const SkColor kCSSOrange = 0xffffa500;
    static const SkColor kCSSBrown = 0xffa52a2a;
    static const SkColor kCSSGreen = 0xff008000;

    gfx::DisableNullDrawGLBindings enable_pixel_output_;
    scoped_ptr<PixelComparator> pixel_comparator_;
    PixelTestType test_type_;
    scoped_refptr<Layer> content_root_;
    Layer* readback_target_;
    base::FilePath ref_file_;
    scoped_ptr<SkBitmap> result_bitmap_;
    std::vector<scoped_refptr<TextureLayer>> texture_layers_;
    int pending_texture_mailbox_callbacks_;
    gfx::Vector2d enlarge_texture_amount_;
};

} // namespace cc

#endif // CC_TEST_LAYER_TREE_PIXEL_TEST_H_
