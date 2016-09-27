// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "modules/canvas2d/ClipList.h"

#include "platform/transforms/AffineTransform.h"
#include "third_party/skia/include/core/SkCanvas.h"

namespace blink {

ClipList::ClipList(const ClipList& other) : m_clipList(other.m_clipList) { }

void ClipList::clipPath(const SkPath& path, AntiAliasingMode antiAliasingMode, const SkMatrix& ctm)
{
    ClipOp newClip;
    newClip.m_antiAliasingMode = antiAliasingMode;
    newClip.m_path = path;
    newClip.m_path.transform(ctm);
    m_clipList.append(newClip);
}

void ClipList::playback(SkCanvas* canvas) const
{
    for (const ClipOp* it = m_clipList.begin(); it < m_clipList.end(); it++) {
        canvas->clipPath(it->m_path, SkRegion::kIntersect_Op, it->m_antiAliasingMode == AntiAliased);
    }
}

ClipList::ClipOp::ClipOp()
    : m_antiAliasingMode(AntiAliased)
{ }

ClipList::ClipOp::ClipOp(const ClipOp& other)
    : m_path(other.m_path)
    , m_antiAliasingMode(other.m_antiAliasingMode)
{ }

} // namespace blink
