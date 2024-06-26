// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_TIMEZONEPROVIDER_H_
#define XFA_FXFA_PARSER_CXFA_TIMEZONEPROVIDER_H_

#include "core/fxcrt/cfx_datetime.h"

class CXFA_TimeZoneProvider {
 public:
  CXFA_TimeZoneProvider();
  ~CXFA_TimeZoneProvider();

  FX_TIMEZONE GetTimeZone() const { return m_tz; }

 private:
  FX_TIMEZONE m_tz;
};

#endif  // XFA_FXFA_PARSER_CXFA_TIMEZONEPROVIDER_H_
