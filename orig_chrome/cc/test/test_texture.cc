// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/test_texture.h"

#include "gpu/GLES2/gl2extchromium.h"
#include "third_party/khronos/GLES2/gl2ext.h"

namespace cc {

size_t TextureSizeBytes(const gfx::Size& size, ResourceFormat format)
{
    unsigned int components_per_pixel = 4;
    unsigned int bytes_per_component = 1;
    return size.width() * size.height() * components_per_pixel * bytes_per_component;
}

TestTexture::TestTexture()
    : format(RGBA_8888)
{
    // Initialize default parameter values.
    params[GL_TEXTURE_MAG_FILTER] = GL_LINEAR;
    params[GL_TEXTURE_MIN_FILTER] = GL_NEAREST_MIPMAP_LINEAR;
    params[GL_TEXTURE_WRAP_S] = GL_REPEAT;
    params[GL_TEXTURE_WRAP_T] = GL_REPEAT;
    params[GL_TEXTURE_POOL_CHROMIUM] = GL_TEXTURE_POOL_UNMANAGED_CHROMIUM;
    params[GL_TEXTURE_USAGE_ANGLE] = GL_NONE;
}

TestTexture::~TestTexture() { }

void TestTexture::Reallocate(const gfx::Size& size, ResourceFormat format)
{
    this->size = size;
    this->format = format;
    this->data.reset(new uint8_t[TextureSizeBytes(size, format)]);
}

bool TestTexture::IsValidParameter(GLenum pname)
{
    return params.find(pname) != params.end();
}

} // namespace cc
