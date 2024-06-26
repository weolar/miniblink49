// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_FPDF_PARSER_DECODE_H_
#define CORE_FPDFAPI_PARSER_FPDF_PARSER_DECODE_H_

#include <memory>

#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/unowned_ptr.h"
#include "third_party/base/span.h"

class CCodec_ScanlineDecoder;
class CPDF_Array;
class CPDF_Dictionary;

// Indexed by 8-bit char code, contains unicode code points.
extern const uint16_t PDFDocEncoding[256];

bool ValidateDecoderPipeline(const CPDF_Array* pDecoders);

ByteString PDF_EncodeString(const ByteString& src, bool bHex);
WideString PDF_DecodeText(pdfium::span<const uint8_t> span);
ByteString PDF_EncodeText(const WideString& str);

std::unique_ptr<CCodec_ScanlineDecoder> CreateFaxDecoder(
    pdfium::span<const uint8_t> src_span,
    int width,
    int height,
    const CPDF_Dictionary* pParams);

std::unique_ptr<CCodec_ScanlineDecoder> CreateFlateDecoder(
    pdfium::span<const uint8_t> src_span,
    int width,
    int height,
    int nComps,
    int bpc,
    const CPDF_Dictionary* pParams);

bool FlateEncode(pdfium::span<const uint8_t> src_span,
                 std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                 uint32_t* dest_size);

uint32_t FlateDecode(pdfium::span<const uint8_t> src_span,
                     std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                     uint32_t* dest_size);

uint32_t RunLengthDecode(pdfium::span<const uint8_t> src_span,
                         std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                         uint32_t* dest_size);

uint32_t A85Decode(pdfium::span<const uint8_t> src_span,
                   std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                   uint32_t* dest_size);

uint32_t HexDecode(pdfium::span<const uint8_t> src_span,
                   std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                   uint32_t* dest_size);

uint32_t FlateOrLZWDecode(bool bLZW,
                          pdfium::span<const uint8_t> src_span,
                          const CPDF_Dictionary* pParams,
                          uint32_t estimated_size,
                          std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                          uint32_t* dest_size);

bool PDF_DataDecode(pdfium::span<const uint8_t> src_span,
                    const CPDF_Dictionary* pDict,
                    uint32_t estimated_size,
                    bool bImageAcc,
                    std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                    uint32_t* dest_size,
                    ByteString* ImageEncoding,
                    UnownedPtr<const CPDF_Dictionary>* pImageParms);

#endif  // CORE_FPDFAPI_PARSER_FPDF_PARSER_DECODE_H_
