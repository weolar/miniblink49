/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PictureSnapshot_h
#define PictureSnapshot_h

#include "platform/JSONValues.h"
#include "platform/PlatformExport.h"
#include "platform/graphics/GraphicsContext.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "wtf/RefCounted.h"

namespace blink {

class FloatRect;

class PLATFORM_EXPORT PictureSnapshot : public RefCounted<PictureSnapshot> {
WTF_MAKE_NONCOPYABLE(PictureSnapshot);
public:
    typedef Vector<Vector<double>> Timings;

    struct TilePictureStream : RefCounted<TilePictureStream> {
        FloatPoint layerOffset;
        Vector<char> data;
    };

    static PassRefPtr<PictureSnapshot> load(const Vector<RefPtr<TilePictureStream>>&);

    PictureSnapshot(PassRefPtr<const SkPicture>);

    PassOwnPtr<Vector<char>> replay(unsigned fromStep = 0, unsigned toStep = 0, double scale = 1.0) const;
    PassOwnPtr<Timings> profile(unsigned minIterations, double minDuration, const FloatRect* clipRect) const;
    PassRefPtr<JSONArray> snapshotCommandLog() const;
    bool isEmpty() const;

private:
    PassOwnPtr<SkBitmap> createBitmap() const;

    RefPtr<const SkPicture> m_picture;
};

} // namespace blink

#endif // PictureSnapshot_h
