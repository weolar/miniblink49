// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/plugins/NavigatorPlugins.h"

#include "core/frame/LocalFrame.h"
#include "core/frame/Navigator.h"
#include "core/frame/Settings.h"
#include "modules/plugins/DOMMimeTypeArray.h"
#include "modules/plugins/DOMPluginArray.h"

namespace blink {

NavigatorPlugins::NavigatorPlugins(Navigator& navigator)
    : DOMWindowProperty(navigator.frame())
{
}

NavigatorPlugins::~NavigatorPlugins()
{
}

// static
NavigatorPlugins& NavigatorPlugins::from(Navigator& navigator)
{
    NavigatorPlugins* supplement = toNavigatorPlugins(navigator);
    if (!supplement) {
        supplement = new NavigatorPlugins(navigator);
        provideTo(navigator, supplementName(), supplement);
    }
    return *supplement;
}

// static
NavigatorPlugins* NavigatorPlugins::toNavigatorPlugins(Navigator& navigator)
{
    return static_cast<NavigatorPlugins*>(HeapSupplement<Navigator>::from(navigator, supplementName()));
}

// static
const char* NavigatorPlugins::supplementName()
{
    return "NavigatorPlugins";
}

// static
DOMPluginArray* NavigatorPlugins::plugins(Navigator& navigator)
{
    return NavigatorPlugins::from(navigator).plugins(navigator.frame());
}

// static
DOMMimeTypeArray* NavigatorPlugins::mimeTypes(Navigator& navigator)
{
    return NavigatorPlugins::from(navigator).mimeTypes(navigator.frame());
}

// static
bool NavigatorPlugins::javaEnabled(Navigator& navigator)
{
    return NavigatorPlugins::from(navigator).javaEnabled(navigator.frame());
}

DOMPluginArray* NavigatorPlugins::plugins(LocalFrame* frame) const
{
    if (!m_plugins)
        m_plugins = DOMPluginArray::create(frame);
    return m_plugins.get();
}

DOMMimeTypeArray* NavigatorPlugins::mimeTypes(LocalFrame* frame) const
{
    if (!m_mimeTypes)
        m_mimeTypes = DOMMimeTypeArray::create(frame);
    return m_mimeTypes.get();
}

bool NavigatorPlugins::javaEnabled(LocalFrame* frame) const
{
    if (!frame || !frame->settings())
        return false;

    return frame->settings()->javaEnabled();
}

DEFINE_TRACE(NavigatorPlugins)
{
    visitor->trace(m_plugins);
    visitor->trace(m_mimeTypes);
    HeapSupplement<Navigator>::trace(visitor);
    DOMWindowProperty::trace(visitor);
}

} // namespace blink
