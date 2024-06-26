// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/texture_layer.h"

#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/location.h"
#include "base/synchronization/lock.h"
#include "cc/base/simple_enclosed_region.h"
#include "cc/layers/texture_layer_client.h"
#include "cc/layers/texture_layer_impl.h"
#include "cc/resources/single_release_callback.h"
#include "cc/resources/single_release_callback_impl.h"
#include "cc/trees/blocking_task_runner.h"
#include "cc/trees/layer_tree_host.h"

namespace cc {

scoped_refptr<TextureLayer> TextureLayer::CreateForMailbox(
    const LayerSettings& settings,
    TextureLayerClient* client)
{
    return scoped_refptr<TextureLayer>(new TextureLayer(settings, client));
}

TextureLayer::TextureLayer(const LayerSettings& settings,
    TextureLayerClient* client)
    : Layer(settings)
    , client_(client)
    , flipped_(true)
    , nearest_neighbor_(false)
    , uv_top_left_(0.f, 0.f)
    , uv_bottom_right_(1.f, 1.f)
    , premultiplied_alpha_(true)
    , blend_background_color_(false)
    , needs_set_mailbox_(false)
{
    vertex_opacity_[0] = 1.0f;
    vertex_opacity_[1] = 1.0f;
    vertex_opacity_[2] = 1.0f;
    vertex_opacity_[3] = 1.0f;
}

TextureLayer::~TextureLayer()
{
}

void TextureLayer::ClearClient()
{
    client_ = nullptr;
    ClearTexture();
    UpdateDrawsContent(HasDrawableContent());
}

void TextureLayer::ClearTexture()
{
    SetTextureMailbox(TextureMailbox(), nullptr);
}

scoped_ptr<LayerImpl> TextureLayer::CreateLayerImpl(LayerTreeImpl* tree_impl)
{
    return TextureLayerImpl::Create(tree_impl, id());
}

void TextureLayer::SetFlipped(bool flipped)
{
    if (flipped_ == flipped)
        return;
    flipped_ = flipped;
    SetNeedsCommit();
}

void TextureLayer::SetNearestNeighbor(bool nearest_neighbor)
{
    if (nearest_neighbor_ == nearest_neighbor)
        return;
    nearest_neighbor_ = nearest_neighbor;
    SetNeedsCommit();
}

void TextureLayer::SetUV(const gfx::PointF& top_left,
    const gfx::PointF& bottom_right)
{
    if (uv_top_left_ == top_left && uv_bottom_right_ == bottom_right)
        return;
    uv_top_left_ = top_left;
    uv_bottom_right_ = bottom_right;
    SetNeedsCommit();
}

void TextureLayer::SetVertexOpacity(float bottom_left,
    float top_left,
    float top_right,
    float bottom_right)
{
    // Indexing according to the quad vertex generation:
    // 1--2
    // |  |
    // 0--3
    if (vertex_opacity_[0] == bottom_left && vertex_opacity_[1] == top_left && vertex_opacity_[2] == top_right && vertex_opacity_[3] == bottom_right)
        return;
    vertex_opacity_[0] = bottom_left;
    vertex_opacity_[1] = top_left;
    vertex_opacity_[2] = top_right;
    vertex_opacity_[3] = bottom_right;
    SetNeedsCommit();
}

void TextureLayer::SetPremultipliedAlpha(bool premultiplied_alpha)
{
    if (premultiplied_alpha_ == premultiplied_alpha)
        return;
    premultiplied_alpha_ = premultiplied_alpha;
    SetNeedsCommit();
}

void TextureLayer::SetBlendBackgroundColor(bool blend)
{
    if (blend_background_color_ == blend)
        return;
    blend_background_color_ = blend;
    SetNeedsCommit();
}

void TextureLayer::SetTextureMailboxInternal(
    const TextureMailbox& mailbox,
    scoped_ptr<SingleReleaseCallback> release_callback,
    bool requires_commit,
    bool allow_mailbox_reuse)
{
    DCHECK(!mailbox.IsValid() || !holder_ref_ || !mailbox.Equals(holder_ref_->holder()->mailbox()) || allow_mailbox_reuse);
    DCHECK_EQ(mailbox.IsValid(), !!release_callback);

    // If we never commited the mailbox, we need to release it here.
    if (mailbox.IsValid()) {
        holder_ref_ = TextureMailboxHolder::Create(mailbox, release_callback.Pass());
    } else {
        holder_ref_ = nullptr;
    }
    needs_set_mailbox_ = true;
    // If we are within a commit, no need to do it again immediately after.
    if (requires_commit)
        SetNeedsCommit();
    else
        SetNeedsPushProperties();

    UpdateDrawsContent(HasDrawableContent());
    // The active frame needs to be replaced and the mailbox returned before the
    // commit is called complete.
    SetNextCommitWaitsForActivation();
}

void TextureLayer::SetTextureMailbox(
    const TextureMailbox& mailbox,
    scoped_ptr<SingleReleaseCallback> release_callback)
{
    bool requires_commit = true;
    bool allow_mailbox_reuse = false;
    SetTextureMailboxInternal(
        mailbox, release_callback.Pass(), requires_commit, allow_mailbox_reuse);
}

static void IgnoreReleaseCallback(const gpu::SyncToken& sync_token, bool lost) { }

void TextureLayer::SetTextureMailboxWithoutReleaseCallback(
    const TextureMailbox& mailbox)
{
    // We allow reuse of the mailbox if there is a new sync point signalling new
    // content, and the release callback goes nowhere since we'll be calling it
    // multiple times for the same mailbox.
    DCHECK(!mailbox.IsValid() || !holder_ref_ || !mailbox.Equals(holder_ref_->holder()->mailbox()) || mailbox.sync_token() != holder_ref_->holder()->mailbox().sync_token());
    scoped_ptr<SingleReleaseCallback> release;
    bool requires_commit = true;
    bool allow_mailbox_reuse = true;
    if (mailbox.IsValid())
        release = SingleReleaseCallback::Create(base::Bind(&IgnoreReleaseCallback));
    SetTextureMailboxInternal(
        mailbox, release.Pass(), requires_commit, allow_mailbox_reuse);
}

void TextureLayer::SetNeedsDisplayRect(const gfx::Rect& dirty_rect)
{
    Layer::SetNeedsDisplayRect(dirty_rect);
}

void TextureLayer::SetLayerTreeHost(LayerTreeHost* host)
{
    if (layer_tree_host() == host) {
        Layer::SetLayerTreeHost(host);
        return;
    }

    // If we're removed from the tree, the TextureLayerImpl will be destroyed, and
    // we will need to set the mailbox again on a new TextureLayerImpl the next
    // time we push.
    if (!host && holder_ref_) {
        needs_set_mailbox_ = true;
        // The active frame needs to be replaced and the mailbox returned before the
        // commit is called complete.
        SetNextCommitWaitsForActivation();
    }
    Layer::SetLayerTreeHost(host);
}

bool TextureLayer::HasDrawableContent() const
{
    return (client_ || holder_ref_) && Layer::HasDrawableContent();
}

bool TextureLayer::Update()
{
    bool updated = Layer::Update();
    if (client_) {
        TextureMailbox mailbox;
        scoped_ptr<SingleReleaseCallback> release_callback;
        if (client_->PrepareTextureMailbox(
                &mailbox,
                &release_callback,
                layer_tree_host()->UsingSharedMemoryResources())) {
            // Already within a commit, no need to do another one immediately.
            bool requires_commit = false;
            bool allow_mailbox_reuse = false;
            SetTextureMailboxInternal(mailbox,
                release_callback.Pass(),
                requires_commit,
                allow_mailbox_reuse);
            updated = true;
        }
    }

    // SetTextureMailbox could be called externally and the same mailbox used for
    // different textures.  Such callers notify this layer that the texture has
    // changed by calling SetNeedsDisplay, so check for that here.
    return updated || !update_rect_.IsEmpty();
}

void TextureLayer::PushPropertiesTo(LayerImpl* layer)
{
    Layer::PushPropertiesTo(layer);

    TextureLayerImpl* texture_layer = static_cast<TextureLayerImpl*>(layer);
    texture_layer->SetFlipped(flipped_);
    texture_layer->SetNearestNeighbor(nearest_neighbor_);
    texture_layer->SetUVTopLeft(uv_top_left_);
    texture_layer->SetUVBottomRight(uv_bottom_right_);
    texture_layer->SetVertexOpacity(vertex_opacity_);
    texture_layer->SetPremultipliedAlpha(premultiplied_alpha_);
    texture_layer->SetBlendBackgroundColor(blend_background_color_);
    if (needs_set_mailbox_) {
        TextureMailbox texture_mailbox;
        scoped_ptr<SingleReleaseCallbackImpl> release_callback_impl;
        if (holder_ref_) {
            TextureMailboxHolder* holder = holder_ref_->holder();
            texture_mailbox = holder->mailbox();
            release_callback_impl = holder->GetCallbackForImplThread();
        }
        texture_layer->SetTextureMailbox(texture_mailbox,
            release_callback_impl.Pass());
        needs_set_mailbox_ = false;
    }
}

TextureLayer::TextureMailboxHolder::MainThreadReference::MainThreadReference(
    TextureMailboxHolder* holder)
    : holder_(holder)
{
    holder_->InternalAddRef();
}

TextureLayer::TextureMailboxHolder::MainThreadReference::
    ~MainThreadReference()
{
    holder_->InternalRelease();
}

TextureLayer::TextureMailboxHolder::TextureMailboxHolder(
    const TextureMailbox& mailbox,
    scoped_ptr<SingleReleaseCallback> release_callback)
    : internal_references_(0)
    , mailbox_(mailbox)
    , release_callback_(release_callback.Pass())
    , sync_token_(mailbox.sync_token())
    , is_lost_(false)
{
}

TextureLayer::TextureMailboxHolder::~TextureMailboxHolder()
{
    DCHECK_EQ(0u, internal_references_);
}

scoped_ptr<TextureLayer::TextureMailboxHolder::MainThreadReference>
TextureLayer::TextureMailboxHolder::Create(
    const TextureMailbox& mailbox,
    scoped_ptr<SingleReleaseCallback> release_callback)
{
    return make_scoped_ptr(new MainThreadReference(
        new TextureMailboxHolder(mailbox, release_callback.Pass())));
}

void TextureLayer::TextureMailboxHolder::Return(const gpu::SyncToken& sync_token,
    bool is_lost)
{
    base::AutoLock lock(arguments_lock_);
    sync_token_ = sync_token;
    is_lost_ = is_lost;
}

scoped_ptr<SingleReleaseCallbackImpl>
TextureLayer::TextureMailboxHolder::GetCallbackForImplThread()
{
    // We can't call GetCallbackForImplThread if we released the main thread
    // reference.
    DCHECK_GT(internal_references_, 0u);
    InternalAddRef();
    return SingleReleaseCallbackImpl::Create(
        base::Bind(&TextureMailboxHolder::ReturnAndReleaseOnImplThread, this));
}

void TextureLayer::TextureMailboxHolder::InternalAddRef()
{
    ++internal_references_;
}

void TextureLayer::TextureMailboxHolder::InternalRelease()
{
    DCHECK(main_thread_checker_.CalledOnValidThread());
    if (!--internal_references_) {
        release_callback_->Run(sync_token_, is_lost_);
        mailbox_ = TextureMailbox();
        release_callback_ = nullptr;
    }
}

void TextureLayer::TextureMailboxHolder::ReturnAndReleaseOnImplThread(
    const gpu::SyncToken& sync_token,
    bool is_lost,
    BlockingTaskRunner* main_thread_task_runner)
{
    Return(sync_token, is_lost);
    main_thread_task_runner->PostTask(
        FROM_HERE, base::Bind(&TextureMailboxHolder::InternalRelease, this));
}

} // namespace cc
