// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Originally from chromium's /src/base/md5_unittest.cc.

#include "core/fdrm/fx_crypt.h"

#include <memory>
#include <string>

#include "core/fxcrt/fx_memory.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

namespace {

std::string CRYPT_MD5String(const char* str) {
  return GenerateMD5Base16(reinterpret_cast<const uint8_t*>(str), strlen(str));
}

void CheckArcFourContext(const CRYPT_rc4_context& context,
                         int32_t expected_x,
                         int32_t expected_y,
                         const uint8_t* expected_permutation) {
  EXPECT_EQ(expected_x, context.x);
  EXPECT_EQ(expected_y, context.y);
  for (int32_t i = 0; i < kRC4ContextPermutationLength; ++i)
    EXPECT_EQ(expected_permutation[i], context.m[i]) << i;
}

}  // namespace

TEST(FXCRYPT, CryptToBase16) {
  uint8_t data[] = {0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
                    0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e};

  std::string actual = CryptToBase16(data);
  std::string expected = "d41d8cd98f00b204e9800998ecf8427e";

  EXPECT_EQ(expected, actual);
}

TEST(FXCRYPT, MD5GenerateEmtpyData) {
  uint8_t digest[16];
  const char data[] = "";
  uint32_t length = static_cast<uint32_t>(strlen(data));

  CRYPT_MD5Generate(reinterpret_cast<const uint8_t*>(data), length, digest);

  uint8_t expected[] = {0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
                        0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e};

  for (int i = 0; i < 16; ++i)
    EXPECT_EQ(expected[i], digest[i]);
}

TEST(FXCRYPT, MD5GenerateOneByteData) {
  uint8_t digest[16];
  const char data[] = "a";
  uint32_t length = static_cast<uint32_t>(strlen(data));

  CRYPT_MD5Generate(reinterpret_cast<const uint8_t*>(data), length, digest);

  uint8_t expected[] = {0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8,
                        0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61};

  for (int i = 0; i < 16; ++i)
    EXPECT_EQ(expected[i], digest[i]);
}

TEST(FXCRYPT, MD5GenerateLongData) {
  const uint32_t length = 10 * 1024 * 1024 + 1;
  std::unique_ptr<char[]> data(new char[length]);

  for (uint32_t i = 0; i < length; ++i)
    data[i] = i & 0xFF;

  uint8_t digest[16];
  CRYPT_MD5Generate(reinterpret_cast<const uint8_t*>(data.get()), length,
                    digest);

  uint8_t expected[] = {0x90, 0xbd, 0x6a, 0xd9, 0x0a, 0xce, 0xf5, 0xad,
                        0xaa, 0x92, 0x20, 0x3e, 0x21, 0xc7, 0xa1, 0x3e};

  for (int i = 0; i < 16; ++i)
    EXPECT_EQ(expected[i], digest[i]);
}

TEST(FXCRYPT, ContextWithEmptyData) {
  CRYPT_md5_context ctx;
  CRYPT_MD5Start(&ctx);

  uint8_t digest[16];
  CRYPT_MD5Finish(&ctx, digest);

  uint8_t expected[] = {0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
                        0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e};

  for (int i = 0; i < 16; ++i)
    EXPECT_EQ(expected[i], digest[i]);
}

TEST(FXCRYPT, ContextWithLongData) {
  CRYPT_md5_context ctx;
  CRYPT_MD5Start(&ctx);

  const uint32_t length = 10 * 1024 * 1024 + 1;
  std::unique_ptr<uint8_t[]> data(new uint8_t[length]);

  for (uint32_t i = 0; i < length; ++i)
    data[i] = i & 0xFF;

  uint32_t total = 0;
  while (total < length) {
    uint32_t len = 4097;  // intentionally not 2^k.
    if (len > length - total)
      len = length - total;

    CRYPT_MD5Update(&ctx, data.get() + total, len);
    total += len;
  }

  EXPECT_EQ(length, total);

  uint8_t digest[16];
  CRYPT_MD5Finish(&ctx, digest);

  uint8_t expected[] = {0x90, 0xbd, 0x6a, 0xd9, 0x0a, 0xce, 0xf5, 0xad,
                        0xaa, 0x92, 0x20, 0x3e, 0x21, 0xc7, 0xa1, 0x3e};

  for (int i = 0; i < 16; ++i)
    EXPECT_EQ(expected[i], digest[i]);
}

// Example data from http://www.ietf.org/rfc/rfc1321.txt A.5 Test Suite
TEST(FXCRYPT, MD5StringTestSuite1) {
  std::string actual = CRYPT_MD5String("");
  std::string expected = "d41d8cd98f00b204e9800998ecf8427e";
  EXPECT_EQ(expected, actual);
}

TEST(FXCRYPT, MD5StringTestSuite2) {
  std::string actual = CRYPT_MD5String("a");
  std::string expected = "0cc175b9c0f1b6a831c399e269772661";
  EXPECT_EQ(expected, actual);
}

TEST(FXCRYPT, MD5StringTestSuite3) {
  std::string actual = CRYPT_MD5String("abc");
  std::string expected = "900150983cd24fb0d6963f7d28e17f72";
  EXPECT_EQ(expected, actual);
}

TEST(FXCRYPT, MD5StringTestSuite4) {
  std::string actual = CRYPT_MD5String("message digest");
  std::string expected = "f96b697d7cb7938d525a2f31aaf161d0";
  EXPECT_EQ(expected, actual);
}

TEST(FXCRYPT, MD5StringTestSuite5) {
  std::string actual = CRYPT_MD5String("abcdefghijklmnopqrstuvwxyz");
  std::string expected = "c3fcd3d76192e4007dfb496cca67e13b";
  EXPECT_EQ(expected, actual);
}

TEST(FXCRYPT, MD5StringTestSuite6) {
  std::string actual = CRYPT_MD5String(
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789");
  std::string expected = "d174ab98d277d9f5a5611c2c9f419d9f";
  EXPECT_EQ(expected, actual);
}

TEST(FXCRYPT, MD5StringTestSuite7) {
  std::string actual = CRYPT_MD5String(
      "12345678901234567890"
      "12345678901234567890"
      "12345678901234567890"
      "12345678901234567890");
  std::string expected = "57edf4a22be3c955ac49da2e2107b67a";
  EXPECT_EQ(expected, actual);
}

TEST(FXCRYPT, ContextWithStringData) {
  CRYPT_md5_context ctx;
  CRYPT_MD5Start(&ctx);
  CRYPT_MD5Update(&ctx, reinterpret_cast<const uint8_t*>("abc"), 3);

  uint8_t digest[16];
  CRYPT_MD5Finish(&ctx, digest);

  std::string actual = CryptToBase16(digest);
  std::string expected = "900150983cd24fb0d6963f7d28e17f72";
  EXPECT_EQ(expected, actual);
}

TEST(FXCRYPT, Sha256TestB1) {
  // Example B.1 from FIPS 180-2: one-block message.
  const char* input = "abc";
  const uint8_t expected[32] = {0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
                                0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
                                0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
                                0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad};
  uint8_t actual[32];
  CRYPT_SHA256Generate(reinterpret_cast<const uint8_t*>(input), strlen(input),
                       actual);
  for (size_t i = 0; i < 32; ++i)
    EXPECT_EQ(expected[i], actual[i]) << " at byte " << i;
}

TEST(FXCRYPT, Sha256TestB2) {
  // Example B.2 from FIPS 180-2: multi-block message.
  const char* input =
      "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
  const uint8_t expected[32] = {0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
                                0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
                                0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
                                0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1};
  uint8_t actual[32];
  CRYPT_SHA256Generate(reinterpret_cast<const uint8_t*>(input), strlen(input),
                       actual);
  for (size_t i = 0; i < 32; ++i)
    EXPECT_EQ(expected[i], actual[i]) << " at byte " << i;
}

TEST(FXCRYPT, CRYPT_ArcFourSetup) {
  {
    const uint8_t kNullPermutation[kRC4ContextPermutationLength] = {
        0,   35,  3,   43,  9,   11,  65,  229, 32,  36,  134, 98,  59,  34,
        173, 153, 214, 200, 64,  161, 191, 62,  6,   25,  56,  234, 49,  246,
        69,  133, 203, 194, 10,  42,  228, 198, 195, 245, 236, 91,  206, 23,
        235, 27,  138, 18,  143, 250, 244, 76,  123, 217, 132, 249, 72,  127,
        94,  151, 33,  60,  248, 85,  177, 210, 142, 83,  110, 140, 41,  135,
        196, 238, 156, 242, 141, 67,  5,   185, 131, 63,  137, 37,  172, 121,
        70,  144, 237, 130, 17,  44,  253, 166, 78,  201, 12,  119, 215, 7,
        126, 114, 97,  192, 53,  4,   254, 45,  102, 122, 230, 88,  193, 129,
        160, 124, 84,  108, 239, 189, 152, 120, 115, 207, 50,  176, 86,  157,
        164, 187, 71,  1,   15,  58,  29,  21,  46,  145, 247, 162, 95,  183,
        13,  226, 159, 175, 221, 100, 96,  202, 101, 178, 154, 47,  205, 106,
        148, 104, 93,  112, 26,  165, 128, 186, 146, 218, 66,  211, 171, 90,
        252, 19,  40,  99,  223, 174, 255, 51,  77,  227, 48,  220, 168, 118,
        224, 103, 75,  105, 125, 199, 73,  82,  57,  181, 81,  149, 68,  52,
        232, 22,  2,   216, 113, 30,  109, 163, 92,  61,  14,  8,   38,  225,
        79,  231, 170, 240, 20,  219, 204, 150, 180, 188, 116, 190, 241, 197,
        179, 87,  74,  147, 80,  54,  212, 16,  167, 222, 136, 213, 55,  182,
        139, 24,  209, 251, 208, 28,  111, 89,  158, 155, 243, 107, 233, 169,
        117, 184, 31,  39};
    CRYPT_rc4_context context;
    CRYPT_ArcFourSetup(&context, nullptr, 0);
    CheckArcFourContext(context, 0, 0, kNullPermutation);
  }
  {
    const uint8_t kFoobarPermutation[kRC4ContextPermutationLength] = {
        102, 214, 39,  49,  17,  132, 244, 106, 114, 76,  183, 212, 116, 73,
        42,  103, 128, 246, 139, 199, 31,  234, 25,  109, 48,  19,  121, 4,
        20,  54,  134, 77,  163, 38,  61,  101, 145, 78,  215, 96,  92,  80,
        224, 168, 243, 210, 82,  252, 113, 56,  217, 62,  218, 129, 125, 33,
        99,  9,   153, 59,  43,  13,  206, 124, 131, 18,  213, 118, 173, 122,
        193, 172, 177, 105, 148, 207, 186, 5,   85,  32,  68,  220, 79,  84,
        169, 209, 150, 7,   133, 63,  147, 93,  26,  130, 60,  117, 250, 57,
        24,  247, 200, 127, 136, 66,  112, 107, 140, 154, 70,  170, 185, 138,
        248, 236, 88,  86,  44,  216, 241, 35,  100, 151, 156, 74,  119, 55,
        245, 46,  227, 208, 229, 16,  249, 149, 53,  157, 201, 75,  58,  28,
        142, 238, 182, 180, 179, 144, 12,  6,   176, 10,  90,  239, 104, 40,
        181, 194, 137, 69,  221, 205, 165, 188, 191, 87,  1,   91,  2,   171,
        232, 34,  162, 166, 160, 126, 225, 167, 123, 197, 223, 195, 22,  203,
        189, 237, 37,  27,  222, 175, 23,  143, 152, 192, 21,  231, 228, 141,
        30,  204, 158, 240, 120, 98,  89,  83,  135, 251, 81,  196, 161, 3,
        8,   230, 52,  219, 41,  242, 36,  97,  15,  155, 65,  187, 254, 64,
        159, 67,  211, 108, 178, 146, 202, 11,  164, 226, 184, 50,  190, 174,
        71,  233, 235, 198, 95,  51,  110, 255, 253, 72,  115, 0,   47,  94,
        29,  45,  14,  111};
    CRYPT_rc4_context context;
    const uint8_t kFooBar[] = "foobar";
    CRYPT_ArcFourSetup(&context, kFooBar, FX_ArraySize(kFooBar) - 1);
    CheckArcFourContext(context, 0, 0, kFoobarPermutation);
  }
}

TEST(FXCRYPT, CRYPT_ArcFourCrypt) {
  const uint8_t kDataShort[] = "The Quick Fox Jumped Over The Lazy Brown Dog.";
  const uint8_t kDataLong[] =
      "The Quick Fox Jumped Over The Lazy Brown Dog.\n"
      "1234567890123456789012345678901234567890123456789012345678901234567890\n"
      "1234567890123456789012345678901234567890123456789012345678901234567890\n"
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
      "!@#$%^&*()[]{};':\",.<>/?\\|\r\t\n";
  {
    CRYPT_rc4_context context;
    CRYPT_ArcFourSetup(&context, nullptr, 0);

    uint8_t data_short[FX_ArraySize(kDataShort)];
    memcpy(data_short, kDataShort, FX_ArraySize(kDataShort));
    const uint8_t kExpectedEncryptedDataShort[] = {
        138, 112, 236, 97,  242, 66,  52,  89,  225, 38,  88,  8,
        47,  78,  216, 24,  170, 106, 26,  199, 208, 131, 157, 242,
        55,  11,  25,  90,  66,  182, 19,  255, 210, 181, 85,  69,
        31,  240, 206, 171, 97,  62,  202, 172, 30,  252};
    static_assert(
        FX_ArraySize(kExpectedEncryptedDataShort) == FX_ArraySize(data_short),
        "data_short mismatch");
    CRYPT_ArcFourCrypt(&context, data_short, FX_ArraySize(data_short));
    for (size_t i = 0; i < FX_ArraySize(data_short); ++i)
      EXPECT_EQ(kExpectedEncryptedDataShort[i], data_short[i]) << i;

    const uint8_t kPermutation[kRC4ContextPermutationLength] = {
        0,   198, 10,  37,  253, 192, 171, 183, 99,  8,   144, 103, 208, 191,
        149, 9,   228, 243, 94,  150, 169, 151, 210, 206, 221, 235, 32,  186,
        212, 122, 72,  200, 236, 138, 244, 217, 158, 213, 139, 242, 17,  143,
        50,  132, 12,  160, 145, 250, 214, 76,  123, 35,  27,  249, 203, 127,
        64,  62,  33,  60,  248, 85,  177, 6,   142, 83,  110, 140, 41,  135,
        196, 238, 156, 91,  141, 67,  5,   185, 131, 63,  137, 43,  172, 121,
        70,  134, 237, 130, 25,  44,  153, 166, 78,  201, 42,  119, 215, 7,
        126, 114, 97,  11,  53,  4,   254, 45,  102, 133, 230, 88,  193, 129,
        18,  124, 84,  108, 239, 189, 152, 120, 115, 207, 234, 176, 86,  157,
        164, 187, 71,  1,   15,  58,  29,  21,  46,  23,  247, 162, 95,  229,
        13,  226, 159, 175, 56,  100, 96,  202, 101, 178, 154, 47,  205, 106,
        148, 104, 93,  112, 26,  165, 128, 246, 146, 218, 66,  211, 65,  90,
        252, 19,  40,  49,  223, 174, 255, 51,  77,  227, 48,  220, 168, 118,
        224, 98,  75,  105, 125, 199, 73,  82,  57,  181, 81,  173, 68,  52,
        232, 22,  2,   216, 113, 30,  109, 163, 92,  61,  14,  36,  38,  225,
        79,  231, 170, 240, 20,  219, 204, 161, 180, 188, 116, 190, 241, 197,
        179, 87,  74,  147, 80,  54,  69,  16,  167, 222, 136, 245, 55,  182,
        3,   24,  209, 251, 59,  28,  111, 89,  195, 155, 194, 107, 233, 34,
        117, 184, 31,  39};
    CheckArcFourContext(context, 46, 135, kPermutation);
  }
  {
    CRYPT_rc4_context context;
    CRYPT_ArcFourSetup(&context, nullptr, 0);

    uint8_t data_long[FX_ArraySize(kDataLong)];
    memcpy(data_long, kDataLong, FX_ArraySize(kDataLong));
    const uint8_t kExpectedEncryptedDataLong[] = {
        138, 112, 236, 97,  242, 66,  52,  89,  225, 38,  88,  8,   47,  78,
        216, 24,  170, 106, 26,  199, 208, 131, 157, 242, 55,  11,  25,  90,
        66,  182, 19,  255, 210, 181, 85,  69,  31,  240, 206, 171, 97,  62,
        202, 172, 30,  246, 19,  43,  184, 0,   173, 27,  140, 90,  167, 240,
        122, 125, 184, 49,  149, 71,  63,  104, 171, 144, 242, 106, 121, 124,
        209, 149, 61,  1,   66,  186, 252, 47,  51,  170, 253, 75,  95,  41,
        203, 28,  197, 174, 144, 209, 166, 98,  142, 125, 44,  5,   147, 42,
        73,  178, 119, 90,  253, 69,  103, 178, 15,  136, 51,  112, 39,  81,
        37,  111, 129, 232, 106, 159, 126, 142, 120, 124, 48,  140, 253, 12,
        223, 208, 106, 76,  60,  238, 5,   162, 100, 226, 251, 156, 169, 35,
        193, 10,  242, 210, 20,  96,  37,  84,  99,  183, 179, 203, 62,  122,
        54,  6,   51,  239, 142, 250, 238, 41,  223, 58,  48,  101, 29,  187,
        43,  235, 3,   5,   176, 33,  14,  171, 36,  26,  234, 207, 105, 79,
        69,  126, 82,  183, 105, 228, 31,  173, 8,   240, 99,  5,   147, 206,
        215, 140, 48,  190, 165, 50,  41,  232, 29,  105, 156, 64,  229, 165,
        12,  64,  163, 255, 146, 108, 212, 125, 142, 101, 13,  99,  174, 10,
        160, 68,  196, 120, 110, 201, 254, 158, 97,  215, 0,   207, 90,  23,
        208, 161, 105, 226, 164, 114, 80,  137, 58,  107, 109, 42,  110, 100,
        202, 170, 224, 89,  28,  5,   138, 19,  253, 105, 220, 105, 24,  187,
        109, 89,  205, 89,  202};
    static_assert(
        FX_ArraySize(kExpectedEncryptedDataLong) == FX_ArraySize(data_long),
        "data_long mismatch");
    static_assert(FX_ArraySize(data_long) > 256, "too short");
    CRYPT_ArcFourCrypt(&context, data_long, FX_ArraySize(data_long));
    for (size_t i = 0; i < FX_ArraySize(data_long); ++i)
      EXPECT_EQ(kExpectedEncryptedDataLong[i], data_long[i]) << i;

    const uint8_t kPermutation[kRC4ContextPermutationLength] = {
        172, 59,  196, 72,  101, 21,  215, 210, 212, 52,  243, 73,  47,  213,
        211, 50,  228, 144, 66,  93,  169, 31,  237, 206, 221, 235, 222, 250,
        97,  87,  174, 164, 190, 111, 27,  217, 173, 189, 65,  11,  115, 171,
        104, 132, 12,  170, 205, 114, 7,   105, 37,  83,  78,  134, 236, 70,
        197, 122, 177, 202, 39,  195, 30,  3,   86,  127, 74,  106, 68,  91,
        110, 121, 208, 25,  56,  6,   28,  225, 163, 193, 166, 244, 119, 34,
        23,  88,  108, 123, 162, 159, 242, 61,  230, 227, 254, 14,  4,   156,
        161, 44,  58,  153, 33,  143, 129, 232, 182, 152, 76,  168, 238, 239,
        185, 219, 233, 16,  188, 45,  40,  35,  103, 99,  89,  157, 241, 245,
        192, 180, 248, 8,   85,  231, 146, 154, 252, 181, 107, 126, 98,  80,
        102, 165, 199, 94,  49,  255, 18,  204, 216, 77,  20,  187, 145, 125,
        1,   247, 79,  26,  207, 81,  117, 179, 186, 38,  175, 19,  139, 138,
        149, 54,  64,  109, 249, 135, 142, 118, 17,  13,  201, 184, 55,  224,
        209, 155, 113, 218, 82,  131, 178, 253, 140, 226, 43,  42,  24,  29,
        229, 200, 137, 240, 203, 167, 95,  148, 15,  176, 60,  75,  53,  41,
        150, 112, 160, 96,  22,  10,  234, 116, 130, 158, 214, 36,  9,   67,
        198, 194, 191, 100, 124, 147, 32,  183, 120, 246, 51,  141, 46,  251,
        92,  223, 133, 63,  0,   71,  48,  128, 220, 90,  62,  136, 2,   5,
        69,  57,  151, 84};
    CheckArcFourContext(context, 15, 222, kPermutation);
  }
  {
    CRYPT_rc4_context context;
    const uint8_t kFooBar[] = "foobar";
    CRYPT_ArcFourSetup(&context, kFooBar, FX_ArraySize(kFooBar) - 1);

    uint8_t data_short[FX_ArraySize(kDataShort)];
    memcpy(data_short, kDataShort, FX_ArraySize(kDataShort));
    const uint8_t kExpectedEncryptedDataShort[] = {
        59,  193, 117, 206, 167, 54,  218, 7,   229, 214, 188, 55,
        90,  205, 196, 25,  36,  114, 199, 218, 161, 107, 122, 119,
        106, 167, 44,  175, 240, 123, 192, 102, 174, 167, 105, 187,
        202, 70,  121, 81,  17,  30,  5,   138, 116, 166};
    static_assert(
        FX_ArraySize(kExpectedEncryptedDataShort) == FX_ArraySize(data_short),
        "data_short mismatch");
    CRYPT_ArcFourCrypt(&context, data_short, FX_ArraySize(data_short));
    for (size_t i = 0; i < FX_ArraySize(data_short); ++i)
      EXPECT_EQ(kExpectedEncryptedDataShort[i], data_short[i]) << i;

    const uint8_t kPermutation[kRC4ContextPermutationLength] = {
        102, 41,  45,  82,  124, 141, 237, 38,  6,   64,  90,  140, 254, 96,
        220, 109, 99,  49,  27,  227, 205, 75,  191, 37,  17,  54,  83,  196,
        108, 79,  31,  190, 180, 0,   125, 194, 243, 156, 224, 246, 253, 193,
        42,  81,  117, 56,  181, 252, 113, 210, 217, 62,  218, 129, 61,  33,
        128, 9,   153, 59,  43,  13,  206, 48,  131, 18,  213, 118, 173, 122,
        80,  172, 177, 105, 148, 207, 186, 5,   85,  32,  68,  215, 19,  84,
        169, 209, 150, 7,   133, 63,  147, 93,  26,  130, 60,  145, 250, 57,
        24,  247, 200, 127, 136, 66,  112, 107, 212, 154, 70,  170, 185, 138,
        248, 236, 88,  86,  44,  216, 241, 35,  100, 151, 78,  74,  119, 55,
        245, 46,  199, 208, 229, 16,  249, 149, 53,  157, 201, 234, 58,  28,
        142, 238, 182, 163, 179, 144, 12,  114, 176, 10,  183, 239, 104, 40,
        73,  101, 137, 69,  221, 134, 165, 188, 25,  87,  1,   91,  2,   171,
        232, 34,  162, 166, 160, 126, 225, 167, 123, 197, 223, 195, 22,  203,
        189, 244, 103, 139, 222, 175, 23,  143, 152, 192, 21,  231, 228, 132,
        30,  204, 158, 240, 120, 98,  89,  121, 135, 251, 168, 4,   161, 3,
        8,   230, 52,  219, 214, 242, 36,  97,  15,  155, 65,  187, 116, 76,
        159, 67,  211, 20,  178, 146, 202, 11,  164, 226, 184, 50,  77,  174,
        71,  233, 235, 198, 95,  51,  110, 255, 92,  72,  115, 106, 47,  94,
        29,  39,  14,  111};
    CheckArcFourContext(context, 46, 39, kPermutation);
  }
  {
    CRYPT_rc4_context context;
    const uint8_t kFooBar[] = "foobar";
    CRYPT_ArcFourSetup(&context, kFooBar, FX_ArraySize(kFooBar) - 1);

    uint8_t data_long[FX_ArraySize(kDataLong)];
    memcpy(data_long, kDataLong, FX_ArraySize(kDataLong));
    const uint8_t kExpectedEncryptedDataLong[] = {
        59,  193, 117, 206, 167, 54,  218, 7,   229, 214, 188, 55,  90,  205,
        196, 25,  36,  114, 199, 218, 161, 107, 122, 119, 106, 167, 44,  175,
        240, 123, 192, 102, 174, 167, 105, 187, 202, 70,  121, 81,  17,  30,
        5,   138, 116, 172, 169, 50,  160, 116, 237, 117, 108, 241, 127, 61,
        83,  45,  77,  176, 0,   106, 191, 221, 132, 143, 219, 94,  2,   235,
        204, 166, 201, 139, 140, 163, 104, 115, 48,  37,  18,  114, 168, 49,
        235, 163, 179, 131, 182, 218, 120, 200, 9,   90,  60,  47,  55,  235,
        135, 37,  21,  170, 48,  112, 185, 169, 43,  233, 88,  134, 117, 126,
        248, 40,  176, 248, 30,  131, 108, 43,  139, 68,  232, 219, 7,   39,
        223, 45,  199, 243, 54,  171, 31,  37,  161, 24,  38,  251, 13,  144,
        106, 215, 179, 203, 5,   253, 25,  32,  25,  146, 109, 193, 143, 141,
        177, 226, 134, 222, 95,  79,  156, 202, 240, 34,  153, 145, 169, 150,
        231, 63,  113, 242, 156, 39,  136, 249, 108, 50,  181, 22,  22,  180,
        57,  76,  69,  62,  254, 47,  141, 249, 235, 90,  25,  34,  40,  194,
        66,  86,  110, 192, 235, 191, 205, 133, 91,  32,  104, 65,  43,  36,
        140, 36,  228, 156, 105, 251, 169, 168, 203, 189, 238, 221, 64,  200,
        68,  137, 153, 9,   183, 84,  153, 140, 239, 0,   15,  50,  126, 145,
        22,  110, 43,  56,  94,  127, 48,  96,  47,  172, 3,   31,  130, 249,
        243, 73,  206, 89,  9,   93,  156, 167, 205, 166, 75,  227, 36,  34,
        81,  124, 195, 246, 152};
    static_assert(
        FX_ArraySize(kExpectedEncryptedDataLong) == FX_ArraySize(data_long),
        "data_long mismatch");
    static_assert(FX_ArraySize(data_long) > 256, "too short");
    CRYPT_ArcFourCrypt(&context, data_long, FX_ArraySize(data_long));
    for (size_t i = 0; i < FX_ArraySize(data_long); ++i)
      EXPECT_EQ(kExpectedEncryptedDataLong[i], data_long[i]) << i;

    const uint8_t kPermutation[kRC4ContextPermutationLength] = {
        188, 12,  81,  130, 228, 58,  124, 218, 72,  210, 50,  70,  166, 38,
        110, 111, 73,  49,  27,  227, 249, 21,  1,   226, 17,  54,  53,  16,
        108, 51,  31,  123, 221, 23,  125, 148, 5,   200, 208, 246, 253, 193,
        42,  45,  236, 56,  230, 194, 178, 213, 120, 116, 7,   164, 33,  107,
        189, 20,  133, 114, 173, 161, 59,  128, 3,   238, 65,  69,  144, 179,
        44,  35,  8,   163, 252, 195, 160, 197, 204, 28,  34,  129, 67,  89,
        22,  149, 199, 131, 182, 46,  250, 222, 155, 104, 10,  32,  139, 245,
        90,  41,  132, 224, 83,  242, 135, 75,  74,  61,  62,  141, 43,  127,
        255, 91,  170, 78,  157, 101, 243, 216, 254, 156, 229, 118, 174, 147,
        103, 76,  196, 145, 134, 94,  205, 146, 202, 98,  100, 106, 232, 177,
        187, 13,  80,  137, 151, 11,  82,  40,  167, 175, 25,  219, 168, 240,
        99,  55,  4,   19,  180, 2,   203, 18,  171, 154, 113, 117, 6,   185,
        172, 186, 237, 223, 233, 244, 217, 191, 190, 198, 97,  165, 220, 9,
        214, 150, 184, 143, 206, 24,  209, 207, 36,  142, 87,  15,  159, 71,
        84,  162, 169, 86,  48,  47,  140, 215, 241, 235, 158, 14,  26,  248,
        138, 119, 212, 39,  88,  121, 96,  109, 29,  66,  136, 102, 225, 92,
        201, 126, 122, 192, 60,  0,   64,  239, 183, 37,  57,  63,  234, 181,
        153, 52,  176, 112, 93,  79,  77,  115, 231, 30,  95,  251, 211, 68,
        105, 85,  247, 152};
    CheckArcFourContext(context, 15, 68, kPermutation);
  }
}

TEST(FXCRYPT, Sha512Test) {
  const char* const input =
      "This is a simple test. To see whether it is getting correct value.";
  const uint8_t expected[64] = {
      0x86, 0xB5, 0x05, 0x63, 0xA2, 0x6F, 0xD6, 0xFA, 0xEB, 0x9B, 0xC3,
      0xBB, 0x9E, 0xB7, 0x03, 0x82, 0xB6, 0x50, 0x55, 0x6B, 0x90, 0x69,
      0xD0, 0xA7, 0x53, 0x0A, 0x34, 0xDD, 0xEA, 0x11, 0xCC, 0x91, 0x5C,
      0xC7, 0x93, 0xCA, 0xAE, 0x30, 0xD1, 0x96, 0xBE, 0xD0, 0x35, 0x21,
      0x4A, 0xC6, 0x42, 0x56, 0x0C, 0xA3, 0x00, 0x69, 0x44, 0x77, 0xCC,
      0x3E, 0xD4, 0xD6, 0x10, 0x31, 0xC6, 0xC0, 0x58, 0xCF};
  uint8_t actual[64];
  CRYPT_SHA512Generate(reinterpret_cast<const uint8_t*>(input), strlen(input),
                       actual);
  for (size_t i = 0; i < 64; ++i)
    EXPECT_EQ(expected[i], actual[i]) << " at byte " << i;
}
