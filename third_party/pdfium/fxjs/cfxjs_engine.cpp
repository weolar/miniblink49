// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cfxjs_engine.h"

#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/unowned_ptr.h"
#include "fxjs/cjs_object.h"
#include "fxjs/xfa/cfxjse_runtimedata.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "v8/include/v8-util.h"

class CFXJS_PerObjectData;

namespace {

unsigned int g_embedderDataSlot = 1u;
v8::Isolate* g_isolate = nullptr;
size_t g_isolate_ref_count = 0;
CFX_V8ArrayBufferAllocator* g_arrayBufferAllocator = nullptr;
v8::Global<v8::ObjectTemplate>* g_DefaultGlobalObjectTemplate = nullptr;
const wchar_t kPerObjectDataTag[] = L"CFXJS_PerObjectData";

void* GetAlignedPointerForPerObjectDataTag() {
  return const_cast<void*>(static_cast<const void*>(kPerObjectDataTag));
}

std::pair<int, int> GetLineAndColumnFromError(v8::Local<v8::Message> message,
                                              v8::Local<v8::Context> context) {
  if (message.IsEmpty())
    return std::make_pair(-1, -1);
  return std::make_pair(message->GetLineNumber(context).FromMaybe(-1),
                        message->GetStartColumn());
}

}  // namespace

// Global weak map to save dynamic objects.
class V8TemplateMapTraits final
    : public v8::StdMapTraits<CFXJS_PerObjectData*, v8::Object> {
 public:
  using WeakCallbackDataType = CFXJS_PerObjectData;
  using MapType = v8::
      GlobalValueMap<WeakCallbackDataType*, v8::Object, V8TemplateMapTraits>;

  static const v8::PersistentContainerCallbackType kCallbackType =
      v8::kWeakWithInternalFields;

  static WeakCallbackDataType* WeakCallbackParameter(
      MapType* map,
      WeakCallbackDataType* key,
      v8::Local<v8::Object> value) {
    return key;
  }
  static MapType* MapFromWeakCallbackInfo(
      const v8::WeakCallbackInfo<WeakCallbackDataType>&);
  static WeakCallbackDataType* KeyFromWeakCallbackInfo(
      const v8::WeakCallbackInfo<WeakCallbackDataType>& data) {
    return data.GetParameter();
  }
  static void OnWeakCallback(
      const v8::WeakCallbackInfo<WeakCallbackDataType>& data) {}
  static void DisposeWeak(
      const v8::WeakCallbackInfo<WeakCallbackDataType>& data);
  static void Dispose(v8::Isolate* isolate,
                      v8::Global<v8::Object> value,
                      WeakCallbackDataType* key);
  static void DisposeCallbackData(WeakCallbackDataType* callbackData) {}
};

class V8TemplateMap {
 public:
  using WeakCallbackDataType = CFXJS_PerObjectData;
  using MapType = v8::
      GlobalValueMap<WeakCallbackDataType*, v8::Object, V8TemplateMapTraits>;

  explicit V8TemplateMap(v8::Isolate* isolate) : m_map(isolate) {}
  ~V8TemplateMap() = default;

  void SetAndMakeWeak(WeakCallbackDataType* key, v8::Local<v8::Object> handle) {
    ASSERT(!m_map.Contains(key));

    // Inserting an object into a GlobalValueMap with the appropriate traits
    // has the side-effect of making the object weak deep in the guts of V8,
    // and arranges for it to be cleaned up by the methods in the traits.
    m_map.Set(key, handle);
  }

  friend class V8TemplateMapTraits;

 private:
  MapType m_map;
};

class CFXJS_PerObjectData {
 public:
  explicit CFXJS_PerObjectData(int nObjDefID) : m_ObjDefID(nObjDefID) {}

  ~CFXJS_PerObjectData() = default;

  static void SetInObject(CFXJS_PerObjectData* pData,
                          v8::Local<v8::Object> pObj) {
    if (pObj->InternalFieldCount() == 2) {
      pObj->SetAlignedPointerInInternalField(
          0, GetAlignedPointerForPerObjectDataTag());
      pObj->SetAlignedPointerInInternalField(1, pData);
    }
  }

  static CFXJS_PerObjectData* GetFromObject(v8::Local<v8::Object> pObj) {
    if (pObj.IsEmpty() || pObj->InternalFieldCount() != 2 ||
        pObj->GetAlignedPointerFromInternalField(0) !=
            GetAlignedPointerForPerObjectDataTag()) {
      return nullptr;
    }
    return static_cast<CFXJS_PerObjectData*>(
        pObj->GetAlignedPointerFromInternalField(1));
  }

  const int m_ObjDefID;
  std::unique_ptr<CJS_Object> m_pPrivate;
};

class CFXJS_ObjDefinition {
 public:
  CFXJS_ObjDefinition(v8::Isolate* isolate,
                      const char* sObjName,
                      FXJSOBJTYPE eObjType,
                      CFXJS_Engine::Constructor pConstructor,
                      CFXJS_Engine::Destructor pDestructor)
      : m_ObjName(sObjName),
        m_ObjType(eObjType),
        m_pConstructor(pConstructor),
        m_pDestructor(pDestructor),
        m_pIsolate(isolate) {
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::FunctionTemplate> fun = v8::FunctionTemplate::New(isolate);
    fun->InstanceTemplate()->SetInternalFieldCount(2);
    fun->SetCallHandler(CallHandler, v8::Number::New(isolate, eObjType));
    if (eObjType == FXJSOBJTYPE_GLOBAL) {
      fun->InstanceTemplate()->Set(
          v8::Symbol::GetToStringTag(isolate),
          v8::String::NewFromUtf8(isolate, "global", v8::NewStringType::kNormal)
              .ToLocalChecked());
    }
    m_FunctionTemplate.Reset(isolate, fun);
    m_Signature.Reset(isolate, v8::Signature::New(isolate, fun));
  }

  static void CallHandler(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    if (!info.IsConstructCall()) {
      isolate->ThrowException(
          v8::String::NewFromUtf8(isolate, "illegal constructor",
                                  v8::NewStringType::kNormal)
              .ToLocalChecked());
      return;
    }
    if (info.Data().As<v8::Int32>()->Value() != FXJSOBJTYPE_DYNAMIC) {
      isolate->ThrowException(
          v8::String::NewFromUtf8(isolate, "not a dynamic object",
                                  v8::NewStringType::kNormal)
              .ToLocalChecked());
      return;
    }
    v8::Local<v8::Object> holder = info.Holder();
    ASSERT(holder->InternalFieldCount() == 2);
    holder->SetAlignedPointerInInternalField(0, nullptr);
    holder->SetAlignedPointerInInternalField(1, nullptr);
  }

  v8::Isolate* GetIsolate() const { return m_pIsolate.Get(); }

  void DefineConst(const char* sConstName, v8::Local<v8::Value> pDefault) {
    GetInstanceTemplate()->Set(GetIsolate(), sConstName, pDefault);
  }

  void DefineProperty(v8::Local<v8::String> sPropName,
                      v8::AccessorGetterCallback pPropGet,
                      v8::AccessorSetterCallback pPropPut) {
    GetInstanceTemplate()->SetAccessor(sPropName, pPropGet, pPropPut);
  }

  void DefineMethod(v8::Local<v8::String> sMethodName,
                    v8::FunctionCallback pMethodCall) {
    v8::Local<v8::FunctionTemplate> fun = v8::FunctionTemplate::New(
        GetIsolate(), pMethodCall, v8::Local<v8::Value>(), GetSignature());
    fun->RemovePrototype();
    GetInstanceTemplate()->Set(sMethodName, fun, v8::ReadOnly);
  }

  void DefineAllProperties(v8::GenericNamedPropertyQueryCallback pPropQurey,
                           v8::GenericNamedPropertyGetterCallback pPropGet,
                           v8::GenericNamedPropertySetterCallback pPropPut,
                           v8::GenericNamedPropertyDeleterCallback pPropDel) {
    GetInstanceTemplate()->SetHandler(v8::NamedPropertyHandlerConfiguration(
        pPropGet, pPropPut, pPropQurey, pPropDel, nullptr,
        v8::Local<v8::Value>(),
        v8::PropertyHandlerFlags::kOnlyInterceptStrings));
  }

  v8::Local<v8::ObjectTemplate> GetInstanceTemplate() {
    v8::EscapableHandleScope scope(GetIsolate());
    v8::Local<v8::FunctionTemplate> function =
        m_FunctionTemplate.Get(GetIsolate());
    return scope.Escape(function->InstanceTemplate());
  }

  v8::Local<v8::Signature> GetSignature() {
    v8::EscapableHandleScope scope(GetIsolate());
    return scope.Escape(m_Signature.Get(GetIsolate()));
  }

  const char* const m_ObjName;
  const FXJSOBJTYPE m_ObjType;
  const CFXJS_Engine::Constructor m_pConstructor;
  const CFXJS_Engine::Destructor m_pDestructor;
  UnownedPtr<v8::Isolate> m_pIsolate;
  v8::Global<v8::FunctionTemplate> m_FunctionTemplate;
  v8::Global<v8::Signature> m_Signature;
};

static v8::Local<v8::ObjectTemplate> GetGlobalObjectTemplate(
    v8::Isolate* pIsolate) {
  FXJS_PerIsolateData* pIsolateData = FXJS_PerIsolateData::Get(pIsolate);
  for (int i = 0; i < pIsolateData->MaxObjDefinitionID(); ++i) {
    CFXJS_ObjDefinition* pObjDef = pIsolateData->ObjDefinitionForID(i);
    if (pObjDef->m_ObjType == FXJSOBJTYPE_GLOBAL)
      return pObjDef->GetInstanceTemplate();
  }
  if (!g_DefaultGlobalObjectTemplate) {
    v8::Local<v8::ObjectTemplate> hGlobalTemplate =
        v8::ObjectTemplate::New(pIsolate);
    hGlobalTemplate->Set(
        v8::Symbol::GetToStringTag(pIsolate),
        v8::String::NewFromUtf8(pIsolate, "global", v8::NewStringType::kNormal)
            .ToLocalChecked());
    g_DefaultGlobalObjectTemplate =
        new v8::Global<v8::ObjectTemplate>(pIsolate, hGlobalTemplate);
  }
  return g_DefaultGlobalObjectTemplate->Get(pIsolate);
}

void V8TemplateMapTraits::Dispose(v8::Isolate* isolate,
                                  v8::Global<v8::Object> value,
                                  WeakCallbackDataType* key) {
  v8::Local<v8::Object> obj = value.Get(isolate);
  if (obj.IsEmpty())
    return;
  int id = CFXJS_Engine::GetObjDefnID(obj);
  if (id == -1)
    return;
  FXJS_PerIsolateData* pIsolateData = FXJS_PerIsolateData::Get(isolate);
  CFXJS_ObjDefinition* pObjDef = pIsolateData->ObjDefinitionForID(id);
  if (!pObjDef)
    return;
  if (pObjDef->m_pDestructor)
    pObjDef->m_pDestructor(obj);
  CFXJS_Engine::FreeObjectPrivate(obj);
}

void V8TemplateMapTraits::DisposeWeak(
    const v8::WeakCallbackInfo<WeakCallbackDataType>& data) {
  // TODO(tsepez): this is expected be called during GC.
}

V8TemplateMapTraits::MapType* V8TemplateMapTraits::MapFromWeakCallbackInfo(
    const v8::WeakCallbackInfo<WeakCallbackDataType>& data) {
  V8TemplateMap* pMap =
      FXJS_PerIsolateData::Get(data.GetIsolate())->m_pDynamicObjsMap.get();
  return pMap ? &pMap->m_map : nullptr;
}

void FXJS_Initialize(unsigned int embedderDataSlot, v8::Isolate* pIsolate) {
  if (g_isolate) {
    ASSERT(g_embedderDataSlot == embedderDataSlot);
    ASSERT(g_isolate == pIsolate);
    return;
  }
  g_embedderDataSlot = embedderDataSlot;
  g_isolate = pIsolate;
}

void FXJS_Release() {
  ASSERT(!g_isolate || g_isolate_ref_count == 0);
  delete g_DefaultGlobalObjectTemplate;
  g_DefaultGlobalObjectTemplate = nullptr;
  g_isolate = nullptr;

  delete g_arrayBufferAllocator;
  g_arrayBufferAllocator = nullptr;
}

bool FXJS_GetIsolate(v8::Isolate** pResultIsolate) {
  if (g_isolate) {
    *pResultIsolate = g_isolate;
    return false;
  }
  // Provide backwards compatibility when no external isolate.
  if (!g_arrayBufferAllocator)
    g_arrayBufferAllocator = new CFX_V8ArrayBufferAllocator();
  v8::Isolate::CreateParams params;
  params.array_buffer_allocator = g_arrayBufferAllocator;
  *pResultIsolate = v8::Isolate::New(params);
  return true;
}

size_t FXJS_GlobalIsolateRefCount() {
  return g_isolate_ref_count;
}

FXJS_PerIsolateData::~FXJS_PerIsolateData() {}

// static
void FXJS_PerIsolateData::SetUp(v8::Isolate* pIsolate) {
  if (!pIsolate->GetData(g_embedderDataSlot))
    pIsolate->SetData(g_embedderDataSlot, new FXJS_PerIsolateData(pIsolate));
}

// static
FXJS_PerIsolateData* FXJS_PerIsolateData::Get(v8::Isolate* pIsolate) {
  return static_cast<FXJS_PerIsolateData*>(
      pIsolate->GetData(g_embedderDataSlot));
}

int FXJS_PerIsolateData::MaxObjDefinitionID() const {
  return pdfium::CollectionSize<int>(m_ObjectDefnArray);
}

FXJS_PerIsolateData::FXJS_PerIsolateData(v8::Isolate* pIsolate)
    : m_pDynamicObjsMap(new V8TemplateMap(pIsolate)) {}

CFXJS_ObjDefinition* FXJS_PerIsolateData::ObjDefinitionForID(int id) const {
  return (id >= 0 && id < MaxObjDefinitionID()) ? m_ObjectDefnArray[id].get()
                                                : nullptr;
}

int FXJS_PerIsolateData::AssignIDForObjDefinition(
    std::unique_ptr<CFXJS_ObjDefinition> pDefn) {
  m_ObjectDefnArray.push_back(std::move(pDefn));
  return m_ObjectDefnArray.size() - 1;
}

CFXJS_Engine::CFXJS_Engine() : CFX_V8(nullptr) {}

CFXJS_Engine::CFXJS_Engine(v8::Isolate* pIsolate) : CFX_V8(pIsolate) {}

CFXJS_Engine::~CFXJS_Engine() = default;

// static
int CFXJS_Engine::GetObjDefnID(v8::Local<v8::Object> pObj) {
  CFXJS_PerObjectData* pData = CFXJS_PerObjectData::GetFromObject(pObj);
  return pData ? pData->m_ObjDefID : -1;
}

// static
void CFXJS_Engine::SetObjectPrivate(v8::Local<v8::Object> pObj,
                                    std::unique_ptr<CJS_Object> p) {
  CFXJS_PerObjectData* pPerObjectData =
      CFXJS_PerObjectData::GetFromObject(pObj);
  if (!pPerObjectData)
    return;
  pPerObjectData->m_pPrivate = std::move(p);
}

// static
void CFXJS_Engine::FreeObjectPrivate(v8::Local<v8::Object> pObj) {
  CFXJS_PerObjectData* pData = CFXJS_PerObjectData::GetFromObject(pObj);
  pObj->SetAlignedPointerInInternalField(0, nullptr);
  pObj->SetAlignedPointerInInternalField(1, nullptr);
  delete pData;
}

int CFXJS_Engine::DefineObj(const char* sObjName,
                            FXJSOBJTYPE eObjType,
                            CFXJS_Engine::Constructor pConstructor,
                            CFXJS_Engine::Destructor pDestructor) {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());
  FXJS_PerIsolateData::SetUp(GetIsolate());
  FXJS_PerIsolateData* pIsolateData = FXJS_PerIsolateData::Get(GetIsolate());
  return pIsolateData->AssignIDForObjDefinition(
      pdfium::MakeUnique<CFXJS_ObjDefinition>(GetIsolate(), sObjName, eObjType,
                                              pConstructor, pDestructor));
}

void CFXJS_Engine::DefineObjMethod(int nObjDefnID,
                                   const char* sMethodName,
                                   v8::FunctionCallback pMethodCall) {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());
  FXJS_PerIsolateData* pIsolateData = FXJS_PerIsolateData::Get(GetIsolate());
  CFXJS_ObjDefinition* pObjDef = pIsolateData->ObjDefinitionForID(nObjDefnID);
  pObjDef->DefineMethod(NewString(sMethodName), pMethodCall);
}

void CFXJS_Engine::DefineObjProperty(int nObjDefnID,
                                     const char* sPropName,
                                     v8::AccessorGetterCallback pPropGet,
                                     v8::AccessorSetterCallback pPropPut) {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());
  FXJS_PerIsolateData* pIsolateData = FXJS_PerIsolateData::Get(GetIsolate());
  CFXJS_ObjDefinition* pObjDef = pIsolateData->ObjDefinitionForID(nObjDefnID);
  pObjDef->DefineProperty(NewString(sPropName), pPropGet, pPropPut);
}

void CFXJS_Engine::DefineObjAllProperties(
    int nObjDefnID,
    v8::GenericNamedPropertyQueryCallback pPropQurey,
    v8::GenericNamedPropertyGetterCallback pPropGet,
    v8::GenericNamedPropertySetterCallback pPropPut,
    v8::GenericNamedPropertyDeleterCallback pPropDel) {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());
  FXJS_PerIsolateData* pIsolateData = FXJS_PerIsolateData::Get(GetIsolate());
  CFXJS_ObjDefinition* pObjDef = pIsolateData->ObjDefinitionForID(nObjDefnID);
  pObjDef->DefineAllProperties(pPropQurey, pPropGet, pPropPut, pPropDel);
}

void CFXJS_Engine::DefineObjConst(int nObjDefnID,
                                  const char* sConstName,
                                  v8::Local<v8::Value> pDefault) {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());
  FXJS_PerIsolateData* pIsolateData = FXJS_PerIsolateData::Get(GetIsolate());
  CFXJS_ObjDefinition* pObjDef = pIsolateData->ObjDefinitionForID(nObjDefnID);
  pObjDef->DefineConst(sConstName, pDefault);
}

void CFXJS_Engine::DefineGlobalMethod(const char* sMethodName,
                                      v8::FunctionCallback pMethodCall) {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());
  v8::Local<v8::FunctionTemplate> fun =
      v8::FunctionTemplate::New(GetIsolate(), pMethodCall);
  fun->RemovePrototype();
  GetGlobalObjectTemplate(GetIsolate())
      ->Set(NewString(sMethodName), fun, v8::ReadOnly);
}

void CFXJS_Engine::DefineGlobalConst(const wchar_t* sConstName,
                                     v8::FunctionCallback pConstGetter) {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());
  v8::Local<v8::FunctionTemplate> fun =
      v8::FunctionTemplate::New(GetIsolate(), pConstGetter);
  fun->RemovePrototype();
  GetGlobalObjectTemplate(GetIsolate())
      ->SetAccessorProperty(NewString(sConstName), fun);
}

void CFXJS_Engine::InitializeEngine() {
  if (GetIsolate() == g_isolate)
    ++g_isolate_ref_count;

  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());

  // This has to happen before we call GetGlobalObjectTemplate because that
  // method gets the PerIsolateData from GetIsolate().
  FXJS_PerIsolateData::SetUp(GetIsolate());

  v8::Local<v8::Context> v8Context = v8::Context::New(
      GetIsolate(), nullptr, GetGlobalObjectTemplate(GetIsolate()));

  // May not have the internal fields when called from tests.
  v8::Local<v8::Object> pThisProxy = v8Context->Global();
  if (pThisProxy->InternalFieldCount() == 2) {
    pThisProxy->SetAlignedPointerInInternalField(0, nullptr);
    pThisProxy->SetAlignedPointerInInternalField(1, nullptr);
  }
  v8::Local<v8::Object> pThis = pThisProxy->GetPrototype().As<v8::Object>();
  if (pThis->InternalFieldCount() == 2) {
    pThis->SetAlignedPointerInInternalField(0, nullptr);
    pThis->SetAlignedPointerInInternalField(1, nullptr);
  }

  v8::Context::Scope context_scope(v8Context);
  FXJS_PerIsolateData* pIsolateData = FXJS_PerIsolateData::Get(GetIsolate());
  int maxID = pIsolateData->MaxObjDefinitionID();
  m_StaticObjects.resize(maxID + 1);
  for (int i = 0; i < maxID; ++i) {
    CFXJS_ObjDefinition* pObjDef = pIsolateData->ObjDefinitionForID(i);
    if (pObjDef->m_ObjType == FXJSOBJTYPE_GLOBAL) {
      CFXJS_PerObjectData::SetInObject(new CFXJS_PerObjectData(i),
                                       v8Context->Global()
                                           ->GetPrototype()
                                           ->ToObject(v8Context)
                                           .ToLocalChecked());
      if (pObjDef->m_pConstructor) {
        pObjDef->m_pConstructor(this, v8Context->Global()
                                          ->GetPrototype()
                                          ->ToObject(v8Context)
                                          .ToLocalChecked());
      }
    } else if (pObjDef->m_ObjType == FXJSOBJTYPE_STATIC) {
      v8::Local<v8::String> pObjName = NewString(pObjDef->m_ObjName);
      v8::Local<v8::Object> obj = NewFXJSBoundObject(i, FXJSOBJTYPE_STATIC);
      if (!obj.IsEmpty()) {
        v8Context->Global()->Set(v8Context, pObjName, obj).FromJust();
        m_StaticObjects[i] = v8::Global<v8::Object>(GetIsolate(), obj);
      }
    }
  }
  m_V8Context.Reset(GetIsolate(), v8Context);
}

void CFXJS_Engine::ReleaseEngine() {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::HandleScope handle_scope(GetIsolate());
  v8::Local<v8::Context> context = GetV8Context();
  v8::Context::Scope context_scope(context);
  FXJS_PerIsolateData* pIsolateData = FXJS_PerIsolateData::Get(GetIsolate());
  if (!pIsolateData)
    return;

  m_ConstArrays.clear();

  for (int i = 0; i < pIsolateData->MaxObjDefinitionID(); ++i) {
    CFXJS_ObjDefinition* pObjDef = pIsolateData->ObjDefinitionForID(i);
    v8::Local<v8::Object> pObj;
    if (pObjDef->m_ObjType == FXJSOBJTYPE_GLOBAL) {
      pObj =
          context->Global()->GetPrototype()->ToObject(context).ToLocalChecked();
    } else if (!m_StaticObjects[i].IsEmpty()) {
      pObj = v8::Local<v8::Object>::New(GetIsolate(), m_StaticObjects[i]);
      m_StaticObjects[i].Reset();
    }
    if (!pObj.IsEmpty()) {
      if (pObjDef->m_pDestructor)
        pObjDef->m_pDestructor(pObj);
      FreeObjectPrivate(pObj);
    }
  }

  m_V8Context.Reset();

  if (GetIsolate() == g_isolate && --g_isolate_ref_count > 0)
    return;

  delete pIsolateData;
  GetIsolate()->SetData(g_embedderDataSlot, nullptr);
}

Optional<IJS_Runtime::JS_Error> CFXJS_Engine::Execute(
    const WideString& script) {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::TryCatch try_catch(GetIsolate());
  v8::Local<v8::Context> context = GetIsolate()->GetCurrentContext();
  v8::Local<v8::Script> compiled_script;
  if (!v8::Script::Compile(context, NewString(script.AsStringView()))
           .ToLocal(&compiled_script)) {
    v8::String::Utf8Value error(GetIsolate(), try_catch.Exception());
    v8::Local<v8::Message> msg = try_catch.Message();
    int line = -1;
    int column = -1;
    std::tie(line, column) = GetLineAndColumnFromError(msg, context);
    return IJS_Runtime::JS_Error(line, column, WideString::FromUTF8(*error));
  }

  v8::Local<v8::Value> result;
  if (!compiled_script->Run(context).ToLocal(&result)) {
    v8::String::Utf8Value error(GetIsolate(), try_catch.Exception());
    auto msg = try_catch.Message();
    int line = -1;
    int column = -1;
    std::tie(line, column) = GetLineAndColumnFromError(msg, context);
    return IJS_Runtime::JS_Error(line, column, WideString::FromUTF8(*error));
  }
  return pdfium::nullopt;
}

v8::Local<v8::Object> CFXJS_Engine::NewFXJSBoundObject(int nObjDefnID,
                                                       FXJSOBJTYPE type) {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  v8::Local<v8::Context> context = GetIsolate()->GetCurrentContext();
  FXJS_PerIsolateData* pData = FXJS_PerIsolateData::Get(GetIsolate());
  if (!pData)
    return v8::Local<v8::Object>();

  CFXJS_ObjDefinition* pObjDef = pData->ObjDefinitionForID(nObjDefnID);
  if (!pObjDef)
    return v8::Local<v8::Object>();

  v8::Local<v8::Object> obj;
  if (!pObjDef->GetInstanceTemplate()->NewInstance(context).ToLocal(&obj))
    return v8::Local<v8::Object>();

  CFXJS_PerObjectData* pObjData = new CFXJS_PerObjectData(nObjDefnID);
  CFXJS_PerObjectData::SetInObject(pObjData, obj);
  if (pObjDef->m_pConstructor)
    pObjDef->m_pConstructor(this, obj);

  if (type == FXJSOBJTYPE_DYNAMIC) {
    auto* pIsolateData = FXJS_PerIsolateData::Get(GetIsolate());
    if (pIsolateData->m_pDynamicObjsMap)
      pIsolateData->m_pDynamicObjsMap->SetAndMakeWeak(pObjData, obj);
  }
  return obj;
}

v8::Local<v8::Object> CFXJS_Engine::GetThisObj() {
  v8::Isolate::Scope isolate_scope(GetIsolate());
  if (!FXJS_PerIsolateData::Get(GetIsolate()))
    return v8::Local<v8::Object>();

  // Return the global object.
  v8::Local<v8::Context> context = GetIsolate()->GetCurrentContext();
  return context->Global()->GetPrototype()->ToObject(context).ToLocalChecked();
}

void CFXJS_Engine::Error(const WideString& message) {
  GetIsolate()->ThrowException(NewString(message.AsStringView()));
}

v8::Local<v8::Context> CFXJS_Engine::GetV8Context() {
  return v8::Local<v8::Context>::New(GetIsolate(), m_V8Context);
}

// static
CJS_Object* CFXJS_Engine::GetObjectPrivate(v8::Local<v8::Object> pObj) {
  auto* pData = CFXJS_PerObjectData::GetFromObject(pObj);
  if (pData)
    return pData->m_pPrivate.get();

  if (pObj.IsEmpty())
    return nullptr;

  // It could be a global proxy object, in which case the prototype holds
  // the actual bound object.
  v8::Local<v8::Value> val = pObj->GetPrototype();
  if (!val->IsObject())
    return nullptr;

  auto* pProtoData = CFXJS_PerObjectData::GetFromObject(val.As<v8::Object>());
  if (!pProtoData)
    return nullptr;

  auto* pIsolateData = FXJS_PerIsolateData::Get(v8::Isolate::GetCurrent());
  if (!pIsolateData)
    return nullptr;

  CFXJS_ObjDefinition* pObjDef =
      pIsolateData->ObjDefinitionForID(pProtoData->m_ObjDefID);
  if (!pObjDef || pObjDef->m_ObjType != FXJSOBJTYPE_GLOBAL)
    return nullptr;

  return pProtoData->m_pPrivate.get();
}

v8::Local<v8::Array> CFXJS_Engine::GetConstArray(const WideString& name) {
  return v8::Local<v8::Array>::New(GetIsolate(), m_ConstArrays[name]);
}

void CFXJS_Engine::SetConstArray(const WideString& name,
                                 v8::Local<v8::Array> array) {
  m_ConstArrays[name] = v8::Global<v8::Array>(GetIsolate(), array);
}
