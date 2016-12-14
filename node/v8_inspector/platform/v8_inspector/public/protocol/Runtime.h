// This file is generated

// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef protocol_Runtime_api_h
#define protocol_Runtime_api_h

#include "platform/inspector_protocol/Platform.h"
#include "platform/inspector_protocol/String16.h"

namespace blink {
namespace protocol {
namespace Runtime {
namespace API {

// ------------- Enums.

// ------------- Types.

class PLATFORM_EXPORT RemoteObject {
public:
    virtual String16 toJSONString() const = 0;
    virtual ~RemoteObject() { }
    static std::unique_ptr<protocol::Runtime::API::RemoteObject> fromJSONString(const String16& json);
};

class PLATFORM_EXPORT StackTrace {
public:
    virtual String16 toJSONString() const = 0;
    virtual ~StackTrace() { }
    static std::unique_ptr<protocol::Runtime::API::StackTrace> fromJSONString(const String16& json);
};

} // namespace API
} // namespace Runtime
} // namespace protocol
} // namespace blink

#endif // !defined(protocol_Runtime_api_h)
