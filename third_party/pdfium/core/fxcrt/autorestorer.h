// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FXCRT_AUTORESTORER_H_
#define CORE_FXCRT_AUTORESTORER_H_

namespace fxcrt {

template <typename T>
class AutoRestorer {
 public:
  explicit AutoRestorer(T* location)
      : m_Location(location), m_OldValue(*location) {}
  ~AutoRestorer() {
    if (m_Location)
      *m_Location = m_OldValue;
  }
  void AbandonRestoration() { m_Location = nullptr; }

 private:
  T* m_Location;
  const T m_OldValue;
};

}  // namespace fxcrt

using fxcrt::AutoRestorer;

#endif  // CORE_FXCRT_AUTORESTORER_H_
