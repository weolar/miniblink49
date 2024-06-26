// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/raster/texture_compressor.h"

#include "base/logging.h"
#include "cc/raster/texture_compressor_etc1.h"

#if defined(ARCH_CPU_X86_FAMILY)
#include "base/cpu.h"
#include "cc/raster/texture_compressor_etc1_sse.h"
#endif

namespace cc {

scoped_ptr<TextureCompressor> TextureCompressor::Create(Format format)
{
    switch (format) {
    case kFormatETC1: {
#if defined(ARCH_CPU_X86_FAMILY)
        base::CPU cpu;
        if (cpu.has_sse2()) {
            return make_scoped_ptr(new TextureCompressorETC1SSE());
        }
#endif
        return make_scoped_ptr(new TextureCompressorETC1());
    }
    }

    NOTREACHED();
    return nullptr;
}

} // namespace cc
