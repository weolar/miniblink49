// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_CJS_DOCUMENT_H_
#define FXJS_CJS_DOCUMENT_H_

#include <list>
#include <memory>
#include <vector>

#include "core/fxcrt/observable.h"
#include "fxjs/cjs_object.h"
#include "fxjs/js_define.h"

class CPDFSDK_InteractiveForm;
class CPDF_InteractiveForm;
class CPDF_TextObject;
struct CJS_DelayData;

class CJS_Document final : public CJS_Object, public Observable<CJS_Document> {
 public:
  static int GetObjDefnID();
  static void DefineJSObjects(CFXJS_Engine* pEngine);

  CJS_Document(v8::Local<v8::Object> pObject, CJS_Runtime* pRuntime);
  ~CJS_Document() override;

  void SetFormFillEnv(CPDFSDK_FormFillEnvironment* pFormFillEnv);
  CPDFSDK_FormFillEnvironment* GetFormFillEnv() const {
    return m_pFormFillEnv.Get();
  }
  void AddDelayData(std::unique_ptr<CJS_DelayData> pData);
  void DoFieldDelay(const WideString& sFieldName, int nControlIndex);

  JS_STATIC_PROP(ADBE, ADBE, CJS_Document);
  JS_STATIC_PROP(author, author, CJS_Document);
  JS_STATIC_PROP(baseURL, base_URL, CJS_Document);
  JS_STATIC_PROP(bookmarkRoot, bookmark_root, CJS_Document);
  JS_STATIC_PROP(calculate, calculate, CJS_Document);
  JS_STATIC_PROP(Collab, collab, CJS_Document);
  JS_STATIC_PROP(creationDate, creation_date, CJS_Document);
  JS_STATIC_PROP(creator, creator, CJS_Document);
  JS_STATIC_PROP(delay, delay, CJS_Document);
  JS_STATIC_PROP(dirty, dirty, CJS_Document);
  JS_STATIC_PROP(documentFileName, document_file_name, CJS_Document);
  JS_STATIC_PROP(external, external, CJS_Document);
  JS_STATIC_PROP(filesize, filesize, CJS_Document);
  JS_STATIC_PROP(icons, icons, CJS_Document);
  JS_STATIC_PROP(info, info, CJS_Document);
  JS_STATIC_PROP(keywords, keywords, CJS_Document);
  JS_STATIC_PROP(layout, layout, CJS_Document);
  JS_STATIC_PROP(media, media, CJS_Document);
  JS_STATIC_PROP(modDate, mod_date, CJS_Document);
  JS_STATIC_PROP(mouseX, mouse_x, CJS_Document);
  JS_STATIC_PROP(mouseY, mouse_y, CJS_Document);
  JS_STATIC_PROP(numFields, num_fields, CJS_Document);
  JS_STATIC_PROP(numPages, num_pages, CJS_Document);
  JS_STATIC_PROP(pageNum, page_num, CJS_Document);
  JS_STATIC_PROP(pageWindowRect, page_window_rect, CJS_Document);
  JS_STATIC_PROP(path, path, CJS_Document);
  JS_STATIC_PROP(producer, producer, CJS_Document);
  JS_STATIC_PROP(subject, subject, CJS_Document);
  JS_STATIC_PROP(title, title, CJS_Document);
  JS_STATIC_PROP(URL, URL, CJS_Document);
  JS_STATIC_PROP(zoom, zoom, CJS_Document);
  JS_STATIC_PROP(zoomType, zoom_type, CJS_Document);

  JS_STATIC_METHOD(addAnnot, CJS_Document);
  JS_STATIC_METHOD(addField, CJS_Document);
  JS_STATIC_METHOD(addLink, CJS_Document);
  JS_STATIC_METHOD(addIcon, CJS_Document);
  JS_STATIC_METHOD(calculateNow, CJS_Document);
  JS_STATIC_METHOD(closeDoc, CJS_Document);
  JS_STATIC_METHOD(createDataObject, CJS_Document);
  JS_STATIC_METHOD(deletePages, CJS_Document);
  JS_STATIC_METHOD(exportAsText, CJS_Document);
  JS_STATIC_METHOD(exportAsFDF, CJS_Document);
  JS_STATIC_METHOD(exportAsXFDF, CJS_Document);
  JS_STATIC_METHOD(extractPages, CJS_Document);
  JS_STATIC_METHOD(getAnnot, CJS_Document);
  JS_STATIC_METHOD(getAnnots, CJS_Document);
  JS_STATIC_METHOD(getAnnot3D, CJS_Document);
  JS_STATIC_METHOD(getAnnots3D, CJS_Document);
  JS_STATIC_METHOD(getField, CJS_Document);
  JS_STATIC_METHOD(getIcon, CJS_Document);
  JS_STATIC_METHOD(getLinks, CJS_Document);
  JS_STATIC_METHOD(getNthFieldName, CJS_Document);
  JS_STATIC_METHOD(getOCGs, CJS_Document);
  JS_STATIC_METHOD(getPageBox, CJS_Document);
  JS_STATIC_METHOD(getPageNthWord, CJS_Document);
  JS_STATIC_METHOD(getPageNthWordQuads, CJS_Document);
  JS_STATIC_METHOD(getPageNumWords, CJS_Document);
  JS_STATIC_METHOD(getPrintParams, CJS_Document);
  JS_STATIC_METHOD(getURL, CJS_Document);
  JS_STATIC_METHOD(gotoNamedDest, CJS_Document);
  JS_STATIC_METHOD(importAnFDF, CJS_Document);
  JS_STATIC_METHOD(importAnXFDF, CJS_Document);
  JS_STATIC_METHOD(importTextData, CJS_Document);
  JS_STATIC_METHOD(insertPages, CJS_Document);
  JS_STATIC_METHOD(mailDoc, CJS_Document);
  JS_STATIC_METHOD(mailForm, CJS_Document);
  JS_STATIC_METHOD(print, CJS_Document);
  JS_STATIC_METHOD(removeField, CJS_Document);
  JS_STATIC_METHOD(replacePages, CJS_Document);
  JS_STATIC_METHOD(removeIcon, CJS_Document);
  JS_STATIC_METHOD(resetForm, CJS_Document);
  JS_STATIC_METHOD(saveAs, CJS_Document);
  JS_STATIC_METHOD(submitForm, CJS_Document);
  JS_STATIC_METHOD(syncAnnotScan, CJS_Document);

 private:
  static int ObjDefnID;
  static const char kName[];
  static const JSPropertySpec PropertySpecs[];
  static const JSMethodSpec MethodSpecs[];

  CJS_Result get_ADBE(CJS_Runtime* pRuntime);
  CJS_Result set_ADBE(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_author(CJS_Runtime* pRuntime);
  CJS_Result set_author(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_base_URL(CJS_Runtime* pRuntime);
  CJS_Result set_base_URL(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_bookmark_root(CJS_Runtime* pRuntime);
  CJS_Result set_bookmark_root(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_calculate(CJS_Runtime* pRuntime);
  CJS_Result set_calculate(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_collab(CJS_Runtime* pRuntime);
  CJS_Result set_collab(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_creation_date(CJS_Runtime* pRuntime);
  CJS_Result set_creation_date(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_creator(CJS_Runtime* pRuntime);
  CJS_Result set_creator(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_delay(CJS_Runtime* pRuntime);
  CJS_Result set_delay(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_dirty(CJS_Runtime* pRuntime);
  CJS_Result set_dirty(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_document_file_name(CJS_Runtime* pRuntime);
  CJS_Result set_document_file_name(CJS_Runtime* pRuntime,
                                    v8::Local<v8::Value> vp);

  CJS_Result get_external(CJS_Runtime* pRuntime);
  CJS_Result set_external(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_filesize(CJS_Runtime* pRuntime);
  CJS_Result set_filesize(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_icons(CJS_Runtime* pRuntime);
  CJS_Result set_icons(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_info(CJS_Runtime* pRuntime);
  CJS_Result set_info(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_keywords(CJS_Runtime* pRuntime);
  CJS_Result set_keywords(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_layout(CJS_Runtime* pRuntime);
  CJS_Result set_layout(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_media(CJS_Runtime* pRuntime);
  CJS_Result set_media(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_mod_date(CJS_Runtime* pRuntime);
  CJS_Result set_mod_date(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_mouse_x(CJS_Runtime* pRuntime);
  CJS_Result set_mouse_x(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_mouse_y(CJS_Runtime* pRuntime);
  CJS_Result set_mouse_y(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_num_fields(CJS_Runtime* pRuntime);
  CJS_Result set_num_fields(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_num_pages(CJS_Runtime* pRuntime);
  CJS_Result set_num_pages(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_page_num(CJS_Runtime* pRuntime);
  CJS_Result set_page_num(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_page_window_rect(CJS_Runtime* pRuntime);
  CJS_Result set_page_window_rect(CJS_Runtime* pRuntime,
                                  v8::Local<v8::Value> vp);

  CJS_Result get_path(CJS_Runtime* pRuntime);
  CJS_Result set_path(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_producer(CJS_Runtime* pRuntime);
  CJS_Result set_producer(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_subject(CJS_Runtime* pRuntime);
  CJS_Result set_subject(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_title(CJS_Runtime* pRuntime);
  CJS_Result set_title(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_zoom(CJS_Runtime* pRuntime);
  CJS_Result set_zoom(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_zoom_type(CJS_Runtime* pRuntime);
  CJS_Result set_zoom_type(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result get_URL(CJS_Runtime* pRuntime);
  CJS_Result set_URL(CJS_Runtime* pRuntime, v8::Local<v8::Value> vp);

  CJS_Result addAnnot(CJS_Runtime* pRuntime,
                      const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result addField(CJS_Runtime* pRuntime,
                      const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result addLink(CJS_Runtime* pRuntime,
                     const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result addIcon(CJS_Runtime* pRuntime,
                     const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result calculateNow(CJS_Runtime* pRuntime,
                          const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result closeDoc(CJS_Runtime* pRuntime,
                      const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result createDataObject(CJS_Runtime* pRuntime,
                              const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result deletePages(CJS_Runtime* pRuntime,
                         const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result exportAsText(CJS_Runtime* pRuntime,
                          const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result exportAsFDF(CJS_Runtime* pRuntime,
                         const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result exportAsXFDF(CJS_Runtime* pRuntime,
                          const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result extractPages(CJS_Runtime* pRuntime,
                          const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getAnnot(CJS_Runtime* pRuntime,
                      const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getAnnots(CJS_Runtime* pRuntime,
                       const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getAnnot3D(CJS_Runtime* pRuntime,
                        const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getAnnots3D(CJS_Runtime* pRuntime,
                         const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getField(CJS_Runtime* pRuntime,
                      const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getIcon(CJS_Runtime* pRuntime,
                     const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getLinks(CJS_Runtime* pRuntime,
                      const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getNthFieldName(CJS_Runtime* pRuntime,
                             const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getOCGs(CJS_Runtime* pRuntime,
                     const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getPageBox(CJS_Runtime* pRuntime,
                        const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getPageNthWord(CJS_Runtime* pRuntime,
                            const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getPageNthWordQuads(
      CJS_Runtime* pRuntime,
      const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getPageNumWords(CJS_Runtime* pRuntime,
                             const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getPrintParams(CJS_Runtime* pRuntime,
                            const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result getURL(CJS_Runtime* pRuntime,
                    const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result gotoNamedDest(CJS_Runtime* pRuntime,
                           const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result importAnFDF(CJS_Runtime* pRuntime,
                         const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result importAnXFDF(CJS_Runtime* pRuntime,
                          const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result importTextData(CJS_Runtime* pRuntime,
                            const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result insertPages(CJS_Runtime* pRuntime,
                         const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result mailForm(CJS_Runtime* pRuntime,
                      const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result print(CJS_Runtime* pRuntime,
                   const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result removeField(CJS_Runtime* pRuntime,
                         const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result replacePages(CJS_Runtime* pRuntime,
                          const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result resetForm(CJS_Runtime* pRuntime,
                       const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result saveAs(CJS_Runtime* pRuntime,
                    const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result submitForm(CJS_Runtime* pRuntime,
                        const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result syncAnnotScan(CJS_Runtime* pRuntime,
                           const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result mailDoc(CJS_Runtime* pRuntime,
                     const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result removeIcon(CJS_Runtime* pRuntime,
                        const std::vector<v8::Local<v8::Value>>& params);

  CJS_Result getPropertyInternal(CJS_Runtime* pRuntime,
                                 const ByteString& propName);
  CJS_Result setPropertyInternal(CJS_Runtime* pRuntime,
                                 v8::Local<v8::Value> vp,
                                 const ByteString& propName);

  CPDF_InteractiveForm* GetCoreInteractiveForm();
  CPDFSDK_InteractiveForm* GetSDKInteractiveForm();

  WideString m_cwBaseURL;
  CPDFSDK_FormFillEnvironment::ObservedPtr m_pFormFillEnv;
  std::list<std::unique_ptr<CJS_DelayData>> m_DelayData;
  // Needs to be a std::list for iterator stability.
  std::list<WideString> m_IconNames;
  bool m_bDelay = false;
};

#endif  // FXJS_CJS_DOCUMENT_H_
