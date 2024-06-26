// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fdrm/fx_crypt.h"

#define rol(x, y) (((x) << (y)) | (((unsigned int)x) >> (32 - y)))

#define SHA_GET_UINT32(n, b, i)                                         \
  {                                                                     \
    (n) = ((uint32_t)(b)[(i)] << 24) | ((uint32_t)(b)[(i) + 1] << 16) | \
          ((uint32_t)(b)[(i) + 2] << 8) | ((uint32_t)(b)[(i) + 3]);     \
  }
#define SHA_PUT_UINT32(n, b, i)          \
  {                                      \
    (b)[(i)] = (uint8_t)((n) >> 24);     \
    (b)[(i) + 1] = (uint8_t)((n) >> 16); \
    (b)[(i) + 2] = (uint8_t)((n) >> 8);  \
    (b)[(i) + 3] = (uint8_t)((n));       \
  }

#define SHA384_F0(x, y, z) ((x & y) | (z & (x | y)))
#define SHA384_F1(x, y, z) (z ^ (x & (y ^ z)))
#define SHA384_SHR(x, n) (x >> n)
#define SHA384_ROTR(x, n) (SHA384_SHR(x, n) | x << (64 - n))
#define SHA384_S0(x) (SHA384_ROTR(x, 1) ^ SHA384_ROTR(x, 8) ^ SHA384_SHR(x, 7))
#define SHA384_S1(x) \
  (SHA384_ROTR(x, 19) ^ SHA384_ROTR(x, 61) ^ SHA384_SHR(x, 6))
#define SHA384_S2(x) \
  (SHA384_ROTR(x, 28) ^ SHA384_ROTR(x, 34) ^ SHA384_ROTR(x, 39))
#define SHA384_S3(x) \
  (SHA384_ROTR(x, 14) ^ SHA384_ROTR(x, 18) ^ SHA384_ROTR(x, 41))
#define SHA384_P(a, b, c, d, e, f, g, h, x, K)             \
  {                                                        \
    temp1 = h + SHA384_S3(e) + SHA384_F1(e, f, g) + K + x; \
    temp2 = SHA384_S2(a) + SHA384_F0(a, b, c);             \
    d += temp1;                                            \
    h = temp1 + temp2;                                     \
  }
#define SHA384_R(t) \
  (W[t] = SHA384_S1(W[t - 2]) + W[t - 7] + SHA384_S0(W[t - 15]) + W[t - 16])

#define GET_FX_64WORD(n, b, i)                                              \
  {                                                                         \
    (n) = ((uint64_t)(b)[(i)] << 56) | ((uint64_t)(b)[(i) + 1] << 48) |     \
          ((uint64_t)(b)[(i) + 2] << 40) | ((uint64_t)(b)[(i) + 3] << 32) | \
          ((uint64_t)(b)[(i) + 4] << 24) | ((uint64_t)(b)[(i) + 5] << 16) | \
          ((uint64_t)(b)[(i) + 6] << 8) | ((uint64_t)(b)[(i) + 7]);         \
  }
#define PUT_UINT64(n, b, i)              \
  {                                      \
    (b)[(i)] = (uint8_t)((n) >> 56);     \
    (b)[(i) + 1] = (uint8_t)((n) >> 48); \
    (b)[(i) + 2] = (uint8_t)((n) >> 40); \
    (b)[(i) + 3] = (uint8_t)((n) >> 32); \
    (b)[(i) + 4] = (uint8_t)((n) >> 24); \
    (b)[(i) + 5] = (uint8_t)((n) >> 16); \
    (b)[(i) + 6] = (uint8_t)((n) >> 8);  \
    (b)[(i) + 7] = (uint8_t)((n));       \
  }

#define SHR(x, n) ((x & 0xFFFFFFFF) >> n)
#define ROTR(x, n) (SHR(x, n) | (x << (32 - n)))
#define S0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define S1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))
#define S2(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define S3(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define F0(x, y, z) ((x & y) | (z & (x | y)))
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define R(t) (W[t] = S1(W[t - 2]) + W[t - 7] + S0(W[t - 15]) + W[t - 16])
#define PS(a, b, c, d, e, f, g, h, x, K)     \
  {                                          \
    temp1 = h + S3(e) + F1(e, f, g) + K + x; \
    temp2 = S2(a) + F0(a, b, c);             \
    d += temp1;                              \
    h = temp1 + temp2;                       \
  }

namespace {

void SHA_Core_Init(unsigned int h[5]) {
  h[0] = 0x67452301;
  h[1] = 0xefcdab89;
  h[2] = 0x98badcfe;
  h[3] = 0x10325476;
  h[4] = 0xc3d2e1f0;
}

void SHATransform(unsigned int* digest, unsigned int* block) {
  unsigned int w[80];
  unsigned int a, b, c, d, e;
  int t;
  for (t = 0; t < 16; t++) {
    w[t] = block[t];
  }
  for (t = 16; t < 80; t++) {
    unsigned int tmp = w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16];
    w[t] = rol(tmp, 1);
  }
  a = digest[0];
  b = digest[1];
  c = digest[2];
  d = digest[3];
  e = digest[4];
  for (t = 0; t < 20; t++) {
    unsigned int tmp = rol(a, 5) + ((b & c) | (d & ~b)) + e + w[t] + 0x5a827999;
    e = d;
    d = c;
    c = rol(b, 30);
    b = a;
    a = tmp;
  }
  for (t = 20; t < 40; t++) {
    unsigned int tmp = rol(a, 5) + (b ^ c ^ d) + e + w[t] + 0x6ed9eba1;
    e = d;
    d = c;
    c = rol(b, 30);
    b = a;
    a = tmp;
  }
  for (t = 40; t < 60; t++) {
    unsigned int tmp =
        rol(a, 5) + ((b & c) | (b & d) | (c & d)) + e + w[t] + 0x8f1bbcdc;
    e = d;
    d = c;
    c = rol(b, 30);
    b = a;
    a = tmp;
  }
  for (t = 60; t < 80; t++) {
    unsigned int tmp = rol(a, 5) + (b ^ c ^ d) + e + w[t] + 0xca62c1d6;
    e = d;
    d = c;
    c = rol(b, 30);
    b = a;
    a = tmp;
  }
  digest[0] += a;
  digest[1] += b;
  digest[2] += c;
  digest[3] += d;
  digest[4] += e;
}

void sha256_process(CRYPT_sha2_context* ctx, const uint8_t data[64]) {
  uint32_t W[64];
  SHA_GET_UINT32(W[0], data, 0);
  SHA_GET_UINT32(W[1], data, 4);
  SHA_GET_UINT32(W[2], data, 8);
  SHA_GET_UINT32(W[3], data, 12);
  SHA_GET_UINT32(W[4], data, 16);
  SHA_GET_UINT32(W[5], data, 20);
  SHA_GET_UINT32(W[6], data, 24);
  SHA_GET_UINT32(W[7], data, 28);
  SHA_GET_UINT32(W[8], data, 32);
  SHA_GET_UINT32(W[9], data, 36);
  SHA_GET_UINT32(W[10], data, 40);
  SHA_GET_UINT32(W[11], data, 44);
  SHA_GET_UINT32(W[12], data, 48);
  SHA_GET_UINT32(W[13], data, 52);
  SHA_GET_UINT32(W[14], data, 56);
  SHA_GET_UINT32(W[15], data, 60);

  uint32_t temp1;
  uint32_t temp2;
  uint32_t A = ctx->state[0];
  uint32_t B = ctx->state[1];
  uint32_t C = ctx->state[2];
  uint32_t D = ctx->state[3];
  uint32_t E = ctx->state[4];
  uint32_t F = ctx->state[5];
  uint32_t G = ctx->state[6];
  uint32_t H = ctx->state[7];
  PS(A, B, C, D, E, F, G, H, W[0], 0x428A2F98);
  PS(H, A, B, C, D, E, F, G, W[1], 0x71374491);
  PS(G, H, A, B, C, D, E, F, W[2], 0xB5C0FBCF);
  PS(F, G, H, A, B, C, D, E, W[3], 0xE9B5DBA5);
  PS(E, F, G, H, A, B, C, D, W[4], 0x3956C25B);
  PS(D, E, F, G, H, A, B, C, W[5], 0x59F111F1);
  PS(C, D, E, F, G, H, A, B, W[6], 0x923F82A4);
  PS(B, C, D, E, F, G, H, A, W[7], 0xAB1C5ED5);
  PS(A, B, C, D, E, F, G, H, W[8], 0xD807AA98);
  PS(H, A, B, C, D, E, F, G, W[9], 0x12835B01);
  PS(G, H, A, B, C, D, E, F, W[10], 0x243185BE);
  PS(F, G, H, A, B, C, D, E, W[11], 0x550C7DC3);
  PS(E, F, G, H, A, B, C, D, W[12], 0x72BE5D74);
  PS(D, E, F, G, H, A, B, C, W[13], 0x80DEB1FE);
  PS(C, D, E, F, G, H, A, B, W[14], 0x9BDC06A7);
  PS(B, C, D, E, F, G, H, A, W[15], 0xC19BF174);
  PS(A, B, C, D, E, F, G, H, R(16), 0xE49B69C1);
  PS(H, A, B, C, D, E, F, G, R(17), 0xEFBE4786);
  PS(G, H, A, B, C, D, E, F, R(18), 0x0FC19DC6);
  PS(F, G, H, A, B, C, D, E, R(19), 0x240CA1CC);
  PS(E, F, G, H, A, B, C, D, R(20), 0x2DE92C6F);
  PS(D, E, F, G, H, A, B, C, R(21), 0x4A7484AA);
  PS(C, D, E, F, G, H, A, B, R(22), 0x5CB0A9DC);
  PS(B, C, D, E, F, G, H, A, R(23), 0x76F988DA);
  PS(A, B, C, D, E, F, G, H, R(24), 0x983E5152);
  PS(H, A, B, C, D, E, F, G, R(25), 0xA831C66D);
  PS(G, H, A, B, C, D, E, F, R(26), 0xB00327C8);
  PS(F, G, H, A, B, C, D, E, R(27), 0xBF597FC7);
  PS(E, F, G, H, A, B, C, D, R(28), 0xC6E00BF3);
  PS(D, E, F, G, H, A, B, C, R(29), 0xD5A79147);
  PS(C, D, E, F, G, H, A, B, R(30), 0x06CA6351);
  PS(B, C, D, E, F, G, H, A, R(31), 0x14292967);
  PS(A, B, C, D, E, F, G, H, R(32), 0x27B70A85);
  PS(H, A, B, C, D, E, F, G, R(33), 0x2E1B2138);
  PS(G, H, A, B, C, D, E, F, R(34), 0x4D2C6DFC);
  PS(F, G, H, A, B, C, D, E, R(35), 0x53380D13);
  PS(E, F, G, H, A, B, C, D, R(36), 0x650A7354);
  PS(D, E, F, G, H, A, B, C, R(37), 0x766A0ABB);
  PS(C, D, E, F, G, H, A, B, R(38), 0x81C2C92E);
  PS(B, C, D, E, F, G, H, A, R(39), 0x92722C85);
  PS(A, B, C, D, E, F, G, H, R(40), 0xA2BFE8A1);
  PS(H, A, B, C, D, E, F, G, R(41), 0xA81A664B);
  PS(G, H, A, B, C, D, E, F, R(42), 0xC24B8B70);
  PS(F, G, H, A, B, C, D, E, R(43), 0xC76C51A3);
  PS(E, F, G, H, A, B, C, D, R(44), 0xD192E819);
  PS(D, E, F, G, H, A, B, C, R(45), 0xD6990624);
  PS(C, D, E, F, G, H, A, B, R(46), 0xF40E3585);
  PS(B, C, D, E, F, G, H, A, R(47), 0x106AA070);
  PS(A, B, C, D, E, F, G, H, R(48), 0x19A4C116);
  PS(H, A, B, C, D, E, F, G, R(49), 0x1E376C08);
  PS(G, H, A, B, C, D, E, F, R(50), 0x2748774C);
  PS(F, G, H, A, B, C, D, E, R(51), 0x34B0BCB5);
  PS(E, F, G, H, A, B, C, D, R(52), 0x391C0CB3);
  PS(D, E, F, G, H, A, B, C, R(53), 0x4ED8AA4A);
  PS(C, D, E, F, G, H, A, B, R(54), 0x5B9CCA4F);
  PS(B, C, D, E, F, G, H, A, R(55), 0x682E6FF3);
  PS(A, B, C, D, E, F, G, H, R(56), 0x748F82EE);
  PS(H, A, B, C, D, E, F, G, R(57), 0x78A5636F);
  PS(G, H, A, B, C, D, E, F, R(58), 0x84C87814);
  PS(F, G, H, A, B, C, D, E, R(59), 0x8CC70208);
  PS(E, F, G, H, A, B, C, D, R(60), 0x90BEFFFA);
  PS(D, E, F, G, H, A, B, C, R(61), 0xA4506CEB);
  PS(C, D, E, F, G, H, A, B, R(62), 0xBEF9A3F7);
  PS(B, C, D, E, F, G, H, A, R(63), 0xC67178F2);
  ctx->state[0] += A;
  ctx->state[1] += B;
  ctx->state[2] += C;
  ctx->state[3] += D;
  ctx->state[4] += E;
  ctx->state[5] += F;
  ctx->state[6] += G;
  ctx->state[7] += H;
}

const uint8_t sha256_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const uint8_t sha384_padding[128] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

uint64_t const constants[] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL,
    0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
    0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL,
    0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
    0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
    0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL, 0x2de92c6f592b0275ULL,
    0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL,
    0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
    0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL,
    0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL,
    0x92722c851482353bULL, 0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
    0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
    0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL,
    0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
    0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL,
    0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL,
    0xc67178f2e372532bULL, 0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
    0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL,
    0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
    0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
    0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL,
};

void sha384_process(CRYPT_sha2_context* ctx, const uint8_t data[128]) {
  uint64_t temp1, temp2;
  uint64_t A, B, C, D, E, F, G, H;
  uint64_t W[80];
  GET_FX_64WORD(W[0], data, 0);
  GET_FX_64WORD(W[1], data, 8);
  GET_FX_64WORD(W[2], data, 16);
  GET_FX_64WORD(W[3], data, 24);
  GET_FX_64WORD(W[4], data, 32);
  GET_FX_64WORD(W[5], data, 40);
  GET_FX_64WORD(W[6], data, 48);
  GET_FX_64WORD(W[7], data, 56);
  GET_FX_64WORD(W[8], data, 64);
  GET_FX_64WORD(W[9], data, 72);
  GET_FX_64WORD(W[10], data, 80);
  GET_FX_64WORD(W[11], data, 88);
  GET_FX_64WORD(W[12], data, 96);
  GET_FX_64WORD(W[13], data, 104);
  GET_FX_64WORD(W[14], data, 112);
  GET_FX_64WORD(W[15], data, 120);
  A = ctx->state[0];
  B = ctx->state[1];
  C = ctx->state[2];
  D = ctx->state[3];
  E = ctx->state[4];
  F = ctx->state[5];
  G = ctx->state[6];
  H = ctx->state[7];
  for (int i = 0; i < 10; ++i) {
    uint64_t temp[8];
    if (i < 2) {
      temp[0] = W[i * 8];
      temp[1] = W[i * 8 + 1];
      temp[2] = W[i * 8 + 2];
      temp[3] = W[i * 8 + 3];
      temp[4] = W[i * 8 + 4];
      temp[5] = W[i * 8 + 5];
      temp[6] = W[i * 8 + 6];
      temp[7] = W[i * 8 + 7];
    } else {
      temp[0] = SHA384_R(i * 8);
      temp[1] = SHA384_R(i * 8 + 1);
      temp[2] = SHA384_R(i * 8 + 2);
      temp[3] = SHA384_R(i * 8 + 3);
      temp[4] = SHA384_R(i * 8 + 4);
      temp[5] = SHA384_R(i * 8 + 5);
      temp[6] = SHA384_R(i * 8 + 6);
      temp[7] = SHA384_R(i * 8 + 7);
    }
    SHA384_P(A, B, C, D, E, F, G, H, temp[0], constants[i * 8]);
    SHA384_P(H, A, B, C, D, E, F, G, temp[1], constants[i * 8 + 1]);
    SHA384_P(G, H, A, B, C, D, E, F, temp[2], constants[i * 8 + 2]);
    SHA384_P(F, G, H, A, B, C, D, E, temp[3], constants[i * 8 + 3]);
    SHA384_P(E, F, G, H, A, B, C, D, temp[4], constants[i * 8 + 4]);
    SHA384_P(D, E, F, G, H, A, B, C, temp[5], constants[i * 8 + 5]);
    SHA384_P(C, D, E, F, G, H, A, B, temp[6], constants[i * 8 + 6]);
    SHA384_P(B, C, D, E, F, G, H, A, temp[7], constants[i * 8 + 7]);
  }
  ctx->state[0] += A;
  ctx->state[1] += B;
  ctx->state[2] += C;
  ctx->state[3] += D;
  ctx->state[4] += E;
  ctx->state[5] += F;
  ctx->state[6] += G;
  ctx->state[7] += H;
}

}  // namespace

void CRYPT_SHA1Start(CRYPT_sha1_context* s) {
  SHA_Core_Init(s->h);
  s->blkused = 0;
  s->lenhi = s->lenlo = 0;
}

void CRYPT_SHA1Update(CRYPT_sha1_context* s,
                      const uint8_t* data,
                      uint32_t size) {
  unsigned char* q = (unsigned char*)data;
  unsigned int wordblock[16];
  int len = size;
  unsigned int lenw = len;
  int i;
  s->lenlo += lenw;
  s->lenhi += (s->lenlo < lenw);
  if (s->blkused && s->blkused + len < 64) {
    memcpy(s->block + s->blkused, q, len);
    s->blkused += len;
  } else {
    while (s->blkused + len >= 64) {
      memcpy(s->block + s->blkused, q, 64 - s->blkused);
      q += 64 - s->blkused;
      len -= 64 - s->blkused;
      for (i = 0; i < 16; i++) {
        wordblock[i] = (((unsigned int)s->block[i * 4 + 0]) << 24) |
                       (((unsigned int)s->block[i * 4 + 1]) << 16) |
                       (((unsigned int)s->block[i * 4 + 2]) << 8) |
                       (((unsigned int)s->block[i * 4 + 3]) << 0);
      }
      SHATransform(s->h, wordblock);
      s->blkused = 0;
    }
    memcpy(s->block, q, len);
    s->blkused = len;
  }
}

void CRYPT_SHA1Finish(CRYPT_sha1_context* s, uint8_t digest[20]) {
  int i;
  int pad;
  unsigned char c[64];
  unsigned int lenhi, lenlo;
  if (s->blkused >= 56) {
    pad = 56 + 64 - s->blkused;
  } else {
    pad = 56 - s->blkused;
  }
  lenhi = (s->lenhi << 3) | (s->lenlo >> (32 - 3));
  lenlo = (s->lenlo << 3);
  memset(c, 0, pad);
  c[0] = 0x80;
  CRYPT_SHA1Update(s, c, pad);
  c[0] = (lenhi >> 24) & 0xFF;
  c[1] = (lenhi >> 16) & 0xFF;
  c[2] = (lenhi >> 8) & 0xFF;
  c[3] = (lenhi >> 0) & 0xFF;
  c[4] = (lenlo >> 24) & 0xFF;
  c[5] = (lenlo >> 16) & 0xFF;
  c[6] = (lenlo >> 8) & 0xFF;
  c[7] = (lenlo >> 0) & 0xFF;
  CRYPT_SHA1Update(s, c, 8);
  for (i = 0; i < 5; i++) {
    digest[i * 4] = (s->h[i] >> 24) & 0xFF;
    digest[i * 4 + 1] = (s->h[i] >> 16) & 0xFF;
    digest[i * 4 + 2] = (s->h[i] >> 8) & 0xFF;
    digest[i * 4 + 3] = (s->h[i]) & 0xFF;
  }
}
void CRYPT_SHA1Generate(const uint8_t* data,
                        uint32_t size,
                        uint8_t digest[20]) {
  CRYPT_sha1_context s;
  CRYPT_SHA1Start(&s);
  CRYPT_SHA1Update(&s, data, size);
  CRYPT_SHA1Finish(&s, digest);
}
void CRYPT_SHA256Start(CRYPT_sha2_context* ctx) {
  ctx->total[0] = 0;
  ctx->total[1] = 0;
  ctx->state[0] = 0x6A09E667;
  ctx->state[1] = 0xBB67AE85;
  ctx->state[2] = 0x3C6EF372;
  ctx->state[3] = 0xA54FF53A;
  ctx->state[4] = 0x510E527F;
  ctx->state[5] = 0x9B05688C;
  ctx->state[6] = 0x1F83D9AB;
  ctx->state[7] = 0x5BE0CD19;
}

void CRYPT_SHA256Update(CRYPT_sha2_context* ctx,
                        const uint8_t* input,
                        uint32_t length) {
  if (!length)
    return;

  uint32_t left = ctx->total[0] & 0x3F;
  uint32_t fill = 64 - left;
  ctx->total[0] += length;
  ctx->total[0] &= 0xFFFFFFFF;
  if (ctx->total[0] < length)
    ctx->total[1]++;

  if (left && length >= fill) {
    memcpy(ctx->buffer + left, input, fill);
    sha256_process(ctx, ctx->buffer);
    length -= fill;
    input += fill;
    left = 0;
  }
  while (length >= 64) {
    sha256_process(ctx, input);
    length -= 64;
    input += 64;
  }
  if (length)
    memcpy(ctx->buffer + left, input, length);
}

void CRYPT_SHA256Finish(CRYPT_sha2_context* ctx, uint8_t digest[32]) {
  uint8_t msglen[8];
  uint32_t high = (ctx->total[0] >> 29) | (ctx->total[1] << 3);
  uint32_t low = (ctx->total[0] << 3);
  SHA_PUT_UINT32(high, msglen, 0);
  SHA_PUT_UINT32(low, msglen, 4);
  uint32_t last = ctx->total[0] & 0x3F;
  uint32_t padn = (last < 56) ? (56 - last) : (120 - last);
  CRYPT_SHA256Update(ctx, sha256_padding, padn);
  CRYPT_SHA256Update(ctx, msglen, 8);
  SHA_PUT_UINT32(ctx->state[0], digest, 0);
  SHA_PUT_UINT32(ctx->state[1], digest, 4);
  SHA_PUT_UINT32(ctx->state[2], digest, 8);
  SHA_PUT_UINT32(ctx->state[3], digest, 12);
  SHA_PUT_UINT32(ctx->state[4], digest, 16);
  SHA_PUT_UINT32(ctx->state[5], digest, 20);
  SHA_PUT_UINT32(ctx->state[6], digest, 24);
  SHA_PUT_UINT32(ctx->state[7], digest, 28);
}

void CRYPT_SHA256Generate(const uint8_t* data,
                          uint32_t size,
                          uint8_t digest[32]) {
  CRYPT_sha2_context ctx;
  CRYPT_SHA256Start(&ctx);
  CRYPT_SHA256Update(&ctx, data, size);
  CRYPT_SHA256Finish(&ctx, digest);
}

void CRYPT_SHA384Start(CRYPT_sha2_context* ctx) {
  memset(ctx, 0, sizeof(CRYPT_sha2_context));
  ctx->state[0] = 0xcbbb9d5dc1059ed8ULL;
  ctx->state[1] = 0x629a292a367cd507ULL;
  ctx->state[2] = 0x9159015a3070dd17ULL;
  ctx->state[3] = 0x152fecd8f70e5939ULL;
  ctx->state[4] = 0x67332667ffc00b31ULL;
  ctx->state[5] = 0x8eb44a8768581511ULL;
  ctx->state[6] = 0xdb0c2e0d64f98fa7ULL;
  ctx->state[7] = 0x47b5481dbefa4fa4ULL;
}

void CRYPT_SHA384Update(CRYPT_sha2_context* ctx,
                        const uint8_t* input,
                        uint32_t length) {
  if (!length)
    return;

  uint32_t left = static_cast<uint32_t>(ctx->total[0]) & 0x7F;
  uint32_t fill = 128 - left;
  ctx->total[0] += length;
  if (ctx->total[0] < length)
    ctx->total[1]++;

  if (left && length >= fill) {
    memcpy(ctx->buffer + left, input, fill);
    sha384_process(ctx, ctx->buffer);
    length -= fill;
    input += fill;
    left = 0;
  }
  while (length >= 128) {
    sha384_process(ctx, input);
    length -= 128;
    input += 128;
  }
  if (length)
    memcpy(ctx->buffer + left, input, length);
}

void CRYPT_SHA384Finish(CRYPT_sha2_context* ctx, uint8_t digest[48]) {
  uint32_t last, padn;
  uint8_t msglen[16];
  memset(msglen, 0, 16);
  uint64_t high, low;
  high = (ctx->total[0] >> 29) | (ctx->total[1] << 3);
  low = (ctx->total[0] << 3);
  PUT_UINT64(high, msglen, 0);
  PUT_UINT64(low, msglen, 8);
  last = (uint32_t)ctx->total[0] & 0x7F;
  padn = (last < 112) ? (112 - last) : (240 - last);
  CRYPT_SHA384Update(ctx, sha384_padding, padn);
  CRYPT_SHA384Update(ctx, msglen, 16);
  PUT_UINT64(ctx->state[0], digest, 0);
  PUT_UINT64(ctx->state[1], digest, 8);
  PUT_UINT64(ctx->state[2], digest, 16);
  PUT_UINT64(ctx->state[3], digest, 24);
  PUT_UINT64(ctx->state[4], digest, 32);
  PUT_UINT64(ctx->state[5], digest, 40);
}

void CRYPT_SHA384Generate(const uint8_t* data,
                          uint32_t size,
                          uint8_t digest[64]) {
  CRYPT_sha2_context context;
  CRYPT_SHA384Start(&context);
  CRYPT_SHA384Update(&context, data, size);
  CRYPT_SHA384Finish(&context, digest);
}

void CRYPT_SHA512Start(CRYPT_sha2_context* ctx) {
  memset(ctx, 0, sizeof(CRYPT_sha2_context));
  ctx->state[0] = 0x6a09e667f3bcc908ULL;
  ctx->state[1] = 0xbb67ae8584caa73bULL;
  ctx->state[2] = 0x3c6ef372fe94f82bULL;
  ctx->state[3] = 0xa54ff53a5f1d36f1ULL;
  ctx->state[4] = 0x510e527fade682d1ULL;
  ctx->state[5] = 0x9b05688c2b3e6c1fULL;
  ctx->state[6] = 0x1f83d9abfb41bd6bULL;
  ctx->state[7] = 0x5be0cd19137e2179ULL;
}

void CRYPT_SHA512Update(CRYPT_sha2_context* ctx,
                        const uint8_t* data,
                        uint32_t size) {
  CRYPT_SHA384Update(ctx, data, size);
}

void CRYPT_SHA512Finish(CRYPT_sha2_context* ctx, uint8_t digest[64]) {
  uint32_t last, padn;
  uint8_t msglen[16];
  memset(msglen, 0, 16);
  uint64_t high, low;
  high = (ctx->total[0] >> 29) | (ctx->total[1] << 3);
  low = (ctx->total[0] << 3);
  PUT_UINT64(high, msglen, 0);
  PUT_UINT64(low, msglen, 8);
  last = (uint32_t)ctx->total[0] & 0x7F;
  padn = (last < 112) ? (112 - last) : (240 - last);
  CRYPT_SHA512Update(ctx, sha384_padding, padn);
  CRYPT_SHA512Update(ctx, msglen, 16);
  PUT_UINT64(ctx->state[0], digest, 0);
  PUT_UINT64(ctx->state[1], digest, 8);
  PUT_UINT64(ctx->state[2], digest, 16);
  PUT_UINT64(ctx->state[3], digest, 24);
  PUT_UINT64(ctx->state[4], digest, 32);
  PUT_UINT64(ctx->state[5], digest, 40);
  PUT_UINT64(ctx->state[6], digest, 48);
  PUT_UINT64(ctx->state[7], digest, 56);
}

void CRYPT_SHA512Generate(const uint8_t* data,
                          uint32_t size,
                          uint8_t digest[64]) {
  CRYPT_sha2_context context;
  CRYPT_SHA512Start(&context);
  CRYPT_SHA512Update(&context, data, size);
  CRYPT_SHA512Finish(&context, digest);
}
