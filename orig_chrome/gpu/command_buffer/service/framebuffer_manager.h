// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_FRAMEBUFFER_MANAGER_H_
#define GPU_COMMAND_BUFFER_SERVICE_FRAMEBUFFER_MANAGER_H_

#include <vector>

#include "base/basictypes.h"
#include "base/containers/hash_tables.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "gpu/command_buffer/service/context_group.h"
#include "gpu/command_buffer/service/gl_utils.h"
#include "gpu/gpu_export.h"

namespace gpu {
namespace gles2 {

    class FramebufferCompletenessCache;
    class FramebufferManager;
    class Renderbuffer;
    class RenderbufferManager;
    class Texture;
    class TextureRef;
    class TextureManager;

    // Info about a particular Framebuffer.
    class GPU_EXPORT Framebuffer : public base::RefCounted<Framebuffer> {
    public:
        class Attachment : public base::RefCounted<Attachment> {
        public:
            virtual GLsizei width() const = 0;
            virtual GLsizei height() const = 0;
            virtual GLenum internal_format() const = 0;
            virtual GLenum texture_type() const = 0;
            virtual GLsizei samples() const = 0;
            virtual GLuint object_name() const = 0;
            virtual bool cleared() const = 0;
            virtual void SetCleared(
                RenderbufferManager* renderbuffer_manager,
                TextureManager* texture_manager,
                bool cleared)
                = 0;
            virtual bool IsTexture(TextureRef* texture) const = 0;
            virtual bool IsRenderbuffer(
                Renderbuffer* renderbuffer) const = 0;
            virtual bool CanRenderTo() const = 0;
            virtual void DetachFromFramebuffer(Framebuffer* framebuffer) const = 0;
            virtual bool ValidForAttachmentType(
                GLenum attachment_type, uint32 max_color_attachments)
                = 0;
            virtual size_t GetSignatureSize(TextureManager* texture_manager) const = 0;
            virtual void AddToSignature(
                TextureManager* texture_manager, std::string* signature) const = 0;
            virtual bool FormsFeedbackLoop(TextureRef* texture, GLint level) const = 0;

        protected:
            friend class base::RefCounted<Attachment>;
            virtual ~Attachment() { }
        };

        Framebuffer(FramebufferManager* manager, GLuint service_id);

        GLuint service_id() const
        {
            return service_id_;
        }

        bool HasUnclearedAttachment(GLenum attachment) const;
        bool HasUnclearedColorAttachments() const;

        void MarkAttachmentAsCleared(
            RenderbufferManager* renderbuffer_manager,
            TextureManager* texture_manager,
            GLenum attachment,
            bool cleared);

        // Unbinds all attachments from this framebuffer for workaround
        // 'unbind_attachments_on_bound_render_fbo_delete'.  The Framebuffer must be
        // bound when calling this.
        void DoUnbindGLAttachmentsForWorkaround(GLenum target);

        // Attaches a renderbuffer to a particlar attachment.
        // Pass null to detach.
        void AttachRenderbuffer(
            GLenum attachment, Renderbuffer* renderbuffer);

        // Attaches a texture to a particlar attachment. Pass null to detach.
        void AttachTexture(
            GLenum attachment, TextureRef* texture_ref, GLenum target,
            GLint level, GLsizei samples);
        void AttachTextureLayer(
            GLenum attachment, TextureRef* texture_ref, GLenum target,
            GLint level, GLint layer);

        // Unbinds the given renderbuffer if it is bound.
        void UnbindRenderbuffer(
            GLenum target, Renderbuffer* renderbuffer);

        // Unbinds the given texture if it is bound.
        void UnbindTexture(
            GLenum target, TextureRef* texture_ref);

        const Attachment* GetAttachment(GLenum attachment) const;

        const Attachment* GetReadBufferAttachment() const;

        bool IsDeleted() const
        {
            return deleted_;
        }

        void MarkAsValid()
        {
            has_been_bound_ = true;
        }

        bool IsValid() const
        {
            return has_been_bound_ && !IsDeleted();
        }

        bool HasDepthAttachment() const;
        bool HasStencilAttachment() const;
        GLenum GetDrawBufferInternalFormat() const;
        GLenum GetReadBufferInternalFormat() const;
        // If the color attachment is a texture, returns its type; otherwise,
        // returns 0.
        GLenum GetReadBufferTextureType() const;

        // Verify all the rules in OpenGL ES 2.0.25 4.4.5 are followed.
        // Returns GL_FRAMEBUFFER_COMPLETE if there are no reasons we know we can't
        // use this combination of attachments. Otherwise returns the value
        // that glCheckFramebufferStatus should return for this set of attachments.
        // Note that receiving GL_FRAMEBUFFER_COMPLETE from this function does
        // not mean the real OpenGL will consider it framebuffer complete. It just
        // means it passed our tests.
        GLenum IsPossiblyComplete() const;

        // Implements optimized glGetFramebufferStatus.
        GLenum GetStatus(TextureManager* texture_manager, GLenum target) const;

        // Check all attachments are cleared
        bool IsCleared() const;

        GLenum GetDrawBuffer(GLenum draw_buffer) const;

        void SetDrawBuffers(GLsizei n, const GLenum* bufs);

        // If a color buffer is attached to GL_COLOR_ATTACHMENTi, enable that
        // draw buffer for glClear().
        void PrepareDrawBuffersForClear() const;

        // Restore draw buffers states that have been changed in
        // PrepareDrawBuffersForClear().
        void RestoreDrawBuffersAfterClear() const;

        // Clear all the active INT or UINT type color buffers to (0, 0, 0, 0).
        void ClearIntegerBuffers();

        // Return true if any draw buffers has an alpha channel.
        bool HasAlphaMRT() const;

        // Return false if any two active color attachments have different internal
        // formats.
        bool HasSameInternalFormatsMRT() const;

        void set_read_buffer(GLenum read_buffer)
        {
            read_buffer_ = read_buffer;
        }

        GLenum read_buffer() const
        {
            return read_buffer_;
        }

    private:
        friend class FramebufferManager;
        friend class base::RefCounted<Framebuffer>;

        ~Framebuffer();

        void MarkAsDeleted();

        void MarkAttachmentsAsCleared(
            RenderbufferManager* renderbuffer_manager,
            TextureManager* texture_manager,
            bool cleared);

        void MarkAsComplete(unsigned state_id)
        {
            framebuffer_complete_state_count_id_ = state_id;
        }

        unsigned framebuffer_complete_state_count_id() const
        {
            return framebuffer_complete_state_count_id_;
        }

        // Helper function for PrepareDrawBuffersForClear() and
        // RestoreDrawBuffersAfterClear().
        void ChangeDrawBuffersHelper(bool recover) const;

        // The managers that owns this.
        FramebufferManager* manager_;

        bool deleted_;

        // Service side framebuffer id.
        GLuint service_id_;

        // Whether this framebuffer has ever been bound.
        bool has_been_bound_;

        // state count when this framebuffer was last checked for completeness.
        unsigned framebuffer_complete_state_count_id_;

        // A map of attachments.
        typedef base::hash_map<GLenum, scoped_refptr<Attachment>> AttachmentMap;
        AttachmentMap attachments_;

        scoped_ptr<GLenum[]> draw_buffers_;

        GLenum read_buffer_;

        DISALLOW_COPY_AND_ASSIGN(Framebuffer);
    };

    struct DecoderFramebufferState {
        DecoderFramebufferState();
        ~DecoderFramebufferState();

        // State saved for clearing so we can clear render buffers and then
        // restore to these values.
        bool clear_state_dirty;

        // The currently bound framebuffers
        scoped_refptr<Framebuffer> bound_read_framebuffer;
        scoped_refptr<Framebuffer> bound_draw_framebuffer;
    };

    // This class keeps track of the frambebuffers and their attached renderbuffers
    // so we can correctly clear them.
    class GPU_EXPORT FramebufferManager {
    public:
        FramebufferManager(uint32 max_draw_buffers,
            uint32 max_color_attachments,
            ContextType context_type,
            const scoped_refptr<FramebufferCompletenessCache>&
                framebuffer_combo_complete_cache);
        ~FramebufferManager();

        // Must call before destruction.
        void Destroy(bool have_context);

        // Creates a Framebuffer for the given framebuffer.
        void CreateFramebuffer(GLuint client_id, GLuint service_id);

        // Gets the framebuffer info for the given framebuffer.
        Framebuffer* GetFramebuffer(GLuint client_id);

        // Removes a framebuffer info for the given framebuffer.
        void RemoveFramebuffer(GLuint client_id);

        // Gets a client id for a given service id.
        bool GetClientId(GLuint service_id, GLuint* client_id) const;

        void MarkAttachmentsAsCleared(
            Framebuffer* framebuffer,
            RenderbufferManager* renderbuffer_manager,
            TextureManager* texture_manager);

        void MarkAsComplete(Framebuffer* framebuffer);

        bool IsComplete(Framebuffer* framebuffer);

        void IncFramebufferStateChangeCount()
        {
            // make sure this is never 0.
            framebuffer_state_change_count_ = (framebuffer_state_change_count_ + 1) | 0x80000000U;
        }

        ContextType context_type() const { return context_type_; }

    private:
        friend class Framebuffer;

        void StartTracking(Framebuffer* framebuffer);
        void StopTracking(Framebuffer* framebuffer);

        FramebufferCompletenessCache* GetFramebufferComboCompleteCache()
        {
            return framebuffer_combo_complete_cache_.get();
        }

        // Info for each framebuffer in the system.
        typedef base::hash_map<GLuint, scoped_refptr<Framebuffer>>
            FramebufferMap;
        FramebufferMap framebuffers_;

        // Incremented anytime anything changes that might effect framebuffer
        // state.
        unsigned framebuffer_state_change_count_;

        // Counts the number of Framebuffer allocated with 'this' as its manager.
        // Allows to check no Framebuffer will outlive this.
        unsigned int framebuffer_count_;

        bool have_context_;

        uint32 max_draw_buffers_;
        uint32 max_color_attachments_;

        ContextType context_type_;

        scoped_refptr<FramebufferCompletenessCache> framebuffer_combo_complete_cache_;

        DISALLOW_COPY_AND_ASSIGN(FramebufferManager);
    };

} // namespace gles2
} // namespace gpu

#endif // GPU_COMMAND_BUFFER_SERVICE_FRAMEBUFFER_MANAGER_H_
