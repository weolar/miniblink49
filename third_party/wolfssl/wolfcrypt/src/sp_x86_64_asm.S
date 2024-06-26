/* sp_x86_64_asm
 *
 * Copyright (C) 2006-2019 wolfSSL Inc.
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

#define HAVE_INTEL_AVX2
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_2048_mul_16
.type	sp_2048_mul_16,@function
.align	16
sp_2048_mul_16:
#else
.globl	_sp_2048_mul_16
.p2align	4
_sp_2048_mul_16:
#endif /* __APPLE__ */
        movq	%rdx, %rcx
        subq	$128, %rsp
        # A[0] * B[0]
        movq	(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        movq	%rax, (%rsp)
        movq	%rdx, %r9
        # A[0] * B[1]
        movq	8(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[0]
        movq	(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 8(%rsp)
        # A[0] * B[2]
        movq	16(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[1]
        movq	8(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[0]
        movq	(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 16(%rsp)
        # A[0] * B[3]
        movq	24(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[1] * B[2]
        movq	16(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[1]
        movq	8(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[0]
        movq	(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 24(%rsp)
        # A[0] * B[4]
        movq	32(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[3]
        movq	24(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B[2]
        movq	16(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[1]
        movq	8(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[4] * B[0]
        movq	(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 32(%rsp)
        # A[0] * B[5]
        movq	40(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[4]
        movq	32(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[3]
        movq	24(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B[2]
        movq	16(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[4] * B[1]
        movq	8(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[5] * B[0]
        movq	(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 40(%rsp)
        # A[0] * B[6]
        movq	48(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[1] * B[5]
        movq	40(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[4]
        movq	32(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[3]
        movq	24(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B[2]
        movq	16(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[5] * B[1]
        movq	8(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[6] * B[0]
        movq	(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 48(%rsp)
        # A[0] * B[7]
        movq	56(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[6]
        movq	48(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B[5]
        movq	40(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[4]
        movq	32(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[4] * B[3]
        movq	24(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B[2]
        movq	16(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[6] * B[1]
        movq	8(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[7] * B[0]
        movq	(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 56(%rsp)
        # A[0] * B[8]
        movq	64(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[7]
        movq	56(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[6]
        movq	48(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B[5]
        movq	40(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[4] * B[4]
        movq	32(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[5] * B[3]
        movq	24(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B[2]
        movq	16(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[7] * B[1]
        movq	8(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[8] * B[0]
        movq	(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 64(%rsp)
        # A[0] * B[9]
        movq	72(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[1] * B[8]
        movq	64(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[7]
        movq	56(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[6]
        movq	48(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B[5]
        movq	40(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[5] * B[4]
        movq	32(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[6] * B[3]
        movq	24(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B[2]
        movq	16(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[8] * B[1]
        movq	8(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[9] * B[0]
        movq	(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 72(%rsp)
        # A[0] * B[10]
        movq	80(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[9]
        movq	72(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B[8]
        movq	64(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[7]
        movq	56(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[4] * B[6]
        movq	48(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B[5]
        movq	40(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[6] * B[4]
        movq	32(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[7] * B[3]
        movq	24(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B[2]
        movq	16(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[9] * B[1]
        movq	8(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[10] * B[0]
        movq	(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 80(%rsp)
        # A[0] * B[11]
        movq	88(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[10]
        movq	80(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[9]
        movq	72(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B[8]
        movq	64(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[4] * B[7]
        movq	56(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[5] * B[6]
        movq	48(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B[5]
        movq	40(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[7] * B[4]
        movq	32(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[8] * B[3]
        movq	24(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B[2]
        movq	16(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[10] * B[1]
        movq	8(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[11] * B[0]
        movq	(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 88(%rsp)
        # A[0] * B[12]
        movq	96(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[1] * B[11]
        movq	88(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[10]
        movq	80(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[9]
        movq	72(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B[8]
        movq	64(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[5] * B[7]
        movq	56(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[6] * B[6]
        movq	48(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B[5]
        movq	40(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[8] * B[4]
        movq	32(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[9] * B[3]
        movq	24(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B[2]
        movq	16(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[11] * B[1]
        movq	8(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[12] * B[0]
        movq	(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 96(%rsp)
        # A[0] * B[13]
        movq	104(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[12]
        movq	96(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B[11]
        movq	88(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[10]
        movq	80(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[4] * B[9]
        movq	72(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B[8]
        movq	64(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[6] * B[7]
        movq	56(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[7] * B[6]
        movq	48(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B[5]
        movq	40(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[9] * B[4]
        movq	32(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[10] * B[3]
        movq	24(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B[2]
        movq	16(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[12] * B[1]
        movq	8(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[13] * B[0]
        movq	(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 104(%rsp)
        # A[0] * B[14]
        movq	112(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[13]
        movq	104(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[12]
        movq	96(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B[11]
        movq	88(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[4] * B[10]
        movq	80(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[5] * B[9]
        movq	72(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B[8]
        movq	64(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[7] * B[7]
        movq	56(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[8] * B[6]
        movq	48(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B[5]
        movq	40(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[10] * B[4]
        movq	32(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[11] * B[3]
        movq	24(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B[2]
        movq	16(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * B[1]
        movq	8(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[14] * B[0]
        movq	(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 112(%rsp)
        # A[0] * B[15]
        movq	120(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[1] * B[14]
        movq	112(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[13]
        movq	104(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[12]
        movq	96(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B[11]
        movq	88(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[5] * B[10]
        movq	80(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[6] * B[9]
        movq	72(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B[8]
        movq	64(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[8] * B[7]
        movq	56(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[9] * B[6]
        movq	48(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B[5]
        movq	40(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[11] * B[4]
        movq	32(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[12] * B[3]
        movq	24(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B[2]
        movq	16(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[14] * B[1]
        movq	8(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[15] * B[0]
        movq	(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 120(%rsp)
        # A[1] * B[15]
        movq	120(%rcx), %rax
        mulq	8(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B[14]
        movq	112(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[13]
        movq	104(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[4] * B[12]
        movq	96(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B[11]
        movq	88(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[6] * B[10]
        movq	80(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[7] * B[9]
        movq	72(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B[8]
        movq	64(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[9] * B[7]
        movq	56(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[10] * B[6]
        movq	48(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B[5]
        movq	40(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[12] * B[4]
        movq	32(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[13] * B[3]
        movq	24(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B[2]
        movq	16(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[15] * B[1]
        movq	8(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 128(%rdi)
        # A[2] * B[15]
        movq	120(%rcx), %rax
        mulq	16(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B[14]
        movq	112(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[4] * B[13]
        movq	104(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[5] * B[12]
        movq	96(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B[11]
        movq	88(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[7] * B[10]
        movq	80(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[8] * B[9]
        movq	72(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B[8]
        movq	64(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[10] * B[7]
        movq	56(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[11] * B[6]
        movq	48(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B[5]
        movq	40(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * B[4]
        movq	32(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[14] * B[3]
        movq	24(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B[2]
        movq	16(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 136(%rdi)
        # A[3] * B[15]
        movq	120(%rcx), %rax
        mulq	24(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B[14]
        movq	112(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[5] * B[13]
        movq	104(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[6] * B[12]
        movq	96(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B[11]
        movq	88(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[8] * B[10]
        movq	80(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[9] * B[9]
        movq	72(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B[8]
        movq	64(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[11] * B[7]
        movq	56(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[12] * B[6]
        movq	48(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B[5]
        movq	40(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[14] * B[4]
        movq	32(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[15] * B[3]
        movq	24(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 144(%rdi)
        # A[4] * B[15]
        movq	120(%rcx), %rax
        mulq	32(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B[14]
        movq	112(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[6] * B[13]
        movq	104(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[7] * B[12]
        movq	96(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B[11]
        movq	88(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[9] * B[10]
        movq	80(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[10] * B[9]
        movq	72(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B[8]
        movq	64(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[12] * B[7]
        movq	56(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[13] * B[6]
        movq	48(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B[5]
        movq	40(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[15] * B[4]
        movq	32(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 152(%rdi)
        # A[5] * B[15]
        movq	120(%rcx), %rax
        mulq	40(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B[14]
        movq	112(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[7] * B[13]
        movq	104(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[8] * B[12]
        movq	96(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B[11]
        movq	88(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[10] * B[10]
        movq	80(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[11] * B[9]
        movq	72(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B[8]
        movq	64(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * B[7]
        movq	56(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[14] * B[6]
        movq	48(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B[5]
        movq	40(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 160(%rdi)
        # A[6] * B[15]
        movq	120(%rcx), %rax
        mulq	48(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B[14]
        movq	112(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[8] * B[13]
        movq	104(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[9] * B[12]
        movq	96(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B[11]
        movq	88(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[11] * B[10]
        movq	80(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[12] * B[9]
        movq	72(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B[8]
        movq	64(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[14] * B[7]
        movq	56(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[15] * B[6]
        movq	48(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 168(%rdi)
        # A[7] * B[15]
        movq	120(%rcx), %rax
        mulq	56(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B[14]
        movq	112(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[9] * B[13]
        movq	104(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[10] * B[12]
        movq	96(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B[11]
        movq	88(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[12] * B[10]
        movq	80(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[13] * B[9]
        movq	72(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B[8]
        movq	64(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[15] * B[7]
        movq	56(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 176(%rdi)
        # A[8] * B[15]
        movq	120(%rcx), %rax
        mulq	64(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B[14]
        movq	112(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[10] * B[13]
        movq	104(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[11] * B[12]
        movq	96(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B[11]
        movq	88(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * B[10]
        movq	80(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[14] * B[9]
        movq	72(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B[8]
        movq	64(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 184(%rdi)
        # A[9] * B[15]
        movq	120(%rcx), %rax
        mulq	72(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B[14]
        movq	112(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[11] * B[13]
        movq	104(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[12] * B[12]
        movq	96(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B[11]
        movq	88(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[14] * B[10]
        movq	80(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[15] * B[9]
        movq	72(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 192(%rdi)
        # A[10] * B[15]
        movq	120(%rcx), %rax
        mulq	80(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B[14]
        movq	112(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[12] * B[13]
        movq	104(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[13] * B[12]
        movq	96(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B[11]
        movq	88(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[15] * B[10]
        movq	80(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 200(%rdi)
        # A[11] * B[15]
        movq	120(%rcx), %rax
        mulq	88(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B[14]
        movq	112(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * B[13]
        movq	104(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[14] * B[12]
        movq	96(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B[11]
        movq	88(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 208(%rdi)
        # A[12] * B[15]
        movq	120(%rcx), %rax
        mulq	96(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B[14]
        movq	112(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[14] * B[13]
        movq	104(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[15] * B[12]
        movq	96(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 216(%rdi)
        # A[13] * B[15]
        movq	120(%rcx), %rax
        mulq	104(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B[14]
        movq	112(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[15] * B[13]
        movq	104(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 224(%rdi)
        # A[14] * B[15]
        movq	120(%rcx), %rax
        mulq	112(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B[14]
        movq	112(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 232(%rdi)
        # A[15] * B[15]
        movq	120(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        movq	%r8, 240(%rdi)
        movq	%r9, 248(%rdi)
        movq	(%rsp), %rax
        movq	8(%rsp), %rdx
        movq	16(%rsp), %r8
        movq	24(%rsp), %r9
        movq	%rax, (%rdi)
        movq	%rdx, 8(%rdi)
        movq	%r8, 16(%rdi)
        movq	%r9, 24(%rdi)
        movq	32(%rsp), %rax
        movq	40(%rsp), %rdx
        movq	48(%rsp), %r8
        movq	56(%rsp), %r9
        movq	%rax, 32(%rdi)
        movq	%rdx, 40(%rdi)
        movq	%r8, 48(%rdi)
        movq	%r9, 56(%rdi)
        movq	64(%rsp), %rax
        movq	72(%rsp), %rdx
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	%rax, 64(%rdi)
        movq	%rdx, 72(%rdi)
        movq	%r8, 80(%rdi)
        movq	%r9, 88(%rdi)
        movq	96(%rsp), %rax
        movq	104(%rsp), %rdx
        movq	112(%rsp), %r8
        movq	120(%rsp), %r9
        movq	%rax, 96(%rdi)
        movq	%rdx, 104(%rdi)
        movq	%r8, 112(%rdi)
        movq	%r9, 120(%rdi)
        addq	$128, %rsp
        repz retq
#ifndef __APPLE__
.size	sp_2048_mul_16,.-sp_2048_mul_16
#endif /* __APPLE__ */
/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_2048_sqr_16
.type	sp_2048_sqr_16,@function
.align	16
sp_2048_sqr_16:
#else
.globl	_sp_2048_sqr_16
.p2align	4
_sp_2048_sqr_16:
#endif /* __APPLE__ */
        push	%r12
        subq	$128, %rsp
        # A[0] * A[0]
        movq	(%rsi), %rax
        mulq	%rax
        xorq	%r9, %r9
        movq	%rax, (%rsp)
        movq	%rdx, %r8
        # A[0] * A[1]
        movq	8(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        movq	%r8, 8(%rsp)
        # A[0] * A[2]
        movq	16(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        # A[1] * A[1]
        movq	8(%rsi), %rax
        mulq	%rax
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        movq	%r9, 16(%rsp)
        # A[0] * A[3]
        movq	24(%rsi), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * A[2]
        movq	16(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%rcx, 24(%rsp)
        # A[0] * A[4]
        movq	32(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        # A[1] * A[3]
        movq	24(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        # A[2] * A[2]
        movq	16(%rsi), %rax
        mulq	%rax
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        movq	%r8, 32(%rsp)
        # A[0] * A[5]
        movq	40(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[4]
        movq	32(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[3]
        movq	24(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 40(%rsp)
        # A[0] * A[6]
        movq	48(%rsi), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[5]
        movq	40(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[4]
        movq	32(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[3]
        movq	24(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 48(%rsp)
        # A[0] * A[7]
        movq	56(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[6]
        movq	48(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[5]
        movq	40(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[4]
        movq	32(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 56(%rsp)
        # A[0] * A[8]
        movq	64(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[7]
        movq	56(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[6]
        movq	48(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[5]
        movq	40(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[4]
        movq	32(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 64(%rsp)
        # A[0] * A[9]
        movq	72(%rsi), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[8]
        movq	64(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[7]
        movq	56(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[6]
        movq	48(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[5]
        movq	40(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 72(%rsp)
        # A[0] * A[10]
        movq	80(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[9]
        movq	72(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[8]
        movq	64(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[7]
        movq	56(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[6]
        movq	48(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[5]
        movq	40(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 80(%rsp)
        # A[0] * A[11]
        movq	88(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[10]
        movq	80(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[9]
        movq	72(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[8]
        movq	64(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[7]
        movq	56(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[6]
        movq	48(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 88(%rsp)
        # A[0] * A[12]
        movq	96(%rsi), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[11]
        movq	88(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[10]
        movq	80(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[9]
        movq	72(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[8]
        movq	64(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[7]
        movq	56(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[6]
        movq	48(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 96(%rsp)
        # A[0] * A[13]
        movq	104(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[12]
        movq	96(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[11]
        movq	88(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[10]
        movq	80(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[9]
        movq	72(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[8]
        movq	64(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[7]
        movq	56(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 104(%rsp)
        # A[0] * A[14]
        movq	112(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[13]
        movq	104(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[12]
        movq	96(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[11]
        movq	88(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[10]
        movq	80(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[9]
        movq	72(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[8]
        movq	64(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[7]
        movq	56(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 112(%rsp)
        # A[0] * A[15]
        movq	120(%rsi), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[14]
        movq	112(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[13]
        movq	104(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[12]
        movq	96(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[11]
        movq	88(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[10]
        movq	80(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[9]
        movq	72(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[8]
        movq	64(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 120(%rsp)
        # A[1] * A[15]
        movq	120(%rsi), %rax
        mulq	8(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[2] * A[14]
        movq	112(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[13]
        movq	104(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[12]
        movq	96(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[11]
        movq	88(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[10]
        movq	80(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[9]
        movq	72(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[8]
        movq	64(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 128(%rdi)
        # A[2] * A[15]
        movq	120(%rsi), %rax
        mulq	16(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[3] * A[14]
        movq	112(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[13]
        movq	104(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[12]
        movq	96(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[11]
        movq	88(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[10]
        movq	80(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[9]
        movq	72(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 136(%rdi)
        # A[3] * A[15]
        movq	120(%rsi), %rax
        mulq	24(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[4] * A[14]
        movq	112(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[13]
        movq	104(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[12]
        movq	96(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[11]
        movq	88(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[10]
        movq	80(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[9]
        movq	72(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 144(%rdi)
        # A[4] * A[15]
        movq	120(%rsi), %rax
        mulq	32(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[5] * A[14]
        movq	112(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[13]
        movq	104(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[12]
        movq	96(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[11]
        movq	88(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[10]
        movq	80(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 152(%rdi)
        # A[5] * A[15]
        movq	120(%rsi), %rax
        mulq	40(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[6] * A[14]
        movq	112(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[13]
        movq	104(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[12]
        movq	96(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[11]
        movq	88(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[10]
        movq	80(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 160(%rdi)
        # A[6] * A[15]
        movq	120(%rsi), %rax
        mulq	48(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[7] * A[14]
        movq	112(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[13]
        movq	104(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[12]
        movq	96(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[11]
        movq	88(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 168(%rdi)
        # A[7] * A[15]
        movq	120(%rsi), %rax
        mulq	56(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[8] * A[14]
        movq	112(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[13]
        movq	104(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[12]
        movq	96(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[11]
        movq	88(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 176(%rdi)
        # A[8] * A[15]
        movq	120(%rsi), %rax
        mulq	64(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[9] * A[14]
        movq	112(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[13]
        movq	104(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[12]
        movq	96(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 184(%rdi)
        # A[9] * A[15]
        movq	120(%rsi), %rax
        mulq	72(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[10] * A[14]
        movq	112(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[13]
        movq	104(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[12] * A[12]
        movq	96(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 192(%rdi)
        # A[10] * A[15]
        movq	120(%rsi), %rax
        mulq	80(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[11] * A[14]
        movq	112(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[12] * A[13]
        movq	104(%rsi), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 200(%rdi)
        # A[11] * A[15]
        movq	120(%rsi), %rax
        mulq	88(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        # A[12] * A[14]
        movq	112(%rsi), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        # A[13] * A[13]
        movq	104(%rsi), %rax
        mulq	%rax
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        movq	%r9, 208(%rdi)
        # A[12] * A[15]
        movq	120(%rsi), %rax
        mulq	96(%rsi)
        xorq	%r9, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * A[14]
        movq	112(%rsi), %rax
        mulq	104(%rsi)
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%rcx, 216(%rdi)
        # A[13] * A[15]
        movq	120(%rsi), %rax
        mulq	104(%rsi)
        xorq	%rcx, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        # A[14] * A[14]
        movq	112(%rsi), %rax
        mulq	%rax
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        movq	%r8, 224(%rdi)
        # A[14] * A[15]
        movq	120(%rsi), %rax
        mulq	112(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        movq	%r9, 232(%rdi)
        # A[15] * A[15]
        movq	120(%rsi), %rax
        mulq	%rax
        addq	%rax, %rcx
        adcq	%rdx, %r8
        movq	%rcx, 240(%rdi)
        movq	%r8, 248(%rdi)
        movq	(%rsp), %rax
        movq	8(%rsp), %rdx
        movq	16(%rsp), %r10
        movq	24(%rsp), %r11
        movq	%rax, (%rdi)
        movq	%rdx, 8(%rdi)
        movq	%r10, 16(%rdi)
        movq	%r11, 24(%rdi)
        movq	32(%rsp), %rax
        movq	40(%rsp), %rdx
        movq	48(%rsp), %r10
        movq	56(%rsp), %r11
        movq	%rax, 32(%rdi)
        movq	%rdx, 40(%rdi)
        movq	%r10, 48(%rdi)
        movq	%r11, 56(%rdi)
        movq	64(%rsp), %rax
        movq	72(%rsp), %rdx
        movq	80(%rsp), %r10
        movq	88(%rsp), %r11
        movq	%rax, 64(%rdi)
        movq	%rdx, 72(%rdi)
        movq	%r10, 80(%rdi)
        movq	%r11, 88(%rdi)
        movq	96(%rsp), %rax
        movq	104(%rsp), %rdx
        movq	112(%rsp), %r10
        movq	120(%rsp), %r11
        movq	%rax, 96(%rdi)
        movq	%rdx, 104(%rdi)
        movq	%r10, 112(%rdi)
        movq	%r11, 120(%rdi)
        addq	$128, %rsp
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_2048_sqr_16,.-sp_2048_sqr_16
#endif /* __APPLE__ */
#ifdef HAVE_INTEL_AVX2
/* Multiply a and b into r. (r = a * b)
 *
 * r   Result of multiplication.
 * a   First number to multiply.
 * b   Second number to multiply.
 */
#ifndef __APPLE__
.globl	sp_2048_mul_avx2_16
.type	sp_2048_mul_avx2_16,@function
.align	16
sp_2048_mul_avx2_16:
#else
.globl	_sp_2048_mul_avx2_16
.p2align	4
_sp_2048_mul_avx2_16:
#endif /* __APPLE__ */
        push	%rbp
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        push	%rbx
        movq	%rdx, %rbp
        subq	$128, %rsp
        movq	$0, %r14
        movq	(%rsi), %rdx
        # A[0] * B[0]
        mulx	(%rbp), %r8, %r9
        # A[0] * B[1]
        mulx	8(%rbp), %rax, %r10
        movq	%r8, (%rsp)
        adcxq	%rax, %r9
        # A[0] * B[2]
        mulx	16(%rbp), %rax, %r11
        movq	%r9, 8(%rsp)
        adcxq	%rax, %r10
        # A[0] * B[3]
        mulx	24(%rbp), %rax, %r12
        movq	%r10, 16(%rsp)
        adcxq	%rax, %r11
        movq	%r11, 24(%rsp)
        # A[0] * B[4]
        mulx	32(%rbp), %rax, %r8
        adcxq	%rax, %r12
        # A[0] * B[5]
        mulx	40(%rbp), %rax, %r9
        movq	%r12, 32(%rsp)
        adcxq	%rax, %r8
        # A[0] * B[6]
        mulx	48(%rbp), %rax, %r10
        movq	%r8, 40(%rsp)
        adcxq	%rax, %r9
        # A[0] * B[7]
        mulx	56(%rbp), %rax, %r11
        movq	%r9, 48(%rsp)
        adcxq	%rax, %r10
        movq	%r10, 56(%rsp)
        # A[0] * B[8]
        mulx	64(%rbp), %rax, %r12
        adcxq	%rax, %r11
        # A[0] * B[9]
        mulx	72(%rbp), %rax, %r8
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        # A[0] * B[10]
        mulx	80(%rbp), %rax, %r9
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        # A[0] * B[11]
        mulx	88(%rbp), %rax, %r10
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        movq	%r9, 88(%rsp)
        # A[0] * B[12]
        mulx	96(%rbp), %rax, %r11
        adcxq	%rax, %r10
        # A[0] * B[13]
        mulx	104(%rbp), %rax, %r12
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        # A[0] * B[14]
        mulx	112(%rbp), %rax, %r8
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        # A[0] * B[15]
        mulx	120(%rbp), %rax, %r9
        movq	%r12, %r15
        adcxq	%rax, %r8
        adcxq	%r14, %r9
        movq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r8, %rbx
        movq	%r9, 128(%rdi)
        movq	8(%rsi), %rdx
        movq	8(%rsp), %r9
        movq	16(%rsp), %r10
        movq	24(%rsp), %r11
        movq	32(%rsp), %r12
        movq	40(%rsp), %r8
        # A[1] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[1] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r9, 8(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[1] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r10, 16(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[1] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r11, 24(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 32(%rsp)
        movq	48(%rsp), %r9
        movq	56(%rsp), %r10
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        # A[1] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[1] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r8, 40(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[1] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r9, 48(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[1] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r10, 56(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 64(%rsp)
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        # A[1] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[1] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[1] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[1] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 96(%rsp)
        movq	%r15, %r12
        movq	%rbx, %r8
        movq	128(%rdi), %r9
        # A[1] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[1] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[1] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r12, %r15
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[1] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r8, %rbx
        movq	%r14, %r10
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        adcxq	%r13, %r10
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r9, 128(%rdi)
        movq	%r10, 136(%rdi)
        movq	16(%rsi), %rdx
        movq	16(%rsp), %r10
        movq	24(%rsp), %r11
        movq	32(%rsp), %r12
        movq	40(%rsp), %r8
        movq	48(%rsp), %r9
        # A[2] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[2] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r10, 16(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[2] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r11, 24(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[2] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r12, 32(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 40(%rsp)
        movq	56(%rsp), %r10
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        # A[2] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[2] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r9, 48(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[2] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r10, 56(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[2] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 72(%rsp)
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	%r15, %r12
        # A[2] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[2] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[2] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[2] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 104(%rsp)
        movq	%rbx, %r8
        movq	128(%rdi), %r9
        movq	136(%rdi), %r10
        # A[2] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[2] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r12, %r15
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[2] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r8, %rbx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[2] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r9, 128(%rdi)
        movq	%r14, %r11
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        adcxq	%r13, %r11
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r10, 136(%rdi)
        movq	%r11, 144(%rdi)
        movq	24(%rsi), %rdx
        movq	24(%rsp), %r11
        movq	32(%rsp), %r12
        movq	40(%rsp), %r8
        movq	48(%rsp), %r9
        movq	56(%rsp), %r10
        # A[3] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[3] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r11, 24(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[3] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r12, 32(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[3] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r8, 40(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 48(%rsp)
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        # A[3] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[3] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r10, 56(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[3] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[3] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 80(%rsp)
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	%r15, %r12
        movq	%rbx, %r8
        # A[3] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[3] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[3] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[3] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, %r15
        movq	128(%rdi), %r9
        movq	136(%rdi), %r10
        movq	144(%rdi), %r11
        # A[3] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[3] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r8, %rbx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[3] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r9, 128(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[3] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r10, 136(%rdi)
        movq	%r14, %r12
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        adcxq	%r13, %r12
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r11, 144(%rdi)
        movq	%r12, 152(%rdi)
        movq	32(%rsi), %rdx
        movq	32(%rsp), %r12
        movq	40(%rsp), %r8
        movq	48(%rsp), %r9
        movq	56(%rsp), %r10
        movq	64(%rsp), %r11
        # A[4] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[4] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r12, 32(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[4] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r8, 40(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[4] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r9, 48(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 56(%rsp)
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        # A[4] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[4] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[4] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[4] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 88(%rsp)
        movq	104(%rsp), %r11
        movq	%r15, %r12
        movq	%rbx, %r8
        movq	128(%rdi), %r9
        # A[4] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[4] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[4] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[4] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r12, %r15
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, %rbx
        movq	136(%rdi), %r10
        movq	144(%rdi), %r11
        movq	152(%rdi), %r12
        # A[4] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[4] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r9, 128(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[4] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r10, 136(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[4] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r11, 144(%rdi)
        movq	%r14, %r8
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        adcxq	%r13, %r8
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r12, 152(%rdi)
        movq	%r8, 160(%rdi)
        movq	40(%rsi), %rdx
        movq	40(%rsp), %r8
        movq	48(%rsp), %r9
        movq	56(%rsp), %r10
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        # A[5] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[5] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r8, 40(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[5] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r9, 48(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[5] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r10, 56(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 64(%rsp)
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        # A[5] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[5] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[5] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[5] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 96(%rsp)
        movq	%r15, %r12
        movq	%rbx, %r8
        movq	128(%rdi), %r9
        movq	136(%rdi), %r10
        # A[5] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[5] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[5] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r12, %r15
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[5] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r8, %rbx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 128(%rdi)
        movq	144(%rdi), %r11
        movq	152(%rdi), %r12
        movq	160(%rdi), %r8
        # A[5] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[5] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r10, 136(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[5] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r11, 144(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[5] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r12, 152(%rdi)
        movq	%r14, %r9
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        adcxq	%r13, %r9
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r8, 160(%rdi)
        movq	%r9, 168(%rdi)
        movq	48(%rsi), %rdx
        movq	48(%rsp), %r9
        movq	56(%rsp), %r10
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        # A[6] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[6] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r9, 48(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[6] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r10, 56(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[6] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 72(%rsp)
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	%r15, %r12
        # A[6] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[6] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[6] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[6] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 104(%rsp)
        movq	%rbx, %r8
        movq	128(%rdi), %r9
        movq	136(%rdi), %r10
        movq	144(%rdi), %r11
        # A[6] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[6] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r12, %r15
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[6] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r8, %rbx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[6] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r9, 128(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 136(%rdi)
        movq	152(%rdi), %r12
        movq	160(%rdi), %r8
        movq	168(%rdi), %r9
        # A[6] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[6] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r11, 144(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[6] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r12, 152(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[6] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r8, 160(%rdi)
        movq	%r14, %r10
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        adcxq	%r13, %r10
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r9, 168(%rdi)
        movq	%r10, 176(%rdi)
        movq	56(%rsi), %rdx
        movq	56(%rsp), %r10
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        # A[7] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[7] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r10, 56(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[7] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[7] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 80(%rsp)
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	%r15, %r12
        movq	%rbx, %r8
        # A[7] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[7] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[7] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[7] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, %r15
        movq	128(%rdi), %r9
        movq	136(%rdi), %r10
        movq	144(%rdi), %r11
        movq	152(%rdi), %r12
        # A[7] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[7] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r8, %rbx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[7] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r9, 128(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[7] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r10, 136(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 144(%rdi)
        movq	160(%rdi), %r8
        movq	168(%rdi), %r9
        movq	176(%rdi), %r10
        # A[7] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[7] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r12, 152(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[7] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r8, 160(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[7] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r9, 168(%rdi)
        movq	%r14, %r11
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        adcxq	%r13, %r11
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r10, 176(%rdi)
        movq	%r11, 184(%rdi)
        movq	64(%rsi), %rdx
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        # A[8] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[8] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[8] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[8] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 88(%rsp)
        movq	104(%rsp), %r11
        movq	%r15, %r12
        movq	%rbx, %r8
        movq	128(%rdi), %r9
        # A[8] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[8] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[8] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[8] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r12, %r15
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, %rbx
        movq	136(%rdi), %r10
        movq	144(%rdi), %r11
        movq	152(%rdi), %r12
        movq	160(%rdi), %r8
        # A[8] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[8] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r9, 128(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[8] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r10, 136(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[8] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r11, 144(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 152(%rdi)
        movq	168(%rdi), %r9
        movq	176(%rdi), %r10
        movq	184(%rdi), %r11
        # A[8] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[8] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r8, 160(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[8] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r9, 168(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[8] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r10, 176(%rdi)
        movq	%r14, %r12
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        adcxq	%r13, %r12
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r11, 184(%rdi)
        movq	%r12, 192(%rdi)
        movq	72(%rsi), %rdx
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        # A[9] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[9] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[9] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[9] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 96(%rsp)
        movq	%r15, %r12
        movq	%rbx, %r8
        movq	128(%rdi), %r9
        movq	136(%rdi), %r10
        # A[9] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[9] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[9] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r12, %r15
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[9] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r8, %rbx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 128(%rdi)
        movq	144(%rdi), %r11
        movq	152(%rdi), %r12
        movq	160(%rdi), %r8
        movq	168(%rdi), %r9
        # A[9] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[9] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r10, 136(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[9] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r11, 144(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[9] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r12, 152(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 160(%rdi)
        movq	176(%rdi), %r10
        movq	184(%rdi), %r11
        movq	192(%rdi), %r12
        # A[9] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[9] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r9, 168(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[9] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r10, 176(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[9] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r11, 184(%rdi)
        movq	%r14, %r8
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        adcxq	%r13, %r8
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r12, 192(%rdi)
        movq	%r8, 200(%rdi)
        movq	80(%rsi), %rdx
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	%r15, %r12
        # A[10] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[10] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[10] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[10] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 104(%rsp)
        movq	%rbx, %r8
        movq	128(%rdi), %r9
        movq	136(%rdi), %r10
        movq	144(%rdi), %r11
        # A[10] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[10] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r12, %r15
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[10] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r8, %rbx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[10] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r9, 128(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 136(%rdi)
        movq	152(%rdi), %r12
        movq	160(%rdi), %r8
        movq	168(%rdi), %r9
        movq	176(%rdi), %r10
        # A[10] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[10] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r11, 144(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[10] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r12, 152(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[10] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r8, 160(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 168(%rdi)
        movq	184(%rdi), %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        # A[10] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[10] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r10, 176(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[10] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r11, 184(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[10] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        movq	%r14, %r9
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        adcxq	%r13, %r9
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r8, 200(%rdi)
        movq	%r9, 208(%rdi)
        movq	88(%rsi), %rdx
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	%r15, %r12
        movq	%rbx, %r8
        # A[11] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[11] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[11] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, %r15
        movq	128(%rdi), %r9
        movq	136(%rdi), %r10
        movq	144(%rdi), %r11
        movq	152(%rdi), %r12
        # A[11] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[11] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r8, %rbx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r9, 128(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[11] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r10, 136(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 144(%rdi)
        movq	160(%rdi), %r8
        movq	168(%rdi), %r9
        movq	176(%rdi), %r10
        movq	184(%rdi), %r11
        # A[11] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[11] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r12, 152(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[11] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r8, 160(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r9, 168(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 176(%rdi)
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        # A[11] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[11] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r11, 184(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[11] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[11] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        movq	%r14, %r10
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        adcxq	%r13, %r10
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r9, 208(%rdi)
        movq	%r10, 216(%rdi)
        movq	96(%rsi), %rdx
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	%r15, %r12
        movq	%rbx, %r8
        movq	128(%rdi), %r9
        # A[12] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[12] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[12] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r12, %r15
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, %rbx
        movq	136(%rdi), %r10
        movq	144(%rdi), %r11
        movq	152(%rdi), %r12
        movq	160(%rdi), %r8
        # A[12] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[12] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r9, 128(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r10, 136(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[12] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r11, 144(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 152(%rdi)
        movq	168(%rdi), %r9
        movq	176(%rdi), %r10
        movq	184(%rdi), %r11
        movq	192(%rdi), %r12
        # A[12] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[12] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r8, 160(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[12] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r9, 168(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r10, 176(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 184(%rdi)
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        # A[12] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[12] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[12] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[12] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        movq	%r14, %r11
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        adcxq	%r13, %r11
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r10, 216(%rdi)
        movq	%r11, 224(%rdi)
        movq	104(%rsi), %rdx
        movq	104(%rsp), %r11
        movq	%r15, %r12
        movq	%rbx, %r8
        movq	128(%rdi), %r9
        movq	136(%rdi), %r10
        # A[13] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[13] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[13] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r12, %r15
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[13] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r8, %rbx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 128(%rdi)
        movq	144(%rdi), %r11
        movq	152(%rdi), %r12
        movq	160(%rdi), %r8
        movq	168(%rdi), %r9
        # A[13] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[13] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r10, 136(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[13] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r11, 144(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[13] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r12, 152(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 160(%rdi)
        movq	176(%rdi), %r10
        movq	184(%rdi), %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        # A[13] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[13] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r9, 168(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[13] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r10, 176(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[13] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r11, 184(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 192(%rdi)
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        # A[13] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[13] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[13] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[13] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        movq	%r14, %r12
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        adcxq	%r13, %r12
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r11, 224(%rdi)
        movq	%r12, 232(%rdi)
        movq	112(%rsi), %rdx
        movq	%r15, %r12
        movq	%rbx, %r8
        movq	128(%rdi), %r9
        movq	136(%rdi), %r10
        movq	144(%rdi), %r11
        # A[14] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[14] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r12, %r15
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r8, %rbx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[14] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r9, 128(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 136(%rdi)
        movq	152(%rdi), %r12
        movq	160(%rdi), %r8
        movq	168(%rdi), %r9
        movq	176(%rdi), %r10
        # A[14] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[14] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r11, 144(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[14] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r12, 152(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r8, 160(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 168(%rdi)
        movq	184(%rdi), %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        # A[14] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[14] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r10, 176(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[14] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r11, 184(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[14] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 200(%rdi)
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        # A[14] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[14] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[14] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[14] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        movq	%r14, %r8
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        adcxq	%r13, %r8
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r12, 232(%rdi)
        movq	%r8, 240(%rdi)
        movq	120(%rsi), %rdx
        movq	%rbx, %r8
        movq	128(%rdi), %r9
        movq	136(%rdi), %r10
        movq	144(%rdi), %r11
        movq	152(%rdi), %r12
        # A[15] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[15] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r8, %rbx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[15] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r9, 128(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[15] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r10, 136(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 144(%rdi)
        movq	160(%rdi), %r8
        movq	168(%rdi), %r9
        movq	176(%rdi), %r10
        movq	184(%rdi), %r11
        # A[15] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[15] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r12, 152(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[15] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r8, 160(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[15] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r9, 168(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 176(%rdi)
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        # A[15] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[15] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r11, 184(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[15] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[15] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 208(%rdi)
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        # A[15] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[15] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[15] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[15] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        movq	%r14, %r9
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        adcxq	%r13, %r9
        movq	%r8, 240(%rdi)
        movq	%r9, 248(%rdi)
        movq	(%rsp), %r8
        movq	8(%rsp), %r9
        movq	16(%rsp), %r10
        movq	24(%rsp), %r11
        movq	%r8, (%rdi)
        movq	%r9, 8(%rdi)
        movq	%r10, 16(%rdi)
        movq	%r11, 24(%rdi)
        movq	32(%rsp), %r8
        movq	40(%rsp), %r9
        movq	48(%rsp), %r10
        movq	56(%rsp), %r11
        movq	%r8, 32(%rdi)
        movq	%r9, 40(%rdi)
        movq	%r10, 48(%rdi)
        movq	%r11, 56(%rdi)
        movq	64(%rsp), %r8
        movq	72(%rsp), %r9
        movq	80(%rsp), %r10
        movq	88(%rsp), %r11
        movq	%r8, 64(%rdi)
        movq	%r9, 72(%rdi)
        movq	%r10, 80(%rdi)
        movq	%r11, 88(%rdi)
        movq	96(%rsp), %r8
        movq	104(%rsp), %r9
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	%r8, 96(%rdi)
        movq	%r9, 104(%rdi)
        movq	%r10, 112(%rdi)
        movq	%r11, 120(%rdi)
        addq	$128, %rsp
        pop	%rbx
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        pop	%rbp
        repz retq
#ifndef __APPLE__
.size	sp_2048_mul_avx2_16,.-sp_2048_mul_avx2_16
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
#ifdef HAVE_INTEL_AVX2
/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_2048_sqr_avx2_16
.type	sp_2048_sqr_avx2_16,@function
.align	16
sp_2048_sqr_avx2_16:
#else
.globl	_sp_2048_sqr_avx2_16
.p2align	4
_sp_2048_sqr_avx2_16:
#endif /* __APPLE__ */
        push	%rbp
        push	%r12
        push	%r13
        subq	$128, %rsp
        cmpq	%rdi, %rsi
        movq	%rsp, %rbp
        cmovne	%rdi, %rbp
        xorq	%r12, %r12
        # Diagonal 1
        xorq	%r8, %r8
        xorq	%r9, %r9
        xorq	%r10, %r10
        xorq	%r11, %r11
        # A[1] x A[0]
        movq	(%rsi), %rdx
        mulxq	8(%rsi), %r8, %r9
        # A[2] x A[0]
        mulxq	16(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[3] x A[0]
        mulxq	24(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 8(%rbp)
        movq	%r9, 16(%rbp)
        movq	%r10, 24(%rbp)
        movq	%r12, %r8
        movq	%r12, %r9
        movq	%r12, %r10
        # A[4] x A[0]
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[5] x A[0]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[6] x A[0]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 32(%rbp)
        movq	%r8, 40(%rbp)
        movq	%r9, 48(%rbp)
        movq	%r12, %r11
        movq	%r12, %r8
        movq	%r12, %r9
        # A[7] x A[0]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[8] x A[0]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[9] x A[0]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 56(%rbp)
        movq	%r11, 64(%rbp)
        movq	%r8, 72(%rbp)
        movq	%r12, %r10
        movq	%r12, %r11
        movq	%r12, %r8
        # A[10] x A[0]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] x A[0]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] x A[0]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 80(%rbp)
        movq	%r10, 88(%rbp)
        movq	%r11, 96(%rbp)
        movq	%r12, %r9
        movq	%r12, %r10
        movq	%r12, %r11
        # A[13] x A[0]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] x A[0]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[15] x A[0]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 104(%rbp)
        movq	%r9, 112(%rbp)
        movq	%r10, 120(%rbp)
        #  Carry
        adcxq	%r12, %r11
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r11, 128(%rdi)
        # Diagonal 2
        movq	24(%rbp), %r11
        movq	32(%rbp), %r8
        movq	40(%rbp), %r9
        movq	48(%rbp), %r10
        # A[2] x A[1]
        movq	8(%rsi), %rdx
        mulxq	16(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[3] x A[1]
        mulxq	24(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[4] x A[1]
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 24(%rbp)
        movq	%r8, 32(%rbp)
        movq	%r9, 40(%rbp)
        movq	56(%rbp), %r11
        movq	64(%rbp), %r8
        movq	72(%rbp), %r9
        # A[5] x A[1]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[6] x A[1]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[7] x A[1]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 48(%rbp)
        movq	%r11, 56(%rbp)
        movq	%r8, 64(%rbp)
        movq	80(%rbp), %r10
        movq	88(%rbp), %r11
        movq	96(%rbp), %r8
        # A[8] x A[1]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[9] x A[1]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[10] x A[1]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 72(%rbp)
        movq	%r10, 80(%rbp)
        movq	%r11, 88(%rbp)
        movq	104(%rbp), %r9
        movq	112(%rbp), %r10
        movq	120(%rbp), %r11
        # A[11] x A[1]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[12] x A[1]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[13] x A[1]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 96(%rbp)
        movq	%r9, 104(%rbp)
        movq	%r10, 112(%rbp)
        movq	128(%rdi), %r8
        movq	%r12, %r9
        movq	%r12, %r10
        # A[14] x A[1]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[15] x A[1]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[15] x A[2]
        movq	16(%rsi), %rdx
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 120(%rbp)
        movq	%r8, 128(%rdi)
        movq	%r9, 136(%rdi)
        #  Carry
        adcxq	%r13, %r10
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r10, 144(%rdi)
        # Diagonal 3
        movq	40(%rbp), %r10
        movq	48(%rbp), %r11
        movq	56(%rbp), %r8
        movq	64(%rbp), %r9
        # A[3] x A[2]
        mulxq	24(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[4] x A[2]
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[5] x A[2]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 40(%rbp)
        movq	%r11, 48(%rbp)
        movq	%r8, 56(%rbp)
        movq	72(%rbp), %r10
        movq	80(%rbp), %r11
        movq	88(%rbp), %r8
        # A[6] x A[2]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[7] x A[2]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[8] x A[2]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 64(%rbp)
        movq	%r10, 72(%rbp)
        movq	%r11, 80(%rbp)
        movq	96(%rbp), %r9
        movq	104(%rbp), %r10
        movq	112(%rbp), %r11
        # A[9] x A[2]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[10] x A[2]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] x A[2]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 88(%rbp)
        movq	%r9, 96(%rbp)
        movq	%r10, 104(%rbp)
        movq	120(%rbp), %r8
        movq	128(%rdi), %r9
        movq	136(%rdi), %r10
        # A[12] x A[2]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[13] x A[2]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] x A[2]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 112(%rbp)
        movq	%r8, 120(%rbp)
        movq	%r9, 128(%rdi)
        movq	144(%rdi), %r11
        movq	%r12, %r8
        movq	%r12, %r9
        # A[14] x A[3]
        movq	112(%rsi), %rdx
        mulxq	24(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[14] x A[4]
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[14] x A[5]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 136(%rdi)
        movq	%r11, 144(%rdi)
        movq	%r8, 152(%rdi)
        #  Carry
        adcxq	%r13, %r9
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r9, 160(%rdi)
        # Diagonal 4
        movq	56(%rbp), %r9
        movq	64(%rbp), %r10
        movq	72(%rbp), %r11
        movq	80(%rbp), %r8
        # A[4] x A[3]
        movq	24(%rsi), %rdx
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[5] x A[3]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[6] x A[3]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 56(%rbp)
        movq	%r10, 64(%rbp)
        movq	%r11, 72(%rbp)
        movq	88(%rbp), %r9
        movq	96(%rbp), %r10
        movq	104(%rbp), %r11
        # A[7] x A[3]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[8] x A[3]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[9] x A[3]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 80(%rbp)
        movq	%r9, 88(%rbp)
        movq	%r10, 96(%rbp)
        movq	112(%rbp), %r8
        movq	120(%rbp), %r9
        movq	128(%rdi), %r10
        # A[10] x A[3]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[11] x A[3]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[12] x A[3]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 104(%rbp)
        movq	%r8, 112(%rbp)
        movq	%r9, 120(%rbp)
        movq	136(%rdi), %r11
        movq	144(%rdi), %r8
        movq	152(%rdi), %r9
        # A[13] x A[3]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[13] x A[4]
        movq	104(%rsi), %rdx
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[13] x A[5]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 128(%rdi)
        movq	%r11, 136(%rdi)
        movq	%r8, 144(%rdi)
        movq	160(%rdi), %r10
        movq	%r12, %r11
        movq	%r12, %r8
        # A[13] x A[6]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[13] x A[7]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[13] x A[8]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 152(%rdi)
        movq	%r10, 160(%rdi)
        movq	%r11, 168(%rdi)
        #  Carry
        adcxq	%r13, %r8
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r8, 176(%rdi)
        # Diagonal 5
        movq	72(%rbp), %r8
        movq	80(%rbp), %r9
        movq	88(%rbp), %r10
        movq	96(%rbp), %r11
        # A[5] x A[4]
        movq	32(%rsi), %rdx
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[6] x A[4]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[7] x A[4]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 72(%rbp)
        movq	%r9, 80(%rbp)
        movq	%r10, 88(%rbp)
        movq	104(%rbp), %r8
        movq	112(%rbp), %r9
        movq	120(%rbp), %r10
        # A[8] x A[4]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[9] x A[4]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[10] x A[4]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 96(%rbp)
        movq	%r8, 104(%rbp)
        movq	%r9, 112(%rbp)
        movq	128(%rdi), %r11
        movq	136(%rdi), %r8
        movq	144(%rdi), %r9
        # A[11] x A[4]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] x A[4]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[12] x A[5]
        movq	96(%rsi), %rdx
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 120(%rbp)
        movq	%r11, 128(%rdi)
        movq	%r8, 136(%rdi)
        movq	152(%rdi), %r10
        movq	160(%rdi), %r11
        movq	168(%rdi), %r8
        # A[12] x A[6]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[12] x A[7]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] x A[8]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 144(%rdi)
        movq	%r10, 152(%rdi)
        movq	%r11, 160(%rdi)
        movq	176(%rdi), %r9
        movq	%r12, %r10
        movq	%r12, %r11
        # A[12] x A[9]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[12] x A[10]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[12] x A[11]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 168(%rdi)
        movq	%r9, 176(%rdi)
        movq	%r10, 184(%rdi)
        #  Carry
        adcxq	%r13, %r11
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r11, 192(%rdi)
        # Diagonal 6
        movq	88(%rbp), %r11
        movq	96(%rbp), %r8
        movq	104(%rbp), %r9
        movq	112(%rbp), %r10
        # A[6] x A[5]
        movq	40(%rsi), %rdx
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[7] x A[5]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[8] x A[5]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 88(%rbp)
        movq	%r8, 96(%rbp)
        movq	%r9, 104(%rbp)
        movq	120(%rbp), %r11
        movq	128(%rdi), %r8
        movq	136(%rdi), %r9
        # A[9] x A[5]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[10] x A[5]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[11] x A[5]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 112(%rbp)
        movq	%r11, 120(%rbp)
        movq	%r8, 128(%rdi)
        movq	144(%rdi), %r10
        movq	152(%rdi), %r11
        movq	160(%rdi), %r8
        # A[11] x A[6]
        movq	88(%rsi), %rdx
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] x A[7]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[11] x A[8]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 136(%rdi)
        movq	%r10, 144(%rdi)
        movq	%r11, 152(%rdi)
        movq	168(%rdi), %r9
        movq	176(%rdi), %r10
        movq	184(%rdi), %r11
        # A[11] x A[9]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[11] x A[10]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[13] x A[9]
        movq	104(%rsi), %rdx
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 160(%rdi)
        movq	%r9, 168(%rdi)
        movq	%r10, 176(%rdi)
        movq	192(%rdi), %r8
        movq	%r12, %r9
        movq	%r12, %r10
        # A[13] x A[10]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[13] x A[11]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[13] x A[12]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 184(%rdi)
        movq	%r8, 192(%rdi)
        movq	%r9, 200(%rdi)
        #  Carry
        adcxq	%r13, %r10
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r10, 208(%rdi)
        # Diagonal 7
        movq	104(%rbp), %r10
        movq	112(%rbp), %r11
        movq	120(%rbp), %r8
        movq	128(%rdi), %r9
        # A[7] x A[6]
        movq	48(%rsi), %rdx
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[8] x A[6]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[9] x A[6]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 104(%rbp)
        movq	%r11, 112(%rbp)
        movq	%r8, 120(%rbp)
        movq	136(%rdi), %r10
        movq	144(%rdi), %r11
        movq	152(%rdi), %r8
        # A[10] x A[6]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[10] x A[7]
        movq	80(%rsi), %rdx
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[10] x A[8]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 128(%rdi)
        movq	%r10, 136(%rdi)
        movq	%r11, 144(%rdi)
        movq	160(%rdi), %r9
        movq	168(%rdi), %r10
        movq	176(%rdi), %r11
        # A[10] x A[9]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] x A[6]
        movq	112(%rsi), %rdx
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[14] x A[7]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 152(%rdi)
        movq	%r9, 160(%rdi)
        movq	%r10, 168(%rdi)
        movq	184(%rdi), %r8
        movq	192(%rdi), %r9
        movq	200(%rdi), %r10
        # A[14] x A[8]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[14] x A[9]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] x A[10]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 176(%rdi)
        movq	%r8, 184(%rdi)
        movq	%r9, 192(%rdi)
        movq	208(%rdi), %r11
        movq	%r12, %r8
        movq	%r12, %r9
        # A[14] x A[11]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[14] x A[12]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[14] x A[13]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 200(%rdi)
        movq	%r11, 208(%rdi)
        movq	%r8, 216(%rdi)
        #  Carry
        adcxq	%r13, %r9
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r9, 224(%rdi)
        # Diagonal 8
        movq	120(%rbp), %r9
        movq	128(%rdi), %r10
        movq	136(%rdi), %r11
        movq	144(%rdi), %r8
        # A[8] x A[7]
        movq	56(%rsi), %rdx
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[9] x A[7]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[9] x A[8]
        movq	64(%rsi), %rdx
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 120(%rbp)
        movq	%r10, 128(%rdi)
        movq	%r11, 136(%rdi)
        movq	152(%rdi), %r9
        movq	160(%rdi), %r10
        movq	168(%rdi), %r11
        # A[15] x A[3]
        movq	120(%rsi), %rdx
        mulxq	24(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[15] x A[4]
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[15] x A[5]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 144(%rdi)
        movq	%r9, 152(%rdi)
        movq	%r10, 160(%rdi)
        movq	176(%rdi), %r8
        movq	184(%rdi), %r9
        movq	192(%rdi), %r10
        # A[15] x A[6]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[15] x A[7]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[15] x A[8]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 168(%rdi)
        movq	%r8, 176(%rdi)
        movq	%r9, 184(%rdi)
        movq	200(%rdi), %r11
        movq	208(%rdi), %r8
        movq	216(%rdi), %r9
        # A[15] x A[9]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[15] x A[10]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[15] x A[11]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 192(%rdi)
        movq	%r11, 200(%rdi)
        movq	%r8, 208(%rdi)
        movq	224(%rdi), %r10
        movq	%r12, %r11
        movq	%r12, %r8
        # A[15] x A[12]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[15] x A[13]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[15] x A[14]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 216(%rdi)
        movq	%r10, 224(%rdi)
        movq	%r11, 232(%rdi)
        #  Carry
        adcxq	%r13, %r8
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r8, 240(%rdi)
        movq	%r13, 248(%rdi)
        # Double and Add in A[i] x A[i]
        movq	8(%rbp), %r9
        # A[0] x A[0]
        movq	(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        movq	%rax, (%rbp)
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r9, 8(%rbp)
        movq	16(%rbp), %r8
        movq	24(%rbp), %r9
        # A[1] x A[1]
        movq	8(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 16(%rbp)
        movq	%r9, 24(%rbp)
        movq	32(%rbp), %r8
        movq	40(%rbp), %r9
        # A[2] x A[2]
        movq	16(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 32(%rbp)
        movq	%r9, 40(%rbp)
        movq	48(%rbp), %r8
        movq	56(%rbp), %r9
        # A[3] x A[3]
        movq	24(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 48(%rbp)
        movq	%r9, 56(%rbp)
        movq	64(%rbp), %r8
        movq	72(%rbp), %r9
        # A[4] x A[4]
        movq	32(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 64(%rbp)
        movq	%r9, 72(%rbp)
        movq	80(%rbp), %r8
        movq	88(%rbp), %r9
        # A[5] x A[5]
        movq	40(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 80(%rbp)
        movq	%r9, 88(%rbp)
        movq	96(%rbp), %r8
        movq	104(%rbp), %r9
        # A[6] x A[6]
        movq	48(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 96(%rbp)
        movq	%r9, 104(%rbp)
        movq	112(%rbp), %r8
        movq	120(%rbp), %r9
        # A[7] x A[7]
        movq	56(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 112(%rbp)
        movq	%r9, 120(%rbp)
        movq	128(%rdi), %r8
        movq	136(%rdi), %r9
        # A[8] x A[8]
        movq	64(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 128(%rdi)
        movq	%r9, 136(%rdi)
        movq	144(%rdi), %r8
        movq	152(%rdi), %r9
        # A[9] x A[9]
        movq	72(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 144(%rdi)
        movq	%r9, 152(%rdi)
        movq	160(%rdi), %r8
        movq	168(%rdi), %r9
        # A[10] x A[10]
        movq	80(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 160(%rdi)
        movq	%r9, 168(%rdi)
        movq	176(%rdi), %r8
        movq	184(%rdi), %r9
        # A[11] x A[11]
        movq	88(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 176(%rdi)
        movq	%r9, 184(%rdi)
        movq	192(%rdi), %r8
        movq	200(%rdi), %r9
        # A[12] x A[12]
        movq	96(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 192(%rdi)
        movq	%r9, 200(%rdi)
        movq	208(%rdi), %r8
        movq	216(%rdi), %r9
        # A[13] x A[13]
        movq	104(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 208(%rdi)
        movq	%r9, 216(%rdi)
        movq	224(%rdi), %r8
        movq	232(%rdi), %r9
        # A[14] x A[14]
        movq	112(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 224(%rdi)
        movq	%r9, 232(%rdi)
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        # A[15] x A[15]
        movq	120(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 240(%rdi)
        movq	%r9, 248(%rdi)
        cmpq	%rdi, %rsi
        jne	L_end_2048_sqr_avx2_16
        movq	(%rbp), %r8
        movq	8(%rbp), %r9
        movq	16(%rbp), %r10
        movq	24(%rbp), %r11
        movq	%r8, (%rdi)
        movq	%r9, 8(%rdi)
        movq	%r10, 16(%rdi)
        movq	%r11, 24(%rdi)
        movq	32(%rbp), %r8
        movq	40(%rbp), %r9
        movq	48(%rbp), %r10
        movq	56(%rbp), %r11
        movq	%r8, 32(%rdi)
        movq	%r9, 40(%rdi)
        movq	%r10, 48(%rdi)
        movq	%r11, 56(%rdi)
        movq	64(%rbp), %r8
        movq	72(%rbp), %r9
        movq	80(%rbp), %r10
        movq	88(%rbp), %r11
        movq	%r8, 64(%rdi)
        movq	%r9, 72(%rdi)
        movq	%r10, 80(%rdi)
        movq	%r11, 88(%rdi)
        movq	96(%rbp), %r8
        movq	104(%rbp), %r9
        movq	112(%rbp), %r10
        movq	120(%rbp), %r11
        movq	%r8, 96(%rdi)
        movq	%r9, 104(%rdi)
        movq	%r10, 112(%rdi)
        movq	%r11, 120(%rdi)
L_end_2048_sqr_avx2_16:
        addq	$128, %rsp
        pop	%r13
        pop	%r12
        pop	%rbp
        repz retq
#ifndef __APPLE__
.size	sp_2048_sqr_avx2_16,.-sp_2048_sqr_avx2_16
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_2048_add_16
.type	sp_2048_add_16,@function
.align	16
sp_2048_add_16:
#else
.globl	_sp_2048_add_16
.p2align	4
_sp_2048_add_16:
#endif /* __APPLE__ */
        xorq	%rax, %rax
        movq	(%rsi), %rcx
        addq	(%rdx), %rcx
        movq	%rcx, (%rdi)
        movq	8(%rsi), %rcx
        adcq	8(%rdx), %rcx
        movq	%rcx, 8(%rdi)
        movq	16(%rsi), %rcx
        adcq	16(%rdx), %rcx
        movq	%rcx, 16(%rdi)
        movq	24(%rsi), %rcx
        adcq	24(%rdx), %rcx
        movq	%rcx, 24(%rdi)
        movq	32(%rsi), %rcx
        adcq	32(%rdx), %rcx
        movq	%rcx, 32(%rdi)
        movq	40(%rsi), %rcx
        adcq	40(%rdx), %rcx
        movq	%rcx, 40(%rdi)
        movq	48(%rsi), %rcx
        adcq	48(%rdx), %rcx
        movq	%rcx, 48(%rdi)
        movq	56(%rsi), %rcx
        adcq	56(%rdx), %rcx
        movq	%rcx, 56(%rdi)
        movq	64(%rsi), %rcx
        adcq	64(%rdx), %rcx
        movq	%rcx, 64(%rdi)
        movq	72(%rsi), %rcx
        adcq	72(%rdx), %rcx
        movq	%rcx, 72(%rdi)
        movq	80(%rsi), %rcx
        adcq	80(%rdx), %rcx
        movq	%rcx, 80(%rdi)
        movq	88(%rsi), %rcx
        adcq	88(%rdx), %rcx
        movq	%rcx, 88(%rdi)
        movq	96(%rsi), %rcx
        adcq	96(%rdx), %rcx
        movq	%rcx, 96(%rdi)
        movq	104(%rsi), %rcx
        adcq	104(%rdx), %rcx
        movq	%rcx, 104(%rdi)
        movq	112(%rsi), %rcx
        adcq	112(%rdx), %rcx
        movq	%rcx, 112(%rdi)
        movq	120(%rsi), %rcx
        adcq	120(%rdx), %rcx
        movq	%rcx, 120(%rdi)
        adcq	$0, %rax
        repz retq
#ifndef __APPLE__
.size	sp_2048_add_16,.-sp_2048_add_16
#endif /* __APPLE__ */
/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer and result.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_2048_sub_in_place_32
.type	sp_2048_sub_in_place_32,@function
.align	16
sp_2048_sub_in_place_32:
#else
.globl	_sp_2048_sub_in_place_32
.p2align	4
_sp_2048_sub_in_place_32:
#endif /* __APPLE__ */
        xorq	%rax, %rax
        movq	(%rdi), %rdx
        movq	8(%rdi), %rcx
        movq	(%rsi), %r8
        movq	8(%rsi), %r9
        subq	%r8, %rdx
        movq	16(%rsi), %r8
        movq	%rdx, (%rdi)
        movq	16(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	24(%rsi), %r9
        movq	%rcx, 8(%rdi)
        movq	24(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	32(%rsi), %r8
        movq	%rdx, 16(%rdi)
        movq	32(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	40(%rsi), %r9
        movq	%rcx, 24(%rdi)
        movq	40(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	48(%rsi), %r8
        movq	%rdx, 32(%rdi)
        movq	48(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	56(%rsi), %r9
        movq	%rcx, 40(%rdi)
        movq	56(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	64(%rsi), %r8
        movq	%rdx, 48(%rdi)
        movq	64(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	72(%rsi), %r9
        movq	%rcx, 56(%rdi)
        movq	72(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	80(%rsi), %r8
        movq	%rdx, 64(%rdi)
        movq	80(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	88(%rsi), %r9
        movq	%rcx, 72(%rdi)
        movq	88(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	96(%rsi), %r8
        movq	%rdx, 80(%rdi)
        movq	96(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	104(%rsi), %r9
        movq	%rcx, 88(%rdi)
        movq	104(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	112(%rsi), %r8
        movq	%rdx, 96(%rdi)
        movq	112(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	120(%rsi), %r9
        movq	%rcx, 104(%rdi)
        movq	120(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	128(%rsi), %r8
        movq	%rdx, 112(%rdi)
        movq	128(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	136(%rsi), %r9
        movq	%rcx, 120(%rdi)
        movq	136(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	144(%rsi), %r8
        movq	%rdx, 128(%rdi)
        movq	144(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	152(%rsi), %r9
        movq	%rcx, 136(%rdi)
        movq	152(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	160(%rsi), %r8
        movq	%rdx, 144(%rdi)
        movq	160(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	168(%rsi), %r9
        movq	%rcx, 152(%rdi)
        movq	168(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	176(%rsi), %r8
        movq	%rdx, 160(%rdi)
        movq	176(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	184(%rsi), %r9
        movq	%rcx, 168(%rdi)
        movq	184(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	192(%rsi), %r8
        movq	%rdx, 176(%rdi)
        movq	192(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	200(%rsi), %r9
        movq	%rcx, 184(%rdi)
        movq	200(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	208(%rsi), %r8
        movq	%rdx, 192(%rdi)
        movq	208(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	216(%rsi), %r9
        movq	%rcx, 200(%rdi)
        movq	216(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	224(%rsi), %r8
        movq	%rdx, 208(%rdi)
        movq	224(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	232(%rsi), %r9
        movq	%rcx, 216(%rdi)
        movq	232(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	240(%rsi), %r8
        movq	%rdx, 224(%rdi)
        movq	240(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	248(%rsi), %r9
        movq	%rcx, 232(%rdi)
        movq	248(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	%rdx, 240(%rdi)
        sbbq	%r9, %rcx
        movq	%rcx, 248(%rdi)
        sbbq	$0, %rax
        repz retq
#ifndef __APPLE__
.size	sp_2048_sub_in_place_32,.-sp_2048_sub_in_place_32
#endif /* __APPLE__ */
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_2048_add_32
.type	sp_2048_add_32,@function
.align	16
sp_2048_add_32:
#else
.globl	_sp_2048_add_32
.p2align	4
_sp_2048_add_32:
#endif /* __APPLE__ */
        xorq	%rax, %rax
        movq	(%rsi), %rcx
        addq	(%rdx), %rcx
        movq	%rcx, (%rdi)
        movq	8(%rsi), %rcx
        adcq	8(%rdx), %rcx
        movq	%rcx, 8(%rdi)
        movq	16(%rsi), %rcx
        adcq	16(%rdx), %rcx
        movq	%rcx, 16(%rdi)
        movq	24(%rsi), %rcx
        adcq	24(%rdx), %rcx
        movq	%rcx, 24(%rdi)
        movq	32(%rsi), %rcx
        adcq	32(%rdx), %rcx
        movq	%rcx, 32(%rdi)
        movq	40(%rsi), %rcx
        adcq	40(%rdx), %rcx
        movq	%rcx, 40(%rdi)
        movq	48(%rsi), %rcx
        adcq	48(%rdx), %rcx
        movq	%rcx, 48(%rdi)
        movq	56(%rsi), %rcx
        adcq	56(%rdx), %rcx
        movq	%rcx, 56(%rdi)
        movq	64(%rsi), %rcx
        adcq	64(%rdx), %rcx
        movq	%rcx, 64(%rdi)
        movq	72(%rsi), %rcx
        adcq	72(%rdx), %rcx
        movq	%rcx, 72(%rdi)
        movq	80(%rsi), %rcx
        adcq	80(%rdx), %rcx
        movq	%rcx, 80(%rdi)
        movq	88(%rsi), %rcx
        adcq	88(%rdx), %rcx
        movq	%rcx, 88(%rdi)
        movq	96(%rsi), %rcx
        adcq	96(%rdx), %rcx
        movq	%rcx, 96(%rdi)
        movq	104(%rsi), %rcx
        adcq	104(%rdx), %rcx
        movq	%rcx, 104(%rdi)
        movq	112(%rsi), %rcx
        adcq	112(%rdx), %rcx
        movq	%rcx, 112(%rdi)
        movq	120(%rsi), %rcx
        adcq	120(%rdx), %rcx
        movq	%rcx, 120(%rdi)
        movq	128(%rsi), %rcx
        adcq	128(%rdx), %rcx
        movq	%rcx, 128(%rdi)
        movq	136(%rsi), %rcx
        adcq	136(%rdx), %rcx
        movq	%rcx, 136(%rdi)
        movq	144(%rsi), %rcx
        adcq	144(%rdx), %rcx
        movq	%rcx, 144(%rdi)
        movq	152(%rsi), %rcx
        adcq	152(%rdx), %rcx
        movq	%rcx, 152(%rdi)
        movq	160(%rsi), %rcx
        adcq	160(%rdx), %rcx
        movq	%rcx, 160(%rdi)
        movq	168(%rsi), %rcx
        adcq	168(%rdx), %rcx
        movq	%rcx, 168(%rdi)
        movq	176(%rsi), %rcx
        adcq	176(%rdx), %rcx
        movq	%rcx, 176(%rdi)
        movq	184(%rsi), %rcx
        adcq	184(%rdx), %rcx
        movq	%rcx, 184(%rdi)
        movq	192(%rsi), %rcx
        adcq	192(%rdx), %rcx
        movq	%rcx, 192(%rdi)
        movq	200(%rsi), %rcx
        adcq	200(%rdx), %rcx
        movq	%rcx, 200(%rdi)
        movq	208(%rsi), %rcx
        adcq	208(%rdx), %rcx
        movq	%rcx, 208(%rdi)
        movq	216(%rsi), %rcx
        adcq	216(%rdx), %rcx
        movq	%rcx, 216(%rdi)
        movq	224(%rsi), %rcx
        adcq	224(%rdx), %rcx
        movq	%rcx, 224(%rdi)
        movq	232(%rsi), %rcx
        adcq	232(%rdx), %rcx
        movq	%rcx, 232(%rdi)
        movq	240(%rsi), %rcx
        adcq	240(%rdx), %rcx
        movq	%rcx, 240(%rdi)
        movq	248(%rsi), %rcx
        adcq	248(%rdx), %rcx
        movq	%rcx, 248(%rdi)
        adcq	$0, %rax
        repz retq
#ifndef __APPLE__
.size	sp_2048_add_32,.-sp_2048_add_32
#endif /* __APPLE__ */
/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
#ifndef __APPLE__
.globl	sp_2048_mul_d_32
.type	sp_2048_mul_d_32,@function
.align	16
sp_2048_mul_d_32:
#else
.globl	_sp_2048_mul_d_32
.p2align	4
_sp_2048_mul_d_32:
#endif /* __APPLE__ */
        movq	%rdx, %rcx
        # A[0] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	(%rsi)
        movq	%rax, %r8
        movq	%rdx, %r9
        movq	%r8, (%rdi)
        # A[1] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	8(%rsi)
        addq	%rax, %r9
        movq	%r9, 8(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	16(%rsi)
        addq	%rax, %r10
        movq	%r10, 16(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	24(%rsi)
        addq	%rax, %r8
        movq	%r8, 24(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	32(%rsi)
        addq	%rax, %r9
        movq	%r9, 32(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	40(%rsi)
        addq	%rax, %r10
        movq	%r10, 40(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	48(%rsi)
        addq	%rax, %r8
        movq	%r8, 48(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	56(%rsi)
        addq	%rax, %r9
        movq	%r9, 56(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	64(%rsi)
        addq	%rax, %r10
        movq	%r10, 64(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	72(%rsi)
        addq	%rax, %r8
        movq	%r8, 72(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	80(%rsi)
        addq	%rax, %r9
        movq	%r9, 80(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	88(%rsi)
        addq	%rax, %r10
        movq	%r10, 88(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	96(%rsi)
        addq	%rax, %r8
        movq	%r8, 96(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	104(%rsi)
        addq	%rax, %r9
        movq	%r9, 104(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	112(%rsi)
        addq	%rax, %r10
        movq	%r10, 112(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	120(%rsi)
        addq	%rax, %r8
        movq	%r8, 120(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[16] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	128(%rsi)
        addq	%rax, %r9
        movq	%r9, 128(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[17] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	136(%rsi)
        addq	%rax, %r10
        movq	%r10, 136(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[18] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	144(%rsi)
        addq	%rax, %r8
        movq	%r8, 144(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[19] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	152(%rsi)
        addq	%rax, %r9
        movq	%r9, 152(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[20] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	160(%rsi)
        addq	%rax, %r10
        movq	%r10, 160(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[21] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	168(%rsi)
        addq	%rax, %r8
        movq	%r8, 168(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[22] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	176(%rsi)
        addq	%rax, %r9
        movq	%r9, 176(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[23] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	184(%rsi)
        addq	%rax, %r10
        movq	%r10, 184(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[24] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	192(%rsi)
        addq	%rax, %r8
        movq	%r8, 192(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[25] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	200(%rsi)
        addq	%rax, %r9
        movq	%r9, 200(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[26] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	208(%rsi)
        addq	%rax, %r10
        movq	%r10, 208(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[27] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	216(%rsi)
        addq	%rax, %r8
        movq	%r8, 216(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[28] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	224(%rsi)
        addq	%rax, %r9
        movq	%r9, 224(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[29] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	232(%rsi)
        addq	%rax, %r10
        movq	%r10, 232(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[30] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	240(%rsi)
        addq	%rax, %r8
        movq	%r8, 240(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # # A[31] * B
        movq	%rcx, %rax
        mulq	248(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        movq	%r9, 248(%rdi)
        movq	%r10, 256(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_2048_mul_d_32,.-sp_2048_mul_d_32
#endif /* __APPLE__ */
/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer and result.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_2048_sub_in_place_16
.type	sp_2048_sub_in_place_16,@function
.align	16
sp_2048_sub_in_place_16:
#else
.globl	_sp_2048_sub_in_place_16
.p2align	4
_sp_2048_sub_in_place_16:
#endif /* __APPLE__ */
        xorq	%rax, %rax
        movq	(%rdi), %rdx
        movq	8(%rdi), %rcx
        movq	(%rsi), %r8
        movq	8(%rsi), %r9
        subq	%r8, %rdx
        movq	16(%rsi), %r8
        movq	%rdx, (%rdi)
        movq	16(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	24(%rsi), %r9
        movq	%rcx, 8(%rdi)
        movq	24(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	32(%rsi), %r8
        movq	%rdx, 16(%rdi)
        movq	32(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	40(%rsi), %r9
        movq	%rcx, 24(%rdi)
        movq	40(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	48(%rsi), %r8
        movq	%rdx, 32(%rdi)
        movq	48(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	56(%rsi), %r9
        movq	%rcx, 40(%rdi)
        movq	56(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	64(%rsi), %r8
        movq	%rdx, 48(%rdi)
        movq	64(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	72(%rsi), %r9
        movq	%rcx, 56(%rdi)
        movq	72(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	80(%rsi), %r8
        movq	%rdx, 64(%rdi)
        movq	80(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	88(%rsi), %r9
        movq	%rcx, 72(%rdi)
        movq	88(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	96(%rsi), %r8
        movq	%rdx, 80(%rdi)
        movq	96(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	104(%rsi), %r9
        movq	%rcx, 88(%rdi)
        movq	104(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	112(%rsi), %r8
        movq	%rdx, 96(%rdi)
        movq	112(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	120(%rsi), %r9
        movq	%rcx, 104(%rdi)
        movq	120(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	%rdx, 112(%rdi)
        sbbq	%r9, %rcx
        movq	%rcx, 120(%rdi)
        sbbq	$0, %rax
        repz retq
#ifndef __APPLE__
.size	sp_2048_sub_in_place_16,.-sp_2048_sub_in_place_16
#endif /* __APPLE__ */
/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not copying.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
#ifndef __APPLE__
.globl	sp_2048_cond_sub_16
.type	sp_2048_cond_sub_16,@function
.align	16
sp_2048_cond_sub_16:
#else
.globl	_sp_2048_cond_sub_16
.p2align	4
_sp_2048_cond_sub_16:
#endif /* __APPLE__ */
        subq	$128, %rsp
        movq	$0, %rax
        movq	(%rdx), %r8
        movq	8(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, (%rsp)
        movq	%r9, 8(%rsp)
        movq	16(%rdx), %r8
        movq	24(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 16(%rsp)
        movq	%r9, 24(%rsp)
        movq	32(%rdx), %r8
        movq	40(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 32(%rsp)
        movq	%r9, 40(%rsp)
        movq	48(%rdx), %r8
        movq	56(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 48(%rsp)
        movq	%r9, 56(%rsp)
        movq	64(%rdx), %r8
        movq	72(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 64(%rsp)
        movq	%r9, 72(%rsp)
        movq	80(%rdx), %r8
        movq	88(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 80(%rsp)
        movq	%r9, 88(%rsp)
        movq	96(%rdx), %r8
        movq	104(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 96(%rsp)
        movq	%r9, 104(%rsp)
        movq	112(%rdx), %r8
        movq	120(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 112(%rsp)
        movq	%r9, 120(%rsp)
        movq	(%rsi), %r8
        movq	(%rsp), %rdx
        subq	%rdx, %r8
        movq	8(%rsi), %r9
        movq	8(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, (%rdi)
        movq	16(%rsi), %r8
        movq	16(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 8(%rdi)
        movq	24(%rsi), %r9
        movq	24(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 16(%rdi)
        movq	32(%rsi), %r8
        movq	32(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 24(%rdi)
        movq	40(%rsi), %r9
        movq	40(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 32(%rdi)
        movq	48(%rsi), %r8
        movq	48(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 40(%rdi)
        movq	56(%rsi), %r9
        movq	56(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 48(%rdi)
        movq	64(%rsi), %r8
        movq	64(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 56(%rdi)
        movq	72(%rsi), %r9
        movq	72(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 64(%rdi)
        movq	80(%rsi), %r8
        movq	80(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 72(%rdi)
        movq	88(%rsi), %r9
        movq	88(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 80(%rdi)
        movq	96(%rsi), %r8
        movq	96(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 88(%rdi)
        movq	104(%rsi), %r9
        movq	104(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 96(%rdi)
        movq	112(%rsi), %r8
        movq	112(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 104(%rdi)
        movq	120(%rsi), %r9
        movq	120(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 112(%rdi)
        movq	%r9, 120(%rdi)
        sbbq	$0, %rax
        addq	$128, %rsp
        repz retq
#ifndef __APPLE__
.size	sp_2048_cond_sub_16,.-sp_2048_cond_sub_16
#endif /* __APPLE__ */
/* Reduce the number back to 2048 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
#ifndef __APPLE__
.globl	sp_2048_mont_reduce_16
.type	sp_2048_mont_reduce_16,@function
.align	16
sp_2048_mont_reduce_16:
#else
.globl	_sp_2048_mont_reduce_16
.p2align	4
_sp_2048_mont_reduce_16:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        movq	%rdx, %rcx
        xorq	%r15, %r15
        # i = 0
        movq	$16, %r8
        movq	(%rdi), %r13
        movq	8(%rdi), %r14
L_mont_loop_16:
        # mu = a[i] * mp
        movq	%r13, %r11
        imulq	%rcx, %r11
        # a[i+0] += m[0] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	(%rsi)
        addq	%rax, %r13
        adcq	%rdx, %r10
        # a[i+1] += m[1] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	8(%rsi)
        movq	%r14, %r13
        addq	%rax, %r13
        adcq	%rdx, %r9
        addq	%r10, %r13
        adcq	$0, %r9
        # a[i+2] += m[2] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	16(%rsi)
        movq	16(%rdi), %r14
        addq	%rax, %r14
        adcq	%rdx, %r10
        addq	%r9, %r14
        adcq	$0, %r10
        # a[i+3] += m[3] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	24(%rsi)
        movq	24(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 24(%rdi)
        adcq	$0, %r9
        # a[i+4] += m[4] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	32(%rsi)
        movq	32(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 32(%rdi)
        adcq	$0, %r10
        # a[i+5] += m[5] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	40(%rsi)
        movq	40(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 40(%rdi)
        adcq	$0, %r9
        # a[i+6] += m[6] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	48(%rsi)
        movq	48(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 48(%rdi)
        adcq	$0, %r10
        # a[i+7] += m[7] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	56(%rsi)
        movq	56(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 56(%rdi)
        adcq	$0, %r9
        # a[i+8] += m[8] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	64(%rsi)
        movq	64(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 64(%rdi)
        adcq	$0, %r10
        # a[i+9] += m[9] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	72(%rsi)
        movq	72(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 72(%rdi)
        adcq	$0, %r9
        # a[i+10] += m[10] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	80(%rsi)
        movq	80(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 80(%rdi)
        adcq	$0, %r10
        # a[i+11] += m[11] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	88(%rsi)
        movq	88(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 88(%rdi)
        adcq	$0, %r9
        # a[i+12] += m[12] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	96(%rsi)
        movq	96(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 96(%rdi)
        adcq	$0, %r10
        # a[i+13] += m[13] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	104(%rsi)
        movq	104(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 104(%rdi)
        adcq	$0, %r9
        # a[i+14] += m[14] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	112(%rsi)
        movq	112(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 112(%rdi)
        adcq	$0, %r10
        # a[i+15] += m[15] * mu
        movq	%r11, %rax
        mulq	120(%rsi)
        movq	120(%rdi), %r12
        addq	%rax, %r10
        adcq	%r15, %rdx
        movq	$0, %r15
        adcq	$0, %r15
        addq	%r10, %r12
        movq	%r12, 120(%rdi)
        adcq	%rdx, 128(%rdi)
        adcq	$0, %r15
        # i += 1
        addq	$8, %rdi
        decq	%r8
        jnz	L_mont_loop_16
        movq	%r13, (%rdi)
        movq	%r14, 8(%rdi)
        negq	%r15
        movq	%r15, %rcx
        movq	%rsi, %rdx
        movq	%rdi, %rsi
        movq	%rdi, %rdi
        subq	$128, %rdi
#ifndef __APPLE__
        callq	sp_2048_cond_sub_16@plt
#else
        callq	_sp_2048_cond_sub_16
#endif /* __APPLE__ */
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_2048_mont_reduce_16,.-sp_2048_mont_reduce_16
#endif /* __APPLE__ */
/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
#ifndef __APPLE__
.globl	sp_2048_mul_d_16
.type	sp_2048_mul_d_16,@function
.align	16
sp_2048_mul_d_16:
#else
.globl	_sp_2048_mul_d_16
.p2align	4
_sp_2048_mul_d_16:
#endif /* __APPLE__ */
        movq	%rdx, %rcx
        # A[0] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	(%rsi)
        movq	%rax, %r8
        movq	%rdx, %r9
        movq	%r8, (%rdi)
        # A[1] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	8(%rsi)
        addq	%rax, %r9
        movq	%r9, 8(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	16(%rsi)
        addq	%rax, %r10
        movq	%r10, 16(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	24(%rsi)
        addq	%rax, %r8
        movq	%r8, 24(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	32(%rsi)
        addq	%rax, %r9
        movq	%r9, 32(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	40(%rsi)
        addq	%rax, %r10
        movq	%r10, 40(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	48(%rsi)
        addq	%rax, %r8
        movq	%r8, 48(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	56(%rsi)
        addq	%rax, %r9
        movq	%r9, 56(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	64(%rsi)
        addq	%rax, %r10
        movq	%r10, 64(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	72(%rsi)
        addq	%rax, %r8
        movq	%r8, 72(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	80(%rsi)
        addq	%rax, %r9
        movq	%r9, 80(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	88(%rsi)
        addq	%rax, %r10
        movq	%r10, 88(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	96(%rsi)
        addq	%rax, %r8
        movq	%r8, 96(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	104(%rsi)
        addq	%rax, %r9
        movq	%r9, 104(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	112(%rsi)
        addq	%rax, %r10
        movq	%r10, 112(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # # A[15] * B
        movq	%rcx, %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        movq	%r8, 120(%rdi)
        movq	%r9, 128(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_2048_mul_d_16,.-sp_2048_mul_d_16
#endif /* __APPLE__ */
#ifdef HAVE_INTEL_AVX2
/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
#ifndef __APPLE__
.globl	sp_2048_mul_d_avx2_16
.type	sp_2048_mul_d_avx2_16,@function
.align	16
sp_2048_mul_d_avx2_16:
#else
.globl	_sp_2048_mul_d_avx2_16
.p2align	4
_sp_2048_mul_d_avx2_16:
#endif /* __APPLE__ */
        movq	%rdx, %rax
        # A[0] * B
        movq	%rax, %rdx
        xorq	%r11, %r11
        mulxq	(%rsi), %r9, %r10
        movq	%r9, (%rdi)
        # A[1] * B
        mulxq	8(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 8(%rdi)
        adoxq	%r8, %r9
        # A[2] * B
        mulxq	16(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 16(%rdi)
        adoxq	%r8, %r10
        # A[3] * B
        mulxq	24(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 24(%rdi)
        adoxq	%r8, %r9
        # A[4] * B
        mulxq	32(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 32(%rdi)
        adoxq	%r8, %r10
        # A[5] * B
        mulxq	40(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 40(%rdi)
        adoxq	%r8, %r9
        # A[6] * B
        mulxq	48(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 48(%rdi)
        adoxq	%r8, %r10
        # A[7] * B
        mulxq	56(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 56(%rdi)
        adoxq	%r8, %r9
        # A[8] * B
        mulxq	64(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 64(%rdi)
        adoxq	%r8, %r10
        # A[9] * B
        mulxq	72(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 72(%rdi)
        adoxq	%r8, %r9
        # A[10] * B
        mulxq	80(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 80(%rdi)
        adoxq	%r8, %r10
        # A[11] * B
        mulxq	88(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 88(%rdi)
        adoxq	%r8, %r9
        # A[12] * B
        mulxq	96(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 96(%rdi)
        adoxq	%r8, %r10
        # A[13] * B
        mulxq	104(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 104(%rdi)
        adoxq	%r8, %r9
        # A[14] * B
        mulxq	112(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 112(%rdi)
        adoxq	%r8, %r10
        # A[15] * B
        mulxq	120(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        adoxq	%r8, %r9
        adcxq	%r11, %r9
        movq	%r10, 120(%rdi)
        movq	%r9, 128(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_2048_mul_d_avx2_16,.-sp_2048_mul_d_avx2_16
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
#ifndef __APPLE__
.globl	sp_2048_cmp_16
.type	sp_2048_cmp_16,@function
.align	16
sp_2048_cmp_16:
#else
.globl	_sp_2048_cmp_16
.p2align	4
_sp_2048_cmp_16:
#endif /* __APPLE__ */
        xorq	%rcx, %rcx
        movq	$-1, %rdx
        movq	$-1, %rax
        movq	$1, %r8
        movq	120(%rdi), %r9
        movq	120(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	112(%rdi), %r9
        movq	112(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	104(%rdi), %r9
        movq	104(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	96(%rdi), %r9
        movq	96(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	88(%rdi), %r9
        movq	88(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	80(%rdi), %r9
        movq	80(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	72(%rdi), %r9
        movq	72(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	64(%rdi), %r9
        movq	64(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	56(%rdi), %r9
        movq	56(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	48(%rdi), %r9
        movq	48(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	40(%rdi), %r9
        movq	40(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	32(%rdi), %r9
        movq	32(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	24(%rdi), %r9
        movq	24(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	16(%rdi), %r9
        movq	16(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	8(%rdi), %r9
        movq	8(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	(%rdi), %r9
        movq	(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        xorq	%rdx, %rax
        repz retq
#ifndef __APPLE__
.size	sp_2048_cmp_16,.-sp_2048_cmp_16
#endif /* __APPLE__ */
#ifdef HAVE_INTEL_AVX2
/* Reduce the number back to 2048 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
#ifndef __APPLE__
.globl	sp_2048_mont_reduce_avx2_16
.type	sp_2048_mont_reduce_avx2_16,@function
.align	16
sp_2048_mont_reduce_avx2_16:
#else
.globl	_sp_2048_mont_reduce_avx2_16
.p2align	4
_sp_2048_mont_reduce_avx2_16:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        movq	%rdx, %rax
        xorq	%r14, %r14
        # i = 0
        movq	$16, %r9
        movq	(%rdi), %r13
        xorq	%r12, %r12
L_mont_loop_avx2_16:
        # mu = a[i] * mp
        movq	%r13, %rdx
        mulxq	%rax, %rdx, %r8
        movq	%r13, %r10
        # a[i+0] += m[0] * mu
        mulxq	(%rsi), %rcx, %r8
        movq	8(%rdi), %r13
        adcxq	%rcx, %r10
        adoxq	%r8, %r13
        # a[i+1] += m[1] * mu
        mulxq	8(%rsi), %rcx, %r8
        movq	16(%rdi), %r10
        adcxq	%rcx, %r13
        adoxq	%r8, %r10
        # a[i+2] += m[2] * mu
        mulxq	16(%rsi), %rcx, %r8
        movq	24(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 16(%rdi)
        # a[i+3] += m[3] * mu
        mulxq	24(%rsi), %rcx, %r8
        movq	32(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 24(%rdi)
        # a[i+4] += m[4] * mu
        mulxq	32(%rsi), %rcx, %r8
        movq	40(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 32(%rdi)
        # a[i+5] += m[5] * mu
        mulxq	40(%rsi), %rcx, %r8
        movq	48(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 40(%rdi)
        # a[i+6] += m[6] * mu
        mulxq	48(%rsi), %rcx, %r8
        movq	56(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 48(%rdi)
        # a[i+7] += m[7] * mu
        mulxq	56(%rsi), %rcx, %r8
        movq	64(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 56(%rdi)
        # a[i+8] += m[8] * mu
        mulxq	64(%rsi), %rcx, %r8
        movq	72(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 64(%rdi)
        # a[i+9] += m[9] * mu
        mulxq	72(%rsi), %rcx, %r8
        movq	80(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 72(%rdi)
        # a[i+10] += m[10] * mu
        mulxq	80(%rsi), %rcx, %r8
        movq	88(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 80(%rdi)
        # a[i+11] += m[11] * mu
        mulxq	88(%rsi), %rcx, %r8
        movq	96(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 88(%rdi)
        # a[i+12] += m[12] * mu
        mulxq	96(%rsi), %rcx, %r8
        movq	104(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 96(%rdi)
        # a[i+13] += m[13] * mu
        mulxq	104(%rsi), %rcx, %r8
        movq	112(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 104(%rdi)
        # a[i+14] += m[14] * mu
        mulxq	112(%rsi), %rcx, %r8
        movq	120(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 112(%rdi)
        # a[i+15] += m[15] * mu
        mulxq	120(%rsi), %rcx, %r8
        movq	128(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 120(%rdi)
        adcxq	%r14, %r10
        movq	%r12, %r14
        adoxq	%r12, %r14
        adcxq	%r12, %r14
        movq	%r10, 128(%rdi)
        # i += 1
        addq	$8, %rdi
        decq	%r9
        jnz	L_mont_loop_avx2_16
        movq	%r13, (%rdi)
        negq	%r14
        movq	%r14, %rcx
        movq	%rsi, %rdx
        movq	%rdi, %rsi
        movq	%rdi, %rdi
        subq	$128, %rdi
#ifndef __APPLE__
        callq	sp_2048_cond_sub_16@plt
#else
        callq	_sp_2048_cond_sub_16
#endif /* __APPLE__ */
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_2048_mont_reduce_avx2_16,.-sp_2048_mont_reduce_avx2_16
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not copying.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
#ifndef __APPLE__
.globl	sp_2048_cond_sub_32
.type	sp_2048_cond_sub_32,@function
.align	16
sp_2048_cond_sub_32:
#else
.globl	_sp_2048_cond_sub_32
.p2align	4
_sp_2048_cond_sub_32:
#endif /* __APPLE__ */
        subq	$256, %rsp
        movq	$0, %rax
        movq	(%rdx), %r8
        movq	8(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, (%rsp)
        movq	%r9, 8(%rsp)
        movq	16(%rdx), %r8
        movq	24(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 16(%rsp)
        movq	%r9, 24(%rsp)
        movq	32(%rdx), %r8
        movq	40(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 32(%rsp)
        movq	%r9, 40(%rsp)
        movq	48(%rdx), %r8
        movq	56(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 48(%rsp)
        movq	%r9, 56(%rsp)
        movq	64(%rdx), %r8
        movq	72(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 64(%rsp)
        movq	%r9, 72(%rsp)
        movq	80(%rdx), %r8
        movq	88(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 80(%rsp)
        movq	%r9, 88(%rsp)
        movq	96(%rdx), %r8
        movq	104(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 96(%rsp)
        movq	%r9, 104(%rsp)
        movq	112(%rdx), %r8
        movq	120(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 112(%rsp)
        movq	%r9, 120(%rsp)
        movq	128(%rdx), %r8
        movq	136(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 128(%rsp)
        movq	%r9, 136(%rsp)
        movq	144(%rdx), %r8
        movq	152(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 144(%rsp)
        movq	%r9, 152(%rsp)
        movq	160(%rdx), %r8
        movq	168(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 160(%rsp)
        movq	%r9, 168(%rsp)
        movq	176(%rdx), %r8
        movq	184(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 176(%rsp)
        movq	%r9, 184(%rsp)
        movq	192(%rdx), %r8
        movq	200(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 192(%rsp)
        movq	%r9, 200(%rsp)
        movq	208(%rdx), %r8
        movq	216(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 208(%rsp)
        movq	%r9, 216(%rsp)
        movq	224(%rdx), %r8
        movq	232(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 224(%rsp)
        movq	%r9, 232(%rsp)
        movq	240(%rdx), %r8
        movq	248(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 240(%rsp)
        movq	%r9, 248(%rsp)
        movq	(%rsi), %r8
        movq	(%rsp), %rdx
        subq	%rdx, %r8
        movq	8(%rsi), %r9
        movq	8(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, (%rdi)
        movq	16(%rsi), %r8
        movq	16(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 8(%rdi)
        movq	24(%rsi), %r9
        movq	24(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 16(%rdi)
        movq	32(%rsi), %r8
        movq	32(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 24(%rdi)
        movq	40(%rsi), %r9
        movq	40(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 32(%rdi)
        movq	48(%rsi), %r8
        movq	48(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 40(%rdi)
        movq	56(%rsi), %r9
        movq	56(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 48(%rdi)
        movq	64(%rsi), %r8
        movq	64(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 56(%rdi)
        movq	72(%rsi), %r9
        movq	72(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 64(%rdi)
        movq	80(%rsi), %r8
        movq	80(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 72(%rdi)
        movq	88(%rsi), %r9
        movq	88(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 80(%rdi)
        movq	96(%rsi), %r8
        movq	96(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 88(%rdi)
        movq	104(%rsi), %r9
        movq	104(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 96(%rdi)
        movq	112(%rsi), %r8
        movq	112(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 104(%rdi)
        movq	120(%rsi), %r9
        movq	120(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 112(%rdi)
        movq	128(%rsi), %r8
        movq	128(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 120(%rdi)
        movq	136(%rsi), %r9
        movq	136(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 128(%rdi)
        movq	144(%rsi), %r8
        movq	144(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 136(%rdi)
        movq	152(%rsi), %r9
        movq	152(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 144(%rdi)
        movq	160(%rsi), %r8
        movq	160(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 152(%rdi)
        movq	168(%rsi), %r9
        movq	168(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 160(%rdi)
        movq	176(%rsi), %r8
        movq	176(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 168(%rdi)
        movq	184(%rsi), %r9
        movq	184(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 176(%rdi)
        movq	192(%rsi), %r8
        movq	192(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 184(%rdi)
        movq	200(%rsi), %r9
        movq	200(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 192(%rdi)
        movq	208(%rsi), %r8
        movq	208(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 200(%rdi)
        movq	216(%rsi), %r9
        movq	216(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 208(%rdi)
        movq	224(%rsi), %r8
        movq	224(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 216(%rdi)
        movq	232(%rsi), %r9
        movq	232(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 224(%rdi)
        movq	240(%rsi), %r8
        movq	240(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 232(%rdi)
        movq	248(%rsi), %r9
        movq	248(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 240(%rdi)
        movq	%r9, 248(%rdi)
        sbbq	$0, %rax
        addq	$256, %rsp
        repz retq
#ifndef __APPLE__
.size	sp_2048_cond_sub_32,.-sp_2048_cond_sub_32
#endif /* __APPLE__ */
/* Reduce the number back to 2048 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
#ifndef __APPLE__
.globl	sp_2048_mont_reduce_32
.type	sp_2048_mont_reduce_32,@function
.align	16
sp_2048_mont_reduce_32:
#else
.globl	_sp_2048_mont_reduce_32
.p2align	4
_sp_2048_mont_reduce_32:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        movq	%rdx, %rcx
        xorq	%r15, %r15
        # i = 0
        movq	$32, %r8
        movq	(%rdi), %r13
        movq	8(%rdi), %r14
L_mont_loop_32:
        # mu = a[i] * mp
        movq	%r13, %r11
        imulq	%rcx, %r11
        # a[i+0] += m[0] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	(%rsi)
        addq	%rax, %r13
        adcq	%rdx, %r10
        # a[i+1] += m[1] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	8(%rsi)
        movq	%r14, %r13
        addq	%rax, %r13
        adcq	%rdx, %r9
        addq	%r10, %r13
        adcq	$0, %r9
        # a[i+2] += m[2] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	16(%rsi)
        movq	16(%rdi), %r14
        addq	%rax, %r14
        adcq	%rdx, %r10
        addq	%r9, %r14
        adcq	$0, %r10
        # a[i+3] += m[3] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	24(%rsi)
        movq	24(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 24(%rdi)
        adcq	$0, %r9
        # a[i+4] += m[4] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	32(%rsi)
        movq	32(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 32(%rdi)
        adcq	$0, %r10
        # a[i+5] += m[5] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	40(%rsi)
        movq	40(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 40(%rdi)
        adcq	$0, %r9
        # a[i+6] += m[6] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	48(%rsi)
        movq	48(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 48(%rdi)
        adcq	$0, %r10
        # a[i+7] += m[7] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	56(%rsi)
        movq	56(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 56(%rdi)
        adcq	$0, %r9
        # a[i+8] += m[8] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	64(%rsi)
        movq	64(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 64(%rdi)
        adcq	$0, %r10
        # a[i+9] += m[9] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	72(%rsi)
        movq	72(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 72(%rdi)
        adcq	$0, %r9
        # a[i+10] += m[10] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	80(%rsi)
        movq	80(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 80(%rdi)
        adcq	$0, %r10
        # a[i+11] += m[11] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	88(%rsi)
        movq	88(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 88(%rdi)
        adcq	$0, %r9
        # a[i+12] += m[12] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	96(%rsi)
        movq	96(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 96(%rdi)
        adcq	$0, %r10
        # a[i+13] += m[13] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	104(%rsi)
        movq	104(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 104(%rdi)
        adcq	$0, %r9
        # a[i+14] += m[14] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	112(%rsi)
        movq	112(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 112(%rdi)
        adcq	$0, %r10
        # a[i+15] += m[15] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	120(%rsi)
        movq	120(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 120(%rdi)
        adcq	$0, %r9
        # a[i+16] += m[16] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	128(%rsi)
        movq	128(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 128(%rdi)
        adcq	$0, %r10
        # a[i+17] += m[17] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	136(%rsi)
        movq	136(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 136(%rdi)
        adcq	$0, %r9
        # a[i+18] += m[18] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	144(%rsi)
        movq	144(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 144(%rdi)
        adcq	$0, %r10
        # a[i+19] += m[19] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	152(%rsi)
        movq	152(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 152(%rdi)
        adcq	$0, %r9
        # a[i+20] += m[20] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	160(%rsi)
        movq	160(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 160(%rdi)
        adcq	$0, %r10
        # a[i+21] += m[21] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	168(%rsi)
        movq	168(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 168(%rdi)
        adcq	$0, %r9
        # a[i+22] += m[22] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	176(%rsi)
        movq	176(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 176(%rdi)
        adcq	$0, %r10
        # a[i+23] += m[23] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	184(%rsi)
        movq	184(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 184(%rdi)
        adcq	$0, %r9
        # a[i+24] += m[24] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	192(%rsi)
        movq	192(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 192(%rdi)
        adcq	$0, %r10
        # a[i+25] += m[25] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	200(%rsi)
        movq	200(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 200(%rdi)
        adcq	$0, %r9
        # a[i+26] += m[26] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	208(%rsi)
        movq	208(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 208(%rdi)
        adcq	$0, %r10
        # a[i+27] += m[27] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	216(%rsi)
        movq	216(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 216(%rdi)
        adcq	$0, %r9
        # a[i+28] += m[28] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	224(%rsi)
        movq	224(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 224(%rdi)
        adcq	$0, %r10
        # a[i+29] += m[29] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	232(%rsi)
        movq	232(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 232(%rdi)
        adcq	$0, %r9
        # a[i+30] += m[30] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	240(%rsi)
        movq	240(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 240(%rdi)
        adcq	$0, %r10
        # a[i+31] += m[31] * mu
        movq	%r11, %rax
        mulq	248(%rsi)
        movq	248(%rdi), %r12
        addq	%rax, %r10
        adcq	%r15, %rdx
        movq	$0, %r15
        adcq	$0, %r15
        addq	%r10, %r12
        movq	%r12, 248(%rdi)
        adcq	%rdx, 256(%rdi)
        adcq	$0, %r15
        # i += 1
        addq	$8, %rdi
        decq	%r8
        jnz	L_mont_loop_32
        movq	%r13, (%rdi)
        movq	%r14, 8(%rdi)
        negq	%r15
        movq	%r15, %rcx
        movq	%rsi, %rdx
        movq	%rdi, %rsi
        movq	%rdi, %rdi
        subq	$256, %rdi
#ifndef __APPLE__
        callq	sp_2048_cond_sub_32@plt
#else
        callq	_sp_2048_cond_sub_32
#endif /* __APPLE__ */
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_2048_mont_reduce_32,.-sp_2048_mont_reduce_32
#endif /* __APPLE__ */
#ifdef HAVE_INTEL_AVX2
/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
#ifndef __APPLE__
.globl	sp_2048_mul_d_avx2_32
.type	sp_2048_mul_d_avx2_32,@function
.align	16
sp_2048_mul_d_avx2_32:
#else
.globl	_sp_2048_mul_d_avx2_32
.p2align	4
_sp_2048_mul_d_avx2_32:
#endif /* __APPLE__ */
        movq	%rdx, %rax
        # A[0] * B
        movq	%rax, %rdx
        xorq	%r11, %r11
        mulxq	(%rsi), %r9, %r10
        movq	%r9, (%rdi)
        # A[1] * B
        mulxq	8(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 8(%rdi)
        adoxq	%r8, %r9
        # A[2] * B
        mulxq	16(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 16(%rdi)
        adoxq	%r8, %r10
        # A[3] * B
        mulxq	24(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 24(%rdi)
        adoxq	%r8, %r9
        # A[4] * B
        mulxq	32(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 32(%rdi)
        adoxq	%r8, %r10
        # A[5] * B
        mulxq	40(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 40(%rdi)
        adoxq	%r8, %r9
        # A[6] * B
        mulxq	48(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 48(%rdi)
        adoxq	%r8, %r10
        # A[7] * B
        mulxq	56(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 56(%rdi)
        adoxq	%r8, %r9
        # A[8] * B
        mulxq	64(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 64(%rdi)
        adoxq	%r8, %r10
        # A[9] * B
        mulxq	72(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 72(%rdi)
        adoxq	%r8, %r9
        # A[10] * B
        mulxq	80(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 80(%rdi)
        adoxq	%r8, %r10
        # A[11] * B
        mulxq	88(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 88(%rdi)
        adoxq	%r8, %r9
        # A[12] * B
        mulxq	96(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 96(%rdi)
        adoxq	%r8, %r10
        # A[13] * B
        mulxq	104(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 104(%rdi)
        adoxq	%r8, %r9
        # A[14] * B
        mulxq	112(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 112(%rdi)
        adoxq	%r8, %r10
        # A[15] * B
        mulxq	120(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 120(%rdi)
        adoxq	%r8, %r9
        # A[16] * B
        mulxq	128(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 128(%rdi)
        adoxq	%r8, %r10
        # A[17] * B
        mulxq	136(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 136(%rdi)
        adoxq	%r8, %r9
        # A[18] * B
        mulxq	144(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 144(%rdi)
        adoxq	%r8, %r10
        # A[19] * B
        mulxq	152(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 152(%rdi)
        adoxq	%r8, %r9
        # A[20] * B
        mulxq	160(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 160(%rdi)
        adoxq	%r8, %r10
        # A[21] * B
        mulxq	168(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 168(%rdi)
        adoxq	%r8, %r9
        # A[22] * B
        mulxq	176(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 176(%rdi)
        adoxq	%r8, %r10
        # A[23] * B
        mulxq	184(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 184(%rdi)
        adoxq	%r8, %r9
        # A[24] * B
        mulxq	192(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 192(%rdi)
        adoxq	%r8, %r10
        # A[25] * B
        mulxq	200(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 200(%rdi)
        adoxq	%r8, %r9
        # A[26] * B
        mulxq	208(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 208(%rdi)
        adoxq	%r8, %r10
        # A[27] * B
        mulxq	216(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 216(%rdi)
        adoxq	%r8, %r9
        # A[28] * B
        mulxq	224(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 224(%rdi)
        adoxq	%r8, %r10
        # A[29] * B
        mulxq	232(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 232(%rdi)
        adoxq	%r8, %r9
        # A[30] * B
        mulxq	240(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 240(%rdi)
        adoxq	%r8, %r10
        # A[31] * B
        mulxq	248(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        adoxq	%r8, %r9
        adcxq	%r11, %r9
        movq	%r10, 248(%rdi)
        movq	%r9, 256(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_2048_mul_d_avx2_32,.-sp_2048_mul_d_avx2_32
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
#ifndef __APPLE__
.globl	sp_2048_cmp_32
.type	sp_2048_cmp_32,@function
.align	16
sp_2048_cmp_32:
#else
.globl	_sp_2048_cmp_32
.p2align	4
_sp_2048_cmp_32:
#endif /* __APPLE__ */
        xorq	%rcx, %rcx
        movq	$-1, %rdx
        movq	$-1, %rax
        movq	$1, %r8
        movq	248(%rdi), %r9
        movq	248(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	240(%rdi), %r9
        movq	240(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	232(%rdi), %r9
        movq	232(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	224(%rdi), %r9
        movq	224(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	216(%rdi), %r9
        movq	216(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	208(%rdi), %r9
        movq	208(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	200(%rdi), %r9
        movq	200(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	192(%rdi), %r9
        movq	192(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	184(%rdi), %r9
        movq	184(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	176(%rdi), %r9
        movq	176(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	168(%rdi), %r9
        movq	168(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	160(%rdi), %r9
        movq	160(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	152(%rdi), %r9
        movq	152(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	144(%rdi), %r9
        movq	144(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	136(%rdi), %r9
        movq	136(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	128(%rdi), %r9
        movq	128(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	120(%rdi), %r9
        movq	120(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	112(%rdi), %r9
        movq	112(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	104(%rdi), %r9
        movq	104(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	96(%rdi), %r9
        movq	96(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	88(%rdi), %r9
        movq	88(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	80(%rdi), %r9
        movq	80(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	72(%rdi), %r9
        movq	72(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	64(%rdi), %r9
        movq	64(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	56(%rdi), %r9
        movq	56(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	48(%rdi), %r9
        movq	48(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	40(%rdi), %r9
        movq	40(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	32(%rdi), %r9
        movq	32(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	24(%rdi), %r9
        movq	24(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	16(%rdi), %r9
        movq	16(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	8(%rdi), %r9
        movq	8(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	(%rdi), %r9
        movq	(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        xorq	%rdx, %rax
        repz retq
#ifndef __APPLE__
.size	sp_2048_cmp_32,.-sp_2048_cmp_32
#endif /* __APPLE__ */
#ifdef HAVE_INTEL_AVX2
/* Reduce the number back to 2048 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
#ifndef __APPLE__
.globl	sp_2048_mont_reduce_avx2_32
.type	sp_2048_mont_reduce_avx2_32,@function
.align	16
sp_2048_mont_reduce_avx2_32:
#else
.globl	_sp_2048_mont_reduce_avx2_32
.p2align	4
_sp_2048_mont_reduce_avx2_32:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        movq	%rdx, %rax
        xorq	%r14, %r14
        # i = 0
        movq	$32, %r9
        movq	(%rdi), %r13
        xorq	%r12, %r12
L_mont_loop_avx2_32:
        # mu = a[i] * mp
        movq	%r13, %rdx
        mulxq	%rax, %rdx, %r8
        movq	%r13, %r10
        # a[i+0] += m[0] * mu
        mulxq	(%rsi), %rcx, %r8
        movq	8(%rdi), %r13
        adcxq	%rcx, %r10
        adoxq	%r8, %r13
        # a[i+1] += m[1] * mu
        mulxq	8(%rsi), %rcx, %r8
        movq	16(%rdi), %r10
        adcxq	%rcx, %r13
        adoxq	%r8, %r10
        # a[i+2] += m[2] * mu
        mulxq	16(%rsi), %rcx, %r8
        movq	24(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 16(%rdi)
        # a[i+3] += m[3] * mu
        mulxq	24(%rsi), %rcx, %r8
        movq	32(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 24(%rdi)
        # a[i+4] += m[4] * mu
        mulxq	32(%rsi), %rcx, %r8
        movq	40(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 32(%rdi)
        # a[i+5] += m[5] * mu
        mulxq	40(%rsi), %rcx, %r8
        movq	48(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 40(%rdi)
        # a[i+6] += m[6] * mu
        mulxq	48(%rsi), %rcx, %r8
        movq	56(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 48(%rdi)
        # a[i+7] += m[7] * mu
        mulxq	56(%rsi), %rcx, %r8
        movq	64(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 56(%rdi)
        # a[i+8] += m[8] * mu
        mulxq	64(%rsi), %rcx, %r8
        movq	72(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 64(%rdi)
        # a[i+9] += m[9] * mu
        mulxq	72(%rsi), %rcx, %r8
        movq	80(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 72(%rdi)
        # a[i+10] += m[10] * mu
        mulxq	80(%rsi), %rcx, %r8
        movq	88(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 80(%rdi)
        # a[i+11] += m[11] * mu
        mulxq	88(%rsi), %rcx, %r8
        movq	96(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 88(%rdi)
        # a[i+12] += m[12] * mu
        mulxq	96(%rsi), %rcx, %r8
        movq	104(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 96(%rdi)
        # a[i+13] += m[13] * mu
        mulxq	104(%rsi), %rcx, %r8
        movq	112(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 104(%rdi)
        # a[i+14] += m[14] * mu
        mulxq	112(%rsi), %rcx, %r8
        movq	120(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 112(%rdi)
        # a[i+15] += m[15] * mu
        mulxq	120(%rsi), %rcx, %r8
        movq	128(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 120(%rdi)
        # a[i+16] += m[16] * mu
        mulxq	128(%rsi), %rcx, %r8
        movq	136(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 128(%rdi)
        # a[i+17] += m[17] * mu
        mulxq	136(%rsi), %rcx, %r8
        movq	144(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 136(%rdi)
        # a[i+18] += m[18] * mu
        mulxq	144(%rsi), %rcx, %r8
        movq	152(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 144(%rdi)
        # a[i+19] += m[19] * mu
        mulxq	152(%rsi), %rcx, %r8
        movq	160(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 152(%rdi)
        # a[i+20] += m[20] * mu
        mulxq	160(%rsi), %rcx, %r8
        movq	168(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 160(%rdi)
        # a[i+21] += m[21] * mu
        mulxq	168(%rsi), %rcx, %r8
        movq	176(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 168(%rdi)
        # a[i+22] += m[22] * mu
        mulxq	176(%rsi), %rcx, %r8
        movq	184(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 176(%rdi)
        # a[i+23] += m[23] * mu
        mulxq	184(%rsi), %rcx, %r8
        movq	192(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 184(%rdi)
        # a[i+24] += m[24] * mu
        mulxq	192(%rsi), %rcx, %r8
        movq	200(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 192(%rdi)
        # a[i+25] += m[25] * mu
        mulxq	200(%rsi), %rcx, %r8
        movq	208(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 200(%rdi)
        # a[i+26] += m[26] * mu
        mulxq	208(%rsi), %rcx, %r8
        movq	216(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 208(%rdi)
        # a[i+27] += m[27] * mu
        mulxq	216(%rsi), %rcx, %r8
        movq	224(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 216(%rdi)
        # a[i+28] += m[28] * mu
        mulxq	224(%rsi), %rcx, %r8
        movq	232(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 224(%rdi)
        # a[i+29] += m[29] * mu
        mulxq	232(%rsi), %rcx, %r8
        movq	240(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 232(%rdi)
        # a[i+30] += m[30] * mu
        mulxq	240(%rsi), %rcx, %r8
        movq	248(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 240(%rdi)
        # a[i+31] += m[31] * mu
        mulxq	248(%rsi), %rcx, %r8
        movq	256(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 248(%rdi)
        adcxq	%r14, %r10
        movq	%r12, %r14
        adoxq	%r12, %r14
        adcxq	%r12, %r14
        movq	%r10, 256(%rdi)
        # i += 1
        addq	$8, %rdi
        decq	%r9
        jnz	L_mont_loop_avx2_32
        movq	%r13, (%rdi)
        negq	%r14
        movq	%r14, %rcx
        movq	%rsi, %rdx
        movq	%rdi, %rsi
        movq	%rdi, %rdi
        subq	$256, %rdi
#ifndef __APPLE__
        callq	sp_2048_cond_sub_32@plt
#else
        callq	_sp_2048_cond_sub_32
#endif /* __APPLE__ */
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_2048_mont_reduce_avx2_32,.-sp_2048_mont_reduce_avx2_32
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_3072_mul_24
.type	sp_3072_mul_24,@function
.align	16
sp_3072_mul_24:
#else
.globl	_sp_3072_mul_24
.p2align	4
_sp_3072_mul_24:
#endif /* __APPLE__ */
        movq	%rdx, %rcx
        subq	$192, %rsp
        # A[0] * B[0]
        movq	(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        movq	%rax, (%rsp)
        movq	%rdx, %r9
        # A[0] * B[1]
        movq	8(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[0]
        movq	(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 8(%rsp)
        # A[0] * B[2]
        movq	16(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[1]
        movq	8(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[0]
        movq	(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 16(%rsp)
        # A[0] * B[3]
        movq	24(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[1] * B[2]
        movq	16(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[1]
        movq	8(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[0]
        movq	(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 24(%rsp)
        # A[0] * B[4]
        movq	32(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[3]
        movq	24(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B[2]
        movq	16(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[1]
        movq	8(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[4] * B[0]
        movq	(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 32(%rsp)
        # A[0] * B[5]
        movq	40(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[4]
        movq	32(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[3]
        movq	24(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B[2]
        movq	16(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[4] * B[1]
        movq	8(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[5] * B[0]
        movq	(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 40(%rsp)
        # A[0] * B[6]
        movq	48(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[1] * B[5]
        movq	40(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[4]
        movq	32(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[3]
        movq	24(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B[2]
        movq	16(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[5] * B[1]
        movq	8(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[6] * B[0]
        movq	(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 48(%rsp)
        # A[0] * B[7]
        movq	56(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[6]
        movq	48(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B[5]
        movq	40(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[4]
        movq	32(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[4] * B[3]
        movq	24(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B[2]
        movq	16(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[6] * B[1]
        movq	8(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[7] * B[0]
        movq	(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 56(%rsp)
        # A[0] * B[8]
        movq	64(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[7]
        movq	56(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[6]
        movq	48(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B[5]
        movq	40(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[4] * B[4]
        movq	32(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[5] * B[3]
        movq	24(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B[2]
        movq	16(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[7] * B[1]
        movq	8(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[8] * B[0]
        movq	(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 64(%rsp)
        # A[0] * B[9]
        movq	72(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[1] * B[8]
        movq	64(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[7]
        movq	56(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[6]
        movq	48(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B[5]
        movq	40(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[5] * B[4]
        movq	32(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[6] * B[3]
        movq	24(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B[2]
        movq	16(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[8] * B[1]
        movq	8(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[9] * B[0]
        movq	(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 72(%rsp)
        # A[0] * B[10]
        movq	80(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[9]
        movq	72(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B[8]
        movq	64(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[7]
        movq	56(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[4] * B[6]
        movq	48(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B[5]
        movq	40(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[6] * B[4]
        movq	32(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[7] * B[3]
        movq	24(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B[2]
        movq	16(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[9] * B[1]
        movq	8(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[10] * B[0]
        movq	(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 80(%rsp)
        # A[0] * B[11]
        movq	88(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[10]
        movq	80(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[9]
        movq	72(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B[8]
        movq	64(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[4] * B[7]
        movq	56(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[5] * B[6]
        movq	48(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B[5]
        movq	40(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[7] * B[4]
        movq	32(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[8] * B[3]
        movq	24(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B[2]
        movq	16(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[10] * B[1]
        movq	8(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[11] * B[0]
        movq	(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 88(%rsp)
        # A[0] * B[12]
        movq	96(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[1] * B[11]
        movq	88(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[10]
        movq	80(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[9]
        movq	72(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B[8]
        movq	64(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[5] * B[7]
        movq	56(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[6] * B[6]
        movq	48(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B[5]
        movq	40(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[8] * B[4]
        movq	32(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[9] * B[3]
        movq	24(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B[2]
        movq	16(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[11] * B[1]
        movq	8(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[12] * B[0]
        movq	(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 96(%rsp)
        # A[0] * B[13]
        movq	104(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[12]
        movq	96(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B[11]
        movq	88(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[10]
        movq	80(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[4] * B[9]
        movq	72(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B[8]
        movq	64(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[6] * B[7]
        movq	56(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[7] * B[6]
        movq	48(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B[5]
        movq	40(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[9] * B[4]
        movq	32(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[10] * B[3]
        movq	24(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B[2]
        movq	16(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[12] * B[1]
        movq	8(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[13] * B[0]
        movq	(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 104(%rsp)
        # A[0] * B[14]
        movq	112(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[13]
        movq	104(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[12]
        movq	96(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B[11]
        movq	88(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[4] * B[10]
        movq	80(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[5] * B[9]
        movq	72(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B[8]
        movq	64(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[7] * B[7]
        movq	56(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[8] * B[6]
        movq	48(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B[5]
        movq	40(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[10] * B[4]
        movq	32(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[11] * B[3]
        movq	24(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B[2]
        movq	16(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * B[1]
        movq	8(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[14] * B[0]
        movq	(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 112(%rsp)
        # A[0] * B[15]
        movq	120(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[1] * B[14]
        movq	112(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[13]
        movq	104(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[12]
        movq	96(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B[11]
        movq	88(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[5] * B[10]
        movq	80(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[6] * B[9]
        movq	72(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B[8]
        movq	64(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[8] * B[7]
        movq	56(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[9] * B[6]
        movq	48(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B[5]
        movq	40(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[11] * B[4]
        movq	32(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[12] * B[3]
        movq	24(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B[2]
        movq	16(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[14] * B[1]
        movq	8(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[15] * B[0]
        movq	(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 120(%rsp)
        # A[0] * B[16]
        movq	128(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[15]
        movq	120(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B[14]
        movq	112(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[13]
        movq	104(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[4] * B[12]
        movq	96(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B[11]
        movq	88(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[6] * B[10]
        movq	80(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[7] * B[9]
        movq	72(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B[8]
        movq	64(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[9] * B[7]
        movq	56(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[10] * B[6]
        movq	48(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B[5]
        movq	40(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[12] * B[4]
        movq	32(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[13] * B[3]
        movq	24(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B[2]
        movq	16(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[15] * B[1]
        movq	8(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[16] * B[0]
        movq	(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 128(%rsp)
        # A[0] * B[17]
        movq	136(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[16]
        movq	128(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[15]
        movq	120(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B[14]
        movq	112(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[4] * B[13]
        movq	104(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[5] * B[12]
        movq	96(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B[11]
        movq	88(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[7] * B[10]
        movq	80(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[8] * B[9]
        movq	72(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B[8]
        movq	64(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[10] * B[7]
        movq	56(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[11] * B[6]
        movq	48(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B[5]
        movq	40(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * B[4]
        movq	32(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[14] * B[3]
        movq	24(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B[2]
        movq	16(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[16] * B[1]
        movq	8(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[17] * B[0]
        movq	(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 136(%rsp)
        # A[0] * B[18]
        movq	144(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[1] * B[17]
        movq	136(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[16]
        movq	128(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[15]
        movq	120(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B[14]
        movq	112(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[5] * B[13]
        movq	104(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[6] * B[12]
        movq	96(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B[11]
        movq	88(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[8] * B[10]
        movq	80(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[9] * B[9]
        movq	72(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B[8]
        movq	64(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[11] * B[7]
        movq	56(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[12] * B[6]
        movq	48(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B[5]
        movq	40(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[14] * B[4]
        movq	32(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[15] * B[3]
        movq	24(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[16] * B[2]
        movq	16(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[17] * B[1]
        movq	8(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[18] * B[0]
        movq	(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 144(%rsp)
        # A[0] * B[19]
        movq	152(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[18]
        movq	144(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B[17]
        movq	136(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[16]
        movq	128(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[4] * B[15]
        movq	120(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B[14]
        movq	112(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[6] * B[13]
        movq	104(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[7] * B[12]
        movq	96(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B[11]
        movq	88(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[9] * B[10]
        movq	80(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[10] * B[9]
        movq	72(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B[8]
        movq	64(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[12] * B[7]
        movq	56(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[13] * B[6]
        movq	48(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B[5]
        movq	40(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[15] * B[4]
        movq	32(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[16] * B[3]
        movq	24(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[17] * B[2]
        movq	16(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[18] * B[1]
        movq	8(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[19] * B[0]
        movq	(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 152(%rsp)
        # A[0] * B[20]
        movq	160(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[19]
        movq	152(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[18]
        movq	144(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B[17]
        movq	136(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[4] * B[16]
        movq	128(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[5] * B[15]
        movq	120(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B[14]
        movq	112(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[7] * B[13]
        movq	104(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[8] * B[12]
        movq	96(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B[11]
        movq	88(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[10] * B[10]
        movq	80(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[11] * B[9]
        movq	72(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B[8]
        movq	64(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * B[7]
        movq	56(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[14] * B[6]
        movq	48(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B[5]
        movq	40(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[16] * B[4]
        movq	32(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[17] * B[3]
        movq	24(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[18] * B[2]
        movq	16(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[19] * B[1]
        movq	8(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[20] * B[0]
        movq	(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 160(%rsp)
        # A[0] * B[21]
        movq	168(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[1] * B[20]
        movq	160(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[19]
        movq	152(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[18]
        movq	144(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B[17]
        movq	136(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[5] * B[16]
        movq	128(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[6] * B[15]
        movq	120(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B[14]
        movq	112(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[8] * B[13]
        movq	104(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[9] * B[12]
        movq	96(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B[11]
        movq	88(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[11] * B[10]
        movq	80(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[12] * B[9]
        movq	72(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B[8]
        movq	64(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[14] * B[7]
        movq	56(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[15] * B[6]
        movq	48(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[16] * B[5]
        movq	40(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[17] * B[4]
        movq	32(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[18] * B[3]
        movq	24(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[19] * B[2]
        movq	16(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[20] * B[1]
        movq	8(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[21] * B[0]
        movq	(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 168(%rsp)
        # A[0] * B[22]
        movq	176(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[21]
        movq	168(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B[20]
        movq	160(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[19]
        movq	152(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[4] * B[18]
        movq	144(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B[17]
        movq	136(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[6] * B[16]
        movq	128(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[7] * B[15]
        movq	120(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B[14]
        movq	112(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[9] * B[13]
        movq	104(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[10] * B[12]
        movq	96(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B[11]
        movq	88(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[12] * B[10]
        movq	80(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[13] * B[9]
        movq	72(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B[8]
        movq	64(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[15] * B[7]
        movq	56(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[16] * B[6]
        movq	48(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[17] * B[5]
        movq	40(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[18] * B[4]
        movq	32(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[19] * B[3]
        movq	24(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[20] * B[2]
        movq	16(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[21] * B[1]
        movq	8(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[22] * B[0]
        movq	(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 176(%rsp)
        # A[0] * B[23]
        movq	184(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[22]
        movq	176(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[21]
        movq	168(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B[20]
        movq	160(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[4] * B[19]
        movq	152(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[5] * B[18]
        movq	144(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B[17]
        movq	136(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[7] * B[16]
        movq	128(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[8] * B[15]
        movq	120(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B[14]
        movq	112(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[10] * B[13]
        movq	104(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[11] * B[12]
        movq	96(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B[11]
        movq	88(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * B[10]
        movq	80(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[14] * B[9]
        movq	72(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B[8]
        movq	64(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[16] * B[7]
        movq	56(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[17] * B[6]
        movq	48(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[18] * B[5]
        movq	40(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[19] * B[4]
        movq	32(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[20] * B[3]
        movq	24(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[21] * B[2]
        movq	16(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[22] * B[1]
        movq	8(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[23] * B[0]
        movq	(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 184(%rsp)
        # A[1] * B[23]
        movq	184(%rcx), %rax
        mulq	8(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[22]
        movq	176(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[21]
        movq	168(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B[20]
        movq	160(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[5] * B[19]
        movq	152(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[6] * B[18]
        movq	144(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B[17]
        movq	136(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[8] * B[16]
        movq	128(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[9] * B[15]
        movq	120(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B[14]
        movq	112(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[11] * B[13]
        movq	104(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[12] * B[12]
        movq	96(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B[11]
        movq	88(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[14] * B[10]
        movq	80(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[15] * B[9]
        movq	72(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[16] * B[8]
        movq	64(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[17] * B[7]
        movq	56(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[18] * B[6]
        movq	48(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[19] * B[5]
        movq	40(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[20] * B[4]
        movq	32(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[21] * B[3]
        movq	24(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[22] * B[2]
        movq	16(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[23] * B[1]
        movq	8(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 192(%rdi)
        # A[2] * B[23]
        movq	184(%rcx), %rax
        mulq	16(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[22]
        movq	176(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[4] * B[21]
        movq	168(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B[20]
        movq	160(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[6] * B[19]
        movq	152(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[7] * B[18]
        movq	144(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B[17]
        movq	136(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[9] * B[16]
        movq	128(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[10] * B[15]
        movq	120(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B[14]
        movq	112(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[12] * B[13]
        movq	104(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[13] * B[12]
        movq	96(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B[11]
        movq	88(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[15] * B[10]
        movq	80(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[16] * B[9]
        movq	72(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[17] * B[8]
        movq	64(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[18] * B[7]
        movq	56(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[19] * B[6]
        movq	48(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[20] * B[5]
        movq	40(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[21] * B[4]
        movq	32(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[22] * B[3]
        movq	24(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[23] * B[2]
        movq	16(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 200(%rdi)
        # A[3] * B[23]
        movq	184(%rcx), %rax
        mulq	24(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[4] * B[22]
        movq	176(%rcx), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[5] * B[21]
        movq	168(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B[20]
        movq	160(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[7] * B[19]
        movq	152(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[8] * B[18]
        movq	144(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B[17]
        movq	136(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[10] * B[16]
        movq	128(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[11] * B[15]
        movq	120(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B[14]
        movq	112(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * B[13]
        movq	104(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[14] * B[12]
        movq	96(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B[11]
        movq	88(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[16] * B[10]
        movq	80(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[17] * B[9]
        movq	72(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[18] * B[8]
        movq	64(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[19] * B[7]
        movq	56(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[20] * B[6]
        movq	48(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[21] * B[5]
        movq	40(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[22] * B[4]
        movq	32(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[23] * B[3]
        movq	24(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 208(%rdi)
        # A[4] * B[23]
        movq	184(%rcx), %rax
        mulq	32(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[5] * B[22]
        movq	176(%rcx), %rax
        mulq	40(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[6] * B[21]
        movq	168(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B[20]
        movq	160(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[8] * B[19]
        movq	152(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[9] * B[18]
        movq	144(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B[17]
        movq	136(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[11] * B[16]
        movq	128(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[12] * B[15]
        movq	120(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B[14]
        movq	112(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[14] * B[13]
        movq	104(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[15] * B[12]
        movq	96(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[16] * B[11]
        movq	88(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[17] * B[10]
        movq	80(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[18] * B[9]
        movq	72(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[19] * B[8]
        movq	64(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[20] * B[7]
        movq	56(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[21] * B[6]
        movq	48(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[22] * B[5]
        movq	40(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[23] * B[4]
        movq	32(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 216(%rdi)
        # A[5] * B[23]
        movq	184(%rcx), %rax
        mulq	40(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[6] * B[22]
        movq	176(%rcx), %rax
        mulq	48(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[7] * B[21]
        movq	168(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B[20]
        movq	160(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[9] * B[19]
        movq	152(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[10] * B[18]
        movq	144(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B[17]
        movq	136(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[12] * B[16]
        movq	128(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[13] * B[15]
        movq	120(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B[14]
        movq	112(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[15] * B[13]
        movq	104(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[16] * B[12]
        movq	96(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[17] * B[11]
        movq	88(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[18] * B[10]
        movq	80(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[19] * B[9]
        movq	72(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[20] * B[8]
        movq	64(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[21] * B[7]
        movq	56(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[22] * B[6]
        movq	48(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[23] * B[5]
        movq	40(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 224(%rdi)
        # A[6] * B[23]
        movq	184(%rcx), %rax
        mulq	48(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[7] * B[22]
        movq	176(%rcx), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[8] * B[21]
        movq	168(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B[20]
        movq	160(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[10] * B[19]
        movq	152(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[11] * B[18]
        movq	144(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B[17]
        movq	136(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * B[16]
        movq	128(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[14] * B[15]
        movq	120(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B[14]
        movq	112(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[16] * B[13]
        movq	104(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[17] * B[12]
        movq	96(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[18] * B[11]
        movq	88(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[19] * B[10]
        movq	80(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[20] * B[9]
        movq	72(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[21] * B[8]
        movq	64(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[22] * B[7]
        movq	56(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[23] * B[6]
        movq	48(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 232(%rdi)
        # A[7] * B[23]
        movq	184(%rcx), %rax
        mulq	56(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[8] * B[22]
        movq	176(%rcx), %rax
        mulq	64(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[9] * B[21]
        movq	168(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B[20]
        movq	160(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[11] * B[19]
        movq	152(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[12] * B[18]
        movq	144(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B[17]
        movq	136(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[14] * B[16]
        movq	128(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[15] * B[15]
        movq	120(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[16] * B[14]
        movq	112(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[17] * B[13]
        movq	104(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[18] * B[12]
        movq	96(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[19] * B[11]
        movq	88(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[20] * B[10]
        movq	80(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[21] * B[9]
        movq	72(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[22] * B[8]
        movq	64(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[23] * B[7]
        movq	56(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 240(%rdi)
        # A[8] * B[23]
        movq	184(%rcx), %rax
        mulq	64(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[9] * B[22]
        movq	176(%rcx), %rax
        mulq	72(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[10] * B[21]
        movq	168(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B[20]
        movq	160(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[12] * B[19]
        movq	152(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[13] * B[18]
        movq	144(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B[17]
        movq	136(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[15] * B[16]
        movq	128(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[16] * B[15]
        movq	120(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[17] * B[14]
        movq	112(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[18] * B[13]
        movq	104(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[19] * B[12]
        movq	96(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[20] * B[11]
        movq	88(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[21] * B[10]
        movq	80(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[22] * B[9]
        movq	72(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[23] * B[8]
        movq	64(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 248(%rdi)
        # A[9] * B[23]
        movq	184(%rcx), %rax
        mulq	72(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[10] * B[22]
        movq	176(%rcx), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[11] * B[21]
        movq	168(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B[20]
        movq	160(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * B[19]
        movq	152(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[14] * B[18]
        movq	144(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B[17]
        movq	136(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[16] * B[16]
        movq	128(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[17] * B[15]
        movq	120(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[18] * B[14]
        movq	112(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[19] * B[13]
        movq	104(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[20] * B[12]
        movq	96(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[21] * B[11]
        movq	88(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[22] * B[10]
        movq	80(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[23] * B[9]
        movq	72(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 256(%rdi)
        # A[10] * B[23]
        movq	184(%rcx), %rax
        mulq	80(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[11] * B[22]
        movq	176(%rcx), %rax
        mulq	88(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[12] * B[21]
        movq	168(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B[20]
        movq	160(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[14] * B[19]
        movq	152(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[15] * B[18]
        movq	144(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[16] * B[17]
        movq	136(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[17] * B[16]
        movq	128(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[18] * B[15]
        movq	120(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[19] * B[14]
        movq	112(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[20] * B[13]
        movq	104(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[21] * B[12]
        movq	96(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[22] * B[11]
        movq	88(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[23] * B[10]
        movq	80(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 264(%rdi)
        # A[11] * B[23]
        movq	184(%rcx), %rax
        mulq	88(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[12] * B[22]
        movq	176(%rcx), %rax
        mulq	96(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[13] * B[21]
        movq	168(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B[20]
        movq	160(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[15] * B[19]
        movq	152(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[16] * B[18]
        movq	144(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[17] * B[17]
        movq	136(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[18] * B[16]
        movq	128(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[19] * B[15]
        movq	120(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[20] * B[14]
        movq	112(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[21] * B[13]
        movq	104(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[22] * B[12]
        movq	96(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[23] * B[11]
        movq	88(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 272(%rdi)
        # A[12] * B[23]
        movq	184(%rcx), %rax
        mulq	96(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[13] * B[22]
        movq	176(%rcx), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[14] * B[21]
        movq	168(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B[20]
        movq	160(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[16] * B[19]
        movq	152(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[17] * B[18]
        movq	144(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[18] * B[17]
        movq	136(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[19] * B[16]
        movq	128(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[20] * B[15]
        movq	120(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[21] * B[14]
        movq	112(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[22] * B[13]
        movq	104(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[23] * B[12]
        movq	96(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 280(%rdi)
        # A[13] * B[23]
        movq	184(%rcx), %rax
        mulq	104(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[14] * B[22]
        movq	176(%rcx), %rax
        mulq	112(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[15] * B[21]
        movq	168(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[16] * B[20]
        movq	160(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[17] * B[19]
        movq	152(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[18] * B[18]
        movq	144(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[19] * B[17]
        movq	136(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[20] * B[16]
        movq	128(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[21] * B[15]
        movq	120(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[22] * B[14]
        movq	112(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[23] * B[13]
        movq	104(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 288(%rdi)
        # A[14] * B[23]
        movq	184(%rcx), %rax
        mulq	112(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[15] * B[22]
        movq	176(%rcx), %rax
        mulq	120(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[16] * B[21]
        movq	168(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[17] * B[20]
        movq	160(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[18] * B[19]
        movq	152(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[19] * B[18]
        movq	144(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[20] * B[17]
        movq	136(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[21] * B[16]
        movq	128(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[22] * B[15]
        movq	120(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[23] * B[14]
        movq	112(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 296(%rdi)
        # A[15] * B[23]
        movq	184(%rcx), %rax
        mulq	120(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[16] * B[22]
        movq	176(%rcx), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[17] * B[21]
        movq	168(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[18] * B[20]
        movq	160(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[19] * B[19]
        movq	152(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[20] * B[18]
        movq	144(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[21] * B[17]
        movq	136(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[22] * B[16]
        movq	128(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[23] * B[15]
        movq	120(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 304(%rdi)
        # A[16] * B[23]
        movq	184(%rcx), %rax
        mulq	128(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[17] * B[22]
        movq	176(%rcx), %rax
        mulq	136(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[18] * B[21]
        movq	168(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[19] * B[20]
        movq	160(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[20] * B[19]
        movq	152(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[21] * B[18]
        movq	144(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[22] * B[17]
        movq	136(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[23] * B[16]
        movq	128(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 312(%rdi)
        # A[17] * B[23]
        movq	184(%rcx), %rax
        mulq	136(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[18] * B[22]
        movq	176(%rcx), %rax
        mulq	144(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[19] * B[21]
        movq	168(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[20] * B[20]
        movq	160(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[21] * B[19]
        movq	152(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[22] * B[18]
        movq	144(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[23] * B[17]
        movq	136(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 320(%rdi)
        # A[18] * B[23]
        movq	184(%rcx), %rax
        mulq	144(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[19] * B[22]
        movq	176(%rcx), %rax
        mulq	152(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[20] * B[21]
        movq	168(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[21] * B[20]
        movq	160(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[22] * B[19]
        movq	152(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[23] * B[18]
        movq	144(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 328(%rdi)
        # A[19] * B[23]
        movq	184(%rcx), %rax
        mulq	152(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[20] * B[22]
        movq	176(%rcx), %rax
        mulq	160(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[21] * B[21]
        movq	168(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[22] * B[20]
        movq	160(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[23] * B[19]
        movq	152(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 336(%rdi)
        # A[20] * B[23]
        movq	184(%rcx), %rax
        mulq	160(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[21] * B[22]
        movq	176(%rcx), %rax
        mulq	168(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[22] * B[21]
        movq	168(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[23] * B[20]
        movq	160(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 344(%rdi)
        # A[21] * B[23]
        movq	184(%rcx), %rax
        mulq	168(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[22] * B[22]
        movq	176(%rcx), %rax
        mulq	176(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[23] * B[21]
        movq	168(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 352(%rdi)
        # A[22] * B[23]
        movq	184(%rcx), %rax
        mulq	176(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[23] * B[22]
        movq	176(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 360(%rdi)
        # A[23] * B[23]
        movq	184(%rcx), %rax
        mulq	184(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        movq	%r9, 368(%rdi)
        movq	%r10, 376(%rdi)
        movq	(%rsp), %rax
        movq	8(%rsp), %rdx
        movq	16(%rsp), %r8
        movq	24(%rsp), %r9
        movq	%rax, (%rdi)
        movq	%rdx, 8(%rdi)
        movq	%r8, 16(%rdi)
        movq	%r9, 24(%rdi)
        movq	32(%rsp), %rax
        movq	40(%rsp), %rdx
        movq	48(%rsp), %r8
        movq	56(%rsp), %r9
        movq	%rax, 32(%rdi)
        movq	%rdx, 40(%rdi)
        movq	%r8, 48(%rdi)
        movq	%r9, 56(%rdi)
        movq	64(%rsp), %rax
        movq	72(%rsp), %rdx
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	%rax, 64(%rdi)
        movq	%rdx, 72(%rdi)
        movq	%r8, 80(%rdi)
        movq	%r9, 88(%rdi)
        movq	96(%rsp), %rax
        movq	104(%rsp), %rdx
        movq	112(%rsp), %r8
        movq	120(%rsp), %r9
        movq	%rax, 96(%rdi)
        movq	%rdx, 104(%rdi)
        movq	%r8, 112(%rdi)
        movq	%r9, 120(%rdi)
        movq	128(%rsp), %rax
        movq	136(%rsp), %rdx
        movq	144(%rsp), %r8
        movq	152(%rsp), %r9
        movq	%rax, 128(%rdi)
        movq	%rdx, 136(%rdi)
        movq	%r8, 144(%rdi)
        movq	%r9, 152(%rdi)
        movq	160(%rsp), %rax
        movq	168(%rsp), %rdx
        movq	176(%rsp), %r8
        movq	184(%rsp), %r9
        movq	%rax, 160(%rdi)
        movq	%rdx, 168(%rdi)
        movq	%r8, 176(%rdi)
        movq	%r9, 184(%rdi)
        addq	$192, %rsp
        repz retq
#ifndef __APPLE__
.size	sp_3072_mul_24,.-sp_3072_mul_24
#endif /* __APPLE__ */
/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_3072_sqr_24
.type	sp_3072_sqr_24,@function
.align	16
sp_3072_sqr_24:
#else
.globl	_sp_3072_sqr_24
.p2align	4
_sp_3072_sqr_24:
#endif /* __APPLE__ */
        push	%r12
        subq	$192, %rsp
        # A[0] * A[0]
        movq	(%rsi), %rax
        mulq	%rax
        xorq	%r9, %r9
        movq	%rax, (%rsp)
        movq	%rdx, %r8
        # A[0] * A[1]
        movq	8(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        movq	%r8, 8(%rsp)
        # A[0] * A[2]
        movq	16(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        # A[1] * A[1]
        movq	8(%rsi), %rax
        mulq	%rax
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        movq	%r9, 16(%rsp)
        # A[0] * A[3]
        movq	24(%rsi), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * A[2]
        movq	16(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%rcx, 24(%rsp)
        # A[0] * A[4]
        movq	32(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        # A[1] * A[3]
        movq	24(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        # A[2] * A[2]
        movq	16(%rsi), %rax
        mulq	%rax
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        movq	%r8, 32(%rsp)
        # A[0] * A[5]
        movq	40(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[4]
        movq	32(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[3]
        movq	24(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 40(%rsp)
        # A[0] * A[6]
        movq	48(%rsi), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[5]
        movq	40(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[4]
        movq	32(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[3]
        movq	24(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 48(%rsp)
        # A[0] * A[7]
        movq	56(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[6]
        movq	48(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[5]
        movq	40(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[4]
        movq	32(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 56(%rsp)
        # A[0] * A[8]
        movq	64(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[7]
        movq	56(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[6]
        movq	48(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[5]
        movq	40(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[4]
        movq	32(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 64(%rsp)
        # A[0] * A[9]
        movq	72(%rsi), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[8]
        movq	64(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[7]
        movq	56(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[6]
        movq	48(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[5]
        movq	40(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 72(%rsp)
        # A[0] * A[10]
        movq	80(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[9]
        movq	72(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[8]
        movq	64(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[7]
        movq	56(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[6]
        movq	48(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[5]
        movq	40(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 80(%rsp)
        # A[0] * A[11]
        movq	88(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[10]
        movq	80(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[9]
        movq	72(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[8]
        movq	64(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[7]
        movq	56(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[6]
        movq	48(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 88(%rsp)
        # A[0] * A[12]
        movq	96(%rsi), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[11]
        movq	88(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[10]
        movq	80(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[9]
        movq	72(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[8]
        movq	64(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[7]
        movq	56(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[6]
        movq	48(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 96(%rsp)
        # A[0] * A[13]
        movq	104(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[12]
        movq	96(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[11]
        movq	88(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[10]
        movq	80(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[9]
        movq	72(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[8]
        movq	64(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[7]
        movq	56(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 104(%rsp)
        # A[0] * A[14]
        movq	112(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[13]
        movq	104(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[12]
        movq	96(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[11]
        movq	88(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[10]
        movq	80(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[9]
        movq	72(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[8]
        movq	64(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[7]
        movq	56(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 112(%rsp)
        # A[0] * A[15]
        movq	120(%rsi), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[14]
        movq	112(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[13]
        movq	104(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[12]
        movq	96(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[11]
        movq	88(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[10]
        movq	80(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[9]
        movq	72(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[8]
        movq	64(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 120(%rsp)
        # A[0] * A[16]
        movq	128(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[15]
        movq	120(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[14]
        movq	112(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[13]
        movq	104(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[12]
        movq	96(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[11]
        movq	88(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[10]
        movq	80(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[9]
        movq	72(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[8]
        movq	64(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 128(%rsp)
        # A[0] * A[17]
        movq	136(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[16]
        movq	128(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[15]
        movq	120(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[14]
        movq	112(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[13]
        movq	104(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[12]
        movq	96(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[11]
        movq	88(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[10]
        movq	80(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[9]
        movq	72(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 136(%rsp)
        # A[0] * A[18]
        movq	144(%rsi), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[17]
        movq	136(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[16]
        movq	128(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[15]
        movq	120(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[14]
        movq	112(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[13]
        movq	104(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[12]
        movq	96(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[11]
        movq	88(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[10]
        movq	80(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[9]
        movq	72(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 144(%rsp)
        # A[0] * A[19]
        movq	152(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[18]
        movq	144(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[17]
        movq	136(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[16]
        movq	128(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[15]
        movq	120(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[14]
        movq	112(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[13]
        movq	104(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[12]
        movq	96(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[11]
        movq	88(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[10]
        movq	80(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 152(%rsp)
        # A[0] * A[20]
        movq	160(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[19]
        movq	152(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[18]
        movq	144(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[17]
        movq	136(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[16]
        movq	128(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[15]
        movq	120(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[14]
        movq	112(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[13]
        movq	104(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[12]
        movq	96(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[11]
        movq	88(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[10]
        movq	80(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 160(%rsp)
        # A[0] * A[21]
        movq	168(%rsi), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[20]
        movq	160(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[19]
        movq	152(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[18]
        movq	144(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[17]
        movq	136(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[16]
        movq	128(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[15]
        movq	120(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[14]
        movq	112(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[13]
        movq	104(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[12]
        movq	96(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[11]
        movq	88(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 168(%rsp)
        # A[0] * A[22]
        movq	176(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[21]
        movq	168(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[20]
        movq	160(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[19]
        movq	152(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[18]
        movq	144(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[17]
        movq	136(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[16]
        movq	128(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[15]
        movq	120(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[14]
        movq	112(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[13]
        movq	104(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[12]
        movq	96(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[11]
        movq	88(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 176(%rsp)
        # A[0] * A[23]
        movq	184(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[1] * A[22]
        movq	176(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[2] * A[21]
        movq	168(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[20]
        movq	160(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[19]
        movq	152(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[18]
        movq	144(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[17]
        movq	136(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[16]
        movq	128(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[15]
        movq	120(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[14]
        movq	112(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[13]
        movq	104(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[12]
        movq	96(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 184(%rsp)
        # A[1] * A[23]
        movq	184(%rsi), %rax
        mulq	8(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[2] * A[22]
        movq	176(%rsi), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[3] * A[21]
        movq	168(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[20]
        movq	160(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[19]
        movq	152(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[18]
        movq	144(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[17]
        movq	136(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[16]
        movq	128(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[15]
        movq	120(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[14]
        movq	112(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[13]
        movq	104(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[12] * A[12]
        movq	96(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 192(%rdi)
        # A[2] * A[23]
        movq	184(%rsi), %rax
        mulq	16(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[3] * A[22]
        movq	176(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[4] * A[21]
        movq	168(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[20]
        movq	160(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[19]
        movq	152(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[18]
        movq	144(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[17]
        movq	136(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[16]
        movq	128(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[15]
        movq	120(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[14]
        movq	112(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[12] * A[13]
        movq	104(%rsi), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 200(%rdi)
        # A[3] * A[23]
        movq	184(%rsi), %rax
        mulq	24(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[4] * A[22]
        movq	176(%rsi), %rax
        mulq	32(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[5] * A[21]
        movq	168(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[20]
        movq	160(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[19]
        movq	152(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[18]
        movq	144(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[17]
        movq	136(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[16]
        movq	128(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[15]
        movq	120(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[12] * A[14]
        movq	112(%rsi), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[13] * A[13]
        movq	104(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 208(%rdi)
        # A[4] * A[23]
        movq	184(%rsi), %rax
        mulq	32(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[5] * A[22]
        movq	176(%rsi), %rax
        mulq	40(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[6] * A[21]
        movq	168(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[20]
        movq	160(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[19]
        movq	152(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[18]
        movq	144(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[17]
        movq	136(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[16]
        movq	128(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[12] * A[15]
        movq	120(%rsi), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[13] * A[14]
        movq	112(%rsi), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 216(%rdi)
        # A[5] * A[23]
        movq	184(%rsi), %rax
        mulq	40(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[6] * A[22]
        movq	176(%rsi), %rax
        mulq	48(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[7] * A[21]
        movq	168(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[20]
        movq	160(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[19]
        movq	152(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[18]
        movq	144(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[17]
        movq	136(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[12] * A[16]
        movq	128(%rsi), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[13] * A[15]
        movq	120(%rsi), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[14] * A[14]
        movq	112(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 224(%rdi)
        # A[6] * A[23]
        movq	184(%rsi), %rax
        mulq	48(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[7] * A[22]
        movq	176(%rsi), %rax
        mulq	56(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[8] * A[21]
        movq	168(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[20]
        movq	160(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[19]
        movq	152(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[18]
        movq	144(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[12] * A[17]
        movq	136(%rsi), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[13] * A[16]
        movq	128(%rsi), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[14] * A[15]
        movq	120(%rsi), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 232(%rdi)
        # A[7] * A[23]
        movq	184(%rsi), %rax
        mulq	56(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[8] * A[22]
        movq	176(%rsi), %rax
        mulq	64(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[9] * A[21]
        movq	168(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[20]
        movq	160(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[19]
        movq	152(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[12] * A[18]
        movq	144(%rsi), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[13] * A[17]
        movq	136(%rsi), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[14] * A[16]
        movq	128(%rsi), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[15] * A[15]
        movq	120(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 240(%rdi)
        # A[8] * A[23]
        movq	184(%rsi), %rax
        mulq	64(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[9] * A[22]
        movq	176(%rsi), %rax
        mulq	72(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[10] * A[21]
        movq	168(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[20]
        movq	160(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[12] * A[19]
        movq	152(%rsi), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[13] * A[18]
        movq	144(%rsi), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[14] * A[17]
        movq	136(%rsi), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[15] * A[16]
        movq	128(%rsi), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 248(%rdi)
        # A[9] * A[23]
        movq	184(%rsi), %rax
        mulq	72(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[10] * A[22]
        movq	176(%rsi), %rax
        mulq	80(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[11] * A[21]
        movq	168(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[12] * A[20]
        movq	160(%rsi), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[13] * A[19]
        movq	152(%rsi), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[14] * A[18]
        movq	144(%rsi), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[15] * A[17]
        movq	136(%rsi), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[16] * A[16]
        movq	128(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 256(%rdi)
        # A[10] * A[23]
        movq	184(%rsi), %rax
        mulq	80(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[11] * A[22]
        movq	176(%rsi), %rax
        mulq	88(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[12] * A[21]
        movq	168(%rsi), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[13] * A[20]
        movq	160(%rsi), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[14] * A[19]
        movq	152(%rsi), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[15] * A[18]
        movq	144(%rsi), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[16] * A[17]
        movq	136(%rsi), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 264(%rdi)
        # A[11] * A[23]
        movq	184(%rsi), %rax
        mulq	88(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[12] * A[22]
        movq	176(%rsi), %rax
        mulq	96(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[13] * A[21]
        movq	168(%rsi), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[14] * A[20]
        movq	160(%rsi), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[15] * A[19]
        movq	152(%rsi), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[16] * A[18]
        movq	144(%rsi), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[17] * A[17]
        movq	136(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 272(%rdi)
        # A[12] * A[23]
        movq	184(%rsi), %rax
        mulq	96(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[13] * A[22]
        movq	176(%rsi), %rax
        mulq	104(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[14] * A[21]
        movq	168(%rsi), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[15] * A[20]
        movq	160(%rsi), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[16] * A[19]
        movq	152(%rsi), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[17] * A[18]
        movq	144(%rsi), %rax
        mulq	136(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 280(%rdi)
        # A[13] * A[23]
        movq	184(%rsi), %rax
        mulq	104(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[14] * A[22]
        movq	176(%rsi), %rax
        mulq	112(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[15] * A[21]
        movq	168(%rsi), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[16] * A[20]
        movq	160(%rsi), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[17] * A[19]
        movq	152(%rsi), %rax
        mulq	136(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[18] * A[18]
        movq	144(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 288(%rdi)
        # A[14] * A[23]
        movq	184(%rsi), %rax
        mulq	112(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[15] * A[22]
        movq	176(%rsi), %rax
        mulq	120(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[16] * A[21]
        movq	168(%rsi), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[17] * A[20]
        movq	160(%rsi), %rax
        mulq	136(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[18] * A[19]
        movq	152(%rsi), %rax
        mulq	144(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 296(%rdi)
        # A[15] * A[23]
        movq	184(%rsi), %rax
        mulq	120(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[16] * A[22]
        movq	176(%rsi), %rax
        mulq	128(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[17] * A[21]
        movq	168(%rsi), %rax
        mulq	136(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[18] * A[20]
        movq	160(%rsi), %rax
        mulq	144(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[19] * A[19]
        movq	152(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 304(%rdi)
        # A[16] * A[23]
        movq	184(%rsi), %rax
        mulq	128(%rsi)
        xorq	%r9, %r9
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[17] * A[22]
        movq	176(%rsi), %rax
        mulq	136(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[18] * A[21]
        movq	168(%rsi), %rax
        mulq	144(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[19] * A[20]
        movq	160(%rsi), %rax
        mulq	152(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %rcx
        adcq	%r11, %r8
        adcq	%r12, %r9
        movq	%rcx, 312(%rdi)
        # A[17] * A[23]
        movq	184(%rsi), %rax
        mulq	136(%rsi)
        xorq	%rcx, %rcx
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[18] * A[22]
        movq	176(%rsi), %rax
        mulq	144(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[19] * A[21]
        movq	168(%rsi), %rax
        mulq	152(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[20] * A[20]
        movq	160(%rsi), %rax
        mulq	%rax
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r8
        adcq	%r11, %r9
        adcq	%r12, %rcx
        movq	%r8, 320(%rdi)
        # A[18] * A[23]
        movq	184(%rsi), %rax
        mulq	144(%rsi)
        xorq	%r8, %r8
        xorq	%r12, %r12
        movq	%rax, %r10
        movq	%rdx, %r11
        # A[19] * A[22]
        movq	176(%rsi), %rax
        mulq	152(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        # A[20] * A[21]
        movq	168(%rsi), %rax
        mulq	160(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        addq	%r10, %r9
        adcq	%r11, %rcx
        adcq	%r12, %r8
        movq	%r9, 328(%rdi)
        # A[19] * A[23]
        movq	184(%rsi), %rax
        mulq	152(%rsi)
        xorq	%r9, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[20] * A[22]
        movq	176(%rsi), %rax
        mulq	160(%rsi)
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[21] * A[21]
        movq	168(%rsi), %rax
        mulq	%rax
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%rcx, 336(%rdi)
        # A[20] * A[23]
        movq	184(%rsi), %rax
        mulq	160(%rsi)
        xorq	%rcx, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        # A[21] * A[22]
        movq	176(%rsi), %rax
        mulq	168(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        movq	%r8, 344(%rdi)
        # A[21] * A[23]
        movq	184(%rsi), %rax
        mulq	168(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        # A[22] * A[22]
        movq	176(%rsi), %rax
        mulq	%rax
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        movq	%r9, 352(%rdi)
        # A[22] * A[23]
        movq	184(%rsi), %rax
        mulq	176(%rsi)
        xorq	%r9, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%rcx, 360(%rdi)
        # A[23] * A[23]
        movq	184(%rsi), %rax
        mulq	%rax
        addq	%rax, %r8
        adcq	%rdx, %r9
        movq	%r8, 368(%rdi)
        movq	%r9, 376(%rdi)
        movq	(%rsp), %rax
        movq	8(%rsp), %rdx
        movq	16(%rsp), %r10
        movq	24(%rsp), %r11
        movq	%rax, (%rdi)
        movq	%rdx, 8(%rdi)
        movq	%r10, 16(%rdi)
        movq	%r11, 24(%rdi)
        movq	32(%rsp), %rax
        movq	40(%rsp), %rdx
        movq	48(%rsp), %r10
        movq	56(%rsp), %r11
        movq	%rax, 32(%rdi)
        movq	%rdx, 40(%rdi)
        movq	%r10, 48(%rdi)
        movq	%r11, 56(%rdi)
        movq	64(%rsp), %rax
        movq	72(%rsp), %rdx
        movq	80(%rsp), %r10
        movq	88(%rsp), %r11
        movq	%rax, 64(%rdi)
        movq	%rdx, 72(%rdi)
        movq	%r10, 80(%rdi)
        movq	%r11, 88(%rdi)
        movq	96(%rsp), %rax
        movq	104(%rsp), %rdx
        movq	112(%rsp), %r10
        movq	120(%rsp), %r11
        movq	%rax, 96(%rdi)
        movq	%rdx, 104(%rdi)
        movq	%r10, 112(%rdi)
        movq	%r11, 120(%rdi)
        movq	128(%rsp), %rax
        movq	136(%rsp), %rdx
        movq	144(%rsp), %r10
        movq	152(%rsp), %r11
        movq	%rax, 128(%rdi)
        movq	%rdx, 136(%rdi)
        movq	%r10, 144(%rdi)
        movq	%r11, 152(%rdi)
        movq	160(%rsp), %rax
        movq	168(%rsp), %rdx
        movq	176(%rsp), %r10
        movq	184(%rsp), %r11
        movq	%rax, 160(%rdi)
        movq	%rdx, 168(%rdi)
        movq	%r10, 176(%rdi)
        movq	%r11, 184(%rdi)
        addq	$192, %rsp
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_3072_sqr_24,.-sp_3072_sqr_24
#endif /* __APPLE__ */
#ifdef HAVE_INTEL_AVX2
/* Multiply a and b into r. (r = a * b)
 *
 * r   Result of multiplication.
 * a   First number to multiply.
 * b   Second number to multiply.
 */
#ifndef __APPLE__
.globl	sp_3072_mul_avx2_24
.type	sp_3072_mul_avx2_24,@function
.align	16
sp_3072_mul_avx2_24:
#else
.globl	_sp_3072_mul_avx2_24
.p2align	4
_sp_3072_mul_avx2_24:
#endif /* __APPLE__ */
        push	%rbp
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        push	%rbx
        movq	%rdx, %rbp
        subq	$192, %rsp
        movq	$0, %r14
        movq	(%rsi), %rdx
        # A[0] * B[0]
        mulx	(%rbp), %r8, %r9
        # A[0] * B[1]
        mulx	8(%rbp), %rax, %r10
        movq	%r8, (%rsp)
        adcxq	%rax, %r9
        # A[0] * B[2]
        mulx	16(%rbp), %rax, %r11
        movq	%r9, 8(%rsp)
        adcxq	%rax, %r10
        # A[0] * B[3]
        mulx	24(%rbp), %rax, %r12
        movq	%r10, 16(%rsp)
        adcxq	%rax, %r11
        movq	%r11, 24(%rsp)
        # A[0] * B[4]
        mulx	32(%rbp), %rax, %r8
        adcxq	%rax, %r12
        # A[0] * B[5]
        mulx	40(%rbp), %rax, %r9
        movq	%r12, 32(%rsp)
        adcxq	%rax, %r8
        # A[0] * B[6]
        mulx	48(%rbp), %rax, %r10
        movq	%r8, 40(%rsp)
        adcxq	%rax, %r9
        # A[0] * B[7]
        mulx	56(%rbp), %rax, %r11
        movq	%r9, 48(%rsp)
        adcxq	%rax, %r10
        movq	%r10, 56(%rsp)
        # A[0] * B[8]
        mulx	64(%rbp), %rax, %r12
        adcxq	%rax, %r11
        # A[0] * B[9]
        mulx	72(%rbp), %rax, %r8
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        # A[0] * B[10]
        mulx	80(%rbp), %rax, %r9
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        # A[0] * B[11]
        mulx	88(%rbp), %rax, %r10
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        movq	%r9, 88(%rsp)
        # A[0] * B[12]
        mulx	96(%rbp), %rax, %r11
        adcxq	%rax, %r10
        # A[0] * B[13]
        mulx	104(%rbp), %rax, %r12
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        # A[0] * B[14]
        mulx	112(%rbp), %rax, %r8
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        # A[0] * B[15]
        mulx	120(%rbp), %rax, %r9
        movq	%r12, 112(%rsp)
        adcxq	%rax, %r8
        movq	%r8, 120(%rsp)
        # A[0] * B[16]
        mulx	128(%rbp), %rax, %r10
        adcxq	%rax, %r9
        # A[0] * B[17]
        mulx	136(%rbp), %rax, %r11
        movq	%r9, 128(%rsp)
        adcxq	%rax, %r10
        # A[0] * B[18]
        mulx	144(%rbp), %rax, %r12
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        # A[0] * B[19]
        mulx	152(%rbp), %rax, %r8
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        movq	%r12, 152(%rsp)
        # A[0] * B[20]
        mulx	160(%rbp), %rax, %r9
        adcxq	%rax, %r8
        # A[0] * B[21]
        mulx	168(%rbp), %rax, %r10
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        # A[0] * B[22]
        mulx	176(%rbp), %rax, %r11
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        # A[0] * B[23]
        mulx	184(%rbp), %rax, %r12
        movq	%r10, %r15
        adcxq	%rax, %r11
        adcxq	%r14, %r12
        movq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r11, %rbx
        movq	%r12, 192(%rdi)
        movq	8(%rsi), %rdx
        movq	8(%rsp), %r9
        movq	16(%rsp), %r10
        movq	24(%rsp), %r11
        movq	32(%rsp), %r12
        movq	40(%rsp), %r8
        # A[1] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[1] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r9, 8(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[1] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r10, 16(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[1] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r11, 24(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 32(%rsp)
        movq	48(%rsp), %r9
        movq	56(%rsp), %r10
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        # A[1] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[1] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r8, 40(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[1] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r9, 48(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[1] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r10, 56(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 64(%rsp)
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        # A[1] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[1] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[1] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[1] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 96(%rsp)
        movq	112(%rsp), %r12
        movq	120(%rsp), %r8
        movq	128(%rsp), %r9
        movq	136(%rsp), %r10
        # A[1] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[1] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[1] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r12, 112(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[1] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r8, 120(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 128(%rsp)
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        # A[1] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[1] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[1] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[1] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 160(%rsp)
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        # A[1] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[1] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[1] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[1] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r11, %rbx
        movq	%r14, %r8
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        adcxq	%r13, %r8
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r12, 192(%rdi)
        movq	%r8, 200(%rdi)
        movq	16(%rsi), %rdx
        movq	16(%rsp), %r10
        movq	24(%rsp), %r11
        movq	32(%rsp), %r12
        movq	40(%rsp), %r8
        movq	48(%rsp), %r9
        # A[2] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[2] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r10, 16(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[2] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r11, 24(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[2] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r12, 32(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 40(%rsp)
        movq	56(%rsp), %r10
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        # A[2] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[2] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r9, 48(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[2] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r10, 56(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[2] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 72(%rsp)
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	112(%rsp), %r12
        # A[2] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[2] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[2] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[2] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 104(%rsp)
        movq	120(%rsp), %r8
        movq	128(%rsp), %r9
        movq	136(%rsp), %r10
        movq	144(%rsp), %r11
        # A[2] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[2] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r12, 112(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[2] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r8, 120(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[2] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r9, 128(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 136(%rsp)
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        movq	%r15, %r10
        # A[2] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[2] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[2] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[2] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 168(%rsp)
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        # A[2] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[2] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[2] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[2] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        movq	%r14, %r9
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        adcxq	%r13, %r9
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r8, 200(%rdi)
        movq	%r9, 208(%rdi)
        movq	24(%rsi), %rdx
        movq	24(%rsp), %r11
        movq	32(%rsp), %r12
        movq	40(%rsp), %r8
        movq	48(%rsp), %r9
        movq	56(%rsp), %r10
        # A[3] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[3] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r11, 24(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[3] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r12, 32(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[3] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r8, 40(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 48(%rsp)
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        # A[3] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[3] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r10, 56(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[3] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[3] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 80(%rsp)
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	112(%rsp), %r12
        movq	120(%rsp), %r8
        # A[3] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[3] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[3] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[3] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 112(%rsp)
        movq	128(%rsp), %r9
        movq	136(%rsp), %r10
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        # A[3] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[3] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r8, 120(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[3] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r9, 128(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[3] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 144(%rsp)
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        movq	%r15, %r10
        movq	%rbx, %r11
        # A[3] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[3] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[3] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[3] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, %r15
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        # A[3] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[3] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[3] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[3] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        movq	%r14, %r10
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        adcxq	%r13, %r10
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r9, 208(%rdi)
        movq	%r10, 216(%rdi)
        movq	32(%rsi), %rdx
        movq	32(%rsp), %r12
        movq	40(%rsp), %r8
        movq	48(%rsp), %r9
        movq	56(%rsp), %r10
        movq	64(%rsp), %r11
        # A[4] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[4] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r12, 32(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[4] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r8, 40(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[4] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r9, 48(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 56(%rsp)
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        # A[4] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[4] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[4] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[4] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 88(%rsp)
        movq	104(%rsp), %r11
        movq	112(%rsp), %r12
        movq	120(%rsp), %r8
        movq	128(%rsp), %r9
        # A[4] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[4] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[4] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[4] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r12, 112(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 120(%rsp)
        movq	136(%rsp), %r10
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        # A[4] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[4] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r9, 128(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[4] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[4] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 152(%rsp)
        movq	168(%rsp), %r9
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        # A[4] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[4] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[4] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[4] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, %rbx
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        # A[4] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[4] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[4] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[4] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        movq	%r14, %r11
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        adcxq	%r13, %r11
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r10, 216(%rdi)
        movq	%r11, 224(%rdi)
        movq	40(%rsi), %rdx
        movq	40(%rsp), %r8
        movq	48(%rsp), %r9
        movq	56(%rsp), %r10
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        # A[5] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[5] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r8, 40(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[5] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r9, 48(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[5] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r10, 56(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 64(%rsp)
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        # A[5] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[5] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[5] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[5] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 96(%rsp)
        movq	112(%rsp), %r12
        movq	120(%rsp), %r8
        movq	128(%rsp), %r9
        movq	136(%rsp), %r10
        # A[5] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[5] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[5] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r12, 112(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[5] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r8, 120(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 128(%rsp)
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        # A[5] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[5] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[5] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[5] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 160(%rsp)
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        # A[5] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[5] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[5] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[5] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 192(%rdi)
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        # A[5] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[5] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[5] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[5] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        movq	%r14, %r12
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        adcxq	%r13, %r12
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r11, 224(%rdi)
        movq	%r12, 232(%rdi)
        movq	48(%rsi), %rdx
        movq	48(%rsp), %r9
        movq	56(%rsp), %r10
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        # A[6] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[6] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r9, 48(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[6] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r10, 56(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[6] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 72(%rsp)
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	112(%rsp), %r12
        # A[6] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[6] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[6] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[6] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 104(%rsp)
        movq	120(%rsp), %r8
        movq	128(%rsp), %r9
        movq	136(%rsp), %r10
        movq	144(%rsp), %r11
        # A[6] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[6] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r12, 112(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[6] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r8, 120(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[6] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r9, 128(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 136(%rsp)
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        movq	%r15, %r10
        # A[6] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[6] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[6] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[6] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 168(%rsp)
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        # A[6] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[6] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[6] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[6] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 200(%rdi)
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        # A[6] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[6] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[6] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[6] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        movq	%r14, %r8
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        adcxq	%r13, %r8
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r12, 232(%rdi)
        movq	%r8, 240(%rdi)
        movq	56(%rsi), %rdx
        movq	56(%rsp), %r10
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        # A[7] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[7] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r10, 56(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[7] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[7] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 80(%rsp)
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	112(%rsp), %r12
        movq	120(%rsp), %r8
        # A[7] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[7] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[7] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[7] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 112(%rsp)
        movq	128(%rsp), %r9
        movq	136(%rsp), %r10
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        # A[7] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[7] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r8, 120(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[7] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r9, 128(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[7] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 144(%rsp)
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        movq	%r15, %r10
        movq	%rbx, %r11
        # A[7] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[7] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[7] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[7] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, %r15
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        # A[7] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[7] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[7] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[7] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 208(%rdi)
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        # A[7] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[7] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[7] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[7] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        movq	%r14, %r9
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        adcxq	%r13, %r9
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r8, 240(%rdi)
        movq	%r9, 248(%rdi)
        movq	64(%rsi), %rdx
        movq	64(%rsp), %r11
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        # A[8] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[8] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r11, 64(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[8] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[8] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 88(%rsp)
        movq	104(%rsp), %r11
        movq	112(%rsp), %r12
        movq	120(%rsp), %r8
        movq	128(%rsp), %r9
        # A[8] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[8] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[8] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[8] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r12, 112(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 120(%rsp)
        movq	136(%rsp), %r10
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        # A[8] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[8] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r9, 128(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[8] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[8] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 152(%rsp)
        movq	168(%rsp), %r9
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        # A[8] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[8] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[8] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[8] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, %rbx
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        # A[8] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[8] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[8] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[8] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 216(%rdi)
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        # A[8] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[8] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[8] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[8] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r8, 240(%rdi)
        movq	%r14, %r10
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        adcxq	%r13, %r10
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r9, 248(%rdi)
        movq	%r10, 256(%rdi)
        movq	72(%rsi), %rdx
        movq	72(%rsp), %r12
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        # A[9] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[9] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r12, 72(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[9] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[9] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 96(%rsp)
        movq	112(%rsp), %r12
        movq	120(%rsp), %r8
        movq	128(%rsp), %r9
        movq	136(%rsp), %r10
        # A[9] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[9] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[9] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r12, 112(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[9] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r8, 120(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 128(%rsp)
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        # A[9] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[9] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[9] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[9] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 160(%rsp)
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        # A[9] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[9] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[9] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[9] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 192(%rdi)
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        # A[9] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[9] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[9] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[9] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 224(%rdi)
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        movq	256(%rdi), %r10
        # A[9] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[9] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[9] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r8, 240(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[9] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r9, 248(%rdi)
        movq	%r14, %r11
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        adcxq	%r13, %r11
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r10, 256(%rdi)
        movq	%r11, 264(%rdi)
        movq	80(%rsi), %rdx
        movq	80(%rsp), %r8
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	112(%rsp), %r12
        # A[10] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[10] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r8, 80(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[10] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[10] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 104(%rsp)
        movq	120(%rsp), %r8
        movq	128(%rsp), %r9
        movq	136(%rsp), %r10
        movq	144(%rsp), %r11
        # A[10] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[10] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r12, 112(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[10] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r8, 120(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[10] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r9, 128(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 136(%rsp)
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        movq	%r15, %r10
        # A[10] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[10] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[10] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[10] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 168(%rsp)
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        # A[10] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[10] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[10] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[10] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 200(%rdi)
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        # A[10] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[10] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[10] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[10] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 232(%rdi)
        movq	248(%rdi), %r9
        movq	256(%rdi), %r10
        movq	264(%rdi), %r11
        # A[10] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[10] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r8, 240(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[10] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r9, 248(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[10] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r10, 256(%rdi)
        movq	%r14, %r12
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        adcxq	%r13, %r12
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r11, 264(%rdi)
        movq	%r12, 272(%rdi)
        movq	88(%rsi), %rdx
        movq	88(%rsp), %r9
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	112(%rsp), %r12
        movq	120(%rsp), %r8
        # A[11] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r9, 88(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[11] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[11] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 112(%rsp)
        movq	128(%rsp), %r9
        movq	136(%rsp), %r10
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        # A[11] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[11] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r8, 120(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r9, 128(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[11] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 144(%rsp)
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        movq	%r15, %r10
        movq	%rbx, %r11
        # A[11] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[11] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[11] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, %r15
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        # A[11] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[11] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[11] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[11] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 208(%rdi)
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        # A[11] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[11] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[11] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[11] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 240(%rdi)
        movq	256(%rdi), %r10
        movq	264(%rdi), %r11
        movq	272(%rdi), %r12
        # A[11] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r9, 248(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[11] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r10, 256(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[11] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r11, 264(%rdi)
        movq	%r14, %r8
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        adcxq	%r13, %r8
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r12, 272(%rdi)
        movq	%r8, 280(%rdi)
        movq	96(%rsi), %rdx
        movq	96(%rsp), %r10
        movq	104(%rsp), %r11
        movq	112(%rsp), %r12
        movq	120(%rsp), %r8
        movq	128(%rsp), %r9
        # A[12] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r10, 96(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[12] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[12] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r12, 112(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 120(%rsp)
        movq	136(%rsp), %r10
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        # A[12] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[12] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r9, 128(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[12] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 152(%rsp)
        movq	168(%rsp), %r9
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        # A[12] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[12] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[12] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, %rbx
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        # A[12] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[12] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[12] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[12] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 216(%rdi)
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        movq	256(%rdi), %r10
        # A[12] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[12] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[12] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[12] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r8, 240(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 248(%rdi)
        movq	264(%rdi), %r11
        movq	272(%rdi), %r12
        movq	280(%rdi), %r8
        # A[12] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r10, 256(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[12] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r11, 264(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[12] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r12, 272(%rdi)
        movq	%r14, %r9
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        adcxq	%r13, %r9
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r8, 280(%rdi)
        movq	%r9, 288(%rdi)
        movq	104(%rsi), %rdx
        movq	104(%rsp), %r11
        movq	112(%rsp), %r12
        movq	120(%rsp), %r8
        movq	128(%rsp), %r9
        movq	136(%rsp), %r10
        # A[13] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[13] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r11, 104(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[13] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r12, 112(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[13] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r8, 120(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 128(%rsp)
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        # A[13] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[13] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[13] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[13] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 160(%rsp)
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        # A[13] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[13] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[13] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[13] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 192(%rdi)
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        # A[13] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[13] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[13] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[13] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 224(%rdi)
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        movq	256(%rdi), %r10
        movq	264(%rdi), %r11
        # A[13] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[13] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[13] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r8, 240(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[13] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r9, 248(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 256(%rdi)
        movq	272(%rdi), %r12
        movq	280(%rdi), %r8
        movq	288(%rdi), %r9
        # A[13] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[13] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r11, 264(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[13] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r12, 272(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[13] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r8, 280(%rdi)
        movq	%r14, %r10
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        adcxq	%r13, %r10
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r9, 288(%rdi)
        movq	%r10, 296(%rdi)
        movq	112(%rsi), %rdx
        movq	112(%rsp), %r12
        movq	120(%rsp), %r8
        movq	128(%rsp), %r9
        movq	136(%rsp), %r10
        movq	144(%rsp), %r11
        # A[14] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[14] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r12, 112(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r8, 120(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[14] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r9, 128(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 136(%rsp)
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        movq	%r15, %r10
        # A[14] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[14] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[14] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 168(%rsp)
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        # A[14] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[14] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[14] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[14] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 200(%rdi)
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        # A[14] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[14] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[14] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[14] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 232(%rdi)
        movq	248(%rdi), %r9
        movq	256(%rdi), %r10
        movq	264(%rdi), %r11
        movq	272(%rdi), %r12
        # A[14] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r8, 240(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[14] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r9, 248(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[14] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r10, 256(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 264(%rdi)
        movq	280(%rdi), %r8
        movq	288(%rdi), %r9
        movq	296(%rdi), %r10
        # A[14] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[14] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r12, 272(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r8, 280(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[14] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r9, 288(%rdi)
        movq	%r14, %r11
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        adcxq	%r13, %r11
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r10, 296(%rdi)
        movq	%r11, 304(%rdi)
        movq	120(%rsi), %rdx
        movq	120(%rsp), %r8
        movq	128(%rsp), %r9
        movq	136(%rsp), %r10
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        # A[15] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[15] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r8, 120(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[15] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r9, 128(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[15] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 144(%rsp)
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        movq	%r15, %r10
        movq	%rbx, %r11
        # A[15] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[15] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[15] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[15] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, %r15
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        # A[15] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[15] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[15] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[15] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 208(%rdi)
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        # A[15] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[15] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[15] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[15] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 240(%rdi)
        movq	256(%rdi), %r10
        movq	264(%rdi), %r11
        movq	272(%rdi), %r12
        movq	280(%rdi), %r8
        # A[15] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[15] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r9, 248(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[15] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r10, 256(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[15] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r11, 264(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 272(%rdi)
        movq	288(%rdi), %r9
        movq	296(%rdi), %r10
        movq	304(%rdi), %r11
        # A[15] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[15] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r8, 280(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[15] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r9, 288(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[15] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r10, 296(%rdi)
        movq	%r14, %r12
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        adcxq	%r13, %r12
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r11, 304(%rdi)
        movq	%r12, 312(%rdi)
        movq	128(%rsi), %rdx
        movq	128(%rsp), %r9
        movq	136(%rsp), %r10
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        # A[16] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[16] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r9, 128(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[16] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[16] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 152(%rsp)
        movq	168(%rsp), %r9
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        # A[16] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[16] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[16] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[16] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, %rbx
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        # A[16] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[16] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[16] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[16] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 216(%rdi)
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        movq	256(%rdi), %r10
        # A[16] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[16] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[16] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[16] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r8, 240(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 248(%rdi)
        movq	264(%rdi), %r11
        movq	272(%rdi), %r12
        movq	280(%rdi), %r8
        movq	288(%rdi), %r9
        # A[16] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[16] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r10, 256(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[16] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r11, 264(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[16] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r12, 272(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 280(%rdi)
        movq	296(%rdi), %r10
        movq	304(%rdi), %r11
        movq	312(%rdi), %r12
        # A[16] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[16] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r9, 288(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[16] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r10, 296(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[16] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r11, 304(%rdi)
        movq	%r14, %r8
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        adcxq	%r13, %r8
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r12, 312(%rdi)
        movq	%r8, 320(%rdi)
        movq	136(%rsi), %rdx
        movq	136(%rsp), %r10
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        # A[17] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[17] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r10, 136(%rsp)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[17] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[17] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 160(%rsp)
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        # A[17] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[17] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[17] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[17] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 192(%rdi)
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        # A[17] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[17] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[17] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[17] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 224(%rdi)
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        movq	256(%rdi), %r10
        movq	264(%rdi), %r11
        # A[17] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[17] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[17] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r8, 240(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[17] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r9, 248(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 256(%rdi)
        movq	272(%rdi), %r12
        movq	280(%rdi), %r8
        movq	288(%rdi), %r9
        movq	296(%rdi), %r10
        # A[17] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[17] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r11, 264(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[17] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r12, 272(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[17] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r8, 280(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 288(%rdi)
        movq	304(%rdi), %r11
        movq	312(%rdi), %r12
        movq	320(%rdi), %r8
        # A[17] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[17] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r10, 296(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[17] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r11, 304(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[17] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r12, 312(%rdi)
        movq	%r14, %r9
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        adcxq	%r13, %r9
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r8, 320(%rdi)
        movq	%r9, 328(%rdi)
        movq	144(%rsi), %rdx
        movq	144(%rsp), %r11
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        movq	%r15, %r10
        # A[18] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[18] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r11, 144(%rsp)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[18] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[18] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 168(%rsp)
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        # A[18] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[18] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[18] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[18] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 200(%rdi)
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        # A[18] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[18] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[18] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[18] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 232(%rdi)
        movq	248(%rdi), %r9
        movq	256(%rdi), %r10
        movq	264(%rdi), %r11
        movq	272(%rdi), %r12
        # A[18] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[18] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r8, 240(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[18] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r9, 248(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[18] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r10, 256(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 264(%rdi)
        movq	280(%rdi), %r8
        movq	288(%rdi), %r9
        movq	296(%rdi), %r10
        movq	304(%rdi), %r11
        # A[18] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[18] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r12, 272(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[18] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r8, 280(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[18] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r9, 288(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 296(%rdi)
        movq	312(%rdi), %r12
        movq	320(%rdi), %r8
        movq	328(%rdi), %r9
        # A[18] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[18] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r11, 304(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[18] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r12, 312(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[18] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r8, 320(%rdi)
        movq	%r14, %r10
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        adcxq	%r13, %r10
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r9, 328(%rdi)
        movq	%r10, 336(%rdi)
        movq	152(%rsi), %rdx
        movq	152(%rsp), %r12
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        movq	%r15, %r10
        movq	%rbx, %r11
        # A[19] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[19] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r12, 152(%rsp)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[19] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[19] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, %r15
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        # A[19] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[19] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[19] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[19] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 208(%rdi)
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        # A[19] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[19] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[19] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[19] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 240(%rdi)
        movq	256(%rdi), %r10
        movq	264(%rdi), %r11
        movq	272(%rdi), %r12
        movq	280(%rdi), %r8
        # A[19] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[19] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r9, 248(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[19] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r10, 256(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[19] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r11, 264(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 272(%rdi)
        movq	288(%rdi), %r9
        movq	296(%rdi), %r10
        movq	304(%rdi), %r11
        movq	312(%rdi), %r12
        # A[19] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[19] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r8, 280(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[19] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r9, 288(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[19] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r10, 296(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 304(%rdi)
        movq	320(%rdi), %r8
        movq	328(%rdi), %r9
        movq	336(%rdi), %r10
        # A[19] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[19] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r12, 312(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[19] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r8, 320(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[19] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r9, 328(%rdi)
        movq	%r14, %r11
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        adcxq	%r13, %r11
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r10, 336(%rdi)
        movq	%r11, 344(%rdi)
        movq	160(%rsi), %rdx
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        # A[20] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[20] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r8, 160(%rsp)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[20] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[20] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, %rbx
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        # A[20] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[20] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[20] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[20] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 216(%rdi)
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        movq	256(%rdi), %r10
        # A[20] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[20] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[20] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[20] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r8, 240(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 248(%rdi)
        movq	264(%rdi), %r11
        movq	272(%rdi), %r12
        movq	280(%rdi), %r8
        movq	288(%rdi), %r9
        # A[20] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[20] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r10, 256(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[20] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r11, 264(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[20] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r12, 272(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 280(%rdi)
        movq	296(%rdi), %r10
        movq	304(%rdi), %r11
        movq	312(%rdi), %r12
        movq	320(%rdi), %r8
        # A[20] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[20] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r9, 288(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[20] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r10, 296(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[20] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r11, 304(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 312(%rdi)
        movq	328(%rdi), %r9
        movq	336(%rdi), %r10
        movq	344(%rdi), %r11
        # A[20] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[20] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r8, 320(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[20] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r9, 328(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[20] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r10, 336(%rdi)
        movq	%r14, %r12
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        adcxq	%r13, %r12
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r11, 344(%rdi)
        movq	%r12, 352(%rdi)
        movq	168(%rsi), %rdx
        movq	168(%rsp), %r9
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        # A[21] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[21] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r9, 168(%rsp)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[21] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[21] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 192(%rdi)
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        # A[21] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[21] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[21] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[21] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 224(%rdi)
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        movq	256(%rdi), %r10
        movq	264(%rdi), %r11
        # A[21] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[21] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[21] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r8, 240(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[21] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r9, 248(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 256(%rdi)
        movq	272(%rdi), %r12
        movq	280(%rdi), %r8
        movq	288(%rdi), %r9
        movq	296(%rdi), %r10
        # A[21] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[21] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r11, 264(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[21] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r12, 272(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[21] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r8, 280(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 288(%rdi)
        movq	304(%rdi), %r11
        movq	312(%rdi), %r12
        movq	320(%rdi), %r8
        movq	328(%rdi), %r9
        # A[21] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[21] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r10, 296(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[21] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r11, 304(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[21] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r12, 312(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 320(%rdi)
        movq	336(%rdi), %r10
        movq	344(%rdi), %r11
        movq	352(%rdi), %r12
        # A[21] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[21] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r9, 328(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[21] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r10, 336(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[21] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r11, 344(%rdi)
        movq	%r14, %r8
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        adcxq	%r13, %r8
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r12, 352(%rdi)
        movq	%r8, 360(%rdi)
        movq	176(%rsi), %rdx
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        # A[22] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[22] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r10, %r15
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[22] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[22] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 200(%rdi)
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        # A[22] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[22] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r9, 208(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[22] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[22] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 232(%rdi)
        movq	248(%rdi), %r9
        movq	256(%rdi), %r10
        movq	264(%rdi), %r11
        movq	272(%rdi), %r12
        # A[22] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[22] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r8, 240(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[22] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r9, 248(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[22] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r10, 256(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 264(%rdi)
        movq	280(%rdi), %r8
        movq	288(%rdi), %r9
        movq	296(%rdi), %r10
        movq	304(%rdi), %r11
        # A[22] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[22] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r12, 272(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[22] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r8, 280(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[22] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r9, 288(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 296(%rdi)
        movq	312(%rdi), %r12
        movq	320(%rdi), %r8
        movq	328(%rdi), %r9
        movq	336(%rdi), %r10
        # A[22] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[22] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r11, 304(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[22] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r12, 312(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[22] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r8, 320(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 328(%rdi)
        movq	344(%rdi), %r11
        movq	352(%rdi), %r12
        movq	360(%rdi), %r8
        # A[22] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[22] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r10, 336(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[22] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r11, 344(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[22] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r12, 352(%rdi)
        movq	%r14, %r9
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        adcxq	%r13, %r9
        movq	%r14, %r13
        adoxq	%r14, %r13
        adcxq	%r14, %r13
        movq	%r8, 360(%rdi)
        movq	%r9, 368(%rdi)
        movq	184(%rsi), %rdx
        movq	%rbx, %r11
        movq	192(%rdi), %r12
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        # A[23] * B[0]
        mulx	(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[23] * B[1]
        mulx	8(%rbp), %rax, %rcx
        movq	%r11, %rbx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[23] * B[2]
        mulx	16(%rbp), %rax, %rcx
        movq	%r12, 192(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[23] * B[3]
        mulx	24(%rbp), %rax, %rcx
        movq	%r8, 200(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r9, 208(%rdi)
        movq	224(%rdi), %r11
        movq	232(%rdi), %r12
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        # A[23] * B[4]
        mulx	32(%rbp), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[23] * B[5]
        mulx	40(%rbp), %rax, %rcx
        movq	%r10, 216(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[23] * B[6]
        mulx	48(%rbp), %rax, %rcx
        movq	%r11, 224(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[23] * B[7]
        mulx	56(%rbp), %rax, %rcx
        movq	%r12, 232(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r8, 240(%rdi)
        movq	256(%rdi), %r10
        movq	264(%rdi), %r11
        movq	272(%rdi), %r12
        movq	280(%rdi), %r8
        # A[23] * B[8]
        mulx	64(%rbp), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[23] * B[9]
        mulx	72(%rbp), %rax, %rcx
        movq	%r9, 248(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[23] * B[10]
        mulx	80(%rbp), %rax, %rcx
        movq	%r10, 256(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[23] * B[11]
        mulx	88(%rbp), %rax, %rcx
        movq	%r11, 264(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        movq	%r12, 272(%rdi)
        movq	288(%rdi), %r9
        movq	296(%rdi), %r10
        movq	304(%rdi), %r11
        movq	312(%rdi), %r12
        # A[23] * B[12]
        mulx	96(%rbp), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[23] * B[13]
        mulx	104(%rbp), %rax, %rcx
        movq	%r8, 280(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[23] * B[14]
        mulx	112(%rbp), %rax, %rcx
        movq	%r9, 288(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[23] * B[15]
        mulx	120(%rbp), %rax, %rcx
        movq	%r10, 296(%rdi)
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        movq	%r11, 304(%rdi)
        movq	320(%rdi), %r8
        movq	328(%rdi), %r9
        movq	336(%rdi), %r10
        movq	344(%rdi), %r11
        # A[23] * B[16]
        mulx	128(%rbp), %rax, %rcx
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[23] * B[17]
        mulx	136(%rbp), %rax, %rcx
        movq	%r12, 312(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[23] * B[18]
        mulx	144(%rbp), %rax, %rcx
        movq	%r8, 320(%rdi)
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[23] * B[19]
        mulx	152(%rbp), %rax, %rcx
        movq	%r9, 328(%rdi)
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r10, 336(%rdi)
        movq	352(%rdi), %r12
        movq	360(%rdi), %r8
        movq	368(%rdi), %r9
        # A[23] * B[20]
        mulx	160(%rbp), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r12
        # A[23] * B[21]
        mulx	168(%rbp), %rax, %rcx
        movq	%r11, 344(%rdi)
        adcxq	%rax, %r12
        adoxq	%rcx, %r8
        # A[23] * B[22]
        mulx	176(%rbp), %rax, %rcx
        movq	%r12, 352(%rdi)
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[23] * B[23]
        mulx	184(%rbp), %rax, %rcx
        movq	%r8, 360(%rdi)
        movq	%r14, %r10
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        adcxq	%r13, %r10
        movq	%r9, 368(%rdi)
        movq	%r10, 376(%rdi)
        movq	(%rsp), %r8
        movq	8(%rsp), %r9
        movq	16(%rsp), %r10
        movq	24(%rsp), %r11
        movq	%r8, (%rdi)
        movq	%r9, 8(%rdi)
        movq	%r10, 16(%rdi)
        movq	%r11, 24(%rdi)
        movq	32(%rsp), %r8
        movq	40(%rsp), %r9
        movq	48(%rsp), %r10
        movq	56(%rsp), %r11
        movq	%r8, 32(%rdi)
        movq	%r9, 40(%rdi)
        movq	%r10, 48(%rdi)
        movq	%r11, 56(%rdi)
        movq	64(%rsp), %r8
        movq	72(%rsp), %r9
        movq	80(%rsp), %r10
        movq	88(%rsp), %r11
        movq	%r8, 64(%rdi)
        movq	%r9, 72(%rdi)
        movq	%r10, 80(%rdi)
        movq	%r11, 88(%rdi)
        movq	96(%rsp), %r8
        movq	104(%rsp), %r9
        movq	112(%rsp), %r10
        movq	120(%rsp), %r11
        movq	%r8, 96(%rdi)
        movq	%r9, 104(%rdi)
        movq	%r10, 112(%rdi)
        movq	%r11, 120(%rdi)
        movq	128(%rsp), %r8
        movq	136(%rsp), %r9
        movq	144(%rsp), %r10
        movq	152(%rsp), %r11
        movq	%r8, 128(%rdi)
        movq	%r9, 136(%rdi)
        movq	%r10, 144(%rdi)
        movq	%r11, 152(%rdi)
        movq	160(%rsp), %r8
        movq	168(%rsp), %r9
        movq	%r15, %r10
        movq	%rbx, %r11
        movq	%r8, 160(%rdi)
        movq	%r9, 168(%rdi)
        movq	%r10, 176(%rdi)
        movq	%r11, 184(%rdi)
        addq	$192, %rsp
        pop	%rbx
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        pop	%rbp
        repz retq
#ifndef __APPLE__
.size	sp_3072_mul_avx2_24,.-sp_3072_mul_avx2_24
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
#ifdef HAVE_INTEL_AVX2
/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_3072_sqr_avx2_24
.type	sp_3072_sqr_avx2_24,@function
.align	16
sp_3072_sqr_avx2_24:
#else
.globl	_sp_3072_sqr_avx2_24
.p2align	4
_sp_3072_sqr_avx2_24:
#endif /* __APPLE__ */
        push	%rbp
        push	%r12
        push	%r13
        subq	$192, %rsp
        cmpq	%rdi, %rsi
        movq	%rsp, %rbp
        cmovne	%rdi, %rbp
        xorq	%r12, %r12
        # Diagonal 1
        xorq	%r8, %r8
        xorq	%r9, %r9
        xorq	%r10, %r10
        xorq	%r11, %r11
        # A[1] x A[0]
        movq	(%rsi), %rdx
        mulxq	8(%rsi), %r8, %r9
        # A[2] x A[0]
        mulxq	16(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[3] x A[0]
        mulxq	24(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 8(%rbp)
        movq	%r9, 16(%rbp)
        movq	%r10, 24(%rbp)
        movq	%r12, %r8
        movq	%r12, %r9
        movq	%r12, %r10
        # A[4] x A[0]
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[5] x A[0]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[6] x A[0]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 32(%rbp)
        movq	%r8, 40(%rbp)
        movq	%r9, 48(%rbp)
        movq	%r12, %r11
        movq	%r12, %r8
        movq	%r12, %r9
        # A[7] x A[0]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[8] x A[0]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[9] x A[0]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 56(%rbp)
        movq	%r11, 64(%rbp)
        movq	%r8, 72(%rbp)
        movq	%r12, %r10
        movq	%r12, %r11
        movq	%r12, %r8
        # A[10] x A[0]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] x A[0]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] x A[0]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 80(%rbp)
        movq	%r10, 88(%rbp)
        movq	%r11, 96(%rbp)
        movq	%r12, %r9
        movq	%r12, %r10
        movq	%r12, %r11
        # A[13] x A[0]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] x A[0]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[15] x A[0]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 104(%rbp)
        movq	%r9, 112(%rbp)
        movq	%r10, 120(%rbp)
        movq	%r12, %r8
        movq	%r12, %r9
        movq	%r12, %r10
        # A[16] x A[0]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[17] x A[0]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[18] x A[0]
        mulxq	144(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 128(%rbp)
        movq	%r8, 136(%rbp)
        movq	%r9, 144(%rbp)
        movq	%r12, %r11
        movq	%r12, %r8
        movq	%r12, %r9
        # A[19] x A[0]
        mulxq	152(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[20] x A[0]
        mulxq	160(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[21] x A[0]
        mulxq	168(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 152(%rbp)
        movq	%r11, 160(%rbp)
        movq	%r8, 168(%rbp)
        movq	%r12, %r10
        movq	%r12, %r11
        # A[22] x A[0]
        mulxq	176(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[23] x A[0]
        mulxq	184(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r9, 176(%rbp)
        movq	%r10, 184(%rbp)
        #  Carry
        adcxq	%r12, %r11
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r11, 192(%rdi)
        # Diagonal 2
        movq	24(%rbp), %r11
        movq	32(%rbp), %r8
        movq	40(%rbp), %r9
        movq	48(%rbp), %r10
        # A[2] x A[1]
        movq	8(%rsi), %rdx
        mulxq	16(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[3] x A[1]
        mulxq	24(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[4] x A[1]
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 24(%rbp)
        movq	%r8, 32(%rbp)
        movq	%r9, 40(%rbp)
        movq	56(%rbp), %r11
        movq	64(%rbp), %r8
        movq	72(%rbp), %r9
        # A[5] x A[1]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[6] x A[1]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[7] x A[1]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 48(%rbp)
        movq	%r11, 56(%rbp)
        movq	%r8, 64(%rbp)
        movq	80(%rbp), %r10
        movq	88(%rbp), %r11
        movq	96(%rbp), %r8
        # A[8] x A[1]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[9] x A[1]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[10] x A[1]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 72(%rbp)
        movq	%r10, 80(%rbp)
        movq	%r11, 88(%rbp)
        movq	104(%rbp), %r9
        movq	112(%rbp), %r10
        movq	120(%rbp), %r11
        # A[11] x A[1]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[12] x A[1]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[13] x A[1]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 96(%rbp)
        movq	%r9, 104(%rbp)
        movq	%r10, 112(%rbp)
        movq	128(%rbp), %r8
        movq	136(%rbp), %r9
        movq	144(%rbp), %r10
        # A[14] x A[1]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[15] x A[1]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[16] x A[1]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 120(%rbp)
        movq	%r8, 128(%rbp)
        movq	%r9, 136(%rbp)
        movq	152(%rbp), %r11
        movq	160(%rbp), %r8
        movq	168(%rbp), %r9
        # A[17] x A[1]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[18] x A[1]
        mulxq	144(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[19] x A[1]
        mulxq	152(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 144(%rbp)
        movq	%r11, 152(%rbp)
        movq	%r8, 160(%rbp)
        movq	176(%rbp), %r10
        movq	184(%rbp), %r11
        movq	192(%rdi), %r8
        # A[20] x A[1]
        mulxq	160(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[21] x A[1]
        mulxq	168(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[22] x A[1]
        mulxq	176(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 168(%rbp)
        movq	%r10, 176(%rbp)
        movq	%r11, 184(%rbp)
        movq	%r12, %r9
        movq	%r12, %r10
        # A[23] x A[1]
        mulxq	184(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[23] x A[2]
        movq	16(%rsi), %rdx
        mulxq	184(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r8, 192(%rdi)
        movq	%r9, 200(%rdi)
        #  Carry
        adcxq	%r13, %r10
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r10, 208(%rdi)
        # Diagonal 3
        movq	40(%rbp), %r10
        movq	48(%rbp), %r11
        movq	56(%rbp), %r8
        movq	64(%rbp), %r9
        # A[3] x A[2]
        mulxq	24(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[4] x A[2]
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[5] x A[2]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 40(%rbp)
        movq	%r11, 48(%rbp)
        movq	%r8, 56(%rbp)
        movq	72(%rbp), %r10
        movq	80(%rbp), %r11
        movq	88(%rbp), %r8
        # A[6] x A[2]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[7] x A[2]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[8] x A[2]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 64(%rbp)
        movq	%r10, 72(%rbp)
        movq	%r11, 80(%rbp)
        movq	96(%rbp), %r9
        movq	104(%rbp), %r10
        movq	112(%rbp), %r11
        # A[9] x A[2]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[10] x A[2]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] x A[2]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 88(%rbp)
        movq	%r9, 96(%rbp)
        movq	%r10, 104(%rbp)
        movq	120(%rbp), %r8
        movq	128(%rbp), %r9
        movq	136(%rbp), %r10
        # A[12] x A[2]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[13] x A[2]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] x A[2]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 112(%rbp)
        movq	%r8, 120(%rbp)
        movq	%r9, 128(%rbp)
        movq	144(%rbp), %r11
        movq	152(%rbp), %r8
        movq	160(%rbp), %r9
        # A[15] x A[2]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[16] x A[2]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[17] x A[2]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 136(%rbp)
        movq	%r11, 144(%rbp)
        movq	%r8, 152(%rbp)
        movq	168(%rbp), %r10
        movq	176(%rbp), %r11
        movq	184(%rbp), %r8
        # A[18] x A[2]
        mulxq	144(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[19] x A[2]
        mulxq	152(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[20] x A[2]
        mulxq	160(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 160(%rbp)
        movq	%r10, 168(%rbp)
        movq	%r11, 176(%rbp)
        movq	192(%rdi), %r9
        movq	200(%rdi), %r10
        movq	208(%rdi), %r11
        # A[21] x A[2]
        mulxq	168(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[22] x A[2]
        mulxq	176(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[22] x A[3]
        movq	176(%rsi), %rdx
        mulxq	24(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 184(%rbp)
        movq	%r9, 192(%rdi)
        movq	%r10, 200(%rdi)
        movq	%r12, %r8
        movq	%r12, %r9
        # A[22] x A[4]
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[22] x A[5]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r11, 208(%rdi)
        movq	%r8, 216(%rdi)
        #  Carry
        adcxq	%r13, %r9
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r9, 224(%rdi)
        # Diagonal 4
        movq	56(%rbp), %r9
        movq	64(%rbp), %r10
        movq	72(%rbp), %r11
        movq	80(%rbp), %r8
        # A[4] x A[3]
        movq	24(%rsi), %rdx
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[5] x A[3]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[6] x A[3]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 56(%rbp)
        movq	%r10, 64(%rbp)
        movq	%r11, 72(%rbp)
        movq	88(%rbp), %r9
        movq	96(%rbp), %r10
        movq	104(%rbp), %r11
        # A[7] x A[3]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[8] x A[3]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[9] x A[3]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 80(%rbp)
        movq	%r9, 88(%rbp)
        movq	%r10, 96(%rbp)
        movq	112(%rbp), %r8
        movq	120(%rbp), %r9
        movq	128(%rbp), %r10
        # A[10] x A[3]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[11] x A[3]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[12] x A[3]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 104(%rbp)
        movq	%r8, 112(%rbp)
        movq	%r9, 120(%rbp)
        movq	136(%rbp), %r11
        movq	144(%rbp), %r8
        movq	152(%rbp), %r9
        # A[13] x A[3]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[14] x A[3]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[15] x A[3]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 128(%rbp)
        movq	%r11, 136(%rbp)
        movq	%r8, 144(%rbp)
        movq	160(%rbp), %r10
        movq	168(%rbp), %r11
        movq	176(%rbp), %r8
        # A[16] x A[3]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[17] x A[3]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[18] x A[3]
        mulxq	144(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 152(%rbp)
        movq	%r10, 160(%rbp)
        movq	%r11, 168(%rbp)
        movq	184(%rbp), %r9
        movq	192(%rdi), %r10
        movq	200(%rdi), %r11
        # A[19] x A[3]
        mulxq	152(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[20] x A[3]
        mulxq	160(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[21] x A[3]
        mulxq	168(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 176(%rbp)
        movq	%r9, 184(%rbp)
        movq	%r10, 192(%rdi)
        movq	208(%rdi), %r8
        movq	216(%rdi), %r9
        movq	224(%rdi), %r10
        # A[21] x A[4]
        movq	168(%rsi), %rdx
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[21] x A[5]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[21] x A[6]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 200(%rdi)
        movq	%r8, 208(%rdi)
        movq	%r9, 216(%rdi)
        movq	%r12, %r11
        movq	%r12, %r8
        # A[21] x A[7]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[21] x A[8]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r10, 224(%rdi)
        movq	%r11, 232(%rdi)
        #  Carry
        adcxq	%r13, %r8
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r8, 240(%rdi)
        # Diagonal 5
        movq	72(%rbp), %r8
        movq	80(%rbp), %r9
        movq	88(%rbp), %r10
        movq	96(%rbp), %r11
        # A[5] x A[4]
        movq	32(%rsi), %rdx
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[6] x A[4]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[7] x A[4]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 72(%rbp)
        movq	%r9, 80(%rbp)
        movq	%r10, 88(%rbp)
        movq	104(%rbp), %r8
        movq	112(%rbp), %r9
        movq	120(%rbp), %r10
        # A[8] x A[4]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[9] x A[4]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[10] x A[4]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 96(%rbp)
        movq	%r8, 104(%rbp)
        movq	%r9, 112(%rbp)
        movq	128(%rbp), %r11
        movq	136(%rbp), %r8
        movq	144(%rbp), %r9
        # A[11] x A[4]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] x A[4]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[13] x A[4]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 120(%rbp)
        movq	%r11, 128(%rbp)
        movq	%r8, 136(%rbp)
        movq	152(%rbp), %r10
        movq	160(%rbp), %r11
        movq	168(%rbp), %r8
        # A[14] x A[4]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[15] x A[4]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[16] x A[4]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 144(%rbp)
        movq	%r10, 152(%rbp)
        movq	%r11, 160(%rbp)
        movq	176(%rbp), %r9
        movq	184(%rbp), %r10
        movq	192(%rdi), %r11
        # A[17] x A[4]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[18] x A[4]
        mulxq	144(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[19] x A[4]
        mulxq	152(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 168(%rbp)
        movq	%r9, 176(%rbp)
        movq	%r10, 184(%rbp)
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        movq	216(%rdi), %r10
        # A[20] x A[4]
        mulxq	160(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[20] x A[5]
        movq	160(%rsi), %rdx
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[20] x A[6]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 192(%rdi)
        movq	%r8, 200(%rdi)
        movq	%r9, 208(%rdi)
        movq	224(%rdi), %r11
        movq	232(%rdi), %r8
        movq	240(%rdi), %r9
        # A[20] x A[7]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[20] x A[8]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[20] x A[9]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 216(%rdi)
        movq	%r11, 224(%rdi)
        movq	%r8, 232(%rdi)
        movq	%r12, %r10
        movq	%r12, %r11
        # A[20] x A[10]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[20] x A[11]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r9, 240(%rdi)
        movq	%r10, 248(%rdi)
        #  Carry
        adcxq	%r13, %r11
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r11, 256(%rdi)
        # Diagonal 6
        movq	88(%rbp), %r11
        movq	96(%rbp), %r8
        movq	104(%rbp), %r9
        movq	112(%rbp), %r10
        # A[6] x A[5]
        movq	40(%rsi), %rdx
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[7] x A[5]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[8] x A[5]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 88(%rbp)
        movq	%r8, 96(%rbp)
        movq	%r9, 104(%rbp)
        movq	120(%rbp), %r11
        movq	128(%rbp), %r8
        movq	136(%rbp), %r9
        # A[9] x A[5]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[10] x A[5]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[11] x A[5]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 112(%rbp)
        movq	%r11, 120(%rbp)
        movq	%r8, 128(%rbp)
        movq	144(%rbp), %r10
        movq	152(%rbp), %r11
        movq	160(%rbp), %r8
        # A[12] x A[5]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[13] x A[5]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[14] x A[5]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 136(%rbp)
        movq	%r10, 144(%rbp)
        movq	%r11, 152(%rbp)
        movq	168(%rbp), %r9
        movq	176(%rbp), %r10
        movq	184(%rbp), %r11
        # A[15] x A[5]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[16] x A[5]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[17] x A[5]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 160(%rbp)
        movq	%r9, 168(%rbp)
        movq	%r10, 176(%rbp)
        movq	192(%rdi), %r8
        movq	200(%rdi), %r9
        movq	208(%rdi), %r10
        # A[18] x A[5]
        mulxq	144(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[19] x A[5]
        mulxq	152(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[19] x A[6]
        movq	152(%rsi), %rdx
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 184(%rbp)
        movq	%r8, 192(%rdi)
        movq	%r9, 200(%rdi)
        movq	216(%rdi), %r11
        movq	224(%rdi), %r8
        movq	232(%rdi), %r9
        # A[19] x A[7]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[19] x A[8]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[19] x A[9]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 208(%rdi)
        movq	%r11, 216(%rdi)
        movq	%r8, 224(%rdi)
        movq	240(%rdi), %r10
        movq	248(%rdi), %r11
        movq	256(%rdi), %r8
        # A[19] x A[10]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[19] x A[11]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[19] x A[12]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 232(%rdi)
        movq	%r10, 240(%rdi)
        movq	%r11, 248(%rdi)
        movq	%r12, %r9
        movq	%r12, %r10
        # A[19] x A[13]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[19] x A[14]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r8, 256(%rdi)
        movq	%r9, 264(%rdi)
        #  Carry
        adcxq	%r13, %r10
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r10, 272(%rdi)
        # Diagonal 7
        movq	104(%rbp), %r10
        movq	112(%rbp), %r11
        movq	120(%rbp), %r8
        movq	128(%rbp), %r9
        # A[7] x A[6]
        movq	48(%rsi), %rdx
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[8] x A[6]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[9] x A[6]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 104(%rbp)
        movq	%r11, 112(%rbp)
        movq	%r8, 120(%rbp)
        movq	136(%rbp), %r10
        movq	144(%rbp), %r11
        movq	152(%rbp), %r8
        # A[10] x A[6]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] x A[6]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] x A[6]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 128(%rbp)
        movq	%r10, 136(%rbp)
        movq	%r11, 144(%rbp)
        movq	160(%rbp), %r9
        movq	168(%rbp), %r10
        movq	176(%rbp), %r11
        # A[13] x A[6]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] x A[6]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[15] x A[6]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 152(%rbp)
        movq	%r9, 160(%rbp)
        movq	%r10, 168(%rbp)
        movq	184(%rbp), %r8
        movq	192(%rdi), %r9
        movq	200(%rdi), %r10
        # A[16] x A[6]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[17] x A[6]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[18] x A[6]
        mulxq	144(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 176(%rbp)
        movq	%r8, 184(%rbp)
        movq	%r9, 192(%rdi)
        movq	208(%rdi), %r11
        movq	216(%rdi), %r8
        movq	224(%rdi), %r9
        # A[18] x A[7]
        movq	144(%rsi), %rdx
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[18] x A[8]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[18] x A[9]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 200(%rdi)
        movq	%r11, 208(%rdi)
        movq	%r8, 216(%rdi)
        movq	232(%rdi), %r10
        movq	240(%rdi), %r11
        movq	248(%rdi), %r8
        # A[18] x A[10]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[18] x A[11]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[18] x A[12]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 224(%rdi)
        movq	%r10, 232(%rdi)
        movq	%r11, 240(%rdi)
        movq	256(%rdi), %r9
        movq	264(%rdi), %r10
        movq	272(%rdi), %r11
        # A[18] x A[13]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[18] x A[14]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[18] x A[15]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 248(%rdi)
        movq	%r9, 256(%rdi)
        movq	%r10, 264(%rdi)
        movq	%r12, %r8
        movq	%r12, %r9
        # A[18] x A[16]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[18] x A[17]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r11, 272(%rdi)
        movq	%r8, 280(%rdi)
        #  Carry
        adcxq	%r13, %r9
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r9, 288(%rdi)
        # Diagonal 8
        movq	120(%rbp), %r9
        movq	128(%rbp), %r10
        movq	136(%rbp), %r11
        movq	144(%rbp), %r8
        # A[8] x A[7]
        movq	56(%rsi), %rdx
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[9] x A[7]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[10] x A[7]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 120(%rbp)
        movq	%r10, 128(%rbp)
        movq	%r11, 136(%rbp)
        movq	152(%rbp), %r9
        movq	160(%rbp), %r10
        movq	168(%rbp), %r11
        # A[11] x A[7]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[12] x A[7]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[13] x A[7]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 144(%rbp)
        movq	%r9, 152(%rbp)
        movq	%r10, 160(%rbp)
        movq	176(%rbp), %r8
        movq	184(%rbp), %r9
        movq	192(%rdi), %r10
        # A[14] x A[7]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[15] x A[7]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[16] x A[7]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 168(%rbp)
        movq	%r8, 176(%rbp)
        movq	%r9, 184(%rbp)
        movq	200(%rdi), %r11
        movq	208(%rdi), %r8
        movq	216(%rdi), %r9
        # A[17] x A[7]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[17] x A[8]
        movq	136(%rsi), %rdx
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[17] x A[9]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 192(%rdi)
        movq	%r11, 200(%rdi)
        movq	%r8, 208(%rdi)
        movq	224(%rdi), %r10
        movq	232(%rdi), %r11
        movq	240(%rdi), %r8
        # A[17] x A[10]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[17] x A[11]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[17] x A[12]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 216(%rdi)
        movq	%r10, 224(%rdi)
        movq	%r11, 232(%rdi)
        movq	248(%rdi), %r9
        movq	256(%rdi), %r10
        movq	264(%rdi), %r11
        # A[17] x A[13]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[17] x A[14]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[17] x A[15]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 240(%rdi)
        movq	%r9, 248(%rdi)
        movq	%r10, 256(%rdi)
        movq	272(%rdi), %r8
        movq	280(%rdi), %r9
        movq	288(%rdi), %r10
        # A[17] x A[16]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[19] x A[15]
        movq	152(%rsi), %rdx
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[19] x A[16]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 264(%rdi)
        movq	%r8, 272(%rdi)
        movq	%r9, 280(%rdi)
        movq	%r12, %r11
        movq	%r12, %r8
        # A[19] x A[17]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[19] x A[18]
        mulxq	144(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r10, 288(%rdi)
        movq	%r11, 296(%rdi)
        #  Carry
        adcxq	%r13, %r8
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r8, 304(%rdi)
        # Diagonal 9
        movq	136(%rbp), %r8
        movq	144(%rbp), %r9
        movq	152(%rbp), %r10
        movq	160(%rbp), %r11
        # A[9] x A[8]
        movq	64(%rsi), %rdx
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[10] x A[8]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[11] x A[8]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 136(%rbp)
        movq	%r9, 144(%rbp)
        movq	%r10, 152(%rbp)
        movq	168(%rbp), %r8
        movq	176(%rbp), %r9
        movq	184(%rbp), %r10
        # A[12] x A[8]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[13] x A[8]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[14] x A[8]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 160(%rbp)
        movq	%r8, 168(%rbp)
        movq	%r9, 176(%rbp)
        movq	192(%rdi), %r11
        movq	200(%rdi), %r8
        movq	208(%rdi), %r9
        # A[15] x A[8]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[16] x A[8]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[16] x A[9]
        movq	128(%rsi), %rdx
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 184(%rbp)
        movq	%r11, 192(%rdi)
        movq	%r8, 200(%rdi)
        movq	216(%rdi), %r10
        movq	224(%rdi), %r11
        movq	232(%rdi), %r8
        # A[16] x A[10]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[16] x A[11]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[16] x A[12]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 208(%rdi)
        movq	%r10, 216(%rdi)
        movq	%r11, 224(%rdi)
        movq	240(%rdi), %r9
        movq	248(%rdi), %r10
        movq	256(%rdi), %r11
        # A[16] x A[13]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[16] x A[14]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[16] x A[15]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 232(%rdi)
        movq	%r9, 240(%rdi)
        movq	%r10, 248(%rdi)
        movq	264(%rdi), %r8
        movq	272(%rdi), %r9
        movq	280(%rdi), %r10
        # A[20] x A[12]
        movq	160(%rsi), %rdx
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[20] x A[13]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[20] x A[14]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 256(%rdi)
        movq	%r8, 264(%rdi)
        movq	%r9, 272(%rdi)
        movq	288(%rdi), %r11
        movq	296(%rdi), %r8
        movq	304(%rdi), %r9
        # A[20] x A[15]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[20] x A[16]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[20] x A[17]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 280(%rdi)
        movq	%r11, 288(%rdi)
        movq	%r8, 296(%rdi)
        movq	%r12, %r10
        movq	%r12, %r11
        # A[20] x A[18]
        mulxq	144(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[20] x A[19]
        mulxq	152(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r9, 304(%rdi)
        movq	%r10, 312(%rdi)
        #  Carry
        adcxq	%r13, %r11
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r11, 320(%rdi)
        # Diagonal 10
        movq	152(%rbp), %r11
        movq	160(%rbp), %r8
        movq	168(%rbp), %r9
        movq	176(%rbp), %r10
        # A[10] x A[9]
        movq	72(%rsi), %rdx
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[11] x A[9]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[12] x A[9]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 152(%rbp)
        movq	%r8, 160(%rbp)
        movq	%r9, 168(%rbp)
        movq	184(%rbp), %r11
        movq	192(%rdi), %r8
        movq	200(%rdi), %r9
        # A[13] x A[9]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[14] x A[9]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[15] x A[9]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 176(%rbp)
        movq	%r11, 184(%rbp)
        movq	%r8, 192(%rdi)
        movq	208(%rdi), %r10
        movq	216(%rdi), %r11
        movq	224(%rdi), %r8
        # A[15] x A[10]
        movq	120(%rsi), %rdx
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[15] x A[11]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[15] x A[12]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 200(%rdi)
        movq	%r10, 208(%rdi)
        movq	%r11, 216(%rdi)
        movq	232(%rdi), %r9
        movq	240(%rdi), %r10
        movq	248(%rdi), %r11
        # A[15] x A[13]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[15] x A[14]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[21] x A[9]
        movq	168(%rsi), %rdx
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 224(%rdi)
        movq	%r9, 232(%rdi)
        movq	%r10, 240(%rdi)
        movq	256(%rdi), %r8
        movq	264(%rdi), %r9
        movq	272(%rdi), %r10
        # A[21] x A[10]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[21] x A[11]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[21] x A[12]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 248(%rdi)
        movq	%r8, 256(%rdi)
        movq	%r9, 264(%rdi)
        movq	280(%rdi), %r11
        movq	288(%rdi), %r8
        movq	296(%rdi), %r9
        # A[21] x A[13]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[21] x A[14]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[21] x A[15]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 272(%rdi)
        movq	%r11, 280(%rdi)
        movq	%r8, 288(%rdi)
        movq	304(%rdi), %r10
        movq	312(%rdi), %r11
        movq	320(%rdi), %r8
        # A[21] x A[16]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[21] x A[17]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[21] x A[18]
        mulxq	144(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 296(%rdi)
        movq	%r10, 304(%rdi)
        movq	%r11, 312(%rdi)
        movq	%r12, %r9
        movq	%r12, %r10
        # A[21] x A[19]
        mulxq	152(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[21] x A[20]
        mulxq	160(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r8, 320(%rdi)
        movq	%r9, 328(%rdi)
        #  Carry
        adcxq	%r13, %r10
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r10, 336(%rdi)
        # Diagonal 11
        movq	168(%rbp), %r10
        movq	176(%rbp), %r11
        movq	184(%rbp), %r8
        movq	192(%rdi), %r9
        # A[11] x A[10]
        movq	80(%rsi), %rdx
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[12] x A[10]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[13] x A[10]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 168(%rbp)
        movq	%r11, 176(%rbp)
        movq	%r8, 184(%rbp)
        movq	200(%rdi), %r10
        movq	208(%rdi), %r11
        movq	216(%rdi), %r8
        # A[14] x A[10]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[14] x A[11]
        movq	112(%rsi), %rdx
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[14] x A[12]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 192(%rdi)
        movq	%r10, 200(%rdi)
        movq	%r11, 208(%rdi)
        movq	224(%rdi), %r9
        movq	232(%rdi), %r10
        movq	240(%rdi), %r11
        # A[14] x A[13]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[22] x A[6]
        movq	176(%rsi), %rdx
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[22] x A[7]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 216(%rdi)
        movq	%r9, 224(%rdi)
        movq	%r10, 232(%rdi)
        movq	248(%rdi), %r8
        movq	256(%rdi), %r9
        movq	264(%rdi), %r10
        # A[22] x A[8]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[22] x A[9]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[22] x A[10]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 240(%rdi)
        movq	%r8, 248(%rdi)
        movq	%r9, 256(%rdi)
        movq	272(%rdi), %r11
        movq	280(%rdi), %r8
        movq	288(%rdi), %r9
        # A[22] x A[11]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[22] x A[12]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[22] x A[13]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 264(%rdi)
        movq	%r11, 272(%rdi)
        movq	%r8, 280(%rdi)
        movq	296(%rdi), %r10
        movq	304(%rdi), %r11
        movq	312(%rdi), %r8
        # A[22] x A[14]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[22] x A[15]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[22] x A[16]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 288(%rdi)
        movq	%r10, 296(%rdi)
        movq	%r11, 304(%rdi)
        movq	320(%rdi), %r9
        movq	328(%rdi), %r10
        movq	336(%rdi), %r11
        # A[22] x A[17]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[22] x A[18]
        mulxq	144(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[22] x A[19]
        mulxq	152(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 312(%rdi)
        movq	%r9, 320(%rdi)
        movq	%r10, 328(%rdi)
        movq	%r12, %r8
        movq	%r12, %r9
        # A[22] x A[20]
        mulxq	160(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[22] x A[21]
        mulxq	168(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r11, 336(%rdi)
        movq	%r8, 344(%rdi)
        #  Carry
        adcxq	%r13, %r9
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r9, 352(%rdi)
        # Diagonal 12
        movq	184(%rbp), %r9
        movq	192(%rdi), %r10
        movq	200(%rdi), %r11
        movq	208(%rdi), %r8
        # A[12] x A[11]
        movq	88(%rsi), %rdx
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[13] x A[11]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[13] x A[12]
        movq	96(%rsi), %rdx
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 184(%rbp)
        movq	%r10, 192(%rdi)
        movq	%r11, 200(%rdi)
        movq	216(%rdi), %r9
        movq	224(%rdi), %r10
        movq	232(%rdi), %r11
        # A[23] x A[3]
        movq	184(%rsi), %rdx
        mulxq	24(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[23] x A[4]
        mulxq	32(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[23] x A[5]
        mulxq	40(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 208(%rdi)
        movq	%r9, 216(%rdi)
        movq	%r10, 224(%rdi)
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        movq	256(%rdi), %r10
        # A[23] x A[6]
        mulxq	48(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[23] x A[7]
        mulxq	56(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[23] x A[8]
        mulxq	64(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 232(%rdi)
        movq	%r8, 240(%rdi)
        movq	%r9, 248(%rdi)
        movq	264(%rdi), %r11
        movq	272(%rdi), %r8
        movq	280(%rdi), %r9
        # A[23] x A[9]
        mulxq	72(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[23] x A[10]
        mulxq	80(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[23] x A[11]
        mulxq	88(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        movq	%r10, 256(%rdi)
        movq	%r11, 264(%rdi)
        movq	%r8, 272(%rdi)
        movq	288(%rdi), %r10
        movq	296(%rdi), %r11
        movq	304(%rdi), %r8
        # A[23] x A[12]
        mulxq	96(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[23] x A[13]
        mulxq	104(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[23] x A[14]
        mulxq	112(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r9, 280(%rdi)
        movq	%r10, 288(%rdi)
        movq	%r11, 296(%rdi)
        movq	312(%rdi), %r9
        movq	320(%rdi), %r10
        movq	328(%rdi), %r11
        # A[23] x A[15]
        mulxq	120(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[23] x A[16]
        mulxq	128(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        # A[23] x A[17]
        mulxq	136(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        movq	%r8, 304(%rdi)
        movq	%r9, 312(%rdi)
        movq	%r10, 320(%rdi)
        movq	336(%rdi), %r8
        movq	344(%rdi), %r9
        movq	352(%rdi), %r10
        # A[23] x A[18]
        mulxq	144(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        # A[23] x A[19]
        mulxq	152(%rsi), %rax, %rcx
        adcxq	%rax, %r8
        adoxq	%rcx, %r9
        # A[23] x A[20]
        mulxq	160(%rsi), %rax, %rcx
        adcxq	%rax, %r9
        adoxq	%rcx, %r10
        movq	%r11, 328(%rdi)
        movq	%r8, 336(%rdi)
        movq	%r9, 344(%rdi)
        movq	%r12, %r11
        movq	%r12, %r8
        # A[23] x A[21]
        mulxq	168(%rsi), %rax, %rcx
        adcxq	%rax, %r10
        adoxq	%rcx, %r11
        # A[23] x A[22]
        mulxq	176(%rsi), %rax, %rcx
        adcxq	%rax, %r11
        adoxq	%rcx, %r8
        movq	%r10, 352(%rdi)
        movq	%r11, 360(%rdi)
        #  Carry
        adcxq	%r13, %r8
        movq	%r12, %r13
        adcxq	%r12, %r13
        adoxq	%r12, %r13
        movq	%r8, 368(%rdi)
        movq	%r13, 376(%rdi)
        # Double and Add in A[i] x A[i]
        movq	8(%rbp), %r9
        # A[0] x A[0]
        movq	(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        movq	%rax, (%rbp)
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r9, 8(%rbp)
        movq	16(%rbp), %r8
        movq	24(%rbp), %r9
        # A[1] x A[1]
        movq	8(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 16(%rbp)
        movq	%r9, 24(%rbp)
        movq	32(%rbp), %r8
        movq	40(%rbp), %r9
        # A[2] x A[2]
        movq	16(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 32(%rbp)
        movq	%r9, 40(%rbp)
        movq	48(%rbp), %r8
        movq	56(%rbp), %r9
        # A[3] x A[3]
        movq	24(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 48(%rbp)
        movq	%r9, 56(%rbp)
        movq	64(%rbp), %r8
        movq	72(%rbp), %r9
        # A[4] x A[4]
        movq	32(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 64(%rbp)
        movq	%r9, 72(%rbp)
        movq	80(%rbp), %r8
        movq	88(%rbp), %r9
        # A[5] x A[5]
        movq	40(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 80(%rbp)
        movq	%r9, 88(%rbp)
        movq	96(%rbp), %r8
        movq	104(%rbp), %r9
        # A[6] x A[6]
        movq	48(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 96(%rbp)
        movq	%r9, 104(%rbp)
        movq	112(%rbp), %r8
        movq	120(%rbp), %r9
        # A[7] x A[7]
        movq	56(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 112(%rbp)
        movq	%r9, 120(%rbp)
        movq	128(%rbp), %r8
        movq	136(%rbp), %r9
        # A[8] x A[8]
        movq	64(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 128(%rbp)
        movq	%r9, 136(%rbp)
        movq	144(%rbp), %r8
        movq	152(%rbp), %r9
        # A[9] x A[9]
        movq	72(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 144(%rbp)
        movq	%r9, 152(%rbp)
        movq	160(%rbp), %r8
        movq	168(%rbp), %r9
        # A[10] x A[10]
        movq	80(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 160(%rbp)
        movq	%r9, 168(%rbp)
        movq	176(%rbp), %r8
        movq	184(%rbp), %r9
        # A[11] x A[11]
        movq	88(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 176(%rbp)
        movq	%r9, 184(%rbp)
        movq	192(%rdi), %r8
        movq	200(%rdi), %r9
        # A[12] x A[12]
        movq	96(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 192(%rdi)
        movq	%r9, 200(%rdi)
        movq	208(%rdi), %r8
        movq	216(%rdi), %r9
        # A[13] x A[13]
        movq	104(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 208(%rdi)
        movq	%r9, 216(%rdi)
        movq	224(%rdi), %r8
        movq	232(%rdi), %r9
        # A[14] x A[14]
        movq	112(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 224(%rdi)
        movq	%r9, 232(%rdi)
        movq	240(%rdi), %r8
        movq	248(%rdi), %r9
        # A[15] x A[15]
        movq	120(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 240(%rdi)
        movq	%r9, 248(%rdi)
        movq	256(%rdi), %r8
        movq	264(%rdi), %r9
        # A[16] x A[16]
        movq	128(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 256(%rdi)
        movq	%r9, 264(%rdi)
        movq	272(%rdi), %r8
        movq	280(%rdi), %r9
        # A[17] x A[17]
        movq	136(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 272(%rdi)
        movq	%r9, 280(%rdi)
        movq	288(%rdi), %r8
        movq	296(%rdi), %r9
        # A[18] x A[18]
        movq	144(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 288(%rdi)
        movq	%r9, 296(%rdi)
        movq	304(%rdi), %r8
        movq	312(%rdi), %r9
        # A[19] x A[19]
        movq	152(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 304(%rdi)
        movq	%r9, 312(%rdi)
        movq	320(%rdi), %r8
        movq	328(%rdi), %r9
        # A[20] x A[20]
        movq	160(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 320(%rdi)
        movq	%r9, 328(%rdi)
        movq	336(%rdi), %r8
        movq	344(%rdi), %r9
        # A[21] x A[21]
        movq	168(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 336(%rdi)
        movq	%r9, 344(%rdi)
        movq	352(%rdi), %r8
        movq	360(%rdi), %r9
        # A[22] x A[22]
        movq	176(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 352(%rdi)
        movq	%r9, 360(%rdi)
        movq	368(%rdi), %r8
        movq	376(%rdi), %r9
        # A[23] x A[23]
        movq	184(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adoxq	%r8, %r8
        adcxq	%rax, %r8
        adoxq	%r9, %r9
        adcxq	%rcx, %r9
        movq	%r8, 368(%rdi)
        movq	%r9, 376(%rdi)
        cmpq	%rdi, %rsi
        jne	L_end_3072_sqr_avx2_24
        movq	(%rbp), %r8
        movq	8(%rbp), %r9
        movq	16(%rbp), %r10
        movq	24(%rbp), %r11
        movq	%r8, (%rdi)
        movq	%r9, 8(%rdi)
        movq	%r10, 16(%rdi)
        movq	%r11, 24(%rdi)
        movq	32(%rbp), %r8
        movq	40(%rbp), %r9
        movq	48(%rbp), %r10
        movq	56(%rbp), %r11
        movq	%r8, 32(%rdi)
        movq	%r9, 40(%rdi)
        movq	%r10, 48(%rdi)
        movq	%r11, 56(%rdi)
        movq	64(%rbp), %r8
        movq	72(%rbp), %r9
        movq	80(%rbp), %r10
        movq	88(%rbp), %r11
        movq	%r8, 64(%rdi)
        movq	%r9, 72(%rdi)
        movq	%r10, 80(%rdi)
        movq	%r11, 88(%rdi)
        movq	96(%rbp), %r8
        movq	104(%rbp), %r9
        movq	112(%rbp), %r10
        movq	120(%rbp), %r11
        movq	%r8, 96(%rdi)
        movq	%r9, 104(%rdi)
        movq	%r10, 112(%rdi)
        movq	%r11, 120(%rdi)
        movq	128(%rbp), %r8
        movq	136(%rbp), %r9
        movq	144(%rbp), %r10
        movq	152(%rbp), %r11
        movq	%r8, 128(%rdi)
        movq	%r9, 136(%rdi)
        movq	%r10, 144(%rdi)
        movq	%r11, 152(%rdi)
        movq	160(%rbp), %r8
        movq	168(%rbp), %r9
        movq	176(%rbp), %r10
        movq	184(%rbp), %r11
        movq	%r8, 160(%rdi)
        movq	%r9, 168(%rdi)
        movq	%r10, 176(%rdi)
        movq	%r11, 184(%rdi)
L_end_3072_sqr_avx2_24:
        addq	$192, %rsp
        pop	%r13
        pop	%r12
        pop	%rbp
        repz retq
#ifndef __APPLE__
.size	sp_3072_sqr_avx2_24,.-sp_3072_sqr_avx2_24
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_3072_add_24
.type	sp_3072_add_24,@function
.align	16
sp_3072_add_24:
#else
.globl	_sp_3072_add_24
.p2align	4
_sp_3072_add_24:
#endif /* __APPLE__ */
        xorq	%rax, %rax
        movq	(%rsi), %rcx
        addq	(%rdx), %rcx
        movq	%rcx, (%rdi)
        movq	8(%rsi), %rcx
        adcq	8(%rdx), %rcx
        movq	%rcx, 8(%rdi)
        movq	16(%rsi), %rcx
        adcq	16(%rdx), %rcx
        movq	%rcx, 16(%rdi)
        movq	24(%rsi), %rcx
        adcq	24(%rdx), %rcx
        movq	%rcx, 24(%rdi)
        movq	32(%rsi), %rcx
        adcq	32(%rdx), %rcx
        movq	%rcx, 32(%rdi)
        movq	40(%rsi), %rcx
        adcq	40(%rdx), %rcx
        movq	%rcx, 40(%rdi)
        movq	48(%rsi), %rcx
        adcq	48(%rdx), %rcx
        movq	%rcx, 48(%rdi)
        movq	56(%rsi), %rcx
        adcq	56(%rdx), %rcx
        movq	%rcx, 56(%rdi)
        movq	64(%rsi), %rcx
        adcq	64(%rdx), %rcx
        movq	%rcx, 64(%rdi)
        movq	72(%rsi), %rcx
        adcq	72(%rdx), %rcx
        movq	%rcx, 72(%rdi)
        movq	80(%rsi), %rcx
        adcq	80(%rdx), %rcx
        movq	%rcx, 80(%rdi)
        movq	88(%rsi), %rcx
        adcq	88(%rdx), %rcx
        movq	%rcx, 88(%rdi)
        movq	96(%rsi), %rcx
        adcq	96(%rdx), %rcx
        movq	%rcx, 96(%rdi)
        movq	104(%rsi), %rcx
        adcq	104(%rdx), %rcx
        movq	%rcx, 104(%rdi)
        movq	112(%rsi), %rcx
        adcq	112(%rdx), %rcx
        movq	%rcx, 112(%rdi)
        movq	120(%rsi), %rcx
        adcq	120(%rdx), %rcx
        movq	%rcx, 120(%rdi)
        movq	128(%rsi), %rcx
        adcq	128(%rdx), %rcx
        movq	%rcx, 128(%rdi)
        movq	136(%rsi), %rcx
        adcq	136(%rdx), %rcx
        movq	%rcx, 136(%rdi)
        movq	144(%rsi), %rcx
        adcq	144(%rdx), %rcx
        movq	%rcx, 144(%rdi)
        movq	152(%rsi), %rcx
        adcq	152(%rdx), %rcx
        movq	%rcx, 152(%rdi)
        movq	160(%rsi), %rcx
        adcq	160(%rdx), %rcx
        movq	%rcx, 160(%rdi)
        movq	168(%rsi), %rcx
        adcq	168(%rdx), %rcx
        movq	%rcx, 168(%rdi)
        movq	176(%rsi), %rcx
        adcq	176(%rdx), %rcx
        movq	%rcx, 176(%rdi)
        movq	184(%rsi), %rcx
        adcq	184(%rdx), %rcx
        movq	%rcx, 184(%rdi)
        adcq	$0, %rax
        repz retq
#ifndef __APPLE__
.size	sp_3072_add_24,.-sp_3072_add_24
#endif /* __APPLE__ */
/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer and result.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_3072_sub_in_place_48
.type	sp_3072_sub_in_place_48,@function
.align	16
sp_3072_sub_in_place_48:
#else
.globl	_sp_3072_sub_in_place_48
.p2align	4
_sp_3072_sub_in_place_48:
#endif /* __APPLE__ */
        xorq	%rax, %rax
        movq	(%rdi), %rdx
        movq	8(%rdi), %rcx
        movq	(%rsi), %r8
        movq	8(%rsi), %r9
        subq	%r8, %rdx
        movq	16(%rsi), %r8
        movq	%rdx, (%rdi)
        movq	16(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	24(%rsi), %r9
        movq	%rcx, 8(%rdi)
        movq	24(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	32(%rsi), %r8
        movq	%rdx, 16(%rdi)
        movq	32(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	40(%rsi), %r9
        movq	%rcx, 24(%rdi)
        movq	40(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	48(%rsi), %r8
        movq	%rdx, 32(%rdi)
        movq	48(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	56(%rsi), %r9
        movq	%rcx, 40(%rdi)
        movq	56(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	64(%rsi), %r8
        movq	%rdx, 48(%rdi)
        movq	64(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	72(%rsi), %r9
        movq	%rcx, 56(%rdi)
        movq	72(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	80(%rsi), %r8
        movq	%rdx, 64(%rdi)
        movq	80(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	88(%rsi), %r9
        movq	%rcx, 72(%rdi)
        movq	88(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	96(%rsi), %r8
        movq	%rdx, 80(%rdi)
        movq	96(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	104(%rsi), %r9
        movq	%rcx, 88(%rdi)
        movq	104(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	112(%rsi), %r8
        movq	%rdx, 96(%rdi)
        movq	112(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	120(%rsi), %r9
        movq	%rcx, 104(%rdi)
        movq	120(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	128(%rsi), %r8
        movq	%rdx, 112(%rdi)
        movq	128(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	136(%rsi), %r9
        movq	%rcx, 120(%rdi)
        movq	136(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	144(%rsi), %r8
        movq	%rdx, 128(%rdi)
        movq	144(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	152(%rsi), %r9
        movq	%rcx, 136(%rdi)
        movq	152(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	160(%rsi), %r8
        movq	%rdx, 144(%rdi)
        movq	160(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	168(%rsi), %r9
        movq	%rcx, 152(%rdi)
        movq	168(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	176(%rsi), %r8
        movq	%rdx, 160(%rdi)
        movq	176(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	184(%rsi), %r9
        movq	%rcx, 168(%rdi)
        movq	184(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	192(%rsi), %r8
        movq	%rdx, 176(%rdi)
        movq	192(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	200(%rsi), %r9
        movq	%rcx, 184(%rdi)
        movq	200(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	208(%rsi), %r8
        movq	%rdx, 192(%rdi)
        movq	208(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	216(%rsi), %r9
        movq	%rcx, 200(%rdi)
        movq	216(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	224(%rsi), %r8
        movq	%rdx, 208(%rdi)
        movq	224(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	232(%rsi), %r9
        movq	%rcx, 216(%rdi)
        movq	232(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	240(%rsi), %r8
        movq	%rdx, 224(%rdi)
        movq	240(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	248(%rsi), %r9
        movq	%rcx, 232(%rdi)
        movq	248(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	256(%rsi), %r8
        movq	%rdx, 240(%rdi)
        movq	256(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	264(%rsi), %r9
        movq	%rcx, 248(%rdi)
        movq	264(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	272(%rsi), %r8
        movq	%rdx, 256(%rdi)
        movq	272(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	280(%rsi), %r9
        movq	%rcx, 264(%rdi)
        movq	280(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	288(%rsi), %r8
        movq	%rdx, 272(%rdi)
        movq	288(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	296(%rsi), %r9
        movq	%rcx, 280(%rdi)
        movq	296(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	304(%rsi), %r8
        movq	%rdx, 288(%rdi)
        movq	304(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	312(%rsi), %r9
        movq	%rcx, 296(%rdi)
        movq	312(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	320(%rsi), %r8
        movq	%rdx, 304(%rdi)
        movq	320(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	328(%rsi), %r9
        movq	%rcx, 312(%rdi)
        movq	328(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	336(%rsi), %r8
        movq	%rdx, 320(%rdi)
        movq	336(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	344(%rsi), %r9
        movq	%rcx, 328(%rdi)
        movq	344(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	352(%rsi), %r8
        movq	%rdx, 336(%rdi)
        movq	352(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	360(%rsi), %r9
        movq	%rcx, 344(%rdi)
        movq	360(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	368(%rsi), %r8
        movq	%rdx, 352(%rdi)
        movq	368(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	376(%rsi), %r9
        movq	%rcx, 360(%rdi)
        movq	376(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	%rdx, 368(%rdi)
        sbbq	%r9, %rcx
        movq	%rcx, 376(%rdi)
        sbbq	$0, %rax
        repz retq
#ifndef __APPLE__
.size	sp_3072_sub_in_place_48,.-sp_3072_sub_in_place_48
#endif /* __APPLE__ */
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_3072_add_48
.type	sp_3072_add_48,@function
.align	16
sp_3072_add_48:
#else
.globl	_sp_3072_add_48
.p2align	4
_sp_3072_add_48:
#endif /* __APPLE__ */
        xorq	%rax, %rax
        movq	(%rsi), %rcx
        addq	(%rdx), %rcx
        movq	%rcx, (%rdi)
        movq	8(%rsi), %rcx
        adcq	8(%rdx), %rcx
        movq	%rcx, 8(%rdi)
        movq	16(%rsi), %rcx
        adcq	16(%rdx), %rcx
        movq	%rcx, 16(%rdi)
        movq	24(%rsi), %rcx
        adcq	24(%rdx), %rcx
        movq	%rcx, 24(%rdi)
        movq	32(%rsi), %rcx
        adcq	32(%rdx), %rcx
        movq	%rcx, 32(%rdi)
        movq	40(%rsi), %rcx
        adcq	40(%rdx), %rcx
        movq	%rcx, 40(%rdi)
        movq	48(%rsi), %rcx
        adcq	48(%rdx), %rcx
        movq	%rcx, 48(%rdi)
        movq	56(%rsi), %rcx
        adcq	56(%rdx), %rcx
        movq	%rcx, 56(%rdi)
        movq	64(%rsi), %rcx
        adcq	64(%rdx), %rcx
        movq	%rcx, 64(%rdi)
        movq	72(%rsi), %rcx
        adcq	72(%rdx), %rcx
        movq	%rcx, 72(%rdi)
        movq	80(%rsi), %rcx
        adcq	80(%rdx), %rcx
        movq	%rcx, 80(%rdi)
        movq	88(%rsi), %rcx
        adcq	88(%rdx), %rcx
        movq	%rcx, 88(%rdi)
        movq	96(%rsi), %rcx
        adcq	96(%rdx), %rcx
        movq	%rcx, 96(%rdi)
        movq	104(%rsi), %rcx
        adcq	104(%rdx), %rcx
        movq	%rcx, 104(%rdi)
        movq	112(%rsi), %rcx
        adcq	112(%rdx), %rcx
        movq	%rcx, 112(%rdi)
        movq	120(%rsi), %rcx
        adcq	120(%rdx), %rcx
        movq	%rcx, 120(%rdi)
        movq	128(%rsi), %rcx
        adcq	128(%rdx), %rcx
        movq	%rcx, 128(%rdi)
        movq	136(%rsi), %rcx
        adcq	136(%rdx), %rcx
        movq	%rcx, 136(%rdi)
        movq	144(%rsi), %rcx
        adcq	144(%rdx), %rcx
        movq	%rcx, 144(%rdi)
        movq	152(%rsi), %rcx
        adcq	152(%rdx), %rcx
        movq	%rcx, 152(%rdi)
        movq	160(%rsi), %rcx
        adcq	160(%rdx), %rcx
        movq	%rcx, 160(%rdi)
        movq	168(%rsi), %rcx
        adcq	168(%rdx), %rcx
        movq	%rcx, 168(%rdi)
        movq	176(%rsi), %rcx
        adcq	176(%rdx), %rcx
        movq	%rcx, 176(%rdi)
        movq	184(%rsi), %rcx
        adcq	184(%rdx), %rcx
        movq	%rcx, 184(%rdi)
        movq	192(%rsi), %rcx
        adcq	192(%rdx), %rcx
        movq	%rcx, 192(%rdi)
        movq	200(%rsi), %rcx
        adcq	200(%rdx), %rcx
        movq	%rcx, 200(%rdi)
        movq	208(%rsi), %rcx
        adcq	208(%rdx), %rcx
        movq	%rcx, 208(%rdi)
        movq	216(%rsi), %rcx
        adcq	216(%rdx), %rcx
        movq	%rcx, 216(%rdi)
        movq	224(%rsi), %rcx
        adcq	224(%rdx), %rcx
        movq	%rcx, 224(%rdi)
        movq	232(%rsi), %rcx
        adcq	232(%rdx), %rcx
        movq	%rcx, 232(%rdi)
        movq	240(%rsi), %rcx
        adcq	240(%rdx), %rcx
        movq	%rcx, 240(%rdi)
        movq	248(%rsi), %rcx
        adcq	248(%rdx), %rcx
        movq	%rcx, 248(%rdi)
        movq	256(%rsi), %rcx
        adcq	256(%rdx), %rcx
        movq	%rcx, 256(%rdi)
        movq	264(%rsi), %rcx
        adcq	264(%rdx), %rcx
        movq	%rcx, 264(%rdi)
        movq	272(%rsi), %rcx
        adcq	272(%rdx), %rcx
        movq	%rcx, 272(%rdi)
        movq	280(%rsi), %rcx
        adcq	280(%rdx), %rcx
        movq	%rcx, 280(%rdi)
        movq	288(%rsi), %rcx
        adcq	288(%rdx), %rcx
        movq	%rcx, 288(%rdi)
        movq	296(%rsi), %rcx
        adcq	296(%rdx), %rcx
        movq	%rcx, 296(%rdi)
        movq	304(%rsi), %rcx
        adcq	304(%rdx), %rcx
        movq	%rcx, 304(%rdi)
        movq	312(%rsi), %rcx
        adcq	312(%rdx), %rcx
        movq	%rcx, 312(%rdi)
        movq	320(%rsi), %rcx
        adcq	320(%rdx), %rcx
        movq	%rcx, 320(%rdi)
        movq	328(%rsi), %rcx
        adcq	328(%rdx), %rcx
        movq	%rcx, 328(%rdi)
        movq	336(%rsi), %rcx
        adcq	336(%rdx), %rcx
        movq	%rcx, 336(%rdi)
        movq	344(%rsi), %rcx
        adcq	344(%rdx), %rcx
        movq	%rcx, 344(%rdi)
        movq	352(%rsi), %rcx
        adcq	352(%rdx), %rcx
        movq	%rcx, 352(%rdi)
        movq	360(%rsi), %rcx
        adcq	360(%rdx), %rcx
        movq	%rcx, 360(%rdi)
        movq	368(%rsi), %rcx
        adcq	368(%rdx), %rcx
        movq	%rcx, 368(%rdi)
        movq	376(%rsi), %rcx
        adcq	376(%rdx), %rcx
        movq	%rcx, 376(%rdi)
        adcq	$0, %rax
        repz retq
#ifndef __APPLE__
.size	sp_3072_add_48,.-sp_3072_add_48
#endif /* __APPLE__ */
/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
#ifndef __APPLE__
.globl	sp_3072_mul_d_48
.type	sp_3072_mul_d_48,@function
.align	16
sp_3072_mul_d_48:
#else
.globl	_sp_3072_mul_d_48
.p2align	4
_sp_3072_mul_d_48:
#endif /* __APPLE__ */
        movq	%rdx, %rcx
        # A[0] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	(%rsi)
        movq	%rax, %r8
        movq	%rdx, %r9
        movq	%r8, (%rdi)
        # A[1] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	8(%rsi)
        addq	%rax, %r9
        movq	%r9, 8(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	16(%rsi)
        addq	%rax, %r10
        movq	%r10, 16(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	24(%rsi)
        addq	%rax, %r8
        movq	%r8, 24(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	32(%rsi)
        addq	%rax, %r9
        movq	%r9, 32(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	40(%rsi)
        addq	%rax, %r10
        movq	%r10, 40(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	48(%rsi)
        addq	%rax, %r8
        movq	%r8, 48(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	56(%rsi)
        addq	%rax, %r9
        movq	%r9, 56(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	64(%rsi)
        addq	%rax, %r10
        movq	%r10, 64(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	72(%rsi)
        addq	%rax, %r8
        movq	%r8, 72(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	80(%rsi)
        addq	%rax, %r9
        movq	%r9, 80(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	88(%rsi)
        addq	%rax, %r10
        movq	%r10, 88(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	96(%rsi)
        addq	%rax, %r8
        movq	%r8, 96(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	104(%rsi)
        addq	%rax, %r9
        movq	%r9, 104(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	112(%rsi)
        addq	%rax, %r10
        movq	%r10, 112(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	120(%rsi)
        addq	%rax, %r8
        movq	%r8, 120(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[16] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	128(%rsi)
        addq	%rax, %r9
        movq	%r9, 128(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[17] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	136(%rsi)
        addq	%rax, %r10
        movq	%r10, 136(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[18] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	144(%rsi)
        addq	%rax, %r8
        movq	%r8, 144(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[19] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	152(%rsi)
        addq	%rax, %r9
        movq	%r9, 152(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[20] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	160(%rsi)
        addq	%rax, %r10
        movq	%r10, 160(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[21] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	168(%rsi)
        addq	%rax, %r8
        movq	%r8, 168(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[22] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	176(%rsi)
        addq	%rax, %r9
        movq	%r9, 176(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[23] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	184(%rsi)
        addq	%rax, %r10
        movq	%r10, 184(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[24] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	192(%rsi)
        addq	%rax, %r8
        movq	%r8, 192(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[25] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	200(%rsi)
        addq	%rax, %r9
        movq	%r9, 200(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[26] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	208(%rsi)
        addq	%rax, %r10
        movq	%r10, 208(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[27] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	216(%rsi)
        addq	%rax, %r8
        movq	%r8, 216(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[28] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	224(%rsi)
        addq	%rax, %r9
        movq	%r9, 224(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[29] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	232(%rsi)
        addq	%rax, %r10
        movq	%r10, 232(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[30] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	240(%rsi)
        addq	%rax, %r8
        movq	%r8, 240(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[31] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	248(%rsi)
        addq	%rax, %r9
        movq	%r9, 248(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[32] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	256(%rsi)
        addq	%rax, %r10
        movq	%r10, 256(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[33] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	264(%rsi)
        addq	%rax, %r8
        movq	%r8, 264(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[34] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	272(%rsi)
        addq	%rax, %r9
        movq	%r9, 272(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[35] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	280(%rsi)
        addq	%rax, %r10
        movq	%r10, 280(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[36] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	288(%rsi)
        addq	%rax, %r8
        movq	%r8, 288(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[37] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	296(%rsi)
        addq	%rax, %r9
        movq	%r9, 296(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[38] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	304(%rsi)
        addq	%rax, %r10
        movq	%r10, 304(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[39] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	312(%rsi)
        addq	%rax, %r8
        movq	%r8, 312(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[40] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	320(%rsi)
        addq	%rax, %r9
        movq	%r9, 320(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[41] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	328(%rsi)
        addq	%rax, %r10
        movq	%r10, 328(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[42] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	336(%rsi)
        addq	%rax, %r8
        movq	%r8, 336(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[43] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	344(%rsi)
        addq	%rax, %r9
        movq	%r9, 344(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[44] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	352(%rsi)
        addq	%rax, %r10
        movq	%r10, 352(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[45] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	360(%rsi)
        addq	%rax, %r8
        movq	%r8, 360(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[46] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	368(%rsi)
        addq	%rax, %r9
        movq	%r9, 368(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # # A[47] * B
        movq	%rcx, %rax
        mulq	376(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        movq	%r10, 376(%rdi)
        movq	%r8, 384(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_3072_mul_d_48,.-sp_3072_mul_d_48
#endif /* __APPLE__ */
/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer and result.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_3072_sub_in_place_24
.type	sp_3072_sub_in_place_24,@function
.align	16
sp_3072_sub_in_place_24:
#else
.globl	_sp_3072_sub_in_place_24
.p2align	4
_sp_3072_sub_in_place_24:
#endif /* __APPLE__ */
        xorq	%rax, %rax
        movq	(%rdi), %rdx
        movq	8(%rdi), %rcx
        movq	(%rsi), %r8
        movq	8(%rsi), %r9
        subq	%r8, %rdx
        movq	16(%rsi), %r8
        movq	%rdx, (%rdi)
        movq	16(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	24(%rsi), %r9
        movq	%rcx, 8(%rdi)
        movq	24(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	32(%rsi), %r8
        movq	%rdx, 16(%rdi)
        movq	32(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	40(%rsi), %r9
        movq	%rcx, 24(%rdi)
        movq	40(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	48(%rsi), %r8
        movq	%rdx, 32(%rdi)
        movq	48(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	56(%rsi), %r9
        movq	%rcx, 40(%rdi)
        movq	56(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	64(%rsi), %r8
        movq	%rdx, 48(%rdi)
        movq	64(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	72(%rsi), %r9
        movq	%rcx, 56(%rdi)
        movq	72(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	80(%rsi), %r8
        movq	%rdx, 64(%rdi)
        movq	80(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	88(%rsi), %r9
        movq	%rcx, 72(%rdi)
        movq	88(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	96(%rsi), %r8
        movq	%rdx, 80(%rdi)
        movq	96(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	104(%rsi), %r9
        movq	%rcx, 88(%rdi)
        movq	104(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	112(%rsi), %r8
        movq	%rdx, 96(%rdi)
        movq	112(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	120(%rsi), %r9
        movq	%rcx, 104(%rdi)
        movq	120(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	128(%rsi), %r8
        movq	%rdx, 112(%rdi)
        movq	128(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	136(%rsi), %r9
        movq	%rcx, 120(%rdi)
        movq	136(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	144(%rsi), %r8
        movq	%rdx, 128(%rdi)
        movq	144(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	152(%rsi), %r9
        movq	%rcx, 136(%rdi)
        movq	152(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	160(%rsi), %r8
        movq	%rdx, 144(%rdi)
        movq	160(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	168(%rsi), %r9
        movq	%rcx, 152(%rdi)
        movq	168(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	176(%rsi), %r8
        movq	%rdx, 160(%rdi)
        movq	176(%rdi), %rdx
        sbbq	%r9, %rcx
        movq	184(%rsi), %r9
        movq	%rcx, 168(%rdi)
        movq	184(%rdi), %rcx
        sbbq	%r8, %rdx
        movq	%rdx, 176(%rdi)
        sbbq	%r9, %rcx
        movq	%rcx, 184(%rdi)
        sbbq	$0, %rax
        repz retq
#ifndef __APPLE__
.size	sp_3072_sub_in_place_24,.-sp_3072_sub_in_place_24
#endif /* __APPLE__ */
/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not copying.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
#ifndef __APPLE__
.globl	sp_3072_cond_sub_24
.type	sp_3072_cond_sub_24,@function
.align	16
sp_3072_cond_sub_24:
#else
.globl	_sp_3072_cond_sub_24
.p2align	4
_sp_3072_cond_sub_24:
#endif /* __APPLE__ */
        subq	$192, %rsp
        movq	$0, %rax
        movq	(%rdx), %r8
        movq	8(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, (%rsp)
        movq	%r9, 8(%rsp)
        movq	16(%rdx), %r8
        movq	24(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 16(%rsp)
        movq	%r9, 24(%rsp)
        movq	32(%rdx), %r8
        movq	40(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 32(%rsp)
        movq	%r9, 40(%rsp)
        movq	48(%rdx), %r8
        movq	56(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 48(%rsp)
        movq	%r9, 56(%rsp)
        movq	64(%rdx), %r8
        movq	72(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 64(%rsp)
        movq	%r9, 72(%rsp)
        movq	80(%rdx), %r8
        movq	88(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 80(%rsp)
        movq	%r9, 88(%rsp)
        movq	96(%rdx), %r8
        movq	104(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 96(%rsp)
        movq	%r9, 104(%rsp)
        movq	112(%rdx), %r8
        movq	120(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 112(%rsp)
        movq	%r9, 120(%rsp)
        movq	128(%rdx), %r8
        movq	136(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 128(%rsp)
        movq	%r9, 136(%rsp)
        movq	144(%rdx), %r8
        movq	152(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 144(%rsp)
        movq	%r9, 152(%rsp)
        movq	160(%rdx), %r8
        movq	168(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 160(%rsp)
        movq	%r9, 168(%rsp)
        movq	176(%rdx), %r8
        movq	184(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 176(%rsp)
        movq	%r9, 184(%rsp)
        movq	(%rsi), %r8
        movq	(%rsp), %rdx
        subq	%rdx, %r8
        movq	8(%rsi), %r9
        movq	8(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, (%rdi)
        movq	16(%rsi), %r8
        movq	16(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 8(%rdi)
        movq	24(%rsi), %r9
        movq	24(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 16(%rdi)
        movq	32(%rsi), %r8
        movq	32(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 24(%rdi)
        movq	40(%rsi), %r9
        movq	40(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 32(%rdi)
        movq	48(%rsi), %r8
        movq	48(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 40(%rdi)
        movq	56(%rsi), %r9
        movq	56(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 48(%rdi)
        movq	64(%rsi), %r8
        movq	64(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 56(%rdi)
        movq	72(%rsi), %r9
        movq	72(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 64(%rdi)
        movq	80(%rsi), %r8
        movq	80(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 72(%rdi)
        movq	88(%rsi), %r9
        movq	88(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 80(%rdi)
        movq	96(%rsi), %r8
        movq	96(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 88(%rdi)
        movq	104(%rsi), %r9
        movq	104(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 96(%rdi)
        movq	112(%rsi), %r8
        movq	112(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 104(%rdi)
        movq	120(%rsi), %r9
        movq	120(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 112(%rdi)
        movq	128(%rsi), %r8
        movq	128(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 120(%rdi)
        movq	136(%rsi), %r9
        movq	136(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 128(%rdi)
        movq	144(%rsi), %r8
        movq	144(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 136(%rdi)
        movq	152(%rsi), %r9
        movq	152(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 144(%rdi)
        movq	160(%rsi), %r8
        movq	160(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 152(%rdi)
        movq	168(%rsi), %r9
        movq	168(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 160(%rdi)
        movq	176(%rsi), %r8
        movq	176(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 168(%rdi)
        movq	184(%rsi), %r9
        movq	184(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 176(%rdi)
        movq	%r9, 184(%rdi)
        sbbq	$0, %rax
        addq	$192, %rsp
        repz retq
#ifndef __APPLE__
.size	sp_3072_cond_sub_24,.-sp_3072_cond_sub_24
#endif /* __APPLE__ */
/* Reduce the number back to 3072 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
#ifndef __APPLE__
.globl	sp_3072_mont_reduce_24
.type	sp_3072_mont_reduce_24,@function
.align	16
sp_3072_mont_reduce_24:
#else
.globl	_sp_3072_mont_reduce_24
.p2align	4
_sp_3072_mont_reduce_24:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        movq	%rdx, %rcx
        xorq	%r15, %r15
        # i = 0
        movq	$24, %r8
        movq	(%rdi), %r13
        movq	8(%rdi), %r14
L_mont_loop_24:
        # mu = a[i] * mp
        movq	%r13, %r11
        imulq	%rcx, %r11
        # a[i+0] += m[0] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	(%rsi)
        addq	%rax, %r13
        adcq	%rdx, %r10
        # a[i+1] += m[1] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	8(%rsi)
        movq	%r14, %r13
        addq	%rax, %r13
        adcq	%rdx, %r9
        addq	%r10, %r13
        adcq	$0, %r9
        # a[i+2] += m[2] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	16(%rsi)
        movq	16(%rdi), %r14
        addq	%rax, %r14
        adcq	%rdx, %r10
        addq	%r9, %r14
        adcq	$0, %r10
        # a[i+3] += m[3] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	24(%rsi)
        movq	24(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 24(%rdi)
        adcq	$0, %r9
        # a[i+4] += m[4] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	32(%rsi)
        movq	32(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 32(%rdi)
        adcq	$0, %r10
        # a[i+5] += m[5] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	40(%rsi)
        movq	40(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 40(%rdi)
        adcq	$0, %r9
        # a[i+6] += m[6] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	48(%rsi)
        movq	48(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 48(%rdi)
        adcq	$0, %r10
        # a[i+7] += m[7] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	56(%rsi)
        movq	56(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 56(%rdi)
        adcq	$0, %r9
        # a[i+8] += m[8] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	64(%rsi)
        movq	64(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 64(%rdi)
        adcq	$0, %r10
        # a[i+9] += m[9] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	72(%rsi)
        movq	72(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 72(%rdi)
        adcq	$0, %r9
        # a[i+10] += m[10] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	80(%rsi)
        movq	80(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 80(%rdi)
        adcq	$0, %r10
        # a[i+11] += m[11] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	88(%rsi)
        movq	88(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 88(%rdi)
        adcq	$0, %r9
        # a[i+12] += m[12] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	96(%rsi)
        movq	96(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 96(%rdi)
        adcq	$0, %r10
        # a[i+13] += m[13] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	104(%rsi)
        movq	104(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 104(%rdi)
        adcq	$0, %r9
        # a[i+14] += m[14] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	112(%rsi)
        movq	112(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 112(%rdi)
        adcq	$0, %r10
        # a[i+15] += m[15] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	120(%rsi)
        movq	120(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 120(%rdi)
        adcq	$0, %r9
        # a[i+16] += m[16] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	128(%rsi)
        movq	128(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 128(%rdi)
        adcq	$0, %r10
        # a[i+17] += m[17] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	136(%rsi)
        movq	136(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 136(%rdi)
        adcq	$0, %r9
        # a[i+18] += m[18] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	144(%rsi)
        movq	144(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 144(%rdi)
        adcq	$0, %r10
        # a[i+19] += m[19] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	152(%rsi)
        movq	152(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 152(%rdi)
        adcq	$0, %r9
        # a[i+20] += m[20] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	160(%rsi)
        movq	160(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 160(%rdi)
        adcq	$0, %r10
        # a[i+21] += m[21] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	168(%rsi)
        movq	168(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 168(%rdi)
        adcq	$0, %r9
        # a[i+22] += m[22] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	176(%rsi)
        movq	176(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 176(%rdi)
        adcq	$0, %r10
        # a[i+23] += m[23] * mu
        movq	%r11, %rax
        mulq	184(%rsi)
        movq	184(%rdi), %r12
        addq	%rax, %r10
        adcq	%r15, %rdx
        movq	$0, %r15
        adcq	$0, %r15
        addq	%r10, %r12
        movq	%r12, 184(%rdi)
        adcq	%rdx, 192(%rdi)
        adcq	$0, %r15
        # i += 1
        addq	$8, %rdi
        decq	%r8
        jnz	L_mont_loop_24
        movq	%r13, (%rdi)
        movq	%r14, 8(%rdi)
        negq	%r15
        movq	%r15, %rcx
        movq	%rsi, %rdx
        movq	%rdi, %rsi
        movq	%rdi, %rdi
        subq	$192, %rdi
#ifndef __APPLE__
        callq	sp_3072_cond_sub_24@plt
#else
        callq	_sp_3072_cond_sub_24
#endif /* __APPLE__ */
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_3072_mont_reduce_24,.-sp_3072_mont_reduce_24
#endif /* __APPLE__ */
/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
#ifndef __APPLE__
.globl	sp_3072_mul_d_24
.type	sp_3072_mul_d_24,@function
.align	16
sp_3072_mul_d_24:
#else
.globl	_sp_3072_mul_d_24
.p2align	4
_sp_3072_mul_d_24:
#endif /* __APPLE__ */
        movq	%rdx, %rcx
        # A[0] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	(%rsi)
        movq	%rax, %r8
        movq	%rdx, %r9
        movq	%r8, (%rdi)
        # A[1] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	8(%rsi)
        addq	%rax, %r9
        movq	%r9, 8(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	16(%rsi)
        addq	%rax, %r10
        movq	%r10, 16(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	24(%rsi)
        addq	%rax, %r8
        movq	%r8, 24(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[4] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	32(%rsi)
        addq	%rax, %r9
        movq	%r9, 32(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[5] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	40(%rsi)
        addq	%rax, %r10
        movq	%r10, 40(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[6] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	48(%rsi)
        addq	%rax, %r8
        movq	%r8, 48(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[7] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	56(%rsi)
        addq	%rax, %r9
        movq	%r9, 56(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[8] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	64(%rsi)
        addq	%rax, %r10
        movq	%r10, 64(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[9] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	72(%rsi)
        addq	%rax, %r8
        movq	%r8, 72(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[10] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	80(%rsi)
        addq	%rax, %r9
        movq	%r9, 80(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[11] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	88(%rsi)
        addq	%rax, %r10
        movq	%r10, 88(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[12] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	96(%rsi)
        addq	%rax, %r8
        movq	%r8, 96(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[13] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	104(%rsi)
        addq	%rax, %r9
        movq	%r9, 104(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[14] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	112(%rsi)
        addq	%rax, %r10
        movq	%r10, 112(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[15] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	120(%rsi)
        addq	%rax, %r8
        movq	%r8, 120(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[16] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	128(%rsi)
        addq	%rax, %r9
        movq	%r9, 128(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[17] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	136(%rsi)
        addq	%rax, %r10
        movq	%r10, 136(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[18] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	144(%rsi)
        addq	%rax, %r8
        movq	%r8, 144(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[19] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	152(%rsi)
        addq	%rax, %r9
        movq	%r9, 152(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[20] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	160(%rsi)
        addq	%rax, %r10
        movq	%r10, 160(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[21] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	168(%rsi)
        addq	%rax, %r8
        movq	%r8, 168(%rdi)
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[22] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	176(%rsi)
        addq	%rax, %r9
        movq	%r9, 176(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # # A[23] * B
        movq	%rcx, %rax
        mulq	184(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        movq	%r10, 184(%rdi)
        movq	%r8, 192(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_3072_mul_d_24,.-sp_3072_mul_d_24
#endif /* __APPLE__ */
#ifdef HAVE_INTEL_AVX2
/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
#ifndef __APPLE__
.globl	sp_3072_mul_d_avx2_24
.type	sp_3072_mul_d_avx2_24,@function
.align	16
sp_3072_mul_d_avx2_24:
#else
.globl	_sp_3072_mul_d_avx2_24
.p2align	4
_sp_3072_mul_d_avx2_24:
#endif /* __APPLE__ */
        movq	%rdx, %rax
        # A[0] * B
        movq	%rax, %rdx
        xorq	%r11, %r11
        mulxq	(%rsi), %r9, %r10
        movq	%r9, (%rdi)
        # A[1] * B
        mulxq	8(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 8(%rdi)
        adoxq	%r8, %r9
        # A[2] * B
        mulxq	16(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 16(%rdi)
        adoxq	%r8, %r10
        # A[3] * B
        mulxq	24(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 24(%rdi)
        adoxq	%r8, %r9
        # A[4] * B
        mulxq	32(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 32(%rdi)
        adoxq	%r8, %r10
        # A[5] * B
        mulxq	40(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 40(%rdi)
        adoxq	%r8, %r9
        # A[6] * B
        mulxq	48(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 48(%rdi)
        adoxq	%r8, %r10
        # A[7] * B
        mulxq	56(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 56(%rdi)
        adoxq	%r8, %r9
        # A[8] * B
        mulxq	64(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 64(%rdi)
        adoxq	%r8, %r10
        # A[9] * B
        mulxq	72(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 72(%rdi)
        adoxq	%r8, %r9
        # A[10] * B
        mulxq	80(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 80(%rdi)
        adoxq	%r8, %r10
        # A[11] * B
        mulxq	88(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 88(%rdi)
        adoxq	%r8, %r9
        # A[12] * B
        mulxq	96(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 96(%rdi)
        adoxq	%r8, %r10
        # A[13] * B
        mulxq	104(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 104(%rdi)
        adoxq	%r8, %r9
        # A[14] * B
        mulxq	112(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 112(%rdi)
        adoxq	%r8, %r10
        # A[15] * B
        mulxq	120(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 120(%rdi)
        adoxq	%r8, %r9
        # A[16] * B
        mulxq	128(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 128(%rdi)
        adoxq	%r8, %r10
        # A[17] * B
        mulxq	136(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 136(%rdi)
        adoxq	%r8, %r9
        # A[18] * B
        mulxq	144(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 144(%rdi)
        adoxq	%r8, %r10
        # A[19] * B
        mulxq	152(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 152(%rdi)
        adoxq	%r8, %r9
        # A[20] * B
        mulxq	160(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 160(%rdi)
        adoxq	%r8, %r10
        # A[21] * B
        mulxq	168(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 168(%rdi)
        adoxq	%r8, %r9
        # A[22] * B
        mulxq	176(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 176(%rdi)
        adoxq	%r8, %r10
        # A[23] * B
        mulxq	184(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        adoxq	%r8, %r9
        adcxq	%r11, %r9
        movq	%r10, 184(%rdi)
        movq	%r9, 192(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_3072_mul_d_avx2_24,.-sp_3072_mul_d_avx2_24
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
#ifndef __APPLE__
.globl	sp_3072_cmp_24
.type	sp_3072_cmp_24,@function
.align	16
sp_3072_cmp_24:
#else
.globl	_sp_3072_cmp_24
.p2align	4
_sp_3072_cmp_24:
#endif /* __APPLE__ */
        xorq	%rcx, %rcx
        movq	$-1, %rdx
        movq	$-1, %rax
        movq	$1, %r8
        movq	184(%rdi), %r9
        movq	184(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	176(%rdi), %r9
        movq	176(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	168(%rdi), %r9
        movq	168(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	160(%rdi), %r9
        movq	160(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	152(%rdi), %r9
        movq	152(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	144(%rdi), %r9
        movq	144(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	136(%rdi), %r9
        movq	136(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	128(%rdi), %r9
        movq	128(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	120(%rdi), %r9
        movq	120(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	112(%rdi), %r9
        movq	112(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	104(%rdi), %r9
        movq	104(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	96(%rdi), %r9
        movq	96(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	88(%rdi), %r9
        movq	88(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	80(%rdi), %r9
        movq	80(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	72(%rdi), %r9
        movq	72(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	64(%rdi), %r9
        movq	64(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	56(%rdi), %r9
        movq	56(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	48(%rdi), %r9
        movq	48(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	40(%rdi), %r9
        movq	40(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	32(%rdi), %r9
        movq	32(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	24(%rdi), %r9
        movq	24(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	16(%rdi), %r9
        movq	16(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	8(%rdi), %r9
        movq	8(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	(%rdi), %r9
        movq	(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        xorq	%rdx, %rax
        repz retq
#ifndef __APPLE__
.size	sp_3072_cmp_24,.-sp_3072_cmp_24
#endif /* __APPLE__ */
#ifdef HAVE_INTEL_AVX2
/* Reduce the number back to 3072 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
#ifndef __APPLE__
.globl	sp_3072_mont_reduce_avx2_24
.type	sp_3072_mont_reduce_avx2_24,@function
.align	16
sp_3072_mont_reduce_avx2_24:
#else
.globl	_sp_3072_mont_reduce_avx2_24
.p2align	4
_sp_3072_mont_reduce_avx2_24:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        movq	%rdx, %rax
        xorq	%r14, %r14
        # i = 0
        movq	$24, %r9
        movq	(%rdi), %r13
        xorq	%r12, %r12
L_mont_loop_avx2_24:
        # mu = a[i] * mp
        movq	%r13, %rdx
        mulxq	%rax, %rdx, %r8
        movq	%r13, %r10
        # a[i+0] += m[0] * mu
        mulxq	(%rsi), %rcx, %r8
        movq	8(%rdi), %r13
        adcxq	%rcx, %r10
        adoxq	%r8, %r13
        # a[i+1] += m[1] * mu
        mulxq	8(%rsi), %rcx, %r8
        movq	16(%rdi), %r10
        adcxq	%rcx, %r13
        adoxq	%r8, %r10
        # a[i+2] += m[2] * mu
        mulxq	16(%rsi), %rcx, %r8
        movq	24(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 16(%rdi)
        # a[i+3] += m[3] * mu
        mulxq	24(%rsi), %rcx, %r8
        movq	32(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 24(%rdi)
        # a[i+4] += m[4] * mu
        mulxq	32(%rsi), %rcx, %r8
        movq	40(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 32(%rdi)
        # a[i+5] += m[5] * mu
        mulxq	40(%rsi), %rcx, %r8
        movq	48(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 40(%rdi)
        # a[i+6] += m[6] * mu
        mulxq	48(%rsi), %rcx, %r8
        movq	56(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 48(%rdi)
        # a[i+7] += m[7] * mu
        mulxq	56(%rsi), %rcx, %r8
        movq	64(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 56(%rdi)
        # a[i+8] += m[8] * mu
        mulxq	64(%rsi), %rcx, %r8
        movq	72(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 64(%rdi)
        # a[i+9] += m[9] * mu
        mulxq	72(%rsi), %rcx, %r8
        movq	80(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 72(%rdi)
        # a[i+10] += m[10] * mu
        mulxq	80(%rsi), %rcx, %r8
        movq	88(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 80(%rdi)
        # a[i+11] += m[11] * mu
        mulxq	88(%rsi), %rcx, %r8
        movq	96(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 88(%rdi)
        # a[i+12] += m[12] * mu
        mulxq	96(%rsi), %rcx, %r8
        movq	104(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 96(%rdi)
        # a[i+13] += m[13] * mu
        mulxq	104(%rsi), %rcx, %r8
        movq	112(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 104(%rdi)
        # a[i+14] += m[14] * mu
        mulxq	112(%rsi), %rcx, %r8
        movq	120(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 112(%rdi)
        # a[i+15] += m[15] * mu
        mulxq	120(%rsi), %rcx, %r8
        movq	128(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 120(%rdi)
        # a[i+16] += m[16] * mu
        mulxq	128(%rsi), %rcx, %r8
        movq	136(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 128(%rdi)
        # a[i+17] += m[17] * mu
        mulxq	136(%rsi), %rcx, %r8
        movq	144(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 136(%rdi)
        # a[i+18] += m[18] * mu
        mulxq	144(%rsi), %rcx, %r8
        movq	152(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 144(%rdi)
        # a[i+19] += m[19] * mu
        mulxq	152(%rsi), %rcx, %r8
        movq	160(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 152(%rdi)
        # a[i+20] += m[20] * mu
        mulxq	160(%rsi), %rcx, %r8
        movq	168(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 160(%rdi)
        # a[i+21] += m[21] * mu
        mulxq	168(%rsi), %rcx, %r8
        movq	176(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 168(%rdi)
        # a[i+22] += m[22] * mu
        mulxq	176(%rsi), %rcx, %r8
        movq	184(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 176(%rdi)
        # a[i+23] += m[23] * mu
        mulxq	184(%rsi), %rcx, %r8
        movq	192(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 184(%rdi)
        adcxq	%r14, %r10
        movq	%r12, %r14
        adoxq	%r12, %r14
        adcxq	%r12, %r14
        movq	%r10, 192(%rdi)
        # i += 1
        addq	$8, %rdi
        decq	%r9
        jnz	L_mont_loop_avx2_24
        movq	%r13, (%rdi)
        negq	%r14
        movq	%r14, %rcx
        movq	%rsi, %rdx
        movq	%rdi, %rsi
        movq	%rdi, %rdi
        subq	$192, %rdi
#ifndef __APPLE__
        callq	sp_3072_cond_sub_24@plt
#else
        callq	_sp_3072_cond_sub_24
#endif /* __APPLE__ */
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_3072_mont_reduce_avx2_24,.-sp_3072_mont_reduce_avx2_24
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not copying.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
#ifndef __APPLE__
.globl	sp_3072_cond_sub_48
.type	sp_3072_cond_sub_48,@function
.align	16
sp_3072_cond_sub_48:
#else
.globl	_sp_3072_cond_sub_48
.p2align	4
_sp_3072_cond_sub_48:
#endif /* __APPLE__ */
        subq	$384, %rsp
        movq	$0, %rax
        movq	(%rdx), %r8
        movq	8(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, (%rsp)
        movq	%r9, 8(%rsp)
        movq	16(%rdx), %r8
        movq	24(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 16(%rsp)
        movq	%r9, 24(%rsp)
        movq	32(%rdx), %r8
        movq	40(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 32(%rsp)
        movq	%r9, 40(%rsp)
        movq	48(%rdx), %r8
        movq	56(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 48(%rsp)
        movq	%r9, 56(%rsp)
        movq	64(%rdx), %r8
        movq	72(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 64(%rsp)
        movq	%r9, 72(%rsp)
        movq	80(%rdx), %r8
        movq	88(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 80(%rsp)
        movq	%r9, 88(%rsp)
        movq	96(%rdx), %r8
        movq	104(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 96(%rsp)
        movq	%r9, 104(%rsp)
        movq	112(%rdx), %r8
        movq	120(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 112(%rsp)
        movq	%r9, 120(%rsp)
        movq	128(%rdx), %r8
        movq	136(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 128(%rsp)
        movq	%r9, 136(%rsp)
        movq	144(%rdx), %r8
        movq	152(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 144(%rsp)
        movq	%r9, 152(%rsp)
        movq	160(%rdx), %r8
        movq	168(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 160(%rsp)
        movq	%r9, 168(%rsp)
        movq	176(%rdx), %r8
        movq	184(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 176(%rsp)
        movq	%r9, 184(%rsp)
        movq	192(%rdx), %r8
        movq	200(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 192(%rsp)
        movq	%r9, 200(%rsp)
        movq	208(%rdx), %r8
        movq	216(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 208(%rsp)
        movq	%r9, 216(%rsp)
        movq	224(%rdx), %r8
        movq	232(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 224(%rsp)
        movq	%r9, 232(%rsp)
        movq	240(%rdx), %r8
        movq	248(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 240(%rsp)
        movq	%r9, 248(%rsp)
        movq	256(%rdx), %r8
        movq	264(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 256(%rsp)
        movq	%r9, 264(%rsp)
        movq	272(%rdx), %r8
        movq	280(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 272(%rsp)
        movq	%r9, 280(%rsp)
        movq	288(%rdx), %r8
        movq	296(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 288(%rsp)
        movq	%r9, 296(%rsp)
        movq	304(%rdx), %r8
        movq	312(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 304(%rsp)
        movq	%r9, 312(%rsp)
        movq	320(%rdx), %r8
        movq	328(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 320(%rsp)
        movq	%r9, 328(%rsp)
        movq	336(%rdx), %r8
        movq	344(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 336(%rsp)
        movq	%r9, 344(%rsp)
        movq	352(%rdx), %r8
        movq	360(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 352(%rsp)
        movq	%r9, 360(%rsp)
        movq	368(%rdx), %r8
        movq	376(%rdx), %r9
        andq	%rcx, %r8
        andq	%rcx, %r9
        movq	%r8, 368(%rsp)
        movq	%r9, 376(%rsp)
        movq	(%rsi), %r8
        movq	(%rsp), %rdx
        subq	%rdx, %r8
        movq	8(%rsi), %r9
        movq	8(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, (%rdi)
        movq	16(%rsi), %r8
        movq	16(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 8(%rdi)
        movq	24(%rsi), %r9
        movq	24(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 16(%rdi)
        movq	32(%rsi), %r8
        movq	32(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 24(%rdi)
        movq	40(%rsi), %r9
        movq	40(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 32(%rdi)
        movq	48(%rsi), %r8
        movq	48(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 40(%rdi)
        movq	56(%rsi), %r9
        movq	56(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 48(%rdi)
        movq	64(%rsi), %r8
        movq	64(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 56(%rdi)
        movq	72(%rsi), %r9
        movq	72(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 64(%rdi)
        movq	80(%rsi), %r8
        movq	80(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 72(%rdi)
        movq	88(%rsi), %r9
        movq	88(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 80(%rdi)
        movq	96(%rsi), %r8
        movq	96(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 88(%rdi)
        movq	104(%rsi), %r9
        movq	104(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 96(%rdi)
        movq	112(%rsi), %r8
        movq	112(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 104(%rdi)
        movq	120(%rsi), %r9
        movq	120(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 112(%rdi)
        movq	128(%rsi), %r8
        movq	128(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 120(%rdi)
        movq	136(%rsi), %r9
        movq	136(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 128(%rdi)
        movq	144(%rsi), %r8
        movq	144(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 136(%rdi)
        movq	152(%rsi), %r9
        movq	152(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 144(%rdi)
        movq	160(%rsi), %r8
        movq	160(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 152(%rdi)
        movq	168(%rsi), %r9
        movq	168(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 160(%rdi)
        movq	176(%rsi), %r8
        movq	176(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 168(%rdi)
        movq	184(%rsi), %r9
        movq	184(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 176(%rdi)
        movq	192(%rsi), %r8
        movq	192(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 184(%rdi)
        movq	200(%rsi), %r9
        movq	200(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 192(%rdi)
        movq	208(%rsi), %r8
        movq	208(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 200(%rdi)
        movq	216(%rsi), %r9
        movq	216(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 208(%rdi)
        movq	224(%rsi), %r8
        movq	224(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 216(%rdi)
        movq	232(%rsi), %r9
        movq	232(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 224(%rdi)
        movq	240(%rsi), %r8
        movq	240(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 232(%rdi)
        movq	248(%rsi), %r9
        movq	248(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 240(%rdi)
        movq	256(%rsi), %r8
        movq	256(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 248(%rdi)
        movq	264(%rsi), %r9
        movq	264(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 256(%rdi)
        movq	272(%rsi), %r8
        movq	272(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 264(%rdi)
        movq	280(%rsi), %r9
        movq	280(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 272(%rdi)
        movq	288(%rsi), %r8
        movq	288(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 280(%rdi)
        movq	296(%rsi), %r9
        movq	296(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 288(%rdi)
        movq	304(%rsi), %r8
        movq	304(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 296(%rdi)
        movq	312(%rsi), %r9
        movq	312(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 304(%rdi)
        movq	320(%rsi), %r8
        movq	320(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 312(%rdi)
        movq	328(%rsi), %r9
        movq	328(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 320(%rdi)
        movq	336(%rsi), %r8
        movq	336(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 328(%rdi)
        movq	344(%rsi), %r9
        movq	344(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 336(%rdi)
        movq	352(%rsi), %r8
        movq	352(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 344(%rdi)
        movq	360(%rsi), %r9
        movq	360(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 352(%rdi)
        movq	368(%rsi), %r8
        movq	368(%rsp), %rdx
        sbbq	%rdx, %r8
        movq	%r9, 360(%rdi)
        movq	376(%rsi), %r9
        movq	376(%rsp), %rdx
        sbbq	%rdx, %r9
        movq	%r8, 368(%rdi)
        movq	%r9, 376(%rdi)
        sbbq	$0, %rax
        addq	$384, %rsp
        repz retq
#ifndef __APPLE__
.size	sp_3072_cond_sub_48,.-sp_3072_cond_sub_48
#endif /* __APPLE__ */
/* Reduce the number back to 3072 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
#ifndef __APPLE__
.globl	sp_3072_mont_reduce_48
.type	sp_3072_mont_reduce_48,@function
.align	16
sp_3072_mont_reduce_48:
#else
.globl	_sp_3072_mont_reduce_48
.p2align	4
_sp_3072_mont_reduce_48:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        movq	%rdx, %rcx
        xorq	%r15, %r15
        # i = 0
        movq	$48, %r8
        movq	(%rdi), %r13
        movq	8(%rdi), %r14
L_mont_loop_48:
        # mu = a[i] * mp
        movq	%r13, %r11
        imulq	%rcx, %r11
        # a[i+0] += m[0] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	(%rsi)
        addq	%rax, %r13
        adcq	%rdx, %r10
        # a[i+1] += m[1] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	8(%rsi)
        movq	%r14, %r13
        addq	%rax, %r13
        adcq	%rdx, %r9
        addq	%r10, %r13
        adcq	$0, %r9
        # a[i+2] += m[2] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	16(%rsi)
        movq	16(%rdi), %r14
        addq	%rax, %r14
        adcq	%rdx, %r10
        addq	%r9, %r14
        adcq	$0, %r10
        # a[i+3] += m[3] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	24(%rsi)
        movq	24(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 24(%rdi)
        adcq	$0, %r9
        # a[i+4] += m[4] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	32(%rsi)
        movq	32(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 32(%rdi)
        adcq	$0, %r10
        # a[i+5] += m[5] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	40(%rsi)
        movq	40(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 40(%rdi)
        adcq	$0, %r9
        # a[i+6] += m[6] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	48(%rsi)
        movq	48(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 48(%rdi)
        adcq	$0, %r10
        # a[i+7] += m[7] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	56(%rsi)
        movq	56(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 56(%rdi)
        adcq	$0, %r9
        # a[i+8] += m[8] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	64(%rsi)
        movq	64(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 64(%rdi)
        adcq	$0, %r10
        # a[i+9] += m[9] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	72(%rsi)
        movq	72(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 72(%rdi)
        adcq	$0, %r9
        # a[i+10] += m[10] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	80(%rsi)
        movq	80(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 80(%rdi)
        adcq	$0, %r10
        # a[i+11] += m[11] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	88(%rsi)
        movq	88(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 88(%rdi)
        adcq	$0, %r9
        # a[i+12] += m[12] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	96(%rsi)
        movq	96(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 96(%rdi)
        adcq	$0, %r10
        # a[i+13] += m[13] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	104(%rsi)
        movq	104(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 104(%rdi)
        adcq	$0, %r9
        # a[i+14] += m[14] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	112(%rsi)
        movq	112(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 112(%rdi)
        adcq	$0, %r10
        # a[i+15] += m[15] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	120(%rsi)
        movq	120(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 120(%rdi)
        adcq	$0, %r9
        # a[i+16] += m[16] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	128(%rsi)
        movq	128(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 128(%rdi)
        adcq	$0, %r10
        # a[i+17] += m[17] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	136(%rsi)
        movq	136(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 136(%rdi)
        adcq	$0, %r9
        # a[i+18] += m[18] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	144(%rsi)
        movq	144(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 144(%rdi)
        adcq	$0, %r10
        # a[i+19] += m[19] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	152(%rsi)
        movq	152(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 152(%rdi)
        adcq	$0, %r9
        # a[i+20] += m[20] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	160(%rsi)
        movq	160(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 160(%rdi)
        adcq	$0, %r10
        # a[i+21] += m[21] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	168(%rsi)
        movq	168(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 168(%rdi)
        adcq	$0, %r9
        # a[i+22] += m[22] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	176(%rsi)
        movq	176(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 176(%rdi)
        adcq	$0, %r10
        # a[i+23] += m[23] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	184(%rsi)
        movq	184(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 184(%rdi)
        adcq	$0, %r9
        # a[i+24] += m[24] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	192(%rsi)
        movq	192(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 192(%rdi)
        adcq	$0, %r10
        # a[i+25] += m[25] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	200(%rsi)
        movq	200(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 200(%rdi)
        adcq	$0, %r9
        # a[i+26] += m[26] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	208(%rsi)
        movq	208(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 208(%rdi)
        adcq	$0, %r10
        # a[i+27] += m[27] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	216(%rsi)
        movq	216(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 216(%rdi)
        adcq	$0, %r9
        # a[i+28] += m[28] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	224(%rsi)
        movq	224(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 224(%rdi)
        adcq	$0, %r10
        # a[i+29] += m[29] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	232(%rsi)
        movq	232(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 232(%rdi)
        adcq	$0, %r9
        # a[i+30] += m[30] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	240(%rsi)
        movq	240(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 240(%rdi)
        adcq	$0, %r10
        # a[i+31] += m[31] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	248(%rsi)
        movq	248(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 248(%rdi)
        adcq	$0, %r9
        # a[i+32] += m[32] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	256(%rsi)
        movq	256(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 256(%rdi)
        adcq	$0, %r10
        # a[i+33] += m[33] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	264(%rsi)
        movq	264(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 264(%rdi)
        adcq	$0, %r9
        # a[i+34] += m[34] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	272(%rsi)
        movq	272(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 272(%rdi)
        adcq	$0, %r10
        # a[i+35] += m[35] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	280(%rsi)
        movq	280(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 280(%rdi)
        adcq	$0, %r9
        # a[i+36] += m[36] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	288(%rsi)
        movq	288(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 288(%rdi)
        adcq	$0, %r10
        # a[i+37] += m[37] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	296(%rsi)
        movq	296(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 296(%rdi)
        adcq	$0, %r9
        # a[i+38] += m[38] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	304(%rsi)
        movq	304(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 304(%rdi)
        adcq	$0, %r10
        # a[i+39] += m[39] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	312(%rsi)
        movq	312(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 312(%rdi)
        adcq	$0, %r9
        # a[i+40] += m[40] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	320(%rsi)
        movq	320(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 320(%rdi)
        adcq	$0, %r10
        # a[i+41] += m[41] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	328(%rsi)
        movq	328(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 328(%rdi)
        adcq	$0, %r9
        # a[i+42] += m[42] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	336(%rsi)
        movq	336(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 336(%rdi)
        adcq	$0, %r10
        # a[i+43] += m[43] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	344(%rsi)
        movq	344(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 344(%rdi)
        adcq	$0, %r9
        # a[i+44] += m[44] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	352(%rsi)
        movq	352(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 352(%rdi)
        adcq	$0, %r10
        # a[i+45] += m[45] * mu
        movq	%r11, %rax
        xorq	%r9, %r9
        mulq	360(%rsi)
        movq	360(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r9
        addq	%r10, %r12
        movq	%r12, 360(%rdi)
        adcq	$0, %r9
        # a[i+46] += m[46] * mu
        movq	%r11, %rax
        xorq	%r10, %r10
        mulq	368(%rsi)
        movq	368(%rdi), %r12
        addq	%rax, %r12
        adcq	%rdx, %r10
        addq	%r9, %r12
        movq	%r12, 368(%rdi)
        adcq	$0, %r10
        # a[i+47] += m[47] * mu
        movq	%r11, %rax
        mulq	376(%rsi)
        movq	376(%rdi), %r12
        addq	%rax, %r10
        adcq	%r15, %rdx
        movq	$0, %r15
        adcq	$0, %r15
        addq	%r10, %r12
        movq	%r12, 376(%rdi)
        adcq	%rdx, 384(%rdi)
        adcq	$0, %r15
        # i += 1
        addq	$8, %rdi
        decq	%r8
        jnz	L_mont_loop_48
        movq	%r13, (%rdi)
        movq	%r14, 8(%rdi)
        negq	%r15
        movq	%r15, %rcx
        movq	%rsi, %rdx
        movq	%rdi, %rsi
        movq	%rdi, %rdi
        subq	$384, %rdi
#ifndef __APPLE__
        callq	sp_3072_cond_sub_48@plt
#else
        callq	_sp_3072_cond_sub_48
#endif /* __APPLE__ */
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_3072_mont_reduce_48,.-sp_3072_mont_reduce_48
#endif /* __APPLE__ */
#ifdef HAVE_INTEL_AVX2
/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
#ifndef __APPLE__
.globl	sp_3072_mul_d_avx2_48
.type	sp_3072_mul_d_avx2_48,@function
.align	16
sp_3072_mul_d_avx2_48:
#else
.globl	_sp_3072_mul_d_avx2_48
.p2align	4
_sp_3072_mul_d_avx2_48:
#endif /* __APPLE__ */
        movq	%rdx, %rax
        # A[0] * B
        movq	%rax, %rdx
        xorq	%r11, %r11
        mulxq	(%rsi), %r9, %r10
        movq	%r9, (%rdi)
        # A[1] * B
        mulxq	8(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 8(%rdi)
        adoxq	%r8, %r9
        # A[2] * B
        mulxq	16(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 16(%rdi)
        adoxq	%r8, %r10
        # A[3] * B
        mulxq	24(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 24(%rdi)
        adoxq	%r8, %r9
        # A[4] * B
        mulxq	32(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 32(%rdi)
        adoxq	%r8, %r10
        # A[5] * B
        mulxq	40(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 40(%rdi)
        adoxq	%r8, %r9
        # A[6] * B
        mulxq	48(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 48(%rdi)
        adoxq	%r8, %r10
        # A[7] * B
        mulxq	56(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 56(%rdi)
        adoxq	%r8, %r9
        # A[8] * B
        mulxq	64(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 64(%rdi)
        adoxq	%r8, %r10
        # A[9] * B
        mulxq	72(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 72(%rdi)
        adoxq	%r8, %r9
        # A[10] * B
        mulxq	80(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 80(%rdi)
        adoxq	%r8, %r10
        # A[11] * B
        mulxq	88(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 88(%rdi)
        adoxq	%r8, %r9
        # A[12] * B
        mulxq	96(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 96(%rdi)
        adoxq	%r8, %r10
        # A[13] * B
        mulxq	104(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 104(%rdi)
        adoxq	%r8, %r9
        # A[14] * B
        mulxq	112(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 112(%rdi)
        adoxq	%r8, %r10
        # A[15] * B
        mulxq	120(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 120(%rdi)
        adoxq	%r8, %r9
        # A[16] * B
        mulxq	128(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 128(%rdi)
        adoxq	%r8, %r10
        # A[17] * B
        mulxq	136(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 136(%rdi)
        adoxq	%r8, %r9
        # A[18] * B
        mulxq	144(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 144(%rdi)
        adoxq	%r8, %r10
        # A[19] * B
        mulxq	152(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 152(%rdi)
        adoxq	%r8, %r9
        # A[20] * B
        mulxq	160(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 160(%rdi)
        adoxq	%r8, %r10
        # A[21] * B
        mulxq	168(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 168(%rdi)
        adoxq	%r8, %r9
        # A[22] * B
        mulxq	176(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 176(%rdi)
        adoxq	%r8, %r10
        # A[23] * B
        mulxq	184(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 184(%rdi)
        adoxq	%r8, %r9
        # A[24] * B
        mulxq	192(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 192(%rdi)
        adoxq	%r8, %r10
        # A[25] * B
        mulxq	200(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 200(%rdi)
        adoxq	%r8, %r9
        # A[26] * B
        mulxq	208(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 208(%rdi)
        adoxq	%r8, %r10
        # A[27] * B
        mulxq	216(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 216(%rdi)
        adoxq	%r8, %r9
        # A[28] * B
        mulxq	224(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 224(%rdi)
        adoxq	%r8, %r10
        # A[29] * B
        mulxq	232(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 232(%rdi)
        adoxq	%r8, %r9
        # A[30] * B
        mulxq	240(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 240(%rdi)
        adoxq	%r8, %r10
        # A[31] * B
        mulxq	248(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 248(%rdi)
        adoxq	%r8, %r9
        # A[32] * B
        mulxq	256(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 256(%rdi)
        adoxq	%r8, %r10
        # A[33] * B
        mulxq	264(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 264(%rdi)
        adoxq	%r8, %r9
        # A[34] * B
        mulxq	272(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 272(%rdi)
        adoxq	%r8, %r10
        # A[35] * B
        mulxq	280(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 280(%rdi)
        adoxq	%r8, %r9
        # A[36] * B
        mulxq	288(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 288(%rdi)
        adoxq	%r8, %r10
        # A[37] * B
        mulxq	296(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 296(%rdi)
        adoxq	%r8, %r9
        # A[38] * B
        mulxq	304(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 304(%rdi)
        adoxq	%r8, %r10
        # A[39] * B
        mulxq	312(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 312(%rdi)
        adoxq	%r8, %r9
        # A[40] * B
        mulxq	320(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 320(%rdi)
        adoxq	%r8, %r10
        # A[41] * B
        mulxq	328(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 328(%rdi)
        adoxq	%r8, %r9
        # A[42] * B
        mulxq	336(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 336(%rdi)
        adoxq	%r8, %r10
        # A[43] * B
        mulxq	344(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 344(%rdi)
        adoxq	%r8, %r9
        # A[44] * B
        mulxq	352(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 352(%rdi)
        adoxq	%r8, %r10
        # A[45] * B
        mulxq	360(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 360(%rdi)
        adoxq	%r8, %r9
        # A[46] * B
        mulxq	368(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 368(%rdi)
        adoxq	%r8, %r10
        # A[47] * B
        mulxq	376(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        adoxq	%r8, %r9
        adcxq	%r11, %r9
        movq	%r10, 376(%rdi)
        movq	%r9, 384(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_3072_mul_d_avx2_48,.-sp_3072_mul_d_avx2_48
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
#ifndef __APPLE__
.globl	sp_3072_cmp_48
.type	sp_3072_cmp_48,@function
.align	16
sp_3072_cmp_48:
#else
.globl	_sp_3072_cmp_48
.p2align	4
_sp_3072_cmp_48:
#endif /* __APPLE__ */
        xorq	%rcx, %rcx
        movq	$-1, %rdx
        movq	$-1, %rax
        movq	$1, %r8
        movq	376(%rdi), %r9
        movq	376(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	368(%rdi), %r9
        movq	368(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	360(%rdi), %r9
        movq	360(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	352(%rdi), %r9
        movq	352(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	344(%rdi), %r9
        movq	344(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	336(%rdi), %r9
        movq	336(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	328(%rdi), %r9
        movq	328(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	320(%rdi), %r9
        movq	320(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	312(%rdi), %r9
        movq	312(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	304(%rdi), %r9
        movq	304(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	296(%rdi), %r9
        movq	296(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	288(%rdi), %r9
        movq	288(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	280(%rdi), %r9
        movq	280(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	272(%rdi), %r9
        movq	272(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	264(%rdi), %r9
        movq	264(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	256(%rdi), %r9
        movq	256(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	248(%rdi), %r9
        movq	248(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	240(%rdi), %r9
        movq	240(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	232(%rdi), %r9
        movq	232(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	224(%rdi), %r9
        movq	224(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	216(%rdi), %r9
        movq	216(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	208(%rdi), %r9
        movq	208(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	200(%rdi), %r9
        movq	200(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	192(%rdi), %r9
        movq	192(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	184(%rdi), %r9
        movq	184(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	176(%rdi), %r9
        movq	176(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	168(%rdi), %r9
        movq	168(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	160(%rdi), %r9
        movq	160(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	152(%rdi), %r9
        movq	152(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	144(%rdi), %r9
        movq	144(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	136(%rdi), %r9
        movq	136(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	128(%rdi), %r9
        movq	128(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	120(%rdi), %r9
        movq	120(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	112(%rdi), %r9
        movq	112(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	104(%rdi), %r9
        movq	104(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	96(%rdi), %r9
        movq	96(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	88(%rdi), %r9
        movq	88(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	80(%rdi), %r9
        movq	80(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	72(%rdi), %r9
        movq	72(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	64(%rdi), %r9
        movq	64(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	56(%rdi), %r9
        movq	56(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	48(%rdi), %r9
        movq	48(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	40(%rdi), %r9
        movq	40(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	32(%rdi), %r9
        movq	32(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	24(%rdi), %r9
        movq	24(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	16(%rdi), %r9
        movq	16(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	8(%rdi), %r9
        movq	8(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	(%rdi), %r9
        movq	(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        xorq	%rdx, %rax
        repz retq
#ifndef __APPLE__
.size	sp_3072_cmp_48,.-sp_3072_cmp_48
#endif /* __APPLE__ */
#ifdef HAVE_INTEL_AVX2
/* Reduce the number back to 3072 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
#ifndef __APPLE__
.globl	sp_3072_mont_reduce_avx2_48
.type	sp_3072_mont_reduce_avx2_48,@function
.align	16
sp_3072_mont_reduce_avx2_48:
#else
.globl	_sp_3072_mont_reduce_avx2_48
.p2align	4
_sp_3072_mont_reduce_avx2_48:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        movq	%rdx, %rax
        xorq	%r14, %r14
        # i = 0
        movq	$48, %r9
        movq	(%rdi), %r13
        xorq	%r12, %r12
L_mont_loop_avx2_48:
        # mu = a[i] * mp
        movq	%r13, %rdx
        mulxq	%rax, %rdx, %r8
        movq	%r13, %r10
        # a[i+0] += m[0] * mu
        mulxq	(%rsi), %rcx, %r8
        movq	8(%rdi), %r13
        adcxq	%rcx, %r10
        adoxq	%r8, %r13
        # a[i+1] += m[1] * mu
        mulxq	8(%rsi), %rcx, %r8
        movq	16(%rdi), %r10
        adcxq	%rcx, %r13
        adoxq	%r8, %r10
        # a[i+2] += m[2] * mu
        mulxq	16(%rsi), %rcx, %r8
        movq	24(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 16(%rdi)
        # a[i+3] += m[3] * mu
        mulxq	24(%rsi), %rcx, %r8
        movq	32(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 24(%rdi)
        # a[i+4] += m[4] * mu
        mulxq	32(%rsi), %rcx, %r8
        movq	40(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 32(%rdi)
        # a[i+5] += m[5] * mu
        mulxq	40(%rsi), %rcx, %r8
        movq	48(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 40(%rdi)
        # a[i+6] += m[6] * mu
        mulxq	48(%rsi), %rcx, %r8
        movq	56(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 48(%rdi)
        # a[i+7] += m[7] * mu
        mulxq	56(%rsi), %rcx, %r8
        movq	64(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 56(%rdi)
        # a[i+8] += m[8] * mu
        mulxq	64(%rsi), %rcx, %r8
        movq	72(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 64(%rdi)
        # a[i+9] += m[9] * mu
        mulxq	72(%rsi), %rcx, %r8
        movq	80(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 72(%rdi)
        # a[i+10] += m[10] * mu
        mulxq	80(%rsi), %rcx, %r8
        movq	88(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 80(%rdi)
        # a[i+11] += m[11] * mu
        mulxq	88(%rsi), %rcx, %r8
        movq	96(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 88(%rdi)
        # a[i+12] += m[12] * mu
        mulxq	96(%rsi), %rcx, %r8
        movq	104(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 96(%rdi)
        # a[i+13] += m[13] * mu
        mulxq	104(%rsi), %rcx, %r8
        movq	112(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 104(%rdi)
        # a[i+14] += m[14] * mu
        mulxq	112(%rsi), %rcx, %r8
        movq	120(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 112(%rdi)
        # a[i+15] += m[15] * mu
        mulxq	120(%rsi), %rcx, %r8
        movq	128(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 120(%rdi)
        # a[i+16] += m[16] * mu
        mulxq	128(%rsi), %rcx, %r8
        movq	136(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 128(%rdi)
        # a[i+17] += m[17] * mu
        mulxq	136(%rsi), %rcx, %r8
        movq	144(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 136(%rdi)
        # a[i+18] += m[18] * mu
        mulxq	144(%rsi), %rcx, %r8
        movq	152(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 144(%rdi)
        # a[i+19] += m[19] * mu
        mulxq	152(%rsi), %rcx, %r8
        movq	160(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 152(%rdi)
        # a[i+20] += m[20] * mu
        mulxq	160(%rsi), %rcx, %r8
        movq	168(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 160(%rdi)
        # a[i+21] += m[21] * mu
        mulxq	168(%rsi), %rcx, %r8
        movq	176(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 168(%rdi)
        # a[i+22] += m[22] * mu
        mulxq	176(%rsi), %rcx, %r8
        movq	184(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 176(%rdi)
        # a[i+23] += m[23] * mu
        mulxq	184(%rsi), %rcx, %r8
        movq	192(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 184(%rdi)
        # a[i+24] += m[24] * mu
        mulxq	192(%rsi), %rcx, %r8
        movq	200(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 192(%rdi)
        # a[i+25] += m[25] * mu
        mulxq	200(%rsi), %rcx, %r8
        movq	208(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 200(%rdi)
        # a[i+26] += m[26] * mu
        mulxq	208(%rsi), %rcx, %r8
        movq	216(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 208(%rdi)
        # a[i+27] += m[27] * mu
        mulxq	216(%rsi), %rcx, %r8
        movq	224(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 216(%rdi)
        # a[i+28] += m[28] * mu
        mulxq	224(%rsi), %rcx, %r8
        movq	232(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 224(%rdi)
        # a[i+29] += m[29] * mu
        mulxq	232(%rsi), %rcx, %r8
        movq	240(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 232(%rdi)
        # a[i+30] += m[30] * mu
        mulxq	240(%rsi), %rcx, %r8
        movq	248(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 240(%rdi)
        # a[i+31] += m[31] * mu
        mulxq	248(%rsi), %rcx, %r8
        movq	256(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 248(%rdi)
        # a[i+32] += m[32] * mu
        mulxq	256(%rsi), %rcx, %r8
        movq	264(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 256(%rdi)
        # a[i+33] += m[33] * mu
        mulxq	264(%rsi), %rcx, %r8
        movq	272(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 264(%rdi)
        # a[i+34] += m[34] * mu
        mulxq	272(%rsi), %rcx, %r8
        movq	280(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 272(%rdi)
        # a[i+35] += m[35] * mu
        mulxq	280(%rsi), %rcx, %r8
        movq	288(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 280(%rdi)
        # a[i+36] += m[36] * mu
        mulxq	288(%rsi), %rcx, %r8
        movq	296(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 288(%rdi)
        # a[i+37] += m[37] * mu
        mulxq	296(%rsi), %rcx, %r8
        movq	304(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 296(%rdi)
        # a[i+38] += m[38] * mu
        mulxq	304(%rsi), %rcx, %r8
        movq	312(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 304(%rdi)
        # a[i+39] += m[39] * mu
        mulxq	312(%rsi), %rcx, %r8
        movq	320(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 312(%rdi)
        # a[i+40] += m[40] * mu
        mulxq	320(%rsi), %rcx, %r8
        movq	328(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 320(%rdi)
        # a[i+41] += m[41] * mu
        mulxq	328(%rsi), %rcx, %r8
        movq	336(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 328(%rdi)
        # a[i+42] += m[42] * mu
        mulxq	336(%rsi), %rcx, %r8
        movq	344(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 336(%rdi)
        # a[i+43] += m[43] * mu
        mulxq	344(%rsi), %rcx, %r8
        movq	352(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 344(%rdi)
        # a[i+44] += m[44] * mu
        mulxq	352(%rsi), %rcx, %r8
        movq	360(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 352(%rdi)
        # a[i+45] += m[45] * mu
        mulxq	360(%rsi), %rcx, %r8
        movq	368(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 360(%rdi)
        # a[i+46] += m[46] * mu
        mulxq	368(%rsi), %rcx, %r8
        movq	376(%rdi), %r11
        adcxq	%rcx, %r10
        adoxq	%r8, %r11
        movq	%r10, 368(%rdi)
        # a[i+47] += m[47] * mu
        mulxq	376(%rsi), %rcx, %r8
        movq	384(%rdi), %r10
        adcxq	%rcx, %r11
        adoxq	%r8, %r10
        movq	%r11, 376(%rdi)
        adcxq	%r14, %r10
        movq	%r12, %r14
        adoxq	%r12, %r14
        adcxq	%r12, %r14
        movq	%r10, 384(%rdi)
        # i += 1
        addq	$8, %rdi
        decq	%r9
        jnz	L_mont_loop_avx2_48
        movq	%r13, (%rdi)
        negq	%r14
        movq	%r14, %rcx
        movq	%rsi, %rdx
        movq	%rdi, %rsi
        movq	%rdi, %rdi
        subq	$384, %rdi
#ifndef __APPLE__
        callq	sp_3072_cond_sub_48@plt
#else
        callq	_sp_3072_cond_sub_48
#endif /* __APPLE__ */
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_3072_mont_reduce_avx2_48,.-sp_3072_mont_reduce_avx2_48
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
/* Conditionally copy a into r using the mask m.
 * m is -1 to copy and 0 when not.
 *
 * r  A single precision number to copy over.
 * a  A single precision number to copy.
 * m  Mask value to apply.
 */
#ifndef __APPLE__
.globl	sp_256_cond_copy_4
.type	sp_256_cond_copy_4,@function
.align	16
sp_256_cond_copy_4:
#else
.globl	_sp_256_cond_copy_4
.p2align	4
_sp_256_cond_copy_4:
#endif /* __APPLE__ */
        movq	(%rdi), %rax
        movq	8(%rdi), %rcx
        movq	16(%rdi), %r8
        movq	24(%rdi), %r9
        xorq	(%rsi), %rax
        xorq	8(%rsi), %rcx
        xorq	16(%rsi), %r8
        xorq	24(%rsi), %r9
        andq	%rdx, %rax
        andq	%rdx, %rcx
        andq	%rdx, %r8
        andq	%rdx, %r9
        xorq	%rax, (%rdi)
        xorq	%rcx, 8(%rdi)
        xorq	%r8, 16(%rdi)
        xorq	%r9, 24(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_256_cond_copy_4,.-sp_256_cond_copy_4
#endif /* __APPLE__ */
/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
#ifndef __APPLE__
.globl	sp_256_cmp_4
.type	sp_256_cmp_4,@function
.align	16
sp_256_cmp_4:
#else
.globl	_sp_256_cmp_4
.p2align	4
_sp_256_cmp_4:
#endif /* __APPLE__ */
        xorq	%rcx, %rcx
        movq	$-1, %rdx
        movq	$-1, %rax
        movq	$1, %r8
        movq	24(%rdi), %r9
        movq	24(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	16(%rdi), %r9
        movq	16(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	8(%rdi), %r9
        movq	8(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        movq	(%rdi), %r9
        movq	(%rsi), %r10
        andq	%rdx, %r9
        andq	%rdx, %r10
        subq	%r10, %r9
        cmova	%r8, %rax
        cmovc	%rdx, %rax
        cmovnz	%rcx, %rdx
        xorq	%rdx, %rax
        repz retq
#ifndef __APPLE__
.size	sp_256_cmp_4,.-sp_256_cmp_4
#endif /* __APPLE__ */
/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not copying.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
#ifndef __APPLE__
.globl	sp_256_cond_sub_4
.type	sp_256_cond_sub_4,@function
.align	16
sp_256_cond_sub_4:
#else
.globl	_sp_256_cond_sub_4
.p2align	4
_sp_256_cond_sub_4:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        movq	$0, %rax
        movq	(%rdx), %r12
        movq	8(%rdx), %r13
        movq	16(%rdx), %r14
        movq	24(%rdx), %r15
        andq	%rcx, %r12
        andq	%rcx, %r13
        andq	%rcx, %r14
        andq	%rcx, %r15
        movq	(%rsi), %r8
        movq	8(%rsi), %r9
        movq	16(%rsi), %r10
        movq	24(%rsi), %r11
        subq	%r12, %r8
        sbbq	%r13, %r9
        sbbq	%r14, %r10
        sbbq	%r15, %r11
        movq	%r8, (%rdi)
        movq	%r9, 8(%rdi)
        movq	%r10, 16(%rdi)
        movq	%r11, 24(%rdi)
        sbbq	$0, %rax
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_256_cond_sub_4,.-sp_256_cond_sub_4
#endif /* __APPLE__ */
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_256_sub_4
.type	sp_256_sub_4,@function
.align	16
sp_256_sub_4:
#else
.globl	_sp_256_sub_4
.p2align	4
_sp_256_sub_4:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        xorq	%rax, %rax
        movq	(%rsi), %r8
        movq	8(%rsi), %r9
        movq	16(%rsi), %r10
        movq	24(%rsi), %r11
        movq	(%rdx), %r12
        movq	8(%rdx), %r13
        movq	16(%rdx), %r14
        movq	24(%rdx), %r15
        subq	%r12, %r8
        sbbq	%r13, %r9
        sbbq	%r14, %r10
        sbbq	%r15, %r11
        movq	%r8, (%rdi)
        movq	%r9, 8(%rdi)
        movq	%r10, 16(%rdi)
        movq	%r11, 24(%rdi)
        sbbq	$0, %rax
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_256_sub_4,.-sp_256_sub_4
#endif /* __APPLE__ */
/* Reduce the number back to 256 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
#ifndef __APPLE__
.globl	sp_256_mont_reduce_4
.type	sp_256_mont_reduce_4,@function
.align	16
sp_256_mont_reduce_4:
#else
.globl	_sp_256_mont_reduce_4
.p2align	4
_sp_256_mont_reduce_4:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        movq	%rdx, %rcx
        # i = 0
        xorq	%r14, %r14
        movq	$4, %r8
        movq	%rdi, %r13
L_mont_loop_4:
        # mu = a[i] * mp
        movq	(%r13), %r12
        imulq	%rcx, %r12
        # a[i+0] += m[0] * mu
        movq	(%rsi), %rax
        movq	8(%rsi), %r10
        mulq	%r12
        movq	(%r13), %r15
        addq	%rax, %r15
        movq	%rdx, %r9
        movq	%r15, (%r13)
        adcq	$0, %r9
        # a[i+1] += m[1] * mu
        movq	%r10, %rax
        mulq	%r12
        movq	16(%rsi), %r10
        movq	8(%r13), %r15
        addq	%r9, %rax
        movq	%rdx, %r11
        adcq	$0, %r11
        addq	%rax, %r15
        movq	%r15, 8(%r13)
        adcq	$0, %r11
        # a[i+2] += m[2] * mu
        movq	%r10, %rax
        mulq	%r12
        movq	24(%rsi), %r10
        movq	16(%r13), %r15
        addq	%r11, %rax
        movq	%rdx, %r9
        adcq	$0, %r9
        addq	%rax, %r15
        movq	%r15, 16(%r13)
        adcq	$0, %r9
        # a[i+3] += m[3] * mu
        movq	%r10, %rax
        mulq	%r12
        movq	24(%r13), %r15
        addq	%r9, %rax
        adcq	%r14, %rdx
        movq	$0, %r14
        adcq	$0, %r14
        addq	%rax, %r15
        movq	%r15, 24(%r13)
        adcq	%rdx, 32(%r13)
        adcq	$0, %r14
        # i += 1
        addq	$8, %r13
        decq	%r8
        jnz	L_mont_loop_4
        xorq	%rax, %rax
        movq	32(%rdi), %rdx
        movq	40(%rdi), %r8
        movq	48(%rdi), %r15
        movq	56(%rdi), %r9
        subq	%r14, %rax
        movq	(%rsi), %r10
        movq	8(%rsi), %r11
        movq	16(%rsi), %r12
        movq	24(%rsi), %r13
        andq	%rax, %r10
        andq	%rax, %r11
        andq	%rax, %r12
        andq	%rax, %r13
        subq	%r10, %rdx
        sbbq	%r11, %r8
        sbbq	%r12, %r15
        sbbq	%r13, %r9
        movq	%rdx, (%rdi)
        movq	%r8, 8(%rdi)
        movq	%r15, 16(%rdi)
        movq	%r9, 24(%rdi)
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_256_mont_reduce_4,.-sp_256_mont_reduce_4
#endif /* __APPLE__ */
/* Multiply two Montogmery form numbers mod the modulus (prime).
 * (r = a * b mod m)
 *
 * r   Result of multiplication.
 * a   First number to multiply in Montogmery form.
 * b   Second number to multiply in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
#ifndef __APPLE__
.globl	sp_256_mont_mul_4
.type	sp_256_mont_mul_4,@function
.align	16
sp_256_mont_mul_4:
#else
.globl	_sp_256_mont_mul_4
.p2align	4
_sp_256_mont_mul_4:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        push	%rbx
        movq	%rdx, %r8
        #  A[0] * B[0]
        movq	(%r8), %rax
        mulq	(%rsi)
        movq	%rax, %r9
        movq	%rdx, %r10
        #  A[0] * B[1]
        movq	8(%r8), %rax
        mulq	(%rsi)
        xorq	%r11, %r11
        addq	%rax, %r10
        adcq	%rdx, %r11
        #  A[1] * B[0]
        movq	(%r8), %rax
        mulq	8(%rsi)
        xorq	%r12, %r12
        addq	%rax, %r10
        adcq	%rdx, %r11
        adcq	$0, %r12
        #  A[0] * B[2]
        movq	16(%r8), %rax
        mulq	(%rsi)
        addq	%rax, %r11
        adcq	%rdx, %r12
        #  A[1] * B[1]
        movq	8(%r8), %rax
        mulq	8(%rsi)
        xorq	%r13, %r13
        addq	%rax, %r11
        adcq	%rdx, %r12
        adcq	$0, %r13
        #  A[2] * B[0]
        movq	(%r8), %rax
        mulq	16(%rsi)
        addq	%rax, %r11
        adcq	%rdx, %r12
        adcq	$0, %r13
        #  A[0] * B[3]
        movq	24(%r8), %rax
        mulq	(%rsi)
        xorq	%r14, %r14
        addq	%rax, %r12
        adcq	%rdx, %r13
        adcq	$0, %r14
        #  A[1] * B[2]
        movq	16(%r8), %rax
        mulq	8(%rsi)
        addq	%rax, %r12
        adcq	%rdx, %r13
        adcq	$0, %r14
        #  A[2] * B[1]
        movq	8(%r8), %rax
        mulq	16(%rsi)
        addq	%rax, %r12
        adcq	%rdx, %r13
        adcq	$0, %r14
        #  A[3] * B[0]
        movq	(%r8), %rax
        mulq	24(%rsi)
        addq	%rax, %r12
        adcq	%rdx, %r13
        adcq	$0, %r14
        #  A[1] * B[3]
        movq	24(%r8), %rax
        mulq	8(%rsi)
        xorq	%r15, %r15
        addq	%rax, %r13
        adcq	%rdx, %r14
        adcq	$0, %r15
        #  A[2] * B[2]
        movq	16(%r8), %rax
        mulq	16(%rsi)
        addq	%rax, %r13
        adcq	%rdx, %r14
        adcq	$0, %r15
        #  A[3] * B[1]
        movq	8(%r8), %rax
        mulq	24(%rsi)
        addq	%rax, %r13
        adcq	%rdx, %r14
        adcq	$0, %r15
        #  A[2] * B[3]
        movq	24(%r8), %rax
        mulq	16(%rsi)
        xorq	%rbx, %rbx
        addq	%rax, %r14
        adcq	%rdx, %r15
        adcq	$0, %rbx
        #  A[3] * B[2]
        movq	16(%r8), %rax
        mulq	24(%rsi)
        addq	%rax, %r14
        adcq	%rdx, %r15
        adcq	$0, %rbx
        #  A[3] * B[3]
        movq	24(%r8), %rax
        mulq	24(%rsi)
        addq	%rax, %r15
        adcq	%rdx, %rbx
        # Start Reduction
        # mu = a[0]-a[3] + a[0]-a[2] << 32 << 64 + (a[0] * 2) << 192
        #    - a[0] << 32 << 192
        #   + (a[0] * 2) << 192
        movq	%r9, %rax
        movq	%r12, %rdx
        addq	%r9, %rdx
        movq	%r10, %rsi
        addq	%r9, %rdx
        movq	%r11, %r8
        #   a[0]-a[2] << 32
        shlq	$32, %r9
        shldq	$32, %rsi, %r11
        shldq	$32, %rax, %r10
        #   - a[0] << 32 << 192
        subq	%r9, %rdx
        #   + a[0]-a[2] << 32 << 64
        addq	%r9, %rsi
        adcq	%r10, %r8
        adcq	%r11, %rdx
        # a += (mu << 256) - (mu << 224) + (mu << 192) + (mu << 96) - mu
        #   a += mu << 256
        xorq	%r9, %r9
        addq	%rax, %r13
        adcq	%rsi, %r14
        adcq	%r8, %r15
        adcq	%rdx, %rbx
        sbbq	$0, %r9
        #   a += mu << 192
        addq	%rax, %r12
        adcq	%rsi, %r13
        adcq	%r8, %r14
        adcq	%rdx, %r15
        adcq	$0, %rbx
        sbbq	$0, %r9
        # mu <<= 32
        movq	%rdx, %rcx
        shldq	$32, %r8, %rdx
        shldq	$32, %rsi, %r8
        shldq	$32, %rax, %rsi
        shrq	$32, %rcx
        shlq	$32, %rax
        #   a += (mu << 32) << 64
        addq	%r8, %r12
        adcq	%rdx, %r13
        adcq	%rcx, %r14
        adcq	$0, %r15
        adcq	$0, %rbx
        sbbq	$0, %r9
        #   a -= (mu << 32) << 192
        subq	%rax, %r12
        movq	$4294967295, %rax
        sbbq	%rsi, %r13
        movq	$18446744069414584321, %rsi
        sbbq	%r8, %r14
        sbbq	%rdx, %r15
        sbbq	%rcx, %rbx
        adcq	$0, %r9
        # mask m and sub from result if overflow
        #  m[0] = -1 & mask = mask
        andq	%r9, %rax
        #  m[2] =  0 & mask = 0
        andq	%r9, %rsi
        subq	%r9, %r13
        sbbq	%rax, %r14
        sbbq	$0, %r15
        sbbq	%rsi, %rbx
        movq	%r13, (%rdi)
        movq	%r14, 8(%rdi)
        movq	%r15, 16(%rdi)
        movq	%rbx, 24(%rdi)
        pop	%rbx
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_256_mont_mul_4,.-sp_256_mont_mul_4
#endif /* __APPLE__ */
/* Square the Montgomery form number mod the modulus (prime). (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
#ifndef __APPLE__
.globl	sp_256_mont_sqr_4
.type	sp_256_mont_sqr_4,@function
.align	16
sp_256_mont_sqr_4:
#else
.globl	_sp_256_mont_sqr_4
.p2align	4
_sp_256_mont_sqr_4:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        push	%rbx
        #  A[0] * A[1]
        movq	(%rsi), %rax
        mulq	8(%rsi)
        movq	%rax, %r10
        movq	%rdx, %r11
        #  A[0] * A[2]
        movq	(%rsi), %rax
        mulq	16(%rsi)
        xorq	%r12, %r12
        addq	%rax, %r11
        adcq	%rdx, %r12
        #  A[0] * A[3]
        movq	(%rsi), %rax
        mulq	24(%rsi)
        xorq	%r13, %r13
        addq	%rax, %r12
        adcq	%rdx, %r13
        #  A[1] * A[2]
        movq	8(%rsi), %rax
        mulq	16(%rsi)
        xorq	%r14, %r14
        addq	%rax, %r12
        adcq	%rdx, %r13
        adcq	$0, %r14
        #  A[1] * A[3]
        movq	8(%rsi), %rax
        mulq	24(%rsi)
        addq	%rax, %r13
        adcq	%rdx, %r14
        #  A[2] * A[3]
        movq	16(%rsi), %rax
        mulq	24(%rsi)
        xorq	%r15, %r15
        addq	%rax, %r14
        adcq	%rdx, %r15
        # Double
        xorq	%rbx, %rbx
        addq	%r10, %r10
        adcq	%r11, %r11
        adcq	%r12, %r12
        adcq	%r13, %r13
        adcq	%r14, %r14
        adcq	%r15, %r15
        adcq	$0, %rbx
        #  A[0] * A[0]
        movq	(%rsi), %rax
        mulq	%rax
        movq	%rax, %rax
        movq	%rdx, %rdx
        movq	%rax, %r9
        movq	%rdx, %r8
        #  A[1] * A[1]
        movq	8(%rsi), %rax
        mulq	%rax
        movq	%rax, %rax
        movq	%rdx, %rdx
        addq	%r8, %r10
        adcq	%rax, %r11
        adcq	$0, %rdx
        movq	%rdx, %r8
        #  A[2] * A[2]
        movq	16(%rsi), %rax
        mulq	%rax
        movq	%rax, %rax
        movq	%rdx, %rdx
        addq	%r8, %r12
        adcq	%rax, %r13
        adcq	$0, %rdx
        movq	%rdx, %r8
        #  A[3] * A[3]
        movq	24(%rsi), %rax
        mulq	%rax
        addq	%rax, %r15
        adcq	%rdx, %rbx
        addq	%r8, %r14
        adcq	$0, %r15
        adcq	$0, %rbx
        # Start Reduction
        # mu = a[0]-a[3] + a[0]-a[2] << 32 << 64 + (a[0] * 2) << 192
        #    - a[0] << 32 << 192
        #   + (a[0] * 2) << 192
        movq	%r9, %rax
        movq	%r12, %rdx
        addq	%r9, %rdx
        movq	%r10, %rsi
        addq	%r9, %rdx
        movq	%r11, %r8
        #   a[0]-a[2] << 32
        shlq	$32, %r9
        shldq	$32, %rsi, %r11
        shldq	$32, %rax, %r10
        #   - a[0] << 32 << 192
        subq	%r9, %rdx
        #   + a[0]-a[2] << 32 << 64
        addq	%r9, %rsi
        adcq	%r10, %r8
        adcq	%r11, %rdx
        # a += (mu << 256) - (mu << 224) + (mu << 192) + (mu << 96) - mu
        #   a += mu << 256
        xorq	%r9, %r9
        addq	%rax, %r13
        adcq	%rsi, %r14
        adcq	%r8, %r15
        adcq	%rdx, %rbx
        sbbq	$0, %r9
        #   a += mu << 192
        addq	%rax, %r12
        adcq	%rsi, %r13
        adcq	%r8, %r14
        adcq	%rdx, %r15
        adcq	$0, %rbx
        sbbq	$0, %r9
        # mu <<= 32
        movq	%rdx, %rcx
        shldq	$32, %r8, %rdx
        shldq	$32, %rsi, %r8
        shldq	$32, %rax, %rsi
        shrq	$32, %rcx
        shlq	$32, %rax
        #   a += (mu << 32) << 64
        addq	%r8, %r12
        adcq	%rdx, %r13
        adcq	%rcx, %r14
        adcq	$0, %r15
        adcq	$0, %rbx
        sbbq	$0, %r9
        #   a -= (mu << 32) << 192
        subq	%rax, %r12
        movq	$4294967295, %rax
        sbbq	%rsi, %r13
        movq	$18446744069414584321, %rsi
        sbbq	%r8, %r14
        sbbq	%rdx, %r15
        sbbq	%rcx, %rbx
        adcq	$0, %r9
        # mask m and sub from result if overflow
        #  m[0] = -1 & mask = mask
        andq	%r9, %rax
        #  m[2] =  0 & mask = 0
        andq	%r9, %rsi
        subq	%r9, %r13
        sbbq	%rax, %r14
        sbbq	$0, %r15
        sbbq	%rsi, %rbx
        movq	%r13, (%rdi)
        movq	%r14, 8(%rdi)
        movq	%r15, 16(%rdi)
        movq	%rbx, 24(%rdi)
        pop	%rbx
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_256_mont_sqr_4,.-sp_256_mont_sqr_4
#endif /* __APPLE__ */
/* Add two Montgomery form numbers (r = a + b % m).
 *
 * r   Result of addition.
 * a   First number to add in Montogmery form.
 * b   Second number to add in Montogmery form.
 * m   Modulus (prime).
 */
#ifndef __APPLE__
.globl	sp_256_mont_add_4
.type	sp_256_mont_add_4,@function
.align	16
sp_256_mont_add_4:
#else
.globl	_sp_256_mont_add_4
.p2align	4
_sp_256_mont_add_4:
#endif /* __APPLE__ */
        movq	(%rsi), %rax
        movq	8(%rsi), %rcx
        movq	16(%rsi), %r8
        movq	24(%rsi), %r9
        addq	(%rdx), %rax
        adcq	8(%rdx), %rcx
        movq	$4294967295, %r10
        adcq	16(%rdx), %r8
        movq	$18446744069414584321, %r11
        adcq	24(%rdx), %r9
        movq	$0, %rdx
        sbbq	$0, %rdx
        andq	%rdx, %r10
        andq	%rdx, %r11
        subq	%rdx, %rax
        sbbq	%r10, %rcx
        movq	%rax, (%rdi)
        sbbq	$0, %r8
        movq	%rcx, 8(%rdi)
        sbbq	%r11, %r9
        movq	%r8, 16(%rdi)
        movq	%r9, 24(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_256_mont_add_4,.-sp_256_mont_add_4
#endif /* __APPLE__ */
/* Double a Montgomery form number (r = a + a % m).
 *
 * r   Result of doubling.
 * a   Number to double in Montogmery form.
 * m   Modulus (prime).
 */
#ifndef __APPLE__
.globl	sp_256_mont_dbl_4
.type	sp_256_mont_dbl_4,@function
.align	16
sp_256_mont_dbl_4:
#else
.globl	_sp_256_mont_dbl_4
.p2align	4
_sp_256_mont_dbl_4:
#endif /* __APPLE__ */
        movq	(%rsi), %rdx
        movq	8(%rsi), %rax
        movq	16(%rsi), %rcx
        movq	24(%rsi), %r8
        xorq	%r11, %r11
        addq	%rdx, %rdx
        adcq	%rax, %rax
        movq	$4294967295, %r9
        adcq	%rcx, %rcx
        adcq	%r8, %r8
        movq	$18446744069414584321, %r10
        sbbq	$0, %r11
        andq	%r11, %r9
        andq	%r11, %r10
        subq	%r11, %rdx
        sbbq	%r9, %rax
        movq	%rdx, (%rdi)
        sbbq	$0, %rcx
        movq	%rax, 8(%rdi)
        sbbq	%r10, %r8
        movq	%rcx, 16(%rdi)
        movq	%r8, 24(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_256_mont_dbl_4,.-sp_256_mont_dbl_4
#endif /* __APPLE__ */
/* Triple a Montgomery form number (r = a + a + a % m).
 *
 * r   Result of Tripling.
 * a   Number to triple in Montogmery form.
 * m   Modulus (prime).
 */
#ifndef __APPLE__
.globl	sp_256_mont_tpl_4
.type	sp_256_mont_tpl_4,@function
.align	16
sp_256_mont_tpl_4:
#else
.globl	_sp_256_mont_tpl_4
.p2align	4
_sp_256_mont_tpl_4:
#endif /* __APPLE__ */
        movq	(%rsi), %rdx
        movq	8(%rsi), %rax
        movq	16(%rsi), %rcx
        movq	24(%rsi), %r8
        xorq	%r11, %r11
        addq	%rdx, %rdx
        adcq	%rax, %rax
        movq	$4294967295, %r9
        adcq	%rcx, %rcx
        adcq	%r8, %r8
        movq	$18446744069414584321, %r10
        sbbq	$0, %r11
        andq	%r11, %r9
        andq	%r11, %r10
        subq	%r11, %rdx
        sbbq	%r9, %rax
        sbbq	$0, %rcx
        sbbq	%r10, %r8
        xorq	%r11, %r11
        addq	(%rsi), %rdx
        adcq	8(%rsi), %rax
        movq	$4294967295, %r9
        adcq	16(%rsi), %rcx
        adcq	24(%rsi), %r8
        movq	$18446744069414584321, %r10
        sbbq	$0, %r11
        andq	%r11, %r9
        andq	%r11, %r10
        subq	%r11, %rdx
        sbbq	%r9, %rax
        movq	%rdx, (%rdi)
        sbbq	$0, %rcx
        movq	%rax, 8(%rdi)
        sbbq	%r10, %r8
        movq	%rcx, 16(%rdi)
        movq	%r8, 24(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_256_mont_tpl_4,.-sp_256_mont_tpl_4
#endif /* __APPLE__ */
/* Subtract two Montgomery form numbers (r = a - b % m).
 *
 * r   Result of subtration.
 * a   Number to subtract from in Montogmery form.
 * b   Number to subtract with in Montogmery form.
 * m   Modulus (prime).
 */
#ifndef __APPLE__
.globl	sp_256_mont_sub_4
.type	sp_256_mont_sub_4,@function
.align	16
sp_256_mont_sub_4:
#else
.globl	_sp_256_mont_sub_4
.p2align	4
_sp_256_mont_sub_4:
#endif /* __APPLE__ */
        movq	(%rsi), %rax
        movq	8(%rsi), %rcx
        movq	16(%rsi), %r8
        movq	24(%rsi), %r9
        subq	(%rdx), %rax
        sbbq	8(%rdx), %rcx
        movq	$4294967295, %r10
        sbbq	16(%rdx), %r8
        movq	$18446744069414584321, %r11
        sbbq	24(%rdx), %r9
        movq	$0, %rdx
        sbbq	$0, %rdx
        andq	%rdx, %r10
        andq	%rdx, %r11
        addq	%rdx, %rax
        adcq	%r10, %rcx
        movq	%rax, (%rdi)
        adcq	$0, %r8
        movq	%rcx, 8(%rdi)
        adcq	%r11, %r9
        movq	%r8, 16(%rdi)
        movq	%r9, 24(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_256_mont_sub_4,.-sp_256_mont_sub_4
#endif /* __APPLE__ */
/* Divide the number by 2 mod the modulus (prime). (r = a / 2 % m)
 *
 * r  Result of division by 2.
 * a  Number to divide.
 * m  Modulus (prime).
 */
#ifndef __APPLE__
.globl	sp_256_div2_4
.type	sp_256_div2_4,@function
.align	16
sp_256_div2_4:
#else
.globl	_sp_256_div2_4
.p2align	4
_sp_256_div2_4:
#endif /* __APPLE__ */
        movq	(%rsi), %rdx
        movq	8(%rsi), %rax
        movq	16(%rsi), %rcx
        movq	24(%rsi), %r8
        movq	$4294967295, %r9
        movq	$18446744069414584321, %r10
        movq	%rdx, %r11
        andq	$1, %r11
        negq	%r11
        andq	%r11, %r9
        andq	%r11, %r10
        addq	%r11, %rdx
        adcq	%r9, %rax
        adcq	$0, %rcx
        adcq	%r10, %r8
        movq	$0, %r11
        adcq	$0, %r11
        shrdq	$1, %rax, %rdx
        shrdq	$1, %rcx, %rax
        shrdq	$1, %r8, %rcx
        shrdq	$1, %r11, %r8
        movq	%rdx, (%rdi)
        movq	%rax, 8(%rdi)
        movq	%rcx, 16(%rdi)
        movq	%r8, 24(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_256_div2_4,.-sp_256_div2_4
#endif /* __APPLE__ */
/* Multiply two Montogmery form numbers mod the modulus (prime).
 * (r = a * b mod m)
 *
 * r   Result of multiplication.
 * a   First number to multiply in Montogmery form.
 * b   Second number to multiply in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
#ifndef __APPLE__
.globl	sp_256_mont_mul_avx2_4
.type	sp_256_mont_mul_avx2_4,@function
.align	16
sp_256_mont_mul_avx2_4:
#else
.globl	_sp_256_mont_mul_avx2_4
.p2align	4
_sp_256_mont_mul_avx2_4:
#endif /* __APPLE__ */
        push	%rbx
        push	%rbp
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        movq	%rdx, %rbp
        #  A[0] * B[0]
        movq	(%rbp), %rdx
        mulxq	(%rsi), %r8, %r9
        #  A[2] * B[0]
        mulxq	16(%rsi), %r10, %r11
        #  A[1] * B[0]
        mulxq	8(%rsi), %rax, %rcx
        xorq	%r15, %r15
        adcxq	%rax, %r9
        #  A[1] * B[3]
        movq	24(%rbp), %rdx
        mulxq	8(%rsi), %r12, %r13
        adcxq	%rcx, %r10
        #  A[0] * B[1]
        movq	8(%rbp), %rdx
        mulxq	(%rsi), %rax, %rcx
        adoxq	%rax, %r9
        #  A[2] * B[1]
        mulxq	16(%rsi), %rax, %r14
        adoxq	%rcx, %r10
        adcxq	%rax, %r11
        #  A[1] * B[2]
        movq	16(%rbp), %rdx
        mulxq	8(%rsi), %rax, %rcx
        adcxq	%r14, %r12
        adoxq	%rax, %r11
        adcxq	%r15, %r13
        adoxq	%rcx, %r12
        #  A[0] * B[2]
        mulxq	(%rsi), %rax, %rcx
        adoxq	%r15, %r13
        xorq	%r14, %r14
        adcxq	%rax, %r10
        #  A[1] * B[1]
        movq	8(%rbp), %rdx
        mulxq	8(%rsi), %rdx, %rax
        adcxq	%rcx, %r11
        adoxq	%rdx, %r10
        #  A[3] * B[1]
        movq	8(%rbp), %rdx
        adoxq	%rax, %r11
        mulxq	24(%rsi), %rax, %rcx
        adcxq	%rax, %r12
        #  A[2] * B[2]
        movq	16(%rbp), %rdx
        mulxq	16(%rsi), %rdx, %rax
        adcxq	%rcx, %r13
        adoxq	%rdx, %r12
        #  A[3] * B[3]
        movq	24(%rbp), %rdx
        adoxq	%rax, %r13
        mulxq	24(%rsi), %rax, %rcx
        adoxq	%r15, %r14
        adcxq	%rax, %r14
        #  A[0] * B[3]
        mulxq	(%rsi), %rdx, %rax
        adcxq	%rcx, %r15
        xorq	%rcx, %rcx
        adcxq	%rdx, %r11
        #  A[3] * B[0]
        movq	24(%rsi), %rdx
        adcxq	%rax, %r12
        mulxq	(%rbp), %rbx, %rax
        adoxq	%rbx, %r11
        adoxq	%rax, %r12
        #  A[3] * B[2]
        mulxq	16(%rbp), %rdx, %rax
        adcxq	%rdx, %r13
        #  A[2] * B[3]
        movq	24(%rbp), %rdx
        adcxq	%rax, %r14
        mulxq	16(%rsi), %rax, %rdx
        adcxq	%rcx, %r15
        adoxq	%rax, %r13
        adoxq	%rdx, %r14
        adoxq	%rcx, %r15
        # Start Reduction
        # mu = a[0]-a[3] + a[0]-a[2] << 32 << 64 + (a[0] * 2) << 192
        #    - a[0] << 32 << 192
        #   + (a[0] * 2) << 192
        movq	%r8, %rax
        movq	%r11, %rdx
        addq	%r8, %rdx
        movq	%r9, %rsi
        addq	%r8, %rdx
        movq	%r10, %rbp
        #   a[0]-a[2] << 32
        shlq	$32, %r8
        shldq	$32, %rsi, %r10
        shldq	$32, %rax, %r9
        #   - a[0] << 32 << 192
        subq	%r8, %rdx
        #   + a[0]-a[2] << 32 << 64
        addq	%r8, %rsi
        adcq	%r9, %rbp
        adcq	%r10, %rdx
        # a += (mu << 256) - (mu << 224) + (mu << 192) + (mu << 96) - mu
        #   a += mu << 256
        xorq	%r8, %r8
        addq	%rax, %r12
        adcq	%rsi, %r13
        adcq	%rbp, %r14
        adcq	%rdx, %r15
        sbbq	$0, %r8
        #   a += mu << 192
        addq	%rax, %r11
        adcq	%rsi, %r12
        adcq	%rbp, %r13
        adcq	%rdx, %r14
        adcq	$0, %r15
        sbbq	$0, %r8
        # mu <<= 32
        movq	%rdx, %rcx
        shldq	$32, %rbp, %rdx
        shldq	$32, %rsi, %rbp
        shldq	$32, %rax, %rsi
        shrq	$32, %rcx
        shlq	$32, %rax
        #   a += (mu << 32) << 64
        addq	%rbp, %r11
        adcq	%rdx, %r12
        adcq	%rcx, %r13
        adcq	$0, %r14
        adcq	$0, %r15
        sbbq	$0, %r8
        #   a -= (mu << 32) << 192
        subq	%rax, %r11
        movq	$4294967295, %rax
        sbbq	%rsi, %r12
        movq	$18446744069414584321, %rsi
        sbbq	%rbp, %r13
        sbbq	%rdx, %r14
        sbbq	%rcx, %r15
        adcq	$0, %r8
        # mask m and sub from result if overflow
        #  m[0] = -1 & mask = mask
        andq	%r8, %rax
        #  m[2] =  0 & mask = 0
        andq	%r8, %rsi
        subq	%r8, %r12
        sbbq	%rax, %r13
        sbbq	$0, %r14
        sbbq	%rsi, %r15
        movq	%r12, (%rdi)
        movq	%r13, 8(%rdi)
        movq	%r14, 16(%rdi)
        movq	%r15, 24(%rdi)
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        pop	%rbp
        pop	%rbx
        repz retq
#ifndef __APPLE__
.size	sp_256_mont_mul_avx2_4,.-sp_256_mont_mul_avx2_4
#endif /* __APPLE__ */
/* Square the Montgomery form number mod the modulus (prime). (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
#ifndef __APPLE__
.globl	sp_256_mont_sqr_avx2_4
.type	sp_256_mont_sqr_avx2_4,@function
.align	16
sp_256_mont_sqr_avx2_4:
#else
.globl	_sp_256_mont_sqr_avx2_4
.p2align	4
_sp_256_mont_sqr_avx2_4:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        push	%rbx
        # A[0] * A[1]
        movq	(%rsi), %rdx
        mulxq	8(%rsi), %r9, %r10
        # A[0] * A[3]
        mulxq	24(%rsi), %r11, %r12
        # A[2] * A[1]
        movq	16(%rsi), %rdx
        mulxq	8(%rsi), %rcx, %rbx
        xorq	%r15, %r15
        adoxq	%rcx, %r11
        # A[2] * A[3]
        mulxq	24(%rsi), %r13, %r14
        adoxq	%rbx, %r12
        # A[2] * A[0]
        mulxq	(%rsi), %rcx, %rbx
        adoxq	%r15, %r13
        adcxq	%rcx, %r10
        adoxq	%r15, %r14
        # A[1] * A[3]
        movq	8(%rsi), %rdx
        mulxq	24(%rsi), %rax, %r8
        adcxq	%rbx, %r11
        adcxq	%rax, %r12
        adcxq	%r8, %r13
        adcxq	%r15, %r14
        # Double with Carry Flag
        # A[0] * A[0]
        movq	(%rsi), %rdx
        mulxq	%rdx, %r8, %rax
        adcxq	%r9, %r9
        adcxq	%r10, %r10
        adoxq	%rax, %r9
        # A[1] * A[1]
        movq	8(%rsi), %rdx
        mulxq	%rdx, %rcx, %rbx
        adcxq	%r11, %r11
        adoxq	%rcx, %r10
        # A[2] * A[2]
        movq	16(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adcxq	%r12, %r12
        adoxq	%rbx, %r11
        adcxq	%r13, %r13
        adoxq	%rax, %r12
        adcxq	%r14, %r14
        # A[3] * A[3]
        movq	24(%rsi), %rdx
        mulxq	%rdx, %rax, %rbx
        adoxq	%rcx, %r13
        adcxq	%r15, %r15
        adoxq	%rax, %r14
        adoxq	%rbx, %r15
        # Start Reduction
        # mu = a[0]-a[3] + a[0]-a[2] << 32 << 64 + (a[0] * 2) << 192
        #    - a[0] << 32 << 192
        #   + (a[0] * 2) << 192
        movq	%r8, %rax
        movq	%r11, %rdx
        addq	%r8, %rdx
        movq	%r9, %rsi
        addq	%r8, %rdx
        movq	%r10, %rcx
        #   a[0]-a[2] << 32
        shlq	$32, %r8
        shldq	$32, %rsi, %r10
        shldq	$32, %rax, %r9
        #   - a[0] << 32 << 192
        subq	%r8, %rdx
        #   + a[0]-a[2] << 32 << 64
        addq	%r8, %rsi
        adcq	%r9, %rcx
        adcq	%r10, %rdx
        # a += (mu << 256) - (mu << 224) + (mu << 192) + (mu << 96) - mu
        #   a += mu << 256
        movq	$0, %r8
        addq	%rax, %r12
        adcq	%rsi, %r13
        adcq	%rcx, %r14
        adcq	%rdx, %r15
        sbbq	$0, %r8
        #   a += mu << 192
        addq	%rax, %r11
        adcq	%rsi, %r12
        adcq	%rcx, %r13
        adcq	%rdx, %r14
        adcq	$0, %r15
        sbbq	$0, %r8
        # mu <<= 32
        movq	%rdx, %rbx
        shldq	$32, %rcx, %rdx
        shldq	$32, %rsi, %rcx
        shldq	$32, %rax, %rsi
        shrq	$32, %rbx
        shlq	$32, %rax
        #   a += (mu << 32) << 64
        addq	%rcx, %r11
        adcq	%rdx, %r12
        adcq	%rbx, %r13
        adcq	$0, %r14
        adcq	$0, %r15
        sbbq	$0, %r8
        #   a -= (mu << 32) << 192
        subq	%rax, %r11
        movq	$4294967295, %rax
        sbbq	%rsi, %r12
        movq	$18446744069414584321, %rsi
        sbbq	%rcx, %r13
        sbbq	%rdx, %r14
        sbbq	%rbx, %r15
        adcq	$0, %r8
        # mask m and sub from result if overflow
        #  m[0] = -1 & mask = mask
        andq	%r8, %rax
        #  m[2] =  0 & mask = 0
        andq	%r8, %rsi
        subq	%r8, %r12
        sbbq	%rax, %r13
        sbbq	$0, %r14
        sbbq	%rsi, %r15
        movq	%r12, (%rdi)
        movq	%r13, 8(%rdi)
        movq	%r14, 16(%rdi)
        movq	%r15, 24(%rdi)
        pop	%rbx
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_256_mont_sqr_avx2_4,.-sp_256_mont_sqr_avx2_4
#endif /* __APPLE__ */
/* Add 1 to a. (a = a + 1)
 *
 * a  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_256_add_one_4
.type	sp_256_add_one_4,@function
.align	16
sp_256_add_one_4:
#else
.globl	_sp_256_add_one_4
.p2align	4
_sp_256_add_one_4:
#endif /* __APPLE__ */
        addq	$1, (%rdi)
        adcq	$0, 8(%rdi)
        adcq	$0, 16(%rdi)
        adcq	$0, 24(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_256_add_one_4,.-sp_256_add_one_4
#endif /* __APPLE__ */
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_256_add_4
.type	sp_256_add_4,@function
.align	16
sp_256_add_4:
#else
.globl	_sp_256_add_4
.p2align	4
_sp_256_add_4:
#endif /* __APPLE__ */
        xorq	%rax, %rax
        movq	(%rsi), %rcx
        addq	(%rdx), %rcx
        movq	%rcx, (%rdi)
        movq	8(%rsi), %rcx
        adcq	8(%rdx), %rcx
        movq	%rcx, 8(%rdi)
        movq	16(%rsi), %rcx
        adcq	16(%rdx), %rcx
        movq	%rcx, 16(%rdi)
        movq	24(%rsi), %rcx
        adcq	24(%rdx), %rcx
        movq	%rcx, 24(%rdi)
        adcq	$0, %rax
        repz retq
#ifndef __APPLE__
.size	sp_256_add_4,.-sp_256_add_4
#endif /* __APPLE__ */
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_256_mul_4
.type	sp_256_mul_4,@function
.align	16
sp_256_mul_4:
#else
.globl	_sp_256_mul_4
.p2align	4
_sp_256_mul_4:
#endif /* __APPLE__ */
        movq	%rdx, %rcx
        subq	$32, %rsp
        # A[0] * B[0]
        movq	(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        movq	%rax, (%rsp)
        movq	%rdx, %r9
        # A[0] * B[1]
        movq	8(%rcx), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[1] * B[0]
        movq	(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 8(%rsp)
        # A[0] * B[2]
        movq	16(%rcx), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * B[1]
        movq	8(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[2] * B[0]
        movq	(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 16(%rsp)
        # A[0] * B[3]
        movq	24(%rcx), %rax
        mulq	(%rsi)
        xorq	%r10, %r10
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[1] * B[2]
        movq	16(%rcx), %rax
        mulq	8(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[2] * B[1]
        movq	8(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        # A[3] * B[0]
        movq	(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %r10
        movq	%r8, 24(%rsp)
        # A[1] * B[3]
        movq	24(%rcx), %rax
        mulq	8(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B[2]
        movq	16(%rcx), %rax
        mulq	16(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[3] * B[1]
        movq	8(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r9
        adcq	%rdx, %r10
        adcq	$0, %r8
        movq	%r9, 32(%rdi)
        # A[2] * B[3]
        movq	24(%rcx), %rax
        mulq	16(%rsi)
        xorq	%r9, %r9
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[3] * B[2]
        movq	16(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r10
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%r10, 40(%rdi)
        # A[3] * B[3]
        movq	24(%rcx), %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        movq	%r8, 48(%rdi)
        movq	%r9, 56(%rdi)
        movq	(%rsp), %rax
        movq	8(%rsp), %rdx
        movq	16(%rsp), %r8
        movq	24(%rsp), %r9
        movq	%rax, (%rdi)
        movq	%rdx, 8(%rdi)
        movq	%r8, 16(%rdi)
        movq	%r9, 24(%rdi)
        addq	$32, %rsp
        repz retq
#ifndef __APPLE__
.size	sp_256_mul_4,.-sp_256_mul_4
#endif /* __APPLE__ */
/* Multiply a and b into r. (r = a * b)
 *
 * r   Result of multiplication.
 * a   First number to multiply.
 * b   Second number to multiply.
 */
#ifndef __APPLE__
.globl	sp_256_mul_avx2_4
.type	sp_256_mul_avx2_4,@function
.align	16
sp_256_mul_avx2_4:
#else
.globl	_sp_256_mul_avx2_4
.p2align	4
_sp_256_mul_avx2_4:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        push	%rbx
        movq	%rdx, %rax
        # A[0] * B[0]
        movq	(%rax), %rdx
        mulxq	(%rsi), %r9, %r10
        # A[2] * B[0]
        mulxq	16(%rsi), %r11, %r12
        # A[1] * B[0]
        mulxq	8(%rsi), %rcx, %r8
        xorq	%rbx, %rbx
        adcxq	%rcx, %r10
        # A[1] * B[3]
        movq	24(%rax), %rdx
        mulxq	8(%rsi), %r13, %r14
        adcxq	%r8, %r11
        # A[0] * B[1]
        movq	8(%rax), %rdx
        mulxq	(%rsi), %rcx, %r8
        adoxq	%rcx, %r10
        # A[2] * B[1]
        mulxq	16(%rsi), %rcx, %r15
        adoxq	%r8, %r11
        adcxq	%rcx, %r12
        # A[1] * B[2]
        movq	16(%rax), %rdx
        mulxq	8(%rsi), %rcx, %r8
        adcxq	%r15, %r13
        adoxq	%rcx, %r12
        adcxq	%rbx, %r14
        adoxq	%r8, %r13
        # A[0] * B[2]
        mulxq	(%rsi), %rcx, %r8
        adoxq	%rbx, %r14
        xorq	%r15, %r15
        adcxq	%rcx, %r11
        # A[1] * B[1]
        movq	8(%rax), %rdx
        mulxq	8(%rsi), %rdx, %rcx
        adcxq	%r8, %r12
        adoxq	%rdx, %r11
        # A[3] * B[1]
        movq	8(%rax), %rdx
        adoxq	%rcx, %r12
        mulxq	24(%rsi), %rcx, %r8
        adcxq	%rcx, %r13
        # A[2] * B[2]
        movq	16(%rax), %rdx
        mulxq	16(%rsi), %rdx, %rcx
        adcxq	%r8, %r14
        adoxq	%rdx, %r13
        # A[3] * B[3]
        movq	24(%rax), %rdx
        adoxq	%rcx, %r14
        mulxq	24(%rsi), %rcx, %r8
        adoxq	%rbx, %r15
        adcxq	%rcx, %r15
        # A[0] * B[3]
        mulxq	(%rsi), %rdx, %rcx
        adcxq	%r8, %rbx
        xorq	%r8, %r8
        adcxq	%rdx, %r12
        # A[3] * B[0]
        movq	(%rax), %rdx
        adcxq	%rcx, %r13
        mulxq	24(%rsi), %rdx, %rcx
        adoxq	%rdx, %r12
        adoxq	%rcx, %r13
        # A[2] * B[3]
        movq	24(%rax), %rdx
        mulxq	16(%rsi), %rdx, %rcx
        adcxq	%rdx, %r14
        # A[3] * B[2]
        movq	16(%rax), %rdx
        adcxq	%rcx, %r15
        mulxq	24(%rsi), %rcx, %rdx
        adcxq	%r8, %rbx
        adoxq	%rcx, %r14
        adoxq	%rdx, %r15
        adoxq	%r8, %rbx
        movq	%r9, (%rdi)
        movq	%r10, 8(%rdi)
        movq	%r11, 16(%rdi)
        movq	%r12, 24(%rdi)
        movq	%r13, 32(%rdi)
        movq	%r14, 40(%rdi)
        movq	%r15, 48(%rdi)
        movq	%rbx, 56(%rdi)
        pop	%rbx
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_256_mul_avx2_4,.-sp_256_mul_avx2_4
#endif /* __APPLE__ */
/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer and result.
 * b  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_256_sub_in_place_4
.type	sp_256_sub_in_place_4,@function
.align	16
sp_256_sub_in_place_4:
#else
.globl	_sp_256_sub_in_place_4
.p2align	4
_sp_256_sub_in_place_4:
#endif /* __APPLE__ */
        xorq	%rax, %rax
        movq	(%rsi), %rdx
        movq	8(%rsi), %rcx
        movq	16(%rsi), %r8
        movq	24(%rsi), %r9
        subq	%rdx, (%rdi)
        sbbq	%rcx, 8(%rdi)
        sbbq	%r8, 16(%rdi)
        sbbq	%r9, 24(%rdi)
        sbbq	$0, %rax
        repz retq
#ifndef __APPLE__
.size	sp_256_sub_in_place_4,.-sp_256_sub_in_place_4
#endif /* __APPLE__ */
/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
#ifndef __APPLE__
.globl	sp_256_mul_d_4
.type	sp_256_mul_d_4,@function
.align	16
sp_256_mul_d_4:
#else
.globl	_sp_256_mul_d_4
.p2align	4
_sp_256_mul_d_4:
#endif /* __APPLE__ */
        movq	%rdx, %rcx
        # A[0] * B
        movq	%rcx, %rax
        xorq	%r10, %r10
        mulq	(%rsi)
        movq	%rax, %r8
        movq	%rdx, %r9
        movq	%r8, (%rdi)
        # A[1] * B
        movq	%rcx, %rax
        xorq	%r8, %r8
        mulq	8(%rsi)
        addq	%rax, %r9
        movq	%r9, 8(%rdi)
        adcq	%rdx, %r10
        adcq	$0, %r8
        # A[2] * B
        movq	%rcx, %rax
        xorq	%r9, %r9
        mulq	16(%rsi)
        addq	%rax, %r10
        movq	%r10, 16(%rdi)
        adcq	%rdx, %r8
        adcq	$0, %r9
        # # A[3] * B
        movq	%rcx, %rax
        mulq	24(%rsi)
        addq	%rax, %r8
        adcq	%rdx, %r9
        movq	%r8, 24(%rdi)
        movq	%r9, 32(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_256_mul_d_4,.-sp_256_mul_d_4
#endif /* __APPLE__ */
#ifdef HAVE_INTEL_AVX2
/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
#ifndef __APPLE__
.globl	sp_256_mul_d_avx2_4
.type	sp_256_mul_d_avx2_4,@function
.align	16
sp_256_mul_d_avx2_4:
#else
.globl	_sp_256_mul_d_avx2_4
.p2align	4
_sp_256_mul_d_avx2_4:
#endif /* __APPLE__ */
        movq	%rdx, %rax
        # A[0] * B
        movq	%rax, %rdx
        xorq	%r11, %r11
        mulxq	(%rsi), %r9, %r10
        movq	%r9, (%rdi)
        # A[1] * B
        mulxq	8(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        movq	%r10, 8(%rdi)
        adoxq	%r8, %r9
        # A[2] * B
        mulxq	16(%rsi), %rcx, %r8
        movq	%r11, %r10
        adcxq	%rcx, %r9
        movq	%r9, 16(%rdi)
        adoxq	%r8, %r10
        # A[3] * B
        mulxq	24(%rsi), %rcx, %r8
        movq	%r11, %r9
        adcxq	%rcx, %r10
        adoxq	%r8, %r9
        adcxq	%r11, %r9
        movq	%r10, 24(%rdi)
        movq	%r9, 32(%rdi)
        repz retq
#ifndef __APPLE__
.size	sp_256_mul_d_avx2_4,.-sp_256_mul_d_avx2_4
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
#ifndef __APPLE__
.globl	sp_256_sqr_4
.type	sp_256_sqr_4,@function
.align	16
sp_256_sqr_4:
#else
.globl	_sp_256_sqr_4
.p2align	4
_sp_256_sqr_4:
#endif /* __APPLE__ */
        push	%r12
        subq	$32, %rsp
        # A[0] * A[0]
        movq	(%rsi), %rax
        mulq	%rax
        xorq	%r9, %r9
        movq	%rax, (%rsp)
        movq	%rdx, %r8
        # A[0] * A[1]
        movq	8(%rsi), %rax
        mulq	(%rsi)
        xorq	%rcx, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        movq	%r8, 8(%rsp)
        # A[0] * A[2]
        movq	16(%rsi), %rax
        mulq	(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        # A[1] * A[1]
        movq	8(%rsi), %rax
        mulq	%rax
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        movq	%r9, 16(%rsp)
        # A[0] * A[3]
        movq	24(%rsi), %rax
        mulq	(%rsi)
        xorq	%r9, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        # A[1] * A[2]
        movq	16(%rsi), %rax
        mulq	8(%rsi)
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        addq	%rax, %rcx
        adcq	%rdx, %r8
        adcq	$0, %r9
        movq	%rcx, 24(%rsp)
        # A[1] * A[3]
        movq	24(%rsi), %rax
        mulq	8(%rsi)
        xorq	%rcx, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        # A[2] * A[2]
        movq	16(%rsi), %rax
        mulq	%rax
        addq	%rax, %r8
        adcq	%rdx, %r9
        adcq	$0, %rcx
        movq	%r8, 32(%rdi)
        # A[2] * A[3]
        movq	24(%rsi), %rax
        mulq	16(%rsi)
        xorq	%r8, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        addq	%rax, %r9
        adcq	%rdx, %rcx
        adcq	$0, %r8
        movq	%r9, 40(%rdi)
        # A[3] * A[3]
        movq	24(%rsi), %rax
        mulq	%rax
        addq	%rax, %rcx
        adcq	%rdx, %r8
        movq	%rcx, 48(%rdi)
        movq	%r8, 56(%rdi)
        movq	(%rsp), %rax
        movq	8(%rsp), %rdx
        movq	16(%rsp), %r10
        movq	24(%rsp), %r11
        movq	%rax, (%rdi)
        movq	%rdx, 8(%rdi)
        movq	%r10, 16(%rdi)
        movq	%r11, 24(%rdi)
        addq	$32, %rsp
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_256_sqr_4,.-sp_256_sqr_4
#endif /* __APPLE__ */
/* Square a and put result in r. (r = a * a)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 */
#ifndef __APPLE__
.globl	sp_256_sqr_avx2_4
.type	sp_256_sqr_avx2_4,@function
.align	16
sp_256_sqr_avx2_4:
#else
.globl	_sp_256_sqr_avx2_4
.p2align	4
_sp_256_sqr_avx2_4:
#endif /* __APPLE__ */
        push	%rbx
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        # A[0] * A[1]
        movq	(%rsi), %rdx
        mulxq	8(%rsi), %r9, %r10
        # A[0] * A[3]
        mulxq	24(%rsi), %r11, %r12
        # A[2] * A[1]
        movq	16(%rsi), %rdx
        mulxq	8(%rsi), %rcx, %rbx
        xorq	%r15, %r15
        adoxq	%rcx, %r11
        # A[2] * A[3]
        mulxq	24(%rsi), %r13, %r14
        adoxq	%rbx, %r12
        # A[2] * A[0]
        mulxq	(%rsi), %rcx, %rbx
        adoxq	%r15, %r13
        adcxq	%rcx, %r10
        adoxq	%r15, %r14
        # A[1] * A[3]
        movq	8(%rsi), %rdx
        mulxq	24(%rsi), %rax, %r8
        adcxq	%rbx, %r11
        adcxq	%rax, %r12
        adcxq	%r8, %r13
        adcxq	%r15, %r14
        # Double with Carry Flag
        xorq	%r15, %r15
        # A[0] * A[0]
        movq	(%rsi), %rdx
        mulxq	%rdx, %r8, %rax
        adcxq	%r9, %r9
        # A[1] * A[1]
        movq	8(%rsi), %rdx
        mulxq	%rdx, %rcx, %rbx
        adcxq	%r10, %r10
        adoxq	%rax, %r9
        adcxq	%r11, %r11
        adoxq	%rcx, %r10
        # A[2] * A[2]
        movq	16(%rsi), %rdx
        mulxq	%rdx, %rax, %rcx
        adcxq	%r12, %r12
        adoxq	%rbx, %r11
        adcxq	%r13, %r13
        adoxq	%rax, %r12
        # A[3] * A[3]
        movq	24(%rsi), %rdx
        mulxq	%rdx, %rax, %rbx
        adcxq	%r14, %r14
        adoxq	%rcx, %r13
        adcxq	%r15, %r15
        adoxq	%rax, %r14
        adoxq	%rbx, %r15
        movq	%r8, (%rdi)
        movq	%r9, 8(%rdi)
        movq	%r10, 16(%rdi)
        movq	%r11, 24(%rdi)
        movq	%r12, 32(%rdi)
        movq	%r13, 40(%rdi)
        movq	%r14, 48(%rdi)
        movq	%r15, 56(%rdi)
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        pop	%rbx
        repz retq
#ifndef __APPLE__
.size	sp_256_sqr_avx2_4,.-sp_256_sqr_avx2_4
#endif /* __APPLE__ */
#ifdef HAVE_INTEL_AVX2
/* Reduce the number back to 256 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
#ifndef __APPLE__
.globl	sp_256_mont_reduce_avx2_4
.type	sp_256_mont_reduce_avx2_4,@function
.align	16
sp_256_mont_reduce_avx2_4:
#else
.globl	_sp_256_mont_reduce_avx2_4
.p2align	4
_sp_256_mont_reduce_avx2_4:
#endif /* __APPLE__ */
        push	%r12
        push	%r13
        push	%r14
        push	%r15
        push	%rbx
        movq	%rdx, %rax
        movq	(%rdi), %r12
        movq	8(%rdi), %r13
        movq	16(%rdi), %r14
        movq	24(%rdi), %r15
        xorq	%r11, %r11
        xorq	%r10, %r10
        # a[0-4] += m[0-3] * mu = m[0-3] * (a[0] * mp)
        movq	32(%rdi), %rbx
        #   mu = a[0] * mp
        movq	%r12, %rdx
        mulxq	%rax, %rdx, %rcx
        #   a[0] += m[0] * mu
        mulx	(%rsi), %r8, %r9
        adcxq	%r8, %r12
        #   a[1] += m[1] * mu
        mulx	8(%rsi), %r8, %rcx
        adoxq	%r9, %r13
        adcxq	%r8, %r13
        #   a[2] += m[2] * mu
        mulx	16(%rsi), %r8, %r9
        adoxq	%rcx, %r14
        adcxq	%r8, %r14
        #   a[3] += m[3] * mu
        mulx	24(%rsi), %r8, %rcx
        adoxq	%r9, %r15
        adcxq	%r8, %r15
        #   a[4] += carry
        adoxq	%rcx, %rbx
        adcxq	%r10, %rbx
        #   carry
        adoxq	%r10, %r11
        adcxq	%r10, %r11
        # a[1-5] += m[0-3] * mu = m[0-3] * (a[1] * mp)
        movq	40(%rdi), %r12
        #   mu = a[1] * mp
        movq	%r13, %rdx
        mulxq	%rax, %rdx, %rcx
        #   a[1] += m[0] * mu
        mulx	(%rsi), %r8, %r9
        adcxq	%r8, %r13
        #   a[2] += m[1] * mu
        mulx	8(%rsi), %r8, %rcx
        adoxq	%r9, %r14
        adcxq	%r8, %r14
        #   a[3] += m[2] * mu
        mulx	16(%rsi), %r8, %r9
        adoxq	%rcx, %r15
        adcxq	%r8, %r15
        #   a[4] += m[3] * mu
        mulx	24(%rsi), %r8, %rcx
        adoxq	%r9, %rbx
        adcxq	%r8, %rbx
        #   a[5] += carry
        adoxq	%rcx, %r12
        adcxq	%r11, %r12
        movq	%r10, %r11
        #   carry
        adoxq	%r10, %r11
        adcxq	%r10, %r11
        # a[2-6] += m[0-3] * mu = m[0-3] * (a[2] * mp)
        movq	48(%rdi), %r13
        #   mu = a[2] * mp
        movq	%r14, %rdx
        mulxq	%rax, %rdx, %rcx
        #   a[2] += m[0] * mu
        mulx	(%rsi), %r8, %r9
        adcxq	%r8, %r14
        #   a[3] += m[1] * mu
        mulx	8(%rsi), %r8, %rcx
        adoxq	%r9, %r15
        adcxq	%r8, %r15
        #   a[4] += m[2] * mu
        mulx	16(%rsi), %r8, %r9
        adoxq	%rcx, %rbx
        adcxq	%r8, %rbx
        #   a[5] += m[3] * mu
        mulx	24(%rsi), %r8, %rcx
        adoxq	%r9, %r12
        adcxq	%r8, %r12
        #   a[6] += carry
        adoxq	%rcx, %r13
        adcxq	%r11, %r13
        movq	%r10, %r11
        #   carry
        adoxq	%r10, %r11
        adcxq	%r10, %r11
        # a[3-7] += m[0-3] * mu = m[0-3] * (a[3] * mp)
        movq	56(%rdi), %r14
        #   mu = a[3] * mp
        movq	%r15, %rdx
        mulxq	%rax, %rdx, %rcx
        #   a[3] += m[0] * mu
        mulx	(%rsi), %r8, %r9
        adcxq	%r8, %r15
        #   a[4] += m[1] * mu
        mulx	8(%rsi), %r8, %rcx
        adoxq	%r9, %rbx
        adcxq	%r8, %rbx
        #   a[5] += m[2] * mu
        mulx	16(%rsi), %r8, %r9
        adoxq	%rcx, %r12
        adcxq	%r8, %r12
        #   a[6] += m[3] * mu
        mulx	24(%rsi), %r8, %rcx
        adoxq	%r9, %r13
        adcxq	%r8, %r13
        #   a[7] += carry
        adoxq	%rcx, %r14
        adcxq	%r11, %r14
        movq	%r10, %r11
        #   carry
        adoxq	%r10, %r11
        adcxq	%r10, %r11
        # Subtract mod if carry
        negq	%r11
        movq	$17562291160714782033, %r8
        movq	$13611842547513532036, %r9
        movq	$18446744069414584320, %rdx
        andq	%r11, %r8
        andq	%r11, %r9
        movq	%r11, %r11
        andq	%r11, %rdx
        subq	%r8, %rbx
        sbbq	%r9, %r12
        sbbq	%r11, %r13
        sbbq	%rdx, %r14
        movq	%rbx, (%rdi)
        movq	%r12, 8(%rdi)
        movq	%r13, 16(%rdi)
        movq	%r14, 24(%rdi)
        pop	%rbx
        pop	%r15
        pop	%r14
        pop	%r13
        pop	%r12
        repz retq
#ifndef __APPLE__
.size	sp_256_mont_reduce_avx2_4,.-sp_256_mont_reduce_avx2_4
#endif /* __APPLE__ */
#endif /* HAVE_INTEL_AVX2 */
