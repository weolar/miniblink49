// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_FXJSE_H_
#define FXJS_XFA_FXJSE_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
//#include "v8/include/v8.h"

namespace pdfium {
namespace fxjse {

extern const char kFuncTag[];
extern const char kClassTag[];

}  // namespace fxjse
}  // namespace pdfium

class CFXJSE_Arguments;
class CFXJSE_FormCalcContext;
class CFXJSE_Value;
class CJS_Result;
class CXFA_Object;

// C++ object which is retrieved from v8 object's slot.
class CFXJSE_HostObject {
 public:
  virtual ~CFXJSE_HostObject();

  // Two subclasses.
  virtual CFXJSE_FormCalcContext* AsFormCalcContext();
  virtual CXFA_Object* AsCXFAObject();

 protected:
  CFXJSE_HostObject();
};

// weolar
// typedef CJS_Result (*FXJSE_MethodCallback)(
//     const v8::FunctionCallbackInfo<v8::Value>& info,
//     const WideString& functionName);
typedef void (*FXJSE_FuncCallback)(CFXJSE_Value* pThis,
                                   ByteStringView szFuncName,
                                   CFXJSE_Arguments& args);
typedef void (*FXJSE_PropAccessor)(CFXJSE_Value* pObject,
                                   ByteStringView szPropName,
                                   CFXJSE_Value* pValue);
typedef int32_t (*FXJSE_PropTypeGetter)(CFXJSE_Value* pObject,
                                        ByteStringView szPropName,
                                        bool bQueryIn);

enum FXJSE_ClassPropTypes {
  FXJSE_ClassPropType_None,
  FXJSE_ClassPropType_Property,
  FXJSE_ClassPropType_Method
};

struct FXJSE_FUNCTION_DESCRIPTOR {
  const char* tag;  // pdfium::kFuncTag always.
  const char* name;
  FXJSE_FuncCallback callbackProc;
};

struct FXJSE_CLASS_DESCRIPTOR {
  const char* tag;  // pdfium::kClassTag always.
  const char* name;
  const FXJSE_FUNCTION_DESCRIPTOR* methods;
  int32_t methNum;
  FXJSE_PropTypeGetter dynPropTypeGetter;
  FXJSE_PropAccessor dynPropGetter;
  FXJSE_PropAccessor dynPropSetter;
  //FXJSE_MethodCallback dynMethodCall;
};

extern const FXJSE_CLASS_DESCRIPTOR GlobalClassDescriptor;
extern const FXJSE_CLASS_DESCRIPTOR NormalClassDescriptor;
extern const FXJSE_CLASS_DESCRIPTOR VariablesClassDescriptor;
extern const FXJSE_CLASS_DESCRIPTOR kFormCalcFM2JSDescriptor;

void FXJSE_ThrowMessage(ByteStringView utf8Message);

#endif  // FXJS_XFA_FXJSE_H_
