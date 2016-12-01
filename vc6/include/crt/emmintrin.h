/**
*** Copyright (C) 1985-1999 Intel Corporation.  All rights reserved.
***
*** The information and source code contained herein is the exclusive
*** property of Intel Corporation and may not be disclosed, examined
*** or reproduced in whole or in part without explicit written authorization
*** from the company.
***
**/

/*
 * emmintrin.h
 *
 * Principal header file for Willamette New Instruction intrinsics
 *
 * The intrinsics package can be used in 2 ways, based whether or not
 * _EMM_FUNCTIONALITY is defined; if it is, the C implementation
 * will be used (the "functional intrinsics").
 */

#ifndef _INCLUDED_EMM
#define _INCLUDED_EMM

/*
 * the __m128 & __m64 types are required for the intrinsics
 */
#include <xmmintrin.h>

#ifdef __ICL
#ifdef _EMM_FUNCTIONALITY
#include "emm_func.h"
#else
typedef long long __m128d;
typedef long long __m128i;
#endif
#elif _MSC_VER >= 1300
typedef union __declspec(intrin_type) __declspec(align(16)) __m128i {
    __int8              m128i_i8[16];
    __int16             m128i_i16[8];
    __int32             m128i_i32[4];    
    __int64             m128i_i64[2];
    unsigned __int8     m128i_u8[16];
    unsigned __int16    m128i_u16[8];
    unsigned __int32    m128i_u32[4];
    unsigned __int64    m128i_u64[2];
} __m128i;

typedef struct __declspec(intrin_type) __declspec(align(16)) __m128d {
    double              m128d_f64[2];
} __m128d;
#endif

/*
 * Macro function for shuffle
 */
#define _MM_SHUFFLE2(x,y) (((x)<<1) | (y))

/*****************************************************/
/*     INTRINSICS FUNCTION PROTOTYPES START HERE     */
/*****************************************************/

#if defined __cplusplus
extern "C" { /* Begin "C" */
  /* Intrinsics use C name-mangling. */
#endif /* __cplusplus */

/*
 * DP, arithmetic
 */

extern __m128d _mm_add_sd(__m128d a, __m128d b);
extern __m128d _mm_add_pd(__m128d a, __m128d b);
extern __m128d _mm_sub_sd(__m128d a, __m128d b);
extern __m128d _mm_sub_pd(__m128d a, __m128d b);
extern __m128d _mm_mul_sd(__m128d a, __m128d b);
extern __m128d _mm_mul_pd(__m128d a, __m128d b);
extern __m128d _mm_sqrt_sd(__m128d a, __m128d b);
extern __m128d _mm_sqrt_pd(__m128d a);
extern __m128d _mm_div_sd(__m128d a, __m128d b);
extern __m128d _mm_div_pd(__m128d a, __m128d b);
extern __m128d _mm_min_sd(__m128d a, __m128d b);
extern __m128d _mm_min_pd(__m128d a, __m128d b);
extern __m128d _mm_max_sd(__m128d a, __m128d b);
extern __m128d _mm_max_pd(__m128d a, __m128d b);

/*
 * DP, logicals
 */

extern __m128d _mm_and_pd(__m128d a, __m128d b);
extern __m128d _mm_andnot_pd(__m128d a, __m128d b);
extern __m128d _mm_or_pd(__m128d a, __m128d b);
extern __m128d _mm_xor_pd(__m128d a, __m128d b);

/*
 * DP, comparisons
 */

extern __m128d _mm_cmpeq_sd(__m128d a, __m128d b);
extern __m128d _mm_cmpeq_pd(__m128d a, __m128d b);
extern __m128d _mm_cmplt_sd(__m128d a, __m128d b);
extern __m128d _mm_cmplt_pd(__m128d a, __m128d b);
extern __m128d _mm_cmple_sd(__m128d a, __m128d b);
extern __m128d _mm_cmple_pd(__m128d a, __m128d b);
extern __m128d _mm_cmpgt_sd(__m128d a, __m128d b);
extern __m128d _mm_cmpgt_pd(__m128d a, __m128d b);
extern __m128d _mm_cmpge_sd(__m128d a, __m128d b);
extern __m128d _mm_cmpge_pd(__m128d a, __m128d b);
extern __m128d _mm_cmpneq_sd(__m128d a, __m128d b);
extern __m128d _mm_cmpneq_pd(__m128d a, __m128d b);
extern __m128d _mm_cmpnlt_sd(__m128d a, __m128d b);
extern __m128d _mm_cmpnlt_pd(__m128d a, __m128d b);
extern __m128d _mm_cmpnle_sd(__m128d a, __m128d b);
extern __m128d _mm_cmpnle_pd(__m128d a, __m128d b);
extern __m128d _mm_cmpngt_sd(__m128d a, __m128d b);
extern __m128d _mm_cmpngt_pd(__m128d a, __m128d b);
extern __m128d _mm_cmpnge_sd(__m128d a, __m128d b);
extern __m128d _mm_cmpnge_pd(__m128d a, __m128d b);
extern __m128d _mm_cmpord_pd(__m128d a, __m128d b);
extern __m128d _mm_cmpord_sd(__m128d a, __m128d b);
extern __m128d _mm_cmpunord_pd(__m128d a, __m128d b);
extern __m128d _mm_cmpunord_sd(__m128d a, __m128d b);
extern int _mm_comieq_sd(__m128d a, __m128d b);
extern int _mm_comilt_sd(__m128d a, __m128d b);
extern int _mm_comile_sd(__m128d a, __m128d b);
extern int _mm_comigt_sd(__m128d a, __m128d b);
extern int _mm_comige_sd(__m128d a, __m128d b);
extern int _mm_comineq_sd(__m128d a, __m128d b);
extern int _mm_ucomieq_sd(__m128d a, __m128d b);
extern int _mm_ucomilt_sd(__m128d a, __m128d b);
extern int _mm_ucomile_sd(__m128d a, __m128d b);
extern int _mm_ucomigt_sd(__m128d a, __m128d b);
extern int _mm_ucomige_sd(__m128d a, __m128d b);
extern int _mm_ucomineq_sd(__m128d a, __m128d b);

/*
 * DP, converts
 */

extern __m128d _mm_cvtepi32_pd(__m128i a);
extern __m128i _mm_cvtpd_epi32(__m128d a);
extern __m128i _mm_cvttpd_epi32(__m128d a);
extern __m128 _mm_cvtepi32_ps(__m128i a);
extern __m128i _mm_cvtps_epi32(__m128 a);
extern __m128i _mm_cvttps_epi32(__m128 a);
extern __m128 _mm_cvtpd_ps(__m128d a);
extern __m128d _mm_cvtps_pd(__m128 a);
extern __m128 _mm_cvtsd_ss(__m128 a, __m128d b);
extern __m128d _mm_cvtss_sd(__m128d a, __m128 b);

extern int _mm_cvtsd_si32(__m128d a);
extern int _mm_cvttsd_si32(__m128d a);
extern __m128d _mm_cvtsi32_sd(__m128d a, int b);

extern __m64 _mm_cvtpd_pi32(__m128d a);
extern __m64 _mm_cvttpd_pi32(__m128d a);
extern __m128d _mm_cvtpi32_pd(__m64 a);

/*
 * DP, misc
 */

extern __m128d _mm_unpackhi_pd(__m128d a, __m128d b);
extern __m128d _mm_unpacklo_pd(__m128d a, __m128d b);
extern int _mm_movemask_pd(__m128d a);
extern __m128d _mm_shuffle_pd(__m128d a, __m128d b, int i);

/*
 * DP, loads
 */

extern __m128d _mm_load_pd(double const*dp);
extern __m128d _mm_load1_pd(double const*dp);
extern __m128d _mm_loadr_pd(double const*dp);
extern __m128d _mm_loadu_pd(double const*dp);
extern __m128d _mm_load_sd(double const*dp);
extern __m128d _mm_loadh_pd(__m128d a, double const*dp);
extern __m128d _mm_loadl_pd(__m128d a, double const*dp);

/*
 * DP, sets
 */

extern __m128d _mm_set_sd(double w);
extern __m128d _mm_set1_pd(double a);
extern __m128d _mm_set_pd(double z, double y);
extern __m128d _mm_setr_pd(double y, double z);
extern __m128d _mm_setzero_pd(void);
extern __m128d _mm_move_sd(__m128d a, __m128d b);

/* 
 * DP, stores
 */

extern void _mm_store_sd(double *dp, __m128d a);
extern void _mm_store1_pd(double *dp, __m128d a);
extern void _mm_store_pd(double *dp, __m128d a);
extern void _mm_storeu_pd(double *dp, __m128d a);
extern void _mm_storer_pd(double *dp, __m128d a);
extern void _mm_storeh_pd(double *dp, __m128d a);
extern void _mm_storel_pd(double *dp, __m128d a);

/*
 * Integer, arithmetic
 */

extern __m128i _mm_add_epi8(__m128i a, __m128i b);
extern __m128i _mm_add_epi16(__m128i a, __m128i b);
extern __m128i _mm_add_epi32(__m128i a, __m128i b);
extern __m64 _mm_add_si64(__m64 a, __m64 b);
extern __m128i _mm_add_epi64(__m128i a, __m128i b);
extern __m128i _mm_adds_epi8(__m128i a, __m128i b);
extern __m128i _mm_adds_epi16(__m128i a, __m128i b);
extern __m128i _mm_adds_epu8(__m128i a, __m128i b);
extern __m128i _mm_adds_epu16(__m128i a, __m128i b);
extern __m128i _mm_avg_epu8(__m128i a, __m128i b);
extern __m128i _mm_avg_epu16(__m128i a, __m128i b);
extern __m128i _mm_madd_epi16(__m128i a, __m128i b);
extern __m128i _mm_max_epi16(__m128i a, __m128i b);
extern __m128i _mm_max_epu8(__m128i a, __m128i b);
extern __m128i _mm_min_epi16(__m128i a, __m128i b);
extern __m128i _mm_min_epu8(__m128i a, __m128i b);
extern __m128i _mm_mulhi_epi16(__m128i a, __m128i b);
extern __m128i _mm_mulhi_epu16(__m128i a, __m128i b);
extern __m128i _mm_mullo_epi16(__m128i a, __m128i b);
extern __m64 _mm_mul_su32(__m64 a, __m64 b);
extern __m128i _mm_mul_epu32(__m128i a, __m128i b);
extern __m128i _mm_sad_epu8(__m128i a, __m128i b);
extern __m128i _mm_sub_epi8(__m128i a, __m128i b);
extern __m128i _mm_sub_epi16(__m128i a, __m128i b);
extern __m128i _mm_sub_epi32(__m128i a, __m128i b);
extern __m64 _mm_sub_si64(__m64 a, __m64 b);
extern __m128i _mm_sub_epi64(__m128i a, __m128i b);
extern __m128i _mm_subs_epi8(__m128i a, __m128i b);
extern __m128i _mm_subs_epi16(__m128i a, __m128i b);
extern __m128i _mm_subs_epu8(__m128i a, __m128i b);
extern __m128i _mm_subs_epu16(__m128i a, __m128i b);

/*
 * Integer, logicals
 */

extern __m128i _mm_and_si128(__m128i a, __m128i b);
extern __m128i _mm_andnot_si128(__m128i a, __m128i b);
extern __m128i _mm_or_si128(__m128i a, __m128i b);
extern __m128i _mm_xor_si128(__m128i a, __m128i b);

/*
 * Integer, shifts
 */

extern __m128i _mm_slli_si128(__m128i a, int imm);
extern __m128i _mm_slli_epi16(__m128i a, int count);
extern __m128i _mm_sll_epi16(__m128i a, __m128i count);
extern __m128i _mm_slli_epi32(__m128i a, int count);
extern __m128i _mm_sll_epi32(__m128i a, __m128i count);
extern __m128i _mm_slli_epi64(__m128i a, int count);
extern __m128i _mm_sll_epi64(__m128i a, __m128i count);
extern __m128i _mm_srai_epi16(__m128i a, int count);
extern __m128i _mm_sra_epi16(__m128i a, __m128i count);
extern __m128i _mm_srai_epi32(__m128i a, int count);
extern __m128i _mm_sra_epi32(__m128i a, __m128i count);
extern __m128i _mm_srli_si128(__m128i a, int imm);
extern __m128i _mm_srli_epi16(__m128i a, int count);
extern __m128i _mm_srl_epi16(__m128i a, __m128i count);
extern __m128i _mm_srli_epi32(__m128i a, int count);
extern __m128i _mm_srl_epi32(__m128i a, __m128i count);
extern __m128i _mm_srli_epi64(__m128i a, int count);
extern __m128i _mm_srl_epi64(__m128i a, __m128i count);

/*
 * Integer, comparisons
 */

extern __m128i _mm_cmpeq_epi8(__m128i a, __m128i b);
extern __m128i _mm_cmpeq_epi16(__m128i a, __m128i b);
extern __m128i _mm_cmpeq_epi32(__m128i a, __m128i b);
extern __m128i _mm_cmpgt_epi8(__m128i a, __m128i b);
extern __m128i _mm_cmpgt_epi16(__m128i a, __m128i b);
extern __m128i _mm_cmpgt_epi32(__m128i a, __m128i b);
extern __m128i _mm_cmplt_epi8(__m128i a, __m128i b);
extern __m128i _mm_cmplt_epi16(__m128i a, __m128i b);
extern __m128i _mm_cmplt_epi32(__m128i a, __m128i b);

/*
 * Integer, converts
 */

extern __m128i _mm_cvtsi32_si128(int a);
extern int _mm_cvtsi128_si32(__m128i a);

/*
 * Integer, misc
 */

extern __m128i _mm_packs_epi16(__m128i a, __m128i b);
extern __m128i _mm_packs_epi32(__m128i a, __m128i b);
extern __m128i _mm_packus_epi16(__m128i a, __m128i b);
extern int _mm_extract_epi16(__m128i a, int imm);
extern __m128i _mm_insert_epi16(__m128i a, int b, int imm);
extern int _mm_movemask_epi8(__m128i a);
extern __m128i _mm_shuffle_epi32(__m128i a, int imm);
extern __m128i _mm_shufflehi_epi16(__m128i a, int imm);
extern __m128i _mm_shufflelo_epi16(__m128i a, int imm);
extern __m128i _mm_unpackhi_epi8(__m128i a, __m128i b);
extern __m128i _mm_unpackhi_epi16(__m128i a, __m128i b);
extern __m128i _mm_unpackhi_epi32(__m128i a, __m128i b);
extern __m128i _mm_unpackhi_epi64(__m128i a, __m128i b);
extern __m128i _mm_unpacklo_epi8(__m128i a, __m128i b);
extern __m128i _mm_unpacklo_epi16(__m128i a, __m128i b);
extern __m128i _mm_unpacklo_epi32(__m128i a, __m128i b);
extern __m128i _mm_unpacklo_epi64(__m128i a, __m128i b);

/*
 * Integer, loads
 */

extern __m128i _mm_load_si128(__m128i const*p);
extern __m128i _mm_loadu_si128(__m128i const*p);
extern __m128i _mm_loadl_epi64(__m128i const*p);

/*
 * Integer, sets
 */

extern __m128i _mm_set_epi64(__m64 q1, __m64 q0);
extern __m128i _mm_set_epi32(int i3, int i2, int i1, int i0);
extern __m128i _mm_set_epi16(short w7, short w6, short w5, short w4,
                             short w3, short w2, short w1, short w0);
extern __m128i _mm_set_epi8(char b15, char b14, char b13, char b12, 
                            char b11, char b10, char b9, char b8, 
                            char b7, char b6, char b5, char b4, 
                            char b3, char b2, char b1, char b0);
extern __m128i _mm_set1_epi64(__m64 q);
extern __m128i _mm_set1_epi32(int i);
extern __m128i _mm_set1_epi16(short w);
extern __m128i _mm_set1_epi8(char b);
extern __m128i _mm_setl_epi64(__m128i q);
extern __m128i _mm_setr_epi64(__m64 q0, __m64 q1);
extern __m128i _mm_setr_epi32(int i0, int i1, int i2, int i3);
extern __m128i _mm_setr_epi16(short w0, short w1, short w2, short w3, 
                              short w4, short w5, short w6, short w7);
extern __m128i _mm_setr_epi8(char b15, char b14, char b13, char b12, 
                             char b11, char b10, char b9, char b8, 
                             char b7, char b6, char b5, char b4, 
                             char b3, char b2, char b1, char b0);
extern __m128i _mm_setzero_si128();

/*
 * Integer, stores
 */

extern void _mm_store_si128(__m128i *p, __m128i b);
extern void _mm_storeu_si128(__m128i *p, __m128i b);
extern void _mm_storel_epi64(__m128i *p, __m128i q);
extern void _mm_maskmoveu_si128(__m128i d, __m128i n, char *p);

/*
 * Integer, moves
 */

extern __m128i _mm_move_epi64(__m128i q);
extern __m128i _mm_movpi64_epi64(__m64 q);
extern __m64 _mm_movepi64_pi64(__m128i q);

/*
 * Cacheability support
 */

extern void _mm_stream_pd(double *dp, __m128d a);
extern void _mm_stream_si128(__m128i *p, __m128i a);
extern void _mm_clflush(void const*p);
extern void _mm_lfence(void);
extern void _mm_mfence(void);
extern void _mm_stream_si32(int *p, int i);
extern void _mm_pause(void);

#if defined __cplusplus
}; /* End "C" */
#endif /* __cplusplus */

#endif /* _INCLUDED_EMM */

