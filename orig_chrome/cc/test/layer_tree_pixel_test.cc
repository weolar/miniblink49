// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/layer_tree_pixel_test.h"

#include "base/command_line.h"
#include "base/path_service.h"
#include "cc/base/switches.h"
#include "cc/layers/solid_color_layer.h"
#include "cc/layers/texture_layer.h"
#include "cc/output/copy_output_request.h"
#include "cc/output/copy_output_result.h"
#include "cc/output/direct_renderer.h"
#include "cc/resources/texture_mailbox.h"
#include "cc/test/paths.h"
#include "cc/test/pixel_comparator.h"
#include "cc/test/pixel_test_output_surface.h"
#include "cc/test/pixel_test_software_output_device.h"
#include "cc/test/pixel_test_utils.h"
#include "cc/test/test_in_process_context_provider.h"
#include "cc/trees/layer_tree_impl.h"
#include "gpu/command_buffer/client/gl_in_process_context.h"
#include "gpu/command_buffer/client/gles2_implementation.h"

using gpu::gles2::GLES2Interface;

namespace cc {

LayerTreePixelTest::LayerTreePixelTest()
    : pixel_comparator_(new ExactPixelComparator(true))
    , test_type_(PIXEL_TEST_GL)
    , pending_texture_mailbox_callbacks_(0)
{
}

LayerTreePixelTest::~LayerTreePixelTest() { }

scoped_ptr<OutputSurface> LayerTreePixelTest::CreateOutputSurface()
{
    gfx::Size surface_expansion_size(40, 60);
    scoped_ptr<PixelTestOutputSurface> output_surface;

    switch (test_type_) {
    case PIXEL_TEST_SOFTWARE: {
        scoped_ptr<PixelTestSoftwareOutputDevice> software_output_device(
            new PixelTestSoftwareOutputDevice);
        software_output_device->set_surface_expansion_size(
            surface_expansion_size);
        output_surface = make_scoped_ptr(
            new PixelTestOutputSurface(software_output_device.Pass()));
        break;
    }
    case PIXEL_TEST_GL: {
        bool flipped_output_surface = false;
        output_surface = make_scoped_ptr(new PixelTestOutputSurface(
            new TestInProcessContextProvider, new TestInProcessContextProvider,
            flipped_output_surface));
        break;
    }
    }

    output_surface->set_surface_expansion_size(surface_expansion_size);
    return output_surface.Pass();
}

void LayerTreePixelTest::WillCommitCompleteOnThread(LayerTreeHostImpl* impl)
{
    if (impl->sync_tree()->source_frame_number() != 0)
        return;

    DirectRenderer* renderer = static_cast<DirectRenderer*>(impl->renderer());
    renderer->SetEnlargePassTextureAmountForTesting(enlarge_texture_amount_);

    gfx::Rect viewport = impl->DeviceViewport();
    // The viewport has a 0,0 origin without external influence.
    EXPECT_EQ(gfx::Point().ToString(), viewport.origin().ToString());
    // Be that influence!
    viewport += gfx::Vector2d(20, 10);
    bool resourceless_software_draw = false;
    gfx::Transform identity = gfx::Transform();
    impl->SetExternalDrawConstraints(identity,
        viewport,
        viewport,
        viewport,
        identity,
        resourceless_software_draw);
    EXPECT_EQ(viewport.ToString(), impl->DeviceViewport().ToString());
}

scoped_ptr<CopyOutputRequest> LayerTreePixelTest::CreateCopyOutputRequest()
{
    return CopyOutputRequest::CreateBitmapRequest(
        base::Bind(&LayerTreePixelTest::ReadbackResult, base::Unretained(this)));
}

void LayerTreePixelTest::ReadbackResult(scoped_ptr<CopyOutputResult> result)
{
    ASSERT_TRUE(result->HasBitmap());
    result_bitmap_ = result->TakeBitmap().Pass();
    EndTest();
}

void LayerTreePixelTest::BeginTest()
{
    Layer* target = readback_target_ ? readback_target_
                                     : layer_tree_host()->root_layer();
    target->RequestCopyOfOutput(CreateCopyOutputRequest().Pass());
    PostSetNeedsCommitToMainThread();
}

void LayerTreePixelTest::AfterTest()
{
    base::FilePath test_data_dir;
    EXPECT_TRUE(PathService::Get(CCPaths::DIR_TEST_DATA, &test_data_dir));
    base::FilePath ref_file_path = test_data_dir.Append(ref_file_);

    base::CommandLine* cmd = base::CommandLine::ForCurrentProcess();
    if (cmd->HasSwitch(switches::kCCRebaselinePixeltests))
        EXPECT_TRUE(WritePNGFile(*result_bitmap_, ref_file_path, true));
    EXPECT_TRUE(MatchesPNGFile(*result_bitmap_,
        ref_file_path,
        *pixel_comparator_));
}

scoped_refptr<SolidColorLayer> LayerTreePixelTest::CreateSolidColorLayer(
    const gfx::Rect& rect, SkColor color)
{
    scoped_refptr<SolidColorLayer> layer = SolidColorLayer::Create(layer_settings());
    layer->SetIsDrawable(true);
    layer->SetBounds(rect.size());
    layer->SetPosition(rect.origin());
    layer->SetBackgroundColor(color);
    return layer;
}

void LayerTreePixelTest::EndTest()
{
    // Drop TextureMailboxes on the main thread so that they can be cleaned up and
    // the pending callbacks will fire.
    for (size_t i = 0; i < texture_layers_.size(); ++i) {
        texture_layers_[i]->SetTextureMailbox(TextureMailbox(), nullptr);
    }

    TryEndTest();
}

void LayerTreePixelTest::TryEndTest()
{
    if (!result_bitmap_)
        return;
    if (pending_texture_mailbox_callbacks_)
        return;
    LayerTreeTest::EndTest();
}

scoped_refptr<SolidColorLayer> LayerTreePixelTest::
    CreateSolidColorLayerWithBorder(
        const gfx::Rect& rect, SkColor color,
        int border_width, SkColor border_color)
{
    scoped_refptr<SolidColorLayer> layer = CreateSolidColorLayer(rect, color);
    scoped_refptr<SolidColorLayer> border_top = CreateSolidColorLayer(
        gfx::Rect(0, 0, rect.width(), border_width), border_color);
    scoped_refptr<SolidColorLayer> border_left = CreateSolidColorLayer(
        gfx::Rect(0,
            border_width,
            border_width,
            rect.height() - border_width * 2),
        border_color);
    scoped_refptr<SolidColorLayer> border_right = CreateSolidColorLayer(gfx::Rect(rect.width() - border_width,
                                                                            border_width,
                                                                            border_width,
                                                                            rect.height() - border_width * 2),
        border_color);
    scoped_refptr<SolidColorLayer> border_bottom = CreateSolidColorLayer(
        gfx::Rect(0, rect.height() - border_width, rect.width(), border_width),
        border_color);
    layer->AddChild(border_top);
    layer->AddChild(border_left);
    layer->AddChild(border_right);
    layer->AddChild(border_bottom);
    return layer;
}

scoped_refptr<TextureLayer> LayerTreePixelTest::CreateTextureLayer(
    const gfx::Rect& rect, const SkBitmap& bitmap)
{
    scoped_refptr<TextureLayer> layer = TextureLayer::CreateForMailbox(layer_settings(), NULL);
    layer->SetIsDrawable(true);
    layer->SetBounds(rect.size());
    layer->SetPosition(rect.origin());

    TextureMailbox texture_mailbox;
    scoped_ptr<SingleReleaseCallback> release_callback;
    CopyBitmapToTextureMailboxAsTexture(
        bitmap, &texture_mailbox, &release_callback);
    layer->SetTextureMailbox(texture_mailbox, release_callback.Pass());

    texture_layers_.push_back(layer);
    pending_texture_mailbox_callbacks_++;
    return layer;
}

void LayerTreePixelTest::RunPixelTest(
    PixelTestType test_type,
    scoped_refptr<Layer> content_root,
    base::FilePath file_name)
{
    test_type_ = test_type;
    content_root_ = content_root;
    readback_target_ = NULL;
    ref_file_ = file_name;
    bool threaded = true;
    RunTest(threaded, false);
}

void LayerTreePixelTest::RunSingleThreadedPixelTest(
    PixelTestType test_type,
    scoped_refptr<Layer> content_root,
    base::FilePath file_name)
{
    test_type_ = test_type;
    content_root_ = content_root;
    readback_target_ = NULL;
    ref_file_ = file_name;
    bool threaded = false;
    RunTest(threaded, false);
}

void LayerTreePixelTest::RunPixelTestWithReadbackTarget(
    PixelTestType test_type,
    scoped_refptr<Layer> content_root,
    Layer* target,
    base::FilePath file_name)
{
    test_type_ = test_type;
    content_root_ = content_root;
    readback_target_ = target;
    ref_file_ = file_name;
    RunTest(true, false);
}

void LayerTreePixelTest::SetupTree()
{
    scoped_refptr<Layer> root = Layer::Create(layer_settings());
    root->SetBounds(content_root_->bounds());
    root->AddChild(content_root_);
    layer_tree_host()->SetRootLayer(root);
    LayerTreeTest::SetupTree();
}

scoped_ptr<SkBitmap> LayerTreePixelTest::CopyTextureMailboxToBitmap(
    const gfx::Size& size,
    const TextureMailbox& texture_mailbox)
{
    DCHECK(texture_mailbox.IsTexture());
    if (!texture_mailbox.IsTexture())
        return nullptr;

    scoped_ptr<gpu::GLInProcessContext> context = CreateTestInProcessContext();
    GLES2Interface* gl = context->GetImplementation();

    if (texture_mailbox.sync_point())
        gl->WaitSyncPointCHROMIUM(texture_mailbox.sync_point());

    GLuint texture_id = 0;
    gl->GenTextures(1, &texture_id);
    gl->BindTexture(GL_TEXTURE_2D, texture_id);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->ConsumeTextureCHROMIUM(texture_mailbox.target(), texture_mailbox.name());
    gl->BindTexture(GL_TEXTURE_2D, 0);

    GLuint fbo = 0;
    gl->GenFramebuffers(1, &fbo);
    gl->BindFramebuffer(GL_FRAMEBUFFER, fbo);
    gl->FramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);
    EXPECT_EQ(static_cast<unsigned>(GL_FRAMEBUFFER_COMPLETE),
        gl->CheckFramebufferStatus(GL_FRAMEBUFFER));

    scoped_ptr<uint8[]> pixels(new uint8[size.GetArea() * 4]);
    gl->ReadPixels(0,
        0,
        size.width(),
        size.height(),
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels.get());

    gl->DeleteFramebuffers(1, &fbo);
    gl->DeleteTextures(1, &texture_id);

    scoped_ptr<SkBitmap> bitmap(new SkBitmap);
    bitmap->allocN32Pixels(size.width(), size.height());

    uint8* out_pixels = static_cast<uint8*>(bitmap->getPixels());

    size_t row_bytes = size.width() * 4;
    size_t total_bytes = size.height() * row_bytes;
    for (size_t dest_y = 0; dest_y < total_bytes; dest_y += row_bytes) {
        // Flip Y axis.
        size_t src_y = total_bytes - dest_y - row_bytes;
        // Swizzle OpenGL -> Skia byte order.
        for (size_t x = 0; x < row_bytes; x += 4) {
            out_pixels[dest_y + x + SK_R32_SHIFT / 8] = pixels.get()[src_y + x + 0];
            out_pixels[dest_y + x + SK_G32_SHIFT / 8] = pixels.get()[src_y + x + 1];
            out_pixels[dest_y + x + SK_B32_SHIFT / 8] = pixels.get()[src_y + x + 2];
            out_pixels[dest_y + x + SK_A32_SHIFT / 8] = pixels.get()[src_y + x + 3];
        }
    }

    return bitmap.Pass();
}

void LayerTreePixelTest::ReleaseTextureMailbox(
    scoped_ptr<gpu::GLInProcessContext> context,
    uint32 texture,
    uint32 sync_point,
    bool lost_resource)
{
    GLES2Interface* gl = context->GetImplementation();
    if (sync_point)
        gl->WaitSyncPointCHROMIUM(sync_point);
    gl->DeleteTextures(1, &texture);
    pending_texture_mailbox_callbacks_--;
    TryEndTest();
}

void LayerTreePixelTest::CopyBitmapToTextureMailboxAsTexture(
    const SkBitmap& bitmap,
    TextureMailbox* texture_mailbox,
    scoped_ptr<SingleReleaseCallback>* release_callback)
{
    DCHECK_GT(bitmap.width(), 0);
    DCHECK_GT(bitmap.height(), 0);

    scoped_ptr<gpu::GLInProcessContext> context = CreateTestInProcessContext();
    GLES2Interface* gl = context->GetImplementation();

    GLuint texture_id = 0;
    gl->GenTextures(1, &texture_id);
    gl->BindTexture(GL_TEXTURE_2D, texture_id);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    DCHECK_EQ(kN32_SkColorType, bitmap.colorType());

    {
        SkAutoLockPixels lock(bitmap);

        size_t row_bytes = bitmap.width() * 4;
        size_t total_bytes = bitmap.height() * row_bytes;

        scoped_ptr<uint8[]> gl_pixels(new uint8[total_bytes]);
        uint8* bitmap_pixels = static_cast<uint8*>(bitmap.getPixels());

        for (size_t y = 0; y < total_bytes; y += row_bytes) {
            // Flip Y axis.
            size_t src_y = total_bytes - y - row_bytes;
            // Swizzle Skia -> OpenGL byte order.
            for (size_t x = 0; x < row_bytes; x += 4) {
                gl_pixels.get()[y + x + 0] = bitmap_pixels[src_y + x + SK_R32_SHIFT / 8];
                gl_pixels.get()[y + x + 1] = bitmap_pixels[src_y + x + SK_G32_SHIFT / 8];
                gl_pixels.get()[y + x + 2] = bitmap_pixels[src_y + x + SK_B32_SHIFT / 8];
                gl_pixels.get()[y + x + 3] = bitmap_pixels[src_y + x + SK_A32_SHIFT / 8];
            }
        }

        gl->TexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGBA,
            bitmap.width(),
            bitmap.height(),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            gl_pixels.get());
    }

    gpu::Mailbox mailbox;
    gl->GenMailboxCHROMIUM(mailbox.name);
    gl->ProduceTextureCHROMIUM(GL_TEXTURE_2D, mailbox.name);
    gl->BindTexture(GL_TEXTURE_2D, 0);
    uint32 sync_point = gl->InsertSyncPointCHROMIUM();

    *texture_mailbox = TextureMailbox(mailbox, GL_TEXTURE_2D, sync_point);
    *release_callback = SingleReleaseCallback::Create(
        base::Bind(&LayerTreePixelTest::ReleaseTextureMailbox,
            base::Unretained(this),
            base::Passed(&context),
            texture_id));
}

void LayerTreePixelTest::Finish()
{
    scoped_ptr<gpu::GLInProcessContext> context = CreateTestInProcessContext();
    GLES2Interface* gl = context->GetImplementation();
    gl->Finish();
}

} // namespace cc
