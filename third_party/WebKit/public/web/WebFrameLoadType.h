// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebFrameLoadType_h
#define WebFrameLoadType_h


namespace blink {

// The type of load for a navigation.
// TODO(clamy): Return a WebFrameLoadType instead of a WebHistoryCommitType
// in DidCommitProvisionalLoad.
enum class WebFrameLoadType {
    Standard,
    BackForward,
    Reload,
    Same, // user loads same URL again (but not reload button)
    RedirectWithLockedBackForwardList,
    InitialInChildFrame,
    InitialHistoryLoad, // history navigation in a newly created frame
    ReloadFromOrigin, // reload bypassing cache
};

} // namespace blink

#endif // WebFrameLoadType_h

