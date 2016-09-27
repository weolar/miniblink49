// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/shadow/PluginPlaceholderElement.h"

#include "bindings/core/v8/V8PluginPlaceholderElement.h"
#include "core/dom/Document.h"
#include "wtf/Assertions.h"

namespace blink {

PluginPlaceholderElement::PluginPlaceholderElement(Document& document)
    : HTMLDivElement(document)
{
}

PassRefPtrWillBeRawPtr<PluginPlaceholderElement> PluginPlaceholderElement::create(Document& document)
{
    RefPtrWillBeRawPtr<PluginPlaceholderElement> element = adoptRefWillBeNoop(new PluginPlaceholderElement(document));
    bool success = V8PluginPlaceholderElement::PrivateScript::createdCallbackMethod(document.frame(), element.get());
    ASSERT_UNUSED(success, success);
    return element.release();
}

void PluginPlaceholderElement::setMessage(const String& message)
{
    bool success = V8PluginPlaceholderElement::PrivateScript::messageAttributeSetter(document().frame(), this, message);
    ASSERT_UNUSED(success, success);
}

void PluginPlaceholderElement::setIsCloseable(bool closeable)
{
    bool success = V8PluginPlaceholderElement::PrivateScript::closeableAttributeSetter(document().frame(), this, closeable);
    ASSERT_UNUSED(success, success);
}

} // namespace blink
