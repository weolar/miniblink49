// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebHistoryCommitType_h
#define WebHistoryCommitType_h

namespace blink {

enum WebHistoryCommitType {
    // The default case (link clicks, user-typed urls, etc.), appends
    // a new history entry to the back/forward list.
    WebStandardCommit,
    // A load that originated from history, whether from the
    // back/forward list or session restore. The back/forward list is
    // not modified, but our position in the list is.
    WebBackForwardCommit,
    // Each history entry is represented by a tree, where each node
    // contains the history state of a single frame in the page. Initial
    // loads in a new subframe append a new node to that tree.
    WebInitialCommitInChildFrame,
    // Reloads, client redirects, etc. Loads that neither originate from
    // nor add entries to the back/forward list.
    WebHistoryInertCommit
};

} // namespace blink

#endif
