// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef WebTreeScopeType_h
#define WebTreeScopeType_h

namespace blink {

// Indicates if an item is in a document tree or in a shadow tree, per the
// Shadow DOM spec: https://w3c.github.io/webcomponents/spec/shadow/
enum class WebTreeScopeType {
    Document,
    Shadow,
    Last = Shadow,
};

} // namespace blink

#endif // WebTreeScopeType_h
