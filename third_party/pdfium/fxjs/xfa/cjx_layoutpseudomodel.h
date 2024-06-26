// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_LAYOUTPSEUDOMODEL_H_
#define FXJS_XFA_CJX_LAYOUTPSEUDOMODEL_H_

#include <vector>

#include "fxjs/xfa/cjx_object.h"
#include "fxjs/xfa/jse_define.h"

enum XFA_LAYOUTMODEL_HWXY {
  XFA_LAYOUTMODEL_H,
  XFA_LAYOUTMODEL_W,
  XFA_LAYOUTMODEL_X,
  XFA_LAYOUTMODEL_Y
};

class CFXJSE_Value;
class CScript_LayoutPseudoModel;
class CXFA_LayoutProcessor;
class CXFA_Node;

class CJX_LayoutPseudoModel final : public CJX_Object {
 public:
  explicit CJX_LayoutPseudoModel(CScript_LayoutPseudoModel* model);
  ~CJX_LayoutPseudoModel() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_METHOD(absPage);
  JSE_METHOD(absPageCount);
  JSE_METHOD(absPageCountInBatch);
  JSE_METHOD(absPageInBatch);
  JSE_METHOD(absPageSpan);
  JSE_METHOD(h);
  JSE_METHOD(page);
  JSE_METHOD(pageContent);
  JSE_METHOD(pageCount);
  JSE_METHOD(pageSpan);
  JSE_METHOD(relayout);
  JSE_METHOD(relayoutPageArea);
  JSE_METHOD(sheet);
  JSE_METHOD(sheetCount);
  JSE_METHOD(sheetCountInBatch);
  JSE_METHOD(sheetInBatch);
  JSE_METHOD(w);
  JSE_METHOD(x);
  JSE_METHOD(y);

  JSE_PROP(ready);

 private:
  using Type__ = CJX_LayoutPseudoModel;
  using ParentType__ = CJX_Object;

  static const TypeTag static_type__ = TypeTag::LayoutPseudoModel;
  static const CJX_MethodSpec MethodSpecs[];

  CJS_Result NumberedPageCount(CFX_V8* runtime, bool bNumbered);
  CJS_Result HWXY(CFX_V8* runtime,
                  const std::vector<v8::Local<v8::Value>>& params,
                  XFA_LAYOUTMODEL_HWXY layoutModel);
  std::vector<CXFA_Node*> GetObjArray(CXFA_LayoutProcessor* pDocLayout,
                                      int32_t iPageNo,
                                      const WideString& wsType,
                                      bool bOnPageArea);
  CJS_Result PageInternals(CFX_V8* runtime,
                           const std::vector<v8::Local<v8::Value>>& params,
                           bool bAbsPage);
};

#endif  // FXJS_XFA_CJX_LAYOUTPSEUDOMODEL_H_
