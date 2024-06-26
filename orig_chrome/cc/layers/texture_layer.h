// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_TEXTURE_LAYER_H_
#define CC_LAYERS_TEXTURE_LAYER_H_

#include <string>

#include "base/callback.h"
#include "base/synchronization/lock.h"
#include "base/threading/thread_checker.h"
#include "cc/base/cc_export.h"
#include "cc/layers/layer.h"
#include "cc/resources/texture_mailbox.h"

namespace cc {
class BlockingTaskRunner;
class SingleReleaseCallback;
class SingleReleaseCallbackImpl;
class TextureLayerClient;

// A Layer containing a the rendered output of a plugin instance.
class CC_EXPORT TextureLayer : public Layer {
public:
    class CC_EXPORT TextureMailboxHolder
        : public base::RefCountedThreadSafe<TextureMailboxHolder> {
    public:
        class CC_EXPORT MainThreadReference {
        public:
            explicit MainThreadReference(TextureMailboxHolder* holder);
            ~MainThreadReference();
            TextureMailboxHolder* holder() { return holder_.get(); }

        private:
            scoped_refptr<TextureMailboxHolder> holder_;
            DISALLOW_COPY_AND_ASSIGN(MainThreadReference);
        };

        const TextureMailbox& mailbox() const { return mailbox_; }
        void Return(const gpu::SyncToken& sync_token, bool is_lost);

        // Gets a ReleaseCallback that can be called from another thread. Note: the
        // caller must ensure the callback is called.
        scoped_ptr<SingleReleaseCallbackImpl> GetCallbackForImplThread();

    protected:
        friend class TextureLayer;

        // Protected visiblity so only TextureLayer and unit tests can create these.
        static scoped_ptr<MainThreadReference> Create(
            const TextureMailbox& mailbox,
            scoped_ptr<SingleReleaseCallback> release_callback);
        virtual ~TextureMailboxHolder();

    private:
        friend class base::RefCountedThreadSafe<TextureMailboxHolder>;
        friend class MainThreadReference;
        explicit TextureMailboxHolder(
            const TextureMailbox& mailbox,
            scoped_ptr<SingleReleaseCallback> release_callback);

        void InternalAddRef();
        void InternalRelease();
        void ReturnAndReleaseOnImplThread(
            const gpu::SyncToken& sync_token,
            bool is_lost,
            BlockingTaskRunner* main_thread_task_runner);

        // These members are only accessed on the main thread, or on the impl thread
        // during commit where the main thread is blocked.
        unsigned internal_references_;
        TextureMailbox mailbox_;
        scoped_ptr<SingleReleaseCallback> release_callback_;

        // This lock guards the sync_token_ and is_lost_ fields because they can be
        // accessed on both the impl and main thread. We do this to ensure that the
        // values of these fields are well-ordered such that the last call to
        // ReturnAndReleaseOnImplThread() defines their values.
        base::Lock arguments_lock_;
        gpu::SyncToken sync_token_;
        bool is_lost_;
        base::ThreadChecker main_thread_checker_;
        DISALLOW_COPY_AND_ASSIGN(TextureMailboxHolder);
    };

    // Used when mailbox names are specified instead of texture IDs.
    static scoped_refptr<TextureLayer> CreateForMailbox(
        const LayerSettings& settings,
        TextureLayerClient* client);

    // Resets the client, which also resets the texture.
    void ClearClient();

    // Resets the texture.
    void ClearTexture();

    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;

    // Sets whether this texture should be Y-flipped at draw time. Defaults to
    // true.
    void SetFlipped(bool flipped);
    bool flipped() const { return flipped_; }

    // Sets whether this texture should use nearest neighbor interpolation as
    // opposed to bilinear. Defaults to false.
    void SetNearestNeighbor(bool nearest_neighbor);

    // Sets a UV transform to be used at draw time. Defaults to (0, 0) and (1, 1).
    void SetUV(const gfx::PointF& top_left, const gfx::PointF& bottom_right);

    // Sets an opacity value per vertex. It will be multiplied by the layer
    // opacity value.
    void SetVertexOpacity(float bottom_left,
        float top_left,
        float top_right,
        float bottom_right);

    // Sets whether the alpha channel is premultiplied or unpremultiplied.
    // Defaults to true.
    void SetPremultipliedAlpha(bool premultiplied_alpha);

    // Sets whether the texture should be blended with the background color
    // at draw time. Defaults to false.
    void SetBlendBackgroundColor(bool blend);

    // Code path for plugins which supply their own mailbox.
    void SetTextureMailbox(const TextureMailbox& mailbox,
        scoped_ptr<SingleReleaseCallback> release_callback);

    // Use this for special cases where the same texture is used to back the
    // TextureLayer across all frames.
    // WARNING: DON'T ACTUALLY USE THIS WHAT YOU ARE DOING IS WRONG.
    // TODO(danakj): Remove this when pepper doesn't need it. crbug.com/350204
    void SetTextureMailboxWithoutReleaseCallback(const TextureMailbox& mailbox);

    void SetNeedsDisplayRect(const gfx::Rect& dirty_rect) override;

    void SetLayerTreeHost(LayerTreeHost* layer_tree_host) override;
    bool Update() override;
    void PushPropertiesTo(LayerImpl* layer) override;

protected:
    TextureLayer(const LayerSettings& settings, TextureLayerClient* client);
    ~TextureLayer() override;
    bool HasDrawableContent() const override;

private:
    void SetTextureMailboxInternal(
        const TextureMailbox& mailbox,
        scoped_ptr<SingleReleaseCallback> release_callback,
        bool requires_commit,
        bool allow_mailbox_reuse);

    TextureLayerClient* client_;

    bool flipped_;
    bool nearest_neighbor_;
    gfx::PointF uv_top_left_;
    gfx::PointF uv_bottom_right_;
    // [bottom left, top left, top right, bottom right]
    float vertex_opacity_[4];
    bool premultiplied_alpha_;
    bool blend_background_color_;

    scoped_ptr<TextureMailboxHolder::MainThreadReference> holder_ref_;
    bool needs_set_mailbox_;

    DISALLOW_COPY_AND_ASSIGN(TextureLayer);
};

} // namespace cc
#endif // CC_LAYERS_TEXTURE_LAYER_H_
