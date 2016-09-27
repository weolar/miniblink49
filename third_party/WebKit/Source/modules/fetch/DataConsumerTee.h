// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DataConsumerTee_h
#define DataConsumerTee_h

#include "modules/ModulesExport.h"
#include "modules/fetch/FetchDataConsumerHandle.h"
#include "public/platform/WebDataConsumerHandle.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class ExecutionContext;

class MODULES_EXPORT DataConsumerTee {
public:
    // Create two handles from one. |src| must be a valid unlocked handle.
    static void create(ExecutionContext*, PassOwnPtr<WebDataConsumerHandle> src, OwnPtr<WebDataConsumerHandle>* dest1, OwnPtr<WebDataConsumerHandle>* dest2);
    static void create(ExecutionContext*, PassOwnPtr<FetchDataConsumerHandle> src, OwnPtr<FetchDataConsumerHandle>* dest1, OwnPtr<FetchDataConsumerHandle>* dest2);
};

} // namespace blink

#endif // DataConsumerTee_h
