// This file is generated

// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef protocol_Debugger_api_h
#define protocol_Debugger_api_h

#include "platform/inspector_protocol/Platform.h"
#include "platform/inspector_protocol/String16.h"

namespace blink {
namespace protocol {
namespace Debugger {
namespace API {

// ------------- Enums.

namespace Paused {
namespace ReasonEnum {
PLATFORM_EXPORT extern const char* XHR;
PLATFORM_EXPORT extern const char* DOM;
PLATFORM_EXPORT extern const char* EventListener;
PLATFORM_EXPORT extern const char* Exception;
PLATFORM_EXPORT extern const char* Assert;
PLATFORM_EXPORT extern const char* DebugCommand;
PLATFORM_EXPORT extern const char* PromiseRejection;
PLATFORM_EXPORT extern const char* Other;
} // ReasonEnum
} // Paused

// ------------- Types.

class PLATFORM_EXPORT SearchMatch {
public:
    virtual String16 toJSONString() const = 0;
    virtual ~SearchMatch() { }
    static std::unique_ptr<protocol::Debugger::API::SearchMatch> fromJSONString(const String16& json);
};

} // namespace API
} // namespace Debugger
} // namespace protocol
} // namespace blink

#endif // !defined(protocol_Debugger_api_h)
