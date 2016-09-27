// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/editing/TextOffset.h"

#include "core/dom/Text.h"

namespace blink {

TextOffset::TextOffset()
    : m_offset(0)
{
}

TextOffset::TextOffset(PassRefPtrWillBeRawPtr<Text> text, int offset)
    : m_text(text)
    , m_offset(offset)
{
}

TextOffset::TextOffset(const TextOffset& other)
    : m_text(other.m_text)
    , m_offset(other.m_offset)
{
}

bool TextOffset::isNull() const
{
    return !m_text;
}

bool TextOffset::isNotNull() const
{
    return m_text;
}

} // namespace blink
