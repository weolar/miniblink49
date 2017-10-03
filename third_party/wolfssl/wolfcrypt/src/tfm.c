/* tfm.c
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */



/*
 * Based on public domain TomsFastMath 0.10 by Tom St Denis, tomstdenis@iahu.ca,
 * http://math.libtomcrypt.com
 */

/**
 *  Edited by Moises Guimaraes (moisesguimaraesm@gmail.com)
 *  to fit CyaSSL's needs.
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

/* in case user set USE_FAST_MATH there */
#include <wolfssl/wolfcrypt/settings.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifdef USE_FAST_MATH

#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/tfm.h>
#include <wolfcrypt/src/asm.c>  /* will define asm MACROS or C ones */
#include <wolfssl/wolfcrypt/wolfmath.h> /* common functions */

#if defined(FREESCALE_LTC_TFM)
    #include <wolfssl/wolfcrypt/port/nxp/ksdk_port.h>
#endif
#ifdef WOLFSSL_DEBUG_MATH
    #include <stdio.h>
#endif


/* math settings check */
word32 CheckRunTimeSettings(void)
{
    return CTC_SETTINGS;
}


/* math settings size check */
word32 CheckRunTimeFastMath(void)
{
    return FP_SIZE;
}


/* Functions */

void fp_add(fp_int *a, fp_int *b, fp_int *c)
{
  int     sa, sb;

  /* get sign of both inputs */
  sa = a->sign;
  sb = b->sign;

  /* handle two cases, not four */
  if (sa == sb) {
    /* both positive or both negative */
    /* add their magnitudes, copy the sign */
    c->sign = sa;
    s_fp_add (a, b, c);
  } else {
    /* one positive, the other negative */
    /* subtract the one with the greater magnitude from */
    /* the one of the lesser magnitude.  The result gets */
    /* the sign of the one with the greater magnitude. */
    if (fp_cmp_mag (a, b) == FP_LT) {
      c->sign = sb;
      s_fp_sub (b, a, c);
    } else {
      c->sign = sa;
      s_fp_sub (a, b, c);
    }
  }
}

/* unsigned addition */
void s_fp_add(fp_int *a, fp_int *b, fp_int *c)
{
  int      x, y, oldused;
  fp_word  t;

  y       = MAX(a->used, b->used);
  oldused = MIN(c->used, FP_SIZE);   /* help static analysis w/ largest size */
  c->used = y;

  t = 0;
  for (x = 0; x < y; x++) {
      t         += ((fp_word)a->dp[x]) + ((fp_word)b->dp[x]);
      c->dp[x]   = (fp_digit)t;
      t        >>= DIGIT_BIT;
  }
  if (t != 0 && x < FP_SIZE) {
     c->dp[c->used++] = (fp_digit)t;
     ++x;
  }

  c->used = x;

  /* zero any excess digits on the destination that we didn't write to */
  for (; x < oldused; x++) {
     c->dp[x] = 0;
  }
  fp_clamp(c);
}

/* c = a - b */
void fp_sub(fp_int *a, fp_int *b, fp_int *c)
{
  int     sa, sb;

  sa = a->sign;
  sb = b->sign;

  if (sa != sb) {
    /* subtract a negative from a positive, OR */
    /* subtract a positive from a negative. */
    /* In either case, ADD their magnitudes, */
    /* and use the sign of the first number. */
    c->sign = sa;
    s_fp_add (a, b, c);
  } else {
    /* subtract a positive from a positive, OR */
    /* subtract a negative from a negative. */
    /* First, take the difference between their */
    /* magnitudes, then... */
    if (fp_cmp_mag (a, b) != FP_LT) {
      /* Copy the sign from the first */
      c->sign = sa;
      /* The first has a larger or equal magnitude */
      s_fp_sub (a, b, c);
    } else {
      /* The result has the *opposite* sign from */
      /* the first number. */
      c->sign = (sa == FP_ZPOS) ? FP_NEG : FP_ZPOS;
      /* The second has a larger magnitude */
      s_fp_sub (b, a, c);
    }
  }
}

/* unsigned subtraction ||a|| >= ||b|| ALWAYS! */
void s_fp_sub(fp_int *a, fp_int *b, fp_int *c)
{
  int      x, oldbused, oldused;
  fp_word  t;

  oldused  = c->used;
  oldbused = b->used;
  c->used  = a->used;
  t       = 0;
  for (x = 0; x < oldbused; x++) {
     t         = ((fp_word)a->dp[x]) - (((fp_word)b->dp[x]) + t);
     c->dp[x]  = (fp_digit)t;
     t         = (t >> DIGIT_BIT)&1;
  }
  for (; x < a->used; x++) {
     t         = ((fp_word)a->dp[x]) - t;
     c->dp[x]  = (fp_digit)t;
     t         = (t >> DIGIT_BIT)&1;
   }

  /* zero any excess digits on the destination that we didn't write to */
  for (; x < oldused; x++) {
     c->dp[x] = 0;
  }
  fp_clamp(c);
}

/* c = a * b */
void fp_mul(fp_int *A, fp_int *B, fp_int *C)
{
    int   y, yy, oldused;

    oldused = C->used;

    y  = MAX(A->used, B->used);
    yy = MIN(A->used, B->used);

    /* call generic if we're out of range */
    if (y + yy > FP_SIZE) {
       fp_mul_comba(A, B, C);
       goto clean;
    }

    /* pick a comba (unrolled 4/8/16/32 x or rolled) based on the size
       of the largest input.  We also want to avoid doing excess mults if the
       inputs are not close to the next power of two.  That is, for example,
       if say y=17 then we would do (32-17)^2 = 225 unneeded multiplications
    */

#if defined(TFM_MUL3) && FP_SIZE >= 6
        if (y <= 3) {
           fp_mul_comba3(A,B,C);
           goto clean;
        }
#endif
#if defined(TFM_MUL4) && FP_SIZE >= 8
        if (y == 4) {
           fp_mul_comba4(A,B,C);
           goto clean;
        }
#endif
#if defined(TFM_MUL6) && FP_SIZE >= 12
        if (y <= 6) {
           fp_mul_comba6(A,B,C);
           goto clean;
        }
#endif
#if defined(TFM_MUL7) && FP_SIZE >= 14
        if (y == 7) {
           fp_mul_comba7(A,B,C);
           goto clean;
        }
#endif
#if defined(TFM_MUL8) && FP_SIZE >= 16
        if (y == 8) {
           fp_mul_comba8(A,B,C);
           goto clean;
        }
#endif
#if defined(TFM_MUL9) && FP_SIZE >= 18
        if (y == 9) {
           fp_mul_comba9(A,B,C);
           goto clean;
        }
#endif
#if defined(TFM_MUL12) && FP_SIZE >= 24
        if (y <= 12) {
           fp_mul_comba12(A,B,C);
           goto clean;
        }
#endif
#if defined(TFM_MUL17) && FP_SIZE >= 34
        if (y <= 17) {
           fp_mul_comba17(A,B,C);
           goto clean;
        }
#endif

#if defined(TFM_SMALL_SET) && FP_SIZE >= 32
        if (y <= 16) {
           fp_mul_comba_small(A,B,C);
           goto clean;
        }
#endif
#if defined(TFM_MUL20) && FP_SIZE >= 40
        if (y <= 20) {
           fp_mul_comba20(A,B,C);
           goto clean;
        }
#endif
#if defined(TFM_MUL24) && FP_SIZE >= 48
        if (yy >= 16 && y <= 24) {
           fp_mul_comba24(A,B,C);
           goto clean;
        }
#endif
#if defined(TFM_MUL28) && FP_SIZE >= 56
        if (yy >= 20 && y <= 28) {
           fp_mul_comba28(A,B,C);
           goto clean;
        }
#endif
#if defined(TFM_MUL32) && FP_SIZE >= 64
        if (yy >= 24 && y <= 32) {
           fp_mul_comba32(A,B,C);
           goto clean;
        }
#endif
#if defined(TFM_MUL48) && FP_SIZE >= 96
        if (yy >= 40 && y <= 48) {
          fp_mul_comba48(A,B,C);
          goto clean;
        }
#endif
#if defined(TFM_MUL64) && FP_SIZE >= 128
        if (yy >= 56 && y <= 64) {
           fp_mul_comba64(A,B,C);
           goto clean;
        }
#endif
        fp_mul_comba(A,B,C);

clean:
    /* zero any excess digits on the destination that we didn't write to */
    for (y = C->used; y >= 0 && y < oldused; y++) {
        C->dp[y] = 0;
    }
}

void fp_mul_2(fp_int * a, fp_int * b)
{
  int     x, oldused;

  oldused = b->used;
  b->used = a->used;

  {
    fp_digit r, rr, *tmpa, *tmpb;

    /* alias for source */
    tmpa = a->dp;

    /* alias for dest */
    tmpb = b->dp;

    /* carry */
    r = 0;
    for (x = 0; x < a->used; x++) {

      /* get what will be the *next* carry bit from the
       * MSB of the current digit
       */
      rr = *tmpa >> ((fp_digit)(DIGIT_BIT - 1));

      /* now shift up this digit, add in the carry [from the previous] */
      *tmpb++ = ((*tmpa++ << ((fp_digit)1)) | r);

      /* copy the carry that would be from the source
       * digit into the next iteration
       */
      r = rr;
    }

    /* new leading digit? */
    if (r != 0 && b->used != (FP_SIZE-1)) {
      /* add a MSB which is always 1 at this point */
      *tmpb = 1;
      ++(b->used);
    }

    /* zero any excess digits on the destination that we didn't write to */
    tmpb = b->dp + b->used;
    for (x = b->used; x < oldused; x++) {
      *tmpb++ = 0;
    }
  }
  b->sign = a->sign;
}

/* c = a * b */
void fp_mul_d(fp_int *a, fp_digit b, fp_int *c)
{
   fp_word  w;
   int      x, oldused;

   oldused = c->used;
   c->used = a->used;
   c->sign = a->sign;
   w       = 0;
   for (x = 0; x < a->used; x++) {
       w         = ((fp_word)a->dp[x]) * ((fp_word)b) + w;
       c->dp[x]  = (fp_digit)w;
       w         = w >> DIGIT_BIT;
   }
   if (w != 0 && (a->used != FP_SIZE)) {
      c->dp[c->used++] = (fp_digit) w;
      ++x;
   }

   /* zero any excess digits on the destination that we didn't write to */
   for (; x < oldused; x++) {
      c->dp[x] = 0;
   }
   fp_clamp(c);
}

/* c = a * 2**d */
void fp_mul_2d(fp_int *a, int b, fp_int *c)
{
   fp_digit carry, carrytmp, shift;
   int x;

   /* copy it */
   fp_copy(a, c);

   /* handle whole digits */
   if (b >= DIGIT_BIT) {
      fp_lshd(c, b/DIGIT_BIT);
   }
   b %= DIGIT_BIT;

   /* shift the digits */
   if (b != 0) {
      carry = 0;
      shift = DIGIT_BIT - b;
      for (x = 0; x < c->used; x++) {
          carrytmp = c->dp[x] >> shift;
          c->dp[x] = (c->dp[x] << b) + carry;
          carry = carrytmp;
      }
      /* store last carry if room */
      if (carry && x < FP_SIZE) {
         c->dp[c->used++] = carry;
      }
   }
   fp_clamp(c);
}

/* generic PxQ multiplier */
#if defined(HAVE_INTEL_MULX)

INLINE static void fp_mul_comba_mulx(fp_int *A, fp_int *B, fp_int *C)

{
   int       ix, iy, iz, pa;
   fp_int    tmp, *dst;

   /* get size of output and trim */
   pa = A->used + B->used;
   if (pa >= FP_SIZE) {
      pa = FP_SIZE-1;
   }

   /* Always take branch to use tmp variable. This avoids a cache attack for
    * determining if C equals A */
   if (1) {
      fp_init(&tmp);
      dst = &tmp;
   }

   TFM_INTEL_MUL_COMBA(A, B, dst) ;

  dst->used = pa;
  dst->sign = A->sign ^ B->sign;
  fp_clamp(dst);
  fp_copy(dst, C);
}
#endif

void fp_mul_comba(fp_int *A, fp_int *B, fp_int *C)
{
   int       ix, iy, iz, tx, ty, pa;
   fp_digit  c0, c1, c2, *tmpx, *tmpy;
   fp_int    tmp, *dst;

   IF_HAVE_INTEL_MULX(fp_mul_comba_mulx(A, B, C), return) ;

   COMBA_START;
   COMBA_CLEAR;

   /* get size of output and trim */
   pa = A->used + B->used;
   if (pa >= FP_SIZE) {
      pa = FP_SIZE-1;
   }

   /* Always take branch to use tmp variable. This avoids a cache attack for
    * determining if C equals A */
   if (1) {
      fp_init(&tmp);
      dst = &tmp;
   }

   for (ix = 0; ix < pa; ix++) {
      /* get offsets into the two bignums */
      ty = MIN(ix, B->used-1);
      tx = ix - ty;

      /* setup temp aliases */
      tmpx = A->dp + tx;
      tmpy = B->dp + ty;

      /* this is the number of times the loop will iterate, essentially its
         while (tx++ < a->used && ty-- >= 0) { ... }
       */
      iy = MIN(A->used-tx, ty+1);

      /* execute loop */
      COMBA_FORWARD;
      for (iz = 0; iz < iy; ++iz) {
          fp_digit _tmpx = *tmpx++;
          fp_digit _tmpy = *tmpy--;
          MULADD(_tmpx, _tmpy);
      }

      /* store term */
      COMBA_STORE(dst->dp[ix]);
  }
  COMBA_FINI;

  dst->used = pa;
  dst->sign = A->sign ^ B->sign;
  fp_clamp(dst);
  fp_copy(dst, C);
}

/* a/b => cb + d == a */
int fp_div(fp_int *a, fp_int *b, fp_int *c, fp_int *d)
{
  fp_int  q, x, y, t1, t2;
  int     n, t, i, norm, neg;

  /* is divisor zero ? */
  if (fp_iszero (b) == FP_YES) {
    return FP_VAL;
  }

  /* if a < b then q=0, r = a */
  if (fp_cmp_mag (a, b) == FP_LT) {
    if (d != NULL) {
      fp_copy (a, d);
    }
    if (c != NULL) {
      fp_zero (c);
    }
    return FP_OKAY;
  }

  fp_init(&q);
  q.used = a->used + 2;

  fp_init(&t1);
  fp_init(&t2);
  fp_init_copy(&x, a);
  fp_init_copy(&y, b);

  /* fix the sign */
  neg = (a->sign == b->sign) ? FP_ZPOS : FP_NEG;
  x.sign = y.sign = FP_ZPOS;

  /* normalize both x and y, ensure that y >= b/2, [b == 2**DIGIT_BIT] */
  norm = fp_count_bits(&y) % DIGIT_BIT;
  if (norm < (int)(DIGIT_BIT-1)) {
     norm = (DIGIT_BIT-1) - norm;
     fp_mul_2d (&x, norm, &x);
     fp_mul_2d (&y, norm, &y);
  } else {
     norm = 0;
  }

  /* note hac does 0 based, so if used==5 then its 0,1,2,3,4, e.g. use 4 */
  n = x.used - 1;
  t = y.used - 1;

  /* while (x >= y*b**n-t) do { q[n-t] += 1; x -= y*b**{n-t} } */
  fp_lshd (&y, n - t); /* y = y*b**{n-t} */

  while (fp_cmp (&x, &y) != FP_LT) {
    ++(q.dp[n - t]);
    fp_sub (&x, &y, &x);
  }

  /* reset y by shifting it back down */
  fp_rshd (&y, n - t);

  /* step 3. for i from n down to (t + 1) */
  for (i = n; i >= (t + 1); i--) {
    if (i > x.used) {
      continue;
    }

    /* step 3.1 if xi == yt then set q{i-t-1} to b-1,
     * otherwise set q{i-t-1} to (xi*b + x{i-1})/yt */
    if (x.dp[i] == y.dp[t]) {
      q.dp[i - t - 1] = (fp_digit) ((((fp_word)1) << DIGIT_BIT) - 1);
    } else {
      fp_word tmp;
      tmp = ((fp_word) x.dp[i]) << ((fp_word) DIGIT_BIT);
      tmp |= ((fp_word) x.dp[i - 1]);
      tmp /= ((fp_word)y.dp[t]);
      q.dp[i - t - 1] = (fp_digit) (tmp);
    }

    /* while (q{i-t-1} * (yt * b + y{t-1})) >
             xi * b**2 + xi-1 * b + xi-2

       do q{i-t-1} -= 1;
    */
    q.dp[i - t - 1] = (q.dp[i - t - 1] + 1);
    do {
      q.dp[i - t - 1] = (q.dp[i - t - 1] - 1);

      /* find left hand */
      fp_zero (&t1);
      t1.dp[0] = (t - 1 < 0) ? 0 : y.dp[t - 1];
      t1.dp[1] = y.dp[t];
      t1.used = 2;
      fp_mul_d (&t1, q.dp[i - t - 1], &t1);

      /* find right hand */
      t2.dp[0] = (i - 2 < 0) ? 0 : x.dp[i - 2];
      t2.dp[1] = (i - 1 < 0) ? 0 : x.dp[i - 1];
      t2.dp[2] = x.dp[i];
      t2.used = 3;
    } while (fp_cmp_mag(&t1, &t2) == FP_GT);

    /* step 3.3 x = x - q{i-t-1} * y * b**{i-t-1} */
    fp_mul_d (&y, q.dp[i - t - 1], &t1);
    fp_lshd  (&t1, i - t - 1);
    fp_sub   (&x, &t1, &x);

    /* if x < 0 then { x = x + y*b**{i-t-1}; q{i-t-1} -= 1; } */
    if (x.sign == FP_NEG) {
      fp_copy (&y, &t1);
      fp_lshd (&t1, i - t - 1);
      fp_add (&x, &t1, &x);
      q.dp[i - t - 1] = q.dp[i - t - 1] - 1;
    }
  }

  /* now q is the quotient and x is the remainder
   * [which we have to normalize]
   */

  /* get sign before writing to c */
  x.sign = x.used == 0 ? FP_ZPOS : a->sign;

  if (c != NULL) {
    fp_clamp (&q);
    fp_copy (&q, c);
    c->sign = neg;
  }

  if (d != NULL) {
    fp_div_2d (&x, norm, &x, NULL);

    /* zero any excess digits on the destination that we didn't write to */
    for (i = b->used; i < x.used; i++) {
        x.dp[i] = 0;
    }
    fp_clamp(&x);
    fp_copy (&x, d);
  }

  return FP_OKAY;
}

/* b = a/2 */
void fp_div_2(fp_int * a, fp_int * b)
{
  int     x, oldused;

  oldused = b->used;
  b->used = a->used;
  {
    fp_digit r, rr, *tmpa, *tmpb;

    /* source alias */
    tmpa = a->dp + b->used - 1;

    /* dest alias */
    tmpb = b->dp + b->used - 1;

    /* carry */
    r = 0;
    for (x = b->used - 1; x >= 0; x--) {
      /* get the carry for the next iteration */
      rr = *tmpa & 1;

      /* shift the current digit, add in carry and store */
      *tmpb-- = (*tmpa-- >> 1) | (r << (DIGIT_BIT - 1));

      /* forward carry to next iteration */
      r = rr;
    }

    /* zero any excess digits on the destination that we didn't write to */
    tmpb = b->dp + b->used;
    for (x = b->used; x < oldused; x++) {
      *tmpb++ = 0;
    }
  }
  b->sign = a->sign;
  fp_clamp (b);
}

/* c = a / 2**b */
void fp_div_2d(fp_int *a, int b, fp_int *c, fp_int *d)
{
  int      D;
  fp_int   t;

  /* if the shift count is <= 0 then we do no work */
  if (b <= 0) {
    fp_copy (a, c);
    if (d != NULL) {
      fp_zero (d);
    }
    return;
  }

  fp_init(&t);

  /* get the remainder */
  if (d != NULL) {
    fp_mod_2d (a, b, &t);
  }

  /* copy */
  fp_copy(a, c);

  /* shift by as many digits in the bit count */
  if (b >= (int)DIGIT_BIT) {
    fp_rshd (c, b / DIGIT_BIT);
  }

  /* shift any bit count < DIGIT_BIT */
  D = (b % DIGIT_BIT);
  if (D != 0) {
    fp_rshb(c, D);
  }
  fp_clamp (c);
  if (d != NULL) {
    fp_copy (&t, d);
  }
}

/* c = a mod b, 0 <= c < b  */
int fp_mod(fp_int *a, fp_int *b, fp_int *c)
{
   fp_int t;
   int    err;

   fp_init(&t);
   if ((err = fp_div(a, b, NULL, &t)) != FP_OKAY) {
      return err;
   }
   if (t.sign != b->sign) {
      fp_add(&t, b, c);
   } else {
      fp_copy(&t, c);
  }
  return FP_OKAY;
}

/* c = a mod 2**d */
void fp_mod_2d(fp_int *a, int b, fp_int *c)
{
   int x;

   /* zero if count less than or equal to zero */
   if (b <= 0) {
      fp_zero(c);
      return;
   }

   /* get copy of input */
   fp_copy(a, c);

   /* if 2**d is larger than we just return */
   if (b >= (DIGIT_BIT * a->used)) {
      return;
   }

  /* zero digits above the last digit of the modulus */
  for (x = (b / DIGIT_BIT) + ((b % DIGIT_BIT) == 0 ? 0 : 1); x < c->used; x++) {
    c->dp[x] = 0;
  }
  /* clear the digit that is not completely outside/inside the modulus */
  c->dp[b / DIGIT_BIT] &= ~((fp_digit)0) >> (DIGIT_BIT - b);
  fp_clamp (c);
}

static int fp_invmod_slow (fp_int * a, fp_int * b, fp_int * c)
{
  fp_int  x, y, u, v, A, B, C, D;
  int     res;

  /* b cannot be negative */
  if (b->sign == FP_NEG || fp_iszero(b) == FP_YES) {
    return FP_VAL;
  }

  /* init temps */
  fp_init(&x);    fp_init(&y);
  fp_init(&u);    fp_init(&v);
  fp_init(&A);    fp_init(&B);
  fp_init(&C);    fp_init(&D);

  /* x = a, y = b */
  if ((res = fp_mod(a, b, &x)) != FP_OKAY) {
      return res;
  }
  fp_copy(b, &y);

  /* 2. [modified] if x,y are both even then return an error! */
  if (fp_iseven (&x) == FP_YES && fp_iseven (&y) == FP_YES) {
    return FP_VAL;
  }

  /* 3. u=x, v=y, A=1, B=0, C=0,D=1 */
  fp_copy (&x, &u);
  fp_copy (&y, &v);
  fp_set (&A, 1);
  fp_set (&D, 1);

top:
  /* 4.  while u is even do */
  while (fp_iseven (&u) == FP_YES) {
    /* 4.1 u = u/2 */
    fp_div_2 (&u, &u);

    /* 4.2 if A or B is odd then */
    if (fp_isodd (&A) == FP_YES || fp_isodd (&B) == FP_YES) {
      /* A = (A+y)/2, B = (B-x)/2 */
      fp_add (&A, &y, &A);
      fp_sub (&B, &x, &B);
    }
    /* A = A/2, B = B/2 */
    fp_div_2 (&A, &A);
    fp_div_2 (&B, &B);
  }

  /* 5.  while v is even do */
  while (fp_iseven (&v) == FP_YES) {
    /* 5.1 v = v/2 */
    fp_div_2 (&v, &v);

    /* 5.2 if C or D is odd then */
    if (fp_isodd (&C) == FP_YES || fp_isodd (&D) == FP_YES) {
      /* C = (C+y)/2, D = (D-x)/2 */
      fp_add (&C, &y, &C);
      fp_sub (&D, &x, &D);
    }
    /* C = C/2, D = D/2 */
    fp_div_2 (&C, &C);
    fp_div_2 (&D, &D);
  }

  /* 6.  if u >= v then */
  if (fp_cmp (&u, &v) != FP_LT) {
    /* u = u - v, A = A - C, B = B - D */
    fp_sub (&u, &v, &u);
    fp_sub (&A, &C, &A);
    fp_sub (&B, &D, &B);
  } else {
    /* v - v - u, C = C - A, D = D - B */
    fp_sub (&v, &u, &v);
    fp_sub (&C, &A, &C);
    fp_sub (&D, &B, &D);
  }

  /* if not zero goto step 4 */
  if (fp_iszero (&u) == FP_NO)
    goto top;

  /* now a = C, b = D, gcd == g*v */

  /* if v != 1 then there is no inverse */
  if (fp_cmp_d (&v, 1) != FP_EQ) {
    return FP_VAL;
  }

  /* if its too low */
  while (fp_cmp_d(&C, 0) == FP_LT) {
      fp_add(&C, b, &C);
  }

  /* too big */
  while (fp_cmp_mag(&C, b) != FP_LT) {
      fp_sub(&C, b, &C);
  }

  /* C is now the inverse */
  fp_copy(&C, c);
  return FP_OKAY;
}

/* c = 1/a (mod b) for odd b only */
int fp_invmod(fp_int *a, fp_int *b, fp_int *c)
{
  fp_int  x, y, u, v, B, D;
  int     neg;

  /* 2. [modified] b must be odd   */
  if (fp_iseven (b) == FP_YES) {
    return fp_invmod_slow(a,b,c);
  }

  /* init all our temps */
  fp_init(&x);  fp_init(&y);
  fp_init(&u);  fp_init(&v);
  fp_init(&B);  fp_init(&D);

  /* x == modulus, y == value to invert */
  fp_copy(b, &x);

  /* we need y = |a| */
  fp_abs(a, &y);

  /* 3. u=x, v=y, A=1, B=0, C=0,D=1 */
  fp_copy(&x, &u);
  fp_copy(&y, &v);
  fp_set (&D, 1);

top:
  /* 4.  while u is even do */
  while (fp_iseven (&u) == FP_YES) {
    /* 4.1 u = u/2 */
    fp_div_2 (&u, &u);

    /* 4.2 if B is odd then */
    if (fp_isodd (&B) == FP_YES) {
      fp_sub (&B, &x, &B);
    }
    /* B = B/2 */
    fp_div_2 (&B, &B);
  }

  /* 5.  while v is even do */
  while (fp_iseven (&v) == FP_YES) {
    /* 5.1 v = v/2 */
    fp_div_2 (&v, &v);

    /* 5.2 if D is odd then */
    if (fp_isodd (&D) == FP_YES) {
      /* D = (D-x)/2 */
      fp_sub (&D, &x, &D);
    }
    /* D = D/2 */
    fp_div_2 (&D, &D);
  }

  /* 6.  if u >= v then */
  if (fp_cmp (&u, &v) != FP_LT) {
    /* u = u - v, B = B - D */
    fp_sub (&u, &v, &u);
    fp_sub (&B, &D, &B);
  } else {
    /* v - v - u, D = D - B */
    fp_sub (&v, &u, &v);
    fp_sub (&D, &B, &D);
  }

  /* if not zero goto step 4 */
  if (fp_iszero (&u) == FP_NO) {
    goto top;
  }

  /* now a = C, b = D, gcd == g*v */

  /* if v != 1 then there is no inverse */
  if (fp_cmp_d (&v, 1) != FP_EQ) {
    return FP_VAL;
  }

  /* b is now the inverse */
  neg = a->sign;
  while (D.sign == FP_NEG) {
    fp_add (&D, b, &D);
  }
  /* too big */
  while (fp_cmp_mag(&D, b) != FP_LT) {
    fp_sub(&D, b, &D);
  }
  fp_copy (&D, c);
  c->sign = neg;
  return FP_OKAY;
}

/* d = a * b (mod c) */
int fp_mulmod(fp_int *a, fp_int *b, fp_int *c, fp_int *d)
{
  int err;
  fp_int t;

  fp_init(&t);
  fp_mul(a, b, &t);
#if defined(ALT_ECC_SIZE) || defined(HAVE_WOLF_BIGINT)
  if (d->size < FP_SIZE) {
    err = fp_mod(&t, c, &t);
    fp_copy(&t, d);
  } else
#endif
  {
    err = fp_mod(&t, c, d);
  }

  return err;
}

/* d = a - b (mod c) */
int fp_submod(fp_int *a, fp_int *b, fp_int *c, fp_int *d)
{
  int err;
  fp_int t;

  fp_init(&t);
  fp_sub(a, b, &t);
#if defined(ALT_ECC_SIZE) || defined(HAVE_WOLF_BIGINT)
  if (d->size < FP_SIZE) {
    err = fp_mod(&t, c, &t);
    fp_copy(&t, d);
  } else
#endif
  {
    err = fp_mod(&t, c, d);
  }

  return err;
}

/* d = a + b (mod c) */
int fp_addmod(fp_int *a, fp_int *b, fp_int *c, fp_int *d)
{
  int err;
  fp_int t;

  fp_init(&t);
  fp_add(a, b, &t);
#if defined(ALT_ECC_SIZE) || defined(HAVE_WOLF_BIGINT)
  if (d->size < FP_SIZE) {
    err = fp_mod(&t, c, &t);
    fp_copy(&t, d);
  } else
#endif
  {
    err = fp_mod(&t, c, d);
  }

  return err;
}

#ifdef TFM_TIMING_RESISTANT

/* timing resistant montgomery ladder based exptmod
   Based on work by Marc Joye, Sung-Ming Yen, "The Montgomery Powering Ladder",
   Cryptographic Hardware and Embedded Systems, CHES 2002
*/
static int _fp_exptmod(fp_int * G, fp_int * X, fp_int * P, fp_int * Y)
{
#ifdef WC_NO_CACHE_RESISTANT
  fp_int   R[2];
#else
  fp_int   R[3];   /* need a temp for cache resistance */
#endif
  fp_digit buf, mp;
  int      err, bitcnt, digidx, y;

  /* now setup montgomery  */
  if ((err = fp_montgomery_setup (P, &mp)) != FP_OKAY) {
     return err;
  }

  fp_init(&R[0]);
  fp_init(&R[1]);
#ifndef WC_NO_CACHE_RESISTANT
  fp_init(&R[2]);
#endif

  /* now we need R mod m */
  fp_montgomery_calc_normalization (&R[0], P);

  /* now set R[0][1] to G * R mod m */
  if (fp_cmp_mag(P, G) != FP_GT) {
     /* G > P so we reduce it first */
     fp_mod(G, P, &R[1]);
  } else {
     fp_copy(G, &R[1]);
  }
  fp_mulmod (&R[1], &R[0], P, &R[1]);

  /* for j = t-1 downto 0 do
        r_!k = R0*R1; r_k = r_k^2
  */

  /* set initial mode and bit cnt */
  bitcnt = 1;
  buf    = 0;
  digidx = X->used - 1;

  for (;;) {
    /* grab next digit as required */
    if (--bitcnt == 0) {
      /* if digidx == -1 we are out of digits so break */
      if (digidx == -1) {
        break;
      }
      /* read next digit and reset bitcnt */
      buf    = X->dp[digidx--];
      bitcnt = (int)DIGIT_BIT;
    }

    /* grab the next msb from the exponent */
    y     = (int)(buf >> (DIGIT_BIT - 1)) & 1;
    buf <<= (fp_digit)1;

    /* do ops */
    fp_mul(&R[0], &R[1], &R[y^1]); fp_montgomery_reduce(&R[y^1], P, mp);

#ifdef WC_NO_CACHE_RESISTANT
    fp_sqr(&R[y], &R[y]);          fp_montgomery_reduce(&R[y], P, mp);
#else
    /* instead of using R[y] for sqr, which leaks key bit to cache monitor,
     * use R[2] as temp, make sure address calc is constant, keep
     * &R[0] and &R[1] in cache */
    fp_copy((fp_int*) ( ((wolfssl_word)&R[0] & wc_off_on_addr[y^1]) +
                        ((wolfssl_word)&R[1] & wc_off_on_addr[y]) ),
            &R[2]);
    fp_sqr(&R[2], &R[2]);          fp_montgomery_reduce(&R[2], P, mp);
    fp_copy(&R[2],
            (fp_int*) ( ((wolfssl_word)&R[0] & wc_off_on_addr[y^1]) +
                        ((wolfssl_word)&R[1] & wc_off_on_addr[y]) ) );
#endif /* WC_NO_CACHE_RESISTANT */
  }

   fp_montgomery_reduce(&R[0], P, mp);
   fp_copy(&R[0], Y);
   return FP_OKAY;
}

#else /* TFM_TIMING_RESISTANT */

/* y = g**x (mod b)
 * Some restrictions... x must be positive and < b
 */
static int _fp_exptmod(fp_int * G, fp_int * X, fp_int * P, fp_int * Y)
{
  fp_int   res;
  fp_digit buf, mp;
  int      err, bitbuf, bitcpy, bitcnt, mode, digidx, x, y, winsize;
#ifdef WOLFSSL_SMALL_STACK
  fp_int  *M;
#else
  fp_int   M[64];
#endif

  /* find window size */
  x = fp_count_bits (X);
  if (x <= 21) {
    winsize = 1;
  } else if (x <= 36) {
    winsize = 3;
  } else if (x <= 140) {
    winsize = 4;
  } else if (x <= 450) {
    winsize = 5;
  } else {
    winsize = 6;
  }

  /* now setup montgomery  */
  if ((err = fp_montgomery_setup (P, &mp)) != FP_OKAY) {
     return err;
  }

#ifdef WOLFSSL_SMALL_STACK
  /* only allocate space for what's needed */
  M = (fp_int*)XMALLOC(sizeof(fp_int)*(1 << winsize), NULL, DYNAMIC_TYPE_TMP_BUFFER);
  if (M == NULL) {
     return FP_MEM;
  }
#endif

  /* init M array */
  for(x = 0; x < (1 << winsize); x++)
    fp_init(&M[x]);

  /* setup result */
  fp_init(&res);

  /* create M table
   *
   * The M table contains powers of the input base, e.g. M[x] = G^x mod P
   *
   * The first half of the table is not computed though except for M[0] and M[1]
   */

   /* now we need R mod m */
   fp_montgomery_calc_normalization (&res, P);

   /* now set M[1] to G * R mod m */
   if (fp_cmp_mag(P, G) != FP_GT) {
      /* G > P so we reduce it first */
      fp_mod(G, P, &M[1]);
   } else {
      fp_copy(G, &M[1]);
   }
   fp_mulmod (&M[1], &res, P, &M[1]);

  /* compute the value at M[1<<(winsize-1)] by
   * squaring M[1] (winsize-1) times */
  fp_copy (&M[1], &M[1 << (winsize - 1)]);
  for (x = 0; x < (winsize - 1); x++) {
    fp_sqr (&M[1 << (winsize - 1)], &M[1 << (winsize - 1)]);
    fp_montgomery_reduce (&M[1 << (winsize - 1)], P, mp);
  }

  /* create upper table */
  for (x = (1 << (winsize - 1)) + 1; x < (1 << winsize); x++) {
    fp_mul(&M[x - 1], &M[1], &M[x]);
    fp_montgomery_reduce(&M[x], P, mp);
  }

  /* set initial mode and bit cnt */
  mode   = 0;
  bitcnt = 1;
  buf    = 0;
  digidx = X->used - 1;
  bitcpy = 0;
  bitbuf = 0;

  for (;;) {
    /* grab next digit as required */
    if (--bitcnt == 0) {
      /* if digidx == -1 we are out of digits so break */
      if (digidx == -1) {
        break;
      }
      /* read next digit and reset bitcnt */
      buf    = X->dp[digidx--];
      bitcnt = (int)DIGIT_BIT;
    }

    /* grab the next msb from the exponent */
    y     = (int)(buf >> (DIGIT_BIT - 1)) & 1;
    buf <<= (fp_digit)1;

    /* if the bit is zero and mode == 0 then we ignore it
     * These represent the leading zero bits before the first 1 bit
     * in the exponent.  Technically this opt is not required but it
     * does lower the # of trivial squaring/reductions used
     */
    if (mode == 0 && y == 0) {
      continue;
    }

    /* if the bit is zero and mode == 1 then we square */
    if (mode == 1 && y == 0) {
      fp_sqr(&res, &res);
      fp_montgomery_reduce(&res, P, mp);
      continue;
    }

    /* else we add it to the window */
    bitbuf |= (y << (winsize - ++bitcpy));
    mode    = 2;

    if (bitcpy == winsize) {
      /* ok window is filled so square as required and multiply  */
      /* square first */
      for (x = 0; x < winsize; x++) {
        fp_sqr(&res, &res);
        fp_montgomery_reduce(&res, P, mp);
      }

      /* then multiply */
      fp_mul(&res, &M[bitbuf], &res);
      fp_montgomery_reduce(&res, P, mp);

      /* empty window and reset */
      bitcpy = 0;
      bitbuf = 0;
      mode   = 1;
    }
  }

  /* if bits remain then square/multiply */
  if (mode == 2 && bitcpy > 0) {
    /* square then multiply if the bit is set */
    for (x = 0; x < bitcpy; x++) {
      fp_sqr(&res, &res);
      fp_montgomery_reduce(&res, P, mp);

      /* get next bit of the window */
      bitbuf <<= 1;
      if ((bitbuf & (1 << winsize)) != 0) {
        /* then multiply */
        fp_mul(&res, &M[1], &res);
        fp_montgomery_reduce(&res, P, mp);
      }
    }
  }

  /* fixup result if Montgomery reduction is used
   * recall that any value in a Montgomery system is
   * actually multiplied by R mod n.  So we have
   * to reduce one more time to cancel out the factor
   * of R.
   */
  fp_montgomery_reduce(&res, P, mp);

  /* swap res with Y */
  fp_copy (&res, Y);

#ifdef WOLFSSL_SMALL_STACK
  XFREE(M, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

  return FP_OKAY;
}

#endif /* TFM_TIMING_RESISTANT */

int fp_exptmod(fp_int * G, fp_int * X, fp_int * P, fp_int * Y)
{
   /* prevent overflows */
   if (P->used > (FP_SIZE/2)) {
      return FP_VAL;
   }

   if (X->sign == FP_NEG) {
#ifndef POSITIVE_EXP_ONLY  /* reduce stack if assume no negatives */
      int    err;
      fp_int tmp;

      /* yes, copy G and invmod it */
      fp_init_copy(&tmp, G);
      if ((err = fp_invmod(&tmp, P, &tmp)) != FP_OKAY) {
         return err;
      }
      X->sign = FP_ZPOS;
      err =  _fp_exptmod(&tmp, X, P, Y);
      if (X != Y) {
         X->sign = FP_NEG;
      }
      return err;
#else
      return FP_VAL;
#endif
   }
   else {
      /* Positive exponent so just exptmod */
      return _fp_exptmod(G, X, P, Y);
   }
}

/* computes a = 2**b */
void fp_2expt(fp_int *a, int b)
{
   int     z;

   /* zero a as per default */
   fp_zero (a);

   if (b < 0) {
      return;
   }

   z = b / DIGIT_BIT;
   if (z >= FP_SIZE) {
      return;
   }

  /* set the used count of where the bit will go */
  a->used = z + 1;

  /* put the single bit in its place */
  a->dp[z] = ((fp_digit)1) << (b % DIGIT_BIT);
}

/* b = a*a  */
void fp_sqr(fp_int *A, fp_int *B)
{
    int y, oldused;

    oldused = B->used;
    y = A->used;

    /* call generic if we're out of range */
    if (y + y > FP_SIZE) {
       fp_sqr_comba(A, B);
       goto clean;
    }

#if defined(TFM_SQR3) && FP_SIZE >= 6
        if (y <= 3) {
           fp_sqr_comba3(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SQR4) && FP_SIZE >= 8
        if (y == 4) {
           fp_sqr_comba4(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SQR6) && FP_SIZE >= 12
        if (y <= 6) {
           fp_sqr_comba6(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SQR7) && FP_SIZE >= 14
        if (y == 7) {
           fp_sqr_comba7(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SQR8) && FP_SIZE >= 16
        if (y == 8) {
           fp_sqr_comba8(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SQR9) && FP_SIZE >= 18
        if (y == 9) {
           fp_sqr_comba9(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SQR12) && FP_SIZE >= 24
        if (y <= 12) {
           fp_sqr_comba12(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SQR17) && FP_SIZE >= 34
        if (y <= 17) {
           fp_sqr_comba17(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SMALL_SET)
        if (y <= 16) {
           fp_sqr_comba_small(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SQR20) && FP_SIZE >= 40
        if (y <= 20) {
           fp_sqr_comba20(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SQR24) && FP_SIZE >= 48
        if (y <= 24) {
           fp_sqr_comba24(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SQR28) && FP_SIZE >= 56
        if (y <= 28) {
           fp_sqr_comba28(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SQR32) && FP_SIZE >= 64
        if (y <= 32) {
           fp_sqr_comba32(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SQR48) && FP_SIZE >= 96
        if (y <= 48) {
           fp_sqr_comba48(A,B);
           goto clean;
        }
#endif
#if defined(TFM_SQR64) && FP_SIZE >= 128
        if (y <= 64) {
           fp_sqr_comba64(A,B);
           goto clean;
        }
#endif
       fp_sqr_comba(A, B);

clean:
  /* zero any excess digits on the destination that we didn't write to */
  for (y = B->used; y >= 0 && y < oldused; y++) {
    B->dp[y] = 0;
  }
}

/* generic comba squarer */
void fp_sqr_comba(fp_int *A, fp_int *B)
{
  int       pa, ix, iz;
  fp_digit  c0, c1, c2;
  fp_int    tmp, *dst;
#ifdef TFM_ISO
  fp_word   tt;
#endif

  /* get size of output and trim */
  pa = A->used + A->used;
  if (pa >= FP_SIZE) {
     pa = FP_SIZE-1;
  }

  /* number of output digits to produce */
  COMBA_START;
  COMBA_CLEAR;

  if (A == B) {
     fp_init(&tmp);
     dst = &tmp;
  } else {
     fp_zero(B);
     dst = B;
  }

  for (ix = 0; ix < pa; ix++) {
      int      tx, ty, iy;
      fp_digit *tmpy, *tmpx;

      /* get offsets into the two bignums */
      ty = MIN(A->used-1, ix);
      tx = ix - ty;

      /* setup temp aliases */
      tmpx = A->dp + tx;
      tmpy = A->dp + ty;

      /* this is the number of times the loop will iterate,
         while (tx++ < a->used && ty-- >= 0) { ... }
       */
      iy = MIN(A->used-tx, ty+1);

      /* now for squaring tx can never equal ty
       * we halve the distance since they approach
       * at a rate of 2x and we have to round because
       * odd cases need to be executed
       */
      iy = MIN(iy, (ty-tx+1)>>1);

      /* forward carries */
      COMBA_FORWARD;

      /* execute loop */
      for (iz = 0; iz < iy; iz++) {
          SQRADD2(*tmpx++, *tmpy--);
      }

      /* even columns have the square term in them */
      if ((ix&1) == 0) {
          /* TAO change COMBA_ADD back to SQRADD */
          SQRADD(A->dp[ix>>1], A->dp[ix>>1]);
      }

      /* store it */
      COMBA_STORE(dst->dp[ix]);
  }

  COMBA_FINI;

  /* setup dest */
  dst->used = pa;
  fp_clamp (dst);
  if (dst != B) {
     fp_copy(dst, B);
  }
}

int fp_cmp(fp_int *a, fp_int *b)
{
   if (a->sign == FP_NEG && b->sign == FP_ZPOS) {
      return FP_LT;
   } else if (a->sign == FP_ZPOS && b->sign == FP_NEG) {
      return FP_GT;
   } else {
      /* compare digits */
      if (a->sign == FP_NEG) {
         /* if negative compare opposite direction */
         return fp_cmp_mag(b, a);
      } else {
         return fp_cmp_mag(a, b);
      }
   }
}

/* compare against a single digit */
int fp_cmp_d(fp_int *a, fp_digit b)
{
  /* special case for zero*/
  if (a->used == 0 && b == 0)
    return FP_EQ;

  /* compare based on sign */
  if ((b && a->used == 0) || a->sign == FP_NEG) {
    return FP_LT;
  }

  /* compare based on magnitude */
  if (a->used > 1) {
    return FP_GT;
  }

  /* compare the only digit of a to b */
  if (a->dp[0] > b) {
    return FP_GT;
  } else if (a->dp[0] < b) {
    return FP_LT;
  } else {
    return FP_EQ;
  }

}

int fp_cmp_mag(fp_int *a, fp_int *b)
{
   int x;

   if (a->used > b->used) {
      return FP_GT;
   } else if (a->used < b->used) {
      return FP_LT;
   } else {
      for (x = a->used - 1; x >= 0; x--) {
          if (a->dp[x] > b->dp[x]) {
             return FP_GT;
          } else if (a->dp[x] < b->dp[x]) {
             return FP_LT;
          }
      }
   }
   return FP_EQ;
}

/* sets up the montgomery reduction */
int fp_montgomery_setup(fp_int *a, fp_digit *rho)
{
  fp_digit x, b;

/* fast inversion mod 2**k
 *
 * Based on the fact that
 *
 * XA = 1 (mod 2**n)  =>  (X(2-XA)) A = 1 (mod 2**2n)
 *                    =>  2*X*A - X*X*A*A = 1
 *                    =>  2*(1) - (1)     = 1
 */
  b = a->dp[0];

  if ((b & 1) == 0) {
    return FP_VAL;
  }

  x = (((b + 2) & 4) << 1) + b; /* here x*a==1 mod 2**4 */
  x *= 2 - b * x;               /* here x*a==1 mod 2**8 */
  x *= 2 - b * x;               /* here x*a==1 mod 2**16 */
  x *= 2 - b * x;               /* here x*a==1 mod 2**32 */
#ifdef FP_64BIT
  x *= 2 - b * x;               /* here x*a==1 mod 2**64 */
#endif

  /* rho = -1/m mod b */
  *rho = (fp_digit) (((fp_word) 1 << ((fp_word) DIGIT_BIT)) - ((fp_word)x));

  return FP_OKAY;
}

/* computes a = B**n mod b without division or multiplication useful for
 * normalizing numbers in a Montgomery system.
 */
void fp_montgomery_calc_normalization(fp_int *a, fp_int *b)
{
  int     x, bits;

  /* how many bits of last digit does b use */
  bits = fp_count_bits (b) % DIGIT_BIT;
  if (!bits) bits = DIGIT_BIT;

  /* compute A = B^(n-1) * 2^(bits-1) */
  if (b->used > 1) {
     fp_2expt (a, (b->used - 1) * DIGIT_BIT + bits - 1);
  } else {
     fp_set(a, 1);
     bits = 1;
  }

  /* now compute C = A * B mod b */
  for (x = bits - 1; x < (int)DIGIT_BIT; x++) {
    fp_mul_2 (a, a);
    if (fp_cmp_mag (a, b) != FP_LT) {
      s_fp_sub (a, b, a);
    }
  }
}


#ifdef TFM_SMALL_MONT_SET
    #include "fp_mont_small.i"
#endif

#ifdef HAVE_INTEL_MULX
static INLINE void innermul8_mulx(fp_digit *c_mulx, fp_digit *cy_mulx, fp_digit *tmpm, fp_digit mu)
{
    fp_digit cy = *cy_mulx ;
    INNERMUL8_MULX ;
    *cy_mulx = cy ;
}

/* computes x/R == x (mod N) via Montgomery Reduction */
static void fp_montgomery_reduce_mulx(fp_int *a, fp_int *m, fp_digit mp)
{
   fp_digit c[FP_SIZE+1], *_c, *tmpm, mu = 0;
   int      oldused, x, y, pa;

   /* bail if too large */
   if (m->used > (FP_SIZE/2)) {
      (void)mu;                     /* shut up compiler */
      return;
   }

#ifdef TFM_SMALL_MONT_SET
   if (m->used <= 16) {
      fp_montgomery_reduce_small(a, m, mp);
      return;
   }
#endif


   /* now zero the buff */
   XMEMSET(c, 0, sizeof(c));
   pa = m->used;

   /* copy the input */
   oldused = a->used;
   for (x = 0; x < oldused; x++) {
       c[x] = a->dp[x];
   }
   MONT_START;

   for (x = 0; x < pa; x++) {
       fp_digit cy = 0;
       /* get Mu for this round */
       LOOP_START;
       _c   = c + x;
       tmpm = m->dp;
       y = 0;
        for (; y < (pa & ~7); y += 8) {
              innermul8_mulx(_c, &cy, tmpm, mu) ;
              _c   += 8;
              tmpm += 8;
           }
       for (; y < pa; y++) {
          INNERMUL;
          ++_c;
       }
       LOOP_END;
       while (cy) {
           PROPCARRY;
           ++_c;
       }
  }

  /* now copy out */
  _c   = c + pa;
  tmpm = a->dp;
  for (x = 0; x < pa+1; x++) {
     *tmpm++ = *_c++;
  }

  /* zero any excess digits on the destination that we didn't write to */
  for (; x < oldused; x++) {
     *tmpm++ = 0;
  }

  MONT_FINI;

  a->used = pa+1;
  fp_clamp(a);

  /* if A >= m then A = A - m */
  if (fp_cmp_mag (a, m) != FP_LT) {
    s_fp_sub (a, m, a);
  }
}
#endif

/* computes x/R == x (mod N) via Montgomery Reduction */
void fp_montgomery_reduce(fp_int *a, fp_int *m, fp_digit mp)
{
   fp_digit c[FP_SIZE+1], *_c, *tmpm, mu = 0;
   int      oldused, x, y, pa;

   IF_HAVE_INTEL_MULX(fp_montgomery_reduce_mulx(a, m, mp), return) ;

   /* bail if too large */
   if (m->used > (FP_SIZE/2)) {
      (void)mu;                     /* shut up compiler */
      return;
   }

#ifdef TFM_SMALL_MONT_SET
   if (m->used <= 16) {
      fp_montgomery_reduce_small(a, m, mp);
      return;
   }
#endif


   /* now zero the buff */
   XMEMSET(c, 0, sizeof(c));
   pa = m->used;

   /* copy the input */
   oldused = a->used;
   for (x = 0; x < oldused; x++) {
       c[x] = a->dp[x];
   }
   MONT_START;

   for (x = 0; x < pa; x++) {
       fp_digit cy = 0;
       /* get Mu for this round */
       LOOP_START;
       _c   = c + x;
       tmpm = m->dp;
       y = 0;
#if defined(INNERMUL8)
        for (; y < (pa & ~7); y += 8) {
              INNERMUL8 ;
              _c   += 8;
              tmpm += 8;
           }
#endif
       for (; y < pa; y++) {
          INNERMUL;
          ++_c;
       }
       LOOP_END;
       while (cy) {
           PROPCARRY;
           ++_c;
       }
  }

  /* now copy out */
  _c   = c + pa;
  tmpm = a->dp;
  for (x = 0; x < pa+1; x++) {
     *tmpm++ = *_c++;
  }

  /* zero any excess digits on the destination that we didn't write to */
  for (; x < oldused; x++) {
     *tmpm++ = 0;
  }

  MONT_FINI;

  a->used = pa+1;
  fp_clamp(a);

  /* if A >= m then A = A - m */
  if (fp_cmp_mag (a, m) != FP_LT) {
    s_fp_sub (a, m, a);
  }
}

void fp_read_unsigned_bin(fp_int *a, const unsigned char *b, int c)
{
#if defined(ALT_ECC_SIZE) || defined(HAVE_WOLF_BIGINT)
  const word32 maxC = (a->size * sizeof(fp_digit));
#else
  const word32 maxC = (FP_SIZE * sizeof(fp_digit));
#endif

  /* zero the int */
  fp_zero (a);

  /* if input b excess max, then truncate */
  if (c > 0 && (word32)c > maxC) {
     int excess = (c - maxC);
     c -= excess;
     b += excess;
  }

  /* If we know the endianness of this architecture, and we're using
     32-bit fp_digits, we can optimize this */
#if (defined(LITTLE_ENDIAN_ORDER) || defined(BIG_ENDIAN_ORDER)) && \
    defined(FP_32BIT)
  /* But not for both simultaneously */
#if defined(LITTLE_ENDIAN_ORDER) && defined(BIG_ENDIAN_ORDER)
#error Both LITTLE_ENDIAN_ORDER and BIG_ENDIAN_ORDER defined.
#endif
  {
     unsigned char *pd = (unsigned char *)a->dp;

     a->used = (c + sizeof(fp_digit) - 1)/sizeof(fp_digit);
     /* read the bytes in */
#ifdef BIG_ENDIAN_ORDER
     {
       /* Use Duff's device to unroll the loop. */
       int idx = (c - 1) & ~3;
       switch (c % 4) {
       case 0:    do { pd[idx+0] = *b++;
       case 3:         pd[idx+1] = *b++;
       case 2:         pd[idx+2] = *b++;
       case 1:         pd[idx+3] = *b++;
                     idx -= 4;
                 } while ((c -= 4) > 0);
       }
     }
#else
     for (c -= 1; c >= 0; c -= 1) {
       pd[c] = *b++;
     }
#endif
  }
#else
  /* read the bytes in */
  for (; c > 0; c--) {
     fp_mul_2d (a, 8, a);
     a->dp[0] |= *b++;

     if (a->used == 0) {
         a->used = 1;
     }
  }
#endif
  fp_clamp (a);
}

int fp_to_unsigned_bin_at_pos(int x, fp_int *t, unsigned char *b)
{
   while (fp_iszero (t) == FP_NO) {
      b[x++] = (unsigned char) (t->dp[0] & 255);
      fp_div_2d (t, 8, t, NULL);
  }
  return x;
}

void fp_to_unsigned_bin(fp_int *a, unsigned char *b)
{
  int     x;
  fp_int  t;

  fp_init_copy(&t, a);

  x = fp_to_unsigned_bin_at_pos(0, &t, b);
  fp_reverse (b, x);
}

int fp_unsigned_bin_size(fp_int *a)
{
  int     size = fp_count_bits (a);
  return (size / 8 + ((size & 7) != 0 ? 1 : 0));
}

void fp_set(fp_int *a, fp_digit b)
{
   fp_zero(a);
   a->dp[0] = b;
   a->used  = a->dp[0] ? 1 : 0;
}


#ifndef MP_SET_CHUNK_BITS
    #define MP_SET_CHUNK_BITS 4
#endif
void fp_set_int(fp_int *a, unsigned long b)
{
  int x;

  /* use direct fp_set if b is less than fp_digit max */
  if (b < FP_DIGIT_MAX) {
    fp_set (a, b);
    return;
  }

  fp_zero (a);

  /* set chunk bits at a time */
  for (x = 0; x < (int)(sizeof(b) * 8) / MP_SET_CHUNK_BITS; x++) {
    fp_mul_2d (a, MP_SET_CHUNK_BITS, a);

    /* OR in the top bits of the source */
    a->dp[0] |= (b >> ((sizeof(b) * 8) - MP_SET_CHUNK_BITS)) &
                                  ((1 << MP_SET_CHUNK_BITS) - 1);

    /* shift the source up to the next chunk bits */
    b <<= MP_SET_CHUNK_BITS;

    /* ensure that digits are not clamped off */
    a->used += 1;
  }

  /* clamp digits */
  fp_clamp(a);
}

/* check if a bit is set */
int fp_is_bit_set (fp_int *a, fp_digit b)
{
    fp_digit i;

    if (b > FP_MAX_BITS)
        return 0;
    else
        i = b/DIGIT_BIT;

    if ((fp_digit)a->used < i)
        return 0;

    return (int)((a->dp[i] >> b%DIGIT_BIT) & (fp_digit)1);
}

/* set the b bit of a */
int fp_set_bit (fp_int * a, fp_digit b)
{
    fp_digit i;

    if (b > FP_MAX_BITS)
        return 0;
    else
        i = b/DIGIT_BIT;

    /* set the used count of where the bit will go if required */
    if (a->used < (int)(i+1))
        a->used = (int)(i+1);

    /* put the single bit in its place */
    a->dp[i] |= ((fp_digit)1) << (b % DIGIT_BIT);

    return MP_OKAY;
}

int fp_count_bits (fp_int * a)
{
  int     r;
  fp_digit q;

  /* shortcut */
  if (a->used == 0) {
    return 0;
  }

  /* get number of digits and add that */
  r = (a->used - 1) * DIGIT_BIT;

  /* take the last digit and count the bits in it */
  q = a->dp[a->used - 1];
  while (q > ((fp_digit) 0)) {
    ++r;
    q >>= ((fp_digit) 1);
  }

  return r;
}

int fp_leading_bit(fp_int *a)
{
    int bit = 0;

    if (a->used != 0) {
        fp_digit q = a->dp[a->used - 1];
        int qSz = sizeof(fp_digit);

        while (qSz > 0) {
            if ((unsigned char)q != 0)
                bit = (q & 0x80) != 0;
            q >>= 8;
            qSz--;
        }
    }

    return bit;
}

void fp_lshd(fp_int *a, int x)
{
    int y;

    /* move up and truncate as required */
    y = MIN(a->used + x - 1, (int)(FP_SIZE-1));

    /* store new size */
    a->used = y + 1;

    /* move digits */
    for (; y >= x; y--) {
        a->dp[y] = a->dp[y-x];
    }

    /* zero lower digits */
    for (; y >= 0; y--) {
        a->dp[y] = 0;
    }

    /* clamp digits */
    fp_clamp(a);
}


/* right shift by bit count */
void fp_rshb(fp_int *c, int x)
{
    fp_digit *tmpc, mask, shift;
    fp_digit r, rr;
    fp_digit D = x;

    /* mask */
    mask = (((fp_digit)1) << D) - 1;

    /* shift for lsb */
    shift = DIGIT_BIT - D;

    /* alias */
    tmpc = c->dp + (c->used - 1);

    /* carry */
    r = 0;
    for (x = c->used - 1; x >= 0; x--) {
      /* get the lower  bits of this word in a temp */
      rr = *tmpc & mask;

      /* shift the current word and mix in the carry bits from previous word */
      *tmpc = (*tmpc >> D) | (r << shift);
      --tmpc;

      /* set the carry to the carry bits of the current word found above */
      r = rr;
    }

    /* clamp digits */
    fp_clamp(c);
}


void fp_rshd(fp_int *a, int x)
{
  int y;

  /* too many digits just zero and return */
  if (x >= a->used) {
     fp_zero(a);
     return;
  }

   /* shift */
   for (y = 0; y < a->used - x; y++) {
      a->dp[y] = a->dp[y+x];
   }

   /* zero rest */
   for (; y < a->used; y++) {
      a->dp[y] = 0;
   }

   /* decrement count */
   a->used -= x;
   fp_clamp(a);
}

/* reverse an array, used for radix code */
void fp_reverse (unsigned char *s, int len)
{
  int     ix, iy;
  unsigned char t;

  ix = 0;
  iy = len - 1;
  while (ix < iy) {
    t     = s[ix];
    s[ix] = s[iy];
    s[iy] = t;
    ++ix;
    --iy;
  }
}


/* c = a - b */
void fp_sub_d(fp_int *a, fp_digit b, fp_int *c)
{
   fp_int tmp;
   fp_init(&tmp);
   fp_set(&tmp, b);
#if defined(ALT_ECC_SIZE) || defined(HAVE_WOLF_BIGINT)
   if (c->size < FP_SIZE) {
     fp_sub(a, &tmp, &tmp);
     fp_copy(&tmp, c);
   } else
#endif
   {
     fp_sub(a, &tmp, c);
   }
}


/* wolfSSL callers from normal lib */

/* init a new mp_int */
int mp_init (mp_int * a)
{
  if (a)
    fp_init(a);
  return MP_OKAY;
}

void fp_init(fp_int *a)
{
#if defined(ALT_ECC_SIZE) || defined(HAVE_WOLF_BIGINT)
    a->size = FP_SIZE;
#endif
#ifdef HAVE_WOLF_BIGINT
    wc_bigint_init(&a->raw);
#endif
    fp_zero(a);
}

void fp_zero(fp_int *a)
{
    int size = FP_SIZE;
    a->used = 0;
    a->sign = FP_ZPOS;
#if defined(ALT_ECC_SIZE) || defined(HAVE_WOLF_BIGINT)
    size = a->size;
#endif
    XMEMSET(a->dp, 0, size * sizeof(fp_digit));
}

void fp_clear(fp_int *a)
{
    int size = FP_SIZE;
    a->used = 0;
    a->sign = FP_ZPOS;
#if defined(ALT_ECC_SIZE) || defined(HAVE_WOLF_BIGINT)
    size = a->size;
#endif
    XMEMSET(a->dp, 0, size * sizeof(fp_digit));
    fp_free(a);
}

void fp_forcezero (mp_int * a)
{
    int size = FP_SIZE;
    a->used = 0;
    a->sign = FP_ZPOS;
#if defined(ALT_ECC_SIZE) || defined(HAVE_WOLF_BIGINT)
    size = a->size;
#endif
    ForceZero(a->dp, size * sizeof(fp_digit));
#ifdef HAVE_WOLF_BIGINT
    wc_bigint_zero(&a->raw);
#endif
    fp_free(a);
}

void mp_forcezero (mp_int * a)
{
    fp_forcezero(a);
}

void fp_free(fp_int* a)
{
#ifdef HAVE_WOLF_BIGINT
    wc_bigint_free(&a->raw);
#else
    (void)a;
#endif
}


/* clear one (frees)  */
void mp_clear (mp_int * a)
{
    if (a == NULL)
        return;
    fp_clear(a);
}

void mp_free(mp_int* a)
{
    fp_free(a);
}

/* handle up to 6 inits */
int mp_init_multi(mp_int* a, mp_int* b, mp_int* c, mp_int* d,
                  mp_int* e, mp_int* f)
{
    if (a)
        fp_init(a);
    if (b)
        fp_init(b);
    if (c)
        fp_init(c);
    if (d)
        fp_init(d);
    if (e)
        fp_init(e);
    if (f)
        fp_init(f);

    return MP_OKAY;
}

/* high level addition (handles signs) */
int mp_add (mp_int * a, mp_int * b, mp_int * c)
{
  fp_add(a, b, c);
  return MP_OKAY;
}

/* high level subtraction (handles signs) */
int mp_sub (mp_int * a, mp_int * b, mp_int * c)
{
  fp_sub(a, b, c);
  return MP_OKAY;
}

/* high level multiplication (handles sign) */
#if defined(FREESCALE_LTC_TFM)
int wolfcrypt_mp_mul(mp_int * a, mp_int * b, mp_int * c)
#else
int mp_mul (mp_int * a, mp_int * b, mp_int * c)
#endif
{
  fp_mul(a, b, c);
  return MP_OKAY;
}

int mp_mul_d (mp_int * a, mp_digit b, mp_int * c)
{
  fp_mul_d(a, b, c);
  return MP_OKAY;
}

/* d = a * b (mod c) */
#if defined(FREESCALE_LTC_TFM)
int wolfcrypt_mp_mulmod (mp_int * a, mp_int * b, mp_int * c, mp_int * d)
#else
int mp_mulmod (mp_int * a, mp_int * b, mp_int * c, mp_int * d)
#endif
{
  return fp_mulmod(a, b, c, d);
}

/* d = a - b (mod c) */
int mp_submod(mp_int *a, mp_int *b, mp_int *c, mp_int *d)
{
  return fp_submod(a, b, c, d);
}

/* d = a + b (mod c) */
int mp_addmod(mp_int *a, mp_int *b, mp_int *c, mp_int *d)
{
  return fp_addmod(a, b, c, d);
}

/* c = a mod b, 0 <= c < b */
#if defined(FREESCALE_LTC_TFM)
int wolfcrypt_mp_mod (mp_int * a, mp_int * b, mp_int * c)
#else
int mp_mod (mp_int * a, mp_int * b, mp_int * c)
#endif
{
  return fp_mod (a, b, c);
}

/* hac 14.61, pp608 */
#if defined(FREESCALE_LTC_TFM)
int wolfcrypt_mp_invmod (mp_int * a, mp_int * b, mp_int * c)
#else
int mp_invmod (mp_int * a, mp_int * b, mp_int * c)
#endif
{
  return fp_invmod(a, b, c);
}

/* this is a shell function that calls either the normal or Montgomery
 * exptmod functions.  Originally the call to the montgomery code was
 * embedded in the normal function but that wasted alot of stack space
 * for nothing (since 99% of the time the Montgomery code would be called)
 */
#if defined(FREESCALE_LTC_TFM)
int wolfcrypt_mp_exptmod (mp_int * G, mp_int * X, mp_int * P, mp_int * Y)
#else
int mp_exptmod (mp_int * G, mp_int * X, mp_int * P, mp_int * Y)
#endif
{
  return fp_exptmod(G, X, P, Y);
}

/* compare two ints (signed)*/
int mp_cmp (mp_int * a, mp_int * b)
{
  return fp_cmp(a, b);
}

/* compare a digit */
int mp_cmp_d(mp_int * a, mp_digit b)
{
  return fp_cmp_d(a, b);
}

/* get the size for an unsigned equivalent */
int mp_unsigned_bin_size (mp_int * a)
{
  return fp_unsigned_bin_size(a);
}

int mp_to_unsigned_bin_at_pos(int x, fp_int *t, unsigned char *b)
{
    return fp_to_unsigned_bin_at_pos(x, t, b);
}

/* store in unsigned [big endian] format */
int mp_to_unsigned_bin (mp_int * a, unsigned char *b)
{
  fp_to_unsigned_bin(a,b);
  return MP_OKAY;
}

/* reads a unsigned char array, assumes the msb is stored first [big endian] */
int mp_read_unsigned_bin (mp_int * a, const unsigned char *b, int c)
{
  fp_read_unsigned_bin(a, b, c);
  return MP_OKAY;
}


int mp_sub_d(fp_int *a, fp_digit b, fp_int *c)
{
    fp_sub_d(a, b, c);
    return MP_OKAY;
}

int mp_mul_2d(fp_int *a, int b, fp_int *c)
{
    fp_mul_2d(a, b, c);
	return MP_OKAY;
}

int mp_div(fp_int * a, fp_int * b, fp_int * c, fp_int * d)
{
    return fp_div(a, b, c, d);
}

int mp_div_2d(fp_int* a, int b, fp_int* c, fp_int* d)
{
    fp_div_2d(a, b, c, d);
    return MP_OKAY;
}

void fp_copy(fp_int *a, fp_int *b)
{
    /* if source and destination are different */
    if (a != b) {
#if defined(ALT_ECC_SIZE) || defined(HAVE_WOLF_BIGINT)
        /* verify a will fit in b */
        if (b->size >= a->used) {
            int x, oldused;
            oldused = b->used;
            b->used = a->used;
            b->sign = a->sign;

            XMEMCPY(b->dp, a->dp, a->used * sizeof(fp_digit));

            /* zero any excess digits on the destination that we didn't write to */
            for (x = b->used; x >= 0 && x < oldused; x++) {
                b->dp[x] = 0;
            }
        }
        else {
            /* TODO: Handle error case */
        }
#else
        /* all dp's are same size, so do straight copy */
        b->used = a->used;
        b->sign = a->sign;
        XMEMCPY(b->dp, a->dp, FP_SIZE * sizeof(fp_digit));
#endif
    }
}

void fp_init_copy(fp_int *a, fp_int* b)
{
    if (a != b) {
        fp_init(a);
        fp_copy(b, a);
    }
}

/* fast math wrappers */
int mp_copy(fp_int* a, fp_int* b)
{
    fp_copy(a, b);
    return MP_OKAY;
}

int mp_isodd(mp_int* a)
{
    return fp_isodd(a);
}

int mp_iszero(mp_int* a)
{
    return fp_iszero(a);
}

int mp_count_bits (mp_int* a)
{
    return fp_count_bits(a);
}

int mp_leading_bit (mp_int* a)
{
    return fp_leading_bit(a);
}

void mp_rshb (mp_int* a, int x)
{
    fp_rshb(a, x);
}

void mp_rshd (mp_int* a, int x)
{
    fp_rshd(a, x);
}

int mp_set_int(mp_int *a, unsigned long b)
{
    fp_set_int(a, b);
    return MP_OKAY;
}

int mp_is_bit_set (mp_int *a, mp_digit b)
{
    return fp_is_bit_set(a, b);
}

int mp_set_bit(mp_int *a, mp_digit b)
{
    return fp_set_bit(a, b);
}

#if defined(WOLFSSL_KEY_GEN) || defined (HAVE_ECC)

/* c = a * a (mod b) */
int fp_sqrmod(fp_int *a, fp_int *b, fp_int *c)
{
  int err;
  fp_int t;

  fp_init(&t);
  fp_sqr(a, &t);

#if defined(ALT_ECC_SIZE) || defined(HAVE_WOLF_BIGINT)
  if (c->size < FP_SIZE) {
    err = fp_mod(&t, b, &t);
    fp_copy(&t, c);
  }
  else
#endif
  {
    err = fp_mod(&t, b, c);
  }

  return err;
}

/* fast math conversion */
int mp_sqrmod(mp_int *a, mp_int *b, mp_int *c)
{
    return fp_sqrmod(a, b, c);
}

/* fast math conversion */
int mp_montgomery_calc_normalization(mp_int *a, mp_int *b)
{
    fp_montgomery_calc_normalization(a, b);
    return MP_OKAY;
}

#endif /* WOLFSSL_KEYGEN || HAVE_ECC */


#if defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY) || \
    defined(WOLFSSL_DEBUG_MATH)

#ifdef WOLFSSL_KEY_GEN
/* swap the elements of two integers, for cases where you can't simply swap the
 * mp_int pointers around
 */
static void fp_exch (fp_int * a, fp_int * b)
{
    fp_int  t;

    t  = *a;
    *a = *b;
    *b = t;
}
#endif

static const int lnz[16] = {
   4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

/* Counts the number of lsbs which are zero before the first zero bit */
int fp_cnt_lsb(fp_int *a)
{
   int x;
   fp_digit q, qq;

   /* easy out */
   if (fp_iszero(a) == FP_YES) {
      return 0;
   }

   /* scan lower digits until non-zero */
   for (x = 0; x < a->used && a->dp[x] == 0; x++) {}
   q = a->dp[x];
   x *= DIGIT_BIT;

   /* now scan this digit until a 1 is found */
   if ((q & 1) == 0) {
      do {
         qq  = q & 15;
         x  += lnz[qq];
         q >>= 4;
      } while (qq == 0);
   }
   return x;
}


static int s_is_power_of_two(fp_digit b, int *p)
{
   int x;

   /* fast return if no power of two */
   if ((b==0) || (b & (b-1))) {
      return FP_NO;
   }

   for (x = 0; x < DIGIT_BIT; x++) {
      if (b == (((fp_digit)1)<<x)) {
         *p = x;
         return FP_YES;
      }
   }
   return FP_NO;
}

/* a/b => cb + d == a */
static int fp_div_d(fp_int *a, fp_digit b, fp_int *c, fp_digit *d)
{
  fp_int   q;
  fp_word  w;
  fp_digit t;
  int      ix;

  /* cannot divide by zero */
  if (b == 0) {
     return FP_VAL;
  }

  /* quick outs */
  if (b == 1 || fp_iszero(a) == FP_YES) {
     if (d != NULL) {
        *d = 0;
     }
     if (c != NULL) {
        fp_copy(a, c);
     }
     return FP_OKAY;
  }

  /* power of two ? */
  if (s_is_power_of_two(b, &ix) == FP_YES) {
     if (d != NULL) {
        *d = a->dp[0] & ((((fp_digit)1)<<ix) - 1);
     }
     if (c != NULL) {
        fp_div_2d(a, ix, c, NULL);
     }
     return FP_OKAY;
  }

  if (c != NULL) {
    /* no easy answer [c'est la vie].  Just division */
    fp_init(&q);

    q.used = a->used;
    q.sign = a->sign;
  }

  w = 0;
  for (ix = a->used - 1; ix >= 0; ix--) {
     w = (w << ((fp_word)DIGIT_BIT)) | ((fp_word)a->dp[ix]);

     if (w >= b) {
        t = (fp_digit)(w / b);
        w -= ((fp_word)t) * ((fp_word)b);
      } else {
        t = 0;
      }
      if (c != NULL)
        q.dp[ix] = (fp_digit)t;
  }

  if (d != NULL) {
     *d = (fp_digit)w;
  }

  if (c != NULL) {
     fp_clamp(&q);
     fp_copy(&q, c);
  }

  return FP_OKAY;
}


/* c = a mod b, 0 <= c < b  */
static int fp_mod_d(fp_int *a, fp_digit b, fp_digit *c)
{
   return fp_div_d(a, b, NULL, c);
}

int mp_mod_d(fp_int *a, fp_digit b, fp_digit *c)
{
   return fp_mod_d(a, b, c);
}

#endif /* defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY) || defined(WOLFSSL_DEBUG_MATH) */

#ifdef WOLFSSL_KEY_GEN

static void fp_gcd(fp_int *a, fp_int *b, fp_int *c);
static void fp_lcm(fp_int *a, fp_int *b, fp_int *c);
static int  fp_isprime_ex(fp_int *a, int t);
static int  fp_isprime(fp_int *a);
static int  fp_randprime(fp_int* N, int len, WC_RNG* rng, void* heap);

int mp_gcd(fp_int *a, fp_int *b, fp_int *c)
{
    fp_gcd(a, b, c);
    return MP_OKAY;
}


int mp_lcm(fp_int *a, fp_int *b, fp_int *c)
{
    fp_lcm(a, b, c);
    return MP_OKAY;
}


int mp_prime_is_prime(mp_int* a, int t, int* result)
{
    (void)t;
    *result = fp_isprime(a);
    return MP_OKAY;
}

int mp_rand_prime(mp_int* N, int len, WC_RNG* rng, void* heap)
{
    int err;

    err = fp_randprime(N, len, rng, heap);
    switch(err) {
        case FP_VAL:
            return MP_VAL;
        case FP_MEM:
            return MP_MEM;
        default:
            break;
    }

    return MP_OKAY;
}

int mp_exch (mp_int * a, mp_int * b)
{
    fp_exch(a, b);
    return MP_OKAY;
}

/* Miller-Rabin test of "a" to the base of "b" as described in
 * HAC pp. 139 Algorithm 4.24
 *
 * Sets result to 0 if definitely composite or 1 if probably prime.
 * Randomly the chance of error is no more than 1/4 and often
 * very much lower.
 */
static void fp_prime_miller_rabin (fp_int * a, fp_int * b, int *result)
{
  fp_int  n1, y, r;
  int     s, j;

  /* default */
  *result = FP_NO;

  /* ensure b > 1 */
  if (fp_cmp_d(b, 1) != FP_GT) {
     return;
  }

  /* get n1 = a - 1 */
  fp_init_copy(&n1, a);
  fp_sub_d(&n1, 1, &n1);

  /* set 2**s * r = n1 */
  fp_init_copy(&r, &n1);

  /* count the number of least significant bits
   * which are zero
   */
  s = fp_cnt_lsb(&r);

  /* now divide n - 1 by 2**s */
  fp_div_2d (&r, s, &r, NULL);

  /* compute y = b**r mod a */
  fp_init(&y);
  fp_exptmod(b, &r, a, &y);

  /* if y != 1 and y != n1 do */
  if (fp_cmp_d (&y, 1) != FP_EQ && fp_cmp (&y, &n1) != FP_EQ) {
    j = 1;
    /* while j <= s-1 and y != n1 */
    while ((j <= (s - 1)) && fp_cmp (&y, &n1) != FP_EQ) {
      fp_sqrmod (&y, a, &y);

      /* if y == 1 then composite */
      if (fp_cmp_d (&y, 1) == FP_EQ) {
         return;
      }
      ++j;
    }

    /* if y != n1 then composite */
    if (fp_cmp (&y, &n1) != FP_EQ) {
       return;
    }
  }

  /* probably prime now */
  *result = FP_YES;
}


/* a few primes */
static const fp_digit primes[FP_PRIME_SIZE] = {
  0x0002, 0x0003, 0x0005, 0x0007, 0x000B, 0x000D, 0x0011, 0x0013,
  0x0017, 0x001D, 0x001F, 0x0025, 0x0029, 0x002B, 0x002F, 0x0035,
  0x003B, 0x003D, 0x0043, 0x0047, 0x0049, 0x004F, 0x0053, 0x0059,
  0x0061, 0x0065, 0x0067, 0x006B, 0x006D, 0x0071, 0x007F, 0x0083,
  0x0089, 0x008B, 0x0095, 0x0097, 0x009D, 0x00A3, 0x00A7, 0x00AD,
  0x00B3, 0x00B5, 0x00BF, 0x00C1, 0x00C5, 0x00C7, 0x00D3, 0x00DF,
  0x00E3, 0x00E5, 0x00E9, 0x00EF, 0x00F1, 0x00FB, 0x0101, 0x0107,
  0x010D, 0x010F, 0x0115, 0x0119, 0x011B, 0x0125, 0x0133, 0x0137,

  0x0139, 0x013D, 0x014B, 0x0151, 0x015B, 0x015D, 0x0161, 0x0167,
  0x016F, 0x0175, 0x017B, 0x017F, 0x0185, 0x018D, 0x0191, 0x0199,
  0x01A3, 0x01A5, 0x01AF, 0x01B1, 0x01B7, 0x01BB, 0x01C1, 0x01C9,
  0x01CD, 0x01CF, 0x01D3, 0x01DF, 0x01E7, 0x01EB, 0x01F3, 0x01F7,
  0x01FD, 0x0209, 0x020B, 0x021D, 0x0223, 0x022D, 0x0233, 0x0239,
  0x023B, 0x0241, 0x024B, 0x0251, 0x0257, 0x0259, 0x025F, 0x0265,
  0x0269, 0x026B, 0x0277, 0x0281, 0x0283, 0x0287, 0x028D, 0x0293,
  0x0295, 0x02A1, 0x02A5, 0x02AB, 0x02B3, 0x02BD, 0x02C5, 0x02CF,

  0x02D7, 0x02DD, 0x02E3, 0x02E7, 0x02EF, 0x02F5, 0x02F9, 0x0301,
  0x0305, 0x0313, 0x031D, 0x0329, 0x032B, 0x0335, 0x0337, 0x033B,
  0x033D, 0x0347, 0x0355, 0x0359, 0x035B, 0x035F, 0x036D, 0x0371,
  0x0373, 0x0377, 0x038B, 0x038F, 0x0397, 0x03A1, 0x03A9, 0x03AD,
  0x03B3, 0x03B9, 0x03C7, 0x03CB, 0x03D1, 0x03D7, 0x03DF, 0x03E5,
  0x03F1, 0x03F5, 0x03FB, 0x03FD, 0x0407, 0x0409, 0x040F, 0x0419,
  0x041B, 0x0425, 0x0427, 0x042D, 0x043F, 0x0443, 0x0445, 0x0449,
  0x044F, 0x0455, 0x045D, 0x0463, 0x0469, 0x047F, 0x0481, 0x048B,

  0x0493, 0x049D, 0x04A3, 0x04A9, 0x04B1, 0x04BD, 0x04C1, 0x04C7,
  0x04CD, 0x04CF, 0x04D5, 0x04E1, 0x04EB, 0x04FD, 0x04FF, 0x0503,
  0x0509, 0x050B, 0x0511, 0x0515, 0x0517, 0x051B, 0x0527, 0x0529,
  0x052F, 0x0551, 0x0557, 0x055D, 0x0565, 0x0577, 0x0581, 0x058F,
  0x0593, 0x0595, 0x0599, 0x059F, 0x05A7, 0x05AB, 0x05AD, 0x05B3,
  0x05BF, 0x05C9, 0x05CB, 0x05CF, 0x05D1, 0x05D5, 0x05DB, 0x05E7,
  0x05F3, 0x05FB, 0x0607, 0x060D, 0x0611, 0x0617, 0x061F, 0x0623,
  0x062B, 0x062F, 0x063D, 0x0641, 0x0647, 0x0649, 0x064D, 0x0653
};

int fp_isprime_ex(fp_int *a, int t)
{
   fp_int   b;
   fp_digit d;
   int      r, res;

   if (t <= 0 || t > FP_PRIME_SIZE) {
     return FP_NO;
   }

   /* do trial division */
   for (r = 0; r < FP_PRIME_SIZE; r++) {
       fp_mod_d(a, primes[r], &d);
       if (d == 0) {
          return FP_NO;
       }
   }

   /* now do 't' miller rabins */
   fp_init(&b);
   for (r = 0; r < t; r++) {
       fp_set(&b, primes[r]);
       fp_prime_miller_rabin(a, &b, &res);
       if (res == FP_NO) {
          return FP_NO;
       }
   }
   return FP_YES;
}

int fp_isprime(fp_int *a)
{
  return fp_isprime_ex(a, 8);
}

int fp_randprime(fp_int* N, int len, WC_RNG* rng, void* heap)
{
    static const int USE_BBS = 1;
    int   err, type;
    byte* buf;

    /* get type */
    if (len < 0) {
        type = USE_BBS;
        len = -len;
    } else {
        type = 0;
    }

    /* allow sizes between 2 and 512 bytes for a prime size */
    if (len < 2 || len > 512) {
        return FP_VAL;
    }

    /* allocate buffer to work with */
    buf = (byte*)XMALLOC(len, heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (buf == NULL) {
        return FP_MEM;
    }
    XMEMSET(buf, 0, len);

    do {
#ifdef SHOW_GEN
        printf(".");
        fflush(stdout);
#endif
        /* generate value */
        err = wc_RNG_GenerateBlock(rng, buf, len);
        if (err != 0) {
            XFREE(buf, heap, DYNAMIC_TYPE_TMP_BUFFER);
            return FP_VAL;
        }

        /* munge bits */
        buf[0]     |= 0x80 | 0x40;
        buf[len-1] |= 0x01 | ((type & USE_BBS) ? 0x02 : 0x00);

        /* load value */
        fp_read_unsigned_bin(N, buf, len);

        /* test */
    } while (fp_isprime(N) == FP_NO);

    XMEMSET(buf, 0, len);
    XFREE(buf, heap, DYNAMIC_TYPE_TMP_BUFFER);

    return FP_OKAY;
}

/* c = [a, b] */
void fp_lcm(fp_int *a, fp_int *b, fp_int *c)
{
   fp_int  t1, t2;

   fp_init(&t1);
   fp_init(&t2);
   fp_gcd(a, b, &t1);
   if (fp_cmp_mag(a, b) == FP_GT) {
      fp_div(a, &t1, &t2, NULL);
      fp_mul(b, &t2, c);
   } else {
      fp_div(b, &t1, &t2, NULL);
      fp_mul(a, &t2, c);
   }
}



/* c = (a, b) */
void fp_gcd(fp_int *a, fp_int *b, fp_int *c)
{
   fp_int u, v, r;

   /* either zero than gcd is the largest */
   if (fp_iszero (a) == FP_YES && fp_iszero (b) == FP_NO) {
     fp_abs (b, c);
     return;
   }
   if (fp_iszero (a) == FP_NO && fp_iszero (b) == FP_YES) {
     fp_abs (a, c);
     return;
   }

   /* optimized.  At this point if a == 0 then
    * b must equal zero too
    */
   if (fp_iszero (a) == FP_YES) {
     fp_zero(c);
     return;
   }

   /* sort inputs */
   if (fp_cmp_mag(a, b) != FP_LT) {
      fp_init_copy(&u, a);
      fp_init_copy(&v, b);
   } else {
      fp_init_copy(&u, b);
      fp_init_copy(&v, a);
   }

   fp_init(&r);
   while (fp_iszero(&v) == FP_NO) {
      fp_mod(&u, &v, &r);
      fp_copy(&v, &u);
      fp_copy(&r, &v);
   }
   fp_copy(&u, c);
}

#endif /* WOLFSSL_KEY_GEN */


#if defined(HAVE_ECC) || !defined(NO_PWDBASED) || defined(OPENSSL_EXTRA) || \
    defined(WC_RSA_BLINDING)
/* c = a + b */
void fp_add_d(fp_int *a, fp_digit b, fp_int *c)
{
   fp_int tmp;
   fp_init(&tmp);
   fp_set(&tmp, b);
   fp_add(a, &tmp, c);
}

/* external compatibility */
int mp_add_d(fp_int *a, fp_digit b, fp_int *c)
{
    fp_add_d(a, b, c);
    return MP_OKAY;
}

#endif  /* HAVE_ECC || !NO_PWDBASED */


#if defined(HAVE_ECC) || defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY)

/* chars used in radix conversions */
static const char *fp_s_rmap = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ\
                                abcdefghijklmnopqrstuvwxyz+/";
#endif

#ifdef HAVE_ECC
static int fp_read_radix(fp_int *a, const char *str, int radix)
{
  int     y, neg;
  char    ch;

  /* set the integer to the default of zero */
  fp_zero (a);

  /* make sure the radix is ok */
  if (radix < 2 || radix > 64) {
    return FP_VAL;
  }

  /* if the leading digit is a
   * minus set the sign to negative.
   */
  if (*str == '-') {
    ++str;
    neg = FP_NEG;
  } else {
    neg = FP_ZPOS;
  }

  /* process each digit of the string */
  while (*str) {
    /* if the radix <= 36 the conversion is case insensitive
     * this allows numbers like 1AB and 1ab to represent the same  value
     * [e.g. in hex]
     */
    ch = (char)((radix <= 36) ? XTOUPPER((unsigned char)*str) : *str);
    for (y = 0; y < 64; y++) {
      if (ch == fp_s_rmap[y]) {
         break;
      }
    }

    /* if the char was found in the map
     * and is less than the given radix add it
     * to the number, otherwise exit the loop.
     */
    if (y < radix) {
      fp_mul_d (a, (fp_digit) radix, a);
      fp_add_d (a, (fp_digit) y, a);
    } else {
      break;
    }
    ++str;
  }

  /* set the sign only if a != 0 */
  if (fp_iszero(a) != FP_YES) {
     a->sign = neg;
  }
  return FP_OKAY;
}

/* fast math conversion */
int mp_read_radix(mp_int *a, const char *str, int radix)
{
    return fp_read_radix(a, str, radix);
}

/* fast math conversion */
int mp_sqr(fp_int *A, fp_int *B)
{
    fp_sqr(A, B);
    return MP_OKAY;
}

/* fast math conversion */
int mp_montgomery_reduce(fp_int *a, fp_int *m, fp_digit mp)
{
    fp_montgomery_reduce(a, m, mp);
    return MP_OKAY;
}


/* fast math conversion */
int mp_montgomery_setup(fp_int *a, fp_digit *rho)
{
    return fp_montgomery_setup(a, rho);
}

int mp_div_2(fp_int * a, fp_int * b)
{
    fp_div_2(a, b);
    return MP_OKAY;
}


int mp_init_copy(fp_int * a, fp_int * b)
{
    fp_init_copy(a, b);
    return MP_OKAY;
}

#ifdef HAVE_COMP_KEY

int mp_cnt_lsb(fp_int* a)
{
    return fp_cnt_lsb(a);
}

#endif /* HAVE_COMP_KEY */

#endif /* HAVE_ECC */

#if defined(HAVE_ECC) || !defined(NO_RSA) || !defined(NO_DSA)
/* fast math conversion */
int mp_set(fp_int *a, fp_digit b)
{
    fp_set(a,b);
    return MP_OKAY;
}
#endif

#if defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY) || \
    defined(WOLFSSL_DEBUG_MATH)

/* returns size of ASCII representation */
int mp_radix_size (mp_int *a, int radix, int *size)
{
    int     res, digs;
    fp_int  t;
    fp_digit d;

    *size = 0;

    /* special case for binary */
    if (radix == 2) {
        *size = fp_count_bits (a) + (a->sign == FP_NEG ? 1 : 0) + 1;
        return FP_YES;
    }

    /* make sure the radix is in range */
    if (radix < 2 || radix > 64) {
        return FP_VAL;
    }

    if (fp_iszero(a) == MP_YES) {
        *size = 2;
        return FP_OKAY;
    }

    /* digs is the digit count */
    digs = 0;

    /* if it's negative add one for the sign */
    if (a->sign == FP_NEG) {
        ++digs;
    }

    /* init a copy of the input */
    fp_init_copy (&t, a);

    /* force temp to positive */
    t.sign = FP_ZPOS;

    /* fetch out all of the digits */
    while (fp_iszero (&t) == FP_NO) {
        if ((res = fp_div_d (&t, (mp_digit) radix, &t, &d)) != FP_OKAY) {
            fp_zero (&t);
            return res;
        }
        ++digs;
    }
    fp_zero (&t);

    /* return digs + 1, the 1 is for the NULL byte that would be required. */
    *size = digs + 1;
    return FP_OKAY;
}

/* stores a bignum as a ASCII string in a given radix (2..64) */
int mp_toradix (mp_int *a, char *str, int radix)
{
    int     res, digs;
    fp_int  t;
    fp_digit d;
    char   *_s = str;

    /* check range of the radix */
    if (radix < 2 || radix > 64) {
        return FP_VAL;
    }

    /* quick out if its zero */
    if (fp_iszero(a) == FP_YES) {
        *str++ = '0';
        *str = '\0';
        return FP_YES;
    }

    /* init a copy of the input */
    fp_init_copy (&t, a);

    /* if it is negative output a - */
    if (t.sign == FP_NEG) {
        ++_s;
        *str++ = '-';
        t.sign = FP_ZPOS;
    }

    digs = 0;
    while (fp_iszero (&t) == FP_NO) {
        if ((res = fp_div_d (&t, (fp_digit) radix, &t, &d)) != FP_OKAY) {
            fp_zero (&t);
            return res;
        }
        *str++ = fp_s_rmap[d];
        ++digs;
    }

    /* reverse the digits of the string.  In this case _s points
     * to the first digit [excluding the sign] of the number]
     */
    fp_reverse ((unsigned char *)_s, digs);

    /* append a NULL so the string is properly terminated */
    *str = '\0';

    fp_zero (&t);
    return FP_OKAY;
}

#ifdef WOLFSSL_DEBUG_MATH
void mp_dump(const char* desc, mp_int* a, byte verbose)
{
  char buffer[FP_SIZE * sizeof(fp_digit) * 2];
  int size = FP_SIZE;

#if defined(ALT_ECC_SIZE) || defined(HAVE_WOLF_BIGINT)
  size = a->size;
#endif

  printf("%s: ptr=%p, used=%d, sign=%d, size=%d, fpd=%d\n",
    desc, a, a->used, a->sign, size, (int)sizeof(fp_digit));

  mp_toradix(a, buffer, 16);
  printf("  %s\n  ", buffer);

  if (verbose) {
    int i;
    for(i=0; i<size * (int)sizeof(fp_digit); i++) {
      printf("%x ", *(((byte*)a->dp) + i));
    }
    printf("\n");
  }
}
#endif /* WOLFSSL_DEBUG_MATH */

#endif /* defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY) || defined(WOLFSSL_DEBUG_MATH) */


int mp_lshd (mp_int * a, int b)
{
    fp_lshd(a, b);
    return FP_OKAY;
}

#endif /* USE_FAST_MATH */
