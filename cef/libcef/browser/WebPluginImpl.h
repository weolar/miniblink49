// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_WEB_PLUGIN_IMPL_H_
#define CEF_LIBCEF_BROWSER_WEB_PLUGIN_IMPL_H_
#pragma once

#include "include/cef_web_plugin.h"

void CefVisitWebPluginInfo(CefRefPtr<CefWebPluginInfoVisitor> visitor);

void CefRefreshWebPlugins();

void CefAddWebPluginPath(const CefString& path);

void CefAddWebPluginDirectory(const CefString& dir);

void CefRemoveWebPluginPath(const CefString& path);

void CefUnregisterInternalWebPlugin(const CefString& path);

void CefForceWebPluginShutdown(const CefString& path);

void CefRegisterWebPluginCrash(const CefString& path);

#endif  // CEF_LIBCEF_BROWSER_WEB_PLUGIN_IMPL_H_
