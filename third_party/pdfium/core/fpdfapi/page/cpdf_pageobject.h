// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_PAGEOBJECT_H_
#define CORE_FPDFAPI_PAGE_CPDF_PAGEOBJECT_H_

#include "core/fpdfapi/page/cpdf_contentmarks.h"
#include "core/fpdfapi/page/cpdf_graphicstates.h"
#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_system.h"

class CPDF_TextObject;
class CPDF_PathObject;
class CPDF_ImageObject;
class CPDF_ShadingObject;
class CPDF_FormObject;

class CPDF_PageObject : public CPDF_GraphicStates {
 public:
  enum Type {
    TEXT = 1,
    PATH,
    IMAGE,
    SHADING,
    FORM,
  };

  static constexpr int32_t kNoContentStream = -1;

  explicit CPDF_PageObject(int32_t content_stream);
  CPDF_PageObject();
  ~CPDF_PageObject() override;

  virtual Type GetType() const = 0;
  virtual void Transform(const CFX_Matrix& matrix) = 0;
  virtual bool IsText() const;
  virtual bool IsPath() const;
  virtual bool IsImage() const;
  virtual bool IsShading() const;
  virtual bool IsForm() const;
  virtual CPDF_TextObject* AsText();
  virtual const CPDF_TextObject* AsText() const;
  virtual CPDF_PathObject* AsPath();
  virtual const CPDF_PathObject* AsPath() const;
  virtual CPDF_ImageObject* AsImage();
  virtual const CPDF_ImageObject* AsImage() const;
  virtual CPDF_ShadingObject* AsShading();
  virtual const CPDF_ShadingObject* AsShading() const;
  virtual CPDF_FormObject* AsForm();
  virtual const CPDF_FormObject* AsForm() const;

  void SetDirty(bool value) { m_bDirty = value; }
  bool IsDirty() const { return m_bDirty; }
  void TransformClipPath(const CFX_Matrix& matrix);
  void TransformGeneralState(const CFX_Matrix& matrix);

  void SetRect(const CFX_FloatRect& rect) { m_Rect = rect; }
  const CFX_FloatRect& GetRect() const { return m_Rect; }
  FX_RECT GetBBox() const;
  FX_RECT GetTransformedBBox(const CFX_Matrix& matrix) const;

  // Get what content stream the object was parsed from in its page. This number
  // is the index of the content stream in the "Contents" array, or 0 if there
  // is a single content stream. If the object is newly created,
  // |kNoContentStream| is returned.
  //
  // If the object is spread among more than one content stream, this is the
  // index of the last stream.
  int32_t GetContentStream() const { return m_ContentStream; }
  void SetContentStream(int32_t new_content_stream) {
    m_ContentStream = new_content_stream;
  }

  CPDF_ContentMarks m_ContentMarks;

 protected:
  void CopyData(const CPDF_PageObject* pSrcObject);

  CFX_FloatRect m_Rect;

 private:
  CPDF_PageObject(const CPDF_PageObject& src) = delete;
  void operator=(const CPDF_PageObject& src) = delete;

  bool m_bDirty = false;
  int32_t m_ContentStream;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_PAGEOBJECT_H_
