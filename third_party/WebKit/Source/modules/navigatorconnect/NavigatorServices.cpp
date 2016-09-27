// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/navigatorconnect/NavigatorServices.h"

#include "core/frame/Navigator.h"
#include "modules/navigatorconnect/ServicePortCollection.h"

namespace blink {

NavigatorServices::~NavigatorServices()
{
}

NavigatorServices& NavigatorServices::from(Navigator& navigator)
{
    NavigatorServices* supplement = static_cast<NavigatorServices*>(HeapSupplement<Navigator>::from(navigator, supplementName()));
    if (!supplement) {
        supplement = new NavigatorServices();
        provideTo(navigator, supplementName(), supplement);
    }
    return *supplement;
}

ServicePortCollection* NavigatorServices::services(ExecutionContext* context, Navigator& navigator)
{
    return NavigatorServices::from(navigator).services(context);
}

ServicePortCollection* NavigatorServices::services(ExecutionContext* context)
{
    if (!m_services)
        m_services = ServicePortCollection::create(context);
    return m_services.get();
}

DEFINE_TRACE(NavigatorServices)
{
    visitor->trace(m_services);
    HeapSupplement<Navigator>::trace(visitor);
}

NavigatorServices::NavigatorServices()
{
}

const char* NavigatorServices::supplementName()
{
    return "NavigatorServices";
}

} // namespace blink
