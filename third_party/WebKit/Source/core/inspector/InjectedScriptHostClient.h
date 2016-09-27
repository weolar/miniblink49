// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InjectedScriptHostClient_h
#define InjectedScriptHostClient_h

namespace blink {

class InjectedScriptHostClient  {
public:
    virtual void muteWarningsAndDeprecations() { }
    virtual void unmuteWarningsAndDeprecations() { }
    virtual ~InjectedScriptHostClient() { }
};

} // namespace blink

#endif // InjectedScriptHostClient_h
