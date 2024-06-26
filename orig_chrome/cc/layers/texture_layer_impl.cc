// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/texture_layer_impl.h"

#include <vector>

#include "base/strings/stringprintf.h"
#include "cc/output/renderer.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/resources/platform_color.h"
#include "cc/resources/scoped_resource.h"
#include "cc/resources/single_release_callback_impl.h"
#include "cc/trees/layer_tree_impl.h"
#include "cc/trees/occlusion.h"

namespace cc {

TextureLayerImpl::TextureLayerImpl(LayerTreeImpl* tree_impl, int id)
    : LayerImpl(tree_impl, id)
    , external_texture_resource_(0)
    , premultiplied_alpha_(true)
    , blend_background_color_(false)
    , flipped_(true)
    , nearest_neighbor_(false)
    , uv_top_left_(0.f, 0.f)
    , uv_bottom_right_(1.f, 1.f)
    , own_mailbox_(false)
    , valid_texture_copy_(false)
{
    vertex_opacity_[0] = 1.0f;
    vertex_opacity_[1] = 1.0f;
    vertex_opacity_[2] = 1.0f;
    vertex_opacity_[3] = 1.0f;
}

TextureLayerImpl::~TextureLayerImpl() { FreeTextureMailbox(); }

void TextureLayerImpl::SetTextureMailbox(
    const TextureMailbox& mailbox,
    scoped_ptr<SingleReleaseCallbackImpl> release_callback)
{
    DCHECK_EQ(mailbox.IsValid(), !!release_callback);
    FreeTextureMailbox();
    texture_mailbox_ = mailbox;
    release_callback_ = release_callback.Pass();
    own_mailbox_ = true;
    valid_texture_copy_ = false;
    SetNeedsPushProperties();
}

scoped_ptr<LayerImpl> TextureLayerImpl::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    return TextureLayerImpl::Create(tree_impl, id());
}

void TextureLayerImpl::PushPropertiesTo(LayerImpl* layer)
{
    LayerImpl::PushPropertiesTo(layer);

    TextureLayerImpl* texture_layer = static_cast<TextureLayerImpl*>(layer);
    texture_layer->SetFlipped(flipped_);
    texture_layer->SetUVTopLeft(uv_top_left_);
    texture_layer->SetUVBottomRight(uv_bottom_right_);
    texture_layer->SetVertexOpacity(vertex_opacity_);
    texture_layer->SetPremultipliedAlpha(premultiplied_alpha_);
    texture_layer->SetBlendBackgroundColor(blend_background_color_);
    texture_layer->SetNearestNeighbor(nearest_neighbor_);
    if (own_mailbox_) {
        texture_layer->SetTextureMailbox(texture_mailbox_,
            release_callback_.Pass());
        own_mailbox_ = false;
    }
}

bool TextureLayerImpl::WillDraw(DrawMode draw_mode,
    ResourceProvider* resource_provider)
{
    if (draw_mode == DRAW_MODE_RESOURCELESS_SOFTWARE)
        return false;

    if (own_mailbox_) {
        DCHECK(!external_texture_resource_);
        if ((draw_mode == DRAW_MODE_HARDWARE && texture_mailbox_.IsTexture()) || (draw_mode == DRAW_MODE_SOFTWARE && texture_mailbox_.IsSharedMemory())) {
            external_texture_resource_ = resource_provider->CreateResourceFromTextureMailbox(
                texture_mailbox_, release_callback_.Pass());
            DCHECK(external_texture_resource_);
            texture_copy_ = nullptr;
            valid_texture_copy_ = false;
        }
        if (external_texture_resource_)
            own_mailbox_ = false;
    }

    if (!valid_texture_copy_ && draw_mode == DRAW_MODE_HARDWARE && texture_mailbox_.IsSharedMemory()) {
        DCHECK(!external_texture_resource_);
        // Have to upload a copy to a texture for it to be used in a
        // hardware draw.
        if (!texture_copy_)
            texture_copy_ = ScopedResource::Create(resource_provider);
        if (texture_copy_->size() != texture_mailbox_.size_in_pixels() || resource_provider->InUseByConsumer(texture_copy_->id()))
            texture_copy_->Free();

        if (!texture_copy_->id()) {
            texture_copy_->Allocate(texture_mailbox_.size_in_pixels(),
                ResourceProvider::TEXTURE_HINT_IMMUTABLE,
                resource_provider->best_texture_format());
        }

        if (texture_copy_->id()) {
            std::vector<uint8> swizzled;
            uint8* pixels = texture_mailbox_.shared_bitmap()->pixels();

            if (!PlatformColor::SameComponentOrder(texture_copy_->format())) {
                // Swizzle colors. This is slow, but should be really uncommon.
                size_t bytes = texture_mailbox_.SharedMemorySizeInBytes();
                swizzled.resize(bytes);
                for (size_t i = 0; i < bytes; i += 4) {
                    swizzled[i] = pixels[i + 2];
                    swizzled[i + 1] = pixels[i + 1];
                    swizzled[i + 2] = pixels[i];
                    swizzled[i + 3] = pixels[i + 3];
                }
                pixels = &swizzled[0];
            }

            resource_provider->CopyToResource(texture_copy_->id(), pixels,
                texture_mailbox_.size_in_pixels());

            valid_texture_copy_ = true;
        }
    }
    return (external_texture_resource_ || valid_texture_copy_) && LayerImpl::WillDraw(draw_mode, resource_provider);
}

void TextureLayerImpl::AppendQuads(RenderPass* render_pass,
    AppendQuadsData* append_quads_data)
{
    DCHECK(external_texture_resource_ || valid_texture_copy_);

    SharedQuadState* shared_quad_state = render_pass->CreateAndAppendSharedQuadState();
    PopulateSharedQuadState(shared_quad_state);

    AppendDebugBorderQuad(render_pass, bounds(), shared_quad_state,
        append_quads_data);

    SkColor bg_color = blend_background_color_ ? background_color() : SK_ColorTRANSPARENT;
    bool opaque = contents_opaque() || (SkColorGetA(bg_color) == 0xFF);

    gfx::Rect quad_rect(bounds());
    gfx::Rect opaque_rect = opaque ? quad_rect : gfx::Rect();
    gfx::Rect visible_quad_rect = draw_properties().occlusion_in_content_space.GetUnoccludedContentRect(
        quad_rect);
    if (visible_quad_rect.IsEmpty())
        return;

    TextureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
    ResourceId id = valid_texture_copy_ ? texture_copy_->id() : external_texture_resource_;
    quad->SetNew(shared_quad_state,
        quad_rect,
        opaque_rect,
        visible_quad_rect,
        id,
        premultiplied_alpha_,
        uv_top_left_,
        uv_bottom_right_,
        bg_color,
        vertex_opacity_,
        flipped_,
        nearest_neighbor_);
    if (!valid_texture_copy_) {
        quad->set_resource_size_in_pixels(texture_mailbox_.size_in_pixels());
        quad->set_allow_overlay(texture_mailbox_.allow_overlay());
    }
    ValidateQuadResources(quad);
}

SimpleEnclosedRegion TextureLayerImpl::VisibleOpaqueRegion() const
{
    if (contents_opaque())
        return SimpleEnclosedRegion(visible_layer_rect());

    if (blend_background_color_ && (SkColorGetA(background_color()) == 0xFF))
        return SimpleEnclosedRegion(visible_layer_rect());

    return SimpleEnclosedRegion();
}

void TextureLayerImpl::ReleaseResources()
{
    FreeTextureMailbox();
    texture_copy_ = nullptr;
    external_texture_resource_ = 0;
    valid_texture_copy_ = false;
}

void TextureLayerImpl::SetPremultipliedAlpha(bool premultiplied_alpha)
{
    premultiplied_alpha_ = premultiplied_alpha;
    SetNeedsPushProperties();
}

void TextureLayerImpl::SetBlendBackgroundColor(bool blend)
{
    blend_background_color_ = blend;
    SetNeedsPushProperties();
}

void TextureLayerImpl::SetFlipped(bool flipped)
{
    flipped_ = flipped;
    SetNeedsPushProperties();
}

void TextureLayerImpl::SetNearestNeighbor(bool nearest_neighbor)
{
    nearest_neighbor_ = nearest_neighbor;
    SetNeedsPushProperties();
}

void TextureLayerImpl::SetUVTopLeft(const gfx::PointF& top_left)
{
    uv_top_left_ = top_left;
    SetNeedsPushProperties();
}

void TextureLayerImpl::SetUVBottomRight(const gfx::PointF& bottom_right)
{
    uv_bottom_right_ = bottom_right;
    SetNeedsPushProperties();
}

// 1--2
// |  |
// 0--3
void TextureLayerImpl::SetVertexOpacity(const float vertex_opacity[4])
{
    vertex_opacity_[0] = vertex_opacity[0];
    vertex_opacity_[1] = vertex_opacity[1];
    vertex_opacity_[2] = vertex_opacity[2];
    vertex_opacity_[3] = vertex_opacity[3];
    SetNeedsPushProperties();
}

const char* TextureLayerImpl::LayerTypeAsString() const
{
    return "cc::TextureLayerImpl";
}

void TextureLayerImpl::FreeTextureMailbox()
{
    if (own_mailbox_) {
        DCHECK(!external_texture_resource_);
        if (release_callback_) {
            release_callback_->Run(texture_mailbox_.sync_token(),
                false,
                layer_tree_impl()->BlockingMainThreadTaskRunner());
        }
        texture_mailbox_ = TextureMailbox();
        release_callback_ = nullptr;
    } else if (external_texture_resource_) {
        DCHECK(!own_mailbox_);
        ResourceProvider* resource_provider = layer_tree_impl()->resource_provider();
        resource_provider->DeleteResource(external_texture_resource_);
        external_texture_resource_ = 0;
    }
}

} // namespace cc
