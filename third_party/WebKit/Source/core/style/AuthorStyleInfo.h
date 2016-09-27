// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AuthorStyleInfo_h
#define AuthorStyleInfo_h

namespace blink {

// This class knows whether the author specified a background and/or border on
// some element. Some control elements use this information to figure out if the
// element is sufficiently styled by the author to disable the native look.
class AuthorStyleInfo {
public:
    AuthorStyleInfo() : AuthorStyleInfo(false, false) { }
    AuthorStyleInfo(bool specifiesBackground, bool specifiesBorder)
        : m_specifiesBackground(specifiesBackground)
        , m_specifiesBorder(specifiesBorder)
    {
    }

    bool specifiesBackground() const { return m_specifiesBackground; }
    bool specifiesBorder() const { return m_specifiesBorder; }

private:
    bool m_specifiesBackground;
    bool m_specifiesBorder;
};

} // namespace blink

#endif // AuthorStyleInfo_h
