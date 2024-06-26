// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_annot.h"

#include "fpdfsdk/cpdfsdk_baannot.h"
#include "fxjs/cjs_event_context.h"
#include "fxjs/cjs_object.h"
#include "fxjs/js_define.h"
#include "fxjs/js_resources.h"

const JSPropertySpec CJS_Annot::PropertySpecs[] = {
    {"hidden", get_hidden_static, set_hidden_static},
    {"name", get_name_static, set_name_static},
    {"type", get_type_static, set_type_static}};

int CJS_Annot::ObjDefnID = -1;

const char CJS_Annot::kName[] = "Annot";

// static
int CJS_Annot::GetObjDefnID() {
  return ObjDefnID;
}

// static
void CJS_Annot::DefineJSObjects(CFXJS_Engine* pEngine) {
  ObjDefnID = pEngine->DefineObj(CJS_Annot::kName, FXJSOBJTYPE_DYNAMIC,
                                 JSConstructor<CJS_Annot>, JSDestructor);
  DefineProps(pEngine, ObjDefnID, PropertySpecs);
}

CJS_Annot::CJS_Annot(v8::Local<v8::Object> pObject, CJS_Runtime* pRuntime)
    : CJS_Object(pObject, pRuntime) {}

CJS_Annot::~CJS_Annot() = default;

void CJS_Annot::SetSDKAnnot(CPDFSDK_BAAnnot* annot) {
  m_pAnnot.Reset(annot);
}

CJS_Result CJS_Annot::get_hidden(CJS_Runtime* pRuntime) {
  if (!m_pAnnot)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  CPDF_Annot* pPDFAnnot = m_pAnnot->AsBAAnnot()->GetPDFAnnot();
  return CJS_Result::Success(pRuntime->NewBoolean(
      CPDF_Annot::IsAnnotationHidden(pPDFAnnot->GetAnnotDict())));
}

CJS_Result CJS_Annot::set_hidden(CJS_Runtime* pRuntime,
                                 v8::Local<v8::Value> vp) {
  // May invalidate m_pAnnot.
  bool bHidden = pRuntime->ToBoolean(vp);

  CPDFSDK_BAAnnot* pBAAnnot = ToBAAnnot(m_pAnnot.Get());
  if (!pBAAnnot)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  uint32_t flags = pBAAnnot->GetFlags();
  if (bHidden) {
    flags |= ANNOTFLAG_HIDDEN;
    flags |= ANNOTFLAG_INVISIBLE;
    flags |= ANNOTFLAG_NOVIEW;
    flags &= ~ANNOTFLAG_PRINT;
  } else {
    flags &= ~ANNOTFLAG_HIDDEN;
    flags &= ~ANNOTFLAG_INVISIBLE;
    flags &= ~ANNOTFLAG_NOVIEW;
    flags |= ANNOTFLAG_PRINT;
  }
  pBAAnnot->SetFlags(flags);
  return CJS_Result::Success();
}

CJS_Result CJS_Annot::get_name(CJS_Runtime* pRuntime) {
  CPDFSDK_BAAnnot* pBAAnnot = ToBAAnnot(m_pAnnot.Get());
  if (!pBAAnnot)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  return CJS_Result::Success(
      pRuntime->NewString(pBAAnnot->GetAnnotName().AsStringView()));
}

CJS_Result CJS_Annot::set_name(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp) {
  // May invalidate m_pAnnot.
  WideString annotName = pRuntime->ToWideString(vp);

  CPDFSDK_BAAnnot* pBAAnnot = ToBAAnnot(m_pAnnot.Get());
  if (!pBAAnnot)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  pBAAnnot->SetAnnotName(annotName);
  return CJS_Result::Success();
}

CJS_Result CJS_Annot::get_type(CJS_Runtime* pRuntime) {
  CPDFSDK_BAAnnot* pBAAnnot = ToBAAnnot(m_pAnnot.Get());
  if (!pBAAnnot)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  return CJS_Result::Success(pRuntime->NewString(
      WideString::FromDefANSI(
          CPDF_Annot::AnnotSubtypeToString(pBAAnnot->GetAnnotSubtype())
              .AsStringView())
          .AsStringView()));
}

CJS_Result CJS_Annot::set_type(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kReadOnlyError);
}
