// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CDM_SUPPORTED_CDM_VERSIONS_H_
#define MEDIA_CDM_SUPPORTED_CDM_VERSIONS_H_

#ifdef USE_PPAPI_CDM_ADAPTER
// When building the adapter these functions need to be local.
#define FUNCTION_EXPORT
#else
#include "media/base/media_export.h"
#define FUNCTION_EXPORT MEDIA_EXPORT
#endif

namespace media {

FUNCTION_EXPORT bool IsSupportedCdmModuleVersion(int version);

FUNCTION_EXPORT bool IsSupportedCdmInterfaceVersion(int version);

FUNCTION_EXPORT bool IsSupportedCdmHostVersion(int version);

} // namespace media

#undef FUNCTION_EXPORT

#endif // MEDIA_CDM_SUPPORTED_CDM_VERSIONS_H_
