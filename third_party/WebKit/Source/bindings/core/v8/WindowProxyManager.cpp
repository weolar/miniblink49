// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/WindowProxyManager.h"

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/WindowProxy.h"
#include "core/frame/Frame.h"

namespace blink {

PassOwnPtrWillBeRawPtr<WindowProxyManager> WindowProxyManager::create(Frame& frame)
{
    return adoptPtrWillBeNoop(new WindowProxyManager(frame));
}

WindowProxyManager::~WindowProxyManager()
{
}

DEFINE_TRACE(WindowProxyManager)
{
#if ENABLE(OILPAN)
    visitor->trace(m_frame);
    visitor->trace(m_windowProxy);
    visitor->trace(m_isolatedWorlds);
#endif
}

WindowProxy* WindowProxyManager::windowProxy(DOMWrapperWorld& world)
{
    WindowProxy* windowProxy = nullptr;
    if (world.isMainWorld()) {
        windowProxy = m_windowProxy.get();
    } else {
        IsolatedWorldMap::iterator iter = m_isolatedWorlds.find(world.worldId());
        if (iter != m_isolatedWorlds.end()) {
            windowProxy = iter->value.get();
        } else {
            OwnPtrWillBeRawPtr<WindowProxy> isolatedWorldWindowProxy = WindowProxy::create(m_isolate, m_frame, world);
            windowProxy = isolatedWorldWindowProxy.get();
            m_isolatedWorlds.set(world.worldId(), isolatedWorldWindowProxy.release());
        }
    }
    return windowProxy;
}

void WindowProxyManager::clearForClose()
{
    m_windowProxy->clearForClose();
    for (auto& entry : m_isolatedWorlds)
        entry.value->clearForClose();
}

void WindowProxyManager::clearForNavigation()
{
    m_windowProxy->clearForNavigation();
    for (auto& entry : m_isolatedWorlds)
        entry.value->clearForNavigation();
}

WindowProxy* WindowProxyManager::existingWindowProxy(DOMWrapperWorld& world)
{
    if (world.isMainWorld())
        return m_windowProxy->isContextInitialized() ? m_windowProxy.get() : nullptr;

    IsolatedWorldMap::iterator iter = m_isolatedWorlds.find(world.worldId());
    if (iter == m_isolatedWorlds.end())
        return nullptr;
    return iter->value->isContextInitialized() ? iter->value.get() : nullptr;
}

void WindowProxyManager::collectIsolatedContexts(Vector<std::pair<ScriptState*, SecurityOrigin*>>& result)
{
    for (auto& entry : m_isolatedWorlds) {
        WindowProxy* isolatedWorldWindowProxy = entry.value.get();
        SecurityOrigin* origin = isolatedWorldWindowProxy->world().isolatedWorldSecurityOrigin();
        if (!isolatedWorldWindowProxy->isContextInitialized())
            continue;
        result.append(std::make_pair(isolatedWorldWindowProxy->scriptState(), origin));
    }
}

void WindowProxyManager::takeGlobalFrom(WindowProxyManager* other)
{
    m_windowProxy->takeGlobalFrom(other->m_windowProxy.get());
    for (auto& entry : other->m_isolatedWorlds)
        windowProxy(entry.value->world())->takeGlobalFrom(entry.value.get());
}

WindowProxyManager::WindowProxyManager(Frame& frame)
    : m_frame(&frame)
    , m_isolate(v8::Isolate::GetCurrent())
    , m_windowProxy(WindowProxy::create(m_isolate, &frame, DOMWrapperWorld::mainWorld()))
{
}

} // namespace blink
