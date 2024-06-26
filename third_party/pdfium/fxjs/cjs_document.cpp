// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_document.h"

#include <utility>

#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fpdfapi/page/cpdf_textobject.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfdoc/cpdf_interactiveform.h"
#include "core/fpdfdoc/cpdf_nametree.h"
#include "fpdfsdk/cpdfsdk_annotiteration.h"
#include "fpdfsdk/cpdfsdk_interactiveform.h"
#include "fpdfsdk/cpdfsdk_pageview.h"
#include "fxjs/cjs_annot.h"
#include "fxjs/cjs_app.h"
#include "fxjs/cjs_delaydata.h"
#include "fxjs/cjs_field.h"
#include "fxjs/cjs_icon.h"
#include "fxjs/js_resources.h"

namespace {

#define ISLATINWORD(u) (u != 0x20 && u <= 0x28FF)

int CountWords(const CPDF_TextObject* pTextObj) {
  CPDF_Font* pFont = pTextObj->GetFont();
  if (!pFont)
    return 0;

  bool bInLatinWord = false;
  int nWords = 0;
  for (size_t i = 0, sz = pTextObj->CountChars(); i < sz; ++i) {
    uint32_t charcode = CPDF_Font::kInvalidCharCode;
    float unused_kerning;

    pTextObj->GetCharInfo(i, &charcode, &unused_kerning);
    WideString swUnicode = pFont->UnicodeFromCharCode(charcode);

    uint16_t unicode = 0;
    if (swUnicode.GetLength() > 0)
      unicode = swUnicode[0];

    bool bIsLatin = ISLATINWORD(unicode);
    if (bIsLatin && bInLatinWord)
      continue;

    bInLatinWord = bIsLatin;
    if (unicode != 0x20)
      nWords++;
  }

  return nWords;
}

WideString GetObjWordStr(const CPDF_TextObject* pTextObj, int nWordIndex) {
  CPDF_Font* pFont = pTextObj->GetFont();
  if (!pFont)
    return WideString();

  WideString swRet;
  int nWords = 0;
  bool bInLatinWord = false;
  for (size_t i = 0, sz = pTextObj->CountChars(); i < sz; ++i) {
    uint32_t charcode = CPDF_Font::kInvalidCharCode;
    float unused_kerning;

    pTextObj->GetCharInfo(i, &charcode, &unused_kerning);
    WideString swUnicode = pFont->UnicodeFromCharCode(charcode);

    uint16_t unicode = 0;
    if (swUnicode.GetLength() > 0)
      unicode = swUnicode[0];

    bool bIsLatin = ISLATINWORD(unicode);
    if (!bIsLatin || !bInLatinWord) {
      bInLatinWord = bIsLatin;
      if (unicode != 0x20)
        nWords++;
    }

    if (nWords - 1 == nWordIndex)
      swRet += unicode;
  }

  return swRet;
}

}  // namespace

const JSPropertySpec CJS_Document::PropertySpecs[] = {
    {"ADBE", get_ADBE_static, set_ADBE_static},
    {"author", get_author_static, set_author_static},
    {"baseURL", get_base_URL_static, set_base_URL_static},
    {"bookmarkRoot", get_bookmark_root_static, set_bookmark_root_static},
    {"calculate", get_calculate_static, set_calculate_static},
    {"Collab", get_collab_static, set_collab_static},
    {"creationDate", get_creation_date_static, set_creation_date_static},
    {"creator", get_creator_static, set_creator_static},
    {"delay", get_delay_static, set_delay_static},
    {"dirty", get_dirty_static, set_dirty_static},
    {"documentFileName", get_document_file_name_static,
     set_document_file_name_static},
    {"external", get_external_static, set_external_static},
    {"filesize", get_filesize_static, set_filesize_static},
    {"icons", get_icons_static, set_icons_static},
    {"info", get_info_static, set_info_static},
    {"keywords", get_keywords_static, set_keywords_static},
    {"layout", get_layout_static, set_layout_static},
    {"media", get_media_static, set_media_static},
    {"modDate", get_mod_date_static, set_mod_date_static},
    {"mouseX", get_mouse_x_static, set_mouse_x_static},
    {"mouseY", get_mouse_y_static, set_mouse_y_static},
    {"numFields", get_num_fields_static, set_num_fields_static},
    {"numPages", get_num_pages_static, set_num_pages_static},
    {"pageNum", get_page_num_static, set_page_num_static},
    {"pageWindowRect", get_page_window_rect_static,
     set_page_window_rect_static},
    {"path", get_path_static, set_path_static},
    {"producer", get_producer_static, set_producer_static},
    {"subject", get_subject_static, set_subject_static},
    {"title", get_title_static, set_title_static},
    {"URL", get_URL_static, set_URL_static},
    {"zoom", get_zoom_static, set_zoom_static},
    {"zoomType", get_zoom_type_static, set_zoom_type_static}};

const JSMethodSpec CJS_Document::MethodSpecs[] = {
    {"addAnnot", addAnnot_static},
    {"addField", addField_static},
    {"addLink", addLink_static},
    {"addIcon", addIcon_static},
    {"calculateNow", calculateNow_static},
    {"closeDoc", closeDoc_static},
    {"createDataObject", createDataObject_static},
    {"deletePages", deletePages_static},
    {"exportAsText", exportAsText_static},
    {"exportAsFDF", exportAsFDF_static},
    {"exportAsXFDF", exportAsXFDF_static},
    {"extractPages", extractPages_static},
    {"getAnnot", getAnnot_static},
    {"getAnnots", getAnnots_static},
    {"getAnnot3D", getAnnot3D_static},
    {"getAnnots3D", getAnnots3D_static},
    {"getField", getField_static},
    {"getIcon", getIcon_static},
    {"getLinks", getLinks_static},
    {"getNthFieldName", getNthFieldName_static},
    {"getOCGs", getOCGs_static},
    {"getPageBox", getPageBox_static},
    {"getPageNthWord", getPageNthWord_static},
    {"getPageNthWordQuads", getPageNthWordQuads_static},
    {"getPageNumWords", getPageNumWords_static},
    {"getPrintParams", getPrintParams_static},
    {"getURL", getURL_static},
    {"gotoNamedDest", gotoNamedDest_static},
    {"importAnFDF", importAnFDF_static},
    {"importAnXFDF", importAnXFDF_static},
    {"importTextData", importTextData_static},
    {"insertPages", insertPages_static},
    {"mailDoc", mailDoc_static},
    {"mailForm", mailForm_static},
    {"print", print_static},
    {"removeField", removeField_static},
    {"replacePages", replacePages_static},
    {"resetForm", resetForm_static},
    {"removeIcon", removeIcon_static},
    {"saveAs", saveAs_static},
    {"submitForm", submitForm_static},
    {"syncAnnotScan", syncAnnotScan_static}};

int CJS_Document::ObjDefnID = -1;
const char CJS_Document::kName[] = "Document";

// static
int CJS_Document::GetObjDefnID() {
  return ObjDefnID;
}

// static
void CJS_Document::DefineJSObjects(CFXJS_Engine* pEngine) {
  ObjDefnID = pEngine->DefineObj(CJS_Document::kName, FXJSOBJTYPE_GLOBAL,
                                 JSConstructor<CJS_Document>, JSDestructor);
  DefineProps(pEngine, ObjDefnID, PropertySpecs);
  DefineMethods(pEngine, ObjDefnID, MethodSpecs);
}

CJS_Document::CJS_Document(v8::Local<v8::Object> pObject, CJS_Runtime* pRuntime)
    : CJS_Object(pObject, pRuntime) {
  SetFormFillEnv(GetRuntime()->GetFormFillEnv());
}

CJS_Document::~CJS_Document() = default;

// The total number of fields in document.
CJS_Result CJS_Document::get_num_fields(CJS_Runtime* pRuntime) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  CPDF_InteractiveForm* pPDFForm = GetCoreInteractiveForm();
  return CJS_Result::Success(pRuntime->NewNumber(
      static_cast<int>(pPDFForm->CountFields(WideString()))));
}

CJS_Result CJS_Document::set_num_fields(CJS_Runtime* pRuntime,
                                        v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kReadOnlyError);
}

CJS_Result CJS_Document::get_dirty(CJS_Runtime* pRuntime) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  return CJS_Result::Success(
      pRuntime->NewBoolean(!!m_pFormFillEnv->GetChangeMark()));
}

CJS_Result CJS_Document::set_dirty(CJS_Runtime* pRuntime,
                                   v8::Local<v8::Value> vp) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  pRuntime->ToBoolean(vp) ? m_pFormFillEnv->SetChangeMark()
                          : m_pFormFillEnv->ClearChangeMark();
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_ADBE(CJS_Runtime* pRuntime) {
  return CJS_Result::Success(pRuntime->NewUndefined());
}

CJS_Result CJS_Document::set_ADBE(CJS_Runtime* pRuntime,
                                  v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_page_num(CJS_Runtime* pRuntime) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  CPDFSDK_PageView* pPageView = m_pFormFillEnv->GetCurrentView();
  if (!pPageView)
    return CJS_Result::Success(pRuntime->NewUndefined());

  return CJS_Result::Success(pRuntime->NewNumber(pPageView->GetPageIndex()));
}

CJS_Result CJS_Document::set_page_num(CJS_Runtime* pRuntime,
                                      v8::Local<v8::Value> vp) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  int iPageCount = m_pFormFillEnv->GetPageCount();
  int iPageNum = pRuntime->ToInt32(vp);
  if (iPageNum >= 0 && iPageNum < iPageCount)
    m_pFormFillEnv->JS_docgotoPage(iPageNum);
  else if (iPageNum >= iPageCount)
    m_pFormFillEnv->JS_docgotoPage(iPageCount - 1);
  else if (iPageNum < 0)
    m_pFormFillEnv->JS_docgotoPage(0);

  return CJS_Result::Success();
}

CJS_Result CJS_Document::addAnnot(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Not supported, but do not return an error.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::addField(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Not supported, but do not return an error.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::exportAsText(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported, but do not return an error.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::exportAsFDF(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported, but do not return an error.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::exportAsXFDF(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported, but do not return an error.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::getField(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.empty())
    return CJS_Result::Failure(JSMessage::kParamError);

  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  WideString wideName = pRuntime->ToWideString(params[0]);
  CPDF_InteractiveForm* pPDFForm = GetCoreInteractiveForm();
  if (pPDFForm->CountFields(wideName) <= 0)
    return CJS_Result::Success(pRuntime->NewUndefined());

  v8::Local<v8::Object> pFieldObj = pRuntime->NewFXJSBoundObject(
      CJS_Field::GetObjDefnID(), FXJSOBJTYPE_DYNAMIC);
  if (pFieldObj.IsEmpty())
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  auto* pJSField =
      static_cast<CJS_Field*>(CFXJS_Engine::GetObjectPrivate(pFieldObj));
  if (!pJSField)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  pJSField->AttachField(this, wideName);
  return CJS_Result::Success(pJSField->ToV8Object());
}

// Gets the name of the nth field in the document
CJS_Result CJS_Document::getNthFieldName(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  int nIndex = pRuntime->ToInt32(params[0]);
  if (nIndex < 0)
    return CJS_Result::Failure(JSMessage::kValueError);

  CPDF_InteractiveForm* pPDFForm = GetCoreInteractiveForm();
  CPDF_FormField* pField = pPDFForm->GetField(nIndex, WideString());
  if (!pField)
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  return CJS_Result::Success(
      pRuntime->NewString(pField->GetFullName().AsStringView()));
}

CJS_Result CJS_Document::importAnFDF(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::importAnXFDF(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::importTextData(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::mailDoc(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  std::vector<v8::Local<v8::Value>> newParams = ExpandKeywordParams(
      pRuntime, params, 6, "bUI", "cTo", "cCc", "cBcc", "cSubject", "cMsg");

  bool bUI = true;
  if (IsExpandedParamKnown(newParams[0]))
    bUI = pRuntime->ToBoolean(newParams[0]);

  WideString cTo;
  if (IsExpandedParamKnown(newParams[1]))
    cTo = pRuntime->ToWideString(newParams[1]);

  WideString cCc;
  if (IsExpandedParamKnown(newParams[2]))
    cCc = pRuntime->ToWideString(newParams[2]);

  WideString cBcc;
  if (IsExpandedParamKnown(newParams[3]))
    cBcc = pRuntime->ToWideString(newParams[3]);

  WideString cSubject;
  if (IsExpandedParamKnown(newParams[4]))
    cSubject = pRuntime->ToWideString(newParams[4]);

  WideString cMsg;
  if (IsExpandedParamKnown(newParams[5]))
    cMsg = pRuntime->ToWideString(newParams[5]);

  pRuntime->BeginBlock();
  m_pFormFillEnv->JS_docmailForm(nullptr, 0, bUI, cTo, cSubject, cCc, cBcc,
                                 cMsg);
  pRuntime->EndBlock();
  return CJS_Result::Success();
}

// exports the form data and mails the resulting fdf file as an attachment to
// all recipients.
// comment: need reader supports
CJS_Result CJS_Document::mailForm(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  if (!m_pFormFillEnv->GetPermissions(FPDFPERM_EXTRACT_ACCESS))
    return CJS_Result::Failure(JSMessage::kPermissionError);

  CPDFSDK_InteractiveForm* pInteractiveForm = GetSDKInteractiveForm();
  ByteString sTextBuf = pInteractiveForm->ExportFormToFDFTextBuf();
  if (sTextBuf.GetLength() == 0)
    return CJS_Result::Failure(L"Bad FDF format.");

  std::vector<v8::Local<v8::Value>> newParams = ExpandKeywordParams(
      pRuntime, params, 6, "bUI", "cTo", "cCc", "cBcc", "cSubject", "cMsg");

  bool bUI = true;
  if (IsExpandedParamKnown(newParams[0]))
    bUI = pRuntime->ToBoolean(newParams[0]);

  WideString cTo;
  if (IsExpandedParamKnown(newParams[1]))
    cTo = pRuntime->ToWideString(newParams[1]);

  WideString cCc;
  if (IsExpandedParamKnown(newParams[2]))
    cCc = pRuntime->ToWideString(newParams[2]);

  WideString cBcc;
  if (IsExpandedParamKnown(newParams[3]))
    cBcc = pRuntime->ToWideString(newParams[3]);

  WideString cSubject;
  if (IsExpandedParamKnown(newParams[4]))
    cSubject = pRuntime->ToWideString(newParams[4]);

  WideString cMsg;
  if (IsExpandedParamKnown(newParams[5]))
    cMsg = pRuntime->ToWideString(newParams[5]);

  std::vector<char> mutable_buf(sTextBuf.begin(), sTextBuf.end());
  pRuntime->BeginBlock();
  m_pFormFillEnv->JS_docmailForm(mutable_buf.data(), mutable_buf.size(), bUI,
                                 cTo, cSubject, cCc, cBcc, cMsg);
  pRuntime->EndBlock();
  return CJS_Result::Success();
}

CJS_Result CJS_Document::print(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  std::vector<v8::Local<v8::Value>> newParams = ExpandKeywordParams(
      pRuntime, params, 8, "bUI", "nStart", "nEnd", "bSilent", "bShrinkToFit",
      "bPrintAsImage", "bReverse", "bAnnotations");

  bool bUI = true;
  if (IsExpandedParamKnown(newParams[0]))
    bUI = pRuntime->ToBoolean(newParams[0]);

  int nStart = 0;
  if (IsExpandedParamKnown(newParams[1]))
    nStart = pRuntime->ToInt32(newParams[1]);

  int nEnd = 0;
  if (IsExpandedParamKnown(newParams[2]))
    nEnd = pRuntime->ToInt32(newParams[2]);

  bool bSilent = false;
  if (IsExpandedParamKnown(newParams[3]))
    bSilent = pRuntime->ToBoolean(newParams[3]);

  bool bShrinkToFit = false;
  if (IsExpandedParamKnown(newParams[4]))
    bShrinkToFit = pRuntime->ToBoolean(newParams[4]);

  bool bPrintAsImage = false;
  if (IsExpandedParamKnown(newParams[5]))
    bPrintAsImage = pRuntime->ToBoolean(newParams[5]);

  bool bReverse = false;
  if (IsExpandedParamKnown(newParams[6]))
    bReverse = pRuntime->ToBoolean(newParams[6]);

  bool bAnnotations = false;
  if (IsExpandedParamKnown(newParams[7]))
    bAnnotations = pRuntime->ToBoolean(newParams[7]);

  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  m_pFormFillEnv->JS_docprint(bUI, nStart, nEnd, bSilent, bShrinkToFit,
                              bPrintAsImage, bReverse, bAnnotations);
  return CJS_Result::Success();
}

// removes the specified field from the document.
// comment:
// note: if the filed name is not rational, adobe is dumb for it.

CJS_Result CJS_Document::removeField(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  if (!(m_pFormFillEnv->GetPermissions(FPDFPERM_MODIFY) ||
        m_pFormFillEnv->GetPermissions(FPDFPERM_ANNOT_FORM)))
    return CJS_Result::Failure(JSMessage::kPermissionError);

  WideString sFieldName = pRuntime->ToWideString(params[0]);
  CPDFSDK_InteractiveForm* pInteractiveForm = GetSDKInteractiveForm();
  std::vector<CPDFSDK_Annot::ObservedPtr> widgets;
  pInteractiveForm->GetWidgets(sFieldName, &widgets);
  if (widgets.empty())
    return CJS_Result::Success();

  for (const auto& pAnnot : widgets) {
    CPDFSDK_Widget* pWidget = ToCPDFSDKWidget(pAnnot.Get());
    if (!pWidget)
      continue;

    CFX_FloatRect rcAnnot = pWidget->GetRect();
    --rcAnnot.left;
    --rcAnnot.bottom;
    ++rcAnnot.right;
    ++rcAnnot.top;

    std::vector<CFX_FloatRect> aRefresh(1, rcAnnot);
    IPDF_Page* pPage = pWidget->GetPage();
    ASSERT(pPage);

    // If there is currently no pageview associated with the page being used
    // do not create one. We may be in the process of tearing down the document
    // and creating a new pageview at this point will cause bad things.
    CPDFSDK_PageView* pPageView = m_pFormFillEnv->GetPageView(pPage, false);
    if (pPageView)
      pPageView->UpdateRects(aRefresh);
  }
  m_pFormFillEnv->SetChangeMark();
  return CJS_Result::Success();
}

// reset filed values within a document.
// comment:
// note: if the fields names r not rational, aodbe is dumb for it.

CJS_Result CJS_Document::resetForm(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  if (!(m_pFormFillEnv->GetPermissions(FPDFPERM_MODIFY) ||
        m_pFormFillEnv->GetPermissions(FPDFPERM_ANNOT_FORM) ||
        m_pFormFillEnv->GetPermissions(FPDFPERM_FILL_FORM))) {
    return CJS_Result::Failure(JSMessage::kPermissionError);
  }

  CPDF_InteractiveForm* pPDFForm = GetCoreInteractiveForm();
  if (params.empty()) {
    pPDFForm->ResetForm(NotificationOption::kNotify);
    m_pFormFillEnv->SetChangeMark();
    return CJS_Result::Success();
  }

  v8::Local<v8::Array> array;
  if (params[0]->IsString()) {
    array = pRuntime->NewArray();
    pRuntime->PutArrayElement(array, 0, params[0]);
  } else {
    array = pRuntime->ToArray(params[0]);
  }

  std::vector<CPDF_FormField*> aFields;
  for (size_t i = 0; i < pRuntime->GetArrayLength(array); ++i) {
    WideString swVal =
        pRuntime->ToWideString(pRuntime->GetArrayElement(array, i));
    for (int j = 0, jsz = pPDFForm->CountFields(swVal); j < jsz; ++j)
      aFields.push_back(pPDFForm->GetField(j, swVal));
  }

  if (!aFields.empty()) {
    pPDFForm->ResetForm(aFields, true, NotificationOption::kNotify);
    m_pFormFillEnv->SetChangeMark();
  }

  return CJS_Result::Success();
}

CJS_Result CJS_Document::saveAs(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::syncAnnotScan(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::submitForm(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  size_t nSize = params.size();
  if (nSize < 1)
    return CJS_Result::Failure(JSMessage::kParamError);
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  v8::Local<v8::Array> aFields;
  WideString strURL;
  bool bFDF = true;
  bool bEmpty = false;
  if (params[0]->IsString()) {
    strURL = pRuntime->ToWideString(params[0]);
    if (nSize > 1)
      bFDF = pRuntime->ToBoolean(params[1]);
    if (nSize > 2)
      bEmpty = pRuntime->ToBoolean(params[2]);
    if (nSize > 3)
      aFields = pRuntime->ToArray(params[3]);
  } else if (params[0]->IsObject()) {
    v8::Local<v8::Object> pObj = pRuntime->ToObject(params[0]);
    v8::Local<v8::Value> pValue = pRuntime->GetObjectProperty(pObj, "cURL");
    if (!pValue.IsEmpty())
      strURL = pRuntime->ToWideString(pValue);

    bFDF = pRuntime->ToBoolean(pRuntime->GetObjectProperty(pObj, "bFDF"));
    bEmpty = pRuntime->ToBoolean(pRuntime->GetObjectProperty(pObj, "bEmpty"));
    aFields = pRuntime->ToArray(pRuntime->GetObjectProperty(pObj, "aFields"));
  }

  CPDF_InteractiveForm* pPDFForm = GetCoreInteractiveForm();
  if (pRuntime->GetArrayLength(aFields) == 0 && bEmpty) {
    if (pPDFForm->CheckRequiredFields(nullptr, true)) {
      pRuntime->BeginBlock();
      GetSDKInteractiveForm()->SubmitForm(strURL, false);
      pRuntime->EndBlock();
    }
    return CJS_Result::Success();
  }

  std::vector<CPDF_FormField*> fieldObjects;
  for (size_t i = 0; i < pRuntime->GetArrayLength(aFields); ++i) {
    WideString sName =
        pRuntime->ToWideString(pRuntime->GetArrayElement(aFields, i));
    for (int j = 0, jsz = pPDFForm->CountFields(sName); j < jsz; ++j) {
      CPDF_FormField* pField = pPDFForm->GetField(j, sName);
      if (!bEmpty && pField->GetValue().IsEmpty())
        continue;

      fieldObjects.push_back(pField);
    }
  }

  if (pPDFForm->CheckRequiredFields(&fieldObjects, true)) {
    pRuntime->BeginBlock();
    GetSDKInteractiveForm()->SubmitFields(strURL, fieldObjects, true, !bFDF);
    pRuntime->EndBlock();
  }
  return CJS_Result::Success();
}

void CJS_Document::SetFormFillEnv(CPDFSDK_FormFillEnvironment* pFormFillEnv) {
  m_pFormFillEnv.Reset(pFormFillEnv);
}

CJS_Result CJS_Document::get_bookmark_root(CJS_Runtime* pRuntime) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::set_bookmark_root(CJS_Runtime* pRuntime,
                                           v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_author(CJS_Runtime* pRuntime) {
  return getPropertyInternal(pRuntime, "Author");
}

CJS_Result CJS_Document::set_author(CJS_Runtime* pRuntime,
                                    v8::Local<v8::Value> vp) {
  return setPropertyInternal(pRuntime, vp, "Author");
}

CJS_Result CJS_Document::get_info(CJS_Runtime* pRuntime) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  const auto* pDictionary = m_pFormFillEnv->GetPDFDocument()->GetInfo();
  if (!pDictionary)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  WideString cwAuthor = pDictionary->GetUnicodeTextFor("Author");
  WideString cwTitle = pDictionary->GetUnicodeTextFor("Title");
  WideString cwSubject = pDictionary->GetUnicodeTextFor("Subject");
  WideString cwKeywords = pDictionary->GetUnicodeTextFor("Keywords");
  WideString cwCreator = pDictionary->GetUnicodeTextFor("Creator");
  WideString cwProducer = pDictionary->GetUnicodeTextFor("Producer");
  WideString cwCreationDate = pDictionary->GetUnicodeTextFor("CreationDate");
  WideString cwModDate = pDictionary->GetUnicodeTextFor("ModDate");
  WideString cwTrapped = pDictionary->GetUnicodeTextFor("Trapped");

  v8::Local<v8::Object> pObj = pRuntime->NewObject();
  pRuntime->PutObjectProperty(pObj, "Author",
                              pRuntime->NewString(cwAuthor.AsStringView()));
  pRuntime->PutObjectProperty(pObj, "Title",
                              pRuntime->NewString(cwTitle.AsStringView()));
  pRuntime->PutObjectProperty(pObj, "Subject",
                              pRuntime->NewString(cwSubject.AsStringView()));
  pRuntime->PutObjectProperty(pObj, "Keywords",
                              pRuntime->NewString(cwKeywords.AsStringView()));
  pRuntime->PutObjectProperty(pObj, "Creator",
                              pRuntime->NewString(cwCreator.AsStringView()));
  pRuntime->PutObjectProperty(pObj, "Producer",
                              pRuntime->NewString(cwProducer.AsStringView()));
  pRuntime->PutObjectProperty(
      pObj, "CreationDate", pRuntime->NewString(cwCreationDate.AsStringView()));
  pRuntime->PutObjectProperty(pObj, "ModDate",
                              pRuntime->NewString(cwModDate.AsStringView()));
  pRuntime->PutObjectProperty(pObj, "Trapped",
                              pRuntime->NewString(cwTrapped.AsStringView()));

  // PutObjectProperty() calls below may re-enter JS and change info dict.
  auto pCopy = pDictionary->Clone();
  CPDF_DictionaryLocker locker(ToDictionary(pCopy.get()));
  for (const auto& it : locker) {
    const ByteString& bsKey = it.first;
    CPDF_Object* pValueObj = it.second.get();
    if (pValueObj->IsString() || pValueObj->IsName()) {
      pRuntime->PutObjectProperty(
          pObj, bsKey.AsStringView(),
          pRuntime->NewString(pValueObj->GetUnicodeText().AsStringView()));
    } else if (pValueObj->IsNumber()) {
      pRuntime->PutObjectProperty(pObj, bsKey.AsStringView(),
                                  pRuntime->NewNumber(pValueObj->GetNumber()));
    } else if (pValueObj->IsBoolean()) {
      pRuntime->PutObjectProperty(
          pObj, bsKey.AsStringView(),
          pRuntime->NewBoolean(!!pValueObj->GetInteger()));
    }
  }
  return CJS_Result::Success(pObj);
}

CJS_Result CJS_Document::set_info(CJS_Runtime* pRuntime,
                                  v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kReadOnlyError);
}

CJS_Result CJS_Document::getPropertyInternal(CJS_Runtime* pRuntime,
                                             const ByteString& propName) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  CPDF_Dictionary* pDictionary = m_pFormFillEnv->GetPDFDocument()->GetInfo();
  if (!pDictionary)
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  return CJS_Result::Success(pRuntime->NewString(
      pDictionary->GetUnicodeTextFor(propName).AsStringView()));
}

CJS_Result CJS_Document::setPropertyInternal(CJS_Runtime* pRuntime,
                                             v8::Local<v8::Value> vp,
                                             const ByteString& propName) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  CPDF_Dictionary* pDictionary = m_pFormFillEnv->GetPDFDocument()->GetInfo();
  if (!pDictionary)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  if (!m_pFormFillEnv->GetPermissions(FPDFPERM_MODIFY))
    return CJS_Result::Failure(JSMessage::kPermissionError);

  pDictionary->SetNewFor<CPDF_String>(propName, pRuntime->ToWideString(vp));
  m_pFormFillEnv->SetChangeMark();
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_creation_date(CJS_Runtime* pRuntime) {
  return getPropertyInternal(pRuntime, "CreationDate");
}

CJS_Result CJS_Document::set_creation_date(CJS_Runtime* pRuntime,
                                           v8::Local<v8::Value> vp) {
  return setPropertyInternal(pRuntime, vp, "CreationDate");
}

CJS_Result CJS_Document::get_creator(CJS_Runtime* pRuntime) {
  return getPropertyInternal(pRuntime, "Creator");
}

CJS_Result CJS_Document::set_creator(CJS_Runtime* pRuntime,
                                     v8::Local<v8::Value> vp) {
  return setPropertyInternal(pRuntime, vp, "Creator");
}

CJS_Result CJS_Document::get_delay(CJS_Runtime* pRuntime) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  return CJS_Result::Success(pRuntime->NewBoolean(m_bDelay));
}

CJS_Result CJS_Document::set_delay(CJS_Runtime* pRuntime,
                                   v8::Local<v8::Value> vp) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  if (!m_pFormFillEnv->GetPermissions(FPDFPERM_MODIFY))
    return CJS_Result::Failure(JSMessage::kPermissionError);

  m_bDelay = pRuntime->ToBoolean(vp);
  if (m_bDelay) {
    m_DelayData.clear();
    return CJS_Result::Success();
  }

  std::list<std::unique_ptr<CJS_DelayData>> DelayDataToProcess;
  DelayDataToProcess.swap(m_DelayData);
  for (const auto& pData : DelayDataToProcess)
    CJS_Field::DoDelay(m_pFormFillEnv.Get(), pData.get());

  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_keywords(CJS_Runtime* pRuntime) {
  return getPropertyInternal(pRuntime, "Keywords");
}

CJS_Result CJS_Document::set_keywords(CJS_Runtime* pRuntime,
                                      v8::Local<v8::Value> vp) {
  return setPropertyInternal(pRuntime, vp, "Keywords");
}

CJS_Result CJS_Document::get_mod_date(CJS_Runtime* pRuntime) {
  return getPropertyInternal(pRuntime, "ModDate");
}

CJS_Result CJS_Document::set_mod_date(CJS_Runtime* pRuntime,
                                      v8::Local<v8::Value> vp) {
  return setPropertyInternal(pRuntime, vp, "ModDate");
}

CJS_Result CJS_Document::get_producer(CJS_Runtime* pRuntime) {
  return getPropertyInternal(pRuntime, "Producer");
}

CJS_Result CJS_Document::set_producer(CJS_Runtime* pRuntime,
                                      v8::Local<v8::Value> vp) {
  return setPropertyInternal(pRuntime, vp, "Producer");
}

CJS_Result CJS_Document::get_subject(CJS_Runtime* pRuntime) {
  return getPropertyInternal(pRuntime, "Subject");
}

CJS_Result CJS_Document::set_subject(CJS_Runtime* pRuntime,
                                     v8::Local<v8::Value> vp) {
  return setPropertyInternal(pRuntime, vp, "Subject");
}

CJS_Result CJS_Document::get_title(CJS_Runtime* pRuntime) {
  if (!m_pFormFillEnv || !m_pFormFillEnv->GetPDFDocument())
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  return getPropertyInternal(pRuntime, "Title");
}

CJS_Result CJS_Document::set_title(CJS_Runtime* pRuntime,
                                   v8::Local<v8::Value> vp) {
  if (!m_pFormFillEnv || !m_pFormFillEnv->GetPDFDocument())
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  return setPropertyInternal(pRuntime, vp, "Title");
}

CJS_Result CJS_Document::get_num_pages(CJS_Runtime* pRuntime) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  return CJS_Result::Success(
      pRuntime->NewNumber(m_pFormFillEnv->GetPageCount()));
}

CJS_Result CJS_Document::set_num_pages(CJS_Runtime* pRuntime,
                                       v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kReadOnlyError);
}

CJS_Result CJS_Document::get_external(CJS_Runtime* pRuntime) {
  // In Chrome case, should always return true.
  return CJS_Result::Success(pRuntime->NewBoolean(true));
}

CJS_Result CJS_Document::set_external(CJS_Runtime* pRuntime,
                                      v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_filesize(CJS_Runtime* pRuntime) {
  return CJS_Result::Success(pRuntime->NewNumber(0));
}

CJS_Result CJS_Document::set_filesize(CJS_Runtime* pRuntime,
                                      v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kReadOnlyError);
}

CJS_Result CJS_Document::get_mouse_x(CJS_Runtime* pRuntime) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::set_mouse_x(CJS_Runtime* pRuntime,
                                     v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_mouse_y(CJS_Runtime* pRuntime) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::set_mouse_y(CJS_Runtime* pRuntime,
                                     v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_URL(CJS_Runtime* pRuntime) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  return CJS_Result::Success(
      pRuntime->NewString(m_pFormFillEnv->JS_docGetFilePath().AsStringView()));
}

CJS_Result CJS_Document::set_URL(CJS_Runtime* pRuntime,
                                 v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kReadOnlyError);
}

CJS_Result CJS_Document::get_base_URL(CJS_Runtime* pRuntime) {
  return CJS_Result::Success(pRuntime->NewString(m_cwBaseURL.AsStringView()));
}

CJS_Result CJS_Document::set_base_URL(CJS_Runtime* pRuntime,
                                      v8::Local<v8::Value> vp) {
  m_cwBaseURL = pRuntime->ToWideString(vp);
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_calculate(CJS_Runtime* pRuntime) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  CPDFSDK_InteractiveForm* pInteractiveForm = GetSDKInteractiveForm();
  return CJS_Result::Success(
      pRuntime->NewBoolean(!!pInteractiveForm->IsCalculateEnabled()));
}

CJS_Result CJS_Document::set_calculate(CJS_Runtime* pRuntime,
                                       v8::Local<v8::Value> vp) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  CPDFSDK_InteractiveForm* pInteractiveForm = GetSDKInteractiveForm();
  pInteractiveForm->EnableCalculate(pRuntime->ToBoolean(vp));
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_document_file_name(CJS_Runtime* pRuntime) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  WideString wsFilePath = m_pFormFillEnv->JS_docGetFilePath();
  size_t i = wsFilePath.GetLength();
  for (; i > 0; i--) {
    if (wsFilePath[i - 1] == L'\\' || wsFilePath[i - 1] == L'/')
      break;
  }
  if (i > 0 && i < wsFilePath.GetLength())
    return CJS_Result::Success(pRuntime->NewString(wsFilePath.c_str() + i));

  return CJS_Result::Success(pRuntime->NewString(""));
}

CJS_Result CJS_Document::set_document_file_name(CJS_Runtime* pRuntime,
                                                v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kReadOnlyError);
}

CJS_Result CJS_Document::get_path(CJS_Runtime* pRuntime) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  return CJS_Result::Success(pRuntime->NewString(
      CJS_App::SysPathToPDFPath(m_pFormFillEnv->JS_docGetFilePath())
          .AsStringView()));
}

CJS_Result CJS_Document::set_path(CJS_Runtime* pRuntime,
                                  v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kReadOnlyError);
}

CJS_Result CJS_Document::get_page_window_rect(CJS_Runtime* pRuntime) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::set_page_window_rect(CJS_Runtime* pRuntime,
                                              v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_layout(CJS_Runtime* pRuntime) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::set_layout(CJS_Runtime* pRuntime,
                                    v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::addLink(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::closeDoc(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::getPageBox(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::getAnnot(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 2)
    return CJS_Result::Failure(JSMessage::kParamError);
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  int nPageNo = pRuntime->ToInt32(params[0]);
  WideString swAnnotName = pRuntime->ToWideString(params[1]);
  CPDFSDK_PageView* pPageView = m_pFormFillEnv->GetPageView(nPageNo);
  if (!pPageView)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  CPDFSDK_AnnotIteration annotIteration(pPageView, false);
  CPDFSDK_BAAnnot* pSDKBAAnnot = nullptr;
  for (const auto& pSDKAnnotCur : annotIteration) {
    auto* pBAAnnot = pSDKAnnotCur->AsBAAnnot();
    if (pBAAnnot && pBAAnnot->GetAnnotName() == swAnnotName) {
      pSDKBAAnnot = pBAAnnot;
      break;
    }
  }
  if (!pSDKBAAnnot)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  v8::Local<v8::Object> pObj = pRuntime->NewFXJSBoundObject(
      CJS_Annot::GetObjDefnID(), FXJSOBJTYPE_DYNAMIC);
  if (pObj.IsEmpty())
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  auto* pJS_Annot =
      static_cast<CJS_Annot*>(CFXJS_Engine::GetObjectPrivate(pObj));
  if (!pJS_Annot)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  pJS_Annot->SetSDKAnnot(pSDKBAAnnot);
  return CJS_Result::Success(pJS_Annot->ToV8Object());
}

CJS_Result CJS_Document::getAnnots(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  // TODO(tonikitoo): Add support supported parameters as per
  // the PDF spec.

  int nPageNo = m_pFormFillEnv->GetPageCount();
  v8::Local<v8::Array> annots = pRuntime->NewArray();
  for (int i = 0; i < nPageNo; ++i) {
    CPDFSDK_PageView* pPageView = m_pFormFillEnv->GetPageView(i);
    if (!pPageView)
      return CJS_Result::Failure(JSMessage::kBadObjectError);

    CPDFSDK_AnnotIteration annotIteration(pPageView, false);
    for (const auto& pSDKAnnotCur : annotIteration) {
      if (!pSDKAnnotCur)
        return CJS_Result::Failure(JSMessage::kBadObjectError);

      v8::Local<v8::Object> pObj = pRuntime->NewFXJSBoundObject(
          CJS_Annot::GetObjDefnID(), FXJSOBJTYPE_DYNAMIC);
      if (pObj.IsEmpty())
        return CJS_Result::Failure(JSMessage::kBadObjectError);

      auto* pJS_Annot =
          static_cast<CJS_Annot*>(CFXJS_Engine::GetObjectPrivate(pObj));
      pJS_Annot->SetSDKAnnot(pSDKAnnotCur->AsBAAnnot());
      pRuntime->PutArrayElement(
          annots, i,
          pJS_Annot ? v8::Local<v8::Value>(pJS_Annot->ToV8Object())
                    : v8::Local<v8::Value>());
    }
  }
  return CJS_Result::Success(annots);
}

CJS_Result CJS_Document::getAnnot3D(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Success(pRuntime->NewUndefined());
}

CJS_Result CJS_Document::getAnnots3D(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::getOCGs(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::getLinks(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::addIcon(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 2)
    return CJS_Result::Failure(JSMessage::kParamError);

  if (!params[1]->IsObject())
    return CJS_Result::Failure(JSMessage::kTypeError);

  v8::Local<v8::Object> pObj = pRuntime->ToObject(params[1]);
  if (!JSGetObject<CJS_Icon>(pObj))
    return CJS_Result::Failure(JSMessage::kTypeError);

  WideString swIconName = pRuntime->ToWideString(params[0]);
  m_IconNames.push_back(swIconName);
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_icons(CJS_Runtime* pRuntime) {
  if (m_IconNames.empty())
    return CJS_Result::Success(pRuntime->NewUndefined());

  v8::Local<v8::Array> Icons = pRuntime->NewArray();
  int i = 0;
  for (const auto& name : m_IconNames) {
    v8::Local<v8::Object> pObj = pRuntime->NewFXJSBoundObject(
        CJS_Icon::GetObjDefnID(), FXJSOBJTYPE_DYNAMIC);
    if (pObj.IsEmpty())
      return CJS_Result::Failure(JSMessage::kBadObjectError);

    auto* pJS_Icon =
        static_cast<CJS_Icon*>(CFXJS_Engine::GetObjectPrivate(pObj));
    pJS_Icon->SetIconName(name);
    pRuntime->PutArrayElement(Icons, i++,
                              pJS_Icon
                                  ? v8::Local<v8::Value>(pJS_Icon->ToV8Object())
                                  : v8::Local<v8::Value>());
  }
  return CJS_Result::Success(Icons);
}

CJS_Result CJS_Document::set_icons(CJS_Runtime* pRuntime,
                                   v8::Local<v8::Value> vp) {
  return CJS_Result::Failure(JSMessage::kReadOnlyError);
}

CJS_Result CJS_Document::getIcon(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  WideString swIconName = pRuntime->ToWideString(params[0]);
  auto it = std::find(m_IconNames.begin(), m_IconNames.end(), swIconName);
  if (it == m_IconNames.end())
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  v8::Local<v8::Object> pObj = pRuntime->NewFXJSBoundObject(
      CJS_Icon::GetObjDefnID(), FXJSOBJTYPE_DYNAMIC);
  if (pObj.IsEmpty())
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  auto* pJSIcon = static_cast<CJS_Icon*>(CFXJS_Engine::GetObjectPrivate(pObj));
  if (!pJSIcon)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  pJSIcon->SetIconName(*it);
  return CJS_Result::Success(pJSIcon->ToV8Object());
}

CJS_Result CJS_Document::removeIcon(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, no supported.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::createDataObject(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not implemented.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_media(CJS_Runtime* pRuntime) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::set_media(CJS_Runtime* pRuntime,
                                   v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::calculateNow(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  if (!(m_pFormFillEnv->GetPermissions(FPDFPERM_MODIFY) ||
        m_pFormFillEnv->GetPermissions(FPDFPERM_ANNOT_FORM) ||
        m_pFormFillEnv->GetPermissions(FPDFPERM_FILL_FORM))) {
    return CJS_Result::Failure(JSMessage::kPermissionError);
  }

  GetSDKInteractiveForm()->OnCalculate(nullptr);
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_collab(CJS_Runtime* pRuntime) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::set_collab(CJS_Runtime* pRuntime,
                                    v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::getPageNthWord(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  if (!m_pFormFillEnv->GetPermissions(FPDFPERM_EXTRACT_ACCESS))
    return CJS_Result::Failure(JSMessage::kPermissionError);

  // TODO(tsepez): check maximum allowable params.

  int nPageNo = params.size() > 0 ? pRuntime->ToInt32(params[0]) : 0;
  int nWordNo = params.size() > 1 ? pRuntime->ToInt32(params[1]) : 0;
  bool bStrip = params.size() > 2 ? pRuntime->ToBoolean(params[2]) : true;

  CPDF_Document* pDocument = m_pFormFillEnv->GetPDFDocument();
  if (!pDocument)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  if (nPageNo < 0 || nPageNo >= pDocument->GetPageCount())
    return CJS_Result::Failure(JSMessage::kValueError);

  CPDF_Dictionary* pPageDict = pDocument->GetPageDictionary(nPageNo);
  if (!pPageDict)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  auto page = pdfium::MakeRetain<CPDF_Page>(pDocument, pPageDict, true);
  page->ParseContent();

  int nWords = 0;
  WideString swRet;
  for (auto& pPageObj : *page->GetPageObjectList()) {
    if (pPageObj->IsText()) {
      CPDF_TextObject* pTextObj = pPageObj->AsText();
      int nObjWords = CountWords(pTextObj);
      if (nWords + nObjWords >= nWordNo) {
        swRet = GetObjWordStr(pTextObj, nWordNo - nWords);
        break;
      }
      nWords += nObjWords;
    }
  }

  if (bStrip)
    swRet.Trim();
  return CJS_Result::Success(pRuntime->NewString(swRet.AsStringView()));
}

CJS_Result CJS_Document::getPageNthWordQuads(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  if (!m_pFormFillEnv->GetPermissions(FPDFPERM_EXTRACT_ACCESS))
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}

CJS_Result CJS_Document::getPageNumWords(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);
  if (!m_pFormFillEnv->GetPermissions(FPDFPERM_EXTRACT_ACCESS))
    return CJS_Result::Failure(JSMessage::kPermissionError);

  int nPageNo = params.size() > 0 ? pRuntime->ToInt32(params[0]) : 0;
  CPDF_Document* pDocument = m_pFormFillEnv->GetPDFDocument();
  if (nPageNo < 0 || nPageNo >= pDocument->GetPageCount())
    return CJS_Result::Failure(JSMessage::kValueError);

  CPDF_Dictionary* pPageDict = pDocument->GetPageDictionary(nPageNo);
  if (!pPageDict)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  auto page = pdfium::MakeRetain<CPDF_Page>(pDocument, pPageDict, true);
  page->ParseContent();

  int nWords = 0;
  for (auto& pPageObj : *page->GetPageObjectList()) {
    if (pPageObj->IsText())
      nWords += CountWords(pPageObj->AsText());
  }

  return CJS_Result::Success(pRuntime->NewNumber(nWords));
}

CJS_Result CJS_Document::getPrintParams(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Failure(JSMessage::kNotSupportedError);
}

CJS_Result CJS_Document::get_zoom(CJS_Runtime* pRuntime) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::set_zoom(CJS_Runtime* pRuntime,
                                  v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::get_zoom_type(CJS_Runtime* pRuntime) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::set_zoom_type(CJS_Runtime* pRuntime,
                                       v8::Local<v8::Value> vp) {
  return CJS_Result::Success();
}

CJS_Result CJS_Document::deletePages(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::extractPages(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::insertPages(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::replacePages(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::getURL(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Unsafe, not supported.
  return CJS_Result::Success();
}

CJS_Result CJS_Document::gotoNamedDest(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (params.size() != 1)
    return CJS_Result::Failure(JSMessage::kParamError);

  if (!m_pFormFillEnv)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  CPDF_Document* pDocument = m_pFormFillEnv->GetPDFDocument();
  if (!pDocument)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  CPDF_NameTree nameTree(pDocument, "Dests");
  CPDF_Array* destArray =
      nameTree.LookupNamedDest(pDocument, pRuntime->ToWideString(params[0]));
  if (!destArray)
    return CJS_Result::Failure(JSMessage::kBadObjectError);

  CPDF_Dest dest(destArray);
  const CPDF_Array* arrayObject = dest.GetArray();
  std::vector<float> scrollPositionArray;
  if (arrayObject) {
    for (size_t i = 2; i < arrayObject->size(); i++)
      scrollPositionArray.push_back(arrayObject->GetFloatAt(i));
  }
  pRuntime->BeginBlock();
  m_pFormFillEnv->DoGoToAction(dest.GetDestPageIndex(pDocument),
                               dest.GetZoomMode(), scrollPositionArray.data(),
                               scrollPositionArray.size());
  pRuntime->EndBlock();
  return CJS_Result::Success();
}

void CJS_Document::AddDelayData(std::unique_ptr<CJS_DelayData> pData) {
  m_DelayData.push_back(std::move(pData));
}

void CJS_Document::DoFieldDelay(const WideString& sFieldName,
                                int nControlIndex) {
  std::vector<std::unique_ptr<CJS_DelayData>> delayed_data;
  auto iter = m_DelayData.begin();
  while (iter != m_DelayData.end()) {
    auto old = iter++;
    if ((*old)->sFieldName == sFieldName &&
        (*old)->nControlIndex == nControlIndex) {
      delayed_data.push_back(std::move(*old));
      m_DelayData.erase(old);
    }
  }

  for (const auto& pData : delayed_data)
    CJS_Field::DoDelay(m_pFormFillEnv.Get(), pData.get());
}

CPDF_InteractiveForm* CJS_Document::GetCoreInteractiveForm() {
  return GetSDKInteractiveForm()->GetInteractiveForm();
}

CPDFSDK_InteractiveForm* CJS_Document::GetSDKInteractiveForm() {
  return m_pFormFillEnv->GetInteractiveForm();
}
