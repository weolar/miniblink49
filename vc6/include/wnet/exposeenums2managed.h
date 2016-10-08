//------------------------------------------------------------------------------
// File: exposeenums2managed.h
//
// Desc: macros to allow the same enum to be exposed to native and managed
//
// USAGE:
//
// in your whatever.h file that defines the enums use ENUM or FLAGS(for enums defining bitmasks/flags)
// at the top of the file include this .h
// at the bottom of the file include unexposeenums2managed.h(resets the macro state)
//
// in a native client .idl/.h/.cpp file as normal just
// #include <whatever.h>  
// this will include the file normally
//
// in a mgd cpp file 
// #include <whatever.h>
// once normally, this will make the enums available to native
//
// Copyright (c) 2003-2004, Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

// !!! do not pragma once or macro guard this file.
// it gets used multiple times by the same compilation units

#ifdef MANAGED_ENUMS

#ifndef _MANAGED
#error "you can only generate managed enums when compiling managed code"
#endif

#define ENUM typedef public __value enum
#define ENUM16 ENUM
#define FLAGS [System::Flags] ENUM
#define TAG(x) x

#else

#ifdef __midl
#define V1_ENUM [v1_enum]
#else
#define V1_ENUM
#endif
#define ENUM typedef V1_ENUM enum
#define ENUM16 typedef enum
#define FLAGS ENUM
#define TAG(x) tag##x

#endif

// end of file - exposeenums2managed.h

