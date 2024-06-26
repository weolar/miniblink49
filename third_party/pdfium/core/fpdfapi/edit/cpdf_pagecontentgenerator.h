// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_EDIT_CPDF_PAGECONTENTGENERATOR_H_
#define CORE_FPDFAPI_EDIT_CPDF_PAGECONTENTGENERATOR_H_

#include <map>
#include <memory>
#include <sstream>
#include <vector>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"

class CPDF_ContentMarks;
class CPDF_Document;
class CPDF_ImageObject;
class CPDF_Object;
class CPDF_PageObject;
class CPDF_PageObjectHolder;
class CPDF_PathObject;
class CPDF_TextObject;

class CPDF_PageContentGenerator {
 public:
  explicit CPDF_PageContentGenerator(CPDF_PageObjectHolder* pObjHolder);
  ~CPDF_PageContentGenerator();

  void GenerateContent();
  bool ProcessPageObjects(std::ostringstream* buf);

 private:
  friend class CPDF_PageContentGeneratorTest;

  void ProcessPageObject(std::ostringstream* buf, CPDF_PageObject* pPageObj);
  void ProcessPath(std::ostringstream* buf, CPDF_PathObject* pPathObj);
  void ProcessImage(std::ostringstream* buf, CPDF_ImageObject* pImageObj);
  void ProcessGraphics(std::ostringstream* buf, CPDF_PageObject* pPageObj);
  void ProcessDefaultGraphics(std::ostringstream* buf);
  void ProcessText(std::ostringstream* buf, CPDF_TextObject* pTextObj);
  ByteString GetOrCreateDefaultGraphics() const;
  ByteString RealizeResource(const CPDF_Object* pResource,
                             const ByteString& bsType) const;
  const CPDF_ContentMarks* ProcessContentMarks(std::ostringstream* buf,
                                               const CPDF_PageObject* pPageObj,
                                               const CPDF_ContentMarks* pPrev);
  void FinishMarks(std::ostringstream* buf,
                   const CPDF_ContentMarks* pContentMarks);

  // Returns a map from content stream index to new stream data. Unmodified
  // streams are not touched.
  std::map<int32_t, std::unique_ptr<std::ostringstream>>
  GenerateModifiedStreams();

  // Add buffer as a stream in page's 'Contents'
  void UpdateContentStreams(
      std::map<int32_t, std::unique_ptr<std::ostringstream>>* buf);

  // Set the stream index of all page objects with stream index ==
  // |CPDF_PageObject::kNoContentStream|. These are new objects that had not
  // been parsed from or written to any content stream yet.
  void UpdateStreamlessPageObjects(int new_content_stream_index);

  UnownedPtr<CPDF_PageObjectHolder> const m_pObjHolder;
  UnownedPtr<CPDF_Document> const m_pDocument;
  std::vector<UnownedPtr<CPDF_PageObject>> m_pageObjects;
};

#endif  // CORE_FPDFAPI_EDIT_CPDF_PAGECONTENTGENERATOR_H_
