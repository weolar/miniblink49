// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CoreTestHelpers_h
#define CoreTestHelpers_h

#include "core/dom/Position.h"
#include "wtf/testing/WTFTestHelpers.h"

namespace blink {

class Node;

std::ostream& operator<<(std::ostream&, const Node&);
std::ostream& operator<<(std::ostream&, const Node*);
std::ostream& operator<<(std::ostream&, PositionAnchorType);
std::ostream& operator<<(std::ostream&, const Position&);
std::ostream& operator<<(std::ostream&, const PositionInComposedTree&);

} // namespace blink

#endif // CoreTestHelpers_h
