// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/public/wrapper_info.h"

namespace gin {

WrapperInfo* WrapperInfo::From(v8::Local<v8::Object> object) {
  int count = object->InternalFieldCount();
  if (count != kNumberOfInternalFields)
    return NULL;
  WrapperInfo* info = static_cast<WrapperInfo*>(
      object->GetAlignedPointerFromInternalField(kWrapperInfoIndex));
  return info->embedder == kEmbedderNativeGin ? info : NULL;
}

}  // namespace gin
