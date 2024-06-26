// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

// CFXJS_ENGINE is a layer that makes it easier to define native objects in V8,
// but has no knowledge of PDF-specific native objects. It could in theory be
// used to implement other sets of native objects.

// PDFium code should include this file rather than including V8 headers
// directly.

#ifndef FXJS_CFXJS_ENGINE_H_
#define FXJS_CFXJS_ENGINE_H_

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "core/fxcrt/fx_string.h"
#include "fxjs/cfx_v8.h"
#include "fxjs/ijs_runtime.h"
#include "v8/include/v8.h"

#ifdef PDF_ENABLE_XFA
// CFXJS_ENGINE doesn't interpret this class, it is just passed along to XFA.
class CFXJSE_RuntimeData;
#endif  // PDF_ENABLE_XFA

class CFXJS_ObjDefinition;
class CJS_Object;
class V8TemplateMap;

// CFXJS_ENGINE places no restrictions on this class; it merely passes it
// on to caller-provided methods.
class IJS_EventContext;  // A description of the event that caused JS execution.

enum FXJSOBJTYPE {
  FXJSOBJTYPE_DYNAMIC = 0,  // Created by native method and returned to JS.
  FXJSOBJTYPE_STATIC,       // Created by init and hung off of global object.
  FXJSOBJTYPE_GLOBAL,       // The global object itself (may only appear once).
};

class FXJS_PerIsolateData {
 public:
  ~FXJS_PerIsolateData();

  static void SetUp(v8::Isolate* pIsolate);
  static FXJS_PerIsolateData* Get(v8::Isolate* pIsolate);

  int MaxObjDefinitionID() const;
  CFXJS_ObjDefinition* ObjDefinitionForID(int id) const;
  int AssignIDForObjDefinition(std::unique_ptr<CFXJS_ObjDefinition> pDefn);

  std::vector<std::unique_ptr<CFXJS_ObjDefinition>> m_ObjectDefnArray;
  std::unique_ptr<V8TemplateMap> m_pDynamicObjsMap;
#ifdef PDF_ENABLE_XFA
  std::unique_ptr<CFXJSE_RuntimeData> m_pFXJSERuntimeData;
#endif  // PDF_ENABLE_XFA

 protected:
  explicit FXJS_PerIsolateData(v8::Isolate* pIsolate);
};

void FXJS_Initialize(unsigned int embedderDataSlot, v8::Isolate* pIsolate);
void FXJS_Release();

// Gets the global isolate set by FXJS_Initialize(), or makes a new one each
// time if there is no such isolate. Returns true if a new isolate had to be
// created.
bool FXJS_GetIsolate(v8::Isolate** pResultIsolate);

// Get the global isolate's ref count.
size_t FXJS_GlobalIsolateRefCount();

class CFXJS_Engine : public CFX_V8 {
 public:
  explicit CFXJS_Engine(v8::Isolate* pIsolate);
  ~CFXJS_Engine() override;

  using Constructor =
      std::function<void(CFXJS_Engine* pEngine, v8::Local<v8::Object> obj)>;
  using Destructor = std::function<void(v8::Local<v8::Object> obj)>;

  static int GetObjDefnID(v8::Local<v8::Object> pObj);
  static CJS_Object* GetObjectPrivate(v8::Local<v8::Object> pObj);
  static void SetObjectPrivate(v8::Local<v8::Object> pObj,
                               std::unique_ptr<CJS_Object> p);
  static void FreeObjectPrivate(v8::Local<v8::Object> pObj);

  // Always returns a valid, newly-created objDefnID.
  int DefineObj(const char* sObjName,
                FXJSOBJTYPE eObjType,
                Constructor pConstructor,
                Destructor pDestructor);

  void DefineObjMethod(int nObjDefnID,
                       const char* sMethodName,
                       v8::FunctionCallback pMethodCall);
  void DefineObjProperty(int nObjDefnID,
                         const char* sPropName,
                         v8::AccessorGetterCallback pPropGet,
                         v8::AccessorSetterCallback pPropPut);
  void DefineObjAllProperties(int nObjDefnID,
                              v8::GenericNamedPropertyQueryCallback pPropQurey,
                              v8::GenericNamedPropertyGetterCallback pPropGet,
                              v8::GenericNamedPropertySetterCallback pPropPut,
                              v8::GenericNamedPropertyDeleterCallback pPropDel);
  void DefineObjConst(int nObjDefnID,
                      const char* sConstName,
                      v8::Local<v8::Value> pDefault);
  void DefineGlobalMethod(const char* sMethodName,
                          v8::FunctionCallback pMethodCall);
  void DefineGlobalConst(const wchar_t* sConstName,
                         v8::FunctionCallback pConstGetter);

  // Called after FXJS_Define* calls made.
  void InitializeEngine();
  void ReleaseEngine();

  // Called after FXJS_InitializeEngine call made.
  Optional<IJS_Runtime::JS_Error> Execute(const WideString& script);

  v8::Local<v8::Object> GetThisObj();
  v8::Local<v8::Object> NewFXJSBoundObject(int nObjDefnID, FXJSOBJTYPE type);
  void Error(const WideString& message);

  v8::Local<v8::Context> GetV8Context();

  v8::Local<v8::Array> GetConstArray(const WideString& name);
  void SetConstArray(const WideString& name, v8::Local<v8::Array> array);

 protected:
  CFXJS_Engine();

 private:
  v8::Global<v8::Context> m_V8Context;
  std::vector<v8::Global<v8::Object>> m_StaticObjects;
  std::map<WideString, v8::Global<v8::Array>> m_ConstArrays;
};

#endif  // FXJS_CFXJS_ENGINE_H_
