// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TextOffset_h
#define TextOffset_h

#include "public/platform/Platform.h"
#include "wtf/Forward.h"

namespace blink {

class Text;

class TextOffset {
    STACK_ALLOCATED();
public:
    TextOffset();
    TextOffset(PassRefPtrWillBeRawPtr<Text>, int);
    TextOffset(const TextOffset&);

    Text* text() const { return m_text.get(); }
    int offset() const { return m_offset; }

    bool isNull() const;
    bool isNotNull() const;

private:
    RefPtrWillBeMember<Text> m_text;
    int m_offset;
};

} // namespace blink

#endif // TextOffset_h
