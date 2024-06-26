// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_TEST_TEXTURE_H_
#define CC_TEST_TEST_TEXTURE_H_

#include "base/containers/hash_tables.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "cc/resources/resource_format.h"
#include "third_party/khronos/GLES2/gl2.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

size_t TextureSizeBytes(const gfx::Size& size, ResourceFormat format);

struct TestTexture : public base::RefCounted<TestTexture> {
    TestTexture();

    void Reallocate(const gfx::Size& size, ResourceFormat format);
    bool IsValidParameter(GLenum pname);

    gfx::Size size;
    ResourceFormat format;
    scoped_ptr<uint8_t[]> data;

    typedef base::hash_map<GLenum, GLint>
        TextureParametersMap;
    TextureParametersMap params;

private:
    friend class base::RefCounted<TestTexture>;
    ~TestTexture();
};

} // namespace cc

#endif // CC_TEST_TEST_TEXTURE_H_
