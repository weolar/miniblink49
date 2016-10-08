/**
***  Copyright  (C) 1996-99 Intel Corporation. All rights reserved.
***
*** The information and source code contained herein is the exclusive
*** property of Intel Corporation and may not be disclosed, examined
*** or reproduced in whole or in part without explicit written authorization
*** from the company.
**/

#if defined(__assembler)

//
// Define standard integer registers.
//

        zero        = r0                        // always 0
        gp          = r1                        // global pointer
        v0          = r8                        // return value
        sp          = r12                       // stack pointer
        s0          = r4                        // saved (preserved) integer registers
        s1          = r5
        s2          = r6
        s3          = r7

//
// temporary (volatile) integer registers
//

        t0          = r2
        t1          = r3
        t2          = r9
        t3          = r10
        t4          = r11
        t5          = r14
        t6          = r15
        t7          = r16
        t8          = r17
        t9          = r18
        t10         = r19
        t11         = r20
        t12         = r21
        t13         = r22
        t14         = r23
        t15         = r24
        t16         = r25
        t17         = r26
        t18         = r27
        t19         = r28
        t20         = r29
        t21         = r30
        t22         = r31

//
// Floating point saved (preserved) registers
//

        fs0         = f2
        fs1         = f3
        fs2         = f4
        fs3         = f5
        fs4         = f16
        fs5         = f17
        fs6         = f18
        fs7         = f19
        fs8         = f20
        fs9         = f21
        fs10        = f22
        fs11        = f23
        fs12        = f24
        fs13        = f25
        fs14        = f26
        fs15        = f27
        fs16        = f28
        fs17        = f29
        fs18        = f30
        fs19        = f31

//
// Low floating point temporary (volatile) registers
//

        ft0         = f6
        ft1         = f7
        ft2         = f8
        ft3         = f9
        ft4         = f10
        ft5         = f11
        ft6         = f12
        ft7         = f13
        ft8         = f14
        ft9         = f15

//
// input arguments
// Should be:
// a0  = in0
// a1  = in1
// a2  = in2
// a3  = in3
// a4  = in4
// a5  = in5
// a6  = in6
// a7  = in7
//

        a0          = r32
        a1          = r33
        a2          = r34
        a3          = r35
        a4          = r36
        a5          = r37
        a6          = r38
        a7          = r39

//
// branch return pointer (b0)
//

        brp         = rp

//
// branch saved (preserved)
//

        bs0         = b1
        bs1         = b2
        bs2         = b3
        bs3         = b4
        bs4         = b5

//
// branch temporary (volatile) registers
//

        bt0         = b6
        bt1         = b7

//
// predicate registers
//
// p0 predicate register always 1
//

        ps0         = p1                        // saved (preserved) predicate registers
        ps1         = p2
        ps2         = p3
        ps3         = p4
        ps4         = p5
        ps5         = p16                       // Predicates p16-p63 are also preserved
        ps6         = p17
        ps7         = p18
        ps8         = p19
        ps9         = p20

        pt0         = p6                        // temporary (volatile) predicate registers
        pt1         = p7
        pt2         = p8
        pt3         = p9
        pt4         = p10
        pt5         = p11
        pt6         = p12
        pt7         = p13
        pt8         = p14
        pt9         = p15

//
// Kernel registers
//

        k0          = ar.k0
        k1          = ar.k1
        k2          = ar.k2
        k3          = ar.k3
        k4          = ar.k4
        k5          = ar.k5
        k6          = ar.k6
        k7          = ar.k7

        ia32dr67    = ar.k5                    // dr6/dr7 for iA32

        ia32fcr     = ar21                     // FCR for iA32
        ia32eflag   = ar24                     // EFLAG for iA32
        ia32csd     = ar25                     // CSD for iA32
        ia32ssd     = ar26                     // SSD for iA32
        ia32cflag   = ar27                     // CFLG (cr0/cr4) for iA32
        ia32fsr     = ar28                     // FSR for iA32
        ia32fir     = ar29                     // FIR for iA32
        ia32fdr     = ar30                     // FDR for iA32

//
// Define iA32 constants, to be used by ISA transition code
//
        _DataSelector       ==  0x23
        _CodeSelector       ==  0x1b
        _FsSelector         ==  0x3b
        _LdtSelector        ==  0x4b

//
// Define the IA-32 registers
//
        rEax        =   r8             // v0
        rEcx        =   r9             // t2
        rEdx        =   r10            // t3
        rEbx        =   r11            // t4
        rEsp        =   r12            // sp
        rEbp        =   r13            // teb
        rEsi        =   r14            // t5
        rEdi        =   r15            // t6


//
//  Define iA-32 Segment Registers mapping
//
        rDSESFSGS     =   r16     // ES selector register (t7)
        rCSSSLDTTSS     =   r17     // CS selector register (t8)
        rEFLAG      =   ar24        // Eflag register
        rESD        =   r24     // ES Descriptor register (t15)
        rCSD        =   ar25        // CS Descriptor register
        rSSD        =   ar26        // SS Descriptor register
        rDSD        =   r27     // DS Descriptor register (t18)
        rFSD        =   r28     // FS Descriptor register (t19)
        rGSD        =   r29     // GS Descriptor register (t20)
        rLDTD       =   r30     // LDT Descriptor register (t21)
        rGDTD       =   r31     // GDT Descriptor register (t22)

//
// pointer to thread environment block
//

        teb         = r13        // per s/w convention
        kteb        = ar.k3      // known "true" value (changed only by kernel)

//
// kernel bank shadow (hidden) registers
//

        h16         = r16
        h17         = r17
        h18         = r18
        h19         = r19
        h20         = r20
        h21         = r21
        h22         = r22
        h23         = r23
        h24         = r24
        h25         = r25
        h26         = r26
        h27         = r27
        h28         = r28
        h29         = r29
        h30         = r30
        h31         = r31

// Standard register aliases for procedure entry/exit

// Should be:
// savedpfs = loc0
// savedbrp = loc1

#define savedpfs    loc0
#define savedbrp    loc1

#endif

