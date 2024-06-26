// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CFXJSE_CLASS_H_
#define FXJS_XFA_CFXJSE_CLASS_H_

#include "core/fxcrt/unowned_ptr.h"
#include "fxjs/xfa/fxjse.h"
#include "v8/include/v8.h"

class CFXJSE_Context;
class CFXJSE_Value;
struct FXJSE_CLASS_DESCRIPTOR;

class CFXJSE_Class {
 public:
  static CFXJSE_Class* Create(CFXJSE_Context* pContext,
                              const FXJSE_CLASS_DESCRIPTOR* lpClassDefintion,
                              bool bIsJSGlobal);

  explicit CFXJSE_Class(CFXJSE_Context* lpContext);
  ~CFXJSE_Class();

  CFXJSE_Context* GetContext() const { return m_pContext.Get(); }
  v8::Global<v8::FunctionTemplate>& GetTemplate() { return m_hTemplate; }

 protected:
  friend class CFXJSE_Context;
  friend class CFXJSE_Value;

  ByteString m_szClassName;
  UnownedPtr<const FXJSE_CLASS_DESCRIPTOR> m_lpClassDefinition;
  UnownedPtr<CFXJSE_Context> const m_pContext;
  v8::Global<v8::FunctionTemplate> m_hTemplate;
};

#endif  // FXJS_XFA_CFXJSE_CLASS_H_
