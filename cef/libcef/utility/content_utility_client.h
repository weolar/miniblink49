// Copyright (c) 2014 the Chromium Embedded Framework authors.
// Portions Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIBCEF_UTILITY_CONTENT_UTILITY_CLIENT_H_
#define LIBCEF_UTILITY_CONTENT_UTILITY_CLIENT_H_

#include "base/memory/scoped_vector.h"
#include "content/public/utility/content_utility_client.h"

class UtilityMessageHandler;

class CefContentUtilityClient : public content::ContentUtilityClient {
 public:
  CefContentUtilityClient();
  ~CefContentUtilityClient() override;

  bool OnMessageReceived(const IPC::Message& message) override;
  void RegisterMojoServices(content::ServiceRegistry* registry) override;

 private:
  // IPC message handlers.
  void OnStartupPing();

  typedef ScopedVector<UtilityMessageHandler> Handlers;
  Handlers handlers_;

  DISALLOW_COPY_AND_ASSIGN(CefContentUtilityClient);
};

#endif  // LIBCEF_UTILITY_CONTENT_UTILITY_CLIENT_H_
