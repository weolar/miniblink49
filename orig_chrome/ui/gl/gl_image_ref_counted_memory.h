// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_IMAGE_REF_COUNTED_MEMORY_H_
#define UI_GL_GL_IMAGE_REF_COUNTED_MEMORY_H_

#include "base/memory/ref_counted.h"
#include "ui/gl/gl_image_memory.h"

namespace base {
class RefCountedMemory;
}

namespace gl {

class GL_EXPORT GLImageRefCountedMemory : public gl::GLImageMemory {
public:
    GLImageRefCountedMemory(const gfx::Size& size, unsigned internalformat);

    bool Initialize(base::RefCountedMemory* ref_counted_memory,
        gfx::BufferFormat format);

    // Overridden from GLImage:
    void Destroy(bool have_context) override;
    void OnMemoryDump(base::trace_event::ProcessMemoryDump* pmd,
        uint64_t process_tracing_id,
        const std::string& dump_name) override;

protected:
    ~GLImageRefCountedMemory() override;

private:
    scoped_refptr<base::RefCountedMemory> ref_counted_memory_;

    DISALLOW_COPY_AND_ASSIGN(GLImageRefCountedMemory);
};

} // namespace gl

#endif // UI_GL_GL_IMAGE_REF_COUNTED_MEMORY_H_
