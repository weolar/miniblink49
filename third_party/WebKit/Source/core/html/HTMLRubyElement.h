// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTMLRubyElement_h
#define HTMLRubyElement_h

#include "core/html/HTMLElement.h"

namespace blink {

// <ruby> is an HTMLElement in script, but we use a separate interface here
// so HTMLElement's createLayoutObject doesn't need to know about it.
class HTMLRubyElement final : public HTMLElement {
public:
    DECLARE_NODE_FACTORY(HTMLRubyElement);

private:
    explicit HTMLRubyElement(Document&);

    LayoutObject* createLayoutObject(const ComputedStyle&) override;
};

} // namespace

#endif
