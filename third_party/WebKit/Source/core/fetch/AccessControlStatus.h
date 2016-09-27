// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AccessControlStatus_h
#define AccessControlStatus_h

namespace blink {

enum AccessControlStatus {
    NotSharableCrossOrigin,
    SharableCrossOrigin,
    OpaqueResource
};

} // namespace blink

#endif // AccessControlStatus_h
