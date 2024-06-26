// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/program_binding.h"

#include "base/trace_event/trace_event.h"
#include "cc/output/geometry_binding.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "gpu/command_buffer/client/gles2_interface.h"

using gpu::gles2::GLES2Interface;

namespace cc {

ProgramBindingBase::ProgramBindingBase()
    : program_(0)
    , vertex_shader_id_(0)
    , fragment_shader_id_(0)
    , initialized_(false)
{
}

ProgramBindingBase::~ProgramBindingBase()
{
    // If you hit these asserts, you initialized but forgot to call Cleanup().
    DCHECK(!program_);
    DCHECK(!vertex_shader_id_);
    DCHECK(!fragment_shader_id_);
    DCHECK(!initialized_);
}

bool ProgramBindingBase::Init(GLES2Interface* context,
    const std::string& vertex_shader,
    const std::string& fragment_shader)
{
    TRACE_EVENT0("cc", "ProgramBindingBase::init");
    vertex_shader_id_ = LoadShader(context, GL_VERTEX_SHADER, vertex_shader);
    if (!vertex_shader_id_)
        return false;

    fragment_shader_id_ = LoadShader(context, GL_FRAGMENT_SHADER, fragment_shader);
    if (!fragment_shader_id_) {
        context->DeleteShader(vertex_shader_id_);
        vertex_shader_id_ = 0;
        return false;
    }

    program_ = CreateShaderProgram(context, vertex_shader_id_, fragment_shader_id_);
    return !!program_;
}

bool ProgramBindingBase::Link(GLES2Interface* context)
{
    context->LinkProgram(program_);
    CleanupShaders(context);
    if (!program_)
        return false;
#ifndef NDEBUG
    int linked = 0;
    context->GetProgramiv(program_, GL_LINK_STATUS, &linked);
    if (!linked)
        return false;
#endif
    return true;
}

void ProgramBindingBase::Cleanup(GLES2Interface* context)
{
    initialized_ = false;
    if (!program_)
        return;

    DCHECK(context);
    context->DeleteProgram(program_);
    program_ = 0;

    CleanupShaders(context);
}

unsigned ProgramBindingBase::LoadShader(GLES2Interface* context,
    unsigned type,
    const std::string& shader_source)
{
    unsigned shader = context->CreateShader(type);
    if (!shader)
        return 0u;

    const char* shader_source_str[] = { shader_source.data() };
    int shader_length[] = { static_cast<int>(shader_source.length()) };
    context->ShaderSource(
        shader, 1,
        shader_source_str,
        shader_length);
    context->CompileShader(shader);
#ifndef NDEBUG
    int compiled = 0;
    context->GetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
        return 0u;
#endif
    return shader;
}

unsigned ProgramBindingBase::CreateShaderProgram(GLES2Interface* context,
    unsigned vertex_shader,
    unsigned fragment_shader)
{
    unsigned program_object = context->CreateProgram();
    if (!program_object)
        return 0;

    context->AttachShader(program_object, vertex_shader);
    context->AttachShader(program_object, fragment_shader);

    // Bind the common attrib locations.
    context->BindAttribLocation(
        program_object, GeometryBinding::PositionAttribLocation(), "a_position");
    context->BindAttribLocation(
        program_object, GeometryBinding::TexCoordAttribLocation(), "a_texCoord");
    context->BindAttribLocation(program_object,
        GeometryBinding::TriangleIndexAttribLocation(),
        "a_index");

    return program_object;
}

void ProgramBindingBase::CleanupShaders(GLES2Interface* context)
{
    if (vertex_shader_id_) {
        context->DeleteShader(vertex_shader_id_);
        vertex_shader_id_ = 0;
    }
    if (fragment_shader_id_) {
        context->DeleteShader(fragment_shader_id_);
        fragment_shader_id_ = 0;
    }
}

bool ProgramBindingBase::IsContextLost(GLES2Interface* context)
{
    return context->GetGraphicsResetStatusKHR() != GL_NO_ERROR;
}

} // namespace cc
