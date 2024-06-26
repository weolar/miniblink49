// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_dataavail.h"

#include <memory>
#include <utility>

#include "core/fpdfapi/parser/cpdf_data_avail.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fxcrt/fx_stream.h"
#include "core/fxcrt/retain_ptr.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "public/fpdf_formfill.h"
#include "third_party/base/ptr_util.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/cpdfxfa_context.h"
#endif  // PDF_ENABLE_XFA

// These checks are here because core/ and public/ cannot depend on each other.
static_assert(CPDF_DataAvail::DataError == PDF_DATA_ERROR,
              "CPDF_DataAvail::DataError value mismatch");
static_assert(CPDF_DataAvail::DataNotAvailable == PDF_DATA_NOTAVAIL,
              "CPDF_DataAvail::DataNotAvailable value mismatch");
static_assert(CPDF_DataAvail::DataAvailable == PDF_DATA_AVAIL,
              "CPDF_DataAvail::DataAvailable value mismatch");

static_assert(CPDF_DataAvail::LinearizationUnknown == PDF_LINEARIZATION_UNKNOWN,
              "CPDF_DataAvail::LinearizationUnknown value mismatch");
static_assert(CPDF_DataAvail::NotLinearized == PDF_NOT_LINEARIZED,
              "CPDF_DataAvail::NotLinearized value mismatch");
static_assert(CPDF_DataAvail::Linearized == PDF_LINEARIZED,
              "CPDF_DataAvail::Linearized value mismatch");

static_assert(CPDF_DataAvail::FormError == PDF_FORM_ERROR,
              "CPDF_DataAvail::FormError value mismatch");
static_assert(CPDF_DataAvail::FormNotAvailable == PDF_FORM_NOTAVAIL,
              "CPDF_DataAvail::FormNotAvailable value mismatch");
static_assert(CPDF_DataAvail::FormAvailable == PDF_FORM_AVAIL,
              "CPDF_DataAvail::FormAvailable value mismatch");
static_assert(CPDF_DataAvail::FormNotExist == PDF_FORM_NOTEXIST,
              "CPDF_DataAvail::FormNotExist value mismatch");

namespace {

class FPDF_FileAvailContext final : public CPDF_DataAvail::FileAvail {
 public:
  FPDF_FileAvailContext() : m_pfileAvail(nullptr) {}
  ~FPDF_FileAvailContext() override {}

  void Set(FX_FILEAVAIL* pfileAvail) { m_pfileAvail = pfileAvail; }

  // CPDF_DataAvail::FileAvail:
  bool IsDataAvail(FX_FILESIZE offset, size_t size) override {
    return !!m_pfileAvail->IsDataAvail(m_pfileAvail, offset, size);
  }

 private:
  FX_FILEAVAIL* m_pfileAvail;
};

class FPDF_FileAccessContext final : public IFX_SeekableReadStream {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  void Set(FPDF_FILEACCESS* pFile) { m_pFileAccess = pFile; }

  // IFX_SeekableReadStream
  FX_FILESIZE GetSize() override { return m_pFileAccess->m_FileLen; }

  bool ReadBlockAtOffset(void* buffer,
                         FX_FILESIZE offset,
                         size_t size) override {
    if (!buffer || offset < 0 || !size)
      return false;

    FX_SAFE_FILESIZE new_pos = pdfium::base::checked_cast<FX_FILESIZE>(size);
    new_pos += offset;
    return new_pos.IsValid() && new_pos.ValueOrDie() <= GetSize() &&
           m_pFileAccess->m_GetBlock(m_pFileAccess->m_Param, offset,
                                     static_cast<uint8_t*>(buffer), size);
  }

 private:
  FPDF_FileAccessContext() : m_pFileAccess(nullptr) {}
  ~FPDF_FileAccessContext() override = default;

  FPDF_FILEACCESS* m_pFileAccess;
};

class FPDF_DownloadHintsContext final : public CPDF_DataAvail::DownloadHints {
 public:
  explicit FPDF_DownloadHintsContext(FX_DOWNLOADHINTS* pDownloadHints) {
    m_pDownloadHints = pDownloadHints;
  }
  ~FPDF_DownloadHintsContext() override {}

 public:
  // IFX_DownloadHints
  void AddSegment(FX_FILESIZE offset, size_t size) override {
    if (m_pDownloadHints)
      m_pDownloadHints->AddSegment(m_pDownloadHints, offset, size);
  }

 private:
  FX_DOWNLOADHINTS* m_pDownloadHints;
};

class FPDF_AvailContext {
 public:
  FPDF_AvailContext()
      : m_FileAvail(pdfium::MakeUnique<FPDF_FileAvailContext>()),
        m_FileRead(pdfium::MakeRetain<FPDF_FileAccessContext>()) {}
  ~FPDF_AvailContext() {}

  std::unique_ptr<FPDF_FileAvailContext> m_FileAvail;
  RetainPtr<FPDF_FileAccessContext> m_FileRead;
  std::unique_ptr<CPDF_DataAvail> m_pDataAvail;
};

FPDF_AvailContext* FPDFAvailContextFromFPDFAvail(FPDF_AVAIL avail) {
  return static_cast<FPDF_AvailContext*>(avail);
}

}  // namespace

FPDF_EXPORT FPDF_AVAIL FPDF_CALLCONV FPDFAvail_Create(FX_FILEAVAIL* file_avail,
                                                      FPDF_FILEACCESS* file) {
  auto pAvail = pdfium::MakeUnique<FPDF_AvailContext>();
  pAvail->m_FileAvail->Set(file_avail);
  pAvail->m_FileRead->Set(file);
  pAvail->m_pDataAvail = pdfium::MakeUnique<CPDF_DataAvail>(
      pAvail->m_FileAvail.get(), pAvail->m_FileRead, true);
  return pAvail.release();  // Caller takes ownership.
}

FPDF_EXPORT void FPDF_CALLCONV FPDFAvail_Destroy(FPDF_AVAIL avail) {
  // Take ownership back from caller and destroy.
  std::unique_ptr<FPDF_AvailContext>(FPDFAvailContextFromFPDFAvail(avail));
}

FPDF_EXPORT int FPDF_CALLCONV FPDFAvail_IsDocAvail(FPDF_AVAIL avail,
                                                   FX_DOWNLOADHINTS* hints) {
  if (!avail)
    return PDF_DATA_ERROR;
  FPDF_DownloadHintsContext hints_context(hints);
  return FPDFAvailContextFromFPDFAvail(avail)->m_pDataAvail->IsDocAvail(
      &hints_context);
}

FPDF_EXPORT FPDF_DOCUMENT FPDF_CALLCONV
FPDFAvail_GetDocument(FPDF_AVAIL avail, FPDF_BYTESTRING password) {
  auto* pDataAvail = FPDFAvailContextFromFPDFAvail(avail);
  if (!pDataAvail)
    return nullptr;
  CPDF_Parser::Error error;
  std::unique_ptr<CPDF_Document> document;
  std::tie(error, document) = pDataAvail->m_pDataAvail->ParseDocument(password);
  if (error != CPDF_Parser::SUCCESS) {
    ProcessParseError(error);
    return nullptr;
  }

#ifdef PDF_ENABLE_XFA
  document->SetExtension(pdfium::MakeUnique<CPDFXFA_Context>(document.get()));
#endif  // PDF_ENABLE_XFA

  ReportUnsupportedFeatures(document.get());
  return FPDFDocumentFromCPDFDocument(document.release());
}

FPDF_EXPORT int FPDF_CALLCONV FPDFAvail_GetFirstPageNum(FPDF_DOCUMENT doc) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(doc);
  return pDoc ? pDoc->GetParser()->GetFirstPageNo() : 0;
}

FPDF_EXPORT int FPDF_CALLCONV FPDFAvail_IsPageAvail(FPDF_AVAIL avail,
                                                    int page_index,
                                                    FX_DOWNLOADHINTS* hints) {
  if (!avail)
    return PDF_DATA_ERROR;
  if (page_index < 0)
    return PDF_DATA_NOTAVAIL;
  FPDF_DownloadHintsContext hints_context(hints);
  return FPDFAvailContextFromFPDFAvail(avail)->m_pDataAvail->IsPageAvail(
      page_index, &hints_context);
}

FPDF_EXPORT int FPDF_CALLCONV FPDFAvail_IsFormAvail(FPDF_AVAIL avail,
                                                    FX_DOWNLOADHINTS* hints) {
  if (!avail)
    return PDF_FORM_ERROR;
  FPDF_DownloadHintsContext hints_context(hints);
  return FPDFAvailContextFromFPDFAvail(avail)->m_pDataAvail->IsFormAvail(
      &hints_context);
}

FPDF_EXPORT int FPDF_CALLCONV FPDFAvail_IsLinearized(FPDF_AVAIL avail) {
  if (!avail)
    return PDF_LINEARIZATION_UNKNOWN;
  return FPDFAvailContextFromFPDFAvail(avail)->m_pDataAvail->IsLinearizedPDF();
}
