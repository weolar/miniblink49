/***
*stdiostr.h - definitions/declarations for stdiobuf, stdiostream
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       This file defines the classes, values, macros, and functions
*       used by the stdiostream and stdiobuf classes.
*       [AT&T C++]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifdef  __cplusplus

#ifndef _INC_STDIOSTREAM
#define _INC_STDIOSTREAM

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#ifdef  _MSC_VER
// Currently, all MS C compilers for Win32 platforms default to 8 byte
// alignment.
#pragma pack(push,8)

#include <useoldio.h>

#endif  // _MSC_VER

/* Define _CRTIMP */

#ifndef _CRTIMP
/* current definition */
#ifdef  _DLL
#define _CRTIMP __declspec(dllimport)
#else   /* ndef _DLL */
#define _CRTIMP
#endif  /* _DLL */
#endif  /* _CRTIMP */


#include <iostream.h>
#include <stdio.h>

#ifdef  _MSC_VER
#pragma warning(disable:4514)           // disable unwanted /W4 warning
// #pragma warning(default:4514)        // use this to reenable, if necessary
#endif  // _MSC_VER

class _CRTIMP stdiobuf : public streambuf  {
public:
        stdiobuf(FILE* f);
FILE *  stdiofile() { return _str; }

virtual int pbackfail(int c);
virtual int overflow(int c = EOF);
virtual int underflow();
virtual streampos seekoff( streamoff, ios::seek_dir, int =ios::in|ios::out);
virtual int sync();
        ~stdiobuf();
        int setrwbuf(int _rsize, int _wsize);
// protected:
// virtual int doallocate();
private:
        FILE * _str;
};

// obsolescent
class _CRTIMP stdiostream : public iostream {  // note: spec.'d as : public IOS...
public:
        stdiostream(FILE *);
        ~stdiostream();
        stdiobuf* rdbuf() const { return (stdiobuf*) ostream::rdbuf(); }
        
private:
};

#ifdef  _MSC_VER
// Restore default packing
#pragma pack(pop)
#endif  // _MSC_VER

#endif  // _INC_STDIOSTREAM

#endif  /* __cplusplus */


