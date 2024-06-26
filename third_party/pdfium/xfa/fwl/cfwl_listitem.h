// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_LISTITEM_H_
#define XFA_FWL_CFWL_LISTITEM_H_

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"

class CFWL_ListItem {
 public:
  explicit CFWL_ListItem(const WideString& text);
  ~CFWL_ListItem();

  CFX_RectF GetRect() const { return m_rtItem; }
  void SetRect(const CFX_RectF& rect) { m_rtItem = rect; }

  uint32_t GetStates() const { return m_dwStates; }
  void SetStates(uint32_t dwStates) { m_dwStates = dwStates; }

  WideString GetText() const { return m_wsText; }

 private:
  uint32_t m_dwStates = 0;
  CFX_RectF m_rtItem;
  WideString m_wsText;
};

#endif  // XFA_FWL_CFWL_LISTITEM_H_
