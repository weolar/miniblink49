// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIBCEF_RENDERER_PEPPER_RENDERER_PEPPER_HOST_FACTORY_H_
#define LIBCEF_RENDERER_PEPPER_RENDERER_PEPPER_HOST_FACTORY_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "ppapi/host/host_factory.h"

namespace content {
class RendererPpapiHost;
}

class CefRendererPepperHostFactory : public ppapi::host::HostFactory {
 public:
  explicit CefRendererPepperHostFactory(content::RendererPpapiHost* host);
  ~CefRendererPepperHostFactory() override;

  // HostFactory.
  scoped_ptr<ppapi::host::ResourceHost> CreateResourceHost(
      ppapi::host::PpapiHost* host,
      PP_Resource resource,
      PP_Instance instance,
      const IPC::Message& message) override;

 private:
  // Not owned by this object.
  content::RendererPpapiHost* host_;

  DISALLOW_COPY_AND_ASSIGN(CefRendererPepperHostFactory);
};

#endif  // LIBCEF_RENDERER_PEPPER_RENDERER_PEPPER_HOST_FACTORY_H_
