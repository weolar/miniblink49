// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_PROGRAM_BINDING_H_
#define CC_OUTPUT_PROGRAM_BINDING_H_

#include <string>

#include "base/logging.h"
#include "cc/output/context_provider.h"
#include "cc/output/shader.h"

namespace gpu {
namespace gles2 {
    class GLES2Interface;
}
}

namespace cc {

class ProgramBindingBase {
public:
    ProgramBindingBase();
    ~ProgramBindingBase();

    bool Init(gpu::gles2::GLES2Interface* context,
        const std::string& vertex_shader,
        const std::string& fragment_shader);
    bool Link(gpu::gles2::GLES2Interface* context);
    void Cleanup(gpu::gles2::GLES2Interface* context);

    unsigned program() const { return program_; }
    bool initialized() const { return initialized_; }

protected:
    unsigned LoadShader(gpu::gles2::GLES2Interface* context,
        unsigned type,
        const std::string& shader_source);
    unsigned CreateShaderProgram(gpu::gles2::GLES2Interface* context,
        unsigned vertex_shader,
        unsigned fragment_shader);
    void CleanupShaders(gpu::gles2::GLES2Interface* context);

    bool IsContextLost(gpu::gles2::GLES2Interface* context);

    unsigned program_;
    unsigned vertex_shader_id_;
    unsigned fragment_shader_id_;
    bool initialized_;

private:
    DISALLOW_COPY_AND_ASSIGN(ProgramBindingBase);
};

template <class VertexShader, class FragmentShader>
class ProgramBinding : public ProgramBindingBase {
public:
    ProgramBinding() { }

    void Initialize(ContextProvider* context_provider,
        TexCoordPrecision precision,
        SamplerType sampler)
    {
        return Initialize(
            context_provider, precision, sampler, BLEND_MODE_NONE, false);
    }

    void Initialize(ContextProvider* context_provider,
        TexCoordPrecision precision,
        SamplerType sampler,
        BlendMode blend_mode)
    {
        return Initialize(
            context_provider, precision, sampler, blend_mode, false);
    }

    void Initialize(ContextProvider* context_provider,
        TexCoordPrecision precision,
        SamplerType sampler,
        BlendMode blend_mode,
        bool mask_for_background)
    {
        DCHECK(context_provider);
        DCHECK(!initialized_);

        if (IsContextLost(context_provider->ContextGL()))
            return;

        fragment_shader_.set_blend_mode(blend_mode);
        fragment_shader_.set_mask_for_background(mask_for_background);

        if (!ProgramBindingBase::Init(
                context_provider->ContextGL(),
                vertex_shader_.GetShaderString(),
                fragment_shader_.GetShaderString(precision, sampler))) {
            DCHECK(IsContextLost(context_provider->ContextGL()));
            return;
        }

        int base_uniform_index = 0;
        vertex_shader_.Init(context_provider->ContextGL(),
            program_, &base_uniform_index);
        fragment_shader_.Init(context_provider->ContextGL(),
            program_, &base_uniform_index);

        // Link after binding uniforms
        if (!Link(context_provider->ContextGL())) {
            DCHECK(IsContextLost(context_provider->ContextGL()));
            return;
        }

        initialized_ = true;
    }

    const VertexShader& vertex_shader() const { return vertex_shader_; }
    const FragmentShader& fragment_shader() const { return fragment_shader_; }

private:
    VertexShader vertex_shader_;
    FragmentShader fragment_shader_;

    DISALLOW_COPY_AND_ASSIGN(ProgramBinding);
};

} // namespace cc

#endif // CC_OUTPUT_PROGRAM_BINDING_H_
