/***************************************************************\
*                                                               *
* SpecStrings.h - markers for documenting the semantics of APIs *
*                                                               *
* Version 1.0                                                   *
*                                                               *
* Copyright (c) Microsoft Corporation. All rights reserved.     *
*                                                               *
\***************************************************************/


#if _MSC_VER > 1000
#pragma once
#endif  // #if _MSC_VER > 1000

#define __specstrings

#ifdef  __cplusplus
#ifndef __nothrow
# define __nothrow __declspec(nothrow)
#endif
extern "C" {
#else
#ifndef __nothrow
# define __nothrow
#endif
#endif  // #ifdef __cplusplus


    #define __null
    #define __notnull
    #define __maybenull
    #define __readonly
    #define __notreadonly
    #define __maybereadonly
    #define __valid
    #define __notvalid
    #define __maybevalid
    #define __readableTo(extent)
    #define __elem_readableTo(size)
    #define __byte_readableTo(size)
    #define __writableTo(size)
    #define __elem_writableTo(size)
    #define __byte_writableTo(size)
    #define __deref
    #define __pre
    #define __post
    #define __precond(expr)
    #define __postcond(expr)
    #define __exceptthat
    #define __inner_success(expr)
    #define __inner_checkReturn
    #define __inner_typefix(ctype)
    #define __inner_override
    #define __inner_callback
    #define __inner_blocksOn(resource)
    #define __inner_fallthrough_dec
    #define __inner_fallthrough
    #define __refparam
    #define __inner_control_entrypoint(category)
    #define __inner_data_entrypoint(category)

#define __ecount(size)                                          __notnull __elem_writableTo(size)
#define __bcount(size)                                          __notnull __byte_writableTo(size)
#define __in                                                    __pre __valid __pre __deref __readonly
#define __in_ecount(size)                                       __in __pre __elem_readableTo(size)
#define __in_bcount(size)                                       __in __pre __byte_readableTo(size)
#define __out                                                   __ecount(1) __post __valid __refparam
#define __out_ecount(size)                                      __ecount(size) __post __valid __refparam
#define __out_bcount(size)                                      __bcount(size) __post __valid __refparam
#define __out_ecount_part(size,length)                          __out_ecount(size) __post __elem_readableTo(length)
#define __out_bcount_part(size,length)                          __out_bcount(size) __post __byte_readableTo(length)
#define __out_ecount_full(size)                                 __out_ecount_part(size,size)
#define __out_bcount_full(size)                                 __out_bcount_part(size,size)
#define __inout                                                 __pre __valid __post __valid __refparam
#define __inout_ecount(size)                                    __out_ecount(size) __pre __valid
#define __inout_bcount(size)                                    __out_bcount(size) __pre __valid
#define __inout_ecount_part(size,length)                        __out_ecount_part(size,length) __pre __valid __pre __elem_readableTo(length)
#define __inout_bcount_part(size,length)                        __out_bcount_part(size,length) __pre __valid __pre __byte_readableTo(length)
#define __inout_ecount_full(size)                               __inout_ecount_part(size,size)
#define __inout_bcount_full(size)                               __inout_bcount_part(size,size)

#define __ecount_opt(size)                                      __ecount(size)                              __exceptthat __maybenull
#define __bcount_opt(size)                                      __bcount(size)                              __exceptthat __maybenull
#define __in_opt                                                __in                                        __exceptthat __maybenull
#define __in_ecount_opt(size)                                   __in_ecount(size)                           __exceptthat __maybenull
#define __in_bcount_opt(size)                                   __in_bcount(size)                           __exceptthat __maybenull
#define __out_opt                                               __out                                       __exceptthat __maybenull
#define __out_ecount_opt(size)                                  __out_ecount(size)                          __exceptthat __maybenull
#define __out_bcount_opt(size)                                  __out_bcount(size)                          __exceptthat __maybenull
#define __out_ecount_part_opt(size,length)                      __out_ecount_part(size,length)              __exceptthat __maybenull
#define __out_bcount_part_opt(size,length)                      __out_bcount_part(size,length)              __exceptthat __maybenull
#define __out_ecount_full_opt(size)                             __out_ecount_full(size)                     __exceptthat __maybenull
#define __out_bcount_full_opt(size)                             __out_bcount_full(size)                     __exceptthat __maybenull
#define __inout_opt                                             __inout                                     __exceptthat __maybenull
#define __inout_ecount_opt(size)                                __inout_ecount(size)                        __exceptthat __maybenull
#define __inout_bcount_opt(size)                                __inout_bcount(size)                        __exceptthat __maybenull
#define __inout_ecount_part_opt(size,length)                    __inout_ecount_part(size,length)            __exceptthat __maybenull
#define __inout_bcount_part_opt(size,length)                    __inout_bcount_part(size,length)            __exceptthat __maybenull
#define __inout_ecount_full_opt(size)                           __inout_ecount_full(size)                   __exceptthat __maybenull
#define __inout_bcount_full_opt(size)                           __inout_bcount_full(size)                   __exceptthat __maybenull

#define __deref_ecount(size)                                    __ecount(1) __post __elem_readableTo(1) __post __deref __notnull __post __deref __elem_writableTo(size)
#define __deref_bcount(size)                                    __ecount(1) __post __elem_readableTo(1) __post __deref __notnull __post __deref __byte_writableTo(size)
#define __deref_in                                              __in __pre __deref __deref __readonly
#define __deref_in_ecount(size)                                 __deref_in __pre __deref __elem_readableTo(size)
#define __deref_in_bcount(size)                                 __deref_in __pre __deref __byte_readableTo(size)
#define __deref_out                                             __deref_ecount(1) __post __deref __valid __refparam
#define __deref_out_ecount(size)                                __deref_ecount(size) __post __deref __valid __refparam
#define __deref_out_bcount(size)                                __deref_bcount(size) __post __deref __valid __refparam
#define __deref_out_ecount_part(size,length)                    __deref_out_ecount(size) __post __deref __elem_readableTo(length)
#define __deref_out_bcount_part(size,length)                    __deref_out_bcount(size) __post __deref __byte_readableTo(length)
#define __deref_out_ecount_full(size)                           __deref_out_ecount_part(size,size)
#define __deref_out_bcount_full(size)                           __deref_out_bcount_part(size,size)
#define __deref_inout                                           __notnull __elem_readableTo(1) __pre __deref __valid __post __deref __valid __refparam
#define __deref_inout_ecount(size)                              __deref_inout __pre __deref __elem_writableTo(size) __post __deref __elem_writableTo(size)
#define __deref_inout_bcount(size)                              __deref_inout __pre __deref __byte_writableTo(size) __post __deref __byte_writableTo(size)
#define __deref_inout_ecount_part(size,length)                  __deref_inout_ecount(size) __pre __deref __elem_readableTo(length) __post __deref __elem_readableTo(length)
#define __deref_inout_bcount_part(size,length)                  __deref_inout_bcount(size) __pre __deref __byte_readableTo(length) __post __deref __byte_readableTo(length)
#define __deref_inout_ecount_full(size)                         __deref_inout_ecount_part(size,size)
#define __deref_inout_bcount_full(size)                         __deref_inout_bcount_part(size,size)

#define __deref_ecount_opt(size)                                __deref_ecount(size)                        __post __deref __exceptthat __maybenull
#define __deref_bcount_opt(size)                                __deref_bcount(size)                        __post __deref __exceptthat __maybenull
#define __deref_in_opt                                          __deref_in                                  __pre __deref __exceptthat __maybenull
#define __deref_in_ecount_opt(size)                             __deref_in_ecount(size)                     __pre __deref __exceptthat __maybenull
#define __deref_in_bcount_opt(size)                             __deref_in_bcount(size)                     __pre __deref __exceptthat __maybenull
#define __deref_out_opt                                         __deref_out                                 __post __deref __exceptthat __maybenull
#define __deref_out_ecount_opt(size)                            __deref_out_ecount(size)                    __post __deref __exceptthat __maybenull
#define __deref_out_bcount_opt(size)                            __deref_out_bcount(size)                    __post __deref __exceptthat __maybenull
#define __deref_out_ecount_part_opt(size,length)                __deref_out_ecount_part(size,length)        __post __deref __exceptthat __maybenull
#define __deref_out_bcount_part_opt(size,length)                __deref_out_bcount_part(size,length)        __post __deref __exceptthat __maybenull
#define __deref_out_ecount_full_opt(size)                       __deref_out_ecount_full(size)               __post __deref __exceptthat __maybenull
#define __deref_out_bcount_full_opt(size)                       __deref_out_bcount_full(size)               __post __deref __exceptthat __maybenull
#define __deref_inout_opt                                       __deref_inout                               __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_ecount_opt(size)                          __deref_inout_ecount(size)                  __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_bcount_opt(size)                          __deref_inout_bcount(size)                  __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_ecount_part_opt(size,length)              __deref_inout_ecount_part(size,length)      __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_bcount_part_opt(size,length)              __deref_inout_bcount_part(size,length)      __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_ecount_full_opt(size)                     __deref_inout_ecount_full(size)             __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_bcount_full_opt(size)                     __deref_inout_bcount_full(size)             __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull

#define __deref_opt_ecount(size)                                __deref_ecount(size)                        __exceptthat __maybenull
#define __deref_opt_bcount(size)                                __deref_bcount(size)                        __exceptthat __maybenull
#define __deref_opt_in                                          __deref_in                                  __exceptthat __maybenull
#define __deref_opt_in_ecount(size)                             __deref_in_ecount(size)                     __exceptthat __maybenull
#define __deref_opt_in_bcount(size)                             __deref_in_bcount(size)                     __exceptthat __maybenull
#define __deref_opt_out                                         __deref_out                                 __exceptthat __maybenull
#define __deref_opt_out_ecount(size)                            __deref_out_ecount(size)                    __exceptthat __maybenull
#define __deref_opt_out_bcount(size)                            __deref_out_bcount(size)                    __exceptthat __maybenull
#define __deref_opt_out_ecount_part(size,length)                __deref_out_ecount_part(size,length)        __exceptthat __maybenull
#define __deref_opt_out_bcount_part(size,length)                __deref_out_bcount_part(size,length)        __exceptthat __maybenull
#define __deref_opt_out_ecount_full(size)                       __deref_out_ecount_full(size)               __exceptthat __maybenull
#define __deref_opt_out_bcount_full(size)                       __deref_out_bcount_full(size)               __exceptthat __maybenull
#define __deref_opt_inout                                       __deref_inout                               __exceptthat __maybenull
#define __deref_opt_inout_ecount(size)                          __deref_inout_ecount(size)                  __exceptthat __maybenull
#define __deref_opt_inout_bcount(size)                          __deref_inout_bcount(size)                  __exceptthat __maybenull
#define __deref_opt_inout_ecount_part(size,length)              __deref_inout_ecount_part(size,length)      __exceptthat __maybenull
#define __deref_opt_inout_bcount_part(size,length)              __deref_inout_bcount_part(size,length)      __exceptthat __maybenull
#define __deref_opt_inout_ecount_full(size)                     __deref_inout_ecount_full(size)             __exceptthat __maybenull
#define __deref_opt_inout_bcount_full(size)                     __deref_inout_bcount_full(size)             __exceptthat __maybenull

#define __deref_opt_ecount_opt(size)                            __deref_ecount_opt(size)                    __exceptthat __maybenull
#define __deref_opt_bcount_opt(size)                            __deref_bcount_opt(size)                    __exceptthat __maybenull
#define __deref_opt_in_opt                                      __deref_in_opt                              __exceptthat __maybenull
#define __deref_opt_in_ecount_opt(size)                         __deref_in_ecount_opt(size)                 __exceptthat __maybenull
#define __deref_opt_in_bcount_opt(size)                         __deref_in_bcount_opt(size)                 __exceptthat __maybenull
#define __deref_opt_out_opt                                     __deref_out_opt                             __exceptthat __maybenull
#define __deref_opt_out_ecount_opt(size)                        __deref_out_ecount_opt(size)                __exceptthat __maybenull
#define __deref_opt_out_bcount_opt(size)                        __deref_out_bcount_opt(size)                __exceptthat __maybenull
#define __deref_opt_out_ecount_part_opt(size,length)            __deref_out_ecount_part_opt(size,length)    __exceptthat __maybenull
#define __deref_opt_out_bcount_part_opt(size,length)            __deref_out_bcount_part_opt(size,length)    __exceptthat __maybenull
#define __deref_opt_out_ecount_full_opt(size)                   __deref_out_ecount_full_opt(size)           __exceptthat __maybenull
#define __deref_opt_out_bcount_full_opt(size)                   __deref_out_bcount_full_opt(size)           __exceptthat __maybenull
#define __deref_opt_inout_opt                                   __deref_inout_opt                           __exceptthat __maybenull
#define __deref_opt_inout_ecount_opt(size)                      __deref_inout_ecount_opt(size)              __exceptthat __maybenull
#define __deref_opt_inout_bcount_opt(size)                      __deref_inout_bcount_opt(size)              __exceptthat __maybenull
#define __deref_opt_inout_ecount_part_opt(size,length)          __deref_inout_ecount_part_opt(size,length)  __exceptthat __maybenull
#define __deref_opt_inout_bcount_part_opt(size,length)          __deref_inout_bcount_part_opt(size,length)  __exceptthat __maybenull
#define __deref_opt_inout_ecount_full_opt(size)                 __deref_inout_ecount_full_opt(size)         __exceptthat __maybenull
#define __deref_opt_inout_bcount_full_opt(size)                 __deref_inout_bcount_full_opt(size)         __exceptthat __maybenull


#define __out_awcount(expr,size)            __pre __notnull \
                                            __precond(expr) __byte_writableTo(size) \
                                            __precond(!(expr)) __byte_writableTo((size)*2) \
                                            __post __valid __refparam
#define __in_awcount(expr,size)             __pre __valid \
                                            __pre __deref __readonly \
                                            __precond(expr) __byte_readableTo(size) \
                                            __precond(!(expr)) __elem_readableTo(size)
#define __success(expr)                     __inner_success(expr)
#define __nullterminated                    __readableTo(sentinel(0))
#define __nullnullterminated
#define __reserved                          __pre __null
#define __checkReturn                       __inner_checkReturn
#define __typefix(ctype)                    __inner_typefix(ctype)
#define __override                          __inner_override
#define __callback                          __inner_callback
#define __format_string
#define __blocksOn(resource)                __inner_blocksOn(resource)
#define __control_entrypoint(category)      __inner_control_entrypoint(category)
#define __data_entrypoint(category)         __inner_data_entrypoint(category)

#ifndef __fallthrough
    __inner_fallthrough_dec
    #define __fallthrough __inner_fallthrough
#endif

// -------------------------------------------------------------------------------
// Deprecated Annotation Definitions
//
// These should be removed from existing code.
// -------------------------------------------------------------------------------

// #define __opt                               __exceptthat __maybenull

#ifdef  __cplusplus
}
#endif


