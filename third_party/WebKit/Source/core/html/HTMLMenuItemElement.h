// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTMLMenuItemElement_h
#define HTMLMenuItemElement_h

#include "core/html/HTMLElement.h"

namespace blink {

class HTMLMenuItemElement final : public HTMLElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(HTMLMenuItemElement);

private:
    explicit HTMLMenuItemElement(Document&);
    void defaultEventHandler(Event*) override;
};

} // namespace blink

#endif // HTMLMenuItemElement_h
