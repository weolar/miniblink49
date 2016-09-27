// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/CompositorProxy.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/DOMNodeIds.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "platform/ThreadSafeFunctional.h"
#include "public/platform/Platform.h"
#include "public/platform/WebTraceLocation.h"

namespace blink {

struct AttributeFlagMapping {
    const char* name;
    unsigned length;
    CompositorProxy::Attributes attribute;
};

static AttributeFlagMapping allowedAttributes[] = {
    { "opacity", 7, CompositorProxy::Attributes::OPACITY },
    { "scrollleft", 10, CompositorProxy::Attributes::SCROLL_LEFT },
    { "scrolltop", 9, CompositorProxy::Attributes::SCROLL_TOP },
    { "touch", 5, CompositorProxy::Attributes::TOUCH },
    { "transform", 8, CompositorProxy::Attributes::TRANSFORM },
};

static bool CompareAttributeName(const AttributeFlagMapping& attribute, StringImpl* attributeLower)
{
    ASSERT(attributeLower->is8Bit());
    return memcmp(attribute.name, attributeLower->characters8(), std::min(attribute.length, attributeLower->length())) < 0;
}

static CompositorProxy::Attributes attributeFlagForName(const String& attributeName)
{
    CompositorProxy::Attributes attributeFlag = CompositorProxy::Attributes::NONE;
    const String attributeLower = attributeName.lower();
    const AttributeFlagMapping* start = allowedAttributes;
    const AttributeFlagMapping* end = allowedAttributes + WTF_ARRAY_LENGTH(allowedAttributes);
    if (attributeLower.impl()->is8Bit()) {
        const AttributeFlagMapping* match = std::lower_bound(start, end, attributeLower.impl(), CompareAttributeName);
        if (match != end)
            attributeFlag = match->attribute;
    }
    return attributeFlag;
}

static bool isControlThread()
{
    return !isMainThread();
}

static bool isCallingCompositorFrameCallback()
{
    // TODO(sad): Check that the requestCompositorFrame callbacks are currently being called.
    return true;
}

static void decrementCountForElement(uint64_t elementId)
{
    ASSERT(isMainThread());
    Node* node = DOMNodeIds::nodeForId(elementId);
    if (!node)
        return;
    Element* element = toElement(node);
    element->decrementProxyCount();
}

static void incrementProxyCountForElement(uint64_t elementId)
{
    ASSERT(isMainThread());
    Node* node = DOMNodeIds::nodeForId(elementId);
    if (!node)
        return;
    Element* element = toElement(node);
    element->incrementProxyCount();
}

static bool raiseExceptionIfMutationNotAllowed(ExceptionState& exceptionState)
{
    if (!isControlThread()) {
        exceptionState.throwDOMException(NoModificationAllowedError, "Cannot mutate a proxy attribute from the main page.");
        return true;
    }
    if (!isCallingCompositorFrameCallback()) {
        exceptionState.throwDOMException(NoModificationAllowedError, "Cannot mutate a proxy attribute outside of a requestCompositorFrame callback.");
        return true;
    }
    return false;
}

static uint32_t attributesBitfieldFromNames(const Vector<String>& attributeArray)
{
    uint32_t attributesBitfield = 0;
    for (const auto& attribute : attributeArray) {
        attributesBitfield |= static_cast<uint32_t>(attributeFlagForName(attribute));
    }
    return attributesBitfield;
}

#if ENABLE(ASSERT)
static bool sanityCheckAttributeFlags(uint32_t attributeFlags)
{
    uint32_t sanityCheckAttributes = attributeFlags;
    for (unsigned i = 0; i < arraysize(allowedAttributes); ++i) {
        sanityCheckAttributes &= ~static_cast<uint32_t>(allowedAttributes[i].attribute);
    }
    return !sanityCheckAttributes;
}
#endif

CompositorProxy* CompositorProxy::create(ExecutionContext* context, Element* element, const Vector<String>& attributeArray, ExceptionState& exceptionState)
{
    if (!context->isDocument()) {
        exceptionState.throwTypeError(ExceptionMessages::failedToConstruct("CompositorProxy", "Can only be created from the main context."));
        exceptionState.throwIfNeeded();
        return nullptr;
    }

    return new CompositorProxy(*element, attributeArray);
}

CompositorProxy* CompositorProxy::create(uint64_t elementId, uint32_t attributeFlags)
{
    return new CompositorProxy(elementId, attributeFlags);
}

CompositorProxy::CompositorProxy(Element& element, const Vector<String>& attributeArray)
    : m_elementId(DOMNodeIds::idForNode(&element))
    , m_bitfieldsSupported(attributesBitfieldFromNames(attributeArray))
{
    ASSERT(isMainThread());
    ASSERT(m_bitfieldsSupported);
    ASSERT(sanityCheckAttributeFlags(m_bitfieldsSupported));

    incrementProxyCountForElement(m_elementId);
}

CompositorProxy::CompositorProxy(uint64_t elementId, uint32_t attributeFlags)
    : m_elementId(elementId)
    , m_bitfieldsSupported(attributeFlags)
{
    ASSERT(isControlThread());
    ASSERT(sanityCheckAttributeFlags(m_bitfieldsSupported));
    Platform::current()->mainThread()->postTask(FROM_HERE, threadSafeBind(&incrementProxyCountForElement, m_elementId));
}

CompositorProxy::~CompositorProxy()
{
    if (m_connected)
        disconnect();
}

bool CompositorProxy::supports(const String& attributeName) const
{
    return !!(m_bitfieldsSupported & static_cast<uint32_t>(attributeFlagForName(attributeName)));
}

double CompositorProxy::opacity(ExceptionState& exceptionState) const
{
    if (raiseExceptionIfMutationNotAllowed(exceptionState))
        return 0.0;
    if (raiseExceptionIfNotMutable(Attributes::OPACITY, exceptionState))
        return 0.0;
    return m_opacity;
}

double CompositorProxy::scrollLeft(ExceptionState& exceptionState) const
{
    if (raiseExceptionIfMutationNotAllowed(exceptionState))
        return 0.0;
    if (raiseExceptionIfNotMutable(Attributes::SCROLL_LEFT, exceptionState))
        return 0.0;
    return m_scrollLeft;
}

double CompositorProxy::scrollTop(ExceptionState& exceptionState) const
{
    if (raiseExceptionIfMutationNotAllowed(exceptionState))
        return 0.0;
    if (raiseExceptionIfNotMutable(Attributes::SCROLL_TOP, exceptionState))
        return 0.0;
    return m_scrollTop;
}

DOMMatrix* CompositorProxy::transform(ExceptionState& exceptionState) const
{
    if (raiseExceptionIfMutationNotAllowed(exceptionState))
        return nullptr;
    if (raiseExceptionIfNotMutable(Attributes::TRANSFORM, exceptionState))
        return nullptr;
    return m_transform;
}

void CompositorProxy::setOpacity(double opacity, ExceptionState& exceptionState)
{
    if (raiseExceptionIfMutationNotAllowed(exceptionState))
        return;
    if (raiseExceptionIfNotMutable(Attributes::OPACITY, exceptionState))
        return;
    m_opacity = std::min(1., std::max(0., opacity));
    m_mutatedAttributes |= static_cast<uint32_t>(Attributes::OPACITY);
}

void CompositorProxy::setScrollLeft(double scrollLeft, ExceptionState& exceptionState)
{
    if (raiseExceptionIfMutationNotAllowed(exceptionState))
        return;
    if (raiseExceptionIfNotMutable(Attributes::SCROLL_LEFT, exceptionState))
        return;
    m_scrollLeft = scrollLeft;
    m_mutatedAttributes |= static_cast<uint32_t>(Attributes::SCROLL_LEFT);
}

void CompositorProxy::setScrollTop(double scrollTop, ExceptionState& exceptionState)
{
    if (raiseExceptionIfMutationNotAllowed(exceptionState))
        return;
    if (raiseExceptionIfNotMutable(Attributes::SCROLL_TOP, exceptionState))
        return;
    m_scrollTop = scrollTop;
    m_mutatedAttributes |= static_cast<uint32_t>(Attributes::SCROLL_TOP);
}

void CompositorProxy::setTransform(DOMMatrix* transform, ExceptionState& exceptionState)
{
    if (raiseExceptionIfMutationNotAllowed(exceptionState))
        return;
    if (raiseExceptionIfNotMutable(Attributes::TRANSFORM, exceptionState))
        return;
    m_transform = transform;
    m_mutatedAttributes |= static_cast<uint32_t>(Attributes::TRANSFORM);
}

bool CompositorProxy::raiseExceptionIfNotMutable(Attributes attribute, ExceptionState& exceptionState) const
{
    if (m_connected && (m_bitfieldsSupported & static_cast<uint32_t>(attribute)))
        return false;
    exceptionState.throwDOMException(NoModificationAllowedError,
        m_connected ? "Attempted to mutate non-mutable attribute." : "Attempted to mutate attribute on a disconnected proxy.");
    return true;
}

void CompositorProxy::disconnect()
{
    m_connected = false;
    if (isMainThread())
        decrementCountForElement(m_elementId);
    else
        Platform::current()->mainThread()->postTask(FROM_HERE, threadSafeBind(&decrementCountForElement, m_elementId));
}

} // namespace blink
