// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <limits>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/font/cpdf_type1font.h"
#include "core/fpdfapi/page/cpdf_docpagedata.h"
#include "core/fpdfapi/page/cpdf_textobject.h"
#include "core/fpdfapi/page/cpdf_textstate.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdftext/cpdf_textpage.h"
#include "core/fxcrt/fx_extension.h"
#include "core/fxge/cfx_fontmgr.h"
#include "core/fxge/fx_font.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "public/fpdf_edit.h"
#include "third_party/base/ptr_util.h"

// These checks are here because core/ and public/ cannot depend on each other.
static_assert(static_cast<int>(TextRenderingMode::MODE_FILL) ==
                  FPDF_TEXTRENDERMODE_FILL,
              "TextRenderingMode::MODE_FILL value mismatch");
static_assert(static_cast<int>(TextRenderingMode::MODE_STROKE) ==
                  FPDF_TEXTRENDERMODE_STROKE,
              "TextRenderingMode::MODE_STROKE value mismatch");
static_assert(static_cast<int>(TextRenderingMode::MODE_FILL_STROKE) ==
                  FPDF_TEXTRENDERMODE_FILL_STROKE,
              "TextRenderingMode::MODE_FILL_STROKE value mismatch");
static_assert(static_cast<int>(TextRenderingMode::MODE_INVISIBLE) ==
                  FPDF_TEXTRENDERMODE_INVISIBLE,
              "TextRenderingMode::MODE_INVISIBLE value mismatch");
static_assert(static_cast<int>(TextRenderingMode::MODE_FILL_CLIP) ==
                  FPDF_TEXTRENDERMODE_FILL_CLIP,
              "TextRenderingMode::MODE_FILL_CLIP value mismatch");
static_assert(static_cast<int>(TextRenderingMode::MODE_STROKE_CLIP) ==
                  FPDF_TEXTRENDERMODE_STROKE_CLIP,
              "TextRenderingMode::MODE_STROKE_CLIP value mismatch");
static_assert(static_cast<int>(TextRenderingMode::MODE_FILL_STROKE_CLIP) ==
                  FPDF_TEXTRENDERMODE_FILL_STROKE_CLIP,
              "TextRenderingMode::MODE_FILL_STROKE_CLIP value mismatch");
static_assert(static_cast<int>(TextRenderingMode::MODE_CLIP) ==
                  FPDF_TEXTRENDERMODE_CLIP,
              "TextRenderingMode::MODE_CLIP value mismatch");
namespace {

CPDF_Dictionary* LoadFontDesc(CPDF_Document* pDoc,
                              const ByteString& font_name,
                              CFX_Font* pFont,
                              pdfium::span<const uint8_t> span,
                              int font_type) {
  CPDF_Dictionary* pFontDesc = pDoc->NewIndirect<CPDF_Dictionary>();
  pFontDesc->SetNewFor<CPDF_Name>("Type", "FontDescriptor");
  pFontDesc->SetNewFor<CPDF_Name>("FontName", font_name);
  int flags = 0;
  if (FXFT_Is_Face_fixedwidth(pFont->GetFace()))
    flags |= FXFONT_FIXED_PITCH;
  if (font_name.Contains("Serif"))
    flags |= FXFONT_SERIF;
  if (FXFT_Is_Face_Italic(pFont->GetFace()))
    flags |= FXFONT_ITALIC;
  if (FXFT_Is_Face_Bold(pFont->GetFace()))
    flags |= FXFONT_BOLD;

  // TODO(npm): How do I know if a  font is symbolic, script, allcap, smallcap
  flags |= FXFONT_NONSYMBOLIC;

  pFontDesc->SetNewFor<CPDF_Number>("Flags", flags);
  FX_RECT bbox;
  pFont->GetBBox(&bbox);
  pFontDesc->SetRectFor("FontBBox", CFX_FloatRect(bbox));

  // TODO(npm): calculate italic angle correctly
  pFontDesc->SetNewFor<CPDF_Number>("ItalicAngle", pFont->IsItalic() ? -12 : 0);

  pFontDesc->SetNewFor<CPDF_Number>("Ascent", pFont->GetAscent());
  pFontDesc->SetNewFor<CPDF_Number>("Descent", pFont->GetDescent());

  // TODO(npm): calculate the capheight, stemV correctly
  pFontDesc->SetNewFor<CPDF_Number>("CapHeight", pFont->GetAscent());
  pFontDesc->SetNewFor<CPDF_Number>("StemV", pFont->IsBold() ? 120 : 70);

  CPDF_Stream* pStream = pDoc->NewIndirect<CPDF_Stream>();
  pStream->SetData(span);
  // TODO(npm): Lengths for Type1 fonts.
  if (font_type == FPDF_FONT_TRUETYPE) {
    pStream->GetDict()->SetNewFor<CPDF_Number>("Length1",
                                               static_cast<int>(span.size()));
  }
  ByteString fontFile = font_type == FPDF_FONT_TYPE1 ? "FontFile" : "FontFile2";
  pFontDesc->SetFor(fontFile, pStream->MakeReference(pDoc));
  return pFontDesc;
}

const char ToUnicodeStart[] =
    "/CIDInit /ProcSet findresource begin\n"
    "12 dict begin\n"
    "begincmap\n"
    "/CIDSystemInfo\n"
    "<</Registry (Adobe)\n"
    "/Ordering (Identity)\n"
    "/Supplement 0\n"
    ">> def\n"
    "/CMapName /Adobe-Identity-H def\n"
    "CMapType 2 def\n"
    "1 begincodespacerange\n"
    "<0000> <FFFFF>\n"
    "endcodespacerange\n";

const char ToUnicodeEnd[] =
    "endcmap\n"
    "CMapName currentdict /CMap defineresource pop\n"
    "end\n"
    "end\n";

void AddCharcode(std::ostringstream* pBuffer, uint32_t number) {
  ASSERT(number <= 0xFFFF);
  *pBuffer << "<";
  char ans[4];
  FXSYS_IntToFourHexChars(number, ans);
  for (size_t i = 0; i < 4; ++i)
    *pBuffer << ans[i];
  *pBuffer << ">";
}

// PDF spec 1.7 Section 5.9.2: "Unicode character sequences as expressed in
// UTF-16BE encoding." See https://en.wikipedia.org/wiki/UTF-16#Description
void AddUnicode(std::ostringstream* pBuffer, uint32_t unicode) {
  if (unicode >= 0xD800 && unicode <= 0xDFFF)
    unicode = 0;

  char ans[8];
  *pBuffer << "<";
  size_t numChars = FXSYS_ToUTF16BE(unicode, ans);
  for (size_t i = 0; i < numChars; ++i)
    *pBuffer << ans[i];
  *pBuffer << ">";
}

// Loads the charcode to unicode mapping into a stream
CPDF_Stream* LoadUnicode(CPDF_Document* pDoc,
                         const std::map<uint32_t, uint32_t>& to_unicode) {
  // A map charcode->unicode
  std::map<uint32_t, uint32_t> char_to_uni;
  // A map <char_start, char_end> to vector v of unicode characters of size (end
  // - start + 1). This abbreviates: start->v[0], start+1->v[1], etc. PDF spec
  // 1.7 Section 5.9.2 says that only the last byte of the unicode may change.
  std::map<std::pair<uint32_t, uint32_t>, std::vector<uint32_t>>
      map_range_vector;
  // A map <start, end> -> unicode
  // This abbreviates: start->unicode, start+1->unicode+1, etc.
  // PDF spec 1.7 Section 5.9.2 says that only the last byte of the unicode may
  // change.
  std::map<std::pair<uint32_t, uint32_t>, uint32_t> map_range;

  // Calculate the maps
  for (auto iter = to_unicode.begin(); iter != to_unicode.end(); ++iter) {
    uint32_t firstCharcode = iter->first;
    uint32_t firstUnicode = iter->second;
    if (std::next(iter) == to_unicode.end() ||
        firstCharcode + 1 != std::next(iter)->first) {
      char_to_uni[firstCharcode] = firstUnicode;
      continue;
    }
    ++iter;
    uint32_t curCharcode = iter->first;
    uint32_t curUnicode = iter->second;
    if (curCharcode % 256 == 0) {
      char_to_uni[firstCharcode] = firstUnicode;
      char_to_uni[curCharcode] = curUnicode;
      continue;
    }
    const size_t maxExtra = 255 - (curCharcode % 256);
    auto next_it = std::next(iter);
    if (firstUnicode + 1 != curUnicode) {
      // Consecutive charcodes mapping to non-consecutive unicodes
      std::vector<uint32_t> unicodes;
      unicodes.push_back(firstUnicode);
      unicodes.push_back(curUnicode);
      for (size_t i = 0; i < maxExtra; ++i) {
        if (next_it == to_unicode.end() || curCharcode + 1 != next_it->first)
          break;
        ++iter;
        ++curCharcode;
        unicodes.push_back(iter->second);
        next_it = std::next(iter);
      }
      ASSERT(iter->first - firstCharcode + 1 == unicodes.size());
      map_range_vector[std::make_pair(firstCharcode, iter->first)] = unicodes;
      continue;
    }
    // Consecutive charcodes mapping to consecutive unicodes
    for (size_t i = 0; i < maxExtra; ++i) {
      if (next_it == to_unicode.end() || curCharcode + 1 != next_it->first ||
          curUnicode + 1 != next_it->second) {
        break;
      }
      ++iter;
      ++curCharcode;
      ++curUnicode;
      next_it = std::next(iter);
    }
    map_range[std::make_pair(firstCharcode, curCharcode)] = firstUnicode;
  }
  std::ostringstream buffer;
  buffer << ToUnicodeStart;
  // Add maps to buffer
  buffer << static_cast<uint32_t>(char_to_uni.size()) << " beginbfchar\n";
  for (const auto& iter : char_to_uni) {
    AddCharcode(&buffer, iter.first);
    buffer << " ";
    AddUnicode(&buffer, iter.second);
    buffer << "\n";
  }
  buffer << "endbfchar\n"
         << static_cast<uint32_t>(map_range_vector.size() + map_range.size())
         << " beginbfrange\n";
  for (const auto& iter : map_range_vector) {
    const std::pair<uint32_t, uint32_t>& charcodeRange = iter.first;
    AddCharcode(&buffer, charcodeRange.first);
    buffer << " ";
    AddCharcode(&buffer, charcodeRange.second);
    buffer << " [";
    const std::vector<uint32_t>& unicodes = iter.second;
    for (size_t i = 0; i < unicodes.size(); ++i) {
      uint32_t uni = unicodes[i];
      AddUnicode(&buffer, uni);
      if (i != unicodes.size() - 1)
        buffer << " ";
    }
    buffer << "]\n";
  }
  for (const auto& iter : map_range) {
    const std::pair<uint32_t, uint32_t>& charcodeRange = iter.first;
    AddCharcode(&buffer, charcodeRange.first);
    buffer << " ";
    AddCharcode(&buffer, charcodeRange.second);
    buffer << " ";
    AddUnicode(&buffer, iter.second);
    buffer << "\n";
  }
  buffer << "endbfrange\n";
  buffer << ToUnicodeEnd;
  // TODO(npm): Encrypt / Compress?
  CPDF_Stream* stream = pDoc->NewIndirect<CPDF_Stream>();
  stream->SetDataFromStringstream(&buffer);
  return stream;
}

CPDF_Font* LoadSimpleFont(CPDF_Document* pDoc,
                          std::unique_ptr<CFX_Font> pFont,
                          pdfium::span<const uint8_t> span,
                          int font_type) {
  CPDF_Dictionary* fontDict = pDoc->NewIndirect<CPDF_Dictionary>();
  fontDict->SetNewFor<CPDF_Name>("Type", "Font");
  fontDict->SetNewFor<CPDF_Name>(
      "Subtype", font_type == FPDF_FONT_TYPE1 ? "Type1" : "TrueType");
  ByteString name = pFont->GetFaceName();
  if (name.IsEmpty())
    name = "Unnamed";
  fontDict->SetNewFor<CPDF_Name>("BaseFont", name);

  uint32_t glyphIndex;
  uint32_t currentChar = FXFT_Get_First_Char(pFont->GetFace(), &glyphIndex);
  static constexpr uint32_t kMaxSimpleFontChar = 0xFF;
  if (currentChar > kMaxSimpleFontChar || glyphIndex == 0)
    return nullptr;
  fontDict->SetNewFor<CPDF_Number>("FirstChar", static_cast<int>(currentChar));
  CPDF_Array* widthsArray = pDoc->NewIndirect<CPDF_Array>();
  while (true) {
    uint32_t width =
        std::min(pFont->GetGlyphWidth(glyphIndex),
                 static_cast<uint32_t>(std::numeric_limits<int>::max()));
    widthsArray->AddNew<CPDF_Number>(static_cast<int>(width));
    uint32_t nextChar =
        FXFT_Get_Next_Char(pFont->GetFace(), currentChar, &glyphIndex);
    // Simple fonts have 1-byte charcodes only.
    if (nextChar > kMaxSimpleFontChar || glyphIndex == 0)
      break;
    for (uint32_t i = currentChar + 1; i < nextChar; i++)
      widthsArray->AddNew<CPDF_Number>(0);
    currentChar = nextChar;
  }
  fontDict->SetNewFor<CPDF_Number>("LastChar", static_cast<int>(currentChar));
  fontDict->SetFor("Widths", widthsArray->MakeReference(pDoc));
  CPDF_Dictionary* pFontDesc =
      LoadFontDesc(pDoc, name, pFont.get(), span, font_type);

  fontDict->SetFor("FontDescriptor", pFontDesc->MakeReference(pDoc));
  return pDoc->LoadFont(fontDict);
}

CPDF_Font* LoadCompositeFont(CPDF_Document* pDoc,
                             std::unique_ptr<CFX_Font> pFont,
                             pdfium::span<const uint8_t> span,
                             int font_type) {
  CPDF_Dictionary* fontDict = pDoc->NewIndirect<CPDF_Dictionary>();
  fontDict->SetNewFor<CPDF_Name>("Type", "Font");
  fontDict->SetNewFor<CPDF_Name>("Subtype", "Type0");
  // TODO(npm): Get the correct encoding, if it's not identity.
  ByteString encoding = "Identity-H";
  fontDict->SetNewFor<CPDF_Name>("Encoding", encoding);
  ByteString name = pFont->GetFaceName();
  if (name.IsEmpty())
    name = "Unnamed";
  fontDict->SetNewFor<CPDF_Name>(
      "BaseFont", font_type == FPDF_FONT_TYPE1 ? name + "-" + encoding : name);

  CPDF_Dictionary* pCIDFont = pDoc->NewIndirect<CPDF_Dictionary>();
  pCIDFont->SetNewFor<CPDF_Name>("Type", "Font");
  pCIDFont->SetNewFor<CPDF_Name>("Subtype", font_type == FPDF_FONT_TYPE1
                                                ? "CIDFontType0"
                                                : "CIDFontType2");
  pCIDFont->SetNewFor<CPDF_Name>("BaseFont", name);

  // TODO(npm): Maybe use FT_Get_CID_Registry_Ordering_Supplement to get the
  // CIDSystemInfo
  CPDF_Dictionary* pCIDSystemInfo = pDoc->NewIndirect<CPDF_Dictionary>();
  pCIDSystemInfo->SetNewFor<CPDF_String>("Registry", "Adobe", false);
  pCIDSystemInfo->SetNewFor<CPDF_String>("Ordering", "Identity", false);
  pCIDSystemInfo->SetNewFor<CPDF_Number>("Supplement", 0);
  pCIDFont->SetFor("CIDSystemInfo", pCIDSystemInfo->MakeReference(pDoc));

  CPDF_Dictionary* pFontDesc =
      LoadFontDesc(pDoc, name, pFont.get(), span, font_type);
  pCIDFont->SetFor("FontDescriptor", pFontDesc->MakeReference(pDoc));

  uint32_t glyphIndex;
  uint32_t currentChar = FXFT_Get_First_Char(pFont->GetFace(), &glyphIndex);
  static constexpr uint32_t kMaxUnicode = 0x10FFFF;
  // If it doesn't have a single char, just fail
  if (glyphIndex == 0 || currentChar > kMaxUnicode)
    return nullptr;

  std::map<uint32_t, uint32_t> to_unicode;
  std::map<uint32_t, uint32_t> widths;
  while (true) {
    if (currentChar > kMaxUnicode)
      break;

    if (!pdfium::ContainsKey(widths, glyphIndex))
      widths[glyphIndex] = pFont->GetGlyphWidth(glyphIndex);
    to_unicode[glyphIndex] = currentChar;
    currentChar =
        FXFT_Get_Next_Char(pFont->GetFace(), currentChar, &glyphIndex);
    if (glyphIndex == 0)
      break;
  }
  CPDF_Array* widthsArray = pDoc->NewIndirect<CPDF_Array>();
  for (auto it = widths.begin(); it != widths.end(); ++it) {
    int ch = it->first;
    int w = it->second;
    if (std::next(it) == widths.end()) {
      // Only one char left, use format c [w]
      auto oneW = pdfium::MakeUnique<CPDF_Array>();
      oneW->AddNew<CPDF_Number>(w);
      widthsArray->AddNew<CPDF_Number>(ch);
      widthsArray->Add(std::move(oneW));
      break;
    }
    ++it;
    int next_ch = it->first;
    int next_w = it->second;
    if (next_ch == ch + 1 && next_w == w) {
      // The array can have a group c_first c_last w: all CIDs in the range from
      // c_first to c_last will have width w
      widthsArray->AddNew<CPDF_Number>(ch);
      ch = next_ch;
      while (true) {
        auto next_it = std::next(it);
        if (next_it == widths.end() || next_it->first != it->first + 1 ||
            next_it->second != it->second) {
          break;
        }
        ++it;
        ch = it->first;
      }
      widthsArray->AddNew<CPDF_Number>(ch);
      widthsArray->AddNew<CPDF_Number>(w);
      continue;
    }
    // Otherwise we can have a group of the form c [w1 w2 ...]: c has width
    // w1, c+1 has width w2, etc.
    widthsArray->AddNew<CPDF_Number>(ch);
    auto curWidthArray = pdfium::MakeUnique<CPDF_Array>();
    curWidthArray->AddNew<CPDF_Number>(w);
    curWidthArray->AddNew<CPDF_Number>(next_w);
    while (true) {
      auto next_it = std::next(it);
      if (next_it == widths.end() || next_it->first != it->first + 1)
        break;
      ++it;
      curWidthArray->AddNew<CPDF_Number>(static_cast<int>(it->second));
    }
    widthsArray->Add(std::move(curWidthArray));
  }
  pCIDFont->SetFor("W", widthsArray->MakeReference(pDoc));
  // TODO(npm): Support vertical writing

  auto pDescendant = pdfium::MakeUnique<CPDF_Array>();
  pDescendant->Add(pCIDFont->MakeReference(pDoc));
  fontDict->SetFor("DescendantFonts", std::move(pDescendant));
  CPDF_Stream* toUnicodeStream = LoadUnicode(pDoc, to_unicode);
  fontDict->SetFor("ToUnicode", toUnicodeStream->MakeReference(pDoc));
  return pDoc->LoadFont(fontDict);
}

CPDF_TextObject* CPDFTextObjectFromFPDFPageObject(FPDF_PAGEOBJECT page_object) {
  auto* obj = CPDFPageObjectFromFPDFPageObject(page_object);
  return obj ? obj->AsText() : nullptr;
}

}  // namespace

FPDF_EXPORT FPDF_PAGEOBJECT FPDF_CALLCONV
FPDFPageObj_NewTextObj(FPDF_DOCUMENT document,
                       FPDF_BYTESTRING font,
                       float font_size) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;

  CPDF_Font* pFont = CPDF_Font::GetStockFont(pDoc, ByteStringView(font));
  if (!pFont)
    return nullptr;

  auto pTextObj = pdfium::MakeUnique<CPDF_TextObject>();
  pTextObj->m_TextState.SetFont(pFont);
  pTextObj->m_TextState.SetFontSize(font_size);
  pTextObj->DefaultStates();

  // Caller takes ownership.
  return FPDFPageObjectFromCPDFPageObject(pTextObj.release());
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFText_SetText(FPDF_PAGEOBJECT text_object, FPDF_WIDESTRING text) {
  CPDF_TextObject* pTextObj = CPDFTextObjectFromFPDFPageObject(text_object);
  if (!pTextObj)
    return false;

  WideString encodedText = WideStringFromFPDFWideString(text);
  ByteString byteText;
  for (wchar_t wc : encodedText) {
    pTextObj->GetFont()->AppendChar(
        &byteText, pTextObj->GetFont()->CharCodeFromUnicode(wc));
  }
  pTextObj->SetText(byteText);
  return true;
}

FPDF_EXPORT FPDF_FONT FPDF_CALLCONV FPDFText_LoadFont(FPDF_DOCUMENT document,
                                                      const uint8_t* data,
                                                      uint32_t size,
                                                      int font_type,
                                                      FPDF_BOOL cid) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc || !data || size == 0 ||
      (font_type != FPDF_FONT_TYPE1 && font_type != FPDF_FONT_TRUETYPE)) {
    return nullptr;
  }

  auto span = pdfium::make_span(data, size);
  auto pFont = pdfium::MakeUnique<CFX_Font>();

  // TODO(npm): Maybe use FT_Get_X11_Font_Format to check format? Otherwise, we
  // are allowing giving any font that can be loaded on freetype and setting it
  // as any font type.
  if (!pFont->LoadEmbedded(span))
    return nullptr;

  return FPDFFontFromCPDFFont(
      cid ? LoadCompositeFont(pDoc, std::move(pFont), span, font_type)
          : LoadSimpleFont(pDoc, std::move(pFont), span, font_type));
}

FPDF_EXPORT FPDF_FONT FPDF_CALLCONV
FPDFText_LoadStandardFont(FPDF_DOCUMENT document, FPDF_BYTESTRING font) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;

  return FPDFFontFromCPDFFont(
      CPDF_Font::GetStockFont(pDoc, ByteStringView(font)));
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FPDFText_SetFillColor(FPDF_PAGEOBJECT text_object,
                      unsigned int R,
                      unsigned int G,
                      unsigned int B,
                      unsigned int A) {
  return FPDFPageObj_SetFillColor(text_object, R, G, B, A);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FPDFText_GetMatrix(FPDF_PAGEOBJECT text,
                                                       double* a,
                                                       double* b,
                                                       double* c,
                                                       double* d,
                                                       double* e,
                                                       double* f) {
  if (!a || !b || !c || !d || !e || !f)
    return false;

  CPDF_TextObject* pTextObj = CPDFTextObjectFromFPDFPageObject(text);
  if (!pTextObj)
    return false;

  std::tie(*a, *b, *c, *d, *e, *f) = pTextObj->GetTextMatrix().AsTuple();
  return true;
}

FPDF_EXPORT double FPDF_CALLCONV FPDFTextObj_GetFontSize(FPDF_PAGEOBJECT text) {
  CPDF_TextObject* pTextObj = CPDFTextObjectFromFPDFPageObject(text);
  return pTextObj ? pTextObj->GetFontSize() : 0;
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDFTextObj_GetFontName(FPDF_PAGEOBJECT text,
                        void* buffer,
                        unsigned long length) {
  CPDF_TextObject* pTextObj = CPDFTextObjectFromFPDFPageObject(text);
  if (!pTextObj)
    return 0;

  CPDF_Font* pPdfFont = pTextObj->GetFont();
  if (!pPdfFont)
    return 0;

  CFX_Font* pFont = pPdfFont->GetFont();
  ASSERT(pFont);

  ByteString name = pFont->GetFamilyName();
  unsigned long dwStringLen = name.GetLength() + 1;
  if (buffer && length >= dwStringLen)
    memcpy(buffer, name.c_str(), dwStringLen);
  return dwStringLen;
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FPDFTextObj_GetText(FPDF_PAGEOBJECT text_object,
                    FPDF_TEXTPAGE text_page,
                    void* buffer,
                    unsigned long length) {
  CPDF_TextObject* pTextObj = CPDFTextObjectFromFPDFPageObject(text_object);
  if (!pTextObj)
    return 0;

  CPDF_TextPage* pTextPage = CPDFTextPageFromFPDFTextPage(text_page);
  if (!pTextPage)
    return 0;

  WideString text = pTextPage->GetTextByObject(pTextObj);
  return Utf16EncodeMaybeCopyAndReturnLength(text, buffer, length);
}

FPDF_EXPORT void FPDF_CALLCONV FPDFFont_Close(FPDF_FONT font) {
  CPDF_Font* pFont = CPDFFontFromFPDFFont(font);
  if (!pFont)
    return;

  CPDF_Document* pDoc = pFont->GetDocument();
  if (!pDoc)
    return;

  CPDF_DocPageData* pPageData = pDoc->GetPageData();
  if (!pPageData->IsForceClear())
    pPageData->ReleaseFont(pFont->GetFontDict());
}

FPDF_EXPORT FPDF_PAGEOBJECT FPDF_CALLCONV
FPDFPageObj_CreateTextObj(FPDF_DOCUMENT document,
                          FPDF_FONT font,
                          float font_size) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  CPDF_Font* pFont = CPDFFontFromFPDFFont(font);
  if (!pDoc || !pFont)
    return nullptr;

  auto pTextObj = pdfium::MakeUnique<CPDF_TextObject>();
  pTextObj->m_TextState.SetFont(pDoc->LoadFont(pFont->GetFontDict()));
  pTextObj->m_TextState.SetFontSize(font_size);
  pTextObj->DefaultStates();
  return FPDFPageObjectFromCPDFPageObject(pTextObj.release());
}

FPDF_EXPORT int FPDF_CALLCONV FPDFText_GetTextRenderMode(FPDF_PAGEOBJECT text) {
  CPDF_TextObject* pTextObj = CPDFTextObjectFromFPDFPageObject(text);
  return pTextObj ? static_cast<int>(pTextObj->m_TextState.GetTextMode()) : -1;
}
