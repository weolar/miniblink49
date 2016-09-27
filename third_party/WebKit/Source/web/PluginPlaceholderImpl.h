// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PluginPlaceholderImpl_h
#define PluginPlaceholderImpl_h

#include "core/plugins/PluginPlaceholder.h"
#include "platform/heap/Handle.h"
#include "platform/heap/Visitor.h"
#include "wtf/Assertions.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class Document;
class PluginPlaceholderElement;
class WebPluginPlaceholder;

// Populates a plugin placeholder element with content supplied by the embedder.
// Intended to be loaded into a user agent shadow root, but will accept any
// container. Owns a WebPluginPlaceholder instance.
class PluginPlaceholderImpl : public NoBaseWillBeGarbageCollectedFinalized<PluginPlaceholderImpl>, public PluginPlaceholder {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(PluginPlaceholderImpl);
public:
    static PassOwnPtrWillBeRawPtr<PluginPlaceholderImpl> create(PassOwnPtr<WebPluginPlaceholder> webPluginPlaceholder, Document& document)
    {
        ASSERT(webPluginPlaceholder);
        return adoptPtrWillBeNoop(new PluginPlaceholderImpl(webPluginPlaceholder, document));
    }

#if ENABLE(OILPAN)
    ~PluginPlaceholderImpl();
#else
    ~PluginPlaceholderImpl() override;
#endif

    DECLARE_VIRTUAL_TRACE();

    // PluginPlaceholder methods
    void loadIntoContainer(ContainerNode&) override;

    // Visible for testing.
    WebPluginPlaceholder* webPluginPlaceholder() const { return m_webPluginPlaceholder.get(); }

private:
    PluginPlaceholderImpl(PassOwnPtr<WebPluginPlaceholder>, Document&);

    // Update the placeholder element with fresh content.
    void update();

    OwnPtr<WebPluginPlaceholder> m_webPluginPlaceholder;
    RefPtrWillBeMember<PluginPlaceholderElement> m_placeholderElement;
};

} // namespace blink

#endif // PluginPlaceholderImpl_h
