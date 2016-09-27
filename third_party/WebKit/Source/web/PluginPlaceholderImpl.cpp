// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/PluginPlaceholderImpl.h"

#include "core/dom/shadow/ShadowRoot.h"
#include "core/html/HTMLPlugInElement.h"
#include "core/html/shadow/PluginPlaceholderElement.h"
#include "public/web/WebPluginPlaceholder.h"

namespace blink {

PluginPlaceholderImpl::PluginPlaceholderImpl(PassOwnPtr<WebPluginPlaceholder> webPluginPlaceholder, Document& document)
    : m_webPluginPlaceholder(webPluginPlaceholder)
    , m_placeholderElement(PluginPlaceholderElement::create(document))
{
    ASSERT(m_webPluginPlaceholder);
}

PluginPlaceholderImpl::~PluginPlaceholderImpl()
{
}

DEFINE_TRACE(PluginPlaceholderImpl)
{
    visitor->trace(m_placeholderElement);
}

void PluginPlaceholderImpl::loadIntoContainer(ContainerNode& container)
{
    m_placeholderElement->remove(ASSERT_NO_EXCEPTION);
    update();
    container.removeChildren();
    container.appendChild(m_placeholderElement);
}

void PluginPlaceholderImpl::update()
{
    m_placeholderElement->setMessage(m_webPluginPlaceholder->message());
    m_placeholderElement->setIsCloseable(m_webPluginPlaceholder->isCloseable());
}

} // namespace blink
