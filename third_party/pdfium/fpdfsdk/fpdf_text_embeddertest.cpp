// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/fx_memory.h"
#include "core/fxge/fx_font.h"
#include "public/cpp/fpdf_scopers.h"
#include "public/fpdf_text.h"
#include "public/fpdf_transformpage.h"
#include "public/fpdfview.h"
#include "testing/embedder_test.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

namespace {

constexpr char kHelloGoodbyeText[] = "Hello, world!\r\nGoodbye, world!";
constexpr int kHelloGoodbyeTextSize = FX_ArraySize(kHelloGoodbyeText);

bool check_unsigned_shorts(const char* expected,
                           const unsigned short* actual,
                           size_t length) {
  if (length > strlen(expected) + 1)
    return false;

  for (size_t i = 0; i < length; ++i) {
    if (actual[i] != static_cast<unsigned short>(expected[i]))
      return false;
  }
  return true;
}

}  // namespace

class FPDFTextEmbedderTest : public EmbedderTest {};

TEST_F(FPDFTextEmbedderTest, Text) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);

  unsigned short buffer[128];
  memset(buffer, 0xbd, sizeof(buffer));

  // Check that edge cases are handled gracefully
  EXPECT_EQ(0, FPDFText_GetText(textpage, 0, 128, nullptr));
  EXPECT_EQ(0, FPDFText_GetText(textpage, -1, 128, buffer));
  EXPECT_EQ(0, FPDFText_GetText(textpage, 0, -1, buffer));
  EXPECT_EQ(1, FPDFText_GetText(textpage, 0, 0, buffer));
  EXPECT_EQ(0, buffer[0]);

  // Keep going and check the next case.
  memset(buffer, 0xbd, sizeof(buffer));
  EXPECT_EQ(2, FPDFText_GetText(textpage, 0, 1, buffer));
  EXPECT_EQ(kHelloGoodbyeText[0], buffer[0]);
  EXPECT_EQ(0, buffer[1]);

  // Check includes the terminating NUL that is provided.
  int num_chars = FPDFText_GetText(textpage, 0, 128, buffer);
  ASSERT_EQ(kHelloGoodbyeTextSize, num_chars);
  EXPECT_TRUE(
      check_unsigned_shorts(kHelloGoodbyeText, buffer, kHelloGoodbyeTextSize));

  // Count does not include the terminating NUL in the string literal.
  EXPECT_EQ(kHelloGoodbyeTextSize - 1, FPDFText_CountChars(textpage));
  for (size_t i = 0; i < kHelloGoodbyeTextSize - 1; ++i) {
    EXPECT_EQ(static_cast<unsigned int>(kHelloGoodbyeText[i]),
              FPDFText_GetUnicode(textpage, i))
        << " at " << i;
  }

  // Extracting using a buffer that will be completely filled. Small buffer is
  // 12 elements long, since it will need 2 locations per displayed character in
  // the expected string, plus 2 more for the terminating character.
  static const char kSmallExpected[] = "Hello";
  unsigned short small_buffer[12];
  memset(buffer, 0xbd, sizeof(buffer));
  EXPECT_EQ(6, FPDFText_GetText(textpage, 0, 5, small_buffer));
  EXPECT_TRUE(check_unsigned_shorts(kSmallExpected, small_buffer,
                                    sizeof(kSmallExpected)));

  EXPECT_EQ(12.0, FPDFText_GetFontSize(textpage, 0));
  EXPECT_EQ(16.0, FPDFText_GetFontSize(textpage, 15));

  double left = 0.0;
  double right = 0.0;
  double bottom = 0.0;
  double top = 0.0;
  EXPECT_FALSE(FPDFText_GetCharBox(nullptr, 4, &left, &right, &bottom, &top));
  EXPECT_DOUBLE_EQ(0.0, left);
  EXPECT_DOUBLE_EQ(0.0, right);
  EXPECT_DOUBLE_EQ(0.0, bottom);
  EXPECT_DOUBLE_EQ(0.0, top);
  EXPECT_FALSE(FPDFText_GetCharBox(textpage, -1, &left, &right, &bottom, &top));
  EXPECT_DOUBLE_EQ(0.0, left);
  EXPECT_DOUBLE_EQ(0.0, right);
  EXPECT_DOUBLE_EQ(0.0, bottom);
  EXPECT_DOUBLE_EQ(0.0, top);
  EXPECT_FALSE(FPDFText_GetCharBox(textpage, 55, &left, &right, &bottom, &top));
  EXPECT_DOUBLE_EQ(0.0, left);
  EXPECT_DOUBLE_EQ(0.0, right);
  EXPECT_DOUBLE_EQ(0.0, bottom);
  EXPECT_DOUBLE_EQ(0.0, top);

  EXPECT_TRUE(FPDFText_GetCharBox(textpage, 4, &left, &right, &bottom, &top));
  EXPECT_NEAR(41.071, left, 0.001);
  EXPECT_NEAR(46.243, right, 0.001);
  EXPECT_NEAR(49.844, bottom, 0.001);
  EXPECT_NEAR(55.520, top, 0.001);

  double x = 0.0;
  double y = 0.0;
  EXPECT_TRUE(FPDFText_GetCharOrigin(textpage, 4, &x, &y));
  EXPECT_NEAR(40.664, x, 0.001);
  EXPECT_NEAR(50.000, y, 0.001);

  EXPECT_EQ(4, FPDFText_GetCharIndexAtPos(textpage, 42.0, 50.0, 1.0, 1.0));
  EXPECT_EQ(-1, FPDFText_GetCharIndexAtPos(textpage, 0.0, 0.0, 1.0, 1.0));
  EXPECT_EQ(-1, FPDFText_GetCharIndexAtPos(textpage, 199.0, 199.0, 1.0, 1.0));

  // Test out of range indicies.
  EXPECT_EQ(-1,
            FPDFText_GetCharIndexAtPos(textpage, 42.0, 10000000.0, 1.0, 1.0));
  EXPECT_EQ(-1, FPDFText_GetCharIndexAtPos(textpage, -1.0, 50.0, 1.0, 1.0));

  // Count does not include the terminating NUL in the string literal.
  EXPECT_EQ(2, FPDFText_CountRects(textpage, 0, kHelloGoodbyeTextSize - 1));

  left = 0.0;
  right = 0.0;
  bottom = 0.0;
  top = 0.0;
  EXPECT_TRUE(FPDFText_GetRect(textpage, 1, &left, &top, &right, &bottom));
  EXPECT_NEAR(20.847, left, 0.001);
  EXPECT_NEAR(135.167, right, 0.001);
  EXPECT_NEAR(96.655, bottom, 0.001);
  EXPECT_NEAR(116.000, top, 0.001);

  // Test out of range indicies set outputs to (0.0, 0.0, 0.0, 0.0).
  left = -1.0;
  right = -1.0;
  bottom = -1.0;
  top = -1.0;
  EXPECT_FALSE(FPDFText_GetRect(textpage, -1, &left, &top, &right, &bottom));
  EXPECT_EQ(0.0, left);
  EXPECT_EQ(0.0, right);
  EXPECT_EQ(0.0, bottom);
  EXPECT_EQ(0.0, top);

  left = -2.0;
  right = -2.0;
  bottom = -2.0;
  top = -2.0;
  EXPECT_FALSE(FPDFText_GetRect(textpage, 2, &left, &top, &right, &bottom));
  EXPECT_EQ(0.0, left);
  EXPECT_EQ(0.0, right);
  EXPECT_EQ(0.0, bottom);
  EXPECT_EQ(0.0, top);

  EXPECT_EQ(
      9, FPDFText_GetBoundedText(textpage, 41.0, 56.0, 82.0, 48.0, nullptr, 0));

  // Extract starting at character 4 as above.
  memset(buffer, 0xbd, sizeof(buffer));
  EXPECT_EQ(
      1, FPDFText_GetBoundedText(textpage, 41.0, 56.0, 82.0, 48.0, buffer, 1));
  EXPECT_TRUE(check_unsigned_shorts(kHelloGoodbyeText + 4, buffer, 1));
  EXPECT_EQ(0xbdbd, buffer[1]);

  memset(buffer, 0xbd, sizeof(buffer));
  EXPECT_EQ(
      9, FPDFText_GetBoundedText(textpage, 41.0, 56.0, 82.0, 48.0, buffer, 9));
  EXPECT_TRUE(check_unsigned_shorts(kHelloGoodbyeText + 4, buffer, 9));
  EXPECT_EQ(0xbdbd, buffer[9]);

  memset(buffer, 0xbd, sizeof(buffer));
  EXPECT_EQ(10, FPDFText_GetBoundedText(textpage, 41.0, 56.0, 82.0, 48.0,
                                        buffer, 128));
  EXPECT_TRUE(check_unsigned_shorts(kHelloGoodbyeText + 4, buffer, 9));
  EXPECT_EQ(0u, buffer[9]);
  EXPECT_EQ(0xbdbd, buffer[10]);

  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, TextSearch) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);

  std::unique_ptr<unsigned short, pdfium::FreeDeleter> nope =
      GetFPDFWideString(L"nope");
  std::unique_ptr<unsigned short, pdfium::FreeDeleter> world =
      GetFPDFWideString(L"world");
  std::unique_ptr<unsigned short, pdfium::FreeDeleter> world_caps =
      GetFPDFWideString(L"WORLD");
  std::unique_ptr<unsigned short, pdfium::FreeDeleter> world_substr =
      GetFPDFWideString(L"orld");

  // No occurences of "nope" in test page.
  FPDF_SCHHANDLE search = FPDFText_FindStart(textpage, nope.get(), 0, 0);
  EXPECT_TRUE(search);
  EXPECT_EQ(0, FPDFText_GetSchResultIndex(search));
  EXPECT_EQ(0, FPDFText_GetSchCount(search));

  // Advancing finds nothing.
  EXPECT_FALSE(FPDFText_FindNext(search));
  EXPECT_EQ(0, FPDFText_GetSchResultIndex(search));
  EXPECT_EQ(0, FPDFText_GetSchCount(search));

  // Retreating finds nothing.
  EXPECT_FALSE(FPDFText_FindPrev(search));
  EXPECT_EQ(0, FPDFText_GetSchResultIndex(search));
  EXPECT_EQ(0, FPDFText_GetSchCount(search));
  FPDFText_FindClose(search);

  // Two occurences of "world" in test page.
  search = FPDFText_FindStart(textpage, world.get(), 0, 2);
  EXPECT_TRUE(search);

  // Remains not found until advanced.
  EXPECT_EQ(0, FPDFText_GetSchResultIndex(search));
  EXPECT_EQ(0, FPDFText_GetSchCount(search));

  // First occurence of "world" in this test page.
  EXPECT_TRUE(FPDFText_FindNext(search));
  EXPECT_EQ(7, FPDFText_GetSchResultIndex(search));
  EXPECT_EQ(5, FPDFText_GetSchCount(search));

  // Last occurence of "world" in this test page.
  EXPECT_TRUE(FPDFText_FindNext(search));
  EXPECT_EQ(24, FPDFText_GetSchResultIndex(search));
  EXPECT_EQ(5, FPDFText_GetSchCount(search));

  // Found position unchanged when fails to advance.
  EXPECT_FALSE(FPDFText_FindNext(search));
  EXPECT_EQ(24, FPDFText_GetSchResultIndex(search));
  EXPECT_EQ(5, FPDFText_GetSchCount(search));

  // Back to first occurence.
  EXPECT_TRUE(FPDFText_FindPrev(search));
  EXPECT_EQ(7, FPDFText_GetSchResultIndex(search));
  EXPECT_EQ(5, FPDFText_GetSchCount(search));

  // Found position unchanged when fails to retreat.
  EXPECT_FALSE(FPDFText_FindPrev(search));
  EXPECT_EQ(7, FPDFText_GetSchResultIndex(search));
  EXPECT_EQ(5, FPDFText_GetSchCount(search));
  FPDFText_FindClose(search);

  // Exact search unaffected by case sensitiity and whole word flags.
  search = FPDFText_FindStart(textpage, world.get(),
                              FPDF_MATCHCASE | FPDF_MATCHWHOLEWORD, 0);
  EXPECT_TRUE(search);
  EXPECT_TRUE(FPDFText_FindNext(search));
  EXPECT_EQ(7, FPDFText_GetSchResultIndex(search));
  EXPECT_EQ(5, FPDFText_GetSchCount(search));
  FPDFText_FindClose(search);

  // Default is case-insensitive, so matching agaist caps works.
  search = FPDFText_FindStart(textpage, world_caps.get(), 0, 0);
  EXPECT_TRUE(search);
  EXPECT_TRUE(FPDFText_FindNext(search));
  EXPECT_EQ(7, FPDFText_GetSchResultIndex(search));
  EXPECT_EQ(5, FPDFText_GetSchCount(search));
  FPDFText_FindClose(search);

  // But can be made case sensitive, in which case this fails.
  search = FPDFText_FindStart(textpage, world_caps.get(), FPDF_MATCHCASE, 0);
  EXPECT_FALSE(FPDFText_FindNext(search));
  EXPECT_EQ(0, FPDFText_GetSchResultIndex(search));
  EXPECT_EQ(0, FPDFText_GetSchCount(search));
  FPDFText_FindClose(search);

  // Default is match anywhere within word, so matching substirng works.
  search = FPDFText_FindStart(textpage, world_substr.get(), 0, 0);
  EXPECT_TRUE(FPDFText_FindNext(search));
  EXPECT_EQ(8, FPDFText_GetSchResultIndex(search));
  EXPECT_EQ(4, FPDFText_GetSchCount(search));
  FPDFText_FindClose(search);

  // But can be made to mach word boundaries, in which case this fails.
  search =
      FPDFText_FindStart(textpage, world_substr.get(), FPDF_MATCHWHOLEWORD, 0);
  EXPECT_FALSE(FPDFText_FindNext(search));
  // TODO(tsepez): investigate strange index/count values in this state.
  FPDFText_FindClose(search);

  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

// Test that the page has characters despite a bad stream length.
TEST_F(FPDFTextEmbedderTest, StreamLengthPastEndOfFile) {
  ASSERT_TRUE(OpenDocument("bug_57.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);
  EXPECT_EQ(13, FPDFText_CountChars(textpage));

  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, WebLinks) {
  ASSERT_TRUE(OpenDocument("weblinks.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);

  {
    ScopedFPDFPageLink pagelink(FPDFLink_LoadWebLinks(textpage));
    EXPECT_TRUE(pagelink);

    // Page contains two HTTP-style URLs.
    EXPECT_EQ(2, FPDFLink_CountWebLinks(pagelink.get()));

    // Only a terminating NUL required for bogus links.
    EXPECT_EQ(1, FPDFLink_GetURL(pagelink.get(), 2, nullptr, 0));
    EXPECT_EQ(1, FPDFLink_GetURL(pagelink.get(), 1400, nullptr, 0));
    EXPECT_EQ(1, FPDFLink_GetURL(pagelink.get(), -1, nullptr, 0));
  }

  FPDF_PAGELINK pagelink = FPDFLink_LoadWebLinks(textpage);
  EXPECT_TRUE(pagelink);

  // Query the number of characters required for each link (incl NUL).
  EXPECT_EQ(25, FPDFLink_GetURL(pagelink, 0, nullptr, 0));
  EXPECT_EQ(26, FPDFLink_GetURL(pagelink, 1, nullptr, 0));

  static const char expected_url[] = "http://example.com?q=foo";
  static const size_t expected_len = sizeof(expected_url);
  unsigned short buffer[128];

  // Retrieve a link with too small a buffer.  Buffer will not be
  // NUL-terminated, but must not be modified past indicated length,
  // so pre-fill with a pattern to check write bounds.
  memset(buffer, 0xbd, sizeof(buffer));
  EXPECT_EQ(1, FPDFLink_GetURL(pagelink, 0, buffer, 1));
  EXPECT_TRUE(check_unsigned_shorts(expected_url, buffer, 1));
  EXPECT_EQ(0xbdbd, buffer[1]);

  // Check buffer that doesn't have space for a terminating NUL.
  memset(buffer, 0xbd, sizeof(buffer));
  EXPECT_EQ(static_cast<int>(expected_len - 1),
            FPDFLink_GetURL(pagelink, 0, buffer, expected_len - 1));
  EXPECT_TRUE(check_unsigned_shorts(expected_url, buffer, expected_len - 1));
  EXPECT_EQ(0xbdbd, buffer[expected_len - 1]);

  // Retreive link with exactly-sized buffer.
  memset(buffer, 0xbd, sizeof(buffer));
  EXPECT_EQ(static_cast<int>(expected_len),
            FPDFLink_GetURL(pagelink, 0, buffer, expected_len));
  EXPECT_TRUE(check_unsigned_shorts(expected_url, buffer, expected_len));
  EXPECT_EQ(0u, buffer[expected_len - 1]);
  EXPECT_EQ(0xbdbd, buffer[expected_len]);

  // Retreive link with ample-sized-buffer.
  memset(buffer, 0xbd, sizeof(buffer));
  EXPECT_EQ(static_cast<int>(expected_len),
            FPDFLink_GetURL(pagelink, 0, buffer, 128));
  EXPECT_TRUE(check_unsigned_shorts(expected_url, buffer, expected_len));
  EXPECT_EQ(0u, buffer[expected_len - 1]);
  EXPECT_EQ(0xbdbd, buffer[expected_len]);

  // Each link rendered in a single rect in this test page.
  EXPECT_EQ(1, FPDFLink_CountRects(pagelink, 0));
  EXPECT_EQ(1, FPDFLink_CountRects(pagelink, 1));

  // Each link rendered in a single rect in this test page.
  EXPECT_EQ(0, FPDFLink_CountRects(pagelink, -1));
  EXPECT_EQ(0, FPDFLink_CountRects(pagelink, 2));
  EXPECT_EQ(0, FPDFLink_CountRects(pagelink, 10000));

  // Check boundary of valid link index with valid rect index.
  double left = 0.0;
  double right = 0.0;
  double top = 0.0;
  double bottom = 0.0;
  EXPECT_TRUE(FPDFLink_GetRect(pagelink, 0, 0, &left, &top, &right, &bottom));
  EXPECT_NEAR(50.791, left, 0.001);
  EXPECT_NEAR(187.963, right, 0.001);
  EXPECT_NEAR(97.624, bottom, 0.001);
  EXPECT_NEAR(108.736, top, 0.001);

  // Check that valid link with invalid rect index leaves parameters unchanged.
  left = -1.0;
  right = -1.0;
  top = -1.0;
  bottom = -1.0;
  EXPECT_FALSE(FPDFLink_GetRect(pagelink, 0, 1, &left, &top, &right, &bottom));
  EXPECT_EQ(-1.0, left);
  EXPECT_EQ(-1.0, right);
  EXPECT_EQ(-1.0, bottom);
  EXPECT_EQ(-1.0, top);

  // Check that invalid link index leaves parameters unchanged.
  left = -2.0;
  right = -2.0;
  top = -2.0;
  bottom = -2.0;
  EXPECT_FALSE(FPDFLink_GetRect(pagelink, -1, 0, &left, &top, &right, &bottom));
  EXPECT_EQ(-2.0, left);
  EXPECT_EQ(-2.0, right);
  EXPECT_EQ(-2.0, bottom);
  EXPECT_EQ(-2.0, top);

  FPDFLink_CloseWebLinks(pagelink);
  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, WebLinksAcrossLines) {
  ASSERT_TRUE(OpenDocument("weblinks_across_lines.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);

  FPDF_PAGELINK pagelink = FPDFLink_LoadWebLinks(textpage);
  EXPECT_TRUE(pagelink);

  static const char* const kExpectedUrls[] = {
      "http://example.com",           // from "http://www.example.com?\r\nfoo"
      "http://example.com/",          // from "http://www.example.com/\r\nfoo"
      "http://example.com/test-foo",  // from "http://example.com/test-\r\nfoo"
      "http://abc.com/test-foo",      // from "http://abc.com/test-\r\n\r\nfoo"
      // Next two links from "http://www.example.com/\r\nhttp://www.abc.com/"
      "http://example.com/", "http://www.abc.com",
  };
  static const int kNumLinks = static_cast<int>(FX_ArraySize(kExpectedUrls));

  EXPECT_EQ(kNumLinks, FPDFLink_CountWebLinks(pagelink));

  unsigned short buffer[128];
  for (int i = 0; i < kNumLinks; i++) {
    const size_t expected_len = strlen(kExpectedUrls[i]) + 1;
    memset(buffer, 0, sizeof(buffer));
    EXPECT_EQ(static_cast<int>(expected_len),
              FPDFLink_GetURL(pagelink, i, nullptr, 0));
    EXPECT_EQ(static_cast<int>(expected_len),
              FPDFLink_GetURL(pagelink, i, buffer, FX_ArraySize(buffer)));
    EXPECT_TRUE(check_unsigned_shorts(kExpectedUrls[i], buffer, expected_len));
  }

  FPDFLink_CloseWebLinks(pagelink);
  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, WebLinksAcrossLinesBug) {
  ASSERT_TRUE(OpenDocument("bug_650.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);

  FPDF_PAGELINK pagelink = FPDFLink_LoadWebLinks(textpage);
  EXPECT_TRUE(pagelink);

  EXPECT_EQ(2, FPDFLink_CountWebLinks(pagelink));
  unsigned short buffer[128] = {0};
  static const char kExpectedUrl[] =
      "http://tutorial45.com/learn-autocad-basics-day-166/";
  static const int kUrlSize = static_cast<int>(sizeof(kExpectedUrl));

  EXPECT_EQ(kUrlSize, FPDFLink_GetURL(pagelink, 1, nullptr, 0));
  EXPECT_EQ(kUrlSize,
            FPDFLink_GetURL(pagelink, 1, buffer, FX_ArraySize(buffer)));
  EXPECT_TRUE(check_unsigned_shorts(kExpectedUrl, buffer, kUrlSize));

  FPDFLink_CloseWebLinks(pagelink);
  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, GetFontSize) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);

  const double kExpectedFontsSizes[] = {12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
                                        12, 12, 12, 1,  1,  16, 16, 16, 16, 16,
                                        16, 16, 16, 16, 16, 16, 16, 16, 16, 16};

  int count = FPDFText_CountChars(textpage);
  ASSERT_EQ(FX_ArraySize(kExpectedFontsSizes), static_cast<size_t>(count));
  for (int i = 0; i < count; ++i)
    EXPECT_EQ(kExpectedFontsSizes[i], FPDFText_GetFontSize(textpage, i)) << i;

  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, GetFontInfo) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);
  std::vector<char> font_name;
  size_t num_chars1 = strlen("Hello, world!");
  const char kExpectedFontName1[] = "Times-Roman";

  for (size_t i = 0; i < num_chars1; i++) {
    int flags = -1;
    unsigned long length =
        FPDFText_GetFontInfo(textpage, i, nullptr, 0, &flags);
    static constexpr unsigned long expected_length = sizeof(kExpectedFontName1);
    ASSERT_EQ(expected_length, length);
    EXPECT_EQ(FXFONT_NONSYMBOLIC, flags);
    font_name.resize(length);
    std::fill(font_name.begin(), font_name.end(), 'a');
    flags = -1;
    EXPECT_EQ(expected_length,
              FPDFText_GetFontInfo(textpage, i, font_name.data(),
                                   font_name.size(), &flags));
    EXPECT_STREQ(kExpectedFontName1, font_name.data());
    EXPECT_EQ(FXFONT_NONSYMBOLIC, flags);
  }
  // If the size of the buffer is not large enough, the buffer should remain
  // unchanged.
  font_name.pop_back();
  std::fill(font_name.begin(), font_name.end(), 'a');
  EXPECT_EQ(sizeof(kExpectedFontName1),
            FPDFText_GetFontInfo(textpage, 0, font_name.data(),
                                 font_name.size(), nullptr));
  for (char a : font_name)
    EXPECT_EQ('a', a);

  // The text is "Hello, world!\r\nGoodbye, world!", so the next two characters
  // do not have any font information.
  EXPECT_EQ(0u, FPDFText_GetFontInfo(textpage, num_chars1, font_name.data(),
                                     font_name.size(), nullptr));
  EXPECT_EQ(0u, FPDFText_GetFontInfo(textpage, num_chars1 + 1, font_name.data(),
                                     font_name.size(), nullptr));

  size_t num_chars2 = strlen("Goodbye, world!");
  const char kExpectedFontName2[] = "Helvetica";
  for (size_t i = num_chars1 + 2; i < num_chars1 + num_chars2 + 2; i++) {
    int flags = -1;
    unsigned long length =
        FPDFText_GetFontInfo(textpage, i, nullptr, 0, &flags);
    static constexpr unsigned long expected_length = sizeof(kExpectedFontName2);
    ASSERT_EQ(expected_length, length);
    EXPECT_EQ(FXFONT_NONSYMBOLIC, flags);
    font_name.resize(length);
    std::fill(font_name.begin(), font_name.end(), 'a');
    flags = -1;
    EXPECT_EQ(expected_length,
              FPDFText_GetFontInfo(textpage, i, font_name.data(),
                                   font_name.size(), &flags));
    EXPECT_STREQ(kExpectedFontName2, font_name.data());
    EXPECT_EQ(FXFONT_NONSYMBOLIC, flags);
  }

  // Now try some out of bounds indices and null pointers to make sure we do not
  // crash.
  // No textpage.
  EXPECT_EQ(0u, FPDFText_GetFontInfo(nullptr, 0, font_name.data(),
                                     font_name.size(), nullptr));
  // No buffer.
  EXPECT_EQ(sizeof(kExpectedFontName1),
            FPDFText_GetFontInfo(textpage, 0, nullptr, 0, nullptr));
  // Negative index.
  EXPECT_EQ(0u, FPDFText_GetFontInfo(textpage, -1, font_name.data(),
                                     font_name.size(), nullptr));
  // Out of bounds index.
  EXPECT_EQ(0u, FPDFText_GetFontInfo(textpage, 1000, font_name.data(),
                                     font_name.size(), nullptr));

  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, ToUnicode) {
  ASSERT_TRUE(OpenDocument("bug_583.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);

  ASSERT_EQ(1, FPDFText_CountChars(textpage));
  EXPECT_EQ(0U, FPDFText_GetUnicode(textpage, 0));

  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, Bug_921) {
  ASSERT_TRUE(OpenDocument("bug_921.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);

  static constexpr unsigned int kData[] = {
      1095, 1077, 1083, 1086, 1074, 1077, 1095, 1077, 1089, 1082, 1086, 1077,
      32,   1089, 1090, 1088, 1072, 1076, 1072, 1085, 1080, 1077, 46,   32};
  static constexpr int kStartIndex = 238;

  ASSERT_EQ(268, FPDFText_CountChars(textpage));
  for (size_t i = 0; i < FX_ArraySize(kData); ++i)
    EXPECT_EQ(kData[i], FPDFText_GetUnicode(textpage, kStartIndex + i));

  unsigned short buffer[FX_ArraySize(kData) + 1];
  memset(buffer, 0xbd, sizeof(buffer));
  int count =
      FPDFText_GetText(textpage, kStartIndex, FX_ArraySize(kData), buffer);
  ASSERT_GT(count, 0);
  ASSERT_EQ(FX_ArraySize(kData) + 1, static_cast<size_t>(count));
  for (size_t i = 0; i < FX_ArraySize(kData); ++i)
    EXPECT_EQ(kData[i], buffer[i]);
  EXPECT_EQ(0, buffer[FX_ArraySize(kData)]);

  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, GetTextWithHyphen) {
  ASSERT_TRUE(OpenDocument("bug_781804.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);

  // Check that soft hyphens are not included
  // Expecting 'Veritaserum', except there is a \uFFFE where the hyphen was in
  // the original text. This is a weird thing that Adobe does, which we
  // replicate.
  constexpr unsigned short soft_expected[] = {
      0x0056, 0x0065, 0x0072, 0x0069, 0x0074, 0x0061, 0xfffe,
      0x0073, 0x0065, 0x0072, 0x0075, 0x006D, 0x0000};
  {
    constexpr int count = FX_ArraySize(soft_expected) - 1;
    unsigned short buffer[FX_ArraySize(soft_expected)];
    memset(buffer, 0, sizeof(buffer));

    EXPECT_EQ(count + 1, FPDFText_GetText(textpage, 0, count, buffer));
    for (int i = 0; i < count; i++)
      EXPECT_EQ(soft_expected[i], buffer[i]);
  }

  // Check that hard hyphens are included
  {
    // There isn't the \0 in the actual doc, but there is a \r\n, so need to
    // add 1 to get aligned.
    constexpr size_t offset = FX_ArraySize(soft_expected) + 1;
    // Expecting 'User-\r\ngenerated', the - is a unicode character, so cannnot
    // store in a char[].
    constexpr unsigned short hard_expected[] = {
        0x0055, 0x0073, 0x0065, 0x0072, 0x2010, 0x000d, 0x000a, 0x0067, 0x0065,
        0x006e, 0x0065, 0x0072, 0x0061, 0x0074, 0x0065, 0x0064, 0x0000};
    constexpr int count = FX_ArraySize(hard_expected) - 1;
    unsigned short buffer[FX_ArraySize(hard_expected)];

    EXPECT_EQ(count + 1, FPDFText_GetText(textpage, offset, count, buffer));
    for (int i = 0; i < count; i++)
      EXPECT_EQ(hard_expected[i], buffer[i]);
  }

  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, bug_782596) {
  // If there is a regression in this test, it will only fail under ASAN
  ASSERT_TRUE(OpenDocument("bug_782596.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);
  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, ControlCharacters) {
  ASSERT_TRUE(OpenDocument("control_characters.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);

  // Should not include the control characters in the output
  unsigned short buffer[128];
  memset(buffer, 0xbd, sizeof(buffer));
  int num_chars = FPDFText_GetText(textpage, 0, 128, buffer);
  ASSERT_EQ(kHelloGoodbyeTextSize, num_chars);
  EXPECT_TRUE(
      check_unsigned_shorts(kHelloGoodbyeText, buffer, kHelloGoodbyeTextSize));

  // Attempting to get a chunk of text after the control characters
  static const char expected_substring[] = "Goodbye, world!";
  // Offset is the length of 'Hello, world!\r\n' + 2 control characters in the
  // original stream
  static const int offset = 17;
  memset(buffer, 0xbd, sizeof(buffer));
  num_chars = FPDFText_GetText(textpage, offset, 128, buffer);

  ASSERT_GE(num_chars, 0);
  EXPECT_EQ(sizeof(expected_substring), static_cast<size_t>(num_chars));
  EXPECT_TRUE(check_unsigned_shorts(expected_substring, buffer,
                                    sizeof(expected_substring)));

  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

// Testing that hyphen makers (0x0002) are replacing hard hyphens when
// the word contains non-ASCII characters.
TEST_F(FPDFTextEmbedderTest, bug_1029) {
  ASSERT_TRUE(OpenDocument("bug_1029.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);

  constexpr int page_range_offset = 171;
  constexpr int page_range_length = 56;

  // This text is:
  // 'METADATA table. When the split has committed, it noti' followed
  // by a 'soft hyphen' (0x0002) and then 'fi'.
  //
  // The original text has a fi ligature, but that is broken up into
  // two characters when the PDF is processed.
  constexpr unsigned int expected[] = {
      0x004d, 0x0045, 0x0054, 0x0041, 0x0044, 0x0041, 0x0054, 0x0041,
      0x0020, 0x0074, 0x0061, 0x0062, 0x006c, 0x0065, 0x002e, 0x0020,
      0x0057, 0x0068, 0x0065, 0x006e, 0x0020, 0x0074, 0x0068, 0x0065,
      0x0020, 0x0073, 0x0070, 0x006c, 0x0069, 0x0074, 0x0020, 0x0068,
      0x0061, 0x0073, 0x0020, 0x0063, 0x006f, 0x006d, 0x006d, 0x0069,
      0x0074, 0x0074, 0x0065, 0x0064, 0x002c, 0x0020, 0x0069, 0x0074,
      0x0020, 0x006e, 0x006f, 0x0074, 0x0069, 0x0002, 0x0066, 0x0069};
  static_assert(page_range_length == FX_ArraySize(expected),
                "Expected should be the same size as the range being "
                "extracted from page.");
  EXPECT_LT(page_range_offset + page_range_length,
            FPDFText_CountChars(textpage));

  for (int i = 0; i < page_range_length; ++i) {
    EXPECT_EQ(expected[i],
              FPDFText_GetUnicode(textpage, page_range_offset + i));
  }

  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, CountRects) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
  ASSERT_TRUE(textpage);

  // Sanity check hello_world.pdf.
  // |num_chars| check includes the terminating NUL that is provided.
  {
    unsigned short buffer[128];
    int num_chars = FPDFText_GetText(textpage, 0, 128, buffer);
    ASSERT_EQ(kHelloGoodbyeTextSize, num_chars);
    EXPECT_TRUE(check_unsigned_shorts(kHelloGoodbyeText, buffer,
                                      kHelloGoodbyeTextSize));
  }

  // Now test FPDFText_CountRects().
  static const int kHelloWorldEnd = strlen("Hello, world!");
  static const int kGoodbyeWorldStart = kHelloWorldEnd + 2;  // "\r\n"
  for (int start = 0; start < kHelloWorldEnd; ++start) {
    // Always grab some part of "hello world" and some part of "goodbye world"
    // Since -1 means "all".
    EXPECT_EQ(2, FPDFText_CountRects(textpage, start, -1));

    // No characters always means 0 rects.
    EXPECT_EQ(0, FPDFText_CountRects(textpage, start, 0));

    // 1 character stays within "hello world"
    EXPECT_EQ(1, FPDFText_CountRects(textpage, start, 1));

    // When |start| is 0, Having |kGoodbyeWorldStart| char count does not reach
    // "goodbye world".
    int expected_value = start ? 2 : 1;
    EXPECT_EQ(expected_value,
              FPDFText_CountRects(textpage, start, kGoodbyeWorldStart));

    // Extremely large character count will always return 2 rects because
    // |start| starts inside "hello world".
    EXPECT_EQ(2, FPDFText_CountRects(textpage, start, 500));
  }

  // Now test negative counts.
  for (int start = 0; start < kHelloWorldEnd; ++start) {
    EXPECT_EQ(2, FPDFText_CountRects(textpage, start, -100));
    EXPECT_EQ(2, FPDFText_CountRects(textpage, start, -2));
  }

  // Now test larger start values.
  const int kExpectedLength = strlen(kHelloGoodbyeText);
  for (int start = kGoodbyeWorldStart + 1; start < kExpectedLength; ++start) {
    EXPECT_EQ(1, FPDFText_CountRects(textpage, start, -1));
    EXPECT_EQ(0, FPDFText_CountRects(textpage, start, 0));
    EXPECT_EQ(1, FPDFText_CountRects(textpage, start, 1));
    EXPECT_EQ(1, FPDFText_CountRects(textpage, start, 2));
    EXPECT_EQ(1, FPDFText_CountRects(textpage, start, 500));
  }

  // Now test start values that starts beyond the end of the text.
  for (int start = kExpectedLength; start < 100; ++start) {
    EXPECT_EQ(0, FPDFText_CountRects(textpage, start, -1));
    EXPECT_EQ(0, FPDFText_CountRects(textpage, start, 0));
    EXPECT_EQ(0, FPDFText_CountRects(textpage, start, 1));
    EXPECT_EQ(0, FPDFText_CountRects(textpage, start, 2));
    EXPECT_EQ(0, FPDFText_CountRects(textpage, start, 500));
  }

  FPDFText_ClosePage(textpage);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, GetText) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE text_page = FPDFText_LoadPage(page);
  ASSERT_TRUE(text_page);

  EXPECT_EQ(2, FPDFPage_CountObjects(page));
  FPDF_PAGEOBJECT text_object = FPDFPage_GetObject(page, 0);
  ASSERT_TRUE(text_object);

  // Positive testing.
  constexpr char kHelloText[] = "Hello, world!";
  // Return value includes the terminating NUL that is provided.
  constexpr unsigned long kHelloUTF16Size = FX_ArraySize(kHelloText) * 2;
  constexpr wchar_t kHelloWideText[] = L"Hello, world!";
  unsigned long size = FPDFTextObj_GetText(text_object, text_page, nullptr, 0);
  ASSERT_EQ(kHelloUTF16Size, size);

  std::vector<unsigned short> buffer(size);
  ASSERT_EQ(size,
            FPDFTextObj_GetText(text_object, text_page, buffer.data(), size));
  ASSERT_EQ(kHelloWideText, GetPlatformWString(buffer.data()));

  // Negative testing.
  ASSERT_EQ(0U, FPDFTextObj_GetText(nullptr, text_page, nullptr, 0));
  ASSERT_EQ(0U, FPDFTextObj_GetText(text_object, nullptr, nullptr, 0));
  ASSERT_EQ(0U, FPDFTextObj_GetText(nullptr, nullptr, nullptr, 0));

  // Buffer is too small, ensure it's not modified.
  buffer.resize(2);
  buffer[0] = 'x';
  buffer[1] = '\0';
  size =
      FPDFTextObj_GetText(text_object, text_page, buffer.data(), buffer.size());
  ASSERT_EQ(kHelloUTF16Size, size);
  ASSERT_EQ('x', buffer[0]);
  ASSERT_EQ('\0', buffer[1]);

  FPDFText_ClosePage(text_page);
  UnloadPage(page);
}

TEST_F(FPDFTextEmbedderTest, CroppedText) {
  static constexpr int kPageCount = 4;
  static constexpr FS_RECTF kBoxes[kPageCount] = {
      {50.0f, 150.0f, 150.0f, 50.0f},
      {50.0f, 150.0f, 150.0f, 50.0f},
      {60.0f, 150.0f, 150.0f, 60.0f},
      {60.0f, 150.0f, 150.0f, 60.0f},
  };
  static constexpr const char* kExpectedText[kPageCount] = {
      " world!\r\ndbye, world!", " world!\r\ndbye, world!", "bye, world!",
      "bye, world!",
  };

  ASSERT_TRUE(OpenDocument("cropped_text.pdf"));
  ASSERT_EQ(kPageCount, FPDF_GetPageCount(document()));

  for (int i = 0; i < kPageCount; ++i) {
    FPDF_PAGE page = LoadPage(i);
    ASSERT_TRUE(page);

    FS_RECTF box;
    EXPECT_TRUE(FPDF_GetPageBoundingBox(page, &box));
    EXPECT_EQ(kBoxes[i].left, box.left);
    EXPECT_EQ(kBoxes[i].top, box.top);
    EXPECT_EQ(kBoxes[i].right, box.right);
    EXPECT_EQ(kBoxes[i].bottom, box.bottom);

    {
      ScopedFPDFTextPage textpage(FPDFText_LoadPage(page));
      ASSERT_TRUE(textpage);

      unsigned short buffer[128];
      memset(buffer, 0xbd, sizeof(buffer));
      int num_chars = FPDFText_GetText(textpage.get(), 0, 128, buffer);
      ASSERT_EQ(kHelloGoodbyeTextSize, num_chars);
      EXPECT_TRUE(check_unsigned_shorts(kHelloGoodbyeText, buffer,
                                        kHelloGoodbyeTextSize));

      int expected_char_count = strlen(kExpectedText[i]);
      ASSERT_EQ(expected_char_count,
                FPDFText_GetBoundedText(textpage.get(), box.left, box.top,
                                        box.right, box.bottom, nullptr, 0));

      memset(buffer, 0xbd, sizeof(buffer));
      ASSERT_EQ(expected_char_count + 1,
                FPDFText_GetBoundedText(textpage.get(), box.left, box.top,
                                        box.right, box.bottom, buffer, 128));
      EXPECT_TRUE(
          check_unsigned_shorts(kExpectedText[i], buffer, expected_char_count));
    }

    UnloadPage(page);
  }
}

TEST_F(FPDFTextEmbedderTest, Bug_1139) {
  ASSERT_TRUE(OpenDocument("bug_1139.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_TEXTPAGE text_page = FPDFText_LoadPage(page);
  ASSERT_TRUE(text_page);

  // -1 for CountChars not including the \0, but +1 for the extra control
  // character.
  EXPECT_EQ(kHelloGoodbyeTextSize, FPDFText_CountChars(text_page));

  // There is an extra control character at the beginning of the string, but it
  // should not appear in the output nor prevent extracting the text.
  unsigned short buffer[128];
  int num_chars = FPDFText_GetText(text_page, 0, 128, buffer);
  ASSERT_EQ(kHelloGoodbyeTextSize, num_chars);
  EXPECT_TRUE(
      check_unsigned_shorts(kHelloGoodbyeText, buffer, kHelloGoodbyeTextSize));
  FPDFText_ClosePage(text_page);
  UnloadPage(page);
}
