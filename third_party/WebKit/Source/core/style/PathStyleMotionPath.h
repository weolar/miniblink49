// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PathStyleMotionPath_h
#define PathStyleMotionPath_h

#include "core/style/StyleMotionPath.h"

#include "platform/graphics/Path.h"
#include "wtf/Assertions.h"
#include "wtf/text/WTFString.h"

namespace blink {

class PathStyleMotionPath : public StyleMotionPath {
public:
    static PassRefPtr<PathStyleMotionPath> create(const String& pathString)
    {
        return adoptRef(new PathStyleMotionPath(pathString));
    }

    virtual bool isPathStyleMotionPath() const override { return true; }

    bool equals(const PathStyleMotionPath& other) const
    {
        return m_pathString == other.m_pathString;
    }

    const Path& path() const { return m_path; }

    const String& pathString() const { return m_pathString; }

    bool isClosed() const;

    float length() const { return m_length; }

private:
    PathStyleMotionPath(const String& pathString);

    String m_pathString;
    Path m_path;
    float m_length;
};

DEFINE_TYPE_CASTS(PathStyleMotionPath, StyleMotionPath, value, value->isPathStyleMotionPath(), value.isPathStyleMotionPath());

} // namespace blink

#endif // PathStyleMotionPath_h
