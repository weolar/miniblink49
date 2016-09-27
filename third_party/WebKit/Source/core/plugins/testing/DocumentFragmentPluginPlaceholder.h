// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DocumentFragmentPluginPlaceholder_h
#define DocumentFragmentPluginPlaceholder_h

#include "core/dom/Document.h"
#include "core/dom/DocumentFragment.h"
#include "core/plugins/PluginPlaceholder.h"

namespace blink {

// Populates plugin placeholder content with a document fragment.
// Used for layout tests that contain a <template>.
class DocumentFragmentPluginPlaceholder : public NoBaseWillBeGarbageCollected<DocumentFragmentPluginPlaceholder>, public PluginPlaceholder {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DocumentFragmentPluginPlaceholder);
public:
    static PassOwnPtrWillBeRawPtr<DocumentFragmentPluginPlaceholder> create(PassRefPtrWillBeRawPtr<DocumentFragment> fragment)
    {
        return adoptPtrWillBeNoop(new DocumentFragmentPluginPlaceholder(fragment));
    }

#if !ENABLE(OILPAN)
    virtual ~DocumentFragmentPluginPlaceholder() override { }
#endif
    DEFINE_INLINE_VIRTUAL_TRACE() { visitor->trace(m_fragment); }

    virtual void loadIntoContainer(ContainerNode& container) override
    {
        RefPtrWillBeRawPtr<Node> clonedFragment = container.document().importNode(m_fragment.get(), true /* deep */, ASSERT_NO_EXCEPTION);
        container.appendChild(clonedFragment.release(), ASSERT_NO_EXCEPTION);
    }

private:
    DocumentFragmentPluginPlaceholder(PassRefPtrWillBeRawPtr<DocumentFragment> fragment) : m_fragment(fragment) { }

    RefPtrWillBeMember<DocumentFragment> m_fragment;
};

} // namespace blink

#endif // DocumentFragmentPluginPlaceholder_h
