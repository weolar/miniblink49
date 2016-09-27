// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebSelectionBound_h
#define WebSelectionBound_h

#include "public/platform/WebPoint.h"

namespace blink {

// An endpoint for an active selection region.
// TODO(jdduke): Move this to web/ after downstream code adopts |WebSelection|.
struct WebSelectionBound {
    // TODO(jdduke): Remove the type identifier after downstream code adopts
    // |WebSelection| for determining bound orientation.
    enum Type {
        Caret,
        SelectionLeft,
        SelectionRight
    };

    explicit WebSelectionBound(Type type)
        : type(type)
        , layerId(0)
        , isTextDirectionRTL(false)
    {
    }

    // The logical type of the endpoint. Note that this is dependent not only on
    // the bound's relative location, but also the underlying text direction.
    Type type;

    // The id of the platform layer to which the bound should be anchored.
    int layerId;

    // The bottom and top coordinates of the edge (caret), in layer coordinates,
    // that define the selection bound.
    WebPoint edgeTopInLayer;
    WebPoint edgeBottomInLayer;

    // Whether the text direction at this location is RTL.
    bool isTextDirectionRTL;
};

} // namespace blink

#endif
