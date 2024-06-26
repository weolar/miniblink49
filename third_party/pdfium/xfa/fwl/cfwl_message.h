// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_MESSAGE_H_
#define XFA_FWL_CFWL_MESSAGE_H_

#include <memory>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"

class CFWL_Widget;

class CFWL_Message {
 public:
  enum class Type { Key, KillFocus, Mouse, MouseWheel, SetFocus };

  CFWL_Message(Type type, CFWL_Widget* pSrcTarget, CFWL_Widget* pDstTarget);
  virtual ~CFWL_Message();

  virtual std::unique_ptr<CFWL_Message> Clone() = 0;

  Type GetType() const { return m_type; }
  CFWL_Widget* GetSrcTarget() const { return m_pSrcTarget.Get(); }
  CFWL_Widget* GetDstTarget() const { return m_pDstTarget.Get(); }
  void SetSrcTarget(CFWL_Widget* pWidget) { m_pSrcTarget = pWidget; }
  void SetDstTarget(CFWL_Widget* pWidget) { m_pDstTarget = pWidget; }

 protected:
  CFWL_Message(const CFWL_Message& that);

 private:
  const Type m_type;
  UnownedPtr<CFWL_Widget> m_pSrcTarget;
  UnownedPtr<CFWL_Widget> m_pDstTarget;
};

#endif  // XFA_FWL_CFWL_MESSAGE_H_
