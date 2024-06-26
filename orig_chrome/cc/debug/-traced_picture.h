// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_TRACED_PICTURE_H_
#define CC_DEBUG_TRACED_PICTURE_H_

#include <string>

#include "base/memory/scoped_ptr.h"
#include "base/trace_event/trace_event.h"
#include "cc/playback/picture.h"

namespace cc {

class TracedPicture : public base::trace_event::ConvertableToTraceFormat {
public:
    explicit TracedPicture(scoped_refptr<const Picture>);

    static scoped_refptr<base::trace_event::ConvertableToTraceFormat>
    AsTraceablePicture(const Picture* picture);

    static scoped_refptr<base::trace_event::ConvertableToTraceFormat>
    AsTraceablePictureAlias(const Picture* original);

    void AppendAsTraceFormat(std::string* out) const override;

private:
    ~TracedPicture() override;

    void AppendPicture(std::string* out) const;
    void AppendPictureAlias(std::string* out) const;

    scoped_refptr<const Picture> picture_;
    bool is_alias_;

    DISALLOW_COPY_AND_ASSIGN(TracedPicture);
};

} // namespace cc

#endif // CC_DEBUG_TRACED_PICTURE_H_
