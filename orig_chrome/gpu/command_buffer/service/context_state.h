// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains the ContextState class.

#ifndef GPU_COMMAND_BUFFER_SERVICE_CONTEXT_STATE_H_
#define GPU_COMMAND_BUFFER_SERVICE_CONTEXT_STATE_H_

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "gpu/command_buffer/service/gl_utils.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "gpu/command_buffer/service/valuebuffer_manager.h"
#include "gpu/command_buffer/service/vertex_array_manager.h"
#include "gpu/command_buffer/service/vertex_attrib_manager.h"
#include "gpu/gpu_export.h"
#include <vector>

namespace gpu {
namespace gles2 {

    class Buffer;
    class ErrorState;
    class ErrorStateClient;
    class FeatureInfo;
    class Framebuffer;
    class Logger;
    class Program;
    class Renderbuffer;

    // State associated with each texture unit.
    struct GPU_EXPORT TextureUnit {
        TextureUnit();
        ~TextureUnit();

        // The last target that was bound to this texture unit.
        GLenum bind_target;

        // texture currently bound to this unit's GL_TEXTURE_2D with glBindTexture
        scoped_refptr<TextureRef> bound_texture_2d;

        // texture currently bound to this unit's GL_TEXTURE_CUBE_MAP with
        // glBindTexture
        scoped_refptr<TextureRef> bound_texture_cube_map;

        // texture currently bound to this unit's GL_TEXTURE_EXTERNAL_OES with
        // glBindTexture
        scoped_refptr<TextureRef> bound_texture_external_oes;

        // texture currently bound to this unit's GL_TEXTURE_RECTANGLE_ARB with
        // glBindTexture
        scoped_refptr<TextureRef> bound_texture_rectangle_arb;

        // texture currently bound to this unit's GL_TEXTURE_3D with glBindTexture
        scoped_refptr<TextureRef> bound_texture_3d;

        // texture currently bound to this unit's GL_TEXTURE_2D_ARRAY with
        // glBindTexture
        scoped_refptr<TextureRef> bound_texture_2d_array;

        scoped_refptr<TextureRef> GetInfoForSamplerType(
            GLenum type)
        {
            DCHECK(type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE || type == GL_SAMPLER_EXTERNAL_OES || type == GL_SAMPLER_2D_RECT_ARB);
            switch (type) {
            case GL_SAMPLER_2D:
                return bound_texture_2d;
            case GL_SAMPLER_CUBE:
                return bound_texture_cube_map;
            case GL_SAMPLER_EXTERNAL_OES:
                return bound_texture_external_oes;
            case GL_SAMPLER_2D_RECT_ARB:
                return bound_texture_rectangle_arb;
            case GL_SAMPLER_3D:
                return bound_texture_3d;
            case GL_SAMPLER_2D_ARRAY:
                return bound_texture_2d_array;
            }

            NOTREACHED();
            return NULL;
        }
    };

    class GPU_EXPORT Vec4 {
    public:
        enum DataType {
            kFloat,
            kInt,
            kUInt,
        };

        Vec4()
        {
            v_[0].float_value = 0.0f;
            v_[1].float_value = 0.0f;
            v_[2].float_value = 0.0f;
            v_[3].float_value = 1.0f;
            type_ = kFloat;
        }

        template <typename T>
        void GetValues(T* values) const;

        template <typename T>
        void SetValues(const T* values);

        DataType type() const
        {
            return type_;
        }

        bool Equal(const Vec4& other) const;

    private:
        union ValueUnion {
            GLfloat float_value;
            GLint int_value;
            GLuint uint_value;
        };

        ValueUnion v_[4];
        DataType type_;
    };

    template <>
    GPU_EXPORT void Vec4::GetValues<GLfloat>(GLfloat* values) const;
    template <>
    GPU_EXPORT void Vec4::GetValues<GLint>(GLint* values) const;
    template <>
    GPU_EXPORT void Vec4::GetValues<GLuint>(GLuint* values) const;

    template <>
    GPU_EXPORT void Vec4::SetValues<GLfloat>(const GLfloat* values);
    template <>
    GPU_EXPORT void Vec4::SetValues<GLint>(const GLint* values);
    template <>
    GPU_EXPORT void Vec4::SetValues<GLuint>(const GLuint* values);

    struct GPU_EXPORT ContextState {
        ContextState(FeatureInfo* feature_info,
            ErrorStateClient* error_state_client,
            Logger* logger);
        ~ContextState();

        void Initialize();

        void SetIgnoreCachedStateForTest(bool ignore)
        {
            ignore_cached_state = ignore;
        }

        void RestoreState(const ContextState* prev_state);
        void InitCapabilities(const ContextState* prev_state) const;
        void InitState(const ContextState* prev_state) const;

        void RestoreActiveTexture() const;
        void RestoreAllTextureUnitBindings(const ContextState* prev_state) const;
        void RestoreActiveTextureUnitBinding(unsigned int target) const;
        void RestoreVertexAttribValues() const;
        void RestoreVertexAttribArrays(
            const scoped_refptr<VertexAttribManager> attrib_manager) const;
        void RestoreVertexAttribs() const;
        void RestoreBufferBindings() const;
        void RestoreGlobalState(const ContextState* prev_state) const;
        void RestoreProgramBindings() const;
        void RestoreRenderbufferBindings();
        void RestoreTextureUnitBindings(
            GLuint unit, const ContextState* prev_state) const;

        // Helper for getting cached state.
        bool GetStateAsGLint(
            GLenum pname, GLint* params, GLsizei* num_written) const;
        bool GetStateAsGLfloat(
            GLenum pname, GLfloat* params, GLsizei* num_written) const;
        bool GetEnabled(GLenum cap) const;

        inline void SetDeviceColorMask(GLboolean red,
            GLboolean green,
            GLboolean blue,
            GLboolean alpha)
        {
            if (cached_color_mask_red == red && cached_color_mask_green == green && cached_color_mask_blue == blue && cached_color_mask_alpha == alpha && !ignore_cached_state)
                return;
            cached_color_mask_red = red;
            cached_color_mask_green = green;
            cached_color_mask_blue = blue;
            cached_color_mask_alpha = alpha;
            glColorMask(red, green, blue, alpha);
        }

        inline void SetDeviceDepthMask(GLboolean mask)
        {
            if (cached_depth_mask == mask && !ignore_cached_state)
                return;
            cached_depth_mask = mask;
            glDepthMask(mask);
        }

        inline void SetDeviceStencilMaskSeparate(GLenum op, GLuint mask)
        {
            if (op == GL_FRONT) {
                if (cached_stencil_front_writemask == mask && !ignore_cached_state)
                    return;
                cached_stencil_front_writemask = mask;
            } else if (op == GL_BACK) {
                if (cached_stencil_back_writemask == mask && !ignore_cached_state)
                    return;
                cached_stencil_back_writemask = mask;
            } else {
                NOTREACHED();
                return;
            }
            glStencilMaskSeparate(op, mask);
        }

        ErrorState* GetErrorState();

        void SetBoundBuffer(GLenum target, Buffer* buffer);
        void RemoveBoundBuffer(Buffer* buffer);

        void UnbindTexture(TextureRef* texture);

#include "gpu/command_buffer/service/context_state_autogen.h"

        EnableFlags enable_flags;

        // Current active texture by 0 - n index.
        // In other words, if we call glActiveTexture(GL_TEXTURE2) this value would
        // be 2.
        GLuint active_texture_unit;

        // The currently bound array buffer. If this is 0 it is illegal to call
        // glVertexAttribPointer.
        scoped_refptr<Buffer> bound_array_buffer;

        scoped_refptr<Buffer> bound_copy_read_buffer;
        scoped_refptr<Buffer> bound_copy_write_buffer;
        scoped_refptr<Buffer> bound_pixel_pack_buffer;
        scoped_refptr<Buffer> bound_pixel_unpack_buffer;
        scoped_refptr<Buffer> bound_transform_feedback_buffer;
        scoped_refptr<Buffer> bound_uniform_buffer;

        // Which textures are bound to texture units through glActiveTexture.
        std::vector<TextureUnit> texture_units;

        // The values for each attrib.
        std::vector<Vec4> attrib_values;

        // Class that manages vertex attribs.
        scoped_refptr<VertexAttribManager> vertex_attrib_manager;
        scoped_refptr<VertexAttribManager> default_vertex_attrib_manager;

        // The program in use by glUseProgram
        scoped_refptr<Program> current_program;

        // The currently bound renderbuffer
        scoped_refptr<Renderbuffer> bound_renderbuffer;
        bool bound_renderbuffer_valid;

        // The currently bound valuebuffer
        scoped_refptr<Valuebuffer> bound_valuebuffer;

        bool pack_reverse_row_order;
        bool ignore_cached_state;

        mutable bool fbo_binding_for_scissor_workaround_dirty;

    private:
        void EnableDisable(GLenum pname, bool enable) const;

        FeatureInfo* feature_info_;
        scoped_ptr<ErrorState> error_state_;
    };

} // namespace gles2
} // namespace gpu

#endif // GPU_COMMAND_BUFFER_SERVICE_CONTEXT_STATE_H_
